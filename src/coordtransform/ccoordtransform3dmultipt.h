#pragma once
#include "ccoordtransform.h"

class CCoordTransform3DMultiPt :
	public CCoordTransform
{
public:
	CCoordTransform3DMultiPt();
	virtual ~CCoordTransform3DMultiPt();
	virtual int SetTransPoints(std::vector<boost::numeric::ublas::vector<double> > src, 
		std::vector<boost::numeric::ublas::vector<double> > dst);

private:
	bool runRANSAC(const boost::numeric::ublas::matrix<double>& m1,
		const boost::numeric::ublas::matrix<double>& m2,
		boost::numeric::ublas::matrix<double>& model,
		boost::numeric::ublas::matrix<double>& mask0, 
		double reprojThreshold = 3.0, double confidence = 0.99, int maxIters = 2000);

	bool getSubset(const boost::numeric::ublas::matrix<double>& m1,
		const boost::numeric::ublas::matrix<double>& m2,
		boost::numeric::ublas::matrix<double>& ms1,
		boost::numeric::ublas::matrix<double>& ms2, int maxAttempts);

	int runKernel(const boost::numeric::ublas::matrix<double>& m1, 
		const boost::numeric::ublas::matrix<double>& m2, 
		boost::numeric::ublas::matrix<double>& model);

	int RANSACUpdateNumIters(double p, double ep,
		int model_points, int max_iters);

	boost::numeric::ublas::matrix<double> getRows(
		boost::numeric::ublas::matrix<double> src, size_t start, size_t end);

	int findInliers(const boost::numeric::ublas::matrix<double>& m1,
		const boost::numeric::ublas::matrix<double>& m2,
		const boost::numeric::ublas::matrix<double>& model,
		boost::numeric::ublas::matrix<double>& _err,
		boost::numeric::ublas::matrix<unsigned char>& _mask, double threshold);

	void computeReprojError(
		const boost::numeric::ublas::matrix<double>& m1,
		const boost::numeric::ublas::matrix<double>& m2,
		const boost::numeric::ublas::matrix<double>& model,
		boost::numeric::ublas::matrix<double>& error);

	bool checkSubset(
		const boost::numeric::ublas::matrix<double>& ms1, int count);

	boost::numeric::ublas::matrix<double> m_mat;
	int modelPoints;
	bool checkPartialSubsets;
};

