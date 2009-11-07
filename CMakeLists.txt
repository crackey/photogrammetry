CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(photogrammetry)
SET(CMAKE_BUILD_TYPE RELEASE)

FIND_PACKAGE(Qt4 REQUIRED)
include(UseQt4)
ADD_DEFINITIONS( ${QT_DEFINITIONS} )

enable_language(Fortran)
#SET(BLA_STATIC TRUE)
FIND_PACKAGE(BLAS REQUIRED)
FIND_PACKAGE(LAPACK REQUIRED)

set(QTMOC_SRCS 
    photogrammetry.h
    phgproject.h
    projectmodel.h
    photopointsmodel.h
    controlpointsmodel.h
    photopoints.h
    intersection.h
    intersectionwidget.h
    orientationwidget.h
    orientation.h
    onestep.h
    onestepwidget.h
    )
QT4_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(QTUIC_SRCS 
    ui/photogrammetrybase.ui
    ui/intersectionwidgetbase.ui
    ui/orientationwidgetbase.ui
    ui/onestepwidgetbase.ui
    )
QT4_WRAP_UI(QT_UIC_HEADERS ${QTUIC_SRCS})

SET(QT_USE_QTSQL 1)
SET(QT_USE_QTMAIN 1)

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR} 
    ${CMAKE_CURRENT_BINARY_DIR}
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
    intersectionwidget.h
    transform.h
    orientation.h
    orientationwidget.h
    onestep.h
    onestepwidget.h
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
    intersectionwidget.cpp
    transform.cpp
    orientation.cpp
    orientationwidget.cpp
    onestep.cpp
    onestepwidget.cpp
    )
SET(SRCS
    ${_HEADERS}
    ${_SRCS}
    ${QT_MOC_SRCS}
    ${QT_UIC_HEADERS}
    )


# lapack libraries
SET(LIBS ${QT_LIBRARIES} ${BLAS_LIBRARIES} ${LAPACK_LIBRARIES})

# use WIN32 cause /SUBSYSTEM:WINDOWS, or a console built
ADD_EXECUTABLE(photogrammetry
    ${SRCS}
    )


TARGET_LINK_LIBRARIES(photogrammetry lapack ${LIBS})
