//----------------------------------*-C++-*----------------------------------//
// Open Facility Viewer, an application for 3D walk-throughs of facilities
//
// Copyright (C) 2016, Lawrence Livermore National Security, LLC.
//
// Produced at the Lawrence Livermore National Laboratory.
// Prepared by LLNL under Contract DE-AC52-07NA27344.
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
// 
// OFV - HumanMotionModel
// 
// portions taken from dtCore::WalkMotionModel in 
// Delta3d Open Source Game and Simulation Engine
// Copyright (C) 2004-2005 MOVES Institute
// under the terms of the GNU Lesser General Public License 2.1 or later
//
//---------------------------------------------------------------------------//

#include <iostream>

#include <prefix/dtcoreprefix.h>
#include <OFV/GMApp/HumanMotionModel.h>

#include <dtCore/keyboard.h>
#include <dtCore/inputdevice.h>
#include <dtCore/logicalinputdevice.h>
#include <dtCore/mouse.h>
#include <dtCore/scene.h>
#include <dtCore/transform.h>
#include <dtCore/isector.h>
#include <dtCore/system.h>
#include <dtCore/transformable.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/matrixutil.h>

#include <osg/Vec3>


IMPLEMENT_MANAGEMENT_LAYER(HumanMotionModel)

/**
* Constructor.
*
* @param keyboard the keyboard instance, or 0 to
* avoid creating default input mappings
* @param mouse the mouse instance, or 0 to avoid
* creating default input mappings
*/
HumanMotionModel::HumanMotionModel(float pHeight,
	float pRadius,
	float stepUpHeight,
	dtCore::Scene* pScene,
	dtCore::Keyboard* keyboard,
	dtCore::Mouse* mouse)
	: MotionModel("HumanMotionModel")
	, mWalkForwardBackwardAxis(NULL)
	, mTurnLeftRightAxis(NULL)
	, mSidestepLeftRightAxis(NULL)
	, mMaximumWalkSpeed(05.0f)
	, mMaximumTurnSpeed(60.0f)
	, mMaximumSidestepSpeed(1.0f)
	, mHeightAboveTerrain(1.4f)
	, mMaximumStepUpDistance(0.1f)
	, mDownwardSpeed(0.0f)
	, mIsector(NULL)
	, mCollisionOn(true)
	, mMinimumHeight(1.5)
	, mLastLookLeftRight(-100000.0)
	, mLastLookUpDown(-100000.0)
	, mMinLookUpDown(-30.0)
	, mMaxLookUpDown(30.0)
	, mCollider(pHeight, pRadius, stepUpHeight, 0.0f, pScene)
{
	RegisterInstance(this);



	if (keyboard != NULL && mouse != NULL)
	{
		SetDefaultMappings(keyboard, mouse);
	}

	AddSender(&dtCore::System::GetInstance());

	mCollider.SetCollisionBitsForFeet(0x00000020);
	mCollider.SetCollisionBitsForTorso(0x00000020);
	mCollider.SetSlideThreshold(0.1);
	mCollider.SetSlideSpeed(5.0); 
	mCollider.SetJumpSpeed(0.0);
	//mCollider.SetSmoothingSpeed(20.0);

	mMouse = mouse;
	mKeyboard = keyboard;


}

/**
* Destructor.
*/
HumanMotionModel::~HumanMotionModel()
{
	RemoveSender(&dtCore::System::GetInstance());

	DeregisterInstance(this);
}

/**
* Sets the active Scene, which is used for ground following.
*
* @param scene the active scene
*/
void HumanMotionModel::SetScene(dtCore::Scene* scene)
{
	mScene = scene;
}

/**
* Returns the active Scene.
*
* @return the active Scene
*/
dtCore::Scene* HumanMotionModel::GetScene()
{
	return mScene.get();
}

/**
* Sets the input axes to a set of default mappings for mouse
* and keyboard.
*
* @param keyboard the keyboard instance
* @param mouse the mouse instance
*/

