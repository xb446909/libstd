#include "stdafx.h"
#include "ccoordtransform3dmultipt.h"
#include <boost/math/special_functions/round.hpp>
#include <boost/random.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include "matrix.cpp"
#include <algorithm>

CCoordTransform3DMultiPt::CCoordTransform3DMultiPt()
	: modelPoints(4)
	, checkPartialSubsets(true)
{
}


CCoordTransform3DMultiPt::~CCoordTransform3DMultiPt()
{
}

int CCoordTransform3DMultiPt::SetTransPoints(std::vector<boost::numeric::ublas::vector<double> > src, std::vector<boost::numeric::ublas::vector<double> > dst)
{
	if (src.size() != dst.size())
	{
		std::cerr << "Error size is not same, source points number: "
			<< src.size() << " destination points number: " << dst.size() << std::endl;
		return TRANSFORM_ERROR;
	}
	for (size_t i = 0; i < src.size(); i++)
	{
		if ((src[i].size() != 3) || (dst[i].size() != 3))
		{
			std::cerr << "Error points dimension, points dimension must be 3 " << std::endl;
			return TRANSFORM_ERROR;
		}
	}

	boost::numeric::ublas::matrix<double> from(1, src.size() * 3);
	boost::numeric::ublas::matrix<double> to(1, src.size() * 3);
	boost::numeric::ublas::matrix<double> mask(1, src.size(), 1);

	int k = 0;
	for (size_t i = 0; i < src.size(); i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			from.insert_element(0, k, src[i][j]);
			to.insert_element(0, k, dst[i][j]);
			k++;
		}
	}
	return runRANSAC(from, to, m_mat, mask) ? TRANSFORM_SUCCESS : TRANSFORM_ERROR;
}

int CCoordTransform3DMultiPt::TransformPoint(boost::numeric::ublas::vector<double> src, boost::numeric::ublas::vector<double>& dst)
{
	if (src.size() != 3)
	{
		std::cerr << "Error points dimension, points dimension must be 3 " << std::endl;
		return TRANSFORM_ERROR;
	}

	boost::numeric::ublas::vector<double> _src(4);
	for (size_t i = 0; i < src.size(); i++)
	{
		_src[i] = src[i];
	}
	_src[3] = 1;

	dst = boost::numeric::ublas::prod(m_mat, _src);
	return TRANSFORM_SUCCESS;
}

bool CCoordTransform3DMultiPt::runRANSAC(
	const boost::numeric::ublas::matrix<double>& m1, 
	const boost::numeric::ublas::matrix<double>& m2, 
	boost::numeric::ublas::matrix<double>& model, 
	boost::numeric::ublas::matrix<double>& mask0, 
	double reprojThreshold, double confidence, int maxIters)
{
	bool result = false;
	boost::numeric::ublas::matrix<double> ms1, ms2;

	int iter, niters = maxIters;
	int count = m1.size1() * m1.size2() / 3, maxGoodCount = 0;

	boost::numeric::ublas::matrix<double> models(3, 4);
	boost::numeric::ublas::matrix<double> err(1, count);
	boost::numeric::ublas::matrix<unsigned char> tmask(1, count);
	boost::numeric::ublas::matrix<double> mask(mask0);

	if (count > modelPoints)
	{
		ms1 = boost::numeric::ublas::matrix<double>(1, modelPoints * 3);
		ms2 = boost::numeric::ublas::matrix<double>(1, modelPoints * 3);
	}
	else
	{
		niters = 1;
		ms1 = m1;
		ms2 = m2;
	}

	for (iter = 0; iter < niters; iter++)
	{
		int i, goodCount, nmodels;
		if (count > modelPoints)
		{
			bool found = getSubset(m1, m2, ms1, ms2, 300);
			if (!found)
			{
				if (iter == 0)
					return false;
				break;
			}
		}

		nmodels = runKernel(ms1, ms2, models);

		if (nmodels <= 0)
			continue;
		for (i = 0; i < nmodels; i++)
		{
			boost::numeric::ublas::matrix<double> model_i = 
				getRows(models, i * 3, (i + 1) * 3);

			goodCount = findInliers(m1, m2, model_i, err, tmask, reprojThreshold);

			if (goodCount > std::max<int>(maxGoodCount, modelPoints - 1))
			{
				boost::numeric::ublas::matrix<double> temp(tmask);
				tmask = mask;
				mask = temp;
				model = model_i;
				maxGoodCount = goodCount;
				niters = RANSACUpdateNumIters(confidence,
					(double)(count - goodCount) / count, modelPoints, niters);
			}
		}
	}

	if (maxGoodCount > 0)
	{
		mask0 = mask;
		result = true;
	}

	return result;
}


