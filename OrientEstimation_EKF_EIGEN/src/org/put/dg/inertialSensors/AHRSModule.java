/**
 * @author Michal Nowicki michal.nowicki@put.poznan.pl
 *
 */

package org.put.dg.inertialSensors;

import android.util.Log;

public class AHRSModule {

	static {
		System.loadLibrary("AHRSModule");
		Log.d("EKF", "EKF lib loaded!\n");
	}

	// Definitions of methods available in NDK
	public native long EKFcreate(float Qq, float Qw, float Rr);
	public native float[] EKFpredict(long addrEKF, float[] input, float dt);
	public native float[] EKFcorrect(long addrEKF, float[] measurement);
	public native void EKFdestroy(long addrEKF);

	// Address of EKF instance
	private long addrEKF;

	// Most up-to-date estimate
	float[] orientationEstimate = { 0.0f, 0.0f, 0.0f, 0.0f };

	public AHRSModule() {
		final float Qq = (float) (9.342 * Math.pow(10, -7));
		final float Qw = (float) (8.159 * Math.pow(10, -7));
		final float Rr = 3.672f;
		create(Qq, Qw, Rr);
	}

	public AHRSModule(float Qq, float Qw, float Rr) {
		// Experimentally found values
		// - for slow and steady motions:
		// Qq = 2.528 * 10^7, Qw = 4.483 * 10^7, Rr = 3.672
		// - for dynamic motions:
		// Qq = 9.342 * 10^7, Qw = 8.159 * 10^7, Rr = 3.672
		create(Qq, Qw, Rr);
	}

	private void create(float Qq, float Qw, float Rr) {
		addrEKF = EKFcreate(Qq, Qw, Rr);
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
	}

	public float[] getEstimate() {
		return orientationEstimate;
	}

	public void destroy() {
		EKFdestroy(addrEKF);
	}

}
