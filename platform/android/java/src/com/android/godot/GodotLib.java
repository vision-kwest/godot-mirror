/*************************************************************************/
/*  GodotLib.java                                                        */
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

// Wrapper for native library

public class GodotLib {


     //public static GodotIO io;

     static {
	 System.loadLibrary("godot_android");
     }

    /**
     * @param width the current view width
     * @param height the current view height
     */

     public native void initialize(Godot p_instance,boolean need_reload_hook,String[] p_cmdline);
     public native void initializeWallpaper(GodotWallpaperService p_instance,boolean need_reload_hook,String[] p_cmdline);
     public native void resize(int width, int height,boolean reload);
     public native void newcontext();
     public native void quit();
     public native void step();
     public native void touch(int what,int pointer,int howmany, int[] arr);
     public native void accelerometer(float x, float y, float z);
	 public native void key(int p_scancode, int p_unicode_char, boolean p_pressed);
	 public native void joybutton(int p_device, int p_but, boolean p_pressed);
	 public native void joyaxis(int p_device, int p_axis, float p_value);
     public native void focusin();
     public native void focusout();
     public native void audio();
     public native void singleton(String p_name,Object p_object);
     public native void method(String p_sname,String p_name,String p_ret,String[] p_params);
     public native String getGlobal(String p_key);
	public native void callobject(int p_ID, String p_method, Object[] p_params);
	public native void calldeferred(int p_ID, String p_method, Object[] p_params);

}