bool CCoordTransform3DMultiPt::getSubset(const boost::numeric::ublas::matrix<double>& m1,
	const boost::numeric::ublas::matrix<double>& m2,
	boost::numeric::ublas::matrix<double>& ms1,
	boost::numeric::ublas::matrix<double>& ms2, int maxAttempts)
{
	std::vector<int> idx(modelPoints, -1000);
	int i = 0, j, k, idx_i, iters = 0;
	const int *m1ptr = (int*)&m1.data()[0], *m2ptr = (int*)&m2.data()[0];
	int *ms1ptr = (int*)&ms1.data()[0], *ms2ptr = (int*)&ms2.data()[0];
	int count = m1.size1() * m1.size2() / 3;
	int elemSize = sizeof(double) * 3;
	elemSize /= sizeof(int);

	boost::mt19937 gen;
	boost::uniform_int<> dist(-1, INT_MAX);
	boost::variate_generator<boost::mt19937&, boost::uniform_int<> > die(gen, dist);

	int a[] = { 0, 4, 3, 1 };

	for (; iters < maxAttempts; iters++)
	{
		for (i = 0; i < modelPoints && iters < maxAttempts; )
		{
			idx[i] = idx_i =  die() % count;

			for (j = 0; j < i; j++)
				if (idx_i == idx[j])
				{
					break;
				}
			if (j < i)
			{
				continue;
			}
			for (k = 0; k < elemSize; k++)
			{
				ms1ptr[i*elemSize + k] = m1ptr[idx_i*elemSize + k];
				ms2ptr[i*elemSize + k] = m2ptr[idx_i*elemSize + k];
			}
			if (checkPartialSubsets && (!checkSubset(ms1, i + 1) || !checkSubset(ms2, i + 1)))
			{
				iters++;
				continue;
			}
			i++;
		}
		if (!checkPartialSubsets && i == modelPoints &&
			(!checkSubset(ms1, i) || !checkSubset(ms2, i)))
			continue;
		break;
	}


	return i == modelPoints && iters < maxAttempts;
}

bool CCoordTransform3DMultiPt::checkSubset(const boost::numeric::ublas::matrix<double>& ms1, int count)
{
	int j, k, i = count - 1;
	std::vector< boost::numeric::ublas::vector<double> > vecs;
	
	boost::numeric::ublas::vector<double> v(3);

	for (size_t index = 0; index < ms1.size1() * ms1.size2(); index += 3)
	{
		v[0] = ms1.data()[index];
		v[1] = ms1.data()[index + 1];
		v[2] = ms1.data()[index + 2];
		vecs.push_back(v);
	}

	// check that the i-th selected point does not belong
	// to a line connecting some previously selected points

	for (j = 0; j < i; ++j)
	{
		boost::numeric::ublas::vector<double> d1 = vecs[j] - vecs[i];
		double n1 = norm_2(d1);

		for (k = 0; k < j; ++k)
		{
			boost::numeric::ublas::vector<double> d2 = vecs[k] - vecs[i];
			double n = norm_2(d2) * n1;

			if (fabs(boost::numeric::ublas::inner_prod(d1, d2) / n) > 0.996)
				break;
		}
		if (k < j)
			break;
	}

	return j == i;
}

int CCoordTransform3DMultiPt::runKernel(
	const boost::numeric::ublas::matrix<double>& m1, 
	const boost::numeric::ublas::matrix<double>& m2, 
	boost::numeric::ublas::matrix<double>& model)
{
	std::vector< boost::numeric::ublas::vector<double> > vecsFrom;
	std::vector< boost::numeric::ublas::vector<double> > vecsTo;

	boost::numeric::ublas::vector<double> vf(3);
	boost::numeric::ublas::vector<double> vt(3);

	for (size_t index = 0; index < m1.size1() * m1.size2(); index += 3)
	{
		vf[0] = m1.data()[index];
		vt[0] = m2.data()[index];
		vf[1] = m1.data()[index + 1];
		vt[1] = m2.data()[index + 1];
		vf[2] = m1.data()[index + 2];
		vt[2] = m2.data()[index + 2];
		vecsFrom.push_back(vf);
		vecsTo.push_back(vt);
	}

	boost::numeric::ublas::matrix<double> A(12, 12, 0);
	boost::numeric::ublas::vector<double> B(12);

	for (int i = 0; i < modelPoints; ++i)
	{
		B[3 * i] = vecsTo[i][0];
		B[3 * i + 1] = vecsTo[i][1];
		B[3 * i + 2] = vecsTo[i][2];

		double *aptr = (double*)&A.data()[3 * i * A.size2()];
		for (int k = 0; k < 3; ++k)
		{
			aptr[3] = 1.0;
			aptr[0] = vecsFrom[i][0];
			aptr[1] = vecsFrom[i][1];
			aptr[2] = vecsFrom[i][2];
			aptr += 16;
		}
	}
	
	typedef hanssoft::matrix<double>       dMatrix;
	typedef std::valarray<double>          dVector;

	dMatrix dA(A.size1(), A.size2());
	dVector dB(B.size());
	dVector dX(B.size());

	for (size_t i = 0; i < A.size1(); i++)
	{
		for (size_t j = 0; j < A.size2(); j++)
		{
			dA[i][j] = A.at_element(i, j);
		}
	}
	for (size_t i = 0; i < B.size(); i++)
	{
		dB[i] = B[i];
	}

	dA.solve_sv(dB, dX);
	model = boost::numeric::ublas::matrix<double>(3, 4);
	for (size_t i = 0; i < dX.size(); i++)
	{
		model.data()[i] = dX[i];
	}
	return 1;
}