void HumanMotionModel::toggleCollision()
{
	mCollisionOn = !mCollisionOn;
}
unsigned long HumanMotionModel::getCollsionBits()
{
	return mCollider.GetCollisionBitsForFeet();
}
void HumanMotionModel::setCollsionBits(unsigned long bits)
{
	mCollider.SetCollisionBitsForFeet(bits);
	mCollider.SetCollisionBitsForTorso(bits);
}
void HumanMotionModel::SetDefaultMappings(dtCore::Keyboard* keyboard, dtCore::Mouse* mouse)
{
	if (mDefaultInputDevice.get() == 0)
	{
		mDefaultInputDevice = new dtCore::LogicalInputDevice;

		//no button required.
		//dtCore::Axis* leftButtonUpAndDown = mDefaultInputDevice->AddAxis("up/down", mouse->GetAxis(1));

		//dtCore::Axis* leftButtonLeftAndRight = mDefaultInputDevice->AddAxis("left/rigth", mouse->GetAxis(0));

		dtCore::Axis* leftButtonUpAndDown = mDefaultInputDevice->AddAxis(
			"left mouse button up/down",
			mLeftButtonUpDownMapping = new dtCore::ButtonAxisToAxis(
				mouse->GetButton(dtCore::Mouse::LeftButton),
				mouse->GetAxis(1)
			)
		);

		dtCore::Axis* leftButtonLeftAndRight = mDefaultInputDevice->AddAxis(
			"left mouse button left/right",
			mLeftButtonLeftRightMapping = new dtCore::ButtonAxisToAxis(
				mouse->GetButton(dtCore::Mouse::LeftButton),
				mouse->GetAxis(0)
			)
		);


		dtCore::Axis* pageKeysUpAndDown = mDefaultInputDevice->AddAxis(
			"page keys up/down",
			mPageKeysUpDownMapping = new dtCore::ButtonsToAxis(
				keyboard->GetButton('n'),
				keyboard->GetButton('u')
			)
		);

		dtCore::Axis* arrowKeysUpAndDown = mDefaultInputDevice->AddAxis(
			"arrow keys up/down",
			mArrowKeysUpDownMapping = new dtCore::ButtonsToAxis(
				keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Down),
				keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Up)
			)
		);

		dtCore::Axis* arrowKeysLeftAndRight = mDefaultInputDevice->AddAxis(
			"arrow keys left/right",
			mArrowKeysLeftRightMapping = new dtCore::ButtonsToAxis(
				keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Left),
				keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Right)
			)
		);

		dtCore::Axis* adKeysLeftAndRight = mDefaultInputDevice->AddAxis(
			"a/d keys left/right",
			mADKeysLeftRightMapping = new dtCore::ButtonsToAxis(
				keyboard->GetButton('a'),
				keyboard->GetButton('d')
			)
		);
		dtCore::Axis* wsKeysUpAndDown = mDefaultInputDevice->AddAxis(
			"w/s keys up/down",
			mWSKeysUpDownMapping = new dtCore::ButtonsToAxis(
				keyboard->GetButton('s'),
				keyboard->GetButton('w')
			)
		);
		/*	dtCore::Axis* wsKeysUpAndDownCAPS = mDefaultInputDevice->AddAxis(
		"w/s keys up/down",
		mWSKeysUpDownMappingCAPS = new dtCore::ButtonsToAxis(
		keyboard->GetButton('S'),
		keyboard->GetButton('W')
		)
		);
		*/

		mDefaultWalkForwardBackwardAxis = mDefaultInputDevice->AddAxis(
			"default walk forward/backward",
			new dtCore::AxesToAxis(wsKeysUpAndDown, arrowKeysUpAndDown)
		);

		mDefaultTurnLeftRightAxis = mDefaultInputDevice->AddAxis(
			"default turn left/right",
			new dtCore::AxesToAxis(leftButtonLeftAndRight, arrowKeysLeftAndRight)
		);
		mDefaultLookUpDownAxis = mDefaultInputDevice->AddAxis(
			"default look up/down",
			new dtCore::AxesToAxis(leftButtonUpAndDown, pageKeysUpAndDown)
		);
		mDefaultSidestepLeftRightAxis = mDefaultInputDevice->AddAxis(
			"default sidestep left/right",
			new dtCore::AxesToAxis(adKeysLeftAndRight)
		);


	}
	else
	{
		mLeftButtonUpDownMapping->SetSourceButton(mouse->GetButton(dtCore::Mouse::LeftButton));
		mLeftButtonUpDownMapping->SetSourceAxis(mouse->GetAxis(1));

		mLeftButtonLeftRightMapping->SetSourceButton(mouse->GetButton(dtCore::Mouse::LeftButton));
		mLeftButtonLeftRightMapping->SetSourceAxis(mouse->GetAxis(0));
		
		mPageKeysUpDownMapping->SetSourceButtons(
			keyboard->GetButton('n'),
			keyboard->GetButton('u')
		); 

		mArrowKeysUpDownMapping->SetSourceButtons(
			keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Down),
			keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Up)
		);

		mArrowKeysLeftRightMapping->SetSourceButtons(
			keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Left),
			keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Right)
		);

		mADKeysLeftRightMapping->SetSourceButtons(
			keyboard->GetButton('a'),
			keyboard->GetButton('d')
		);
		mWSKeysUpDownMapping->SetSourceButtons(
			keyboard->GetButton('s'),
			keyboard->GetButton('w')
		);

		/*  mWSKeysUpDownMappingCAPS->SetSourceButtons(
		keyboard->GetButton('S'),
		keyboard->GetButton('W')
		);
		*/
	}
	
	SetWalkForwardBackwardAxis(mDefaultWalkForwardBackwardAxis);

	SetTurnLeftRightAxis(mDefaultTurnLeftRightAxis);

	SetLookUpDownAxis(mDefaultLookUpDownAxis);

	SetSidestepLeftRightAxis(mDefaultSidestepLeftRightAxis);
}

