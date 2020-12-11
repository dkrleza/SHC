#ifndef SHCOutlierDefWidget_hpp
#define SHCOutlierDefWidget_hpp

#include <QtWidgets/QWidget>
#include "ui_outlierDef.h"
using namespace std;

class SHCOutlierDefWidget : public QWidget {
Q_OBJECT
private:
    Ui::outlierDef ui;
    string *name=NULL;
signals:
    void removeListItem(string *name);
public:
    SHCOutlierDefWidget(string *name);
    ~SHCOutlierDefWidget();
    string *getName();
    Ui::outlierDef *getUi();
public slots:
    void removeMe();
};

#endif /* SHCOutlierDefWidget_hpp */
