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

#include <OFV/Qt/MainWindow.h>
#include <dtUtil/datapathutils.h>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QKeyEvent>
#include <iostream> 

#define ABOUT_OFV "Open Facility Viewer, an application for 3D walk-throughs of facilities\r\n\
\r\n\
Version 1.6.4\
\r\n\
Copyright (C) 2016, Lawrence Livermore National Security, LLC.\r\n\
\r\n\
Produced at the Lawrence Livermore National Laboratory.\r\n\
\r\n\
All Rights Reserved.\r\n\
\r\n\
See LICENSE-OFV.txt file for complete license and terms."

#define HELP_OFV "Keyboard/Mouse controls:\r\n\
\r\n\
arrow keys - movement\r\n\
\'u\' - look up\r\n\
\'n\' - look down\r\n\
\r\n\
LEFT mouse button - look direction\r\n\
\r\n\
\'b\' - reset position back to start\r\n\
\'m\' - toggle motion model (between fly and FPS)\r\n\
\'SPACE\' - toggle collision model\r\n\
\']'/\'[\' - increase/decrease turn speed\r\n\
\'-\'/\'=\' - increase/decrease walk/fly speed"

////////////////////////////////////////////////////////////////////////////////
MainWindow::MainWindow(QMainWindow* parent)
: QMainWindow(parent)
, mLastMapInfo(NULL)
{
  ui.setupUi(this);

  mLastMapInfo = new QFileInfo(QString::fromStdString(dtUtil::GetEnvironment("DELTA_ROOT") + "/examples/data/maps/"));
  setWindowTitle("Shapash Facility Viewer");

 

}

