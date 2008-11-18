CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(photogrammetry)
SET(CMAKE_BUILD_TYPE DEBUG)

FIND_PACKAGE(Qt4 REQUIRED)
include(UseQt4)
ADD_DEFINITIONS( ${QT_DEFINITIONS} )

set(QTMOC_SRCS 
    photogrammetry.h
    phgproject.h
    projectmodel.h
    photopointsmodel.h
    controlpointsmodel.h
    photopoints.h
    intersection.h
    )
QT4_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(QTUIC_SRCS 
    ui/photogrammetrybase.ui
    )
QT4_WRAP_UI(QT_UIC_HEADERS ${QTUIC_SRCS})

SET(QT_USE_QTSQL 1)
SET(QT_USE_QTMAIN 1)

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR} 
    ${CMAKE_CURRENT_BINARY_DIR}
    "d:/dev/include"
    "~/dev/"
    )
INCLUDE(
    ${QT_USE_FILE}
    )

#AUX_SOURCE_DIRECTORY(. _SRCS)
SET(_HEADERS
    controlpoints.h
    globaldefn.h
    phgproject.h
    photogrammetry.h
    photopoints.h
    projectmodel.h
    photopointsmodel.h
    controlpointsmodel.h
    lls.h
    intersection.h
    )
SET(_SRCS
    main.cpp
    controlpoints.cpp
    photogrammetry.cpp
    photopoints.cpp
    phgproject.cpp
    projectmodel.cpp
    photopointsmodel.cpp
    controlpointsmodel.cpp
    lls.cpp
    intersection.cpp
    )
SET(SRCS
    ${_HEADERS}
    ${_SRCS}
    ${QT_MOC_SRCS}
    ${QT_UIC_HEADERS}
    )

# use WIN32 cause /SUBSYSTEM:WINDOWS, or a console built
ADD_EXECUTABLE(photogrammetry 
    ${SRCS}
    )



IF(WIN32)
    SET(LIBRARYS blas_win32 lapack_win32)
ELSE(WIN32)
    SET(LIBRARYS  blas  lapack  )
ENDIF(WIN32)

TARGET_LINK_LIBRARIES(photogrammetry ${QT_LIBRARIES} ${LIBRARYS})
