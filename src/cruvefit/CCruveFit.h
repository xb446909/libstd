#ifndef __CCRUVEFIT_H
#define __CCRUVEFIT_H

#include <vector>
#include <opencv2/core.hpp>


class CCruveFit
{
public:
	CCruveFit();
	~CCruveFit();
	void SetPoints(std::vector<cv::Point2d> points);
	void GetAnswer(double* answer, int n);

private:
	std::vector<cv::Point2d> m_points;
};

#endif // !__CCRUVEFIT_H
