#ifndef SHCComponentTooltip_hpp
#define SHCComponentTooltip_hpp

#include <QtWidgets/QWidget>
#include "ui_compTooltip.h"
#include "SHC/SHC_Component.hpp"

class SHCComponentTooltip : public QWidget {
Q_OBJECT
private:
    Ui::compTooltip ui;
public:
    SHCComponentTooltip(SHC_Component *comp);
    ~SHCComponentTooltip();
public slots:
};

#endif /* SHCComponentTooltip_hpp */
