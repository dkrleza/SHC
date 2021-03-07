#include "SHCComponentTooltip.hpp"
#include "SHC/SHC_Utils.hpp"
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QStringListModel>

SHCComponentTooltip::SHCComponentTooltip(SHC_Component *comp) : QWidget() {
    ui.setupUi(this);
    stringstream ss;
    ss << "Component " << comp->getId();
    this->setWindowTitle(QString::fromStdString(ss.str()));
    VectorXd *mean=comp->getMean();
    ui.meanX->setText(QString::fromStdString(formatDouble((*mean)(0), 4)));
    ui.meanY->setText(QString::fromStdString(formatDouble((*mean)(1), 4)));
    MatrixXd *cov=comp->getCovariance();
    ui.covX->setText(QString::fromStdString(formatDouble((*cov)(0,0), 4)));
    ui.corrXY->setText(QString::fromStdString(formatDouble((*cov)(0,1), 4)));
    ui.corrYX->setText(QString::fromStdString(formatDouble((*cov)(1,0), 4)));
    ui.covY->setText(QString::fromStdString(formatDouble((*cov)(1,1), 4)));
    delete cov;
    ui.inversion->setChecked(comp->isCovarianceInverted());
    ui.inversion->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui.inversion->setFocusPolicy(Qt::NoFocus);
    ui.outlier->setChecked(comp->isOutlier());
    ui.outlier->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui.outlier->setFocusPolicy(Qt::NoFocus);
    ui.obsolete->setChecked(comp->isObsolete());
    ui.obsolete->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui.obsolete->setFocusPolicy(Qt::NoFocus);
    ui.blocked->setChecked(comp->isBlocked());
    ui.blocked->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui.blocked->setFocusPolicy(Qt::NoFocus);
    ui.elements->setText(QString::fromStdString(formatDouble((double)comp->getElements(), 0)));
    ui.redirectedTo->setText(QString::fromStdString(comp->isRedirected() ? comp->getRedirectedComponent()->getId() : "-----"));
    ui.parent->setText(QString::fromStdString(comp->getParent() ? comp->getParent()->getId() : "-----"));
    ui.sourceNode->setText(QString::fromStdString(comp->getSourceNode()));
    ui.decayEnabled->setChecked(comp->getDecayHandler());
    ui.decayEnabled->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui.decayEnabled->setFocusPolicy(Qt::NoFocus);
    if(comp->getDecayHandler()!=NULL) {
        ui.decayCounter->setText(QString::fromStdString(to_string(comp->getDecayHandler()->getCeilingCount())));
        ui.decayCurrent->setText(QString::fromStdString(to_string(comp->getDecayHandler()->getCurrentPos())));
    }
    QStringListModel *model=new QStringListModel();
    QStringList strlis;
    for(string trace_id:*comp->getTrace()) strlis.push_back(QString::fromStdString(trace_id));
    model->setStringList(strlis);
    ui.traceList->setModel(model);
}

SHCComponentTooltip::~SHCComponentTooltip()=default;
