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
// 
// OFV - HumanMotionModel
// 
// portions taken from dtCore::WalkMotionModel in 
// Delta3d Open Source Game and Simulation Engine
// Copyright (C) 2004-2005 MOVES Institute
// under the terms of the GNU Lesser General Public License 2.1 or later
//
//---------------------------------------------------------------------------//

#ifndef OFV_HUMANMOTIONMODEL
#define OFV_HUMANMOTIONMODEL



//////////////////////////////////////////////////////////////////////
#include <dtCore/base.h>
#include <dtCore/motionmodel.h>
#include <dtCore/fpscollider.h>

namespace dtCore
{
	//forward declaration
	class Scene;
	class Keyboard;
	class Mouse;
	class LogicalInputDevice;
	class ButtonAxisToAxis;
	class Axis;
	class ButtonsToAxis;
	class LogicalAxis;
	class Isector;
}

enum
{
	COLLISION_CATEGORY_BIT_FAC_05_ON =		0x00020000,
	COLLISION_CATEGORY_BIT_FAC_05_OFF =		0x00010000,
	COLLISION_CATEGORY_BIT_FAC_06_ON =		0x00080000,
	COLLISION_CATEGORY_BIT_FAC_06_OFF =		0x00040000,
	COLLISION_CATEGORY_BIT_FENCE_INNER =	0x00100000,
	COLLISION_CATEGORY_BIT_GUARDTOWERS =	0x00400000,
	COLLISION_CATEGORY_BIT_OBJECT =			0x00000020

};

/**
* A motion model that simulates the action of walking or driving.
*/
class HumanMotionModel : public dtCore::MotionModel
{
	DECLARE_MANAGEMENT_LAYER(HumanMotionModel)

public:

	/**
	* Constructor.
	*
	* @param keyboard the keyboard instance, or 0 to
	* avoid creating default input mappings
	* @param mouse the mouse instance, or 0 to avoid
	* creating default input mappings
	*/
	HumanMotionModel(float pHeight,
		float pRadius,
		float stepUpHeight,
		dtCore::Scene* pScene,
		dtCore::Keyboard* keyboard = NULL,
		dtCore::Mouse* mouse = NULL);

protected:

	/**
	* Destructor.
	*/
	virtual ~HumanMotionModel();

public:

	/**
	* Sets the active Scene, which is used for ground following.
	*
	* @param scene the active scene
	*/
	void SetScene(dtCore::Scene* scene);

	/**
	* Returns the active Scene.
	*
	* @return the active Scene
	*/
	dtCore::Scene* GetScene();

	void toggleCollision();
	void setCollsionBits(unsigned long bits);
	unsigned long getCollsionBits();

	inline bool collisionEnabled() { return mCollisionOn; };
	/**
	* Sets the input axes to a set of default mappings for mouse
	* and keyboard.
	*
	* @param keyboard the keyboard instance
	* @param mouse the mouse instance
	*/
	void SetDefaultMappings(dtCore::Keyboard* keyboard, dtCore::Mouse* mouse);

	/**
	* Sets the axis that moves the target forwards (for positive
	* values) or backwards (for negative values).
	*
	* @param walkForwardBackwardAxis the new forward/backward axis
	*/
	void SetWalkForwardBackwardAxis(dtCore::Axis* walkForwardBackwardAxis);

	/**
	* Returns the axis that moves the target forwards (for positive
	* values) or backwards (for negative values).
	*
	* @return the current forward/backward axis
	*/
	dtCore::Axis* GetWalkForwardBackwardAxis();

	/**
	* Sets the axis that turns the target left (for negative values)
	* or right (for positive values).
	*
	* @param turnLeftRightAxis the new turn left/right axis
	*/
	void SetTurnLeftRightAxis(dtCore::Axis* turnLeftRightAxis);

