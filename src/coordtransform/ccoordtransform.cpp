#include "stdafx.h"
#include "ccoordtransform.h"
#include "coordtransform.h"
#include "ccoordtransform3dmultipt.h"

coord_transform_ptr CCoordTransform::Create(int nType)
{
	coord_transform_ptr ptr;
	switch (nType)
	{
	case TRANSFORM_TYPE_3D_MULTI_POINT:
		ptr = boost::make_shared<CCoordTransform3DMultiPt>();
		break;
	default:
		break;
	}
	return ptr;
}

CCoordTransform::CCoordTransform()
{
}


CCoordTransform::~CCoordTransform()
{
}
