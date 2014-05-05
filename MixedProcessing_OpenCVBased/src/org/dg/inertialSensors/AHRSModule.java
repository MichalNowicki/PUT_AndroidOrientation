package org.dg.inertialSensors;


import org.opencv.core.CvType;
import org.opencv.core.Mat;

import android.util.Log;

public class AHRSModule implements Runnable{
	
	public native long EKFcreate(float Q, float R, float dt);
	public native void EKFpredict(long addrEKF, long addrW, float dt);
	public native void EKFcorrect(long addrEKF, long addrZ);
	public native void EKFdestroy(long addrEKF);
	
	
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
		Mat w = Mat.zeros(3,1, CvType.CV_32F);
		w.put(0, 0, -0.3986);
		w.put(1, 0, 0.6132);
		w.put(2, 0, 0.5962);
		
		
		;
		Log.d("EKF", "EKF predict start!\n");
		long timeStart = System.currentTimeMillis();
		long minusTime = 0;
		for (int i=0;i<10000;i++)
		{
			long rStart = System.currentTimeMillis();
			w.put(0, 0, Math.random());
			w.put(1, 0, Math.random());
			w.put(2, 0, Math.random());
			minusTime += System.currentTimeMillis() - rStart;
			
			EKFpredict(addrEKF, w.getNativeObjAddr(), 0.01f);
		}
		long timeEnd = System.currentTimeMillis();
		Log.d("EKF", "EKF predict time : " + (timeEnd - timeStart - minusTime) + "!\n");
	}

	public void correct()
	{
		// z is 4x1 quat orientation
		Mat z = Mat.zeros(4,1, CvType.CV_32F);
		z.put(0, 0, -0.3986);
		z.put(1, 0, 0.6132);
		z.put(2, 0, 0.5962);
		z.put(3, 0, -0.3311);
		
		Log.d("EKF", "EKF correct start!\n");
		long timeStart = System.currentTimeMillis();
		long minusTime = 0;
		for (int i=0;i<10000;i++)
		{
			long rStart = System.currentTimeMillis();
			z.put(0, 0, Math.random());
			z.put(1, 0, Math.random());
			z.put(2, 0, Math.random());
			z.put(3, 0, Math.random());
			minusTime += System.currentTimeMillis() - rStart;
	
			EKFcorrect(addrEKF, z.getNativeObjAddr());
			
		}
		long timeEnd = System.currentTimeMillis();
		Log.d("EKF", "EKF correct time : " + (timeEnd - timeStart - minusTime) + "!\n");
	}
	
	public void predictcorrect()
	{
		// z is 3x1 quat orientation
		Mat z = Mat.zeros(4,1, CvType.CV_32F);
		z.put(0, 0, -0.3986);
		z.put(1, 0, 0.6132);
		z.put(2, 0, 0.5962);
		z.put(3, 0, -0.3311);
		
		Mat w = Mat.zeros(3,1, CvType.CV_32F);
		w.put(0, 0, -0.3986);
		w.put(1, 0, 0.6132);
		w.put(2, 0, 0.5962);
		  
		Log.d("EKF", "EKF predict+correct start!\n");
		long timeStart = System.currentTimeMillis();
		long minusTime = 0;
		for (int i=0;i<10000;i++)
		{
			long rStart = System.currentTimeMillis();
			z.put(0, 0, Math.random());
			z.put(1, 0, Math.random());
			z.put(2, 0, Math.random());
			z.put(3, 0, Math.random());
			w.put(0, 0, Math.random());
			w.put(1, 0, Math.random());
			w.put(2, 0, Math.random());
			minusTime += System.currentTimeMillis() - rStart;
			
			EKFpredict(addrEKF, w.getNativeObjAddr(), 0.01f);
			EKFcorrect(addrEKF, z.getNativeObjAddr());
			
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