	/**
	* Returns the axis that turns the target left (for negative values)
	* or right (for positive values).
	*
	* @return the current turn left/right axis
	*/
	dtCore::Axis* GetTurnLeftRightAxis();

	/**
	* Sets the axis that turns the target up (for negative values)
	* or down (for positive values).
	*
	* @param lookUpDownAxis the new turn up/down axis
	*/
	void SetLookUpDownAxis(dtCore::Axis* lookUpDownAxis);

	/**
	* Returns the axis that turns the target up (for negative values)
	* or down (for positive values).
	*
	* @return the current turn up/down axis
	*/
	dtCore::Axis* GetLookUpDownAxis();

	/**
	* Sets the axis that sidesteps the target left (for negative values)
	* or right (for positive values).
	*
	* @param sidestepLeftRightAxis the new sidestep left/right axis
	*/
	void SetSidestepLeftRightAxis(dtCore::Axis* sidestepLeftRightAxis);

	/**
	* Returns the axis that sidesteps the target left (for negative values)
	* or right (for positive values).
	*
	* @return the current sidestep left/right axis
	*/
	dtCore::Axis* GetSidestepLeftRightAxis();

	/**
	* Sets the maximum walk speed (meters per second).
	*
	* @param maximumWalkSpeed the new maximum walk speed
	*/
	void SetMaximumWalkSpeed(float maximumWalkSpeed);

	/**
	* Returns the maximum walk speed (meters per second).
	*
	* @return the current maximum walk speed
	*/
	float GetMaximumWalkSpeed();

	/**
	* Sets the maximum turn speed (degrees per second).
	*
	* @param maximumTurnSpeed the new maximum turn speed
	*/
	void SetMaximumTurnSpeed(float maximumTurnSpeed);

	/**
	* Returns the maximum turn speed (degrees per second).
	*
	* @return the current maximum turn speed
	*/
	float GetMaximumTurnSpeed();

	/**
	* Sets the maximum sidestep speed (meters per second).
	*
	* @param maximumSidestepSpeed the new maximum sidestep speed
	*/
	void SetMaximumSidestepSpeed(float maximumSidestepSpeed);

	/**
	* Returns the maximum sidestep speed (meters per second).
	*
	* @return the current maximum sidestep speed
	*/
	float GetMaximumSidestepSpeed();

	/**
	* Sets the height to maintain above the terrain (meters).
	*
	* @param heightAboveTerrain the height to maintain above the
	* terrain
	*/
	void SetHeightAboveTerrain(float heightAboveTerrain);

	/**
	* Returns the height to maintain above the terrain (meters).
	*
	* @return the height to maintain above the terrain
	*/
	float GetHeightAboveTerrain();

	/**
	* Sets the maximum step-up distance.  When clamping to the ground, the
	* maximum step-up distance determines whether to rise to a new level
	* (as when the model climbs a staircase) or to stay at the current level
	* (as when the model passes under a roof).  The default is 1.0.
	*
	* @param maximumStepUpDistance the new maximum step-up distance
	*/
	void SetMaximumStepUpDistance(float maximumStepUpDistance);

	/**
	* Returns the current maximum step-up distance.
	*
	* @return the maximum step-up distance
	*/
	float GetMaximumStepUpDistance();

	/**
	* Message handler callback.
	*
	* @param data the message data
	*/
	virtual void OnMessage(dtCore::Base::MessageData *data);

	/**
	* get the distance to polygon in front of player position.
	*/
	float distanceToObjectInFront(const dtCore::Transform &trans, float distance);

	/**
	* Reposition mouse to 0,0 on the screen (center).
	*/
	void ResetMousePosition();

	void PerformTranslation(const double deltaTime, float sideStepFactor, float forwardBackFactor);
private:

	/**
	* A reference to the Scene, used for ground following.
	*/
	dtCore::RefPtr<dtCore::Scene> mScene;

	/**
	* The default input device.
	*/
	dtCore::RefPtr<dtCore::LogicalInputDevice> mDefaultInputDevice;

