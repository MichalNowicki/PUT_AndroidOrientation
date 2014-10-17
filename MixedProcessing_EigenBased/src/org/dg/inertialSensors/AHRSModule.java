package org.dg.inertialSensors;

import android.util.Log;

public class AHRSModule {

	static {
		System.loadLibrary("AHRSModule");
		Log.d("EKF", "EKF lib loaded!\n");
	}

	// Definitions of methods available in NDK
	public native long EKFcreate(float Q, float R);

	public native float[] EKFpredict(long addrEKF, float[] input, float dt);

	public native float[] EKFcorrect(long addrEKF, float[] measurement);

	public native void EKFdestroy(long addrEKF);

	// Address of EKF instance
	private long addrEKF;

	// Most up-to-date estimate
	float[] orientationEstimate = { 0.0f, 0.0f, 0.0f, 0.0f };

	public AHRSModule() {
		create(0.0001f, 10f);
	}

	public AHRSModule(float Q, float R) {
		create(Q, R);
	}

	private void create(float Q, float R) {
		addrEKF = EKFcreate(Q, R);
		Log.d("EKF", "EKF created succesfully!\n");
	}

	public void predict(float wx, float wy, float wz, float dt) {
		// w is 3x1 gyro measurement
		float[] w = new float[3];
		w[0] = wx;
		w[1] = wy;
		w[2] = wz;

		orientationEstimate = EKFpredict(addrEKF, w, dt);
	}

	public void correct(float q1, float q2, float q3, float q4) {
		// z is 4x1 quat orientation
		float[] z = new float[4];
		z[0] = (float) q1;
		z[1] = (float) q2;
		z[2] = (float) q3;
		z[3] = (float) q4;

		orientationEstimate = EKFcorrect(addrEKF, z);
		Log.d("EKF",
				"EKF current estimate: "
						+ Float.toString(orientationEstimate[0]) + " "
						+ Float.toString(orientationEstimate[1]) + " "
						+ Float.toString(orientationEstimate[2]) + " "
						+ Float.toString(orientationEstimate[3])+ "\n");
	}

	public float[] getEstimate() {
		return orientationEstimate;
	}

	public void destroy() {
		EKFdestroy(addrEKF);
	}

}
