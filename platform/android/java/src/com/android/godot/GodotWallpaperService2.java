package com.android.godot;

import android.view.SurfaceHolder;
import android.app.ActivityManager;
import android.content.pm.ConfigurationInfo;
import android.opengl.GLSurfaceView.Renderer;
import android.content.Context;
import com.android.godot.LessonOneRenderer;
import com.android.godot.GLWallpaperService2;

///////////////////
// GODOT IMPORTS //
///////////////////
import android.provider.Settings.Secure;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;
import android.util.Log;

public class GodotWallpaperService2 extends GLWallpaperService2 {
	static public GodotIO io;
	public boolean godot_initialized=false;

    @Override
    public Engine onCreateEngine() {
		Log.d("Godot", "GodotWallpaperService2::onCreateEngine()");
        return new GodotWallpaperEngine();
    }
    
    
    //private static class Renderer implements GLSurfaceView.Renderer {
    class WallpaperRenderer implements Renderer {
    	public int step_count = 0; // for debug
    	private boolean firsttime=true;

		public void onDrawFrame(GL10 gl) {
			if (step_count % 100 == 0) Log.d("Godot", "GodotLib::step(): "+step_count);
			GodotLib.step();
			step_count++;
		}

		public void onSurfaceChanged(GL10 gl, int width, int height) {
			Log.d("Godot", "WallpaperRenderer::onSurfaceChanged()");
			Log.d("Godot", "GodotLib::resize()");
			GodotLib.resize(width, height,!firsttime);
			firsttime=false;
		}

		public void onSurfaceCreated(GL10 gl, EGLConfig config) {
			Log.d("Godot", "WallpaperRenderer::onSurfaceCreated()");
			Log.d("Godot", "GodotLib::newcontext()");
			GodotLib.newcontext();
		}
	}    
    
 
    class GodotWallpaperEngine extends GLWallpaperService2.GLEngine {
    	private String[] command_line;
    	
    	private void initializeGodot() {
    		Log.d("Godot", "GodotWallpaperEngine::initializeGodot()");
    		
			io = new GodotIO(GodotWallpaperService2.this);
			io.unique_id = Secure.getString(getContentResolver(), Secure.ANDROID_ID);
			GodotLib.io=io;
			Log.d("Godot", "GodotLib::initializeWallpaper2()");
			GodotLib.initializeWallpaper2(GodotWallpaperService2.this,io.needsReloadHooks(),command_line,getAssets());
			GodotWallpaperService2.this.godot_initialized=true;
    	}
 
        @Override
        public void onCreate(SurfaceHolder surfaceHolder) {
			Log.d("Godot", "GodotWallpaperEngine::onCreate()");

            super.onCreate(surfaceHolder);
 
            // Check if the system supports OpenGL ES 2.0.
            final ActivityManager activityManager =
                (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
            final ConfigurationInfo configurationInfo =
                activityManager.getDeviceConfigurationInfo();
            final boolean supportsEs2 =
                configurationInfo.reqGlEsVersion >= 0x20000;
 
            if (supportsEs2)
            {
                // Request an OpenGL ES 2.0 compatible context.
                setEGLContextClientVersion(2);
 
                // On Honeycomb+ devices, this improves the performance when
                // leaving and resuming the live wallpaper.
                setPreserveEGLContextOnPause(true);
                
                this.initializeGodot(); // Now that there is a surface init!
                
                // Set the renderer to our user-defined renderer.
                setRenderer(getNewRenderer());
            }
            else
            {
                // This is where you could create an OpenGL ES 1.x compatible
                // renderer if you wanted to support both ES 1 and ES 2.
                return;
            }
        }        
        
        
        
        /////////////////////
        // GODOT FUNCTIONS //
        /////////////////////
		@Override
		public void onVisibilityChanged(boolean visible) {
			Log.d("Godot", "GodotWallpaperEngine::onVisibilityChanged()");

		    super.onVisibilityChanged(visible);

		    if (rendererHasBeenSet && GodotWallpaperService2.this.godot_initialized ) {
		        if (visible) {
		            // Associated wih onResume()
					Log.d("Godot", "GodotLib::focusin()");
		    		GodotLib.focusin();
		        } else {
		            // Associated with onPause()
					Log.d("Godot", "GodotLib::focusout()");
		    		GodotLib.focusout();
		        }
		    }
		}
        
    }
 
    //abstract Renderer getNewRenderer();
    Renderer getNewRenderer() {
		Log.d("Godot", "GodotWallpaperService2::getNewRenderer()");
        //return new LessonOneRenderer();
        return new WallpaperRenderer();        
    }
    
    
    
	public void onDestroy(){
		Log.d("Godot", "GodotWallpaperService2::onDestroy()");
		super.onDestroy();
	}

	public void onVideoInit(boolean use_gl2) {
		Log.d("Godot", "GodotWallpaperService2::onVideoInit()");
		//mView = new GodotView(getApplication(),io,use_gl2);
		//setContentView(mView);
	}
	
    
}

