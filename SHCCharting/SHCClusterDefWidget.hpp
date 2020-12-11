#ifndef SHCClusterDefWidget_hpp
#define SHCClusterDefWidget_hpp

#include <QtWidgets/QWidget>
#include "ui_clusterDef.h"
using namespace std;

class SHCClusterDefWidget : public QWidget {
Q_OBJECT
private:
    Ui::clusterDef ui;
    string *name=NULL;
signals:
    void removeListItem(string *name);
public:
    SHCClusterDefWidget(string *name);
    ~SHCClusterDefWidget();
    string *getName();
    Ui::clusterDef *getUi();
public slots:
    void removeMe();
};

#endif /* SHCClusterDefWidget_hpp */
