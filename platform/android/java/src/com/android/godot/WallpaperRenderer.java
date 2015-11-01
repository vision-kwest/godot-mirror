package com.android.godot;

import java.util.concurrent.Semaphore;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;
import android.opengl.GLSurfaceView.Renderer;
import android.util.Log;


//private static class Renderer implements GLSurfaceView.Renderer {
class WallpaperRenderer implements Renderer {
	public int step_count = 0; // for debug
	private boolean firsttime=true;
	public int renderer_id = -1;
	public Semaphore godot_lock = null;
	
	WallpaperRenderer(Semaphore lock){
		godot_lock = lock;
	}
	
	public void onDrawFrame(GL10 gl) {
		if (step_count % 100 == 0) Log.d("Godot", "GodotLib::step(): "+step_count);
		try {
			godot_lock.acquire();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		GodotLib.step();
		godot_lock.release();
		step_count++;
	}

	public void onSurfaceChanged(GL10 gl, int width, int height) {
		Log.d("Godot", "WallpaperRenderer::onSurfaceChanged()");
		Log.d("Godot", "GodotLib::resize()");
		try {
			godot_lock.acquire();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		GodotLib.resize(width, height,!firsttime);
		godot_lock.release();
		firsttime=false;
	}

	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
		Log.d("Godot", "WallpaperRenderer::onSurfaceCreated()");
		Log.d("Godot", "GodotLib::newcontext()");
		
		this.renderer_id = GodotWallpaperService2.renderer_count;
		GodotWallpaperService2.renderer_count++;
		Log.d("Godot", "renderer_id: " + this.renderer_id);
		try {
			godot_lock.acquire();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}			
		GodotLib.newcontext();
		godot_lock.release();
	}
}    

