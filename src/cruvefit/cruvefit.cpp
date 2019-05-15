#include "cruvefit.h"
#include "CCruveFit.h"
#include <memory>
#include <vector>

#if __cplusplus >= 201103L
using namespace std;
#else
using namespace std::tr1;
#endif

shared_ptr<CCruveFit> g_fit = shared_ptr<CCruveFit>(new CCruveFit());

int __stdcall SetFitPoints(double * srcX, double * srcY, int nSize)
{
	std::vector<cv::Point2d> points;
	for (int i = 0; i < nSize; i++)
	{
		points.push_back(cv::Point2d(srcX[i], srcY[i]));
	}
	g_fit->SetPoints(points);
	return 0;
}

int __stdcall GetFitAnswer(double * answer, int n)
{
	g_fit->GetAnswer(answer, n);
	return 0;
} 
