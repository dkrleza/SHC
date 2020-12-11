#ifndef SHCCircMovingClusterDefWidget_hpp
#define SHCCircMovingClusterDefWidget_hpp

#include <QtWidgets/QWidget>
#include "ui_circClusterMovementDef.h"
using namespace std;

class SHCCircMovingClusterDefWidget : public QWidget {
Q_OBJECT
private:
    Ui::circClusterMovementDef ui;
    string *name=NULL;
signals:
    void removeListItem(string *name);
public:
    SHCCircMovingClusterDefWidget(string *name);
    ~SHCCircMovingClusterDefWidget();
    string *getName();
    Ui::circClusterMovementDef *getUi();
public slots:
    void removeMe();
};

#endif /* SHCCircMovingClusterDefWidget_hpp */
