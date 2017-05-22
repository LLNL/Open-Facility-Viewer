//----------------------------------*-C++-*----------------------------------//
// Open Facility Viewer, an application for 3D walk-throughs of facilities
//
// Copyright (C) 2016, Lawrence Livermore National Security, LLC.
//
// Produced at the Lawrence Livermore National Laboratory.
//
// All Rights Reserved.
//
// Our Preamble Notice
//
// A. This notice is required to be provided under our contract with the
// U.S. Department of Energy (DOE). This work was produced at the
// Lawrence Livermore National Laboratory under Contract
// No. DE-AC52-07NA27344 with the DOE.
//
// B. Neither the United States Government nor Lawrence Livermore
// National Security, LLC nor any of their employees, makes any warranty,
// express or implied, or assumes any liability or responsibility for the
// accuracy, completeness, or usefulness of any information, apparatus,
// product, or process disclosed, or represents that its use would not
// infringe privately-owned rights.
//
// C. Also, reference herein to any specific commercial products,
// process, or services by trade name, trademark, manufacturer or
// otherwise does not necessarily constitute or imply its endorsement,
// recommendation, or favoring by the United States Government or
// Lawrence Livermore National Security, LLC. The views and opinions of
// authors expressed herein do not necessarily state or reflect those of
// the United States Government or Lawrence Livermore National Security,
// LLC, and shall not be used for advertising or product endorsement
// purposes.
//
// The precise terms and conditions for copying, distribution and
// modification follows.
//
// This file is part of Open Facility Viewer (OFV).
// Open Facility Viewer (OFV) is free software: you can redistribute it 
// and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, version 2 of the License.
// Open Facility Viewer (OFV) is distributed in the hope that it will be 
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License 
// along with Open Facility Viewer (OFV). If not, see <http://www.gnu.org/licenses/>
//
//---------------------------------------------------------------------------//


#include "config.h"

#include <iostream>
#include <iomanip>

#include <OFV/GMApp/OFVInputComponent.h>
#include <OFV/GMApp/FlyLimitMotionModel.h>

#include <dtABC/application.h>
#include <dtCore/deltawin.h>
#include <dtCore/actorproxy.h>
#include <dtCore/actorproperty.h>
#include <dtCore/transformableactorproxy.h>
#include <dtCore/map.h>
#include <dtCore/object.h>
#include <dtCore/project.h>
#include <dtCore/camera.h>
#include <dtCore/deltawin.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtCore/mouse.h>
#include <dtCore/transform.h>
#include <dtCore/flymotionmodel.h>
#include <dtCore/light.h>
#include <dtCore/collisionmotionmodel.h>

#include <dtActors/engineactorregistry.h>
#include <dtActors/gamemeshactor.h>
#include <dtGame/message.h>
#include <dtGame/messagetype.h>
#include <dtGame/gameactor.h>
#include <dtGame/gameapplication.h>
#include <dtGame/basemessages.h>
#include <dtActors/skydomeenvironmentactor.h>
#include <dtGame/environmentactor.h>
#include <osgViewer/Renderer>
#include <osgViewer/View>

#include <osg/ComputeBoundsVisitor>
#include <osg/Texture2D>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osg/Geometry>
#include <osg/PositionAttitudeTransform>
#include <osg/PolygonMode>
#include <osg/Material>

#include <osgDB/ReadFile>

#include <dtUtil/fileutils.h>
#include <QtGui/QMessageBox>
#include <QtGui/QProgressDialog>


using dtCore::RefPtr;

OFVInputComponent::OFVInputComponent()
	: dtGame::BaseInputComponent("Input Component"),
	mInitalizedPlayer(false),
	mMapLoaded(false),
	mTicks(0),
	mMapWindowSize(osg::Vec2(200.0, 200.0))
{
	// Create a progress dialog.
	mProgressDialog = new QProgressDialog();
	mProgressDialog->setRange(0, 20);
	mProgressDialog->setLabelText(QString("Loading..."));
	mProgressDialog->setWindowTitle(QString("Please Wait"));
	mProgressDialog->setModal(true);
	mProgressDialog->show();

	mCorrectionX = 0.465;
	mCorrectionY = 0.03;
	mCorrectionSizeY = 5;
	mCorrectionSizeX = -175;

	mModelRenderDistance["fac_02_int"] = osg::Vec3(-19.7174, 44.5868, 2.8669);
	mModelRenderDistance["fac_05_int"] = osg::Vec3(37.4893, 3.78602, 4.48165);
	mModelRenderDistance["fac_06_int"] = osg::Vec3(62.7727, -31.9886, 2.6502);
	
	mMapDebug = MAP_NO_DEBUG;//MAP_X_DEBUG;
}

