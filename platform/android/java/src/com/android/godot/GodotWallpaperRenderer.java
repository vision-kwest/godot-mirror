package com.android.godot;


import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;

import android.hardware.SensorEvent;
import android.view.MotionEvent;
import net.rbgrn.android.glwallpaperservice.*;


// Original code provided by Robert Green
// http://www.rbgrn.net/content/354-glsurfaceview-adapted-3d-live-wallpapers
public class GodotWallpaperRenderer implements GLWallpaperService.Renderer {
    // MAINT: No sure if this should be static or not ...?
    //private static boolean firsttime=true;
    public boolean firsttime;
    public int frame_count; 
    public boolean new_ctx;


	/**
	 * Set this class as renderer for this GLSurfaceView. Request Focus and set
	 * if focusable in touch mode to receive the Input from Screen
	 * 
	 * @param context
	 *            - The Activity Context
	 */

	public void onSensorChanged(SensorEvent event) {

	}

	public GodotWallpaperRenderer() {
        super();
        frame_count = 0;
        firsttime = true;
        new_ctx = false;
	}

	public void release() {
		// TODO stuff to release
	}

	/**
	 * The Surface is created/init()
	 */
	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
		GodotLib.newcontext();
	}

	/**
	 * Here we do our drawing
	 */
	public void onDrawFrame(GL10 gl) {
		if (frame_count%100 == 0 ){
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
		if (GodotWallpaperService.current_gl_thread != android.os.Process.myTid()){
			GodotWallpaperService.current_gl_thread = android.os.Process.myTid();
			GodotLib.newcontext();			
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
		// We handled the event
		return true;
	}
}
