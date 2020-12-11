QT      += gui widgets printsupport core charts opengl

TEMPLATE = app
TARGET  = SHC_Charting
CONFIG  += c++14

FORMS   = SHCTestingShell/shcShell.ui SHCTestingShell/gaussTest.ui SHCTestingShell/diagram.ui \
          SHCTestingShell/clusterDef.ui SHCTestingShell/outlierDef.ui SHCTestingShell/linClusterMovementDef.ui \
          SHCTestingShell/compTooltip.ui SHCTestingShell/circClusterMovementDef.ui SHCTestingShell/openGLparams.ui

HEADERS = \
    SHC/SHC_Component.hpp \
    SHC/SHC.hpp \
    SHC/SHC_Container.hpp \
    SHC/SHC_Utils.hpp \
    SHC/SHC_ComponentConnection.hpp \
    SHC/SHC_Decay.hpp \
    SHC/SigmaIndex.hpp \
    SHC/SigmaIndexProxy.hpp \
    DataSources/SHCDataSource.hpp \
    DataSources/GaussDataSource.hpp \
    SHCCharting/SHCShellWindow.hpp \
    SHCCharting/SHCGaussGenerator.hpp \
    SHCCharting/SHCGaussDiagram.hpp \
    SHCCharting/SHCDiagramWidget.hpp \
    SHCCharting/SHCClusterDefWidget.hpp \
    SHCCharting/SHCOutlierDefWidget.hpp \
    SHCCharting/SHCOpenGLTestWidget.hpp \
    SHCCharting/SHCLinMovingClusterDefWidget.hpp \
    SHCCharting/SHCCircMovingClusterDefWidget.hpp \
    SHCCharting/SHCComponentTooltip.hpp \
    SHCCharting/SHCOpenGLParams.hpp \
    SHCCharting/SHCComponentAreaSeries.hpp \
    SHCCharting/SHCChartingUtils.hpp \
    SHCCharting/SHCDatasetProcessing.hpp

SOURCES = \
    SHC/SHC_Component.cpp \
    SHC/SHC_Container.cpp \
    SHC/SHC_Utils.cpp \
    SHC/SHC.cpp \
    SHC/SHC_ComponentConnection.cpp \
    SHC/SHC_Decay.cpp \
    SHC/SigmaIndex.cpp \
    SHC/SigmaIndex_Inline.cpp \
    SHC/SigmaIndexProxy.cpp \
    DataSources/SHCDataSource.cpp \
    DataSources/GaussDataSource.cpp \
    SHCCharting/main.cpp \
    SHCCharting/SHCShellWindow.cpp \
    SHCCharting/SHCGaussGenerator.cpp \
    SHCCharting/SHCGaussDiagram.cpp \
    SHCCharting/SHCDiagramWidget.cpp \
    SHCCharting/SHCClusterDefWidget.cpp \
    SHCCharting/SHCOutlierDefWidget.cpp \
    SHCCharting/SHCOpenGLTestWidget.cpp \
    SHCCharting/SHCLinMovingClusterDefWidget.cpp \
    SHCCharting/SHCCircMovingClusterDefWidget.cpp \
    SHCCharting/SHCComponentTooltip.cpp \
    SHCCharting/SHCOpenGLParams.cpp \
    SHCCharting/SHCComponentAreaSeries.cpp \
    SHCCharting/SHCChartingUtils.cpp \
    SHCCharting/SHCDatasetProcessing.cpp
    
INCLUDEPATH += \
    /Users/dkrleza/Documents/FER/Ostalo/libs
    
OBJECTS += SHCShellWindow SHCGaussGeneratorWindow SHCGaussChartView SHCGaussDiagramChart SHCDiagramWidget SHCClusterDefWidget \
           SHCOutlierDefWidget OpenGLTestWidget SHCLinMovingClusterDefWidget SHCComponentTooltip SHCComponentAreaSeries \
           SHCCircMovingClusterDefWidget SHCOpenGLParams SHCDatasetProcessing

RESOURCES = SHCCharting/SHCResources.qrc \