int CCoordTransform3DMultiPt::RANSACUpdateNumIters(double p, double ep,
	int model_points, int max_iters)
{
	if (model_points <= 0)
	{
		std::cerr << "the number of model points should be positive" << std::endl;
	}

	p = std::max<double>(p, 0.);
	p = std::min<double>(p, 1.);
	ep = std::max<double>(ep, 0.);
	ep = std::min<double>(ep, 1.);

	// avoid inf's & nan's
	double num = std::max<double>(1. - p, DBL_MIN);
	double denom = 1. - std::pow(1. - ep, model_points);
	if (denom < DBL_MIN)
		return 0;

	num = log(num);
	denom = log(denom);

	return denom >= 0 || -num >= max_iters * (-denom) ?
		max_iters : boost::math::round(num / denom);
}

boost::numeric::ublas::matrix<double> CCoordTransform3DMultiPt::getRows(
	boost::numeric::ublas::matrix<double> src, size_t start, size_t end)
{
	boost::numeric::ublas::matrix<double> mat(end - start, src.size2());
	for (size_t i = 0; i < end - start; i++)
	{
		for (size_t j = 0; j < src.size2(); j++)
		{
			mat.insert_element(i, j, src(start + i, j));
		}
	}
	return mat;
}


int CCoordTransform3DMultiPt::findInliers(const boost::numeric::ublas::matrix<double>& m1, 
	const boost::numeric::ublas::matrix<double>& m2,
	const boost::numeric::ublas::matrix<double>& model, 
	boost::numeric::ublas::matrix<double>& _err,
	boost::numeric::ublas::matrix<unsigned char>& _mask, double threshold)
{
	int i, count = _err.size1() * _err.size2(), goodCount = 0;
	double* err = (double*)&_err.data()[0];
	unsigned char* mask = (unsigned char*)&_mask.data()[0];

	computeReprojError(m1, m2, model, _err);
	threshold *= threshold;
	for (i = 0; i < count; i++)
		goodCount += mask[i] = err[i] <= threshold;
	return goodCount;
}

void CCoordTransform3DMultiPt::computeReprojError(
	const boost::numeric::ublas::matrix<double>& m1, 
	const boost::numeric::ublas::matrix<double>& m2, 
	const boost::numeric::ublas::matrix<double>& model, 
	boost::numeric::ublas::matrix<double>& error)
{
	int count = m1.size1() * m1.size2() / 3;

	std::vector< boost::numeric::ublas::vector<double> > vecsFrom;
	std::vector< boost::numeric::ublas::vector<double> > vecsTo;

	boost::numeric::ublas::vector<double> vf(3);
	boost::numeric::ublas::vector<double> vt(3);

	for (size_t index = 0; index < m1.size1() * m1.size2(); index += 3)
	{
		vf[0] = m1.data()[index];
		vt[0] = m2.data()[index];
		vf[1] = m1.data()[index + 1];
		vt[1] = m2.data()[index + 1];
		vf[2] = m1.data()[index + 2];
		vt[2] = m2.data()[index + 2];
		vecsFrom.push_back(vf);
		vecsTo.push_back(vt);
	}

	for (int i = 0; i < count; i++)
	{
		boost::numeric::ublas::vector<double> f = vecsFrom[i];
		boost::numeric::ublas::vector<double> t = vecsTo[i];

		double a = model(0, 0) * f[0] + model(0, 1) * f[1] + model(0, 2) * f[2] + model(0, 3) - t[0];
		double b = model(1, 0) * f[0] + model(1, 1) * f[1] + model(1, 2) * f[2] + model(1, 3) - t[1];
		double c = model(2, 0) * f[0] + model(2, 1) * f[1] + model(2, 2) * f[2] + model(2, 3) - t[2];

		error(0, i) = sqrt(a*a + b * b + c * c);
	}
}
