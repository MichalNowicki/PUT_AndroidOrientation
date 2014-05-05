#include "../Eigen/Eigen"
#include <android/log.h>

class EKF {

	Eigen::Matrix<float, 4, 4> R;
	Eigen::Matrix<float, 7, 7> Q;

//	Eigen::Matrix<float, 3, 1> w;
	Eigen::Matrix<float, 7, 4> K;
	float dt;
	Eigen::Matrix<float, 7, 1> x_apriori, x_aposteriori;
	Eigen::Matrix<float, 7, 7> I, P_apriori, P_aposteriori;
	Eigen::Matrix<float, 4, 7> H;

public:
	EKF(float _Q, float _R, float dt);
	void predict(float *inputArray, float dt);
	void correct(float *measurement);

private:
	Eigen::Matrix<float, 7, 7> jacobian(float* w);
	Eigen::Matrix<float, 7, 1> state(float* w);
};