	/**
	* The left button up/down mapping.
	*/
	dtCore::ButtonAxisToAxis* mLeftButtonUpDownMapping;

	/**
	* The left button right/left mapping.
	*/
	dtCore::ButtonAxisToAxis* mLeftButtonLeftRightMapping;

	/**
	* The right button left/right mapping.
	*/
	dtCore::ButtonAxisToAxis* mRightButtonLeftRightMapping;

	

	/**
	* The page up/down key up/down mapping.
	*/
	dtCore::ButtonsToAxis* mPageKeysUpDownMapping;

	/**
	* The arrow key up/down mapping.
	*/
	dtCore::ButtonsToAxis* mArrowKeysUpDownMapping;

	/**
	* The arrow key left/right mapping.
	*/
	dtCore::ButtonsToAxis* mArrowKeysLeftRightMapping;

	/**
	* The a/d key left/right mapping.
	*/
	dtCore::ButtonsToAxis* mADKeysLeftRightMapping;

	/**
	* The w/s key up/down mapping.
	*/
	dtCore::ButtonsToAxis* mWSKeysUpDownMapping;

	/**
	* The w/s key up/down mapping. (in case caps lock is on.)
	*/
	dtCore::ButtonsToAxis* mWSKeysUpDownMappingCAPS;

	/**
	* The default walk forward/backward axis.
	*/
	dtCore::LogicalAxis* mDefaultWalkForwardBackwardAxis;

	/**
	* The default turn left/right axis.
	*/
	dtCore::LogicalAxis* mDefaultTurnLeftRightAxis;

	/**
	* The default turn left/right axis.
	*/
	dtCore::LogicalAxis* mDefaultLookUpDownAxis;

	/**
	* The default sidestep left/right axis.
	*/
	dtCore::LogicalAxis* mDefaultSidestepLeftRightAxis;

	/**
	* The axis that moves the target forwards or backwards.
	*/
	dtCore::Axis* mWalkForwardBackwardAxis;

	/**
	* The axis that turns the target to the left or right.
	*/
	dtCore::Axis* mTurnLeftRightAxis;


	/**
	* The axis that turns the target to the left or right.
	*/
	dtCore::Axis* mLookUpDownAxis;

	/**
	* The axis that sidesteps the target left or right.
	*/
	dtCore::Axis* mSidestepLeftRightAxis;

	/**
	* The maximum walk speed (meters per second).
	*/
	float mMaximumWalkSpeed;

	/**
	* The maximum turn speed (degrees per second).
	*/
	float mMaximumTurnSpeed;

	/**
	* The maximum sidestep speed (meters per second).
	*/
	float mMaximumSidestepSpeed;

	/**
	* The height to maintain above terrain (meters).
	*/
	float mHeightAboveTerrain;

	/**
	* The maximum step-up distance (meters).
	*/
	float mMaximumStepUpDistance;

	/**
	* The current downward speed.
	*/
	float mDownwardSpeed;

	/**
	* Intersector for collision tests.
	*/
	dtCore::Isector* mIsector;

	/**
	* Toggle collision with objects IN FRONT of you.
	*/
	bool mCollisionOn;

	/**
	* The lowest level allowed, to prevent falling through a crack.
	*/
	float mMinimumHeight;

	/**
	* Last turn on iteration (from axis->GetState()) so we can turn on mouse position differential.
	*/
	double mLastLookLeftRight;
	double mLastLookUpDown;

	/**
	* Limits to prevent axis fighting from looking completely vertical.
	*/
	double mMinLookUpDown;
	double mMaxLookUpDown;

	dtCore::FPSCollider mCollider;

	dtCore::RefPtr<dtCore::Mouse>    mMouse;
	dtCore::RefPtr<dtCore::Keyboard> mKeyboard;
};


#endif // OFV_HUMANMOTIONMODEL
