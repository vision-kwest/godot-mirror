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
	
	public GodotRenderer()
	{
		super();
		frame_count = 0;
		firsttime = true;
		//Log.d("Godot", "GodotRenderer() - CONSTRUCTOR");
	}

	@Override
	public void onDrawFrame(GL10 gl) {
		if (GodotLib.mIsInitialized){
			GodotLib.step();
			if (frame_count % 100 == 0 || frame_count == 0){
				Log.d("Godot", "GodotLib.step("+frame_count+")");
			}
			frame_count++;
			GodotWallpaperService.step++;
		}
		for(int i=0;i<Godot.singleton_count;i++) {
			Godot.singletons[i].onGLDrawFrame(gl);
		}
		
	}
	@Override
	public void onSurfaceChanged(GL10 gl, int width, int height) {
		//Log.d("Godot", "GodotRenderer.onSurfaceChanged()");
		if (GodotLib.mIsInitialized){
			GodotLib.resize(width, height,!firsttime);
			Log.d("Godot", "GodotLib.resize()");
		}
		firsttime=false;
		for(int i=0;i<Godot.singleton_count;i++) {
			Godot.singletons[i].onGLSurfaceChanged(gl, width, height);
		}
	}
	@Override
	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
		GodotWallpaperService.mNumContext++;
		Log.d("Godot", "GodotRenderer.onSurfaceCreated("+GodotWallpaperService.mNumContext+")");
		//if (GodotWallpaperService.mNumContext == 2){
		if (GodotLib.mIsInitialized){			
			GodotLib.newcontext();
			Log.d("Godot", "GodotLib.newcontext() - NEW CONTEXT");
		}
		/*if (GodotWallpaperService.mNumContext == 1){
			Log.d("Godot", "GodotLib.newcontext() - NEW CONTEXT");
			GodotLib.newcontext();
		}else{
			GodotLib.newcontext2();
		}*/
		
	}
}

