# Open-Facility-Viewer

Open Facility Viewer (OFV) is an application for 3D interactive walk-throughs of facilities

OFV will not run without a "data/ProjectAssets" directory containing a valid Delta3D map file (*.dtmap)

Currently you must provide this. There are many examples in the Delta3D project.


====================================

### How to Build:

* Fully tested with Visual Studio 2008 service pack 1

* Requires CMake 2.6.4 for build system

Dependencies:
- Delta3D v2.7.4 (tested on trunk rev 8551)
- Qt 4.7.3

Delta3D has many dependencies. Get the matching Delta3D 2.7.4 dependency builds (for VS2008 win32) at http://svn.code.sf.net/p/delta3d-extras/code/ExtDep/releases/  (REV 3596)

====================================

### How to Run:

Add Open-Facility-Viewer/bin to your path. Run OFV.exe from the Open-Facility-Viewer directory. Do not run from bin directory, the project assets (data) will not be found.

====================================


### TODO:

- Needs to be tested/upgraded to Delta3d 2.8 RC1