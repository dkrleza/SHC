#include "SHCLinMovingClusterDefWidget.hpp"
using namespace std;

SHCLinMovingClusterDefWidget::SHCLinMovingClusterDefWidget(string *name) {
    this->name=name;
    ui.setupUi(this);
    ui.box->setTitle(QString::fromStdString(*name));
    connect(ui.removeDefinition,SIGNAL(clicked()),this,SLOT(removeMe()));
}

SHCLinMovingClusterDefWidget::~SHCLinMovingClusterDefWidget() {
    delete name;
}

string *SHCLinMovingClusterDefWidget::getName() {
    return this->name;
}

void SHCLinMovingClusterDefWidget::removeMe() {
    removeListItem(name);
}

Ui::linClusterMovementDef *SHCLinMovingClusterDefWidget::getUi() {
    return &ui;
}
