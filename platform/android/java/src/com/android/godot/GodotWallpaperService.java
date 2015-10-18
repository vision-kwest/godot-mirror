package com.android.godot;

import java.util.List;

import net.rbgrn.android.glwallpaperservice.GLWallpaperService;
import android.app.ActivityManager;
import android.app.ActivityManager.RunningAppProcessInfo;
import android.content.SharedPreferences;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.util.Log;

import android.os.Debug;
import android.os.SystemClock;
import android.provider.Settings.Secure;


import  android.app.ActivityManager.MemoryInfo;


public class GodotWallpaperService extends GLWallpaperService {
	static public GodotIO io;
	//public GodotView mView;
	//static public boolean mIsInitialized;
	static public int mNumContext = 0;
	static public int step = 0;

	private String[] command_line;
	
	public static final String PREFERENCES = "nu.danielsundberg.droid.spinbox.livewallpaper";

	private SensorManager mSensorManager;
	private Sensor mAccelerometer;
	//private MyEngine mEngine;

	public GodotWallpaperService() {
		super();
		Log.d("Godot", "GodotWallpaperService()");
		//GodotLib.mIsInitialized = false;
		//mEngine = new MyEngine();
		
		/*
		ActivityManager manager =  (ActivityManager) this.getApplicationContext().getSystemService(this.getApplicationContext().ACTIVITY_SERVICE);
		List<RunningAppProcessInfo> activityes = ((ActivityManager)manager).getRunningAppProcesses();
		for (int iCnt = 0; iCnt < activityes.size(); iCnt++){
			int[] pids = {activityes.get(iCnt).pid};
			Debug.MemoryInfo[] memInfo = manager.getProcessMemoryInfo(pids);
			
			Log.d("Godot", "APP("+iCnt +"): "+ activityes.get(iCnt).processName +" PID: "+ activityes.get(iCnt).pid);
			Log.d("Godot", "Pss("+memInfo[0].getTotalPss() +") PrivateDirty("+ memInfo[0].getTotalPrivateDirty()+")");
		}
		*/
	}
	
	
	public void init(){
		if (!GodotLib.mIsInitialized){
			command_line = new String[0];
			io = new GodotIO(this);
			io.unique_id = Secure.getString(getContentResolver(), Secure.ANDROID_ID);
			//GodotLib.io=GodotWallpaperService.this.io;
			GodotLib.initializeWallpaper(this, !io.needsReloadHooks(), command_line, getAssets());
			Log.d("Godot", "GodotLib.initializeWallpaper()");
			GodotLib.mIsInitialized = true;
			
			io.gotOffsetEvent(0.5f); // init to center of screen

			
			ActivityManager manager =  (ActivityManager) this.getApplicationContext().getSystemService(this.getApplicationContext().ACTIVITY_SERVICE);
			List<RunningAppProcessInfo> activityes = ((ActivityManager)manager).getRunningAppProcesses();
			for (int iCnt = 0; iCnt < activityes.size(); iCnt++){
				int[] pids = {activityes.get(iCnt).pid};
				Debug.MemoryInfo[] memInfo = manager.getProcessMemoryInfo(pids);
				
				Log.d("Godot", "APP("+iCnt +"): "+ activityes.get(iCnt).processName +" PID: "+ activityes.get(iCnt).pid);
				Log.d("Godot", "Pss("+memInfo[0].getTotalPss() +") PrivateDirty("+ memInfo[0].getTotalPrivateDirty()+")");
			}
			
			
		}
	}

	public Engine onCreateEngine() {
		//Log.d("Godot", "GodotWallpaperService.onCreateEngine()");
		
		/*if (mIsInitialized){
			GodotLib.quit();
			mIsInitialized = false;
		}*/
		
		MyEngine engine = new MyEngine();
		return engine;
		//return mEngine;
	}

	public void onVideoInit(boolean use_gl2) {
		
		//mView = new GodotView(getApplication(),io,use_gl2);
		//setContentView(mView);
	}

	public void onDestroy(){
		if (GodotLib.mIsInitialized){
			GodotLib.quit();
			Log.d("Godot", "GodotLib.quit()");
			//GodotLib.mIsInitialized = false;
			super.onDestroy();
			//Log.d("Godot", "GodotLib.step() - QUIT!");
			//GodotLib.step(); // process the quit request
		}
	}
	
	public void forceQuit() {
		System.exit(0);
	}
	
	class MyEngine extends GLEngine implements SharedPreferences.OnSharedPreferenceChangeListener, SensorEventListener {
		GodotRenderer mRenderer;

		public MyEngine() {
			super();
			Log.d("Godot", "GodotWallpaperService.MyEngine.MyEngine()");
			// handle prefs, other initialization
			mRenderer = new GodotRenderer();	
			setEGLContextClientVersion(2);
			setRenderer(mRenderer);
			setRenderMode(RENDERMODE_CONTINUOUSLY);
			

		}

