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

#define ABOUT_OFV "Open Facility Viewer, an application for 3D walk-throughs of facilities\r\n\
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
w/a/s/d - movement\r\n\
\r\n\
LEFT mouse button - direction\r\n\
\r\n\
\'b\' - reset position back to start\r\n\
\'m\' - toggle motion model (between fly and FPS)\r\n\
\'SPACE\' - toggle collision model\r\n\
\'-\'/\'=\' - increase/decrease speed"

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

////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_actionAbout_triggered()
{
	QMessageBox::about(this, "Open Facility Viewer", ABOUT_OFV);
	
}
////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_actionControls_triggered()
{
	QMessageBox::about(this, "OFV Controls", HELP_OFV);
	
}
////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnFileLoaded(bool valid)
{
   if (valid == false)
   {
      QMessageBox::critical(this, tr("Can't load map"), tr("Could not load file: ") + mLastMapInfo->fileName());
   }
}
