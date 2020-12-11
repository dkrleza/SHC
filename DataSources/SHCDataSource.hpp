#ifndef SHCDataSource_hpp
#define SHCDataSource_hpp

#include <Eigen/Dense>
#include <string>
#include <random>
using namespace Eigen;
using namespace std;

class SHCDataSourceException : public exception {
private:
    const char *msg=NULL;
public:
    const char *what() const throw();
    SHCDataSourceException(const char *msg);
};

struct SHC_Generated {
    vector<MatrixXd*> *slice=NULL;
};

class SHCDataSource {
public:
    SHCDataSource();
    virtual ~SHCDataSource();
};

namespace Eigen {
namespace internal {
template<typename Scalar>
struct scalar_normal_dist_op
{
    static mt19937 rng;
    mutable normal_distribution<Scalar> norm;
    
    EIGEN_EMPTY_STRUCT_CTOR(scalar_normal_dist_op)
    
    template<typename Index>
    inline const Scalar operator() (Index, Index = 0) const { return norm(rng); }
    inline void seed(const uint64_t &s) { rng.seed((unsigned int)s); }
};

template<typename Scalar>
mt19937 scalar_normal_dist_op<Scalar>::rng;

template<typename Scalar> struct functor_traits<scalar_normal_dist_op<Scalar> >
{ enum { Cost = 50 * NumTraits<Scalar>::MulCost, PacketAccess = false, IsRepeatable = false }; };

}
template<typename Scalar> class EigenMultivariateNormal
{
    MatrixXd _covar;
    MatrixXd _transform;
    VectorXd _mean;
    internal::scalar_normal_dist_op<Scalar> randN;
    bool _use_cholesky;
    SelfAdjointEigenSolver<Matrix<Scalar,Dynamic,Dynamic>> _eigenSolver;
    
public:
    EigenMultivariateNormal(const VectorXd& mean,const MatrixXd& covar,
                            const bool use_cholesky=false,const uint64_t &seed=mt19937::default_seed):_use_cholesky(use_cholesky) {
        randN.seed(seed);
        setMean(mean);
        setCovar(covar);
    }
    
    void setMean(const Matrix<Scalar,Dynamic,1>& mean) { _mean = mean; }
    void setCovar(const Matrix<Scalar,Dynamic,Dynamic>& covar) {
        _covar = covar;
        
        if (_use_cholesky) {
            Eigen::LLT<Eigen::Matrix<Scalar,Dynamic,Dynamic> > cholSolver(_covar);
            
            if (cholSolver.info()==Eigen::Success) {
                _transform = cholSolver.matrixL();
            } else {
                throw std::runtime_error("Failed computing the Cholesky decomposition. Use solver instead");
            }
        } else {
            _eigenSolver = SelfAdjointEigenSolver<Matrix<Scalar,Dynamic,Dynamic> >(_covar);
            _transform = _eigenSolver.eigenvectors()*_eigenSolver.eigenvalues().cwiseMax(0).cwiseSqrt().asDiagonal();
        }
    }
    Matrix<Scalar,Dynamic,1> *getMean() { return &_mean; }
    Matrix<Scalar,Dynamic,Dynamic> *getCovar() { return &_covar; }
    
    MatrixXd samples(int nn) {
        return (_transform * Matrix<Scalar,Dynamic,-1>::NullaryExpr(_covar.rows(),nn,randN)).colwise() + _mean;
    }
};
}

#endif /* SHCDataSource_hpp */
