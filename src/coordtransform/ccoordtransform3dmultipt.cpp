#include "stdafx.h"
#include "ccoordtransform3dmultipt.h"
#include <algorithm>
#include <iostream>

using namespace cv;

CCoordTransform3DMultiPt::CCoordTransform3DMultiPt()
	: modelPoints(4)
	, checkPartialSubsets(true)
{
}


CCoordTransform3DMultiPt::~CCoordTransform3DMultiPt()
{
}

int CCoordTransform3DMultiPt::SetTransPoints(std::vector<cv::Point3d> src, std::vector<cv::Point3d> dst)
{
	if (src.size() != dst.size())
	{
		std::cerr << "Error size is not same, source points number: "
			<< src.size() << " destination points number: " << dst.size() << std::endl;
		return TRANSFORM_ERROR;
	}

	cv::Mat mask(1, src.size(), CV_8U);
	mask = cv::Scalar::all(1);

	return runRANSAC(src, dst, m_mat, mask) ? TRANSFORM_SUCCESS : TRANSFORM_ERROR;
}

int CCoordTransform3DMultiPt::TransformPoint(cv::Point3d src, cv::Point3d& dst)
{
	cv::Mat _src(4, 1, CV_64F);
	_src.at<double>(0, 0) = src.x;
	_src.at<double>(1, 0) = src.y;
	_src.at<double>(2, 0) = src.z;
	_src.at<double>(3, 0) = 1.0;

	cv::Mat _dst = m_mat * _src;
	dst.x = _dst.at<double>(0, 0);
	dst.y = _dst.at<double>(1, 0);
	dst.z = _dst.at<double>(2, 0);
	return TRANSFORM_SUCCESS;
}

bool CCoordTransform3DMultiPt::runRANSAC(InputArray _m1, InputArray _m2, OutputArray _model, OutputArray _mask,
	double reprojThreshold, double confidence, int maxIters)
{
	const double outlierRatio = 0.45;
	bool result = false;
	Mat from = _m1.getMat();
	Mat to = _m2.getMat();
	int count = from.checkVector(3);
	CV_Assert(count >= 0 && to.checkVector(3) == count);
	Mat m1, m2;
	from.convertTo(m1, CV_32F);
	to.convertTo(m2, CV_32F);
	m1 = m1.reshape(3, count);
	m2 = m2.reshape(3, count);

	Mat ms1, ms2, err, errf, model, bestModel, mask, mask0;

	int d2 = m2.channels() > 1 ? m2.channels() : m2.cols;
	int count2 = m2.checkVector(d2);
	double minMedian = DBL_MAX;

	RNG rng((uint64)-1);

	CV_Assert(confidence > 0 && confidence < 1);

	CV_Assert(count >= 0 && count2 == count);
	if (count < modelPoints)
		return false;

	if (_mask.needed())
	{
		_mask.create(count, 1, CV_8U, -1, true);
		mask0 = mask = _mask.getMat();
		CV_Assert((mask.cols == 1 || mask.rows == 1) && (int)mask.total() == count);
	}

	if (count == modelPoints)
	{
		if (runKernel(m1, m2, bestModel) <= 0)
			return false;
		bestModel.copyTo(_model);
		mask.setTo(Scalar::all(1));
		return true;
	}

	int iter, niters = RANSACUpdateNumIters(confidence, outlierRatio, modelPoints, maxIters);
	niters = MAX(niters, 3);

	for (iter = 0; iter < niters; iter++)
	{
		int i, nmodels;
		if (count > modelPoints)
		{
			bool found = getSubset(m1, m2, ms1, ms2, rng);
			if (!found)
			{
				if (iter == 0)
					return false;
				break;
			}
		}

		nmodels = runKernel(ms1, ms2, model);
		if (nmodels <= 0)
			continue;

		CV_Assert(model.rows % nmodels == 0);
		Size modelSize(model.cols, model.rows / nmodels);

		for (i = 0; i < nmodels; i++)
		{
			Mat model_i = model.rowRange(i*modelSize.height, (i + 1)*modelSize.height);
			computeReprojError(m1, m2, model_i, err);
			if (err.depth() != CV_32F)
				err.convertTo(errf, CV_32F);
			else
				errf = err;
			CV_Assert(errf.isContinuous() && errf.type() == CV_32F && (int)errf.total() == count);
			std::nth_element(errf.ptr<int>(), errf.ptr<int>() + count / 2, errf.ptr<int>() + count);
			double median = errf.at<float>(count / 2);

			if (median < minMedian)
			{
				minMedian = median;
				model_i.copyTo(bestModel);
			}
		}
	}

	if (minMedian < DBL_MAX)
	{
		double sigma = 2.5*1.4826*(1 + 5. / (count - modelPoints))*std::sqrt(minMedian);
		sigma = MAX(sigma, 0.001);

		count = findInliers(m1, m2, bestModel, err, mask, sigma);
		if (_mask.needed() && mask0.data != mask.data)
		{
			if (mask0.size() == mask.size())
				mask.copyTo(mask0);
			else
				transpose(mask, mask0);
		}
		bestModel.copyTo(_model);
		result = count >= modelPoints;
	}
	else
		_model.release();

	return result;
}


