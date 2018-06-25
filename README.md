# Open-Facility-Viewer

Open Facility Viewer (OFV) is an application for 3D interactive walk-throughs of facilities

OFV will not run without a "data/ProjectAssets" directory containing a valid Delta3D map file (*.dtmap)

Currently you must provide this. There are many examples in the Delta3D project.


====================================

### How to Build:

* Fully tested with Visual Studio 2015 64-bit

* Requires CMake 3.7 for build system

Dependencies:
- Delta3D v2.8 (64 bit build - see ext/ for patch)
- Qt 4.8.6 (64 bit build)

Delta3D has many dependencies. Get the matching Delta3D 2.8 dependency builds here:
https://sourceforge.net/projects/delta3d/files/delta3d-dependencies/2.9.0/Windows/delta3d-2.9.0-ext-msvc14.0.zip/download


====================================

### How to Run:

Add Open-Facility-Viewer/bin to your path. Run OFV.exe from the Open-Facility-Viewer directory. Do not run from bin directory, the project assets (data) will not be found.