/**
* Sets the axis that moves the target forwards (for positive
* values) or backwards (for negative values).
*
* @param walkForwardBackwardAxis the new forward/backward axis
*/
void HumanMotionModel::SetWalkForwardBackwardAxis(dtCore::Axis* walkForwardBackwardAxis)
{
	mWalkForwardBackwardAxis = walkForwardBackwardAxis;
}

/**
* Returns the axis that moves the target forwards (for positive
* values) or backwards (for negative values).
*
* @return the current forward/backward axis
*/
dtCore::Axis* HumanMotionModel::GetWalkForwardBackwardAxis()
{
	return mWalkForwardBackwardAxis;
}

/**
* Sets the axis that turns the target left (for negative values)
* or right (for positive values).
*
* @param turnLeftRightAxis the new turn left/right axis
*/
void HumanMotionModel::SetTurnLeftRightAxis(dtCore::Axis* turnLeftRightAxis)
{
	mTurnLeftRightAxis = turnLeftRightAxis;
}

/**
* Returns the axis that turns the target left (for negative values)
* or right (for positive values).
*
* @return the current turn left/right axis
*/
dtCore::Axis* HumanMotionModel::GetTurnLeftRightAxis()
{
	return mTurnLeftRightAxis;
}
/**
* Sets the axis that turns the target up (for negative values)
* or down(for positive values).
*
* @param turnLeftRightAxis the new turn left/right axis
*/
void HumanMotionModel::SetLookUpDownAxis(dtCore::Axis* lookUpDownAxis)
{
	mLookUpDownAxis = lookUpDownAxis;
}

/**
* Returns the axis that turns the target up (for negative values)
* or down (for positive values).
*
* @return the current turn up/down axis
*/
dtCore::Axis* HumanMotionModel::GetLookUpDownAxis()
{
	return mLookUpDownAxis;
}
/**
* Sets the axis that sidesteps the target left (for negative values)
* or right (for positive values).
*
* @param sidestepLeftRightAxis the new sidestep left/right axis
*/
void HumanMotionModel::SetSidestepLeftRightAxis(dtCore::Axis* sidestepLeftRightAxis)
{
	mSidestepLeftRightAxis = sidestepLeftRightAxis;
}

