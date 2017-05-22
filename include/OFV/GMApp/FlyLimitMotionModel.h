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

#ifndef OFV_FLYLIMITMOTIONMODEL
#define OFV_FLYLIMITMOTIONMODEL



//////////////////////////////////////////////////////////////////////
#include <dtCore/base.h>
#include <dtCore/flymotionmodel.h>

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
/**
* A motion model Limits the area where you can fly.
*/
class FlyLimitMotionModel : public dtCore::FlyMotionModel
{
	DECLARE_MANAGEMENT_LAYER(FlyLimitMotionModel)

public:

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
	FlyLimitMotionModel(osg::Vec3 minimumXYZ,
		osg::Vec3 maximumXYZ,
		dtCore::Keyboard* keyboard = NULL,
		dtCore::Mouse* mouse = NULL);

	/**
	* Message handler callback.
	*
	* @param data the message data
	*/
	virtual void OnMessage(MessageData* data);

protected:

	/**
	* Destructor.
	*/
	virtual ~FlyLimitMotionModel();



private:
	bool insideLimits(const osg::Vec3 &xyz);
	osg::Vec3 mMinimumXYZ;
	osg::Vec3 mMaximumXYZ;

};


#endif // OFV_FLYLMITMOTIONMODEL