OFVInputComponent::~OFVInputComponent()
{
}



///////////////////////////////////////////////////////////////////////////
void OFVInputComponent::ProcessMessage(const dtGame::Message& message)
{
	const dtGame::MessageType& msgType = message.GetMessageType();

	// TICK LOCAL
	if (msgType == dtGame::MessageType::TICK_LOCAL && mMapLoaded)
	{
		//std::cout << "TICK " << mTicks << "\n";
		if (!mInitalizedPlayer)
		{
			if (mTicks == 1)
			{
				//std::cout << "Updating map \n";
				updateMap();
				//std::cout << "Done.\n";
			}
			mTicks++;
			if (mTicks > 20)
			{
				
				mInitalizedPlayer = true;
			//	std::cout << "Switching motion models \n";
				switchMotionModels();
			//	std::cout << "Done.\n";
				//mProgressDialog->hide();

			}
			mProgressDialog->setValue(mTicks > 20 ? 20 : mTicks);
	
		}
		else
		{
			mTicks++;
			if (mTicks > 2)
			{
				mTicks = 0;
			//	std::cout << "Updating map \n";
				updateMap();
				//std::cout << "Done.\n";
				//std::cout << "Check model visibility\n";
				checkModelVisibility();
				//std::cout << "Done.\n";
				//std::cout << "checkWindowSizeChanged \n";
				checkWindowSizeChanged();
			//	std::cout << "Done.\n";
			}
		}

		//const dtGame::TickMessage& tick = dynamic_cast<const dtGame::TickMessage&>(message);

	}
	// A Local Player entered world, so create our motion models
	else if (msgType == dtGame::MessageType::INFO_PLAYER_ENTERED_WORLD)
	{
		LOG_INFO("INFO_PLAYER_ENTERED_WORLD");
		

	}//PLAYER_ENTERED_WORLD
	else if (msgType == dtGame::MessageType::INFO_ACTOR_DELETED)
	{

	}
	else if (msgType == dtGame::MessageType::INFO_MAP_CHANGE_END)
	{
		LOG_INFO("INFO_MAP_CHANGE_END");


		dtGame::GameManager &gameManager = *GetGameManager();
		dtCore::Camera& cam = *(GetGameManager()->GetApplication().GetCamera());

		mPlayerActor = NULL;

		dtCore::ActorProxy* playerprox = NULL;
		GetGameManager()->FindActorByName("PlayerJG", playerprox);
		if (playerprox == 0)
		{
			LOG_ERROR("Failed to find Player in the map. ");

		}
		else
		{

			dtCore::TransformableActorProxy* transproxy = dynamic_cast<dtCore::TransformableActorProxy*>(playerprox);
			mPlayerActor = dynamic_cast<dtCore::Transformable*>(transproxy->GetActor());


			printPlayerLocation();

		}

		osg::Vec3 gravity;
		GetGameManager()->GetScene().GetGravity(gravity);
		std::cout << gravity << "\n";
		/*
		mCollisionMotionModel = new dtCore::CollisionMotionModel(1.5f, 0.4f, 0.1f,
			&GetGameManager()->GetScene(),
			GetGameManager()->GetApplication().GetKeyboard(),
			GetGameManager()->GetApplication().GetMouse(), 10.0, 0.5, 10.0, 5.0, 0.0, 5.0, false);
		mCollisionMotionModel->SetTarget(mPlayerActor); 
		mCollisionMotionModel->SetEnabled(false);
		*/
		//mCollisionMotionModel->SetRecenterMouse(false);
		//mCollisionMotionModel->SetUseMouseButtons(true);
		
	

		
		// Motion model to walk around the scene.			
		mHumanMotionModel = new HumanMotionModel(1.5f, 0.2f, 0.2f,
			&GetGameManager()->GetScene(), 
			GetGameManager()->GetApplication().GetKeyboard(),
			GetGameManager()->GetApplication().GetMouse());
		mHumanMotionModel->SetTarget(mPlayerActor);
		mHumanMotionModel->SetScene(&GetGameManager()->GetScene());
		mHumanMotionModel->SetEnabled(true);

		// create the fly motion model


		mFlyMM = new FlyLimitMotionModel(osg::Vec3(-124.016, -107.681, -10.0),
			osg::Vec3(124.106, 108.85, 61.7289),
			GetGameManager()->GetApplication().GetKeyboard(),
			GetGameManager()->GetApplication().GetMouse());
		  
		mFlyMM->SetTarget(mPlayerActor);
		mFlyMM->SetEnabled(false);

		dtCore::System::GetInstance().SetUseFixedTimeStep(false); //CRITICAL FOR GETTING MOTION MODEL TO WORK

		cam.Emancipate();
		dtCore::Transform xform;
		xform.MakeIdentity();
		cam.SetTransform(xform);


		cam.SetNearFarCullingMode(dtCore::Camera::AutoNearFarCullingMode::NO_AUTO_NEAR_FAR);
		osg::Camera* osgCam = cam.GetOSGCamera();
		osgCam->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
		osgCam->setCullingMode(osg::CullSettings::ENABLE_ALL_CULLING);

		double vfov, aspectRatio, nearClip, farClip;
		cam.GetPerspectiveParams(vfov, aspectRatio, nearClip, farClip);
		vfov = 60.0;
		cam.SetPerspectiveParams(vfov, aspectRatio, .1, 20000);
		std::cout << "VFOV " << vfov << "   aspect " << aspectRatio << "\n";

		mPlayerActor->AddChild(&cam);
		 
	
		GetGameManager()->SetPaused(false);

		

		

		//turn off arrow models.
		setActorVisible("arrow_01", false);
		setActorVisible("arrow_02", false);
		setActorVisible("arrow_03", false);
		setActorVisible("arrow_04", false);
		setActorVisible("arrow_05", false);
		setActorVisible("arrow_06", false);
		setActorVisible("arrow_07", false);
		setActorVisible("arrow_08", false);
		setActorVisible("arrow_09", false);
		setActorVisible("arrow_10", false);

		setActorVisible("fac_10", false);
		setActorVisible("acbArrows", false);
		setActorVisible("Colliders", false); 
		setActorVisible("ColliderFence", false);
		setActorVisible("CollidersOuterFence", false);
		setActorVisible("CollidersFlyBox", false);

		/*setActorVisible("fac_02", false);
		setActorVisible("fac_02_int", false);
		setActorVisible("fac_05_int", false);
		setActorVisible("fac_06_int", false);
		*/
		switchMotionModels();
		teleportToPlayerStart();

		mHumanMotionModel->setCollsionBits(0x00000060);


		createHUD();
		createMiniMap();
		createLogo();

		//Doesn't work
		/*
		dtCore::DeltaDrawable* fenceDrawable;
		fenceDrawable = getActorDrawable("fac1_fence_outer");
		fenceDrawable->GetOSGNode()->getOrCreateStateSet()->setMode(GL_CULL_FACE,osg::StateAttribute::OFF|osg::StateAttribute::OVERRIDE);
		*/

		mMapLoaded = true;
	}
}
void OFVInputComponent::checkWindowSizeChanged()
{
	int x, y, width, height;
	dtCore::DeltaWin* window = GetGameManager()->GetApplication().GetWindow();
	window->GetPosition(x, y, width, height);
	if (mLastWindowSize.height() != height || mLastWindowSize.width() != width)
	{
		mLastWindowSize.setHeight(height);
		mLastWindowSize.setWidth(width);

		//update HUD positions.
		mHUD->setViewport(0, 0, width, height);

		// set the projection matrix
		mHUD->setProjectionMatrix(osg::Matrix::ortho2D(0, width, 0, height));
		
		mLogotrans->setPosition(osg::Vec3(width - 25.0, 25.0, 0.0));

		std::cout << "Detected window size change.  New Window " << width << "," << height << "\n";

	}

}
void OFVInputComponent::checkModelVisibility()
{

	dtCore::Transform xform;
	mPlayerActor->GetTransform(xform);
	osg::Vec3 trans = xform.GetTranslation();

	QMap<QString, osg::Vec3>::const_iterator i = mModelRenderDistance.constBegin();
	while (i != mModelRenderDistance.constEnd()) {
		osg::Vec3 pos = i.value();
		QLineF dist(pos.x(), pos.y(), trans.x(), trans.y());
		setActorVisible(i.key().toStdString(), dist.length() < 35.0);
	
		++i;
	}


}
void OFVInputComponent::updateMap()
{
	osg::Vec2 mapSize(2048.0, 2048.0);
	
	dtCore::Transform xform;
	mPlayerActor->GetTransform(xform);
	osg::Vec3 trans = xform.GetTranslation();

	
	osg::Vec2 origin(-113.237, -240.621);
	osg::Vec2 max(124.203, 183.679);

	osg::Vec2 size = max - origin;
	
	size.x() += mCorrectionSizeY;
	size.y() += mCorrectionSizeX; //reversed X and Y 
	
	//create transform based on mapsize
	osg::Vec2 posToMap(mapSize.x() / size.x(), mapSize.y() / size.y());

	//diff is the player offset from origin.
	osg::Vec2 diff(trans.x() - origin.x(), trans.y() - origin.y());

	osg::Vec2 textureOffset(posToMap.x() * diff.x(), posToMap.y() * diff.y());
	//std::cout << "offset " << textureOffset.x() << "," << textureOffset.y() << "\n";


	applyTextureOffset(mMapGeode, textureOffset, mMapWindowSize, mapSize);

}
void OFVInputComponent::createLogo()
{

	int x, y, width, height;
	dtCore::DeltaWin* window = GetGameManager()->GetApplication().GetWindow();
	window->GetPosition(x, y, width, height);

	float scaleFactor = 0.5;

	mLogotrans = new osg::PositionAttitudeTransform();
	mLogotrans->setPosition(osg::Vec3(width - 25.0, 25.0, 0.0));
	mLogotrans->setScale(osg::Vec3(scaleFactor, scaleFactor, scaleFactor));


	osg::Geode* logo = loadOSGImage("iaea.png", osg::Vec2(0, 0), osg::Vec2(0, 0));
	mLogotrans->addChild(logo);

	mHUD->addChild(mLogotrans);

}
void OFVInputComponent::createHUD()
{
	dtCore::Camera& cam = *(GetGameManager()->GetApplication().GetCamera());


	int x, y, width, height;
	dtCore::DeltaWin* window = GetGameManager()->GetApplication().GetWindow();
	window->GetPosition(x, y, width, height);

	// create a camera to set up the projection and model view matrices, and the subgraph to drawn in the HUD
	mHUD = new osg::Camera;

	// set the projection matrix
	mHUD->setProjectionMatrix(osg::Matrix::ortho2D(0, width, 0, height));

	// set the view matrix    
	mHUD->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	mHUD->setViewMatrix(osg::Matrix::identity());

	// only clear the depth buffer
	mHUD->setClearMask(GL_DEPTH_BUFFER_BIT);

	// draw subgraph after main camera view.
	mHUD->setRenderOrder(osg::Camera::POST_RENDER);

	// we don't want the camera to grab event focus from the viewers main camera(s).
	mHUD->setAllowEventFocus(false);

	// set up cameras to rendering on the first window available.
	mHUD->setGraphicsContext(cam.GetOSGCamera()->getGraphicsContext());
	mHUD->setViewport(0, 0, width, height);
	osgViewer::GraphicsWindow::Views views;
	window->GetOsgViewerGraphicsWindow()->getViews(views);
	views.front()->addSlave(mHUD, false);
}
void OFVInputComponent::createMiniMap()
{
	

	
	float scaleFactor = 1.0;


	osg::ref_ptr<osg::PositionAttitudeTransform> maptrans = new osg::PositionAttitudeTransform();
	maptrans->setPosition(osg::Vec3(mMapWindowSize.x() / 2.0, mMapWindowSize.y() / 2.0, 0.0));
	maptrans->setScale(osg::Vec3(scaleFactor, scaleFactor, scaleFactor));


	mMapGeode = loadOSGImage("Facility_Map.png", osg::Vec2(0, 0),  mMapWindowSize);
	maptrans->addChild(mMapGeode);

	mHUD->addChild(maptrans);
	

	osg::ref_ptr<osg::PositionAttitudeTransform> dottrans = new osg::PositionAttitudeTransform();
	dottrans->setPosition(osg::Vec3(mMapWindowSize.x() / 2.0, mMapWindowSize.y() / 2.0, 0.0));
	dottrans->setScale(osg::Vec3(scaleFactor, scaleFactor, scaleFactor));
	mDot = loadOSGImage("dot.png", osg::Vec2(0, 0), osg::Vec2(9,9));
	mDot->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	dottrans->addChild(mDot);
	mHUD->addChild(dottrans);

}
osg::Texture2D* OFVInputComponent::loadTexture(const std::string& path)
{
	osg::Image *im = NULL;
	
	if (path != "")
	{
		im = osgDB::readImageFile(path);
	}
	else
	{
		return NULL;
	}

	if (!im)
	{
		std::cout << "Couldn't find texture.\n";
		return NULL;
	}

	osg::Texture2D* txtr = new osg::Texture2D();
	txtr->setDataVariance(osg::Object::DYNAMIC);
	txtr->setImage(im);
	txtr->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP_TO_BORDER);
	txtr->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::CLAMP_TO_BORDER);

	return txtr;
}

