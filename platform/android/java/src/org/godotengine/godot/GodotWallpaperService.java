package org.godotengine.godot;

import android.opengl.GLSurfaceView;
import android.provider.Settings.Secure;
import android.view.SurfaceHolder;
import android.util.Log;
import android.view.MotionEvent;
import android.content.Context;
import android.content.ContextWrapper;

// Original code provided by Robert Green
// http://www.rbgrn.net/content/354-glsurfaceview-adapted-3d-live-wallpapers
public class GodotWallpaperService extends GLWallpaperService {
    private static final String LOG_TAG = "GodotWallpaperService";
    static public GodotIO io=null;
    public boolean godot_initialized=false;
    static public int current_gl_thread;
    static public boolean use_32_bit=false;    
    private String[] command_line;

	GodotView.Renderer mRenderer;

	public GodotWallpaperService() {
		super();
        Log.v(LOG_TAG, "GodotWallpaperService.GodotWallpaperService()");
	}
	public Engine onCreateEngine() {
        Log.v(LOG_TAG, "GodotWallpaperService.onCreateEngine()");
		GodotWallpaperEngine engine = new GodotWallpaperEngine();
		return engine;
	}
    public void forceQuit() {
        Log.v(LOG_TAG, "GodotWallpaperService.forceQuit()");
        // Godot callback that does the real quitting. Is called by step() after quit().
        System.exit(0);
    }
    
    public void onDestroy(){
        Log.v(LOG_TAG, "GodotWallpaperService.onDestroy()");
        // Kill renderer
/*        if (mRenderer != null) {
        	mRenderer.release(); // assuming yours has this method - it should!
        }*/
        mRenderer = null;
    	
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
	    	
			io = new GodotIO(GodotWallpaperService.this);
			io.unique_id = Secure.getString(getContentResolver(), Secure.ANDROID_ID);
			GodotLib.io=io;
			boolean use_reload_hooks = true; // even when droid sdk < 11
			GodotLib.initializewallpaper(GodotWallpaperService.this,io.needsReloadHooks(),command_line,getAssets());
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
    	
	
	public void onVideoInit(boolean use_gl2) {

//		mView = new GodotView(getApplication(),io,use_gl2);
//		setContentView(mView);

/*		layout = new FrameLayout(this);
		layout.setLayoutParams(new LayoutParams(LayoutParams.FILL_PARENT,LayoutParams.FILL_PARENT));
		setContentView(layout);
		
		// GodotEditText layout
		GodotEditText edittext = new GodotEditText(this); 
		   edittext.setLayoutParams(new ViewGroup.LayoutParams(LayoutParams.FILL_PARENT,LayoutParams.WRAP_CONTENT));
        // ...add to FrameLayout
		   layout.addView(edittext);
		
		mView = new GodotView(getApplication(),io,use_gl2,use_32_bits);
		layout.addView(mView,new LayoutParams(LayoutParams.FILL_PARENT,LayoutParams.FILL_PARENT));
		setKeepScreenOn(GodotLib.getGlobal("display/keep_screen_on").equals("True"));
		
        edittext.setView(mView);
        io.setEdit(edittext);
		
		// Ad layout
		adLayout = new RelativeLayout(this);
		adLayout.setLayoutParams(new LayoutParams(LayoutParams.FILL_PARENT,LayoutParams.FILL_PARENT));
		layout.addView(adLayout);*/
		
	}

	public void setKeepScreenOn(final boolean p_enabled) {
/*		keep_screen_on = p_enabled;
		if (mView != null){
			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					mView.setKeepScreenOn(p_enabled);
				}
			});
		}*/
	}	
	
	class GodotWallpaperEngine extends GLEngine {

		public GodotWallpaperEngine() {
			super();
	        Log.v(LOG_TAG, "GodotWallpaperEngine.GodotWallpaperEngine("+mId+")");
	        // No need to call setRenderer() because it will happen at 
	        // GodotView constructor-time. The RENDERMODE_CONTINUOUSLY setting 
	        // is the default. By the time GodotView.GodotView() is called there
	        // will be a Surface
/*            if (mGLSurfaceView == null) {
				// handle prefs, other initialization
				renderer = new GodotView.Renderer();
				GodotWallpaperService.this.setRenderer(renderer);
				GodotWallpaperService.this.setRenderMode(RENDERMODE_CONTINUOUSLY);
			}*/
		}

		public void onDestroy() {
			super.onDestroy();
	        Log.v(LOG_TAG, "GodotWallpaperEngine.onDestroy("+mId+")");
		}
		
        public GLSurfaceView getGLSurfaceView() {
            Log.v(LOG_TAG, "GodotWallpaperEngine.getGLSurfaceView("+mId+")");
            boolean use_gl2 = true;
            boolean use_32_bits=GodotWallpaperService.use_32_bit;
        	
        	// Sub-classes that need a special version of GLSurfaceView can override this method.
            Context context = (Context) GodotWallpaperService.this;
            // Note: GodotWallpaperService.this.io gets initialized just before getGLSurfaceView() is called
            GodotView retval = new GodotView(context,GodotWallpaperService.this.io,use_gl2,use_32_bits){
								@Override
								public SurfaceHolder getHolder() {
									    Log.v(LOG_TAG, "GodotViewSubClass.getHolder("+mId+")");
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
			// Now we can know who the renderer is.
			mRenderer = retval.mRenderer;
			return retval;
        }
        
        @Override
        public void onTouchEvent(MotionEvent event) {
        	/*
        	 * Forward the handling of user touch events to the single "global"
        	 * renderer.
        	 */
            super.onTouchEvent(event);
            Log.v(LOG_TAG, "GodotWallpaperEngine.onTouchEvent()");
            //boolean retval = true;
        	if (mRenderer != null){ 
	            //retval = mRenderer.onTouchEvent(event);
	            mRenderer.onTouchEvent(event);	            
        	}
        	//return retval;
        }    
	}
}
