/*************************************************************************/
/*  GodotView.java                                                       */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                    http://www.godotengine.org                         */
/*************************************************************************/
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                 */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/
package com.android.godot;
import android.content.Context;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.content.ContextWrapper;
import android.view.InputDevice;

import java.io.File;
import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

import android.provider.Settings.Secure;
import android.os.SystemClock;

/**
 * A simple GLSurfaceView sub-class that demonstrate how to perform
 * OpenGL ES 2.0 rendering into a GL Surface. Note the following important
 * details:
 *
 * - The class must use a custom context factory to enable 2.0 rendering.
 *   See ContextFactory class definition below.
 *
 * - The class must use a custom EGLConfigChooser to be able to select
 *   an EGLConfig that supports 2.0. This is done by providing a config
 *   specification to eglChooseConfig() that has the attribute
 *   EGL10.ELG_RENDERABLE_TYPE containing the EGL_OPENGL_ES2_BIT flag
 *   set. See ConfigChooser class definition below.
 *
 * - The class must select the surface's format, then choose an EGLConfig
 *   that matches it exactly (with regards to red/green/blue/alpha channels
 *   bit depths). Failure to do so would result in an EGL_BAD_MATCH error.
 */
//public static class Renderer implements GLSurfaceView.Renderer {
public class GodotRenderer implements GLSurfaceView.Renderer {
	// MAINT: No sure if this should be static or not ...?
	//private static boolean firsttime=true;
	public boolean firsttime;
	public int frame_count;	
	public GodotLib mEngine;
	public GodotWallpaperService mWallpaper;
	public boolean mIsWallpaperInitialized;	
	static public int count = 0;
	public int mMyEngineId;
	
	public int mSkippedWidth;
	public int mSkippedHeight;
	public boolean mSkippedNotFirstTime;
	
	public void init(){
		String [] command_line = new String[0];
		mWallpaper.io = new GodotIO(mWallpaper, mEngine);
		mWallpaper.io.unique_id = Secure.getString(mWallpaper.getContentResolver(), Secure.ANDROID_ID);
		//GodotLib.io=GodotWallpaperService.this.io;
		mEngine.initializeWallpaper(mWallpaper, mWallpaper.io.needsReloadHooks(), command_line);
		mIsWallpaperInitialized = true;
		
		// Handle skipped events
		mEngine.focusin();
		mEngine.newcontext();
		mEngine.resize(mSkippedWidth, mSkippedHeight, mSkippedNotFirstTime);
	}
	
	public GodotRenderer(GodotLib engine, GodotWallpaperService wallpaper, int id)
	{
		super();
		count++;
		frame_count = 0;
		firsttime = true;
		mEngine = engine;
		mWallpaper = wallpaper;
		mIsWallpaperInitialized = false;
		mMyEngineId = id;
		Log.d("Godot", "GodotRenderer("+count+") - CONSTRUCTOR");
		
		mSkippedWidth = 50;
		mSkippedHeight = 50;
		mSkippedNotFirstTime = false;
	}
	
	public GodotRenderer(GodotLib engine)
	{
		super();
		count++;
		frame_count = 0;
		firsttime = true;
		mEngine = engine;
		mWallpaper = null;
		mIsWallpaperInitialized = false;
		Log.d("Godot", "GodotRenderer("+count+") - CONSTRUCTOR");
		
		mSkippedWidth = -1;
		mSkippedHeight = -1;
		mSkippedNotFirstTime = false;	}
	

	@Override
	public void onDrawFrame(GL10 gl) {
		if (frame_count % 100 == 0 || frame_count == 0){
			Log.d("Godot", "GodotRenderer.onDrawFrame("+frame_count+")");
		}
		if (frame_count == 0 && mWallpaper != null){
			if (mMyEngineId == 1){
				this.init();
			}else{
				//Log.d("Godot", "Slepping: Zzzzzzz...");
				//SystemClock.sleep(1000);
				
				mIsWallpaperInitialized = true;
				
				// Handle skipped events
				//mEngine.focusin();
				mEngine.newcontext();
				mEngine.resize(mSkippedWidth, mSkippedHeight, mSkippedNotFirstTime);
			}
		}
		boolean isCurrThread = mMyEngineId == GodotWallpaperService.count_engine;
		if ( mWallpaper == null || mWallpaper != null && this.mIsWallpaperInitialized && isCurrThread ){
			mEngine.step();
		}else{
			Log.d("Godot", "Skipping context re-draw because engine not initialized yet. Is current thread:"+isCurrThread);			
		}
		for(int i=0;i<Godot.singleton_count;i++) {
			Godot.singletons[i].onGLDrawFrame(gl);
		}
		frame_count++;
	}
	@Override
	public void onSurfaceChanged(GL10 gl, int width, int height) {
		boolean isCurrThread = mMyEngineId == GodotWallpaperService.count_engine;
		Log.d("Godot", "GodotRenderer.onSurfaceChanged()");
		if ( mWallpaper == null || mWallpaper != null && this.mIsWallpaperInitialized && isCurrThread ){
			mEngine.resize(width, height,!firsttime);
		}else{
			Log.d("Godot", "Skipping context re-size because engine not initialized yet. Is current thread:"+isCurrThread);
			mSkippedWidth = width;
			mSkippedHeight = height;
			mSkippedNotFirstTime = !firsttime;
		}
		firsttime=false;
		for(int i=0;i<Godot.singleton_count;i++) {
			Godot.singletons[i].onGLSurfaceChanged(gl, width, height);
		}
	}
	@Override
	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
		boolean isCurrThread = mMyEngineId == GodotWallpaperService.count_engine;
		Log.d("Godot", "GodotRenderer.onSurfaceCreated()");
		if (firsttime){
			Log.d("Godot", "GodotLib.newcontext() - NEW CONTEXT");
			if ( mWallpaper == null || mWallpaper != null && this.mIsWallpaperInitialized && isCurrThread ){
				mEngine.newcontext();
			}else{
				Log.d("Godot", "Skipping context re-build because engine not initialized yet. Is current thread:"+isCurrThread);
			}
		}
	}
}