bool CCoordTransform3DMultiPt::getSubset(const Mat& m1, const Mat& m2,
	Mat& ms1, Mat& ms2, RNG& rng,
	int maxAttempts)
{
	cv::AutoBuffer<int> _idx(modelPoints);
	int* idx = _idx;
	int i = 0, j, k, iters = 0;
	int d1 = m1.channels() > 1 ? m1.channels() : m1.cols;
	int d2 = m2.channels() > 1 ? m2.channels() : m2.cols;
	int esz1 = (int)m1.elemSize1()*d1, esz2 = (int)m2.elemSize1()*d2;
	int count = m1.checkVector(d1), count2 = m2.checkVector(d2);
	const int *m1ptr = m1.ptr<int>(), *m2ptr = m2.ptr<int>();

	ms1.create(modelPoints, 1, CV_MAKETYPE(m1.depth(), d1));
	ms2.create(modelPoints, 1, CV_MAKETYPE(m2.depth(), d2));

	int *ms1ptr = ms1.ptr<int>(), *ms2ptr = ms2.ptr<int>();

	CV_Assert(count >= modelPoints && count == count2);
	CV_Assert((esz1 % sizeof(int)) == 0 && (esz2 % sizeof(int)) == 0);
	esz1 /= sizeof(int);
	esz2 /= sizeof(int);

	for (; iters < maxAttempts; iters++)
	{
		for (i = 0; i < modelPoints && iters < maxAttempts; )
		{
			int idx_i = 0;
			for (;;)
			{
				idx_i = idx[i] = rng.uniform(0, count);
				for (j = 0; j < i; j++)
					if (idx_i == idx[j])
						break;
				if (j == i)
					break;
			}
			for (k = 0; k < esz1; k++)
				ms1ptr[i*esz1 + k] = m1ptr[idx_i*esz1 + k];
			for (k = 0; k < esz2; k++)
				ms2ptr[i*esz2 + k] = m2ptr[idx_i*esz2 + k];
			if (checkPartialSubsets && !checkSubset(ms1, ms2, i + 1))
			{
				// we may have selected some bad points;
				// so, let's remove some of them randomly
				i = rng.uniform(0, i + 1);
				iters++;
				continue;
			}
			i++;
		}
		if (!checkPartialSubsets && i == modelPoints && !checkSubset(ms1, ms2, i))
			continue;
		break;
	}

	return i == modelPoints && iters < maxAttempts;
}

bool CCoordTransform3DMultiPt::checkSubset(const cv::Mat& ms1, const cv::Mat& ms2, int count)
{
	const float threshold = 0.996f;

	for (int inp = 1; inp <= 2; inp++)
	{
		int j, k, i = count - 1;
		const Mat* msi = inp == 1 ? &ms1 : &ms2;
		const Point3f* ptr = msi->ptr<Point3f>();

		CV_Assert(count <= msi->rows);

		// check that the i-th selected point does not belong
		// to a line connecting some previously selected points
		for (j = 0; j < i; ++j)
		{
			Point3f d1 = ptr[j] - ptr[i];
			float n1 = d1.x*d1.x + d1.y*d1.y;

			for (k = 0; k < j; ++k)
			{
				Point3f d2 = ptr[k] - ptr[i];
				float denom = (d2.x*d2.x + d2.y*d2.y)*n1;
				float num = d1.x*d2.x + d1.y*d2.y;

				if (num*num > threshold*threshold*denom)
					return false;
			}
		}
	}
	return true;
}


