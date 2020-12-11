#include "SHCCircMovingClusterDefWidget.hpp"
using namespace std;

SHCCircMovingClusterDefWidget::SHCCircMovingClusterDefWidget(string *name) {
    this->name=name;
    ui.setupUi(this);
    ui.box->setTitle(QString::fromStdString(*name));
    connect(ui.removeDefinition,SIGNAL(clicked()),this,SLOT(removeMe()));
}

SHCCircMovingClusterDefWidget::~SHCCircMovingClusterDefWidget() {
    delete name;
}

string *SHCCircMovingClusterDefWidget::getName() {
    return this->name;
}

void SHCCircMovingClusterDefWidget::removeMe() {
    removeListItem(name);
}

Ui::circClusterMovementDef *SHCCircMovingClusterDefWidget::getUi() {
    return &ui;
}
