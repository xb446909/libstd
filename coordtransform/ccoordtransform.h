#ifndef __CCOORDTRANSFORM_H
#define __CCOORDTRANSFORM_H

#include "coordtransform.h"

#include <boost/make_shared.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/blas.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <vector>

class CCoordTransform;

typedef boost::shared_ptr<CCoordTransform> coord_transform_ptr;

class CCoordTransform
{
public:
	static coord_transform_ptr Create(int nType);
	CCoordTransform();
	virtual ~CCoordTransform();
	virtual int SetTransPoints(std::vector<boost::numeric::ublas::vector<double> > src, std::vector<boost::numeric::ublas::vector<double> > dst) = 0;
	virtual int TransformPoint(boost::numeric::ublas::vector<double> src, boost::numeric::ublas::vector<double>& dst) = 0;
};


#endif // !__CCOORDTRANSFORM_H



