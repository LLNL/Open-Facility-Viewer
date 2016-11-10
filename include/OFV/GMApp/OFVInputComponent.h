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

#ifndef OFV_INPUT_COMPONENT_H_
#define OFV_INPUT_COMPONENT_H_



#include <OFV/GMApp/HumanMotionModel.h>
#include <OFV/GMApp/Export.h>
#include <dtGame/baseinputcomponent.h>




namespace dtCore
{
  class DeltaDrawable;
  class FlyMotionModel;
}

namespace dtGame
{
   class Message;
   class LogKeyframe;
}






   class OFV_GAME_EXPORT OFVInputComponent : public dtGame::BaseInputComponent
   {
   public:

     /// Constructor
     OFVInputComponent();

     virtual void ProcessMessage(const dtGame::Message &message);

     //virtual bool HandleButtonPressed( const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button );
     //virtual bool HandleButtonReleased( const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button );
     virtual bool HandleKeyPressed(const dtCore::Keyboard* keyboard, int key);
     //virtual bool HandleKeyReleased(const dtCore::Keyboard* keyboard, int key);

     

   protected:
     /// Destructor
     virtual ~OFVInputComponent();
    

   private:
	 
     void teleportToPlayerStart();
	 void printPlayerLocation();
	 void switchMotionModels();
	 void multiplyMotionModelSpeed(float factor);
	 dtCore::RefPtr<HumanMotionModel> mHumanMotionModel;
     dtCore::RefPtr<dtCore::Transformable> mPlayerActor;
     dtCore::RefPtr<dtCore::FlyMotionModel> mFlyMM;


   };

#endif