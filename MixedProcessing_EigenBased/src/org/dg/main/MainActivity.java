package org.dg.main;

import java.util.Timer;
import java.util.TimerTask;
import java.lang.Math;

import org.dg.inertialSensors.AHRSModule;
import org.dg.inertialSensors.InertialSensors;
import org.dg.main.R;



import android.app.Activity;
import android.content.Context;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends Activity {
    private static final String    TAG = "Main::Activity";

    private static final int       VIEW_MODE_RGBA     = 0;
    private static final int       VIEW_MODE_GRAY     = 1;
    private static final int       VIEW_MODE_CANNY    = 2;
    private static final int       VIEW_MODE_FEATURES = 5;



    private MenuItem               mItemPreviewRGBA;
    private MenuItem               mItemPreviewGray;
    private MenuItem               mItemPreviewCanny;
    private MenuItem               mItemPreviewFeatures;
	
	// Inertial sensors
	android.hardware.SensorManager sensorManager;
	org.dg.inertialSensors.InertialSensors inertialSensors;
	boolean intertialStarted = false;
	  

	//
	public Handler mHandler;
	private Timer fuseTimer = new Timer();
	
    public MainActivity() {
        Log.i(TAG, "Instantiated new " + this.getClass());
    }

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
    	super.onCreate(savedInstanceState);
		setContentView(R.layout.main_surface_view);
		
		// GUI stuff
        mHandler = new Handler();
		
		// Activate sensor manager
		sensorManager = (android.hardware.SensorManager) getSystemService(Context.SENSOR_SERVICE);
	    inertialSensors = new org.dg.inertialSensors.InertialSensors(sensorManager);
		
	    
	    // Start button
	    Button buttonClick = (Button) findViewById(R.id.buttonClick);
		buttonClick.setOnClickListener(new OnClickListener() {
			public void onClick(View v) {
				// Let's start !
				inertialSensors.start();
				intertialStarted = true;
				
				// wait for one second until gyroscope and magnetometer/accelerometer
		        // data is initialised then scedule the complementary filter task
		        fuseTimer.scheduleAtFixedRate(new calculateFusedOrientationTask(),
		                                      1000, (long) 30.0f);
			}
		});
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        Log.i(TAG, "called onCreateOptionsMenu");
        mItemPreviewRGBA = menu.add("Preview RGBA");
        mItemPreviewGray = menu.add("Preview GRAY");
        mItemPreviewCanny = menu.add("Canny");
        mItemPreviewFeatures = menu.add("Find features");
        return true;
    }

    
    @Override
   	protected void onStop() {
   		if (intertialStarted == true)
   		{
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


    public boolean onOptionsItemSelected(MenuItem item) {
        Log.i(TAG, "called onOptionsItemSelected; selected item: " + item);

        int mViewMode;
		if (item == mItemPreviewRGBA) {
            mViewMode = VIEW_MODE_RGBA;
        } else if (item == mItemPreviewGray) {
            mViewMode = VIEW_MODE_GRAY;
        } else if (item == mItemPreviewCanny) {
            mViewMode = VIEW_MODE_CANNY;
        } else if (item == mItemPreviewFeatures) {
            mViewMode = VIEW_MODE_FEATURES;
        }

        return true;
    }
    
    
    class calculateFusedOrientationTask extends TimerTask {
    	 public void run() {
    		  // update sensor output in GUI
             mHandler.post(updateOreintationDisplayTask);
    	 }
    }
    
    public void updateOreintationDisplay() {
    	
    	// Set GUI
    	float [] estimate = inertialSensors.getCurrentOrientationEstimate();
    	Log.i("GUI", "quat : "
				+ Float.toString(estimate[0]) + " "
				+ Float.toString(estimate[1]) + " "
				+ Float.toString(estimate[2]) + " "
				+ Float.toString(estimate[3])+ "\n");
    	
    	float rotZ = (float) (Math.atan2( 2*(estimate[0]*estimate[1]+estimate[2]*estimate[3]), (1 - 2*(estimate[1]*estimate[1] + estimate[2] * estimate[2]))) * 180.0/3.1415265);
    	float rotY = (float) (Math.asin(2 *(estimate[0]*estimate[2]-estimate[3]*estimate[1])) * 180.0/3.1415265);
    	float rotX = (float) (Math.atan2( 2*(estimate[0]*estimate[3]+estimate[1]*estimate[2]), (1 - 2*(estimate[2]*estimate[2] + estimate[3] * estimate[3]))) * 180.0/3.1415265);
    	
    	 TextView textView = (TextView) findViewById(R.id.TextViewZ);
    	 textView.setText(String.format("\t%.2f", rotZ) + " deg");
    	 
    	 textView = (TextView) findViewById(R.id.TextViewY);
    	 textView.setText(String.format("\t%.2f", rotY) + " deg");
    	 
    	 textView = (TextView) findViewById(R.id.TextViewX);
    	 textView.setText(String.format("\t%.2f", rotX) + " deg");
    }
    

	private Runnable updateOreintationDisplayTask = new Runnable() {
		public void run() {
			updateOreintationDisplay();
		}
	};
}
