#include "SHCOutlierDefWidget.hpp"

SHCOutlierDefWidget::SHCOutlierDefWidget(string *name) {
    this->name=name;
    ui.setupUi(this);
    ui.box->setTitle(QString::fromStdString(*name));
    connect(ui.removeDefinition,SIGNAL(clicked()),this,SLOT(removeMe()));
}

SHCOutlierDefWidget::~SHCOutlierDefWidget() {
    delete name;
}

string *SHCOutlierDefWidget::getName() {
    return this->name;
}

void SHCOutlierDefWidget::removeMe() {
    removeListItem(name);
}

Ui::outlierDef *SHCOutlierDefWidget::getUi() {
    return &ui;
}
