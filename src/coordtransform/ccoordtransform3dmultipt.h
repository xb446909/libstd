#pragma once
#include "ccoordtransform.h"
#include <opencv2/core.hpp>

class CCoordTransform3DMultiPt :
	public CCoordTransform
{
public:
	CCoordTransform3DMultiPt();
	virtual ~CCoordTransform3DMultiPt();
	virtual int SetTransPoints(std::vector<cv::Point3d> src, std::vector<cv::Point3d> dst);
	virtual int TransformPoint(cv::Point3d src, cv::Point3d& dst);

private:
	bool runRANSAC(const cv::Mat& m1, const cv::Mat& m2, cv::Mat& model, cv::Mat& mask0,
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



	cv::Mat m_mat;
	int modelPoints;
	bool checkPartialSubsets;
	CvRNG rng;
};

