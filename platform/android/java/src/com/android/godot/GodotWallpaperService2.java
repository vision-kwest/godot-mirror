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
	
	public static int engine_count = 0;
	public static int renderer_count = 0;

	
	public int curr_engine_id   = -1;
	public int curr_renderer_id = -1;

    @Override
    public Engine onCreateEngine() {
		Log.d("Godot", "GodotWallpaperService2::onCreateEngine()");
        return new GodotWallpaperEngine();
    }
    
    
    //private static class Renderer implements GLSurfaceView.Renderer {
    class WallpaperRenderer implements Renderer {
    	public int step_count = 0; // for debug
    	private boolean firsttime=true;
    	public int renderer_id = -1;
    	   	
		public void onDrawFrame(GL10 gl) {
		    if (GodotWallpaperService2.this.curr_renderer_id == this.renderer_id){
				//if (step_count % 100 == 0) Log.d("Godot", "GodotLib::step(): "+step_count);
				if (step_count % 1 == 0) Log.d("Godot", "GodotLib::step(): "+step_count);
				GodotLib.step();
				step_count++;
		    }else{
				Log.d("Godot", "WallpaperRenderer::onDrawFrame() -- SKIPPED( "+ this.renderer_id +" )");		    	
		    }
		}

		public void onSurfaceChanged(GL10 gl, int width, int height) {
		    if (GodotWallpaperService2.this.curr_renderer_id == this.renderer_id){
				Log.d("Godot", "WallpaperRenderer::onSurfaceChanged()");
				Log.d("Godot", "GodotLib::resize()");
				GodotLib.resize(width, height,!firsttime);
				firsttime=false;
		    }else{
				Log.d("Godot", "WallpaperRenderer::onDrawFrame() -- SKIPPED( "+ this.renderer_id +" )");		    	
		    }
		}

		public void onSurfaceCreated(GL10 gl, EGLConfig config) {
			Log.d("Godot", "WallpaperRenderer::onSurfaceCreated()");
			Log.d("Godot", "GodotLib::newcontext()");
			
			this.renderer_id = GodotWallpaperService2.renderer_count;
			GodotWallpaperService2.this.curr_renderer_id = this.renderer_id;
			GodotWallpaperService2.renderer_count++;
			Log.d("Godot", "renderer_id: " + this.renderer_id);
			
			GodotLib.newcontext();
		}
	}    
    
 
    class GodotWallpaperEngine extends GLWallpaperService2.GLEngine {
    	private String[] command_line;
    	public int engine_id = -1;
    	
    	private void initializeGodot() {
    		if (!GodotWallpaperService2.this.godot_initialized){
	    		Log.d("Godot", "GodotWallpaperEngine::initializeGodot()");
	    		
				io = new GodotIO(GodotWallpaperService2.this);
				io.unique_id = Secure.getString(getContentResolver(), Secure.ANDROID_ID);
				GodotLib.io=io;
				Log.d("Godot", "GodotLib::initializeWallpaper2()");
				boolean use_reload_hooks = true; // even when droid sdk < 11
				GodotLib.initializeWallpaper2(GodotWallpaperService2.this,use_reload_hooks,command_line,getAssets());
				GodotWallpaperService2.this.godot_initialized=true;
    		}else{
    			Log.d("Godot", "GodotWallpaperEngine::initializeGodot() -- SKIPPED");
    			// Godot Engine is a singleton so we only call initialize() once.
    			// If it is used in a new context,
    			// we call newcontext()
    		}
    	}
 
        @Override
        public void onCreate(SurfaceHolder surfaceHolder) {
			
			this.engine_id = GodotWallpaperService2.engine_count;
			GodotWallpaperService2.this.curr_engine_id = this.engine_id;
			GodotWallpaperService2.engine_count++;
			Log.d("Godot", "engine_id: " + this.engine_id);

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
			Log.d("Godot", "GodotWallpaperEngine::onVisibilityChanged( "+ this.engine_id + ", " + visible + " )");

		    super.onVisibilityChanged(visible);

		    if (GodotWallpaperService2.this.curr_engine_id == this.engine_id){
			    if (rendererHasBeenSet && GodotWallpaperService2.this.godot_initialized ) {
			        if (visible) {
			            // Associated wih onResume()
						Log.d("Godot", "GodotLib::focusin( "+ this.engine_id +" )");
			    		GodotLib.focusin();
			        } else {
			            // Associated with onPause()
						Log.d("Godot", "GodotLib::focusout( "+ this.engine_id +" )");
			    		GodotLib.focusout();
			        }
			    }else{
					Log.d("Godot", "GodotWallpaperEngine::onVisibilityChanged( "+ this.engine_id +" ) -- SKIPPED( "+ rendererHasBeenSet +" && " + GodotWallpaperService2.this.godot_initialized + " )");		    			    	
			    }
		    }else{
				Log.d("Godot", "GodotWallpaperEngine::onVisibilityChanged() -- SKIPPED( "+ this.engine_id +" )");		    	
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
		if (GodotWallpaperService2.this.godot_initialized){
			Log.d("Godot", "GodotLib::quit()");
			GodotLib.quit();
			Log.d("Godot", "GodotLib::step()");
			GodotLib.step();
		}
		super.onDestroy();
	}

	public void onVideoInit(boolean use_gl2) {
		Log.d("Godot", "GodotWallpaperService2::onVideoInit()");
		//mView = new GodotView(getApplication(),io,use_gl2);
		//setContentView(mView);
	}
	public void forceQuit() {
		Log.d("Godot", "GodotWallpaperService2::forceQuit()");
		// Godot callback that does the real quitting. Is called by step() after quit().
		System.exit(0);
	}
    
}

