#include "stdafx.h"
#include "coordtransform.h"
#include "ccoordtransform.h"
#include <map>
#include <iostream>

std::map<int, coord_transform_ptr> g_mapTransforms;

int __stdcall InitTransform(int nID, int nType)
{
	g_mapTransforms.erase(nID);
	g_mapTransforms.insert(
			std::pair<int, coord_transform_ptr>(nID,
					CCoordTransform::Create(nType)));
	return 0;
}

int __stdcall SetPoints(int nID, double * srcX, double * srcY, double * srcZ,
		double * dstX, double * dstY, double * dstZ, int nSize)
{
	if (g_mapTransforms.find(nID) == g_mapTransforms.end())
	{
		std::cerr << "SetPoints g_mapTransforms.find(nID) ERR" << std::endl;
		return TRANSFORM_ERROR;
	}

	std::vector<cv::Point3d> srcVec, dstVec;
	for (int i = 0; i < nSize; i++)
	{
		cv::Point3d src;
		cv::Point3d dst;
		src.x = srcX[i];
		src.y = srcY[i];
		src.z = srcZ[i];
		dst.x = dstX[i];
		dst.y = dstY[i];
		dst.z = dstZ[i];
		srcVec.push_back(src);
		dstVec.push_back(dst);
	}

	return g_mapTransforms[nID]->SetTransPoints(srcVec, dstVec);
}

int __stdcall Transform(int nID, double srcX, double srcY, double srcZ,
		double * dstX, double * dstY, double * dstZ)
{
	if (g_mapTransforms.find(nID) == g_mapTransforms.end())
		return TRANSFORM_ERROR;

	cv::Point3d src;
	cv::Point3d dst;
	src.x = srcX;
	src.y = srcY;
	src.z = srcZ;

	int nRet = g_mapTransforms[nID]->TransformPoint(src, dst);
	if (nRet == TRANSFORM_SUCCESS)
	{
		*dstX = dst.x;
		*dstY = dst.y;
		*dstZ = dst.z;
	}
	return nRet;
}
