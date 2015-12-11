/*************************************************************************/
/*  GodotView.java                                                       */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                    http://www.godotengine.org                         */
/*************************************************************************/
/* Copyright (c) 2007-2015 Juan Linietsky, Ariel Manzur.                 */
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
import android.opengl.GLSurfaceView;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.InputDevice;

import javax.microedition.khronos.egl.EGLConfig;
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
public class GodotView extends GLBaseView {

	private static String TAG = "GodotView";

	private static boolean firsttime=true;

	private Godot activity;

	public GodotView(Context context,boolean p_use_gl2, boolean p_use_32_bits, Godot p_activity) {
		super(context, p_use_gl2, p_use_32_bits, GodotIO.needsReloadHooks());
		this.setRenderer(new Renderer());
		activity = p_activity;
    }

    public GodotView(Context context, boolean translucent, int depth, int stencil) {
		super(context, translucent, depth, stencil);
        this.setRenderer(new Renderer());
    }

	@Override public boolean onTouchEvent (MotionEvent event) {

		return activity.gotTouchEvent(event);
	};

	public int get_godot_button(int keyCode) {

		int button = 0;
		switch (keyCode) {
			case KeyEvent.KEYCODE_BUTTON_A: // Android A is SNES B
				button = 0;
				break;
			case KeyEvent.KEYCODE_BUTTON_B:
				button = 1;
				break;
			case KeyEvent.KEYCODE_BUTTON_X: // Android X is SNES Y
				button = 2;
				break;
			case KeyEvent.KEYCODE_BUTTON_Y:
				button = 3;
				break;
			case KeyEvent.KEYCODE_BUTTON_L1:
				button = 4;
				break;
			case KeyEvent.KEYCODE_BUTTON_L2:
				button = 6;
				break;
			case KeyEvent.KEYCODE_BUTTON_R1:
				button = 5;
				break;
			case KeyEvent.KEYCODE_BUTTON_R2:
				button = 7;
				break;
			case KeyEvent.KEYCODE_BUTTON_SELECT:
				button = 10;
				break;
			case KeyEvent.KEYCODE_BUTTON_START:
				button = 11;
				break;
			case KeyEvent.KEYCODE_BUTTON_THUMBL:
				button = 8;
				break;
			case KeyEvent.KEYCODE_BUTTON_THUMBR:
				button = 9;
				break;
			case KeyEvent.KEYCODE_DPAD_UP:
				button = 12;
				break;
			case KeyEvent.KEYCODE_DPAD_DOWN:
				button = 13;
				break;
			case KeyEvent.KEYCODE_DPAD_LEFT:
				button = 14;
				break;
			case KeyEvent.KEYCODE_DPAD_RIGHT:
				button = 15;
				break;

			default:
				button = keyCode - KeyEvent.KEYCODE_BUTTON_1;
				break;
		};

		return button;
	};

	@Override public boolean onKeyUp(int keyCode, KeyEvent event) {

		if (keyCode == KeyEvent.KEYCODE_BACK) {
			return true;
		}

		if (keyCode == KeyEvent.KEYCODE_VOLUME_UP || keyCode == KeyEvent.KEYCODE_VOLUME_DOWN) {
			return super.onKeyUp(keyCode, event);
		};

		int source = event.getSource();
		if ((source & InputDevice.SOURCE_JOYSTICK) != 0 || (source & InputDevice.SOURCE_DPAD) != 0 || (source & InputDevice.SOURCE_GAMEPAD) != 0) {

			int button = get_godot_button(keyCode);
			int device = event.getDeviceId();

			GodotLib.joybutton(device, button, false);
			return true;
		} else {

			GodotLib.key(keyCode, event.getUnicodeChar(0), false);
		};
		return super.onKeyUp(keyCode, event);
	};

	@Override public boolean onKeyDown(int keyCode, KeyEvent event) {

		if (keyCode == KeyEvent.KEYCODE_BACK) {
			GodotLib.quit();
			// press 'back' button should not terminate program
			//	normal handle 'back' event in game logic
			return true;
		}

		if (keyCode == KeyEvent.KEYCODE_VOLUME_UP || keyCode == KeyEvent.KEYCODE_VOLUME_DOWN) {
			return super.onKeyDown(keyCode, event);
		};

		int source = event.getSource();
		//Log.e(TAG, String.format("Key down! source %d, device %d, joystick %d, %d, %d", event.getDeviceId(), source, (source & InputDevice.SOURCE_JOYSTICK), (source & InputDevice.SOURCE_DPAD), (source & InputDevice.SOURCE_GAMEPAD)));

		if ((source & InputDevice.SOURCE_JOYSTICK) != 0 || (source & InputDevice.SOURCE_DPAD) != 0 || (source & InputDevice.SOURCE_GAMEPAD) != 0) {

			if (event.getRepeatCount() > 0) // ignore key echo
				return true;
			int button = get_godot_button(keyCode);
			int device = event.getDeviceId();
			//Log.e(TAG, String.format("joy button down! button %x, %d, device %d", keyCode, button, device));

			GodotLib.joybutton(device, button, true);
			return true;

		} else {
			GodotLib.key(keyCode, event.getUnicodeChar(0), true);
		};
		return super.onKeyDown(keyCode, event);
	}

