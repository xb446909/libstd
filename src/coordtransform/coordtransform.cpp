#include "stdafx.h"
#include "coordtransform.h"
#include "ccoordtransform.h"
#include <map>

std::map<int, coord_transform_ptr> g_mapTransforms;

int __stdcall InitTransform(int nID, int nType)
{
	g_mapTransforms.erase(nID);
	g_mapTransforms.insert(std::pair<int, coord_transform_ptr>(nID, CCoordTransform::Create(nType)));
	return 0;
}

int __stdcall SetPoints(int nID, double * srcX, double * srcY, double * srcZ, double * dstX, double * dstY, double * dstZ, int nSize)
{
	std::vector<boost::numeric::ublas::vector<double> > srcVec, dstVec;
	for (int i = 0; i < nSize; i++)
	{
		boost::numeric::ublas::vector<double> src(3);
		boost::numeric::ublas::vector<double> dst(3);
		src[0] = srcX[i];
		src[1] = srcY[i];
		src[2] = srcZ[i];
		dst[0] = dstX[i];
		dst[1] = dstY[i];
		dst[2] = dstZ[i];
		srcVec.push_back(src);
		dstVec.push_back(dst);
	}

	return g_mapTransforms[nID]->SetTransPoints(srcVec, dstVec);
}

int __stdcall Transform(int nID, double srcX, double srcY, double srcZ, double * dstX, double * dstY, double * dstZ)
{
	return 0;
}
