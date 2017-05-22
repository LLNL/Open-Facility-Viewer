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
// OFV - FlyLimitMotionModel
// 
// portions taken from dtCore::FlyMotionModel in 
// Delta3d Open Source Game and Simulation Engine
// Copyright (C) 2004-2005 MOVES Institute
// under the terms of the GNU Lesser General Public License 2.1 or later
//
//---------------------------------------------------------------------------//

#include <iostream>

#include <prefix/dtcoreprefix.h>
#include <OFV/GMApp/FlyLimitMotionModel.h>

#include <dtCore/keyboard.h>
#include <dtCore/inputdevice.h>
#include <dtCore/logicalinputdevice.h>
#include <dtCore/mouse.h>
#include <dtCore/scene.h>
#include <dtCore/transform.h>
#include <dtCore/isector.h>
#include <dtCore/system.h>
#include <dtCore/transformable.h>
#include <osg/Vec3>
#include <osg/Matrix>




#include <dtUtil/bits.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/log.h>

#include <dtUtil/matrixutil.h>

#include <osg/Vec3>


IMPLEMENT_MANAGEMENT_LAYER(FlyLimitMotionModel)

/**
* Constructor.
*
* @param minimumXYZ min limit of extents where one can fly to.
* @param maximumXYZ max limit of extents where one can fly to.
* @param keyboard the keyboard instance, or 0 to
* avoid creating default input mappings
* @param mouse the mouse instance, or 0 to avoid
* creating default input mappings
*/
FlyLimitMotionModel::FlyLimitMotionModel(osg::Vec3 minimumXYZ,
	osg::Vec3 maximumXYZ,
	dtCore::Keyboard* keyboard,
	dtCore::Mouse* mouse)
	: FlyMotionModel(keyboard,
		mouse,
		dtCore::FlyMotionModel::OPTION_USE_CURSOR_KEYS |
		dtCore::FlyMotionModel::OPTION_REQUIRE_MOUSE_DOWN),
	mMinimumXYZ(minimumXYZ),
	mMaximumXYZ(maximumXYZ)
{


}

/**
* Destructor.
*/
FlyLimitMotionModel::~FlyLimitMotionModel()
{

}
void FlyLimitMotionModel::OnMessage(MessageData* data)
{
	if (GetTarget() != NULL &&
		IsEnabled() &&
		(data->message == dtCore::System::MESSAGE_POST_EVENT_TRAVERSAL/*MESSAGE_PRE_FRAME*/) &&
		// don't move if paused & using simtime for speed (since simtime will be 0 if paused)
		(!HasOption(OPTION_USE_SIMTIME_FOR_SPEED) || !dtCore::System::GetInstance().GetPause()))
	{
		// Get the time change (sim time or real time)
		double delta = GetTimeDelta(data);

		dtCore::Transform transform;

		GetTarget()->GetTransform(transform);

		osg::Vec3 xyz, hpr;

		transform.Get(xyz, hpr);

		// rotation

		bool rotationChanged = false;
		hpr = Rotate(hpr, delta, rotationChanged);
		if (rotationChanged)
		{
			transform.SetRotation(hpr);
		}

		// translation

		bool translationChanged = false;
		xyz = Translate(xyz, delta, translationChanged);

		if (translationChanged)
		{
			if (insideLimits(xyz))
				transform.SetTranslation(xyz);
		}

		// finalize changes

		if (rotationChanged || translationChanged)
		{
			GetTarget()->SetTransform(transform);
		}
	}
}

bool FlyLimitMotionModel::insideLimits(const osg::Vec3 &xyz)
{
	//std::cout << "xyz " << xyz.x() << "," << xyz.y() << "," << xyz.z() << "\n";
	if (xyz.x() > mMinimumXYZ.x() &&
		xyz.y() > mMinimumXYZ.y() &&
		xyz.z() > mMinimumXYZ.z() &&
		xyz.x() < mMaximumXYZ.x() &&
		xyz.y() < mMaximumXYZ.y() &&
		xyz.z() < mMaximumXYZ.z())
	{
		return true;
	}
	return false;
}