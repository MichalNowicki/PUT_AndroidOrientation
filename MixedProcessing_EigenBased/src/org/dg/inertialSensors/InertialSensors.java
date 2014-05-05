package org.dg.inertialSensors;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.PrintStream;

import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Environment;
import android.util.Log;

public  class InertialSensors {

	// Main Android handler
	SensorManager sensorManager;

	// Streams to save data
	PrintStream accStream, gyroStream, magStream, accwogStream, orientStream;

	// Starting timestamp
	long timestampStart;
	
	// Last estimates
	float acc[], mag[], accwog[], gyro[], orient[];
	
	// isRunning
	boolean isStarted;

	InertialSensors(SensorManager _sensorManager) {
		sensorManager = _sensorManager;
		isStarted = false;
		acc = new float[3];
		mag = new float[3];
		accwog = new float[3];
		gyro = new float[3];
		orient = new float[3];
	}
	
	public boolean getState()
	{
		return isStarted;
	}

	public void startNoSave() {

		// Sensor handlers
		Sensor accelerometer = sensorManager
				.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
		Sensor gyro = sensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE);
		Sensor magnetic = sensorManager
				.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD);
		Sensor acc_wo_gravity = sensorManager
				.getDefaultSensor(Sensor.TYPE_LINEAR_ACCELERATION);
		Sensor orientation = sensorManager
				.getDefaultSensor(Sensor.TYPE_ROTATION_VECTOR);

		// Starting listeners
		sensorManager.registerListener(sensorEventListener, accelerometer,
				SensorManager.SENSOR_DELAY_FASTEST);
		sensorManager.registerListener(sensorEventListener, gyro,
				SensorManager.SENSOR_DELAY_FASTEST);
		sensorManager.registerListener(sensorEventListener, magnetic,
				SensorManager.SENSOR_DELAY_FASTEST);
		sensorManager.registerListener(sensorEventListener, acc_wo_gravity,
				SensorManager.SENSOR_DELAY_FASTEST);
		sensorManager.registerListener(sensorEventListener, orientation,
				SensorManager.SENSOR_DELAY_FASTEST);

	}

	public void start(int id) {

		isStarted = true;
		timestampStart = 0;
		try {
			openStreams(id);
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}

		// Sensor handlers
		Sensor accelerometer = sensorManager
				.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
		Sensor gyro = sensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE);
		Sensor magnetic = sensorManager
				.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD);
		Sensor acc_wo_gravity = sensorManager
				.getDefaultSensor(Sensor.TYPE_LINEAR_ACCELERATION);
		Sensor orientation = sensorManager
				.getDefaultSensor(Sensor.TYPE_ROTATION_VECTOR);

		// Starting listeners
		sensorManager.registerListener(sensorEventListenerSave, accelerometer,
				SensorManager.SENSOR_DELAY_FASTEST);
		sensorManager.registerListener(sensorEventListenerSave, gyro,
				SensorManager.SENSOR_DELAY_FASTEST);
		sensorManager.registerListener(sensorEventListenerSave, magnetic,
				SensorManager.SENSOR_DELAY_FASTEST);
		sensorManager.registerListener(sensorEventListenerSave, acc_wo_gravity,
				SensorManager.SENSOR_DELAY_FASTEST);
		sensorManager.registerListener(sensorEventListenerSave, orientation,
				SensorManager.SENSOR_DELAY_FASTEST);

	}

	public void stopNoSave() {
		sensorManager.unregisterListener(sensorEventListener);
	}
	
	public void stop() {
		isStarted=false;
		timestampStart = 0;
		unregister();
		closeStreams();
	}

	public void unregister() {
		sensorManager.unregisterListener(sensorEventListenerSave);
	}
	
	public float[] getCurrentMagnetometer()
	{
		return mag;
	}

	public float[] getCurrentAcc()
	{
		return acc;
	}

	public float[] getCurrentOrient()
	{
		return orient;
	}
	
	private void openStreams(int id ) throws FileNotFoundException {
		
		File dir = new File(String.format("/sdcard/_exp/inertial/%d/", id ));
		if (!dir.exists())
		{
			dir.mkdirs();
		}

		String fileName = Environment.getExternalStorageDirectory().toString() + "/_exp/inertial/"+id+"/acc.log";
		FileOutputStream faccStream = new FileOutputStream(fileName);
		accStream = new PrintStream(faccStream);

		fileName = Environment.getExternalStorageDirectory()
				.toString() + "/_exp/inertial/"+id+"/gyro.log";
		FileOutputStream fgyroStream = new FileOutputStream(fileName);
		gyroStream = new PrintStream(fgyroStream);

		fileName = Environment.getExternalStorageDirectory()
				.toString() + "/_exp/inertial/"+id+"/mag.log";
		FileOutputStream fmagStream = new FileOutputStream(fileName);
		magStream = new PrintStream(fmagStream);

		fileName = Environment.getExternalStorageDirectory()
				.toString() + "/_exp/inertial/"+id+"/accwog.log";
		FileOutputStream faccwogStream = new FileOutputStream(fileName);
		accwogStream = new PrintStream(faccwogStream);

		fileName = Environment.getExternalStorageDirectory()
				.toString() + "/_exp/inertial/"+id+"/orient.log";
		FileOutputStream forientStream = new FileOutputStream(fileName);
		orientStream = new PrintStream(forientStream);

	}

	private void closeStreams() {
		accStream.close();
		gyroStream.close();
		magStream.close();
		accwogStream.close();
		orientStream.close();
	}

	private SensorEventListener sensorEventListenerSave = new SensorEventListener() {

		public void onAccuracyChanged(Sensor sensor, int accuracy) {

		}

		public void onSensorChanged(SensorEvent event) {
			
			if (timestampStart == 0)
			{
				timestampStart = event.timestamp;
			}

			
			String nl = System.getProperty("line.separator");

			if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
				accStream.print(Long.toString(event.timestamp - timestampStart)
						+ " " + Float.toString(event.values[0]) + " "
						+ Float.toString(event.values[1]) + " "
						+ Float.toString(event.values[2]) + nl);

			} else if (event.sensor.getType() == Sensor.TYPE_GYROSCOPE) {

				gyroStream.print(Long.toString(event.timestamp - timestampStart)
						+ " " + Float.toString(event.values[0]) + " "
						+ Float.toString(event.values[1]) + " "
						+ Float.toString(event.values[2]) + nl);

			} else if (event.sensor.getType() == Sensor.TYPE_MAGNETIC_FIELD) {

				magStream.print(Long.toString(event.timestamp - timestampStart)
						+ " " + Float.toString(event.values[0]) + " "
						+ Float.toString(event.values[1]) + " "
						+ Float.toString(event.values[2]) + nl);

			} else if (event.sensor.getType() == Sensor.TYPE_LINEAR_ACCELERATION) {
				accwogStream.print(Long.toString(event.timestamp
						- timestampStart)
						+ " "
						+ Float.toString(event.values[0])
						+ " "
						+ Float.toString(event.values[1])
						+ " "
						+ Float.toString(event.values[2]) + nl);

			} else if (event.sensor.getType() == Sensor.TYPE_ROTATION_VECTOR) {

				orientStream.print(Long.toString(event.timestamp
						- timestampStart)
						+ " "
						+ Float.toString(event.values[0])
						+ " "
						+ Float.toString(event.values[1])
						+ " "
						+ Float.toString(event.values[2]) + nl);

			}
		}
	};
	
	private SensorEventListener sensorEventListener = new SensorEventListener() {

		public void onAccuracyChanged(Sensor sensor, int accuracy) {

		}

		public void onSensorChanged(SensorEvent event) {

			if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
				acc[0] = event.values[0];
				acc[1] = event.values[1];
				acc[2] = event.values[2];

			} else if (event.sensor.getType() == Sensor.TYPE_GYROSCOPE) {
				gyro[0] = event.values[0];
				gyro[1] = event.values[1];
				gyro[2] = event.values[2];

			} else if (event.sensor.getType() == Sensor.TYPE_MAGNETIC_FIELD) {
				mag[0] = event.values[0];
				mag[1] = event.values[1];
				mag[2] = event.values[2];

			} else if (event.sensor.getType() == Sensor.TYPE_LINEAR_ACCELERATION) {
				

			} else if (event.sensor.getType() == Sensor.TYPE_ROTATION_VECTOR) {
				orient[0] = event.values[0];
				orient[1] = event.values[1];
				orient[2] = event.values[2];
				

			}
		}
	};

}

