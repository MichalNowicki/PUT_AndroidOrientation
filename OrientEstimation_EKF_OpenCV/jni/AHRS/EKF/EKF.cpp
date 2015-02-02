/**
 * @author Michal Nowicki michal.nowicki@put.poznan.pl
 *
 */
#include "EKF.h"

EKF::EKF(float Qq, float Qw, float Rr) {

	// Updating covariances based on init values
	this->Q = cv::Mat::eye(7, 7, CV_32F);
	for (int i = 0; i < 4; i++)
		this->Q.at<float>(i, i) = Qq;
	for (int i = 4; i < 7; i++)
		this->Q.at<float>(i, i) = Qw;

	this->R = cv::Mat::eye(4, 4, CV_32F) * Rr;

	this->x_apriori = cv::Mat::zeros(7, 1, CV_32F);
	this->x_aposteriori = cv::Mat::zeros(7, 1, CV_32F);


	this->Q.copyTo(this->P_apriori);
	this->Q.copyTo(this->P_aposteriori);

	this->H = cv::Mat::zeros(4, 7, CV_32F);
	for (int i = 0; i < 4; i++)
		this->H.at<float>(i, i) = 1.0;

	firstMeasurement = true;
	correctTime = true;
}

cv::Mat EKF::jacobian(float * w, float dt) {
	cv::Mat F = cv::Mat::zeros(7, 7, CV_32F);

	// 1st row
	F.at<float>(0, 0) = 1.0;
	F.at<float>(0, 1) = -0.5 * (w[0] - this->x_apriori.at<float>(4)) * dt;
	F.at<float>(0, 2) = -0.5 * (w[1] - this->x_apriori.at<float>(5)) * dt;
	F.at<float>(0, 3) = -0.5 * (w[2] - this->x_apriori.at<float>(6)) * dt;
	F.at<float>(0, 4) = 0.5 * dt * this->x_apriori.at<float>(1);
	F.at<float>(0, 5) = 0.5 * dt * this->x_apriori.at<float>(2);
	F.at<float>(0, 6) = 0.5 * dt * this->x_apriori.at<float>(3);

	// 2nd row
	F.at<float>(1, 0) = 0.5 * (w[0] - this->x_apriori.at<float>(4)) * dt;
	F.at<float>(1, 1) = 1;
	F.at<float>(1, 2) = 0.5 * (w[2] - this->x_apriori.at<float>(6)) * dt;
	F.at<float>(1, 3) = -0.5 * (w[1] - this->x_apriori.at<float>(5)) * dt;
	F.at<float>(1, 4) = -0.5 * dt * this->x_apriori.at<float>(0);
	F.at<float>(1, 5) = 0.5 * dt * this->x_apriori.at<float>(3);
	F.at<float>(1, 6) = -0.5 * dt * this->x_apriori.at<float>(2);

	// 3rd row
	F.at<float>(2, 0) = 0.5 * (w[1] - this->x_apriori.at<float>(5)) * dt;
	F.at<float>(2, 1) = -0.5 * (w[2] - this->x_apriori.at<float>(6)) * dt;
	F.at<float>(2, 2) = 1;
	F.at<float>(2, 3) = 0.5 * (w[0] - this->x_apriori.at<float>(4)) * dt;
	F.at<float>(2, 4) = -0.5 * dt * this->x_apriori.at<float>(3);
	F.at<float>(2, 5) = -0.5 * dt * this->x_apriori.at<float>(0);
	F.at<float>(2, 6) = 0.5 * dt * this->x_apriori.at<float>(1);

	// 4th row
	F.at<float>(3, 0) = 0.5 * (w[2] - this->x_apriori.at<float>(6)) * dt;
	F.at<float>(3, 1) = 0.5 * (w[1] - this->x_apriori.at<float>(5)) * dt;
	F.at<float>(3, 2) = -0.5 * (w[0] - this->x_apriori.at<float>(4)) * dt;
	F.at<float>(3, 3) = 1;
	F.at<float>(3, 4) = 0.5 * dt * this->x_apriori.at<float>(2);
	F.at<float>(3, 5) = -0.5 * dt * this->x_apriori.at<float>(1);
	F.at<float>(3, 6) = -0.5 * dt * this->x_apriori.at<float>(0);

	// 5th row
	F.at<float>(4, 4) = 1.0;

	// 6th row
	F.at<float>(5, 5) = 1.0;

	// 7th row
	F.at<float>(6, 6) = 1.0;

	return F;
}