////////////////////////////////////////////////////////////////////////////////
MainWindow::~MainWindow()
{
   delete mLastMapInfo;
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::SetGraphicsWidget(QWidget* graphics)
{
   QWidget* w = centralWidget();
   QLayout* l = w->layout();
   l->layout()->addWidget(graphics); 
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_actionOpen_triggered()
{ 
   const QString filename = QFileDialog::getOpenFileName(this,
                               tr("Load map"),
                               mLastMapInfo->absolutePath(),
                               "*.dtmap");
   if (filename.isEmpty())
   {
      //user canceled
      return;
   }

   mLastMapInfo->setFile(filename);
   emit LoadFile(filename);
}
//////////////////////////////////////////////
void MainWindow::closeEvent(QCloseEvent* e)
{
	std::cout << "close event\n";

	//REMOVE ALL DRAWABLES???
	emit appShutDown();

	//Disconnect the central widget because OSG wants to close it itself.
	centralWidget()->setParent(NULL);

	//centralWidget()->close();

	//e->accept();
	QApplication::quit();

	
}
////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_actionAbout_triggered()
{
	QMessageBox::about(this, "Open Facility Viewer", ABOUT_OFV);
	
}
////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_actionFuel_Assemblies_triggered()
{
	emit toggleVisibility("arrow_01", ui.actionFuel_Assemblies->isChecked());
}

void MainWindow::on_actionHot_Waste_triggered()
{
	emit toggleVisibility("arrow_02", ui.actionHot_Waste->isChecked());
}
void MainWindow::on_actionIrradiated_Fuel_Pins_triggered()
{
	emit toggleVisibility("arrow_03", ui.actionIrradiated_Fuel_Pins->isChecked());
}
void MainWindow::on_actionLiquid_Effluent_triggered()
{
	emit toggleVisibility("arrow_04", ui.actionLiquid_Effluent->isChecked());
}
void MainWindow::on_actionExperimental_Materials_triggered()
{
	emit toggleVisibility("arrow_05", ui.actionExperimental_Materials->isChecked());
}
void MainWindow::on_actionNuclear_Waste_Drums_triggered()
{
	emit toggleVisibility("arrow_06", ui.actionNuclear_Waste_Drums->isChecked());
}
void MainWindow::on_actionResearch_Waste_triggered()
{
	emit toggleVisibility("arrow_07", ui.actionResearch_Waste->isChecked());
}
void MainWindow::on_actionSolid_Waste_triggered()
{
	emit toggleVisibility("arrow_08", ui.actionSolid_Waste->isChecked());
}
void MainWindow::on_actionUranium_Oxide_Powders_triggered()
{
	emit toggleVisibility("arrow_09", ui.actionUranium_Oxide_Powders->isChecked());
}
void MainWindow::on_actionUranium_Samples_triggered()
{
	emit toggleVisibility("arrow_10", ui.actionUranium_Samples->isChecked());
}
void MainWindow::on_actionAll_triggered()
{
	checkAllArrows(true);
	refreshArrows();
}
void MainWindow::on_actionAll_Off_triggered()
{
	checkAllArrows(false);
	refreshArrows();
}
void MainWindow::checkAllArrows(bool check)
{
	ui.actionFuel_Assemblies->setChecked(check);
	ui.actionHot_Waste->setChecked(check);
	ui.actionIrradiated_Fuel_Pins->setChecked(check);
	ui.actionLiquid_Effluent->setChecked(check);
	ui.actionExperimental_Materials->setChecked(check);
	ui.actionNuclear_Waste_Drums->setChecked(check);
	ui.actionResearch_Waste->setChecked(check);
	ui.actionSolid_Waste->setChecked(check);
	ui.actionUranium_Oxide_Powders->setChecked(check);
	ui.actionUranium_Samples->setChecked(check);

}
void MainWindow::refreshArrows()
{
	
	emit toggleVisibility("arrow_01", ui.actionFuel_Assemblies->isChecked());
	emit toggleVisibility("arrow_02", ui.actionHot_Waste->isChecked());
	emit toggleVisibility("arrow_03", ui.actionIrradiated_Fuel_Pins->isChecked());
	emit toggleVisibility("arrow_04", ui.actionLiquid_Effluent->isChecked());
	emit toggleVisibility("arrow_05", ui.actionExperimental_Materials->isChecked());
	emit toggleVisibility("arrow_06", ui.actionNuclear_Waste_Drums->isChecked());
	emit toggleVisibility("arrow_07", ui.actionResearch_Waste->isChecked());
	emit toggleVisibility("arrow_08", ui.actionSolid_Waste->isChecked());
	emit toggleVisibility("arrow_09", ui.actionUranium_Oxide_Powders->isChecked());
	emit toggleVisibility("arrow_10", ui.actionUranium_Samples->isChecked());
}



/////////////////////////////////////////////////////////////////////////////
void MainWindow::on_actionControls_triggered()
{
	QMessageBox::about(this, "OFV Controls", HELP_OFV);
	
}
void MainWindow::on_actionFlying_Arrows_triggered()
{

	emit toggleVisibility("fac_10", ui.actionFlying_Arrows->isChecked());
}
void MainWindow::on_actionACB_Flow_Arrows_triggered()
{
	emit toggleVisibility("acbArrows", ui.actionACB_Flow_Arrows->isChecked());
}
void MainWindow::on_actionBuilding_Signs_triggered()
{
	emit toggleVisibility("fac_11", ui.actionBuilding_Signs->isChecked());
}
void MainWindow::on_actionSecurity_Signs_triggered()
{
	emit toggleVisibility("fac_13", ui.actionSecurity_Signs->isChecked());
}
void MainWindow::on_actionTowers_and_Fence_triggered()
{
	emit toggleVisibility("fac_12", ui.actionTowers_and_Fence->isChecked());
	emit toggleVisibility("fac1_fence", ui.actionTowers_and_Fence->isChecked());  
}
void MainWindow::on_actionNavigation_Map_triggered()
{
	emit toggleVisibility("navigation_map_OFV_STRING", ui.actionNavigation_Map->isChecked());

}
void MainWindow::on_action1_Shapash_Facility_Entrance_triggered()
{
	emit jumpTo(-108.0, 25.0, 1.6, -90.9702, 0, 0);

}
void MainWindow::on_action2_Storage_Bunker_triggered()
{
	emit jumpTo(86.2744, 40.64, 1.55965, 1.04571, 0, 0);
}
void MainWindow::on_action3_Research_Reactor_triggered()
{
	emit jumpTo(44.0217, 32.4428, 1.55965,179.433, 0, 0);
}
void MainWindow::on_action4_Reactor_Hall_triggered()
{
	emit jumpTo(43.7858, 14.2999, 1.51068,152.728, 0, 0);
}
void MainWindow::on_action5_Fuel_Fabrication_Building_triggered()
{
	emit jumpTo(86.8155, -30.6112, 1.5266, 90.9404, 0, 0);
}
void MainWindow::on_action6_Fuel_Fabrication_Pin_Assembly_Area_triggered()
{
	emit jumpTo(63.0477, -29.5171, 1.53997, 179.437, 0, 0);
}
void MainWindow::on_action7_Fuel_Fabrication_Building_Mezzanine_triggered()
{
	emit jumpTo(64.8261, -30.9273, 7.14687, 179.078, 0, 0);
}
void MainWindow::on_action8_Fuel_Fabrication_Building_Basement_triggered()
{
	emit jumpTo(65.0909, -30.9873, -1.83981, 179.492, 0, 0);
}
void MainWindow::on_action9_Access_Control_Building_triggered()
{
	emit jumpTo(-39.3876, 45.4558, 1.44134, -91.2052, 0, 0);
}
void MainWindow::on_action10_Special_Response_Team_Area_triggered()
{
	emit jumpTo(-21.884, 54.099, 1.54877, 131.474, 0, 0);
}
void MainWindow::on_action11_Central_Alarm_Station_triggered()
{
	emit jumpTo(-16.4768, 37.3994, 1.54877, 90.8022, 0, 0);
}
void MainWindow::on_action12_Scrapyard_triggered()
{
	emit jumpTo(72.4359, 10.051, 1.44134, -90.5736, 0, 0);
}
void MainWindow::on_action13_Radioactive_Waste_Site_triggered()
{
	emit jumpTo(-69.1701, -53.3757, 1.53306, 115.704, 0, 0);
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnFileLoaded(bool valid)
{
   if (valid == false)
   {
      QMessageBox::critical(this, tr("Can't load map"), tr("Could not load file: ") + mLastMapInfo->fileName());
   }
}

