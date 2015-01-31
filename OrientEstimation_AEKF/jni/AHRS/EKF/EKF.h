/**
 * @author Michal Nowicki michal.nowicki@put.poznan.pl
 *
 */

#include "../Eigen/Eigen"
#include <deque>

class EKF {
private:
	// Correct/Predict uncertainty
	Eigen::Matrix<float, 4, 4> R;
	Eigen::Matrix<float, 7, 7> Q, Qmin, Qmax, Qdiff;

	// State estimates (apriori and posteriori)
	Eigen::Matrix<float, 7, 1> x_apriori, x_aposteriori;

	// State estimates uncertainties (apriori and posteriori)
	Eigen::Matrix<float, 7, 7> P_apriori, P_aposteriori;
	Eigen::Matrix<float, 4, 7> H;

	// Additional values to detect estimation start and distinguish between predict/correct order
	bool firstMeasurement;
	bool correctTime;

	// Measurement window used in AEKF
	std::deque<float> measurementWindow;
	static const int measurementWindowSize = 10;

public:
	// Constructor:
	// - Qq, Qw, Rr meaning is the same as in the article published in IEEE Sensors Journal:
	// J. Goslinski, M. Nowicki, P. Skrzypczynski, "Performance Comparison of EKF-based Algorithms for Orientation Estimation Android Platform"
	EKF(float Qqmin, float Qwmin, float Qqmax, float Qwmax, float Rr);

	// Prediction step
	// - takes gyroscope measurements
	// - returns currentEstimate
	void predict(float *inputArray, float dt, float *currentEstimate);

	// Correction step
	// - takes accelerometer and magnetometer coordinate system
	// - returns currentEstimate
	void correct(float *measurement, float *currentEstimate);

private:
	// Additional methods to compute the jacobian
	Eigen::Matrix<float, 7, 7> jacobian(float* w, float dt);

	// Predict based on last estimate and gyroscope measurement
	Eigen::Matrix<float, 7, 1> statePrediction(float* w, float dt);
};
