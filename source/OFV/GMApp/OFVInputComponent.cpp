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

#include <dtABC/application.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/transformableactorproxy.h>
#include <dtDAL/map.h>
#include <dtDAL/project.h>
#include <dtCore/camera.h>
#include <dtCore/deltawin.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtCore/mouse.h>
#include <dtCore/transform.h>
#include <dtCore/flymotionmodel.h>
#include <dtCore/light.h>

#include <dtActors/engineactorregistry.h>
#include <dtActors/gamemeshactor.h>
#include <dtGame/message.h>
#include <dtGame/messagetype.h>
#include <dtGame/gameactor.h>
#include <dtGame/gameapplication.h>
#include <dtGame/basemessages.h>
#include <dtActors/skydomeenvironmentactor.h>
#include <dtGame/environmentactor.h>



#include <dtUtil/fileutils.h>
#include <QtGui/QMessageBox>



using dtCore::RefPtr;

	OFVInputComponent::OFVInputComponent()
    : dtGame::BaseInputComponent("Input Component")
	{
	
  
	}

	OFVInputComponent::~OFVInputComponent()
	{
	}



	///////////////////////////////////////////////////////////////////////////
void OFVInputComponent::ProcessMessage(const dtGame::Message& message)
{
		const dtGame::MessageType& msgType = message.GetMessageType();

   

		// TICK LOCAL
		if (msgType == dtGame::MessageType::TICK_LOCAL)
		{     

            const dtGame::TickMessage& tick = dynamic_cast<const dtGame::TickMessage&>(message);
	
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

			dtDAL::ActorProxy* playerprox = NULL;
			GetGameManager()->FindActorByName("PlayerJG", playerprox);
			if(playerprox == 0)
			{
				LOG_ERROR("Failed to find Player in the map. ");
				
			}
			else
			{

				dtDAL::TransformableActorProxy* transproxy = dynamic_cast<dtDAL::TransformableActorProxy*>(playerprox);	
				mPlayerActor = dynamic_cast<dtCore::Transformable*>(transproxy->GetActor());
				
				
				printPlayerLocation();
				
			}
			
		   // Motion model to walk around the scene.			
			mHumanMotionModel = new HumanMotionModel(GetGameManager()->GetApplication().GetKeyboard(),
												   GetGameManager()->GetApplication().GetMouse());
			mHumanMotionModel->SetTarget(mPlayerActor);
			mHumanMotionModel->SetScene(&GetGameManager()->GetScene());
			mHumanMotionModel->SetEnabled(false);

			// create the fly motion model
		
			mFlyMM = new dtCore::FlyMotionModel(GetGameManager()->GetApplication().GetKeyboard(), 
																	GetGameManager()->GetApplication().GetMouse(), 
																	dtCore::FlyMotionModel::OPTION_USE_CURSOR_KEYS | 
																	dtCore::FlyMotionModel::OPTION_REQUIRE_MOUSE_DOWN);
			
			mFlyMM->SetTarget(mPlayerActor);

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
			cam.SetPerspectiveParams(vfov, aspectRatio, .1, 40000);

			mPlayerActor->AddChild(&cam);

			teleportToPlayerStart();
			
			  
	    } 
   
}

void OFVInputComponent::printPlayerLocation()
{
	dtCore::Transform xform;
	mPlayerActor->GetTransform(xform);
	osg::Vec3 trans = xform.GetTranslation();
	std::cout << "Player location " << trans.x() << "," << trans.y() << "," << trans.z() << "\n";

}
void OFVInputComponent::teleportToPlayerStart()
{
	std::vector<dtDAL::ActorProxy*> actors;
	const dtDAL::ActorType *type = GetGameManager()->FindActorType("dtcore", "Player Start");
	GetGameManager()->FindActorsByType(*type, actors);
		RefPtr<dtDAL::TransformableActorProxy> proxy = NULL;
	if (actors.empty())
	{
		LOG_WARNING("Failed to find a player start proxy in the map. Defaulting to [0, 0, 0]");
	}
	else
	{					
		 proxy  = dynamic_cast<dtDAL::TransformableActorProxy*>(actors[0]);
		osg::Vec3 startPos = proxy->GetTranslation();
		osg::Vec3 startRot = proxy->GetRotation();
		dtCore::Transform stealthStart;
		//stealthStart.SetTranslation(startPos.x() * 2.0, startPos.y() * 2.0, startPos.z() * 2.0 );
		stealthStart.SetTranslation(startPos.x(), startPos.y(), startPos.z());
		stealthStart.SetRotation(startRot);
		std::cout << "setting player location to " << startPos.x() << "," << startPos.y() << "," << startPos.z() << "\n";
		mPlayerActor->SetTransform(stealthStart);
	}
}
void OFVInputComponent::switchMotionModels()
{
	if(mFlyMM->IsEnabled())
	{
		mFlyMM->SetEnabled(false);
		mHumanMotionModel->SetEnabled(true);
	}
	else
	{
		mFlyMM->SetEnabled(true);
		mHumanMotionModel->SetEnabled(false);
	}

}
void OFVInputComponent::multiplyMotionModelSpeed(float factor)
{
	
	if(mFlyMM == NULL || mHumanMotionModel == NULL)
	{
		LOG_ERROR("requested MotionModel which NULL (not yet ready.)");
		return;
	}
	if(mFlyMM->IsEnabled())
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
		
		val = mHumanMotionModel->GetMaximumTurnSpeed();
		mHumanMotionModel->SetMaximumTurnSpeed(val * factor);

		val = mHumanMotionModel->GetMaximumSidestepSpeed();
		mHumanMotionModel->SetMaximumSidestepSpeed(val * factor);
	}
	
}
/////////////////////////////////////////////////////////////////////////////
	bool OFVInputComponent::HandleKeyPressed(const dtCore::Keyboard* keyboard, int key)
	{
		
		bool handled = true;
		switch(key)
		{
			case '=':
			case '+':
				multiplyMotionModelSpeed(2.0);
				break;
			case '-':
			case '_':
				multiplyMotionModelSpeed(0.5);
				break;
			case 'm':
			case 'M':
				switchMotionModels();
				break;
			case 'b':
			case 'B':
				teleportToPlayerStart();
				mFlyMM->SetEnabled(true);
				mHumanMotionModel->SetEnabled(false);
				break;
			case osgGA::GUIEventAdapter::KEY_Space:	
				mHumanMotionModel->toggleCollision();
				break;	
			case 'z':
				printPlayerLocation();
				break; 

			default:
				// Implemented to get rid of warnings in Linux
				handled = false;
				break;
		}

		

		return handled;
	}