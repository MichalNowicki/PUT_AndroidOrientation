/**
 * @author Michal Nowicki michal.nowicki@put.poznan.pl
 *
 */

package org.put.dg.orientEigen;

import java.util.Timer;
import java.util.TimerTask;
import java.lang.Math;

//import org.dg.main.R;
import org.put.dg.orientEigen.R;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends Activity {
	private static final String TAG = "Main::Activity";

	// Inertial sensors
	android.hardware.SensorManager sensorManager;
	org.put.dg.inertialSensors.InertialSensors inertialSensors;
	boolean intertialStarted = false;

	//
	public Handler mHandler;
	private Timer fuseTimer;

	public MainActivity() {
		Log.i(TAG, "Instantiated new " + this.getClass());
	}

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main_surface_view);

		// Handler used to update estimates in the GUI
		mHandler = new Handler();

		// Activate sensor manager
		sensorManager = (android.hardware.SensorManager) getSystemService(Context.SENSOR_SERVICE);
		inertialSensors = new org.put.dg.inertialSensors.InertialSensors(
				sensorManager);

		// Start estimation button
		Button buttonClick = (Button) findViewById(R.id.buttonClick);
		buttonClick.setOnClickListener(new OnClickListener() {
			public void onClick(View v) {
				Button buttonClick = (Button) v;
				if ( inertialSensors.getState() )
				{
					inertialSensors.stop();
					buttonClick.setText("Start estimation");
					
					// Stop updating GUI
					fuseTimer.cancel();
				}
				else
				{
					// Let's start !
					inertialSensors.start();
					buttonClick.setText("Stop estimation");
					
					// Update the GUI with the estimate - 10Hz
					fuseTimer = new Timer();
					fuseTimer.scheduleAtFixedRate(
							new calculateFusedOrientationTask(), 0,
							(long) 100.0f);
				}
			}
		});
	}

	@Override
	protected void onStop() {
		if (intertialStarted == true) {
			inertialSensors.stop();
			intertialStarted = false;
		}
		super.onStop();
	}

	@Override
	protected void onResume() {
		super.onResume();
	}

	@Override
	protected void onPause() {
		super.onPause();
	}

	class calculateFusedOrientationTask extends TimerTask {
		public void run() {
			// update sensor output in GUI
			mHandler.post(updateOrientationDisplayTask);
		}
	}

	private Runnable updateOrientationDisplayTask = new Runnable() {
		public void run() {
			// Set GUI
			float[] estimate = inertialSensors.getCurrentOrientationEstimate();

			float rotZ = (float) (Math
					.atan2(2 * (estimate[0] * estimate[1] + estimate[2]
							* estimate[3]),
							(1 - 2 * (estimate[1] * estimate[1] + estimate[2]
									* estimate[2]))) * 180.0 / 3.1415265);
			float rotY = (float) (Math
					.asin(2 * (estimate[0] * estimate[2] - estimate[3]
							* estimate[1])) * 180.0 / 3.1415265);
			float rotX = (float) (Math
					.atan2(2 * (estimate[0] * estimate[3] + estimate[1]
							* estimate[2]),
							(1 - 2 * (estimate[2] * estimate[2] + estimate[3]
									* estimate[3]))) * 180.0 / 3.1415265);

			TextView textView = (TextView) findViewById(R.id.TextViewZ);
			textView.setText(String.format("\t%.2f", rotZ));

			textView = (TextView) findViewById(R.id.TextViewY);
			textView.setText(String.format("\t%.2f", rotY));

			textView = (TextView) findViewById(R.id.TextViewX);
			textView.setText(String.format("\t%.2f", rotX));
		}
	};
}
