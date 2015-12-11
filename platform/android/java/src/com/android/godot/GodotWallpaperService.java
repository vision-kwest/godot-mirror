package com.android.godot;

import android.opengl.GLSurfaceView;
import android.provider.Settings.Secure;
import android.view.SurfaceHolder;
import net.rbgrn.android.glwallpaperservice.*;

// Original code provided by Robert Green
// http://www.rbgrn.net/content/354-glsurfaceview-adapted-3d-live-wallpapers
public class GodotWallpaperService extends GLWallpaperService {
    static public BaseIO io=null;
    public boolean godot_initialized=false;
    static public int current_gl_thread;
    static public boolean use_32_bit=false;    

	public GodotWallpaperService() {
		super();
	}
	public Engine onCreateEngine() {
		GodotWallpaperEngine engine = new GodotWallpaperEngine();
		return engine;
	}
    public void onVideoInit(boolean use_gl2) {
        //mView = new GodotView(getApplication(),io,use_gl2);
        //setContentView(mView);
    }
    public void forceQuit() {
        // Godot callback that does the real quitting. Is called by step() after quit().
        System.exit(0);
    }
    
    public void onDestroy(){
    	if (GodotWallpaperService.this.godot_initialized){
    		GodotLib.quit();
    		GodotLib.step();
        }
    	super.onDestroy();
    }

	class GodotWallpaperEngine extends GLEngine {
		GodotWallpaperRenderer renderer;
        private String[] command_line;

		public GodotWallpaperEngine() {
			super();
			// handle prefs, other initialization
			renderer = new GodotWallpaperRenderer();
			setRenderer(renderer);
			setRenderMode(RENDERMODE_CONTINUOUSLY);
		}

		public void onDestroy() {
			super.onDestroy();
			if (renderer != null) {
				renderer.release(); // assuming yours has this method - it should!
			}
			renderer = null;
		}
		
        public GLSurfaceView getGLSurfaceView() {
            boolean use_gl2 = true;
            boolean use_32_bits=GodotWallpaperService.use_32_bit;
            boolean use_reload=BaseIO.needsReloadHooks();
        	
        	// Sub-classes that need a special version of GLSurfaceView can override this method.
        	return new GLBaseView(GodotWallpaperService.this, use_gl2, use_32_bits, use_reload){
                @Override
                public SurfaceHolder getHolder() {
                    return GodotWallpaperEngine.this.getSurfaceHolder();
                }
        	};
        }
        
        public void surfaceCreatedCallBack() {
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
        public void resumeCallBack() {
        	GodotLib.focusin();
        }
        public void pauseCallBack() {
        	GodotLib.focusout();
        }
	}
}
