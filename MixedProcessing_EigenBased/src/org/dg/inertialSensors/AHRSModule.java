package org.dg.inertialSensors;


import android.util.Log;

public class AHRSModule implements Runnable {
	
	// Definitions of methods available in NDK
	public native long EKFcreate(float Q, float R, float dt);
	public native void EKFpredict(long addrEKF, float [] input, float dt);
	public native void EKFcorrect(long addrEKF, float [] measurement);
	public native void EKFdestroy(long addrEKF);
	
	// Address of EKF instance
	private long addrEKF;
	
	public AHRSModule() {
		 System.loadLibrary("AHRSModule");
		 Log.d("EKF", "EKF lib loaded!\n");
		 
	}
	
	public void create()
	{
		addrEKF = EKFcreate(0.0001f, 10f, 0.01f);
		Log.d("EKF", "EKF created succesfully!\n");
	}
	
	public void predict()
	{
		// w is 3x1 gyro measurement
		float [] w = new float [3];
		w[0] = (float) -0.3986;
		w[1] = (float) 0.6132;
		w[2] = (float) 0.5962;
	
		Log.d("EKF", "EKF predict start!\n");
		long timeStart = System.currentTimeMillis();
		long minusTime = 0;
		for (int i=0;i<10000;i++)
		{
			long rStart = System.currentTimeMillis();
			w[0] = (float) Math.random();
			w[1] = (float) Math.random();
			w[2] = (float) Math.random();
			minusTime += System.currentTimeMillis() - rStart;
			
			//Log.d("EKF", "EKF: Java side - "+ w[0] + " " + w[1] + " " + w[2] + " \n");
			EKFpredict(addrEKF, w, 0.01f);
		}
		long timeEnd = System.currentTimeMillis();
		Log.d("EKF", "EKF predict time : " + (timeEnd - timeStart - minusTime) + "!\n");
	}

	public void correct()
	{
		// z is 4x1 quat orientation
		float [] z = new float [4];
		z[0] = (float) -0.3986;
		z[1] = (float) 0.6132;
		z[2] = (float) 0.5962;
		z[3] = (float) -0.3311;
		
		Log.d("EKF", "EKF correct start!\n");
		long timeStart = System.currentTimeMillis();
		long minusTime = 0;
		for (int i=0;i<10000;i++)
		{
			long rStart = System.currentTimeMillis();
			z[0] = (float) Math.random();
			z[1] = (float) Math.random();
			z[2] = (float) Math.random();
			z[3] = (float) Math.random();
			minusTime += System.currentTimeMillis() - rStart;
	
			EKFcorrect(addrEKF, z);
			
		}
		long timeEnd = System.currentTimeMillis();
		Log.d("EKF", "EKF correct time : " + (timeEnd - timeStart - minusTime) + "!\n");
	}
	
	public void predictcorrect()
	{
		// z is 3x1 quat orientation
		float [] z = new float [4];
		z[0] = (float) -0.3986;
		z[1] = (float) 0.6132;
		z[2] = (float) 0.5962;
		z[3] = (float) -0.3311;
		
		float [] w = new float [3];
		w[0] = (float) -0.3986;
		w[0] = (float) 0.6132;
		w[2] = (float) 0.5962;
		  
		Log.d("EKF", "EKF predict+correct start!\n");
		long timeStart = System.currentTimeMillis();
		long minusTime = 0;
		for (int i=0;i<10000;i++)
		{
			long rStart = System.currentTimeMillis();
			z[0] = (float) Math.random();
			z[1] = (float) Math.random();
			z[2] = (float) Math.random();
			z[3] = (float) Math.random();
			w[0] = (float) Math.random();
			w[1] = (float) Math.random();
			w[2] = (float) Math.random();
			minusTime += System.currentTimeMillis() - rStart;
			
			EKFpredict(addrEKF, w, 0.01f);
			EKFcorrect(addrEKF, z);
			
		}
		long timeEnd = System.currentTimeMillis();
		Log.d("EKF", "EKF predict+correct time : " + (timeEnd - timeStart - minusTime) + "!\n");
	}
	
	public void destroy()
	{
		EKFdestroy(addrEKF);
	}
	@Override
	public void run() {
		 create();
		 predict();
		 correct();
		 
		 predictcorrect();
		 destroy();
	}
	
}
