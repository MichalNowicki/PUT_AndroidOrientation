package org.dg.inertialSensors;

import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.util.Log;

public  class InertialSensors {

	// Main Android handler
	SensorManager sensorManager;
	
	// AHRS
	AHRSModule ahrs;

	// Last timestamp
	long lastTimestamp;
	
	// Last estimates
	float gyro[], quaternion[];
	
	// isRunning
	boolean isStarted;

	public InertialSensors(SensorManager _sensorManager) {
		sensorManager = _sensorManager;
		
		ahrs = new AHRSModule();
		
		isStarted = false;
		
		
		gyro = new float[3];
		quaternion = new float[4];
	}
	
	public boolean getState()
	{
		return isStarted;
	}

	public void start() {
		lastTimestamp = -1;
		
		// Sensor handlers
		Sensor gyro = sensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE);
		Sensor orientation = sensorManager
				.getDefaultSensor(Sensor.TYPE_ROTATION_VECTOR);

		// Starting listeners
		sensorManager.registerListener(sensorEventListener, gyro,
				SensorManager.SENSOR_DELAY_FASTEST);
		sensorManager.registerListener(sensorEventListener, orientation,
				SensorManager.SENSOR_DELAY_FASTEST);

	}

	public void stop() {
		sensorManager.unregisterListener(sensorEventListener);
	}
	
	public float[] getCurrentOrientationEstimate()
	{
		return ahrs.getEstimate();
	}
	
	private SensorEventListener sensorEventListener = new SensorEventListener() {

		public void onSensorChanged(SensorEvent event) {

			if (event.sensor.getType() == Sensor.TYPE_GYROSCOPE) {
				gyro[0] = event.values[0];
				gyro[1] = event.values[1];
				gyro[2] = event.values[2];
				
			
				// Time from last update converted from ns to ms
				float dt = 0.0f;
				if ( lastTimestamp >= 0)
					dt = (lastTimestamp - event.timestamp)/1000000.0f; 
				lastTimestamp = event.timestamp;
				
				ahrs.predict(event.values[0], event.values[1], event.values[2], dt);

			} else if (event.sensor.getType() == Sensor.TYPE_ROTATION_VECTOR) {		
				// Convention (qw, qx, qy, qz)
			    SensorManager.getQuaternionFromVector(quaternion, event.values);
			    Log.d("EKF",
						"Quaternion: "
								+ Float.toString(quaternion[0]) + " "
								+ Float.toString(quaternion[1]) + " "
								+ Float.toString(quaternion[2]) + " "
								+ Float.toString(quaternion[3])+ "\n");
			    ahrs.correct(quaternion[0], quaternion[1], quaternion[2], quaternion[3]);
			}
		}

		@Override
		public void onAccuracyChanged(Sensor sensor, int accuracy) {
			// TODO Auto-generated method stub
		}
	};

}

