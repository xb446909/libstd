#ifndef __CCOORDTRANSFORM_H
#define __CCOORDTRANSFORM_H

#include "coordtransform.h"

#include <memory>
#include <vector>
#include <opencv2/core.hpp>

#if __cplusplus >= 201103L
using namespace std;
#else
using namespace std::tr1;
#endif

class CCoordTransform;

typedef shared_ptr<CCoordTransform> coord_transform_ptr;

class CCoordTransform
{
public:
	static coord_transform_ptr Create(int nType);
	CCoordTransform();
	virtual ~CCoordTransform();
	virtual int SetTransPoints(std::vector<cv::Point3d> src,
			std::vector<cv::Point3d> dst) = 0;
	virtual int TransformPoint(cv::Point3d src, cv::Point3d& dst) = 0;
};

#endif // !__CCOORDTRANSFORM_H