	public float axis_value(MotionEvent p_event, InputDevice p_device, int p_axis, int p_pos) {

		final InputDevice.MotionRange range = p_device.getMotionRange(p_axis, p_event.getSource());
		if (range == null)
			return 0;

		//Log.e(TAG, String.format("axis ranges %f, %f, %f", range.getRange(), range.getMin(), range.getMax()));

		final float flat = range.getFlat();
		final float value =
			p_pos < 0 ? p_event.getAxisValue(p_axis):
			p_event.getHistoricalAxisValue(p_axis, p_pos);

		final float absval = Math.abs(value);
		if (absval <= flat) {
			return 0;
		};

		final float ret = (value - range.getMin()) / range.getRange() * 2 - 1.0f;

		return ret;
	};

	float[] last_axis_values = { 0, 0, 0, 0, -1, -1 };
	boolean[] last_axis_buttons = { false, false, false, false, false, false }; // dpad up down left right, ltrigger, rtrigger

	public void process_axis_state(MotionEvent p_event, int p_pos) {

		int device_id = p_event.getDeviceId();
		InputDevice device = p_event.getDevice();
		float val;

		val = axis_value(p_event, device, MotionEvent.AXIS_X, p_pos);
		if (val != last_axis_values[0]) {
			last_axis_values[0] = val;
			//Log.e(TAG, String.format("axis moved! axis %d, value %f", 0, val));
			GodotLib.joyaxis(device_id, 0, val);
		};

		val = axis_value(p_event, device, MotionEvent.AXIS_Y, p_pos);
		if (val != last_axis_values[1]) {
			last_axis_values[1] = val;
			//Log.e(TAG, String.format("axis moved! axis %d, value %f", 1, val));
			GodotLib.joyaxis(device_id, 1, val);
		};

		val = axis_value(p_event, device, MotionEvent.AXIS_Z, p_pos);
		if (val != last_axis_values[2]) {
			last_axis_values[2] = val;
			//Log.e(TAG, String.format("axis moved! axis %d, value %f", 2, val));
			GodotLib.joyaxis(device_id, 2, val);
		};

		val = axis_value(p_event, device, MotionEvent.AXIS_RZ, p_pos);
		if (val != last_axis_values[3]) {
			last_axis_values[3] = val;
			//Log.e(TAG, String.format("axis moved! axis %d, value %f", 3, val));
			GodotLib.joyaxis(device_id, 3, val);
		};

		val = axis_value(p_event, device, MotionEvent.AXIS_LTRIGGER, p_pos);
		if (val != last_axis_values[4]) {
			last_axis_values[4] = val;
			if ((val != 0) != (last_axis_buttons[4])) {
				last_axis_buttons[4] = (val != 0);
				GodotLib.joybutton(device_id, 6, (val != 0));
			};
		};

		val = axis_value(p_event, device, MotionEvent.AXIS_RTRIGGER, p_pos);
		if (val != last_axis_values[5]) {
			last_axis_values[5] = val;
			if ((val != 0) != (last_axis_buttons[5])) {
				last_axis_buttons[5] = (val != 0);
				GodotLib.joybutton(device_id, 7, (val != 0));
			};
		};

		val = axis_value(p_event, device, MotionEvent.AXIS_HAT_Y, p_pos);

		if (last_axis_buttons[0] != (val > 0)) {
			last_axis_buttons[0] = val > 0;
			GodotLib.joybutton(device_id, 12, val > 0);
		};
		if (last_axis_buttons[1] != (val < 0)) {
			last_axis_buttons[1] = val < 0;
			GodotLib.joybutton(device_id, 13, val > 0);
		};

		val = axis_value(p_event, device, MotionEvent.AXIS_HAT_X, p_pos);
		if (last_axis_buttons[2] != (val < 0)) {
			last_axis_buttons[2] = val < 0;
			GodotLib.joybutton(device_id, 14, val < 0);
		};
		if (last_axis_buttons[3] != (val > 0)) {
			last_axis_buttons[3] = val > 0;
			GodotLib.joybutton(device_id, 15, val > 0);
		};
	};

	@Override public boolean onGenericMotionEvent(MotionEvent event) {

		if ((event.getSource() & InputDevice.SOURCE_JOYSTICK) == InputDevice.SOURCE_JOYSTICK && event.getAction() == MotionEvent.ACTION_MOVE) {

			// Process all historical movement samples in the batch
			final int historySize = event.getHistorySize();

			// Process the movements starting from the
			// earliest historical position in the batch
			for (int i = 0; i < historySize; i++) {
				// Process the event at historical position i
				process_axis_state(event, i);
			}

			// Process the current movement sample in the batch (position -1)
			process_axis_state(event, -1);
			return true;


		};

		return super.onGenericMotionEvent(event);
	};
   
	private static class Renderer implements GLSurfaceView.Renderer {


		public void onDrawFrame(GL10 gl) {
			GodotLib.step();
			for(int i=0;i<Godot.singleton_count;i++) {
				Godot.singletons[i].onGLDrawFrame(gl);
			}
		}

		public void onSurfaceChanged(GL10 gl, int width, int height) {

			GodotLib.resize(width, height,!firsttime);
			firsttime=false;
			for(int i=0;i<Godot.singleton_count;i++) {
				Godot.singletons[i].onGLSurfaceChanged(gl, width, height);
			}
		}

		public void onSurfaceCreated(GL10 gl, EGLConfig config) {
			GodotLib.newcontext(use_32);
		}
	}
}
