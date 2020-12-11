#include "SHCClusterDefWidget.hpp"
using namespace std;

SHCClusterDefWidget::SHCClusterDefWidget(string *name) {
    this->name=name;
    ui.setupUi(this);
    ui.box->setTitle(QString::fromStdString(*name));
    connect(ui.removeDefinition,SIGNAL(clicked()),this,SLOT(removeMe()));
}

SHCClusterDefWidget::~SHCClusterDefWidget() {
    delete name;
}

string *SHCClusterDefWidget::getName() {
    return this->name;
}

void SHCClusterDefWidget::removeMe() {
    removeListItem(name);
}

Ui::clusterDef *SHCClusterDefWidget::getUi() {
    return &ui;
}
