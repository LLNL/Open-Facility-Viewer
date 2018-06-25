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

#include <dtActors/triggervolumeactorproxy.h>
#include <dtActors/triggervolumeactor.h>
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

#include <dtActors/weatherenvironmentactor.h>
#include <dtUtil/datetime.h>

using dtCore::RefPtr;

OFVInputComponent::OFVInputComponent()
	: dtGame::BaseInputComponent("Input Component"),
	mInitalizedPlayer(false),
	mMapLoaded(false),
	mInteriorMap(false),
	mTicks(0),
	mMapWindowSize(osg::Vec2(250.0, 250.0)),
	mReactorBuildingEnabled(true),
	mFuelFabEnabled(true)
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

	mModelRenderPoint["fac_06"] = osg::Vec3(65.0662, -29.5637, 2.8669);
	mModelRenderDistance["fac_06"] = 120.0;
	mModelRenderPoint["fac_04"] = osg::Vec3(65.0662, -29.5637, 2.8669);
	mModelRenderDistance["fac_04"] = 140.0;
	mModelRenderPoint["fac_07"] = osg::Vec3(-102.015, 26.4696, 1.55965);  //just the guard at the front.
	mModelRenderDistance["fac_07"] = 20.0;
	mModelRenderPoint["switch_guardbooth"] = osg::Vec3(-102.015, 26.4696, 1.55965);  //just the guard at the front.
	mModelRenderDistance["switch_guardbooth"] = 20.0;

	//Model render point for ACB is far corner by start point so that it doesn't render when at opposite corner.
	mModelRenderPoint["fac_02"] = osg::Vec3(-68.6675, 67.334, 1.55965); 
	mModelRenderDistance["fac_02"] = 160.0;
	
	mModelRenderPoint["fac_08_cars"] = osg::Vec3(-68.6675, 67.334, 1.55965);  
	mModelRenderDistance["fac_08_cars"] = 140.0;
	


	//TODO doors

	/******  NAVIGATION MAPS  *****/
	float reactorBuffer = -1.0;
	float acbBuffer = 1.0;
	float fuelFabBuffer = 2.0;
	const osg::Vec4d beige(0.5, 0.4453125, 0.37109375, 1.0);
	const osg::Vec4d gray(0.33, 0.33, 0.33, 1.0);
	
	mMainMapTexture = loadTexture("Facility_Map.png", beige);
	
	
	const float halfFloorHeight = 1.0;

	const QString abFilename = "AccessBuilding.png";
	InteriorMap* map = new InteriorMap();
	
	map->origin = osg::Vec2(-26.0, 34.75);
	map->max = osg::Vec2(-10.3, 56.17);
	float groundLevel = 1.45182;
	map->renderBox = osg::BoundingBoxd(map->origin.x() - acbBuffer,
									   map->origin.y() - acbBuffer,
										groundLevel - halfFloorHeight,
										map->max.x() + acbBuffer,
										map->max.y() + acbBuffer,
										groundLevel + halfFloorHeight);

	map->textureSize = osg::Vec2(556.0, 369.0);
	map->mapVisible = false;
	map->texture = loadTexture(abFilename.toStdString(), gray);
	map->interiorModel = "fac_02_int";
	mInteriorMaps[abFilename] = map;
	

	/////////////////// REACTOR BUILDING ////////////////
	osg::Vec2 reactorTextureSize = osg::Vec2(245.0, 800.0);
	osg::Vec2 reactorOrigin = osg::Vec2(19.0295, 1.14177);
	osg::Vec2 reactorMax = osg::Vec2(74.2006, 17.057); 

	float reactorDistance = 30.0;

	for (int i = 0; i < 5; i++)
	{
		map = new InteriorMap();
		float z = -3.42; //basement
		if (i == 1)
			z = 1.55;  //ground
		else if (i == 2)
			z = 4.62; //1st
		else if (i == 3)
			z = 7.28;  //2nd
		else if (i == 4)
			z = 9.9; //3rd

	
		map->distance = reactorDistance;
		map->origin = reactorOrigin;
		map->max = reactorMax;
	
		map->renderBox = osg::BoundingBoxd( map->origin.x() - reactorBuffer,
											map->origin.y() - reactorBuffer,
											z - halfFloorHeight,
											map->max.x() + reactorBuffer,
											map->max.y() + reactorBuffer,
											z + halfFloorHeight);

		map->textureSize = reactorTextureSize;
		map->mapVisible = false;
		map->interiorModel = "fac_05_int";

		const QString reactorFilename = "ResearchReactor_" + QString::number(i) + ".png";
		map->texture = loadTexture(reactorFilename.toStdString(), gray);
		mInteriorMaps[reactorFilename] = map;
	}


	/////////////////// FUEL FAB BUILDING ////////////////
	osg::Vec2 fabTextureSize = osg::Vec2(484.0, 385.0);
	osg::Vec2 fabOrigin = osg::Vec2(58.7341, -41.4878);
	osg::Vec2 fabMax = osg::Vec2(75.0433, -21.19074);	

	float fabricationDistance = 10.0;

	for (int i = 0; i < 3; i++)
	{
		map = new InteriorMap();
		float z = -1.84;
		if (i == 1)
			z = 1.54;
		else if (i == 2)
			z = 7.14;

		
		map->distance = fabricationDistance;
		map->origin = fabOrigin;
		map->max = fabMax;
		
		map->renderBox = osg::BoundingBoxd(map->origin.x() - fuelFabBuffer,
											map->origin.y() - fuelFabBuffer,
											z - halfFloorHeight,
											map->max.x() + fuelFabBuffer,
											map->max.y() + fuelFabBuffer,
											z + halfFloorHeight);

		map->textureSize = fabTextureSize;
		map->mapVisible = false;
		map->interiorModel = "fac_06_int";

		const QString fabFilename = "FabricationBuilding_" + QString::number(i) + ".png";
		map->texture = loadTexture(fabFilename.toStdString(), gray);
		mInteriorMaps[fabFilename] = map;
	}

	//////////////////// STORAGE BUNKER /////////////////////
	map = new InteriorMap();

	map->distance = 7.0;
	map->origin = osg::Vec2(80.509, 45.809);
	map->max = osg::Vec2(93.1563, 62.157);

	map->renderBox = osg::BoundingBoxd(map->origin.x() - reactorBuffer,
										map->origin.y() - reactorBuffer,
										groundLevel - halfFloorHeight,
										map->max.x() + reactorBuffer,
										map->max.y() + reactorBuffer,
										groundLevel + halfFloorHeight);


	map->textureSize = osg::Vec2(600.0, 485.0);
	map->mapVisible = false;


	const QString bunkerFilename = "StorageBunker.png";
	map->texture = loadTexture(bunkerFilename.toStdString(), gray);
	mInteriorMaps[bunkerFilename] = map;


	//add the bounding box for the basement (ignore z elevation.)

	//Fuel Fab Basement
	mElevationLimitBoxes.push_back(osg::BoundingBoxd(55.3477, -43.0503, -100000.0, 74.1745, -25.5124, 100000.0));
	mElevationLimits.push_back(-1.85);

	//Fuel Fab Ramp
	mElevationLimitBoxes.push_back(osg::BoundingBoxd(41.062, -38.0, -100000.0, 58.6752, -31.0, 100000.0));
	mElevationLimits.push_back(-1.85);
	
	//Reactor Basement
	mElevationLimitBoxes.push_back(osg::BoundingBoxd(25.8509, 2.85, -100000.0, 74.1745, 15.34, 100000.0));
	mElevationLimits.push_back(-4.53);
	
	mGlobalElevationLimit = 1.0;

	mMapDebug = MAP_NO_DEBUG;//;


}