/**
* Returns the axis that sidesteps the target left (for negative values)
* or right (for positive values).
*
* @return the current sidestep left/right axis
*/
dtCore::Axis* HumanMotionModel::GetSidestepLeftRightAxis()
{
	return mSidestepLeftRightAxis;
}

/**
* Sets the maximum walk speed (meters per second).
*
* @param maximumWalkSpeed the new maximum walk speed
*/
void HumanMotionModel::SetMaximumWalkSpeed(float maximumWalkSpeed)
{
	mMaximumWalkSpeed = maximumWalkSpeed;
}

/**
* Returns the maximum walk speed (meters per second).
*
* @return the current maximum walk speed
*/
float HumanMotionModel::GetMaximumWalkSpeed()
{
	return mMaximumWalkSpeed;
}

/**
* Sets the maximum turn speed (degrees per second).
*
* @param maximumTurnSpeed the new maximum turn speed
*/
void HumanMotionModel::SetMaximumTurnSpeed(float maximumTurnSpeed)
{
	mMaximumTurnSpeed = maximumTurnSpeed;
}

/**
* Returns the maximum turn speed (degrees per second).
*
* @return the current maximum turn speed
*/
float HumanMotionModel::GetMaximumTurnSpeed()
{
	return mMaximumTurnSpeed;
}

/**
* Sets the maximum sidestep speed (meters per second).
*
* @param maximumSidestepSpeed the new maximum sidestep speed
*/
void HumanMotionModel::SetMaximumSidestepSpeed(float maximumSidestepSpeed)
{
	mMaximumSidestepSpeed = maximumSidestepSpeed;
}

/**
* Returns the maximum sidestep speed (meters per second).
*
* @return the current maximum sidestep speed
*/
float HumanMotionModel::GetMaximumSidestepSpeed()
{
	return mMaximumSidestepSpeed;
}

/**
* Sets the height to maintain above the terrain (meters).
*
* @param heightAboveTerrain the height to maintain above the
* terrain
*/
void HumanMotionModel::SetHeightAboveTerrain(float heightAboveTerrain)
{
	mHeightAboveTerrain = heightAboveTerrain;
}

/**
* Returns the height to maintain above the terrain (meters).
*
* @return the height to maintain above the terrain
*/
float HumanMotionModel::GetHeightAboveTerrain()
{
	return mHeightAboveTerrain;
}

/**
* Sets the maximum step-up distance.  When clamping to the ground, the
* maximum step-up distance determines whether to rise to a new level
* (as when the model climbs a staircase) or to stay at the current level
* (as when the model passes under a roof).  The default is 1.0.
*
* @param maximumStepUpDistance the new maximum step-up distance
*/
void HumanMotionModel::SetMaximumStepUpDistance(float maximumStepUpDistance)
{
	mMaximumStepUpDistance = maximumStepUpDistance;
}

/**
* Returns the current maximum step-up distance.
*
* @return the maximum step-up distance
*/
float HumanMotionModel::GetMaximumStepUpDistance()
{
	return mMaximumStepUpDistance;
}