osg::Geode* OFVInputComponent::loadOSGImage(std::string textureFile,
											osg::Vec2 textureOffset,
											osg::Vec2 textureSize)
{

	osg::Geode* frontQuad = new osg::Geode();

	osg::Geometry* quadGeometry = new osg::Geometry();
	osg::Vec3Array* quadVertexArray = new osg::Vec3Array();
	osg::Vec2Array* quadTexCoords = new osg::Vec2Array(4);
	osg::DrawElementsUInt* quadPrimSet =
		new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);

	osg::StateSet* frontSS = frontQuad->getOrCreateStateSet();
	osg::PolygonMode* frontPolygonMode = new osg::PolygonMode(osg::PolygonMode::FRONT,
		osg::PolygonMode::FILL);

	frontSS->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

	osg::Texture2D* tex = loadTexture(textureFile);

	int imagewidth;
	int imageheight;
	if (!tex)
	{
		std::cout << "ERROR: could not apply texture\n";

	}
	else
	{
		frontSS->setTextureAttributeAndModes(0, tex, osg::StateAttribute::ON);
		imagewidth = tex->getImage()->s();
		imageheight = tex->getImage()->t();
		//std::cout << "Detected image width as " << imagewidth << "\n";
	}

	//create the geometery (rectangle) to texture with image.
	float geomWidth = textureSize.x();
	float geomHeight = textureSize.y();
	if (textureSize.x() == 0.0 && textureSize.y() == 0)
	{
		geomWidth = imagewidth;
		geomHeight = imageheight;
	}
	quadVertexArray->push_back(osg::Vec3(-geomWidth / 2.0, geomHeight / 2.0, 0.0f));
	quadVertexArray->push_back(osg::Vec3(-geomWidth / 2.0, -geomHeight / 2.0, 0.0f));
	quadVertexArray->push_back(osg::Vec3(geomWidth / 2.0, -geomHeight / 2.0, 0.0f));
	quadVertexArray->push_back(osg::Vec3(geomWidth / 2.0, geomHeight / 2.0, 0.0f));
	
	quadPrimSet->push_back(0);
	quadPrimSet->push_back(1);
	quadPrimSet->push_back(2);
	quadPrimSet->push_back(3);

	//apply offset textureOffset to image texture
	float dx = textureOffset.x() / imagewidth;
	float dy = textureOffset.y() / imageheight;
	float xMax = geomWidth / imagewidth; //  1.0 if geometry is same as texture dimensions.
	float yMax = geomHeight / imageheight;

	(*quadTexCoords)[0].set(0.0f + dx, yMax + dy);
	(*quadTexCoords)[1].set(0.0f + dx, 0.0f + dy);
	(*quadTexCoords)[2].set(xMax + dx, 0.0f + dy);
	(*quadTexCoords)[3].set(xMax + dx, yMax + dy);

	frontQuad->addDrawable(quadGeometry);

	quadGeometry->setUseDisplayList(false);
	quadGeometry->setVertexArray(quadVertexArray);
	quadGeometry->setTexCoordArray(0, quadTexCoords);
	quadGeometry->addPrimitiveSet(quadPrimSet);


	//setup Normals
	osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
	normals->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));
	quadGeometry->setNormalArray(normals.get());
	quadGeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);



	frontSS->setAttributeAndModes(frontPolygonMode,
		osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

	// material

	osg::ref_ptr<osg::Material> material = new osg::Material;
	//material->setColorMode(osg::Material::DIFFUSE);
	material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0, 1.0, 1.0, 1));
	material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0, 1.0, 1.0, 1));

	//material->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
	//material->setShininess(osg::Material::FRONT_AND_BACK, 64.0f);

	frontSS->setAttributeAndModes(material.get(), osg::StateAttribute::ON);

	//frontSS->setMode(GL_LIGHTING,osg::StateAttribute::OFF);


	
	return frontQuad;
}
void OFVInputComponent::applyTextureOffset(osg::Geode* geode,
											osg::Vec2 textureOffset,
											osg::Vec2 GeomSize,
											osg::Vec2 imageSize)
{
	
	osg::Geometry* quadGeometry = geode->getChild(0)->asGeometry();
	osg::Vec2Array* quadTexCoords = (osg::Vec2Array*)quadGeometry->getTexCoordArray(0);

	float geomWidth = GeomSize.x();
	float geomHeight = GeomSize.y();

	//apply offset textureOffset to image texture
	float mapDx = textureOffset.x() / imageSize.x();
	float mapDy = textureOffset.y() / imageSize.y();
	
	float zoom = .08;
	//zoom = 0;

	//float dx = 1.0 + mapDy;
	//float dy = mapDx * -1.0;


	float dx = 1.0 - mapDy;
	float dy = mapDx;
	

	//std::cout << "dx dy " << dx << "," << dy << "\n";
	float xMax = geomWidth / imageSize.x(); //  1.0 if geometry is same as texture dimensions.
	float yMax = geomHeight / imageSize.y();
	 
	//correct texture offset to show player in center of map not bottom left corner.
	dx -= xMax / 2.0;
	dy -= yMax / 2.0;	

	dx += mCorrectionX;
	dy += mCorrectionY;

	(*quadTexCoords)[0].set((0.0f + dx) - zoom, (yMax + dy) + zoom);
	(*quadTexCoords)[1].set((0.0f + dx) - zoom, (0.0f + dy) - zoom);
	(*quadTexCoords)[2].set((xMax + dx) + zoom, (0.0f + dy) - zoom);
	(*quadTexCoords)[3].set((xMax + dx) + zoom, (yMax + dy) + zoom);

	

	
	

}
void OFVInputComponent::printPlayerLocation()
{
	dtCore::Transform xform;
	mPlayerActor->GetTransform(xform);
	osg::Vec3 trans = xform.GetTranslation();
	std::cout << "Player location " << trans.x() << "," << trans.y() << "," << trans.z() << "\n";
	osg::Vec3 rot = xform.GetRotation();
	std::cout << "Player Rotation " << rot.x() << "," << rot.y() << "," << rot.z() << "\n";

}
void OFVInputComponent::teleportToPlayerStart()
{
	std::vector<dtCore::ActorProxy*> actors;
	const dtCore::ActorType *type = GetGameManager()->FindActorType("dtcore", "Player Start");
	GetGameManager()->FindActorsByType(*type, actors);
	RefPtr<dtCore::TransformableActorProxy> proxy = NULL;
	if (actors.empty())
	{
		LOG_WARNING("Failed to find a player start proxy in the map. Defaulting to [0, 0, 0]");
	}
	else
	{
		proxy = dynamic_cast<dtCore::TransformableActorProxy*>(actors[0]);
		osg::Vec3 startPos = proxy->GetTranslation();
		osg::Vec3 startRot = proxy->GetRotation();
		dtCore::Transform stealthStart; 
	
		stealthStart.SetTranslation(startPos.x(), startPos.y(), startPos.z());
		stealthStart.SetRotation(startRot);
		std::cout << "setting player location to " << startPos.x() << "," << startPos.y() << "," << startPos.z() << "\n";
		mPlayerActor->SetTransform(stealthStart);

	//	dtCore::Camera& cam = *(GetGameManager()->GetApplication().GetCamera());
	//	cam.SetTransform(stealthStart);
	}
}
void OFVInputComponent::jumpTo(double x, double y, double z, double h, double p, double r)
{
	dtCore::Transform destination;

	destination.SetTranslation(x, y, z);
	destination.SetRotation(h, p, r);
	std::cout << "setting player location to " << x << "," << y << "," << z << "\n";
	mPlayerActor->SetTransform(destination);
}
void OFVInputComponent::setActorVisible(std::string actorName, bool visible)
{
	if (actorName == "navigation_map_OFV_STRING")
	{
		mDot->setNodeMask(visible ? 0xffffffff : 0x0);
		mMapGeode->setNodeMask(visible ? 0xffffffff : 0x0);
		
		return;
	}

	dtCore::ActorProxy* actor;
	GetGameManager()->FindActorByName(actorName, actor);
	
	if (actor == NULL)
	{
		LOG_WARNING("Failed to find actor in map.");
	}
	else
	{
		dtCore::DeltaDrawable* drawable =
			static_cast<dtCore::DeltaDrawable*>(actor->GetActor());

		drawable->SetActive(visible);
		
	}

	if (actorName == "fac1_fence")
	{
		if (visible)
			mHumanMotionModel->setCollsionBits(0x00000060);//collide with inner fence
		else
			mHumanMotionModel->setCollsionBits(0x00000020);//don't collide with inner fence.
	}
}
dtCore::Transformable* OFVInputComponent::getActorTransformable(std::string actorName)
{
	dtCore::ActorProxy* actor;
	GetGameManager()->FindActorByName(actorName, actor);

	if (actor == NULL)
	{
		LOG_WARNING("Failed to find a player start proxy in the map. Defaulting to [0, 0, 0]");
	}
	else
	{
		dtCore::Transformable* transformable =
			static_cast<dtCore::Transformable*>(actor->GetActor());

		return transformable;

	}
	return NULL;
}
dtCore::DeltaDrawable* OFVInputComponent::getActorDrawable(std::string actorName)
{
	dtCore::ActorProxy* actor;
	GetGameManager()->FindActorByName(actorName, actor);

	if (actor == NULL)
	{
		LOG_WARNING("Failed to find a player start proxy in the map. Defaulting to [0, 0, 0]");
	}
	else
	{
		dtCore::DeltaDrawable* drawable =
			static_cast<dtCore::DeltaDrawable*>(actor->GetActor());

		return drawable;

	}
	return NULL;
}
void OFVInputComponent::switchMotionModels()
{
	if (mFlyMM->IsEnabled())
	{
		//Switch to FPS
		mFlyMM->SetEnabled(false);
		mHumanMotionModel->SetEnabled(true);
	

		//GetGameManager()->GetApplication().GetWindow()->SetShowCursor(false);
	}
	else
	{ 
		//Switch to Fly
		mFlyMM->SetEnabled(true);
		mHumanMotionModel->SetEnabled(false);
	
		//GetGameManager()->GetApplication().GetWindow()->SetShowCursor(true);
	}

}
void OFVInputComponent::multiplyMotionModelSpeed(float factor)
{

	if (mFlyMM == NULL || mHumanMotionModel == NULL)
	{
		LOG_ERROR("requested MotionModel which NULL (not yet ready.)");
		return;
	}
	if (mFlyMM->IsEnabled())
	{
		float val = mFlyMM->GetMaximumFlySpeed();
		mFlyMM->SetMaximumFlySpeed(val * factor);
		std::cout << "Fly speed set to " << (val * factor) << "\n";

		//val = mFlyMM->GetMaximumTurnSpeed();
		//mFlyMM->SetMaximumTurnSpeed(val * factor);
	}
	else
	{
		float val = mHumanMotionModel->GetMaximumWalkSpeed();
		mHumanMotionModel->SetMaximumWalkSpeed(val * factor);
		std::cout << "Walk speed set to " << (val * factor) << "\n";

		val = mHumanMotionModel->GetMaximumSidestepSpeed();
		mHumanMotionModel->SetMaximumSidestepSpeed(val * factor);
	}

}
void OFVInputComponent::multiplyTurnModelSpeed(float factor)
{

	if (mFlyMM == NULL || mHumanMotionModel == NULL)
	{
		LOG_ERROR("requested MotionModel which NULL (not yet ready.)");
		return;
	}
	if (mFlyMM->IsEnabled())
	{

	}
	else
	{

		float val = mHumanMotionModel->GetMaximumTurnSpeed();
		mHumanMotionModel->SetMaximumTurnSpeed(val * factor);
		std::cout << "Walk speed set to " << (val * factor) << "\n";

	}

}
void OFVInputComponent::setFOV(double diff)
{
	
	dtCore::Camera& cam = *(GetGameManager()->GetApplication().GetCamera());
	double vfov, aspectRatio, nearClip, farClip;
	cam.GetPerspectiveParams(vfov, aspectRatio, nearClip, farClip);
	vfov += diff;
	cam.SetPerspectiveParams(vfov, aspectRatio, .1, 20000);
	std::cout << "VFOV " << vfov << "   aspect " << aspectRatio << "\n";
}
/////////////////////////////////////////////////////////////////////////////
bool OFVInputComponent::HandleKeyPressed(const dtCore::Keyboard* keyboard, int key)
{

	bool handled = true;
	switch (key)
	{
	case '=':
	case '+':
		multiplyMotionModelSpeed(2.0);
		break;
	case '-':
	case '_':
		multiplyMotionModelSpeed(0.5);
		break;
	case ']':
	case '}':
		multiplyTurnModelSpeed(2.0);
		break;
	case '{':
	case '[':
		multiplyTurnModelSpeed(0.5);
		break;
	case 'm':
	case 'M':
		switchMotionModels();
		break;
	case 'i':
		if (mMapDebug == MAP_X_DEBUG)
		{
			mCorrectionX += 0.005;
			std::cout << "mCorrectionX " << mCorrectionX << "\n";
		}
		else if(mMapDebug == MAP_Y_DEBUG)
		{
			mCorrectionY += 0.005;
			std::cout << "mCorrectionY " << mCorrectionY << "\n";
		}
		
		break;
	case 'o':
		
		if (mMapDebug == MAP_X_DEBUG)
		{
			mCorrectionX -= 0.005;
			std::cout << "mCorrectionX " << mCorrectionX << "\n";
		}
		else if (mMapDebug == MAP_Y_DEBUG)
		{
			mCorrectionY -= 0.005;
			std::cout << "mCorrectionY " << mCorrectionY << "\n";
		}

		break;
	case 'c':
		if (mMapDebug == MAP_X_DEBUG)
		{
			mCorrectionSizeX += 1.0;
			std::cout << "mCorrectionSizeX " << mCorrectionSizeX << "\n";
		}
		else if (mMapDebug == MAP_Y_DEBUG)
		{
			mCorrectionSizeY += 1.0;
			std::cout << "mCorrectionSizeY " << mCorrectionSizeY << "\n";
		}
		
		break;
	case 'v':
			if (mMapDebug == MAP_X_DEBUG)
		{
			mCorrectionSizeX -= 1.0;
			std::cout << "mCorrectionSizeX " << mCorrectionSizeX << "\n";
		}
		else if (mMapDebug == MAP_Y_DEBUG)
		{
			mCorrectionSizeY -= 1.0;
			std::cout << "mCorrectionSizeY " << mCorrectionSizeY << "\n";
		}
		break;
	case 'b':
	case 'B':
		teleportToPlayerStart();
		//mFlyMM->SetEnabled(false);
		//mHumanMotionModel->SetEnabled(true);
		break;
	case osgGA::GUIEventAdapter::KEY_Space:
		mHumanMotionModel->toggleCollision();
		
		break;
	case 'z':
	case '1':
		printPlayerLocation();
		break;

	default:
		// Implemented to get rid of warnings in Linux
		handled = false;
		break;
	}



	return handled;
}