cv::Mat EKF::state(float * w, float dt) {

	cv::Mat F = cv::Mat::zeros(7, 1, CV_32F);

	F.at<float>(4) = this->x_aposteriori.at<float>(4);
	F.at<float>(5) = this->x_aposteriori.at<float>(5);
	F.at<float>(6) = this->x_aposteriori.at<float>(6);

	cv::Mat A = cv::Mat::zeros(4, 4, CV_32F);
	// A 1st row
	A.at<float>(0, 0) = 1.0;
	A.at<float>(0, 1) = -0.5 * (w[0] - F.at<float>(4)) * dt;
	A.at<float>(0, 2) = -0.5 * (w[1] - F.at<float>(5)) * dt;
	A.at<float>(0, 3) = -0.5 * (w[2] - F.at<float>(6)) * dt;

	// A 2nd row
	A.at<float>(1, 0) = 0.5 * (w[0] - F.at<float>(4)) * dt;
	A.at<float>(1, 1) = 1;
	A.at<float>(1, 2) = 0.5 * (w[2] - F.at<float>(6)) * dt;
	A.at<float>(1, 3) = -0.5 * (w[1] - F.at<float>(5)) * dt;

	// A 3rd row
	A.at<float>(2, 0) = 0.5 * (w[1] - F.at<float>(5)) * dt;
	A.at<float>(2, 1) = -0.5 * (w[2] - F.at<float>(6)) * dt;
	A.at<float>(2, 2) = 1;
	A.at<float>(2, 3) = 0.5 * (w[0] - F.at<float>(4)) * dt;

	// A 4th row
	A.at<float>(3, 0) = 0.5 * (w[2] - F.at<float>(6)) * dt;
	A.at<float>(3, 1) = 0.5 * (w[1] - F.at<float>(5)) * dt;
	A.at<float>(3, 2) = -0.5 * (w[0] - F.at<float>(4)) * dt;
	A.at<float>(3, 3) = 1;

	// Only (1:4)
	cv::Mat x(this->x_aposteriori, cv::Rect(0, 0, 1, 4));
	x = A * x;

	for (int i = 0; i < 4; i++)
		F.at<float>(i) = x.at<float>(i);

	return F;
}

void EKF::predict(float * inputArray, float _dt, float *currentEstimate) {
if (!correctTime) {
		this->x_apriori = this->state(inputArray, _dt);
		cv::Mat F = this->jacobian(inputArray, _dt);
		this->P_apriori = F * this->P_aposteriori * F.t() + this->Q;
		correctTime = true;
	}
	for (int i = 0; i < 4; i++)
		currentEstimate[i] = this->x_aposteriori.at<float>(i);

}


void EKF::correct(float * addrZ, float *currentEstimate) {

	// Converting measurement
	cv::Mat z = cv::Mat::eye(4, 1, CV_32F);
	for (int i = 0; i < 4; i++)
		z.at<float>(i) = addrZ[i];

	if (firstMeasurement) {
		firstMeasurement = false;
		correctTime = false;
		for (int i = 0; i < 4; i++) {
			this->x_aposteriori.at<float>(i) = z.at<float>(i);
		}
	} else if (correctTime) {
		correctTime = false;

		// Additional variables
		cv::Mat I = cv::Mat::eye(7, 7, CV_32F);
		cv::Mat K = cv::Mat::zeros(7, 4, CV_32F);

		K = (this->P_apriori * this->H.t())
				* (this->H * this->P_apriori * this->H.t() + this->R).inv();
		this->x_aposteriori = this->x_apriori
				+ K * (z - this->H * this->x_apriori);
		this->P_aposteriori = (I - K * this->H) * this->P_apriori;
	}
	for (int i = 0; i < 4; i++)
		currentEstimate[i] = this->x_aposteriori.at<float>(i);
}

