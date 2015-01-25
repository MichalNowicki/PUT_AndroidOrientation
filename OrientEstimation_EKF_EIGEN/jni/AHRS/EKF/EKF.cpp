/**
 * @author Michal Nowicki michal.nowicki@put.poznan.pl
 *
 */

#include "EKF.h"

EKF::EKF(float Qq, float Qw, float Rr) {

	// Updating covariances based on init values
	this->Q = Eigen::Matrix<float, 7, 7>::Identity();
	this->Q.block<4,4>(0,0) = Eigen::Matrix<float, 4, 4>::Identity() * Qq;
	this->Q.block<3,3>(4,4) = Eigen::Matrix<float, 3, 3>::Identity() * Qw;
	this->R = Eigen::Matrix<float, 4, 4>::Identity() * Rr;

	// Setting initial values
	this->x_apriori.setZero();
	this->x_aposteriori.setZero();
	this->P_apriori = this->Q;
	this->P_aposteriori = this->Q;

	this->H.setZero();
	for (int i=0;i<4;i++)
		this->H(i, i) = 1.0;

	firstMeasurement = true;
	correctTime = true;
}

Eigen::Matrix<float, 7, 7> EKF::jacobian(float *wArray, float dt) {
	Eigen::Matrix<float, 7, 7> F = Eigen::Matrix<float, 7, 7>::Identity();
	Eigen::Matrix<float, 3, 1> w;
	w << wArray[0], wArray[1], wArray[2];

	// 1st row
	F(0, 0) = 1.0;
	F(0, 1) = -0.5 * (w(0) - this->x_apriori(4)) * dt;
	F(0, 2) = -0.5 * (w(1) - this->x_apriori(5)) * dt;
	F(0, 3) = -0.5 * (w(2) - this->x_apriori(6)) * dt;
	F(0, 4) = 0.5 * dt * this->x_apriori(1);
	F(0, 5) = 0.5 * dt * this->x_apriori(2);
	F(0, 6) = 0.5 * dt * this->x_apriori(3);

	// 2nd row
	F(1, 0) = 0.5 * (w(0) - this->x_apriori(4)) * dt;
	F(1, 1) = 1;
	F(1, 2) = 0.5 * (w(2) - this->x_apriori(6)) * dt;
	F(1, 3) = -0.5 * (w(1) - this->x_apriori(5)) * dt;
	F(1, 4) = -0.5 * dt * this->x_apriori(0);
	F(1, 5) = 0.5 * dt * this->x_apriori(3);
	F(1, 6) = -0.5 * dt * this->x_apriori(2);

	// 3rd row
	F(2, 0) = 0.5 * (w(1) - this->x_apriori(5)) * dt;
	F(2, 1) = -0.5 * (w(2) - this->x_apriori(6)) * dt;
	F(2, 2) = 1;
	F(2, 3) = 0.5 * (w(0) - this->x_apriori(4)) * dt;
	F(2, 4) = -0.5 * dt * this->x_apriori(3);
	F(2, 5) = -0.5 * dt * this->x_apriori(0);
	F(2, 6) = 0.5 * dt * this->x_apriori(1);

	// 4th row
	F(3, 0) = 0.5 * (w(2) - this->x_apriori(6)) * dt;
	F(3, 1) = 0.5 * (w(1) - this->x_apriori(5)) * dt;
	F(3, 2) = -0.5 * (w(0) - this->x_apriori(4)) * dt;
	F(3, 3) = 1;
	F(3, 4) = 0.5 * dt * this->x_apriori(2);
	F(3, 5) = -0.5 * dt * this->x_apriori(1);
	F(3, 6) = -0.5 * dt * this->x_apriori(0);

	// 5th row
	F(4, 4) = 1.0;

	// 6th row
	F(5, 5) = 1.0;

	// 7th row
	F(6, 6) = 1.0;

	return F;
}

Eigen::Matrix<float, 7, 1> EKF::statePrediction(float* wArray, float dt) {
	Eigen::Matrix<float, 7, 1> F = Eigen::Matrix<float, 7, 1>::Identity();
	Eigen::Matrix<float, 3, 1> w;
	w << wArray[0], wArray[1], wArray[2];

	F(4) = this->x_aposteriori(4);
	F(5) = this->x_aposteriori(5);
	F(6) = this->x_aposteriori(6);

	Eigen::Matrix<float, 4, 4> A;
	A.setZero();

	// A 1st row
	A(0, 0) = 1.0;
	A(0, 1) = -0.5 * (w(0) - F(4)) * dt;
	A(0, 2) = -0.5 * (w(1) - F(5)) * dt;
	A(0, 3) = -0.5 * (w(2) - F(6)) * dt;

	// A 2nd row
	A(1, 0) = 0.5 * (w(0) - F(4)) * dt;
	A(1, 1) = 1;
	A(1, 2) = 0.5 * (w(2) - F(6)) * dt;
	A(1, 3) = -0.5 * (w(1) - F(5)) * dt;

	// A 3rd row
	A(2, 0) = 0.5 * (w(1) - F(5)) * dt;
	A(2, 1) = -0.5 * (w(2) - F(6)) * dt;
	A(2, 2) = 1;
	A(2, 3) = 0.5 * (w(0) - F(4)) * dt;

	// A 4th row
	A(3, 0) = 0.5 * (w(2) - F(6)) * dt;
	A(3, 1) = 0.5 * (w(1) - F(5)) * dt;
	A(3, 2) = -0.5 * (w(0) - F(4)) * dt;
	A(3, 3) = 1;

	// Only (1:4)
	Eigen::Matrix<float, 4, 1> x = A * (this->x_aposteriori).block<4, 1>(0, 0);

	for (int i=0;i<4;i++)
		F(i) = x(i);
	return F;
}

void EKF::predict(float* inputArray, float _dt, float *currentEstimate) {

	// We should do predict or correct?
	if (!correctTime)
	{
		this->x_apriori = this->statePrediction(inputArray, _dt);
		Eigen::Matrix<float, 7, 7> F = this->jacobian(inputArray, _dt);
		this->P_apriori = F * this->P_aposteriori * F.transpose() + this->Q;
		correctTime = true;
	}
	// Update current estimate
	for (int i=0;i<4;i++)
		currentEstimate[i] = this->x_aposteriori(i);
}

void EKF::correct(float* measurement, float* currentEstimate) {

	// First measurement -> we start estimation from acc/mag position
	if(firstMeasurement)
	{
		firstMeasurement = false;
		correctTime = false;
		for (int i=0;i<4;i++)
			this->x_aposteriori(i) = measurement[i];
		// Lets init it with some other uncertainty
		// this->P_aposteriori = ??? ;
	}
	// We should do predict or correct?
	else if (correctTime)
	{
		correctTime = false;

		// Converting measurements
		Eigen::Matrix<float, 4, 1> z;
		z << measurement[0], measurement[1], measurement[2], measurement[3];

		// Some additional variables
		Eigen::Matrix<float, 7, 7> I = Eigen::Matrix<float, 7, 7>::Identity();
		Eigen::Matrix<float, 7, 4> K = Eigen::Matrix<float, 7, 4>::Zero();

		// EKF equations
		K =
				(this->P_apriori * this->H.transpose())
						* (this->H * this->P_apriori * this->H.transpose()
								+ this->R).inverse();
		this->x_aposteriori = this->x_apriori
			+ K * (z - this->H * this->x_apriori);
		this->P_aposteriori = (I - K * this->H) * this->P_apriori;

	}

	// Update current estimate
	for (int i=0;i<4;i++)
		currentEstimate[i] = this->x_aposteriori(i);
}
