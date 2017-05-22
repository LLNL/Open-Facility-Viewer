INCLUDE(UtilityMacros)
SET(LIB_NAME     ${OFVGM_LIBRARY})

SET(HEADER_PATH ${OFV_SOURCE_DIR}/include/OFV/GMApp)
SET(SOURCE_PATH ${OFV_SOURCE_DIR}/source/OFV/GMApp)

INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR})
					

FILE(GLOB LIB_PUBLIC_HEADERS 
   "${HEADER_PATH}/*.h")
     
SET(LIB_SOURCES 
   "${SOURCE_PATH}/OFVGameEntryPoint.cpp"
   "${SOURCE_PATH}/OFVInputComponent.cpp"   
   "${SOURCE_PATH}/HumanMotionModel.cpp"
   "${SOURCE_PATH}/FlyLimitMotionModel.cpp"
   )

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h)
   
list(REMOVE_ITEM LIB_SOURCES ${SOURCE_PATH}/precomp.cpp)

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS}
    ${LIB_SOURCES}
)



TARGET_LINK_LIBRARIES(${LIB_NAME} 
   ${OPENGL_LIBRARY}
   ${AGL_LIBRARY}
)

LINK_WITH_VARIABLES(${LIB_NAME}	
						 DTUTIL_LIBRARY
                         DTCORE_LIBRARY
                         DTANIM_LIBRARY
                         DTAI_LIBRARY
                         DTABC_LIBRARY
                         DTGAME_LIBRARY
                         DTGUI_LIBRARY
                         DTAUDIO_LIBRARY
                         DTACTORS_LIBRARY
                         OSG_LIBRARY
                         OSGGA_LIBRARY
                         OSOFVER_LIBRARY
                         OSGUTIL_LIBRARY
                         OSGFX_LIBRARY
                         OSGSIM_LIBRARY
                         OSGPARTICLE_LIBRARY
                         OSGTEXT_LIBRARY
                         GDAL_LIBRARY
                         CAL3D_LIBRARY
                         XERCES_LIBRARY
                         OSGDB_LIBRARY
                         OPENTHREADS_LIBRARY
                         CEGUI_LIBRARY
                         CEGUIOPENGL_LIBRARY
                         DTPHYSICS_LIBRARY
                         PAL_LIBRARY
                         OSGSHADOW_LIBRARY
						 OSGVIEWER_LIBRARY
                         QT_QTCORE_LIBRARY
						QT_QTGUI_LIBRARY
                                   )



SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL OFV_GAME_LIBRARY)


SET_MSVC_TARGET(${LIB_NAME} "${OFV_SOURCE_DIR}/bin")