		public void onOffsetsChanged(float xOffset, float yOffset, float xOffsetStep, float yOffsetStep, int xPixelOffset, int yPixelOffset){
			super.onOffsetsChanged(xOffset,      yOffset,
					               xOffsetStep,  yOffsetStep,
					               xPixelOffset, yPixelOffset);
			
			Log.d("Godot", "xOffset: "+xOffset+" yOffset: "+yOffset);
			Log.d("Godot", "xOffsetStep: "+xOffsetStep+" yOffsetStep: "+yOffsetStep);
			Log.d("Godot", "xPixelOffset: "+xPixelOffset+" yPixelOffset: "+yPixelOffset);
			
			//long downTime = SystemClock.uptimeMillis();
			//long eventTime = SystemClock.uptimeMillis()
			//int action = MotionEvent.ACTION_MOVE
			//MotionEvent event = MotionEvent.obtain(downTime, eventTime, action, x, y, pressure, size, metaState, xPrecision, yPrecision, deviceId, edgeFlags);
			//io.gotTouchEvent(event);
			io.gotOffsetEvent(xOffset);
			
		}
		
		public void onDestroy() {
			//Log.d("Godot", "GodotWallpaperService.MyEngine.onDestroy()");
			/*
			GodotLib.quit();
			mIsInitialized = false;
			Log.d("Godot", "GodotLib.quit()");
			*/
			// Unregister this as listener
			mSensorManager.unregisterListener(this);

			// Kill renderer			
			mRenderer = null;

			setTouchEventsEnabled(false);

			super.onDestroy();
		}

		boolean is_paused = false;
		@Override
		public void onTouchEvent(MotionEvent event) {
			//Log.d("Godot", "GodotWallpaperService.MyEngine.onTouchEvent()");
			super.onTouchEvent(event);	
//			if (this.isPreview()){
			  io.gotTouchEvent(event);
//			}
			/*
			boolean is_button_down = MotionEvent.ACTION_DOWN == event.getActionMasked();
			if (is_button_down){
				togglePause(is_paused);
				is_paused = !is_paused;
			}
			*/
		}

		@Override
		public void onCreate(SurfaceHolder surfaceHolder) {
			//Log.d("Godot", "GodotWallpaperService.MyEngine.onCreate()");
			super.onCreate(surfaceHolder);

			// Add touch events
			setTouchEventsEnabled(true);


			
			GodotWallpaperService.this.init(); // Need a surface to init
			/*
			command_line = new String[0];
			GodotWallpaperService.this.io = new GodotIO(GodotWallpaperService.this);
			GodotWallpaperService.this.io.unique_id = Secure.getString(GodotWallpaperService.this.getContentResolver(), Secure.ANDROID_ID);
			//GodotLib.io=GodotWallpaperService.this.io;
			GodotLib.initializeWallpaper(GodotWallpaperService.this, GodotWallpaperService.this.io.needsReloadHooks(), command_line, getAssets());
			*/
			
			// Get sensormanager and register as listener.
			mSensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
			//Sensor orientationSensor = sm.getDefaultSensor(SensorManager.SENSOR_ORIENTATION);
			//sm.registerListener(this, orientationSensor, SensorManager.SENSOR_DELAY_GAME);
			mAccelerometer = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
			mSensorManager.registerListener(this, mAccelerometer, SensorManager.SENSOR_DELAY_NORMAL);
		}

		public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
			Log.d("Godot", "GodotWallpaperService.MyEngine.onSharedPreferenceChanged()");
		}

		public void onAccuracyChanged(Sensor sensor, int accuracy) {
			Log.d("Godot", "GodotWallpaperService.MyEngine.onAccuracyChanged()");
		}

		public void onSensorChanged(SensorEvent event) {
			//Log.d("Godot", "GodotWallpaperService.MyEngine.onSensorChanged()");
		}
		
		@Override
		public void onVisibilityChanged (boolean visible){
			super.onVisibilityChanged(visible);
			togglePause(visible);
			/*
			if (visible){
				mSensorManager.unregisterListener(this);
				GodotLib.focusout();
			}else{
				mSensorManager.registerListener(this, mAccelerometer, SensorManager.SENSOR_DELAY_NORMAL);
				GodotLib.focusin();
			}*/
		}
		
		public void togglePause(boolean visible){
			//if (GodotWallpaperService.mIsInitialized){
			if (GodotLib.mIsInitialized){
				if (visible){
					mSensorManager.registerListener(this, mAccelerometer, SensorManager.SENSOR_DELAY_NORMAL);
					GodotLib.focusin();
					// Add offset events
					Log.d("Godot", ">>>>>>>>>> setOffsetNotificationsEnabled(true)");
					//setOffsetNotificationsEnabled(true);
					Log.d("Godot", "GodotLib.focusin() - IN");
				}else{
					mSensorManager.unregisterListener(this);
					GodotLib.focusout();
					Log.d("Godot", "GodotLib.focusout() - OUT");
					/*if (GodotWallpaperService.step > 300){
						GodotLib.quit();
						mIsInitialized = false;
						Log.d("Godot", "GodotLib.quit()");
					}*/
				}
			}
		}
	}
}
