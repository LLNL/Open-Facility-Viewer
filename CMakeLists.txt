CMAKE_MINIMUM_REQUIRED(VERSION 2.6 FATAL_ERROR)

if(COMMAND cmake_policy)
   cmake_policy(SET CMP0003 OLD)
endif(COMMAND cmake_policy)

PROJECT(OFV)



IF (WIN32)
   OPTION(BUILD_WITH_MP "Enables the /MP multi-processor compiler option for Visual Studio 2005 and above" ON)
   MARK_AS_ADVANCED(BUILD_WITH_MP)
   IF(BUILD_WITH_MP)
     SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
   ENDIF(BUILD_WITH_MP)
   SET(CMAKE_DEBUG_POSTFIX  "d")
ENDIF (WIN32)


# We have some custom .cmake scripts not in the official distribution.
# Maybe this can be used override existing behavior if needed?
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH};")

  
INCLUDE(UtilityMacros)

# Find OpenGL
FIND_PACKAGE(OpenGL REQUIRED)




IF(UNIX)
     # Some Unicies need explicit linkage to the Math library or the build fails.
    FIND_LIBRARY(MATH_LIBRARY m)

    ADD_DEFINITIONS(-DSIGSLOT_PURE_ISO)

    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-non-virtual-dtor")
ENDIF (UNIX)
        
IF(WIN32)
    ADD_DEFINITIONS(-D_SCL_SECURE_NO_WARNINGS)
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
	
    #needed for net plugin
    #SET (OFV_SOCKET_LIBS wsock32)
    ADD_DEFINITIONS(-DNOMINMAX)
	
