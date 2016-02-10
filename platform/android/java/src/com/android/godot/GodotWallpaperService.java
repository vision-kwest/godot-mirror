package com.android.godot;

import android.opengl.GLSurfaceView;
import android.provider.Settings.Secure;
import android.view.SurfaceHolder;
import net.rbgrn.android.glwallpaperservice.*;
import android.util.Log;

// Original code provided by Robert Green
// http://www.rbgrn.net/content/354-glsurfaceview-adapted-3d-live-wallpapers
public class GodotWallpaperService extends GLWallpaperService {
    private static final String LOG_TAG = "GodotWallpaperService";
    static public BaseIO io=null;
    public boolean godot_initialized=false;
    static public int current_gl_thread;
    static public boolean use_32_bit=false;    
    private String[] command_line;

	GodotWallpaperRenderer renderer;

	public GodotWallpaperService() {
		super();
        Log.v(LOG_TAG, "GodotWallpaperService.GodotWallpaperService()");
	}
	public Engine onCreateEngine() {
        Log.v(LOG_TAG, "GodotWallpaperService.onCreateEngine()");
		GodotWallpaperEngine engine = new GodotWallpaperEngine();
		return engine;
	}
    public void onVideoInit(boolean use_gl2) {
        Log.v(LOG_TAG, "GodotWallpaperService.onVideoInit()");
        //mView = new GodotView(getApplication(),io,use_gl2);
        //setContentView(mView);
    }
    public void forceQuit() {
        Log.v(LOG_TAG, "GodotWallpaperService.forceQuit()");
        // Godot callback that does the real quitting. Is called by step() after quit().
        System.exit(0);
    }
    
    public void onDestroy(){
        Log.v(LOG_TAG, "GodotWallpaperService.onDestroy()");
        // Kill renderer
        if (renderer != null) {
        	renderer.release(); // assuming yours has this method - it should!
        }
        renderer = null;
    	
    	if (GodotWallpaperService.this.godot_initialized){
    		GodotLib.quit();
    		GodotLib.step();
        }
    	super.onDestroy();
    }

	@Override
	public void surfaceCreatedCallBack() {
	    Log.v(LOG_TAG, "GodotWallpaperEngine.surfaceCreatedCallBack()");
		// Now that we have surface, we can init GL context
	    if (!GodotWallpaperService.this.godot_initialized){
	    	
	    	// In the Godot (Activity) the '-use_depth_32' command-line flag can set this here
	    	GodotWallpaperService.use_32_bit = true;  // setting this to 'true' so we get a emulator friendly surface
	    	
			io = new BaseIO(GodotWallpaperService.this);
			io.unique_id = Secure.getString(getContentResolver(), Secure.ANDROID_ID);
			//GodotLib.io=io;
			boolean use_reload_hooks = true; // even when droid sdk < 11
			GodotLib.initializewallpaper(GodotWallpaperService.this,use_reload_hooks,command_line,getAssets());
			GodotWallpaperService.this.godot_initialized=true;
	    }else{
	        // Godot Engine is a singleton so we only call initialize() once.
	        // If it is used in a new context,
	        // we call newcontext()
	    }
	}
	
	// Handle onVisibilityChanged()
	@Override
	public void resumeCallBack() {
	    Log.v(LOG_TAG, "GodotWallpaperEngine.resumeCallBack()");
		GodotLib.focusin();
	}
	
	@Override
	public void pauseCallBack() {
	    Log.v(LOG_TAG, "GodotWallpaperEngine.pauseCallBack()");
		GodotLib.focusout();
	}
    
	class GodotWallpaperEngine extends GLEngine {

		public GodotWallpaperEngine() {
			super();
	        Log.v(LOG_TAG, "GodotWallpaperEngine.GodotWallpaperEngine("+mId+")");
            if (mGLSurfaceView == null) {
				// handle prefs, other initialization
				renderer = new GodotWallpaperRenderer();
				GodotWallpaperService.this.setRenderer(renderer);
				GodotWallpaperService.this.setRenderMode(RENDERMODE_CONTINUOUSLY);
			}
		}

		public void onDestroy() {
			super.onDestroy();
	        Log.v(LOG_TAG, "GodotWallpaperEngine.onDestroy("+mId+")");
		}
		
        public GLSurfaceView getGLSurfaceView() {
            Log.v(LOG_TAG, "GodotWallpaperEngine.getGLSurfaceView("+mId+")");
            boolean use_gl2 = true;
            boolean use_32_bits=GodotWallpaperService.use_32_bit;
            boolean use_reload=BaseIO.needsReloadHooks();
        	
        	// Sub-classes that need a special version of GLSurfaceView can override this method.
        	return new GLBaseView(GodotWallpaperService.this, use_gl2, use_32_bits, use_reload){
                @Override
                public SurfaceHolder getHolder() {
                    Log.v(LOG_TAG, "GLBaseViewSubClass.getHolder("+mId+")");
                    /*
                    When the View tries to get a Surface, it should forward that ask to a
                    WallpaperService.Engine so that it can draw on a Surface that's attached to a
                    window on the screen.  In the old design, every View had its own Engine to grab
                    a Surface from.  In the new design, there is only one View, so it must know
                    which Engine's Surface to draw to and at the right time.  The Service is long
                    lived so it will know the proper Surface to use.  Engines should set the current
                    Surface on the Service when they are created/come into focus.  Views are not
                    really "created" until an Engine Surface has been setup, at onSurfaceCreated()
                    time.  Calls to the View are cached until this time and then they are executed
                    in batch.
                     */
                    return GodotWallpaperService.this.mSurfaceHolder;
                }
        	};
        }
	}
}
