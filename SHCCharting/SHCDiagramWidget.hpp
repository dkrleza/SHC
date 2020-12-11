#ifndef SHCDiagramWidget_hpp
#define SHCDiagramWidget_hpp

#include <QtWidgets/QWidget>
#include "ui_diagram.h"
#include "ui_gaussTest.h"
#include "GaussDataSource.hpp"
#include <chrono>

class SHCDiagramWidget : public QWidget {
Q_OBJECT
private:
    Ui::diagramWidget ui;
    int page=1,totalPages,csp=0;
    void closeEvent(QCloseEvent *event);
    void adjustButtons();
    void printCurrentPage();
    void clusterMapping(unordered_map<string, QColor> &cluster_color_mapping, SHC *shc_classifier);
    void mergeSlices(vector<MatrixXd *> *all_slices, vector<MatrixXd *> *slice);
    void createChart(const unordered_map<string, QColor> &cluster_color_mapping, Ui::gaussTest *params_widget,
                     const vector<QColor> &series_color_mapping, SHC *shc_classifier, vector<MatrixXd *> *slice,
                     const string info, bool drawSI);
    void createChart(const unordered_map<string, QColor> &cluster_color_mapping, Ui::gaussTest *params_widget,
                     SHC *shc_classifier, MatrixXd *slice, const string info, bool drawSI, const int current_slice_pos=-1);
public:
    void extracted(unordered_map<std::string, QColor> &cluster_color_mapping, SHC *shc_classifier);
    
    void extracted(vector<Eigen::MatrixXd *> *all_slices, vector<Eigen::MatrixXd *> *slice);
    
    void extracted(const unordered_map<std::string, QColor> &cluster_color_mapping, Ui::gaussTest *params_widget, const vector<QColor> &series_color_mapping, SHC *shc_classifier, vector<Eigen::MatrixXd *> *slice);
    
    SHCDiagramWidget(Ui::gaussTest *params_widget);
    ~SHCDiagramWidget();
signals:
    void closed();
public slots:
    void nextPage();
    void previousPage();
    void toggleOriginalSeries(bool toggled);
    void toggleSHCModels(bool toggled);
    void toggleSI(bool toggled);
};

#endif /* SHCDiagramWidget_hpp */