########################################################################################################
# the following options are MSVC specific,
# the first OFV_MSVC_VERSIONED_DLL activate a custom build-time layout that should allow to run examples and application 
# fron bin folder without requiring installation step.
# it also prepend "osg${OPENSCENEGRAPH_SOVERSION}-" to only .dll files, leaving .lib files untouched in lib
# it also use a hack to get rid of Debug and Release folder in MSVC projects 
# all the .dll and .pdb are in bin and all the .lib and .exp are in lib
# 
# the second option disable incremental linking in debug build , that is enabled by default by CMake
##########################################################################################################

    IF(MSVC)
        SET(CMAKE_MODULE_LINKER_FLAGS "/NODEFAULTLIB:libc.lib;licmt;libcd;libcmtd")
        SET(CMAKE_SHARED_LINKER_FLAGS "/NODEFAULTLIB:libc.lib;licmt;libcd;libcmtd")
        SET(CMAKE_EXE_LINKER_FLAGS "/NODEFAULTLIB:libc.lib;licmt;libcd;libcmtd")
        IF(${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4 AND ${CMAKE_PATCH_VERSION} LESS 7)
            MESSAGE("Warning:  disabling versioned options 2.4.6 exibits inconsintencies in .pdb naming, at least under MSVC, suggested upgrading at least to 2.4.7")
            SET(OFV_MSVC_VERSIONED_DLL OFF)
            SET(OFV_MSVC_DEBUG_INCREMENTAL_LINK ON)
        ELSE(${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4 AND ${CMAKE_PATCH_VERSION} LESS 7)
            OPTION(OFV_MSVC_VERSIONED_DLL "Set to ON to build OpenSceneGraph with versioned dll names" ON)
            MARK_AS_ADVANCED(OFV_MSVC_VERSIONED_DLL)
            OPTION(OFV_MSVC_DEBUG_INCREMENTAL_LINK "Set to OFF to build OpenSceneGraph without incremental linking in debug (release is off by default)" ON)
            MARK_AS_ADVANCED(OFV_MSVC_DEBUG_INCREMENTAL_LINK)
            IF(NOT OFV_MSVC_DEBUG_INCREMENTAL_LINK)
                SET(CMAKE_MODULE_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:NO")
                SET(CMAKE_SHARED_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:NO")
                SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:NO")
            ENDIF(NOT OFV_MSVC_DEBUG_INCREMENTAL_LINK)
        ENDIF(${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4 AND ${CMAKE_PATCH_VERSION} LESS 7)
    ENDIF(MSVC)
ENDIF(WIN32)


# This is needed for the install command to work so it doesn't install to program files by default.
SET(CMAKE_INSTALL_PREFIX
    "${PROJECT_BINARY_DIR}" CACHE PATH "install prefix" FORCE
    )


################################################################################
# 3rd Party Dependency Stuff
# Common to all platforms:


FIND_PATH(myQt bin/qmake.exe
	PATHS
	${CMAKE_SOURCE_DIR}/../../Qt/4.7.3/
)	 
		
IF(myQt)
 SET(QT_QMAKE_EXECUTABLE "${myQt}/bin/qmake")
ENDIF(myQt)


#message(ENV{QTDIR} $ENV{QTDIR})
FIND_PACKAGE(Qt4 REQUIRED) # needs to be loaded before SceneWorks

FIND_PACKAGE(OFV REQUIRED)

# Qt needs to be loaded before Delta3D
SET(QT_USE_QTOPENGL "true")

   
# this should be first! before we override any other paths
FIND_PACKAGE(Delta3D REQUIRED)
#SET(DELTA3D_LIB_DIR "${DELTA_DIR}/lib")
#message("${CMAKE_MODULE_PATH}")
message("${DELTA3D_LIB_DIR}")


IF (DELTA3D_FOUND) 
   #look for module loaders in delta3d CMakeModules
   SET(CMAKE_MODULE_PATH "${DELTA_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")
   
   
   IF(NOT WIN32)
      FIND_PACKAGE(Curses REQUIRED)
   ENDIF(NOT WIN32)

   FIND_PACKAGE(OpenAL REQUIRED)

   FIND_PACKAGE(OpenThreads REQUIRED)
   FIND_PACKAGE(OSG REQUIRED)

   FIND_PACKAGE(Cal3d REQUIRED)
   FIND_PACKAGE(CEGUI REQUIRED)
   FIND_PACKAGE(CPPUNIT REQUIRED)
   FIND_PACKAGE(ODE REQUIRED)
   FIND_PACKAGE(XercesC REQUIRED)
   FIND_PACKAGE(HAWKNL REQUIRED)
   FIND_PACKAGE(GNE REQUIRED)
   
   IF (NOT WIN32 AND NOT APPLE)
     FIND_PACKAGE(UUID REQUIRED) 
   ENDIF (NOT WIN32 AND NOT APPLE)  

   FIND_PACKAGE(GDAL)
   FIND_PACKAGE(DIS)
  
ENDIF (DELTA3D_FOUND)

# Make the headers visible to everything
INCLUDE_DIRECTORIES(
    ${OFV_INCLUDE_DIR}
    ${DELTA3D_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${OPENTHREADS_INCLUDE_DIR}
    ${OSG_INCLUDE_DIR}
    ${CAL3D_INCLUDE_DIR}
    ${CEGUI_INCLUDE_DIR}
    ${ODE_INCLUDE_DIR}
    ${GNE_INCLUDE_DIR}
    ${HAWKNL_INCLUDE_DIR}
    ${XERCES_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR}
    ${PYTHON_INCLUDE_PATH}  
)

IF (APPLE)
   INCLUDE_DIRECTORIES(/usr/include/malloc)
ENDIF (APPLE)

LINK_DIRECTORIES( ${DELTA3D_LIB_DIR})

IF (DELTA3D_EXT_DIR)
  LINK_DIRECTORIES( ${DELTA3D_EXT_DIR}/lib )
ENDIF (DELTA3D_EXT_DIR)
################################################################################
# Create bin and lib directories if required

IF("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
   FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin ${CMAKE_BINARY_DIR}/lib )
ENDIF("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")


################################################################################
# Installation stuff



IF(UNIX AND NOT WIN32 AND NOT APPLE)
  IF(CMAKE_SIZEOF_VOID_P MATCHES "8")
      SET(LIB_POSTFIX "64" CACHE STRING "suffix for 32/64 dir placement")
      MARK_AS_ADVANCED(LIB_POSTFIX)
  ENDIF(CMAKE_SIZEOF_VOID_P MATCHES "8")
ENDIF(UNIX AND NOT WIN32 AND NOT APPLE)
IF(NOT DEFINED LIB_POSTFIX)
    SET(LIB_POSTFIX "")
ENDIF(NOT DEFINED LIB_POSTFIX)
    
SET(OUTPUT_BINDIR ${PROJECT_BINARY_DIR}/bin)
MAKE_DIRECTORY(${OUTPUT_BINDIR})
SET(EXECUTABLE_OUTPUT_PATH ${OUTPUT_BINDIR})

SET(OUTPUT_LIBDIR ${PROJECT_BINARY_DIR}/lib)
MAKE_DIRECTORY(${OUTPUT_LIBDIR})
SET(LIBRARY_OUTPUT_PATH ${OUTPUT_LIBDIR})


# Expose CMAKE_INCLUDE_PATH and CMAKE_LIBARY_PATH to the GUI so users
# may set these values without needing to manipulate the environment.
SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} CACHE STRING "You may add additional search paths here. Use ; to separate multiple paths.")
SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} CACHE STRING "You may add additional search paths here. Use ; to separate multiple paths.")


# We are proposing that a new variable called CMAKE_PREFIX_PATH be introduced
# to CMake to compliment CMAKE_INCLUDE_PATH and CMAKE_LIBRARY_PATH.
# A formal feature request has been submited to CMake, Bug #4947.
# It is intended for those users who have common prefixes for their INCLUDE
# and LIBRARY locations. So if users have headers in /usr/local/include
# and libraries in /usr/local/lib, the common prefix is /usr/local.
# It should also cover the case where headers and libraries are
# in the same directory.
# Our proposal expects that FIND_* commands will automatically search for
# CMAKE_PREFIX_PATH right after CMAKE_INCLUDE_PATH or CMAKE_LIBRARY_PATH.
# Obviously, since CMake does not currently support this, we must write
# our Find*.cmake modules to explicitly support this. Otherwise, this variable
# will have no impact.
# This is unofficial so this may be removed or changed at anytime.
SET(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} CACHE STRING "(EXPERIMENTAL) You may add additional search paths here. Use ; to separate multiple paths.")


ADD_SUBDIRECTORY(source)



# This is for an advanced option to give aggressive warnings 
# under different compilers. If yours is not implemented, this option
# will not be made available.
IF(CMAKE_COMPILER_IS_GNUCXX)
    # To be complete, we might also do GNUCC flags, 
    # but everything here is C++ code.
    # -Wshadow and -Woverloaded-virtual are also interesting flags, but OSG
    # returns too many hits.
    # FYI, if we do implement GNUCC, then -Wmissing-prototypes in another 
    # interesting C-specific flag.
    # Also, there is a bug in gcc 4.0. Under C++, -pedantic will create
    # errors instead of warnings for certain issues, including superfluous
    # semicolons and commas, and the use of long long. -fpermissive seems
    # to be the workaround. 
    SET(OFV_AGGRESSIVE_WARNING_FLAGS "-Wall -Wparentheses -Wformat=2 -Wno-long-long -Wno-import -pedantic -Wreturn-type -Wmissing-braces -Wunknown-pragmas -Wunused -fpermissive")
ELSE(CMAKE_COMPILER_IS_GNUCXX)
    IF(MSVC)
        # FIXME: What are good aggressive warning flags for Visual Studio?
        # And do we need to further subcase this for different versions of VS?
        # CMake variables: MSVC60, MSVC70, MSVC71, MSVC80, CMAKE_COMPILER_2005
        SET(OFV_AGGRESSIVE_WARNING_FLAGS "/Wall /W4")
        

    ELSE(MSVC)
        # CMake lacks an elseif, so other non-gcc, non-VS compilers need
        # to be listed below. If unhandled, OFV_AGGRESSIVE_WARNING_FLAGS should 
        # remain unset.
    ENDIF(MSVC)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

# This part is for the CMake menu option to toggle the warnings on/off.
# This will only be made available if we set values for OFV_AGGRESSIVE_WARNING_FLAGS.
IF(OFV_AGGRESSIVE_WARNING_FLAGS)
    OPTION(OFV_USE_AGGRESSIVE_WARNINGS "Enable to activate aggressive warnings" OFF)
    MARK_AS_ADVANCED(OFV_USE_AGGRESSIVE_WARNINGS)

    IF(OFV_USE_AGGRESSIVE_WARNINGS)
        IF(NOT "${OLD_CMAKE_CXX_FLAGS_WAS_SET}")
            SET(OLD_CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" CACHE INTERNAL "Old CXX flags")
            SET(OLD_CMAKE_CXX_FLAGS_WAS_SET 1 CACHE INTERNAL "Old CXX flags was set")
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OFV_AGGRESSIVE_WARNING_FLAGS}" CACHE STRING "Flags used by the compiler during all build types." FORCE)
        ENDIF(NOT "${OLD_CMAKE_CXX_FLAGS_WAS_SET}")
    ELSE(OFV_USE_AGGRESSIVE_WARNINGS)
        # FIXME: This will lose any changes made after OLD_CMAKE_CXX_FLAGS was 
        # set. The better way would be to parse the string and remove each
        # option explicitly.
        IF("${OLD_CMAKE_CXX_FLAGS_WAS_SET}")
            SET(CMAKE_CXX_FLAGS "${OLD_CMAKE_CXX_FLAGS}" CACHE STRING "Flags used by the compiler during all build types." FORCE)
            SET(OLD_CMAKE_CXX_FLAGS_WAS_SET 0 CACHE INTERNAL "Old CXX flags was set")
        ENDIF("${OLD_CMAKE_CXX_FLAGS_WAS_SET}")
    ENDIF(OFV_USE_AGGRESSIVE_WARNINGS)
ENDIF(OFV_AGGRESSIVE_WARNING_FLAGS)

# Set defaults for Universal Binaries. We want 32-bit Intel/PPC on 10.4 
# and 32/64-bit Intel/PPC on >= 10.5. Anything <= 10.3 doesn't support.
#IF(APPLE)
#
#    # These are just defaults/recommendations, but how we want to build
#    # out of the box. But the user needs to be able to change these options.
#    # So we must only set the values the first time CMake is run, or we 
#    # will overwrite any changes the user sets.
#    # FORCE is used because the options are not reflected in the UI otherwise.
#    # Seems like a good place to add version specific compiler flags too.
#    IF(NOT OFV_CONFIG_HAS_BEEN_RUN_BEFORE)
#        # This is really fragile, but CMake doesn't provide the OS system 
#        # version information we need. (Darwin versions can be changed 
#        # independently of OS X versions.)
#        # It does look like CMake handles the CMAKE_OSX_SYSROOT automatically.
#        IF(EXISTS /Developer/SDKs/MacOSX10.5.sdk)
#            SET(CMAKE_OSX_ARCHITECTURES "i386" CACHE STRING "Build architectures for OSX" FORCE)
#            SET(CMAKE_OSX_ARCHITECTURES "ppc;i386;ppc64;x86_64" CACHE STRING "Build architectures for OSX" FORCE)
#            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -isysroot /Developer/SDKs/MacOSX10.5.sdk -mmacosx-version-min=10.5 -ftree-vectorize -fvisibility-inlines-hidden" CACHE STRING "Flags used by the compiler during all build types." FORCE)
#        ELSE(EXISTS /Developer/SDKs/MacOSX10.5.sdk)
#            IF(EXISTS /Developer/SDKs/MacOSX10.4u.sdk)
#                SET(CMAKE_OSX_ARCHITECTURES "i386" CACHE STRING "Build architectures for OSX" FORCE)
#                SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -isysroot /Developer/SDKs/MacOSX10.4u.sdk -mmacosx-version-min=10.4 -ftree-vectorize -fvisibility-inlines-hidden" CACHE STRING "Flags used by the compiler during all build types." FORCE)
#            ELSE(EXISTS /Developer/SDKs/MacOSX10.4u.sdk)
#                # No Universal Binary support
#                # Should break down further to set the -mmacosx-version-min,
#                # but the SDK detection is too unreliable here.
#            ENDIF(EXISTS /Developer/SDKs/MacOSX10.4u.sdk)
#        ENDIF(EXISTS /Developer/SDKs/MacOSX10.5.sdk)
#    ENDIF(NOT OFV_CONFIG_HAS_BEEN_RUN_BEFORE)
#    
#    OPTION(OFV_BUILD_APPLICATION_BUNDLES "Enable the building of applications and examples as OSX Bundles" OFF)
#    
#ENDIF(APPLE)


# This needs to be run very last so other parts of the scripts can take
# advantage of this.
#IF(NOT OFV_CONFIG_HAS_BEEN_RUN_BEFORE)
#    SET(OFV_CONFIG_HAS_BEEN_RUN_BEFORE 1 CACHE INTERNAL "Flag to track whether this is the first time running CMake or if CMake has been configured before")
#ENDIF(NOT OFV_CONFIG_HAS_BEEN_RUN_BEFORE)


#-----------------------------------------------------------------------------
### uninstall target
# What does this do exactly???
#-----------------------------------------------------------------------------
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)
ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
