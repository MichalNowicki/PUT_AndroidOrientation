#include "EKF.h"


EKF::EKF(float _Q, float _R, float _dt)
{
	this->Q = cv::Mat::eye(7, 7, CV_32F) * _Q;
	this->R = cv::Mat::eye(4, 4, CV_32F) * _R;

	this->homogra =  cv::Mat::zeros(3, 1, CV_32F);
	this->w = cv::Mat::zeros(3, 1, CV_32F);
	this->I = cv::Mat::eye(7, 7, CV_32F);
	this->K = cv::Mat::zeros(7,4,CV_32F);
	this->dt = _dt;
	this->x_apriori = cv::Mat::zeros(7,1,CV_32F);
	this->x_aposteriori = cv::Mat::zeros(7,1,CV_32F);
	this->P_apriori = cv::Mat::zeros(7,7,CV_32F);
	this->P_aposteriori = cv::Mat::zeros(7,7,CV_32F);

	this->H = cv::Mat::zeros(4, 7, CV_32F);
	this->H.at<float>(0,0) = 1.0;
	this->H.at<float>(1,1) = 1.0;
	this->H.at<float>(2,2) = 1.0;
	this->H.at<float>(3,3) = 1.0;
}

cv::Mat EKF::jacobian(long addrW) {
	cv::Mat &w = *(cv::Mat *) addrW;
	cv::Mat F = cv::Mat::zeros(7, 7, CV_32F);

	// 1st row
	F.at<float>(0, 0) = 1.0;
	F.at<float>(0, 1) = - 0.5 * (w.at<float>(0) - this->x_apriori.at<float>(4))
			* this->dt;
	F.at<float>(0, 2) = - 0.5 * (w.at<float>(1) - this->x_apriori.at<float>(5)) * this->dt;
	F.at<float>(0, 3) = - 0.5 * (w.at<float>(2) - this->x_apriori.at<float>(6)) * this->dt;
	F.at<float>(0, 4) = 0.5 * this->dt * this->x_apriori.at<float>(1);
	F.at<float>(0, 5) = 0.5 * this->dt * this->x_apriori.at<float>(2);
	F.at<float>(0, 6) = 0.5 * this->dt * this->x_apriori.at<float>(3);

	// 2nd row
	F.at<float>(1, 0) = 0.5 * (w.at<float>(0) - this->x_apriori.at<float>(4)) * this->dt;
	F.at<float>(1, 1) = 1;
	F.at<float>(1, 2) = 0.5 * (w.at<float>(2) - this->x_apriori.at<float>(6)) * this->dt;
	F.at<float>(1, 3) = - 0.5 * (w.at<float>(1) - this->x_apriori.at<float>(5)) * this->dt;
	F.at<float>(1, 4) = - 0.5 * this->dt * this->x_apriori.at<float>(0);
	F.at<float>(1, 5) =  0.5 * this->dt * this->x_apriori.at<float>(3);
	F.at<float>(1, 6) = - 0.5 * this->dt * this->x_apriori.at<float>(2);

	// 3rd row
	F.at<float>(2, 0) = 0.5 * (w.at<float>(1) - this->x_apriori.at<float>(5)) * this->dt;
	F.at<float>(2, 1) = - 0.5 * (w.at<float>(2) - this->x_apriori.at<float>(6)) * this->dt;
	F.at<float>(2, 2) = 1;
	F.at<float>(2, 3) = 0.5 * (w.at<float>(0) - this->x_apriori.at<float>(4)) * this->dt;
	F.at<float>(2, 4) = - 0.5 * this->dt * this->x_apriori.at<float>(3);
	F.at<float>(2, 5) = - 0.5 * this->dt * this->x_apriori.at<float>(0);
	F.at<float>(2, 6) = 0.5 * this->dt * this->x_apriori.at<float>(1);

	// 4th row
	F.at<float>(3, 0) = 0.5 * (w.at<float>(2) - this->x_apriori.at<float>(6)) * this->dt;
	F.at<float>(3, 1) =	0.5 * (w.at<float>(1) - this->x_apriori.at<float>(5)) * this->dt;
	F.at<float>(3, 2) =	- 0.5 * (w.at<float>(0) - this->x_apriori.at<float>(4)) * this->dt;
	F.at<float>(3, 3) = 1;
	F.at<float>(3, 4) = 0.5 * this->dt * this->x_apriori.at<float>(2);
	F.at<float>(3, 5) = - 0.5 * this->dt * this->x_apriori.at<float>(1);
	F.at<float>(3, 6) = - 0.5 * this->dt * this->x_apriori.at<float>(0);

	// 5th row
	F.at<float>(4,4) = 1.0;

	// 6th row
	F.at<float>(5,5) = 1.0;

	// 7th row
	F.at<float>(6,6) = 1.0;

	return F;
}

