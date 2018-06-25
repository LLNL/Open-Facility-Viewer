//----------------------------------*-C++-*----------------------------------//
// Open Facility Viewer, an application for 3D walk-throughs of facilities
//
// Copyright (C) 2018, Lawrence Livermore National Security, LLC.
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
#include <OFV/Qt/ImageViewer.h>

#include <dtUtil/datapathutils.h>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QKeyEvent>
#include <iostream> 



#define ABOUT_OFV "Open Facility Viewer, an application for 3D walk-throughs of facilities\r\n\
\r\n\
Version 2.0.4\
\r\n\
Copyright (C) 2018, Lawrence Livermore National Security, LLC.\r\n\
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
, mImageViewerWaste(NULL)
, mImageViewerOxides(NULL)
, mImageViewerFreshFuel(NULL)
, mImageViewerTargets(NULL)
, mImageViewerAllMaterials(NULL)

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


void MainWindow::on_actionWaste_triggered()
{
	if (mImageViewerWaste == NULL)
	{
		mImageViewerWaste = new ImageViewer(this);
		mImageViewerWaste->open("Flow_Waste.png", "Flow Diagram - Waste");
	}
	mImageViewerWaste->show();
}
void MainWindow::on_actionFreshFuel_triggered()
{
	if (mImageViewerFreshFuel == NULL)
	{
		mImageViewerFreshFuel = new ImageViewer(this);
		mImageViewerFreshFuel->open("Flow_Fresh_Fuel.png", "Flow Diagram - Fresh Fuel");
	}
	mImageViewerFreshFuel->show();
}
void MainWindow::on_actionAllMaterials_triggered()
{
	if (mImageViewerAllMaterials == NULL)
	{
		mImageViewerAllMaterials = new ImageViewer(this);
		mImageViewerAllMaterials->open("Flow_All_Materials.png", "Flow Diagram - All Materials");
	}
	mImageViewerAllMaterials->show();
}
void MainWindow::on_actionOxides_triggered()
{
	if (mImageViewerOxides == NULL)
	{
		mImageViewerOxides = new ImageViewer(this);
		mImageViewerOxides->open("Flow_Oxides.png", "Flow Diagram - Oxides");
	}
	mImageViewerOxides->show();
}
void MainWindow::on_actionTargets_triggered()
{
	if (mImageViewerTargets == NULL)
	{
		mImageViewerTargets = new ImageViewer(this);
		mImageViewerTargets->open("Flow_Targets.png", "Flow Diagram - Targets");
	}
	mImageViewerTargets->show();
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::on_actionControls_triggered()
{
	QMessageBox::about(this, "OFV Controls", HELP_OFV);
	
}

void MainWindow::on_actionACB_Flow_Arrows_triggered()
{
	emit toggleVisibility("acbArrows", ui.actionACB_Flow_Arrows->isChecked());
}
void MainWindow::on_actionBuilding_Signs_triggered()
{
	emit toggleVisibility("fixed_materials_signs", ui.actionBuilding_Signs->isChecked());
	emit toggleVisibility("materials_fac_05", ui.actionReactor_Building->isChecked() && ui.actionBuilding_Signs->isChecked());
	emit toggleVisibility("materials_fac_06", ui.actionFuel_Fabrication_Building->isChecked() && ui.actionBuilding_Signs->isChecked());
}
void MainWindow::on_actionSecurity_Signs_triggered()
{
	emit toggleVisibility("fixed_security_signs", ui.actionSecurity_Signs->isChecked());
	emit toggleVisibility("security_signs_fence_admin", ui.actionSecurity_Signs->isChecked());
	emit toggleVisibility("security_signs_lighting_admin", ui.actionPerimeter_Lighting->isChecked() && ui.actionSecurity_Signs->isChecked());
	emit toggleVisibility("security_signs_lighting_inner", ui.actionPerimeter_Lighting->isChecked() && ui.actionSecurity_Signs->isChecked());
	emit toggleVisibility("security_signs_fence_inner", ui.actionInner_Protected_Area_Fence->isChecked() && ui.actionSecurity_Signs->isChecked());
	emit toggleVisibility("security_signs_isolation", ui.actionInner_Protected_Area_Fence->isChecked() && ui.actionSecurity_Signs->isChecked());
	emit toggleVisibility("security_signs_rad_monitors", ui.actionVehicle_Radiation_Portal_Monitors->isChecked() && ui.actionSecurity_Signs->isChecked());
	emit toggleVisibility("security_signs_microwave", ui.actionMicrowave_Sensors->isChecked() && ui.actionSecurity_Signs->isChecked());
	emit toggleVisibility("security_signs_fac_05", ui.actionReactor_Building->isChecked() && ui.actionSecurity_Signs->isChecked());
	emit toggleVisibility("security_signs_fac_06", ui.actionFuel_Fabrication_Building->isChecked() && ui.actionSecurity_Signs->isChecked());
	emit toggleVisibility("security_signs_towers", ui.actionGuard_Towers->isChecked() && ui.actionSecurity_Signs->isChecked());
}
void MainWindow::on_actionPerimeter_Lighting_triggered()
{
	
	emit toggleVisibility("switch_lighting_admin", ui.actionPerimeter_Lighting->isChecked());
	emit toggleVisibility("switch_lighting_outer", ui.actionPerimeter_Lighting->isChecked());
	emit toggleVisibility("security_signs_lighting_admin", ui.actionPerimeter_Lighting->isChecked() && ui.actionSecurity_Signs->isChecked());
	emit toggleVisibility("security_signs_lighting_inner", ui.actionPerimeter_Lighting->isChecked() && ui.actionSecurity_Signs->isChecked());

	emit toggleVisibility("switch_microwave_nolights", ui.actionMicrowave_Sensors->isChecked() && !ui.actionPerimeter_Lighting->isChecked());

}
void MainWindow::on_actionInner_Protected_Area_Fence_triggered()
{
	emit toggleVisibility("fac1_fence", ui.actionInner_Protected_Area_Fence->isChecked());

	//signs
	emit toggleVisibility("security_signs_fence_inner", ui.actionInner_Protected_Area_Fence->isChecked() && ui.actionSecurity_Signs->isChecked());
	emit toggleVisibility("security_signs_isolation", ui.actionInner_Protected_Area_Fence->isChecked() && ui.actionSecurity_Signs->isChecked());
	
	//colliders
	emit toggleVisibility("Colliders_fence_inner", ui.actionInner_Protected_Area_Fence->isChecked());
}
void MainWindow::on_actionVehicle_Radiation_Portal_Monitors_triggered()
{
	emit toggleVisibility("switch_rad_portal", ui.actionVehicle_Radiation_Portal_Monitors->isChecked());
	emit toggleVisibility("security_signs_rad_monitors", ui.actionVehicle_Radiation_Portal_Monitors->isChecked() && ui.actionSecurity_Signs->isChecked());
	
}
void MainWindow::on_actionMicrowave_Sensors_triggered()
{
	
	emit toggleVisibility("switch_microwave", ui.actionMicrowave_Sensors->isChecked());
	emit toggleVisibility("security_signs_microwave", ui.actionMicrowave_Sensors->isChecked() && ui.actionSecurity_Signs->isChecked());
	
	emit toggleVisibility("switch_microwave_nolights", ui.actionMicrowave_Sensors->isChecked() && !ui.actionPerimeter_Lighting->isChecked());


}
void MainWindow::on_actionReactor_Building_triggered()
{
	//OVERRIDE CALL - HANDLES INTERIORS
	emit toggleVisibility("fac_05", ui.actionReactor_Building->isChecked());

	//ground - use model "OFF" when checked
	emit toggleVisibility("switch_ground_reactor_OFF", !ui.actionReactor_Building->isChecked());

	//signs
	emit toggleVisibility("materials_fac_05", ui.actionReactor_Building->isChecked() && ui.actionBuilding_Signs->isChecked());
	emit toggleVisibility("security_signs_fac_05", ui.actionReactor_Building->isChecked() && ui.actionSecurity_Signs->isChecked());
	emit toggleVisibility("topsigns_fac_05", ui.actionReactor_Building->isChecked());

	//arrows
	emit toggleVisibility("switch_arrows_reactor", ui.actionReactor_Building->isChecked());

	//COLLIDERS
	emit toggleVisibility("Colliders_fac_05_OFF", !ui.actionReactor_Building->isChecked());
	emit toggleVisibility("Colliders_fac_05_ON", ui.actionReactor_Building->isChecked());

	//doors
	QStringList doors;
	doors << "13" << "25" << "26";
	for (int i = 48; i <= 78; i++)
	{
		doors << QString::number(i);
	}
	doors << "81" << "82" << "83";
	for (int i = 85; i <= 98; i++)
	{
		doors << QString::number(i);
	}
	foreach(QString s, doors)
	{
		emit toggleVisibility("door_" + s, ui.actionReactor_Building->isChecked());
	}

	//stairwells
	
	emit toggleVisibility("reactor_1_to_2_UP", ui.actionReactor_Building->isChecked());
	emit toggleVisibility("reactor_1_to_G _DOWN", ui.actionReactor_Building->isChecked());
	emit toggleVisibility("reactor_2_to_1_DOWN", ui.actionReactor_Building->isChecked());
	emit toggleVisibility("reactor_2_to_3_UP", ui.actionReactor_Building->isChecked());
	emit toggleVisibility("reactor_3_to_2_DOWN", ui.actionReactor_Building->isChecked());
	emit toggleVisibility("reactor_B_to_G_UP", ui.actionReactor_Building->isChecked());
	emit toggleVisibility("reactor_G_to_1 _UP", ui.actionReactor_Building->isChecked());
	emit toggleVisibility("reactor_G_to_B _DOWN", ui.actionReactor_Building->isChecked());

	emit setTeleportActive("reactor_hall_basement_stairdown", ui.actionReactor_Building->isChecked());
	emit setTeleportActive("reactor_hall_basement_stairup", ui.actionReactor_Building->isChecked());
	emit setTeleportActive("reactor_hall_stairdown", ui.actionReactor_Building->isChecked());
	emit setTeleportActive("reactor_hall_stairup", ui.actionReactor_Building->isChecked());
	
}
void MainWindow::on_actionFuel_Fabrication_Building_triggered()
{
	// OVERRIDE CALL - HANDLES INTERIORS
	emit toggleVisibility("fac_06", ui.actionFuel_Fabrication_Building->isChecked());

	//ground - use model "OFF" when checked
	emit toggleVisibility("switch_ground_fab_OFF", !ui.actionFuel_Fabrication_Building->isChecked());
	emit toggleVisibility("switch_ground_fab_ON", ui.actionFuel_Fabrication_Building->isChecked());

	//signs
	emit toggleVisibility("security_signs_fac_06", ui.actionFuel_Fabrication_Building->isChecked() && ui.actionSecurity_Signs->isChecked());
	emit toggleVisibility("materials_fac_06", ui.actionFuel_Fabrication_Building->isChecked() && ui.actionBuilding_Signs->isChecked());
	emit toggleVisibility("topsigns_fac_06", ui.actionFuel_Fabrication_Building->isChecked());

	//arrows
	emit toggleVisibility("switch_arrows_fab", ui.actionFuel_Fabrication_Building->isChecked());

	//COLLIDERS
	emit toggleVisibility("Colliders_fac_06_OFF", !ui.actionFuel_Fabrication_Building->isChecked());
	emit toggleVisibility("Colliders_fac_06_ON", ui.actionFuel_Fabrication_Building->isChecked());

	//stairwells
	
	emit setTeleportActive("stairdown_1", ui.actionFuel_Fabrication_Building->isChecked());
	emit setTeleportActive("stairdown_basement", ui.actionFuel_Fabrication_Building->isChecked());
	emit setTeleportActive("stairdown_basement_02", ui.actionFuel_Fabrication_Building->isChecked());
	emit setTeleportActive("stairup_1", ui.actionFuel_Fabrication_Building->isChecked());
	emit setTeleportActive("stairup_basement_01", ui.actionFuel_Fabrication_Building->isChecked());
	emit setTeleportActive("stairup_basement_02", ui.actionFuel_Fabrication_Building->isChecked());
	

	//doors
	QStringList doors;
	doors << "12";
	for (int i = 16; i <= 23; i++)
	{
		doors << QString::number(i);
	}
	for (int i = 27; i <= 47; i++)
	{
		doors << QString::number(i);
	}
	doors << "84";

	foreach(QString s, doors)
	{
		emit toggleVisibility("door_" + s, ui.actionFuel_Fabrication_Building->isChecked());
	}
}
void MainWindow::on_actionGuard_Towers_triggered()
{
	emit toggleVisibility("fac_12", ui.actionGuard_Towers->isChecked());
	emit toggleVisibility("security_signs_towers", ui.actionGuard_Towers->isChecked() && ui.actionSecurity_Signs->isChecked());
	emit toggleVisibility("switch_arrows_towers", ui.actionGuard_Towers->isChecked());

	//COLLIDERS
	emit toggleVisibility("Colliders_guard_tower", ui.actionGuard_Towers->isChecked());
	
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
	emit jumpTo(77.4411, 9.67979, 1.55965, 90.0, 0, 0);
}
void MainWindow::on_action4_Reactor_Hall_triggered()
{
	emit jumpTo(46.4216, 8.49228, 9.8967,80.0, 0, 0);
}
void MainWindow::on_action5_Fuel_Fabrication_Building_triggered()
{
	emit jumpTo(82.307, -34.597, 1.55964, 90.9404, 0, 0);
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
	emit jumpTo(76.0858, 1.17021, 1.55965, -61.0, 0, 0);
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

