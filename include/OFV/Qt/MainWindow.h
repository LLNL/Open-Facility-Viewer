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
// OFV - MainWindow
// 
// portions taken from 'testQt' in 
// Delta3d Open Source Game and Simulation Engine
// use 'The MIT License'
//
// For portions taken from 'testQt':
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
//---------------------------------------------------------------------------//

#ifndef MainWindow_h__
#define MainWindow_h__

#include <QtGui/QMainWindow>
#include "ui_MainWindow.h"
#include <dtCore/refptr.h>

class QFileInfo;
class ImageViewer;

class MainWindow : public QMainWindow
{
   Q_OBJECT
public:
	MainWindow(QMainWindow* parent = NULL);
	virtual ~MainWindow();

   void SetGraphicsWidget(QWidget* graphics);
   void closeEvent(QCloseEvent* e);

 public slots:
   void OnFileLoaded(bool valid);

signals:
   void LoadFile(const QString& filename);
   void toggleVisibility(const QString &actorName, bool visible);
   void setTeleportActive(const QString &actorName, bool active);
   
   void jumpTo(double x, double y, double z, double h, double p, double r);
   void appShutDown();

private slots:
   //auto-generated slots 
   void on_actionOpen_triggered();
   void on_actionAbout_triggered();
   void on_actionControls_triggered();
   void on_actionACB_Flow_Arrows_triggered();
   void on_actionBuilding_Signs_triggered();
   void on_actionSecurity_Signs_triggered();
   void on_actionPerimeter_Lighting_triggered();
	void on_actionInner_Protected_Area_Fence_triggered();
	void on_actionVehicle_Radiation_Portal_Monitors_triggered();
	void on_actionMicrowave_Sensors_triggered();
	void on_actionReactor_Building_triggered();
	void on_actionFuel_Fabrication_Building_triggered();
	void on_actionGuard_Towers_triggered();
 

	//JUMP TO LOCATIONS
	void on_action1_Shapash_Facility_Entrance_triggered();
	void on_action2_Storage_Bunker_triggered();
	void on_action3_Research_Reactor_triggered();
	void on_action4_Reactor_Hall_triggered();
	void on_action5_Fuel_Fabrication_Building_triggered();
	void on_action6_Fuel_Fabrication_Pin_Assembly_Area_triggered();
	void on_action7_Fuel_Fabrication_Building_Mezzanine_triggered();
	void on_action8_Fuel_Fabrication_Building_Basement_triggered();
	void on_action9_Access_Control_Building_triggered();
	void on_action10_Special_Response_Team_Area_triggered();
	void on_action11_Central_Alarm_Station_triggered();
	void on_action12_Scrapyard_triggered();
	void on_action13_Radioactive_Waste_Site_triggered();
	void on_actionNavigation_Map_triggered();

	//MATERIAL FLOW IMAGES
	void on_actionWaste_triggered();
	void on_actionFreshFuel_triggered();
	void on_actionAllMaterials_triggered();
	void on_actionOxides_triggered();
	void on_actionTargets_triggered();


	

private:
  Ui::MainWindow ui; 

  QFileInfo* mLastMapInfo;
  ImageViewer* mImageViewerWaste;
  ImageViewer* mImageViewerOxides;
  ImageViewer* mImageViewerFreshFuel;
  ImageViewer* mImageViewerTargets;
  ImageViewer* mImageViewerAllMaterials;


};
#endif // MainWindow_h__
