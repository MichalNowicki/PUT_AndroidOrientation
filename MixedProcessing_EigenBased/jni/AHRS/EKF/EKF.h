#include "../Eigen/Eigen"
#include <android/log.h>

class EKF {

	Eigen::Matrix<float, 4, 4> R;
	Eigen::Matrix<float, 7, 7> Q;

	Eigen::Matrix<float, 7, 4> K;
	Eigen::Matrix<float, 7, 1> x_apriori, x_aposteriori;
	Eigen::Matrix<float, 7, 7> I, P_apriori, P_aposteriori;
	Eigen::Matrix<float, 4, 7> H;

	bool firstMeasurement;
	bool correctTime;

public:
	EKF(float _Q, float _R);
	void predict(float *inputArray, float dt, float *currentEstimate);
	void correct(float *measurement, float *currentEstimate);

private:
	Eigen::Matrix<float, 7, 7> jacobian(float* w, float dt);
	Eigen::Matrix<float, 7, 1> state(float* w, float dt);
};
