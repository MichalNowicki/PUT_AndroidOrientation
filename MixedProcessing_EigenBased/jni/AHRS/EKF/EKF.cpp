#include "EKF.h"


EKF::EKF(float _Q, float _R, float _dt)
{

	this->I =  Eigen::Matrix<float, 7, 7>::Identity();

	this->Q = Eigen::Matrix<float, 7, 7>::Identity() * _Q;
	this->R = Eigen::Matrix<float, 4, 4>::Identity() * _R;

	//this->w.setZero();

	this->K.setZero();
	this->dt = _dt;

	this->x_apriori.setZero();
	this->x_aposteriori.setZero();
	this->P_apriori.setZero();
	this->P_aposteriori.setZero();

	this->H.setZero();
	this->H(0,0) = 1.0;
	this->H(1,1) = 1.0;
	this->H(2,2) = 1.0;
	this->H(3,3) = 1.0;
}


Eigen::Matrix<float, 7, 7> EKF::jacobian(float *wArray) {
	Eigen::Matrix<float, 7, 7> F;
	Eigen::Matrix<float, 3, 1> w;
	w << wArray[0], wArray[1], wArray[2];

	// 1st row
	F(0, 0) = 1.0;
	F(0, 1) = - 0.5 * (w(0) - this->x_apriori(4))
			* this->dt;
	F(0, 2) = - 0.5 * (w(1) - this->x_apriori(5)) * this->dt;
	F(0, 3) = - 0.5 * (w(2) - this->x_apriori(6)) * this->dt;
	F(0, 4) = 0.5 * this->dt * this->x_apriori(1);
	F(0, 5) = 0.5 * this->dt * this->x_apriori(2);
	F(0, 6) = 0.5 * this->dt * this->x_apriori(3);

	// 2nd row
	F(1, 0) = 0.5 * (w(0) - this->x_apriori(4)) * this->dt;
	F(1, 1) = 1;
	F(1, 2) = 0.5 * (w(2) - this->x_apriori(6)) * this->dt;
	F(1, 3) = - 0.5 * (w(1) - this->x_apriori(5)) * this->dt;
	F(1, 4) = - 0.5 * this->dt * this->x_apriori(0);
	F(1, 5) =  0.5 * this->dt * this->x_apriori(3);
	F(1, 6) = - 0.5 * this->dt * this->x_apriori(2);

	// 3rd row
	F(2, 0) = 0.5 * (w(1) - this->x_apriori(5)) * this->dt;
	F(2, 1) = - 0.5 * (w(2) - this->x_apriori(6)) * this->dt;
	F(2, 2) = 1;
	F(2, 3) = 0.5 * (w(0) - this->x_apriori(4)) * this->dt;
	F(2, 4) = - 0.5 * this->dt * this->x_apriori(3);
	F(2, 5) = - 0.5 * this->dt * this->x_apriori(0);
	F(2, 6) = 0.5 * this->dt * this->x_apriori(1);

	// 4th row
	F(3, 0) = 0.5 * (w(2) - this->x_apriori(6)) * this->dt;
	F(3, 1) =	0.5 * (w(1) - this->x_apriori(5)) * this->dt;
	F(3, 2) =	- 0.5 * (w(0) - this->x_apriori(4)) * this->dt;
	F(3, 3) = 1;
	F(3, 4) = 0.5 * this->dt * this->x_apriori(2);
	F(3, 5) = - 0.5 * this->dt * this->x_apriori(1);
	F(3, 6) = - 0.5 * this->dt * this->x_apriori(0);

	// 5th row
	F(4,4) = 1.0;

	// 6th row
	F(5,5) = 1.0;

	// 7th row
	F(6,6) = 1.0;

	return F;
}

Eigen::Matrix<float, 7, 1> EKF::state(float* wArray) {
	Eigen::Matrix<float, 7, 1> F;
	Eigen::Matrix<float, 3, 1> w;
	w << wArray[0], wArray[1], wArray[2];

	//	cv::Mat &w = *(cv::Mat *) addrW;
//	cv::Mat F = cv::Mat::zeros(7, 1, CV_32F);

	F(4) = this->x_aposteriori(4);
	F(5) = this->x_aposteriori(5);
	F(6) = this->x_aposteriori(6);

	Eigen::Matrix<float, 4, 4> A;
	A.setZero();

	// A 1st row
	A(0, 0) = 1.0;
	A(0, 1) = -0.5 * (w(2) - F(6)) * this->dt;
	A(0, 2) = 0.5 * (w(1) - F(5)) * this->dt;
	A(0, 3) = 0.5 * (w(0) - F(4)) * this->dt;

	// A 2nd row
	A(1, 0) = 0.5 * (w(2) - F(6)) * this->dt;
	A(1, 1) = 1;
	A(1, 2) = -0.5 * (w(0) - F(4)) * this->dt;
	A(1, 3) = 0.5 * (w(1) - F(5)) * this->dt;

	// A 3rd row
	A(2, 0) = -0.5 * (w(1) - F(5)) * this->dt;
	A(2, 1) = 0.5 * (w(0) - F(4)) * this->dt;
	A(2, 2) = 1;
	A(2, 3) = 0.5 * (w(2) - F(6)) * this->dt;

	// A 4th row
	A(3, 0) = -0.5 * (w(0) - F(4)) * this->dt;
	A(3, 1) = -0.5 * (w(1) - F(5)) * this->dt;
	A(3, 2) = -0.5 * (w(2) - F(6)) * this->dt;
	A(3, 3) = 1;


	// Only (1:4)
	Eigen::Matrix<float, 1, 4> x = A * (this->x_aposteriori).block<1,4>(0,0);

	F(0) = x(0);
	F(1) = x(1);
	F(2) = x(2);
	F(3) = x(3);
	return F;
}


void EKF::predict(float* inputArray, float _dt) {
	this->dt = _dt;
	this->x_apriori = this->state(inputArray);
	Eigen::Matrix<float, 7, 7> F = this->jacobian(inputArray);
	this->P_apriori = F * this->P_aposteriori * F.transpose() + this->Q;
};

void EKF::correct(float* measurement) {
	Eigen::Matrix<float, 4, 1> z;
	z << measurement[0], measurement[1], measurement[2], measurement[3];

	this->K = (this->P_apriori * this->H.transpose())
			* (this->H * this->P_apriori * this->H.transpose() + this->R).inverse();
	this->x_aposteriori = this->x_apriori
			+ this->K * (z - this->H * this->x_apriori);
	this->P_aposteriori = (this->I - this->K * this->H) * this->P_apriori;
};
