#include "SHCDataSource.hpp"
#include <Eigen/Dense>
using namespace Eigen;

SHCDataSource::SHCDataSource() {}
SHCDataSource::~SHCDataSource() {}

SHCDataSourceException::SHCDataSourceException(const char *msg) {
    this->msg=msg;
}
const char *SHCDataSourceException::what() const throw() {
    return this->msg;
}
