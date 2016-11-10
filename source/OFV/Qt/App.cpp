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
// With the exception of portions noted below.
//---------------------------------------------------------------------------//
// 
// OFV - App
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

#include <OFV/Qt/App.h>

#include <dtCore/deltawin.h>
#include <QtOpenGL/QGLWidget>
#include <dtQt/osggraphicswindowqt.h>
#include <dtCore/project.h>
#include <dtCore/map.h>
#include <dtCore/system.h>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include <iostream>

using namespace dtCore;
using namespace dtABC;

App::App(int argc, char **argv, const std::string& configFilename)
: GameApplication(argc, argv, configFilename)
{
	
}

//////////////////////////////////////////////////////////////////////////
App::~App()
{
   //The widget's about to be destroyed so we need to tell the GraphicsWindow it's gone (to avoid crash on exit)
   dtQt::OSGGraphicsWindowQt* osgGraphWindow = dynamic_cast<dtQt::OSGGraphicsWindowQt*>(GetWindow()->GetOsgViewerGraphicsWindow());
   if (osgGraphWindow)
   {
      osgGraphWindow->SetQGLWidget(NULL);
   }
}

//////////////////////////////////////////////////////////////////////////
void App::Config()
{
   GameApplication::Config();
   connect(&mMainWindow, SIGNAL(LoadFile(const QString&)), this, SLOT(OnLoadFile(const QString&)));

   connect(this, SIGNAL(FileLoaded(bool)), &mMainWindow, SLOT(OnFileLoaded(bool)));

   osg::DisplaySettings::instance()->setNumMultiSamples( 8 ); 

   dtQt::OSGGraphicsWindowQt* osgGraphWindow = dynamic_cast<dtQt::OSGGraphicsWindowQt*>(GetWindow()->GetOsgViewerGraphicsWindow());
   mMainWindow.SetGraphicsWidget(osgGraphWindow->GetQGLWidget());
   
   QGLFormat fmt;
   QGLFormat::setDefaultFormat(fmt); 
   fmt.setSamples(8);
   fmt.setSampleBuffers(true);
   osgGraphWindow->GetQGLWidget()->setFormat(fmt);

   mMainWindow.show(); 
}

////////////////////////////////////////////////////////////////////////////////
void App::OnLoadFile(const QString& filename)
{
   
   bool valid = true;
   QFileInfo file(filename);
   QDir dir = file.dir();
   dir.cdUp();

   std::string contextName = dir.absolutePath().toStdString();
   std::cout << "Setting context to " << contextName << "\n";
   Project::GetInstance().SetContext(contextName, true);
   
   std::cout << "Loading map " << file.baseName().toStdString() << "\n";
   mMap = &Project::GetInstance().GetMap(file.baseName().toStdString());
   
   LoadMap(*mMap);

   System::GetInstance().SetFrameRate(60.0);
   System::GetInstance().SetMaxTimeBetweenDraws(.1);
   System::GetInstance().SetUseFixedTimeStep(true);

   
   GetGameManager()->SetPaused(false);

   emit FileLoaded(valid);
}
