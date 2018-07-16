#ifndef __COORDTRANSFORM_H
#define __COORDTRANSFORM_H

#ifdef __GNUC__
#ifndef __stdcall
#define __stdcall
#endif
#endif

#define TRANSFORM_ERROR			(-1)
#define TRANSFORM_SUCCESS		(0)

#define TRANSFORM_TYPE_3D_MULTI_POINT		0

int __stdcall InitTransform(int nID, int nType);
int __stdcall SetPoints(int nID, double* srcX, double* srcY, double* srcZ,
		double* dstX, double* dstY, double* dstZ, int nSize);
int __stdcall Transform(int nID, double srcX, double srcY, double srcZ,
		double* dstX, double* dstY, double* dstZ);

#endif // !__COORDTRANSFORM_H

