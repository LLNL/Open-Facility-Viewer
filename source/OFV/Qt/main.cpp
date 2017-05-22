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

#include <OFV/Qt/App.h>
#include <dtQt/qtguiwindowsystemwrapper.h>
#include <dtQt/deltastepper.h>
#include <QtGui/QApplication>
#include <dtCore/system.h>

//remove the console window in Release versions.

//#ifdef BUILD_AS_CONSOLE
int main(int argc, char ** argv) {
/*#else
#include <Windows.h>
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// Unicode fail...
	LPWSTR str1 = GetCommandLineW();
	QString command_line = QString::fromUtf16((const ushort*)str1);

	// This is not an obvious solution ... it preserves an argument with its quotes intact
	QRegExp regex(" +(?=(?:[^\"]*\"[^\"]*\")*[^\"]*$)");
	QStringList q_args = command_line.split(regex);


	int argc = q_args.size();
	static char** argv = new char*[argc];

	int i = 0;
	foreach(QString q_arg, q_args) {
		q_arg = q_arg.remove("\""); // until we remove them here
		size_t arg_s = q_arg.size() + 1;
		argv[i] = new char[arg_s];
		memcpy(argv[i], q_arg.toStdString().c_str(), arg_s);
		i++;
	}

#endif     // _CONSOLE
*/
	 
   //Setup Qt windowing system.
   QApplication qapp(argc, argv);
   dtQt::QtGuiWindowSystemWrapper::EnableQtGUIWrapper();
   dtUtil::Log::SetAllLogLevels(dtUtil::Log::LOG_INFO);
   
   App* app = new App(argc, argv);
   try
   {
   //Load our own game library DLL which is the heart of Delta3D.
   app->SetGameLibraryName("OFVGMApp"); 
   
   app->Config(); //configuring the application since we have no config file.
   
   //Do not call app->Run() because we are using Qt.  Use DeltaStepper. 
   //dtCore::System::GetInstance().SetShutdownOnWindowClose(true);
   dtCore::System::GetInstance().Start();
   dtQt::DeltaStepper stepper;
   stepper.Start();
    
   qapp.exec();
   
   stepper.Stop();
   dtCore::System::GetInstance().Stop();
   delete app;

   }
   catch (...)
   {
	  // e.LogException(dtUtil::Log::LOG_ERROR);
	  // return -1;
   }
   return 0;
}
