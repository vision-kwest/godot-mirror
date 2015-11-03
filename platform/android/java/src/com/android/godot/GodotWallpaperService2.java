package com.android.godot;

import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.app.ActivityManager;
import android.content.pm.ConfigurationInfo;
import android.opengl.GLSurfaceView;
import android.opengl.GLSurfaceView.Renderer;
import android.content.Context;
import com.android.godot.LessonOneRenderer;
import com.android.godot.GLWallpaperService2.GLEngine.WallpaperGLSurfaceView;
import com.android.godot.GLWallpaperService2;

///////////////////
// GODOT IMPORTS //
///////////////////
import android.provider.Settings.Secure;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;
import android.util.Log;

import java.util.concurrent.Semaphore;

public class GodotWallpaperService2 extends GLWallpaperService2 implements GodotHelper{
	static public GodotIO io;
	public boolean godot_initialized=false;
	
	public static int engine_count = 0;
	public static int renderer_count = 0;
	
	public Semaphore godot_lock = new Semaphore(1);	
	public int focus_count = 0;


    @Override
    public Engine onCreateEngine() {
		Log.d("Godot", "GodotWallpaperService2::onCreateEngine()");
        return new GodotWallpaperEngine();
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
			GodotWallpaperService2.engine_count++;
			Log.d("Godot", "engine_id: " + this.engine_id);

            super.onCreate(surfaceHolder);
            this.initializeGodot(); // Now that there is a surface init!
        }        
        
        
		public WallpaperGLSurfaceView getGLSurfaceView(){
		    boolean use_gl2 = true;
		    boolean use_32_bits=false;
		    GLSurfaceView.Renderer renderer = new WallpaperRenderer(godot_lock);
		    return new WallpaperGLSurfaceView(GodotWallpaperService2.this, use_gl2, use_32_bits, GodotWallpaperService2.this, renderer);
		}
        
        /////////////////////
        // GODOT FUNCTIONS //
        /////////////////////
		@Override
		public void onVisibilityChanged(boolean visible) {
			Log.d("Godot", "GodotWallpaperEngine::onVisibilityChanged( "+ this.engine_id + ", " + visible + ", [" + GodotWallpaperService2.this.focus_count +"] )");

		    super.onVisibilityChanged(visible);

		    if (rendererHasBeenSet && GodotWallpaperService2.this.godot_initialized ) {
		        if (visible) {
		            // Associated wih onResume()
					if (focus_count == 0){ // handle overlapping calls to focusin()/focusout()
						Log.d("Godot", "GodotLib::focusin( "+ this.engine_id +" )");
						try {
							godot_lock.acquire();
						} catch (InterruptedException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
						GodotLib.focusin();
						godot_lock.release();
					}else{
						Log.d("Godot", "GodotLib::focusin( "+ this.engine_id +" ) -- SKIPPED COUNT: "+ GodotWallpaperService2.this.focus_count);
					}
					GodotWallpaperService2.this.focus_count++; // track every request to resume
		        } else {
		            // Associated with onPause()
		        	GodotWallpaperService2.this.focus_count--; // track every request to pause
		    		if (GodotWallpaperService2.this.focus_count == 0){ // handle overlapping calls to focusin()/focusout()
		    			Log.d("Godot", "GodotLib::focusout( "+ this.engine_id +" )");
						try {
							godot_lock.acquire();
						} catch (InterruptedException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
		    			GodotLib.focusout();
						godot_lock.release();
		    		}else{
						Log.d("Godot", "GodotLib::focusout( "+ this.engine_id +" ) -- SKIPPED COUNT: "+ GodotWallpaperService2.this.focus_count);
		    		}
		        }
		    }else{
				Log.d("Godot", "GodotWallpaperEngine::onVisibilityChanged( "+ this.engine_id +" ) -- SKIPPED( "+ rendererHasBeenSet +" && " + GodotWallpaperService2.this.godot_initialized + " )");		    			    	
		    }
		}
        
    }
 
    //abstract Renderer getNewRenderer();
    Renderer getNewRenderer() {
		Log.d("Godot", "GodotWallpaperService2::getNewRenderer()");
        //return new LessonOneRenderer();
        return new WallpaperRenderer(GodotWallpaperService2.this.godot_lock);        
    }
    
    
    
	public void onDestroy(){
		Log.d("Godot", "GodotWallpaperService2::onDestroy()");
		if (GodotWallpaperService2.this.godot_initialized){
			try {
				godot_lock.acquire();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			Log.d("Godot", "GodotLib::quit()");
			GodotLib.quit();
			Log.d("Godot", "GodotLib::step()");
			GodotLib.step();
			godot_lock.release();
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

	@Override
	public boolean gotTouchEvent(MotionEvent event) {
		// TODO Auto-generated method stub
		return false;
    }
    
}

