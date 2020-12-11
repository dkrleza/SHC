#ifndef SHCLinMovingClusterDefWidget_hpp
#define SHCLinMovingClusterDefWidget_hpp

#include <QtWidgets/QWidget>
#include "ui_linClusterMovementDef.h"
using namespace std;

class SHCLinMovingClusterDefWidget : public QWidget {
Q_OBJECT
private:
    Ui::linClusterMovementDef ui;
    string *name=NULL;
signals:
    void removeListItem(string *name);
public:
    SHCLinMovingClusterDefWidget(string *name);
    ~SHCLinMovingClusterDefWidget();
    string *getName();
    Ui::linClusterMovementDef *getUi();
public slots:
    void removeMe();
};

#endif /* SHCLinMovingClusterDefWidget_hpp */
