package com.android.godot;


import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;

import android.hardware.SensorEvent;
import android.view.MotionEvent;
import net.rbgrn.android.glwallpaperservice.*;
import android.util.Log;


// Original code provided by Robert Green
// http://www.rbgrn.net/content/354-glsurfaceview-adapted-3d-live-wallpapers
public class GodotWallpaperRenderer implements GLWallpaperService.Renderer {
    private static final String LOG_TAG = "GodotWallpaperRenderer";
    public boolean firsttime;
    public int frame_count; 


	/**
	 * Set this class as renderer for this GLSurfaceView. Request Focus and set
	 * if focusable in touch mode to receive the Input from Screen
	 * 
	 * @param context
	 *            - The Activity Context
	 */

	public void onSensorChanged(SensorEvent event) {
        Log.v(LOG_TAG, "GodotWallpaperRenderer.onSensorChanged()");

	}

	public GodotWallpaperRenderer() {
        super();
        Log.v(LOG_TAG, "GodotWallpaperRenderer.GodotWallpaperRenderer()");
        frame_count = 0;
        firsttime = true;
	}

	public void release() {
        Log.v(LOG_TAG, "GodotWallpaperRenderer.release()");
		// TODO stuff to release
	}

	/**
	 * The Surface is created/init()
	 */
	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        Log.v(LOG_TAG, "GodotWallpaperRenderer.onSurfaceCreated()");
		GodotLib.newcontext(GodotWallpaperService.use_32_bit);
		GodotWallpaperService.current_gl_thread = android.os.Process.myTid();
	}

	/**
	 * Here we do our drawing
	 */
	public void onDrawFrame(GL10 gl) {
		if (frame_count%1000 == 0 ){
	        Log.v(LOG_TAG, "GodotWallpaperRenderer.onDrawFrame()");
		}
		GodotLib.step();
		frame_count++;
        for(int i=0;i<Godot.singleton_count;i++) {
        	Godot.singletons[i].onGLDrawFrame(gl);
        }
	}

	/**
	 * If the surface changes, reset the view
	 */
	public void onSurfaceChanged(GL10 gl, int width, int height) {
        Log.v(LOG_TAG, "GodotWallpaperRenderer.onSurfaceChanged()");
		if (GodotWallpaperService.current_gl_thread != android.os.Process.myTid()){
			GodotWallpaperService.current_gl_thread = android.os.Process.myTid();
			GodotLib.newcontext(GodotWallpaperService.use_32_bit);			
		}
		GodotLib.resize(width, height,!firsttime);
        firsttime=false;
        for(int i=0;i<Godot.singleton_count;i++) {
                Godot.singletons[i].onGLSurfaceChanged(gl, width, height);
        }
	}

	/**
	 * Override the touch screen listener.
	 * 
	 * React to moves and presses on the touchscreen.
	 */
	public boolean onTouchEvent(MotionEvent event) {
        Log.v(LOG_TAG, "GodotWallpaperRenderer.onTouchEvent()");
		// We handled the event
		return true;
	}
}
