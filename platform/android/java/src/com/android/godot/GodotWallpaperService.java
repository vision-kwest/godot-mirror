package com.android.godot;

import net.rbgrn.android.glwallpaperservice.GLWallpaperService;
import android.content.SharedPreferences;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.util.Log;

import android.provider.Settings.Secure;

public class GodotWallpaperService extends GLWallpaperService {
	static public int count = 0;
	static public int count_engine = 0;
	static public GodotIO io;
	//public GodotView mView;

	private String[] command_line;
	
	public static final String PREFERENCES = "nu.danielsundberg.droid.spinbox.livewallpaper";

	private SensorManager mSensorManager;
	private Sensor mAccelerometer;

	public GodotWallpaperService() {
		super();
		count++;
		Log.d("Godot", "GodotWallpaperService("+count+")");
		
	}

	public Engine onCreateEngine() {
		Log.d("Godot", "GodotWallpaperService.onCreateEngine()");
		MyEngine engine = new MyEngine();
		return engine;
	}

	public void onVideoInit(boolean use_gl2) {
		
		//mView = new GodotView(getApplication(),io,use_gl2);
		//setContentView(mView);
	}

	public void forceQuit() {

		System.exit(0);
	}
	class MyEngine extends GLEngine implements SharedPreferences.OnSharedPreferenceChangeListener, SensorEventListener {
		GodotRenderer mRenderer;
		GodotLib mEngine;
		final int id;

		public MyEngine() {
			super();
			GodotWallpaperService.count_engine++;
			id = GodotWallpaperService.count_engine; 
			Log.d("Godot", "GodotWallpaperService.MyEngine.MyEngine("+GodotWallpaperService.count_engine+")");
			// handle prefs, other initialization
			mEngine = new GodotLib();
			mRenderer = new GodotRenderer(mEngine, GodotWallpaperService.this, id);	
			setEGLContextClientVersion(2);
			setRenderer(mRenderer);
			setRenderMode(RENDERMODE_CONTINUOUSLY);
		}

		public void onDestroy() {
			Log.d("Godot", "GodotWallpaperService.MyEngine.onDestroy()");
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
			Log.d("Godot", "GodotWallpaperService.MyEngine.onTouchEvent()");
			super.onTouchEvent(event);		
			io.gotTouchEvent(event);
			
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
			Log.d("Godot", "GodotWallpaperService.MyEngine.onCreate()");
			super.onCreate(surfaceHolder);

			// Add touch events
			setTouchEventsEnabled(true);

			/*
			command_line = new String[0];
			GodotWallpaperService.this.io = new GodotIO(GodotWallpaperService.this, mEngine);
			GodotWallpaperService.this.io.unique_id = Secure.getString(GodotWallpaperService.this.getContentResolver(), Secure.ANDROID_ID);
			//GodotLib.io=GodotWallpaperService.this.io;
			mEngine.initializeWallpaper(GodotWallpaperService.this, GodotWallpaperService.this.io.needsReloadHooks(), command_line);
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
			if (visible){
				/*
				if (this.mIsInitialized == false){
					command_line = new String[0];
					GodotWallpaperService.this.io = new GodotIO(GodotWallpaperService.this, mEngine);
					GodotWallpaperService.this.io.unique_id = Secure.getString(GodotWallpaperService.this.getContentResolver(), Secure.ANDROID_ID);
					//GodotLib.io=GodotWallpaperService.this.io;
					mEngine.initializeWallpaper(GodotWallpaperService.this, GodotWallpaperService.this.io.needsReloadHooks(), command_line);
					this.mIsInitialized = true;
				}
				*/
				mSensorManager.registerListener(this, mAccelerometer, SensorManager.SENSOR_DELAY_NORMAL);
				if (this.mRenderer.mWallpaper == null || this.mRenderer.mWallpaper != null && this.mRenderer.mIsWallpaperInitialized){
					mEngine.focusin();
					Log.d("Godot", "GodotWallpaperService.MyEngine.togglePause( IN_FOCUS )");
				}else{
					Log.d("Godot", "Skipping in-focus change because engine not initialized yet.");
				}
			}else{
				mSensorManager.unregisterListener(this);
				if (this.mRenderer.mWallpaper == null || this.mRenderer.mWallpaper != null && this.mRenderer.mIsWallpaperInitialized){
					mEngine.focusout();
					Log.d("Godot", "GodotWallpaperService.MyEngine.togglePause( OUT_FOCUS )");
				}else{
					Log.d("Godot", "Skipping out-focus change because engine not initialized yet.");				
				}
			}			
		}
	}
}
