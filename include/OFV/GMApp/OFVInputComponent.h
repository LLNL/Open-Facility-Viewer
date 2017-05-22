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
#include <QtCore/QMap>
#include <QtCore/QSize>
class FlyLimitMotionModel;
class QProgressDialog;

namespace dtCore
{
	class DeltaDrawable;
	class CollisionMotionModel;
}

namespace dtGame
{
	class Message;
	class LogKeyframe;
}

namespace osg
{
	class Texture2D;
	class Geode;
	class PositionAttitudeTransform;
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
	void setActorVisible(std::string actorName, bool visible);
	dtCore::DeltaDrawable* getActorDrawable(std::string actorName);
	dtCore::Transformable* getActorTransformable(std::string actorName);
	void setFOV(double diff);
	void jumpTo(double x, double y, double z, double h, double p, double r);
	
protected:
	/// Destructor
	virtual ~OFVInputComponent();


private:
	osg::Geode* loadOSGImage(std::string textureFile, osg::Vec2 textureCoordinates, osg::Vec2 textureSize);
	osg::Texture2D* loadTexture(const std::string& path);
	void applyTextureOffset(osg::Geode* geode, osg::Vec2 textureOffset, osg::Vec2 GeomSize, osg::Vec2 imageSize);

	void teleportToPlayerStart();
	void printPlayerLocation();
	void createMiniMap();
	void createLogo();
	void createHUD();
	void updateMap();
	void checkModelVisibility();
	void checkWindowSizeChanged();
	void switchMotionModels();
	void multiplyMotionModelSpeed(float factor);
	void multiplyTurnModelSpeed(float factor);
	dtCore::RefPtr<HumanMotionModel> mHumanMotionModel;
	dtCore::RefPtr<dtCore::Transformable> mPlayerActor;
	dtCore::RefPtr<FlyLimitMotionModel> mFlyMM;
	osg::ref_ptr<osg::Geode> mMapGeode;
	osg::ref_ptr<osg::Geode> mDot;
	
	osg::ref_ptr<osg::PositionAttitudeTransform> mLogotrans;
	osg::ref_ptr<osg::Camera> mHUD;
	osg::Vec2 mMapWindowSize;
	//dtCore::RefPtr<dtCore::CollisionMotionModel> mCollisionMotionModel;
	bool mInitalizedPlayer;
	bool mMapLoaded;
	int mTicks;
	QSize mLastWindowSize;
	QProgressDialog* mProgressDialog;

	QMap<QString, osg::Vec3> mModelRenderDistance;

	float mCorrectionX;
	float mCorrectionY;
	float mCorrectionSizeX;
	float mCorrectionSizeY;

	enum MapDebugLevel {MAP_X_DEBUG, MAP_Y_DEBUG, MAP_NO_DEBUG};
	MapDebugLevel mMapDebug;
};

#endif
