#include "CCruveFit.h"


CCruveFit::CCruveFit()
{
}


CCruveFit::~CCruveFit()
{
}

void CCruveFit::SetPoints(std::vector<cv::Point2d> points)
{
	m_points = points;
}

void CCruveFit::GetAnswer(double * answer, int n)
{
	size_t len = m_points.size();

	//构造矩阵X
	cv::Mat X = cv::Mat::zeros(n + 1, n + 1, CV_64FC1);
	for (int i = 0; i < n + 1; i++)
	{
		for (int j = 0; j < n + 1; j++)
		{
			for (int k = 0; k < len; k++)
			{
				X.at<double>(i, j) = X.at<double>(i, j) +
					std::pow(m_points[k].x, i + j);
			}
		}
	}

	//构造矩阵Y
	cv::Mat Y = cv::Mat::zeros(n + 1, 1, CV_64FC1);
	for (int i = 0; i < n + 1; i++)
	{
		for (int k = 0; k < len; k++)
		{
			Y.at<double>(i, 0) = Y.at<double>(i, 0) +
				std::pow(m_points[k].x, i) * m_points[k].y;
		}
	}

	cv::Mat A = cv::Mat::zeros(n + 1, 1, CV_64FC1);
	//求解矩阵A
	cv::solve(X, Y, A, cv::DECOMP_LU);

	memset(answer, 0, sizeof(double) * (n + 1));

	for (int i = 0; i < n + 1; i++)
	{
		answer[i] = A.at<double>(i, 0);
	}
}