cv::Mat EKF::state(long addrW) {
	cv::Mat &w = *(cv::Mat *) addrW;
	cv::Mat F = cv::Mat::zeros(7, 1, CV_32F);

	F.at<float>(4) = this->x_aposteriori.at<float>(4);
	F.at<float>(5) = this->x_aposteriori.at<float>(5);
	F.at<float>(6) = this->x_aposteriori.at<float>(6);

	cv::Mat A = cv::Mat::zeros(4, 4, CV_32F);
	// A 1st row
	A.at<float>(0, 0) = 1.0;
	A.at<float>(0, 1) = -0.5 * (w.at<float>(2) - F.at<float>(6)) * this->dt;
	A.at<float>(0, 2) = 0.5 * (w.at<float>(1) - F.at<float>(5)) * this->dt;
	A.at<float>(0, 3) = 0.5 * (w.at<float>(0) - F.at<float>(4)) * this->dt;

	// A 2nd row
	A.at<float>(1, 0) = 0.5 * (w.at<float>(2) - F.at<float>(6)) * this->dt;
	A.at<float>(1, 1) = 1;
	A.at<float>(1, 2) = -0.5 * (w.at<float>(0) - F.at<float>(4)) * this->dt;
	A.at<float>(1, 3) = 0.5 * (w.at<float>(1) - F.at<float>(5)) * this->dt;

	// A 3rd row
	A.at<float>(2, 0) = -0.5 * (w.at<float>(1) - F.at<float>(5)) * this->dt;
	A.at<float>(2, 1) = 0.5 * (w.at<float>(0) - F.at<float>(4)) * this->dt;
	A.at<float>(2, 2) = 1;
	A.at<float>(2, 3) = 0.5 * (w.at<float>(2) - F.at<float>(6)) * this->dt;

	// A 4th row
	A.at<float>(3, 0) = -0.5 * (w.at<float>(0) - F.at<float>(4)) * this->dt;
	A.at<float>(3, 1) = -0.5 * (w.at<float>(1) - F.at<float>(5)) * this->dt;
	A.at<float>(3, 2) = -0.5 * (w.at<float>(2) - F.at<float>(6)) * this->dt;
	A.at<float>(3, 3) = 1;


	// Only (1:4)
	cv::Mat x(this->x_aposteriori, cv::Rect(0,0,1,4));
	x = A * x;

	F.at<float>(0) = x.at<float>(0);
	F.at<float>(1) = x.at<float>(1);
	F.at<float>(2) = x.at<float>(2);
	F.at<float>(3) = x.at<float>(3);

	return F;
}


void EKF::predict(long addrW, float _dt) {
	this->dt = _dt;
	this->x_apriori = this->state(addrW);
	cv::Mat F = this->jacobian(addrW);
	this->P_apriori = F * this->P_aposteriori * F.t() + this->Q;
};

void EKF::correct(long addrZ) {
	cv::Mat &z = *(cv::Mat *)addrZ;
	this->K = (this->P_apriori * this->H.t())
			* (this->H * this->P_apriori * this->H.t() + this->R).inv();
	this->x_aposteriori = this->x_apriori
			+ this->K * (z - this->H * this->x_apriori);
	this->P_aposteriori = (this->I - this->K * this->H) * this->P_apriori;
};
