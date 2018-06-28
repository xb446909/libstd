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
	bool runRANSAC(cv::InputArray _m1, cv::InputArray _m2, cv::OutputArray _model, cv::OutputArray _mask,
		double reprojThreshold = 3.0, double confidence = 0.99, int maxIters = 2000);

	bool getSubset(const cv::Mat& m1, const cv::Mat& m2,
		cv::Mat& ms1, cv::Mat& ms2, cv::RNG& rng,
		int maxAttempts = 1000);

	int runKernel(const cv::Mat& m1, 
		const cv::Mat& m2, 
		cv::Mat& model);

	int RANSACUpdateNumIters(double p, double ep,
		int model_points, int max_iters);

	int findInliers(const cv::Mat& m1,
		const cv::Mat& m2,
		const cv::Mat& model,
		cv::Mat& _err,
		cv::Mat& _mask, double threshold);

	void computeReprojError(
		const cv::Mat& m1,
		const cv::Mat& m2,
		const cv::Mat& model,
		cv::Mat& error);

	bool checkSubset(
		const cv::Mat& ms1, const cv::Mat& ms2, int count);



	cv::Mat m_mat;
	int modelPoints;
	bool checkPartialSubsets;
};