/**
* Message handler callback.
*
* @param data the message data
*/
void HumanMotionModel::OnMessage(dtCore::Base::MessageData* data)
{
	float maxMouseTurnLookSpeed = 15.0;

	if (GetTarget() != 0 &&
		IsEnabled() &&
		data->message == dtCore::System::MESSAGE_POST_EVENT_TRAVERSAL/*MESSAGE_PRE_FRAME*/)
	{	
		double deltaTime = *static_cast<double*>(data->userData);

		dtCore::Transform transform;


		GetTarget()->GetTransform(transform);


		osg::Vec3 xyz, hpr;

		transform.Get(xyz, hpr);

		//Use mouse only if Left button is down.
		double turnSpeedFactor = 30.0;
		if (mTurnLeftRightAxis != 0)
		{
			if (mMouse->GetButton(dtCore::Mouse::LeftButton)->GetState())
			{
				
					if (mLastLookLeftRight <= -99999.0)
					{
						mLastLookLeftRight = mTurnLeftRightAxis->GetState();
					}

				//if(fabs(mTurnLeftRightAxis->GetState()) > 0.3)
				
				double mDiffLR = mTurnLeftRightAxis->GetState() - mLastLookLeftRight;
				hpr[0] -= float((mDiffLR * turnSpeedFactor) * maxMouseTurnLookSpeed * deltaTime);
			}
			else
			{ 
				hpr[0] -= float(mTurnLeftRightAxis->GetState() * mMaximumTurnSpeed * deltaTime);
			}
		}
		

		
		hpr[2] = 0.0f;



		if (mLookUpDownAxis != 0)
		{
			if (mMouse->GetButton(dtCore::Mouse::LeftButton)->GetState())
			{
				if (mLastLookUpDown <= -99999.0) 
				{
					mLastLookUpDown = mLookUpDownAxis->GetState();
				}
				//if(fabs(mLookUpDownAxis->GetState()) > 0.3)
				double mDiffUD = mLookUpDownAxis->GetState() - mLastLookUpDown;
				hpr[1] += float((mDiffUD  * turnSpeedFactor) * maxMouseTurnLookSpeed  * deltaTime);
			}
			else
			{
				hpr[1] += float(mLookUpDownAxis->GetState() * mMaximumTurnSpeed * deltaTime);
			}

			if (hpr[1] < mMinLookUpDown)
			{
				hpr[1] = mMinLookUpDown;
			}
			else if (hpr[1] > mMaxLookUpDown)
			{
				hpr[1] = mMaxLookUpDown;
			}
			//std::cout << "HPR   " << hpr[1] << "\n";
		}
		mLastLookUpDown = mLookUpDownAxis->GetState();
		mLastLookLeftRight = mTurnLeftRightAxis->GetState();


		transform.SetRotation(hpr);
		 
		

		if (mCollisionOn)
		{
			GetTarget()->SetTransform(transform);
			PerformTranslation(deltaTime, mSidestepLeftRightAxis->GetState(), mWalkForwardBackwardAxis->GetState());
		}
		else
		{

			osg::Vec3 translation(0, 0, 0);

			if (mWalkForwardBackwardAxis != 0)
			{
				translation[1] = float(mWalkForwardBackwardAxis->GetState() * mMaximumWalkSpeed * deltaTime);
			}

			if (mSidestepLeftRightAxis != 0)
			{
				translation[0] = float(mSidestepLeftRightAxis->GetState() * mMaximumSidestepSpeed * deltaTime);
			}

			osg::Matrix mat;
		 
			transform.GetRotation(mat);

			//sgXformVec3(translation, mat);
			translation = osg::Matrix::transform3x3(translation, mat);

			//sgAddVec3(xyz, translation);
			translation[2] = 0.0; //prevent look up/down direction from influncing translation
			xyz += translation;

			if (mScene.get() != 0)
			{
				float height = 0.0f;

				const osg::Vec3 start(
					xyz[0],
					xyz[1],
					xyz[2] + mMaximumStepUpDistance - mHeightAboveTerrain
				);

				const osg::Vec3 end(
					xyz[0],
					xyz[1],
					xyz[2] - 10000.0f
				);


				// get the height of the terrain
				GetScene()->GetHeightOfTerrain(height, start.x(), start.y(), start.z(), end.z());

				height += mHeightAboveTerrain;

				if (xyz[2] <= height)
				{
					xyz[2] = height;

					mDownwardSpeed = 0.0f;
				}
				else if (xyz[2] > height) //FALLING
				{

					xyz[2] -= float(mDownwardSpeed * deltaTime);

					mDownwardSpeed += float(9.8 * deltaTime);
				}
			}

			 if(xyz[2] < mMinimumHeight)
			 {
				xyz[2] = mMinimumHeight;
			 }
			//std::cout << "h = " << xyz[2] << "\n";
			transform.SetTranslation(xyz);

			dtCore::Transform head(transform);
			osg::Vec3 headxyz = xyz;
			headxyz[2] += 0.5;
			head.SetTranslation(headxyz);
		
			float distance = distanceToObjectInFront(head, 200.0);
			//std::cout << "DISTANCE " << distance << "\n";

			if (distance < 0.25 && mCollisionOn)
			{
				//  std::cout << "Hit Wall\n";
			}
			else
			{
				GetTarget()->SetTransform(transform);
			}

			//if(fabs(mLookUpDownAxis->GetState()) > 0.5 || fabs(mTurnLeftRightAxis->GetState()) > 0.5)
		
			//Useful to reset mouse position if Left button is not required.
			//ResetMousePosition();
		}
	}
}

