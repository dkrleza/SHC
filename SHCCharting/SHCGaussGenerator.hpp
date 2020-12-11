#ifndef SHCGaussGenerator_hpp
#define SHCGaussGenerator_hpp

#include <QtWidgets/QWidget>
#include "ui_gaussTest.h"
#include "SHCDiagramWidget.hpp"

class SHCGaussGeneratorWindow : public QWidget {
Q_OBJECT
private:
    Ui::gaussTest ui;
    int definition=1;
    SHCDiagramWidget *chartView=NULL;
    bool isHavingMovingDefinition();
public:
    SHCGaussGeneratorWindow();
public slots:
    void generate();
    void closeChart();
    void addDefinitionWidget();
    void removeDefinitionWidget(string *name);
};

#endif /* SHCGaussGenerator_hpp */
