#include <jni.h>
#include <vector>

#include "EKF/EKF.h"

using namespace std;

extern "C" {

// Export declarations
JNIEXPORT jlong JNICALL Java_org_dg_inertialSensors_AHRSModule_EKFcreate(JNIEnv*, jobject, jfloat Q,
		jfloat R, jfloat dt);
JNIEXPORT void JNICALL Java_org_dg_inertialSensors_AHRSModule_EKFpredict(JNIEnv* env, jobject obj,
		jlong addrEKF, jfloatArray x, jfloat dt);
JNIEXPORT void JNICALL Java_org_dg_inertialSensors_AHRSModule_EKFcorrect(JNIEnv* env, jobject,
		jlong addrEKF, jfloatArray measurement);
JNIEXPORT void JNICALL Java_org_dg_inertialSensors_AHRSModule_EKFdestroy(JNIEnv*, jobject,
		jlong addrEKF);

// Implementation of export methods
JNIEXPORT jlong JNICALL Java_org_dg_inertialSensors_AHRSModule_EKFcreate(JNIEnv*, jobject, jfloat Q,
		jfloat R, jfloat dt) {

	// Create new object and return the pointer to it's instance
	return (long) (new EKF(Q, R, dt));
}

JNIEXPORT void JNICALL Java_org_dg_inertialSensors_AHRSModule_EKFpredict(JNIEnv* env, jobject obj,
		jlong addrEKF, jfloatArray x, jfloat dt) {

	// Extracting array initialized in Java
	jfloat *inputArray = env->GetFloatArrayElements(x, 0);

	//__android_log_print(ANDROID_LOG_DEBUG, "EKF",
	//					"EKF: NDK side - %f %f %f\n", inputArray[0], inputArray[1], inputArray[2]);

	// Calling predict
	EKF &ekf = *(EKF*) addrEKF;
	ekf.predict(inputArray, dt);

	// Releasing NDK
	env->ReleaseFloatArrayElements(x, inputArray, 0);
	env->DeleteLocalRef(x);
}

JNIEXPORT void JNICALL Java_org_dg_inertialSensors_AHRSModule_EKFcorrect(JNIEnv* env, jobject,
		jlong addrEKF, jfloatArray measurement) {

	// Extracting array initialized in Java
	jfloat *measurementArray = env->GetFloatArrayElements(measurement, 0);

	// Calling correct
	EKF &ekf = *(EKF*) addrEKF;
	ekf.correct(measurementArray);

	// Releasing NDK
	env->ReleaseFloatArrayElements(measurement, measurementArray, 0);
	env->DeleteLocalRef(measurement);
}

JNIEXPORT void JNICALL Java_org_dg_inertialSensors_AHRSModule_EKFdestroy(JNIEnv* env, jobject,
		jlong addrEKF) {

	// Destroy object
	delete (EKF*) (addrEKF);
}

}