void HumanMotionModel::PerformTranslation(const double deltaTime, float sideStepFactor, float forwardBackFactor)
{
	dtCore::Transform transform;
	osg::Vec3 currentXYZ, hpr;

	// query initial status (to change from)
	GetTarget()->GetTransform(transform);
	transform.GetTranslation(currentXYZ);
	transform.GetRotation(hpr);

	// calculate x/y delta
	osg::Vec3 velocity(0.0f, 0.0f, 0.0f);
	velocity[0] = sideStepFactor    * GetMaximumSidestepSpeed();
	//velocity[0] = 0.25f * GetMaximumSidestepSpeed();
	velocity[1] = forwardBackFactor * GetMaximumWalkSpeed();

	// Clamp the velocity so you cannot go beyond our maximum speed.
	float maxSpeed = dtUtil::Max<float>(GetMaximumSidestepSpeed(), GetMaximumWalkSpeed());
	float curSpeed = velocity.length();
	if (curSpeed > maxSpeed && maxSpeed > 0.0f)
	{
		float mul = maxSpeed / curSpeed;
		velocity *= mul;
	}

	// transform our x/y delta by our new heading
	osg::Matrix mat;
	const float heading = hpr[0];
	mat.makeRotate(osg::DegreesToRadians(heading), osg::Vec3(0.0f, 0.0f, 1.0f));
	velocity = velocity * mat;

	// perform integration step, physically constraining translation path to environment
	
	osg::Vec3 last = currentXYZ;
	bool jump = false;
	currentXYZ = mCollider.Update(currentXYZ, velocity, deltaTime, jump);

	// apply changes (new position)
	transform.SetTranslation(currentXYZ);
	GetTarget()->SetTransform(transform);
}

void HumanMotionModel::ResetMousePosition()
{
	mMouse->SetPosition(0.0f, 0.0f); // keeps cursor at center of screen
	mLastLookLeftRight = 0.0;
	mLastLookUpDown = 0.0;
	mLookUpDownAxis->SetState(0.0);
	mTurnLeftRightAxis->SetState(0.0);
}
float HumanMotionModel::distanceToObjectInFront(const dtCore::Transform &trans, float distance)
{

	if (mIsector == NULL && GetScene() != NULL) {


		mIsector = new dtCore::Isector();

		mIsector->SetGeometry(GetScene());


	}


	//Find start and end point of intersector which is our eye vector.

	osg::Matrix mat;

	osg::Vec3 start, end, forward;

	trans.Get(mat);
	forward = dtUtil::MatrixUtil::GetRow3(mat, 1);
	trans.GetTranslation(start);
	trans.GetTranslation(end);

	start += forward * 0.1; //don't want to intersect ourselves.

	float distanceToTarget = 9999.0;
	mIsector->SetStartPosition(start); //start at our ourselves.
	mIsector->SetDirection(forward); //forward
	mIsector->SetLength(distance);
	if (mIsector->Update())
	{
		if (mIsector->GetHitList().size() > 1)
		{


			osg::Vec3 myloc;
			trans.GetTranslation(myloc);

			for (int i = 0; i < mIsector->GetHitList().size(); i++)
			{
				osg::Vec3 objectloc = mIsector->GetHitList()[i].getWorldIntersectPoint();
				float dist = (objectloc - myloc).length();
				if (dist < distanceToTarget)
					distanceToTarget = dist;
			}

		}

	}

	return distanceToTarget; //big number here is better than 0 or -1;

}