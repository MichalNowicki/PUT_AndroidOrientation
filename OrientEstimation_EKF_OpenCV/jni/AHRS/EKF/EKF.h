/**
 * @author Michal Nowicki michal.nowicki@put.poznan.pl
 *
 */
#include <opencv2/core/core.hpp>
#include <android/log.h>

class EKF {
	// Correct/Predict uncertainty
	cv::Mat R, Q;

	// State estimates (apriori and posteriori)
	cv::Mat x_apriori, x_aposteriori; // 7x1 matrices

	// State estimates uncertainties (apriori and posteriori)
	cv::Mat P_apriori, P_aposteriori; // 7x7 matrices

	// Measurement matrix
	cv::Mat H;

	bool firstMeasurement;
	bool correctTime;

public:
	// Constructor:
	// - Qq, Qw, Rr meaning is the same as in the article published in IEEE Sensors Journal:
	// J. Goslinski, M. Nowicki, P. Skrzypczynski, "Performance Comparison of EKF-based Algorithms for Orientation Estimation Android Platform"
	EKF(float Qq, float Qw, float Rr);

	// Prediction step
	// - takes gyroscope measurements
	// - returns currentEstimate
	void predict(float *addrW, float dt, float *currentEstimate);

	// Correction step
	// - takes accelerometer and magnetometer coordinate system
	// - returns currentEstimate
	void correct(float *addrZ, float *currentEstimate);

private:
	// Additional methods to compute the jacobian
	cv::Mat jacobian(float *addrW, float dt);

	// Predict based on last estimate and gyroscope measurement
	cv::Mat state(float * addrW, float dt);
};