int CCoordTransform3DMultiPt::runKernel(
	const cv::Mat& m1, 
	const cv::Mat& m2, 
	cv::Mat& model)
{
	const Point3f* from = m1.ptr<Point3f>();
	const Point3f* to = m2.ptr<Point3f>();

	const int N = 12;
	double buf[N*N + N + N];
	Mat A(N, N, CV_64F, &buf[0]);
	Mat B(N, 1, CV_64F, &buf[0] + N * N);
	Mat X(N, 1, CV_64F, &buf[0] + N * N + N);
	double* Adata = A.ptr<double>();
	double* Bdata = B.ptr<double>();
	A = Scalar::all(0);

	for (int i = 0; i < (N / 3); i++)
	{
		Bdata[i * 3] = to[i].x;
		Bdata[i * 3 + 1] = to[i].y;
		Bdata[i * 3 + 2] = to[i].z;

		double *aptr = Adata + i * 3 * N;
		for (int k = 0; k < 3; ++k)
		{
			aptr[0] = from[i].x;
			aptr[1] = from[i].y;
			aptr[2] = from[i].z;
			aptr[3] = 1.0;
			aptr += 16;
		}
	}

	solve(A, B, X, DECOMP_SVD);
	X.reshape(1, 3).copyTo(model);

	return 1;
}


int CCoordTransform3DMultiPt::RANSACUpdateNumIters(double p, double ep,
	int model_points, int max_iters)
{
	if (modelPoints <= 0)
		std::cerr << "the number of model points should be positive" << std::endl;

	p = MAX(p, 0.);
	p = MIN(p, 1.);
	ep = MAX(ep, 0.);
	ep = MIN(ep, 1.);

	// avoid inf's & nan's
	double num = MAX(1. - p, DBL_MIN);
	double denom = 1. - std::pow(1. - ep, modelPoints);
	if (denom < DBL_MIN)
		return 0;

	num = std::log(num);
	denom = std::log(denom);

	return denom >= 0 || -num >= max_iters * (-denom) ? max_iters : cvRound(num / denom);
}


int CCoordTransform3DMultiPt::findInliers(const Mat& m1, const Mat& m2, const Mat& model, Mat& err, Mat& mask, double thresh)
{
	computeReprojError(m1, m2, model, err);
	mask.create(err.size(), CV_8U);

	CV_Assert(err.isContinuous() && err.type() == CV_32F && mask.isContinuous() && mask.type() == CV_8U);
	const float* errptr = err.ptr<float>();
	uchar* maskptr = mask.ptr<uchar>();
	float t = (float)(thresh*thresh);
	int i, n = (int)err.total(), nz = 0;
	for (i = 0; i < n; i++)
	{
		int f = errptr[i] <= t;
		maskptr[i] = (uchar)f;
		nz += f;
	}
	return nz;
}

void CCoordTransform3DMultiPt::computeReprojError(
	const cv::Mat& m1, 
	const cv::Mat& m2, 
	const cv::Mat& model, 
	cv::Mat& err)
{
	const Point3f* from = m1.ptr<Point3f>();
	const Point3f* to = m2.ptr<Point3f>();
	const double* F = model.ptr<double>();

	int count = m1.checkVector(3);
	CV_Assert(count > 0);

	float* errptr = err.ptr<float>();

	for (int i = 0; i < count; i++)
	{
		const Point3f& f = from[i];
		const Point3f& t = to[i];

		double a = F[0] * f.x + F[1] * f.y + F[2] * f.z + F[3] - t.x;
		double b = F[4] * f.x + F[5] * f.y + F[6] * f.z + F[7] - t.y;
		double c = F[8] * f.x + F[9] * f.y + F[10] * f.z + F[11] - t.z;

		errptr[i] = (float)(a*a + b * b + c * c);
	}
}
