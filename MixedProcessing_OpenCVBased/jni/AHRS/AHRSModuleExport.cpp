#include <jni.h>
#include <opencv2/core/core.hpp>
#include <vector>

#include "EKF/EKF.h"

using namespace std;
using namespace cv;

extern "C" {

// Export declarations
JNIEXPORT jlong JNICALL Java_org_dg_inertialSensors_AHRSModule_EKFcreate(JNIEnv*, jobject, jfloat Q,
		jfloat R, jfloat dt);
JNIEXPORT void JNICALL Java_org_dg_inertialSensors_AHRSModule_EKFpredict(JNIEnv*, jobject,
		jlong addrEKF, jlong addrW, jfloat dt);
JNIEXPORT void JNICALL Java_org_dg_inertialSensors_AHRSModule_EKFcorrect(JNIEnv*, jobject,
		jlong addrEKF, jlong addrZ);
JNIEXPORT void JNICALL Java_org_dg_inertialSensors_AHRSModule_EKFdestroy(JNIEnv*, jobject,
		jlong addrEKF);

// Implementation of export methods
JNIEXPORT jlong JNICALL Java_org_dg_inertialSensors_AHRSModule_EKFcreate(JNIEnv*, jobject, jfloat Q,
		jfloat R, jfloat dt) {

	// Create new object
	return (long) (new EKF(Q, R, dt));
}

JNIEXPORT void JNICALL Java_org_dg_inertialSensors_AHRSModule_EKFpredict(JNIEnv*, jobject,
		jlong addrEKF, jlong addrW, jfloat dt) {

	// Calling predict
	EKF &ekf = *(EKF*) addrEKF;
	ekf.predict(addrW, dt);
}

JNIEXPORT void JNICALL Java_org_dg_inertialSensors_AHRSModule_EKFcorrect(JNIEnv*, jobject,
		jlong addrEKF, jlong addrZ) {

	// Calling correct
	EKF &ekf = *(EKF*) addrEKF;
	ekf.correct(addrZ);
}

JNIEXPORT void JNICALL Java_org_dg_inertialSensors_AHRSModule_EKFdestroy(JNIEnv* env, jobject,
		jlong addrEKF) {

	// Destroy object
	delete (EKF*) (addrEKF);
}

}