OFVInputComponent::~OFVInputComponent()
{
	/*QMap<QString, InteriorMap*>::const_iterator i = mInteriorMaps.constBegin();
	while (i != mInteriorMaps.constEnd()) {
		delete i.value();
	}*/
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
				fixTimeZone();
				
				

				//std::cout << "Done.\n";
			}
			mTicks++;
			if (mTicks < 20)
			{
				//cycle through the interiors and preload each one 
				if (mTicks == 6)
				{
					std::cout << "Loading interior fac_02_int\n";
					setActorVisible("fac_02_int", true);
				}
				else if (mTicks == 8)
				{
					std::cout << "Loading interior fac_05_int\n";
					setActorVisible("fac_02_int", false);
					setActorVisible("fac_05_int", true);
				}
				else if (mTicks == 10)
				{
					std::cout << "Loading interior fac_06_int\n";
					setActorVisible("fac_05_int", false);
					setActorVisible("fac_06_int", true);
				}
				else if (mTicks == 12)
				{
					setActorVisible("fac_06_int", false);
				}
			}
			else if (mTicks > 20)
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
			if (mTicks % 2)
			{
				
				checkVisibilitys();
				updateMap();
				checkPlayerElevation();
			}
			if (mTicks % 5)
			{
				updateModelDistanceVisibility();
				checkWindowSizeChanged();
			}
			
			if (mTicks > 10)
			{
				mTicks = 0;
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



		
		setActorVisible("acbArrows", false);

		setActorVisible("Colliders", false); 
		setActorVisible("Colliders_fac_05_OFF", false);
		setActorVisible("Colliders_fac_05_ON", false);
		setActorVisible("Colliders_fac_06_OFF", false);
		setActorVisible("Colliders_fac_06_ON", false);
		setActorVisible("Colliders_guard_tower", false);
		
		setActorVisible("Colliders_fence_admin", false);
		setActorVisible("Colliders_fence_inner", false);
		setActorVisible("Colliders_fence_outer", false);
		setActorVisible("Colliders_fence_scrapyard", false);


		setActorVisible("CollidersFlyBox", false);

		setActorVisible("switch_ground_fab_OFF", false);
		setActorVisible("switch_ground_reactor_OFF", false);

		setActorVisible("switch_microwave_nolights", false);
		
		
		setActorVisible("fac_02_int", false);
		setActorVisible("fac_05_int", false);
		setActorVisible("fac_06_int", false);
		
		switchMotionModels();
		teleportToPlayerStart();

		//reset collision to be everything.  (Set actor visible change it.)
		mHumanMotionModel->setCollsionBits(COLLISION_CATEGORY_BIT_FAC_05_ON |
										   COLLISION_CATEGORY_BIT_FAC_06_ON | 
										   COLLISION_CATEGORY_BIT_FENCE_INNER |
											COLLISION_CATEGORY_BIT_OBJECT);


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

//called only when going in or out of building or floor.
void OFVInputComponent::updateModelVisibility()
{
	

	//set all false
	{
		QMap<QString, InteriorMap*>::const_iterator i = mInteriorMaps.constBegin();
		while (i != mInteriorMaps.constEnd()) {
			setActorVisible(i.value()->interiorModel.toStdString(), false);
			++i;
		}
	}
	//set only one visible model ON, since there are floors we have duplicate models.
	QMap<QString, InteriorMap*>::const_iterator i = mInteriorMaps.constBegin();
	while (i != mInteriorMaps.constEnd()) {
		if (i.value()->mapVisible)
		{

			//Override visibilty when model building is turned off.
			if (i.value()->interiorModel == "fac_05_int" && !mReactorBuildingEnabled)
				break;
			if (i.value()->interiorModel == "fac_06_int" && !mFuelFabEnabled)
				break;

			setActorVisible(i.value()->interiorModel.toStdString(), true);
			break;
		}
		++i;
	}


	//turn on/off all outdoor models.
	setActorVisible("trees_01", !mInteriorMap);
	setActorVisible("foliage", !mInteriorMap);

	//these should be permanently on (not switchable.)
	setActorVisible("switch_lighting_admin", !mInteriorMap);
	setActorVisible("switch_fence_admin", !mInteriorMap);
	setActorVisible("switch_admin_bldgs", !mInteriorMap);
	setActorVisible("switch_waste_outer", !mInteriorMap);
	//setActorVisible("switch_waste_inner", !mInteriorMap);
	//setActorVisible("fac_fence_scrapyardWaste", !mInteriorMap);

	


}
void OFVInputComponent::updateModelDistanceVisibility()
{

	dtCore::Transform xform;
	mPlayerActor->GetTransform(xform);
	osg::Vec3 trans = xform.GetTranslation();


	QMap<QString, osg::Vec3>::const_iterator i = mModelRenderPoint.constBegin();
	while (i != mModelRenderPoint.constEnd()) {

		if (i.key() == "fac_06" && !mFuelFabEnabled)
		{
			//do nothing
		}
		else if (i.key() == "fac_05" && !mReactorBuildingEnabled)
		{
			//do nothing
		}
		else if (mFlyMM->IsEnabled())
		{
			setActorVisible(i.key().toStdString(), true);
		}
		else
		{
			osg::Vec3 pos = i.value();
			QLineF dist(pos.x(), pos.y(), trans.x(), trans.y());
			setActorVisible(i.key().toStdString(), dist.length() < mModelRenderDistance[i.key()]);
		}
		++i;
	}
}
void OFVInputComponent::setVisibilityAllInteriorMaps(bool visible)
{
	QMap<QString, InteriorMap*>::const_iterator i = mInteriorMaps.constBegin();
	while (i != mInteriorMaps.constEnd()) {
		if (i.value()->mapVisible)
		{
			i.value()->mapVisible = visible;
			
		}
		++i;
	}
}
void OFVInputComponent::updateMapVisibility(osg::Vec3 trans)
{

	QMap<QString, InteriorMap*>::const_iterator i = mInteriorMaps.constBegin();
	while (i != mInteriorMaps.constEnd()) {

		//we are in a building
		if (i.value()->renderBox.contains(trans))
		{
			if (  (i.value()->interiorModel == "fac_05_int" && !mReactorBuildingEnabled) ||
				  (i.value()->interiorModel == "fac_06_int" && !mFuelFabEnabled))
			{
				setVisibilityAllInteriorMaps(false);
				mInteriorMap = false;
				break;
			}
			//check if map is already correct.
			if (i.value()->mapVisible)
				return;

			//change map to current floor.
			setVisibilityAllInteriorMaps(false);
			i.value()->mapVisible = true;
			updateMapWithNewTexture(i.value()->texture, mMapGeode);
			mInteriorMap = true;

			//Set model visibility
			updateModelVisibility();
			
			return;
		}
	
		++i;
	}
	//Check if we are still showing an interior map and shouldn't be.
	if (mInteriorMap)
	{
		setVisibilityAllInteriorMaps(false);
		updateMapWithNewTexture(mMainMapTexture, mMapGeode);
		mInteriorMap = false;

		//Set model visibility
		updateModelVisibility();
		
	}
}
void OFVInputComponent::checkVisibilitys()
{

	dtCore::Transform xform;
	mPlayerActor->GetTransform(xform);
	osg::Vec3 trans = xform.GetTranslation();

	updateMapVisibility(trans);
	

}
void OFVInputComponent::checkPlayerElevation()
{

	if (mHumanMotionModel->collisionEnabled() == false)
		return;

	dtCore::Transform xform;
	mPlayerActor->GetTransform(xform);
	osg::Vec3 trans = xform.GetTranslation();

	//check special cases (basements)
	for (int i = 0; i < mElevationLimitBoxes.size(); i++)
	{
		if (mElevationLimitBoxes.at(i).contains(trans))
		{
			if (trans.z() < mElevationLimits.at(i))
			{
				trans.z() = mElevationLimits.at(i);
				xform.SetTranslation(trans);
				mPlayerActor->SetTransform(xform);
			}
			return; //we are in a special case so return, don't do global.
		}
	}


	//check global limit
	if (trans.z() < mGlobalElevationLimit)
	{
		trans.z() = mGlobalElevationLimit;
		xform.SetTranslation(trans);
		mPlayerActor->SetTransform(xform);
		
	}

}
void OFVInputComponent::updateMap()
{
	osg::Vec2 mapSize(2048.0, 2048.0);
	
	
	dtCore::Transform xform;
	mPlayerActor->GetTransform(xform);
	osg::Vec3 trans = xform.GetTranslation();

	//Facility map	
	osg::Vec2 origin(-113.237, -240.621);
	osg::Vec2 max(124.203, 183.679);

	//Interior Map - use special offsets
	if (mInteriorMap)
	{


		QMap<QString, InteriorMap*>::const_iterator i = mInteriorMaps.constBegin();
		while (i != mInteriorMaps.constEnd()) {	
			if (i.value()->mapVisible)
			{
				mapSize = i.value()->textureSize;
				origin = i.value()->origin;
				max = i.value()->max;
			}
			++i;
		}
	}


	osg::Vec2 size = max - origin;
	if (!mInteriorMap)
	{
		size.x() += mCorrectionSizeY;
		size.y() += mCorrectionSizeX; //reversed X and Y 
	}

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

	const osg::Vec4d beige(0.5, 0.4453125, 0.37109375, 1.0);
	mMapGeode = loadOSGImage("Facility_Map.png", osg::Vec2(0, 0),  mMapWindowSize, beige);
	maptrans->addChild(mMapGeode);

	mHUD->addChild(maptrans);
	

	osg::ref_ptr<osg::PositionAttitudeTransform> dottrans = new osg::PositionAttitudeTransform();
	dottrans->setPosition(osg::Vec3(mMapWindowSize.x() / 2.0, mMapWindowSize.y() / 2.0, 0.0));
	dottrans->setScale(osg::Vec3(scaleFactor, scaleFactor, scaleFactor));
	mDot = loadOSGImage("dot.png", osg::Vec2(0, 0), osg::Vec2(9,9));
	mDot->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	mDot->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	dottrans->addChild(mDot);

	osg::ref_ptr<osg::PositionAttitudeTransform> arrowtrans = new osg::PositionAttitudeTransform();
	arrowtrans->setPosition(osg::Vec3(mMapWindowSize.x() - 20 , mMapWindowSize.y() - 30 , 0.0));
	arrowtrans->setScale(osg::Vec3(scaleFactor, scaleFactor, scaleFactor));
	mArrow = loadOSGImage("arrow.png", osg::Vec2(0, 0), osg::Vec2(30,44));
	mArrow->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	mArrow->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	arrowtrans->addChild(mArrow);

	mHUD->addChild(dottrans);
	mHUD->addChild(arrowtrans);
}
osg::Texture2D* OFVInputComponent::loadTexture(const std::string& path, const osg::Vec4d& borderColor)
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
	txtr->setBorderColor(borderColor);
	return txtr;
}
void OFVInputComponent::updateMapWithNewTexture(osg::Texture2D* tex,
										        osg::Geode* mapQuad)
{
	

	
	//int imagewidth;
	//int imageheight;
	if (!tex)
	{
		std::cout << "ERROR: could not apply texture\n";

	}
	else
	{
		osg::StateSet* frontSS = mapQuad->getOrCreateStateSet();
		frontSS->setTextureAttributeAndModes(0, tex, osg::StateAttribute::ON);
		//imagewidth = tex->getImage()->s();
		//imageheight = tex->getImage()->t();
		//std::cout << "Detected image width as " << imagewidth << "\n";
	}
}
osg::Geode* OFVInputComponent::loadOSGImage(std::string textureFile,
											osg::Vec2 textureOffset,
											osg::Vec2 textureSize,
											const osg::Vec4d& borderColor)
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

	osg::Texture2D* tex = loadTexture(textureFile, borderColor);

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

	if (!mInteriorMap)
	{
		dx += mCorrectionX;
		dy += mCorrectionY;
	}
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
void OFVInputComponent::fixTimeZone()
{
	
	std::vector<dtCore::ActorProxy*> actors;
	const dtCore::ActorType *type = GetGameManager()->FindActorType("dtcore.Environment", "WeatherEnvironment");
	GetGameManager()->FindActorsByType(*type, actors);
	RefPtr<dtActors::WeatherEnvironmentActor> proxy = NULL;
	if (actors.empty())
	{
		LOG_ERROR("Failed to find a weather proxy in the map.");
	}
	else
	{
		proxy = dynamic_cast<dtActors::WeatherEnvironmentActor*>(actors[0]->GetActor());
		unsigned year, month, day, hour, min, sec;
		proxy->GetTimeAndDate(year, month, day, hour, min, sec);
		dtUtil::DateTime* date = new dtUtil::DateTime();
		date->SetTime(year, month, day, hour, min, sec);
		date->AdjustTimeZone(dtUtil::DateTime::GetLocalGMTOffset(true));
		date->GetTime(year, month, day, hour, min, sec);
		proxy->SetTimeAndDate(year, month, day, hour, min, sec);
	}
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
void OFVInputComponent::setTeleportActive(std::string actorName, bool active)
{

	if (actorName == "")
		return;

	
	dtCore::ActorProxy* actorp;
	GetGameManager()->FindActorByName(actorName, actorp);

	if (actorp == NULL)
	{
		LOG_WARNING("Failed to find actor " + actorName + " in map.");
		return;
	}
	RefPtr<dtActors::TriggerVolumeActorProxy> proxy = NULL;
	proxy = dynamic_cast<dtActors::TriggerVolumeActorProxy*>(actorp);
	if (proxy == NULL)
	{
		LOG_WARNING("Failed to convert actor " + actorName + " in map.");
		return;
	}
	if(active)
		proxy->SetCollisionType(dtCore::CollisionGeomType::CUBE);
	else
		proxy->SetCollisionType(dtCore::CollisionGeomType::NONE);
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
	if (actorName == "")
		return;

	if (actorName == "navigation_map_OFV_STRING")
	{
		mDot->setNodeMask(visible ? 0xffffffff : 0x0);
		mMapGeode->setNodeMask(visible ? 0xffffffff : 0x0);
		mArrow->setNodeMask(visible ? 0xffffffff : 0x0);
		return;
	}

	dtCore::ActorProxy* actor;
	GetGameManager()->FindActorByName(actorName, actor);
	
	if (actor == NULL)
	{
		LOG_WARNING("Failed to find actor " + actorName + " in map.");
		return;
	}

	QString name = actorName.c_str();
	

	dtCore::DeltaDrawable* drawable =
		static_cast<dtCore::DeltaDrawable*>(actor->GetActor());
	if (name.startsWith("Colliders"))
	{
		unsigned long currentbits = mHumanMotionModel->getCollsionBits();
		unsigned long collider;
		if (actorName == "Colliders_fence_inner")
		{
			collider = COLLISION_CATEGORY_BIT_FENCE_INNER;
		}
		else if (actorName == "Colliders_fac_06_OFF")
		{
			collider = COLLISION_CATEGORY_BIT_FAC_06_OFF;
		}
		else if (actorName == "Colliders_fac_06_ON")
		{
			collider = COLLISION_CATEGORY_BIT_FAC_06_ON;
		}
		else if (actorName == "Colliders_fac_05_OFF")
		{
			collider = COLLISION_CATEGORY_BIT_FAC_05_OFF;
		}
		else if (actorName == "Colliders_fac_05_ON")
		{
			collider = COLLISION_CATEGORY_BIT_FAC_05_ON;
		}
		else if (actorName == "Colliders_guard_tower")
		{
			collider = COLLISION_CATEGORY_BIT_GUARDTOWERS;
		}

		if (visible)
			mHumanMotionModel->setCollsionBits(currentbits | collider);//collide 
		else
			mHumanMotionModel->setCollsionBits(currentbits & ~(collider));//don't collide


		drawable->SetActive(false);
	}
	else
	{
		drawable->SetActive(visible);
	}
		
	

	
}
void OFVInputComponent::setActorVisibleOverride(std::string actorName, bool visible)
{
	if (actorName == "")
		return;

	if (actorName == "fac_05")
		mReactorBuildingEnabled = visible;
	else if (actorName == "fac_06")
		mFuelFabEnabled = visible;

	setActorVisible(actorName, visible);

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
		std::cout << "Turn speed set to " << (val * factor) << "\n";

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
		printPlayerLocation();
		break;

	case '\\':
		
		GetGameManager()->GetApplication().SetNextStatisticsType();
		break;

	default:
		// Implemented to get rid of warnings in Linux
		handled = false;
		break;
	}



	return handled;
}