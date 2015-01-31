/**
 * @author Michal Nowicki michal.nowicki@put.poznan.pl
 *
 */

#include <jni.h>

#include "EKF/EKF.h"

using namespace std;

extern "C" {

// Export declarations
JNIEXPORT jlong JNICALL Java_org_put_dg_inertialSensors_AHRSModule_EKFcreate(
		JNIEnv*, jobject, jfloat Qqmin, jfloat Qwmin, jfloat Qqmax, jfloat Qwmax, jfloat Rr);
JNIEXPORT jfloatArray JNICALL Java_org_put_dg_inertialSensors_AHRSModule_EKFpredict(
		JNIEnv* env, jobject obj, jlong addrEKF, jfloatArray x, jfloat dt);
JNIEXPORT jfloatArray JNICALL Java_org_put_dg_inertialSensors_AHRSModule_EKFcorrect(
		JNIEnv* env, jobject, jlong addrEKF, jfloatArray measurement);
JNIEXPORT void JNICALL Java_org_put_dg_inertialSensors_AHRSModule_EKFdestroy(
		JNIEnv*, jobject, jlong addrEKF);
//
// Implementation of export methods
//

JNIEXPORT jlong JNICALL Java_org_put_dg_inertialSensors_AHRSModule_EKFcreate(
		JNIEnv*, jobject, jfloat Qqmin, jfloat Qwmin, jfloat Qqmax, jfloat Qwmax, jfloat Rr) {

	// Create new object and return the pointer to it's instance
	return (long) (new EKF(Qqmin, Qwmin, Qqmax, Qwmax, Rr));
}

JNIEXPORT jfloatArray JNICALL Java_org_put_dg_inertialSensors_AHRSModule_EKFpredict(
		JNIEnv* env, jobject obj, jlong addrEKF, jfloatArray x, jfloat dt) {

	// Extracting array initialized in Java
	jfloat *inputArray = env->GetFloatArrayElements(x, 0);

	// Calling predict
	EKF &ekf = *(EKF*) addrEKF;
	float currentEstimate[4];
	ekf.predict(inputArray, dt, currentEstimate);

	// Releasing NDK
	env->ReleaseFloatArrayElements(x, inputArray, 0);
	env->DeleteLocalRef(x);

	// Copying data to return
	jfloatArray state = (env)->NewFloatArray(4);
	(env)->SetFloatArrayRegion(state, 0, 4, currentEstimate);
	return state;
}

JNIEXPORT jfloatArray JNICALL Java_org_put_dg_inertialSensors_AHRSModule_EKFcorrect(
		JNIEnv* env, jobject, jlong addrEKF, jfloatArray measurement) {

	// Extracting array initialized in Java
	jfloat *measurementArray = env->GetFloatArrayElements(measurement, 0);

	// Calling correct
	EKF &ekf = *(EKF*) addrEKF;
	float currentEstimate[4];
	ekf.correct(measurementArray, currentEstimate);

	// Releasing NDK
	env->ReleaseFloatArrayElements(measurement, measurementArray, 0);
	env->DeleteLocalRef(measurement);

	// Copying data to return
	jfloatArray state = (env)->NewFloatArray(4);
	(env)->SetFloatArrayRegion(state, 0, 4, currentEstimate);
	return state;
}

JNIEXPORT void JNICALL Java_org_put_dg_inertialSensors_AHRSModule_EKFdestroy(
		JNIEnv* env, jobject, jlong addrEKF) {

	// Destroy object
	delete (EKF*) (addrEKF);
}

}

