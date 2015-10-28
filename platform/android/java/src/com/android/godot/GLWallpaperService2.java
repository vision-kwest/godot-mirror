package com.android.godot;

import android.service.wallpaper.WallpaperService;
import android.opengl.GLSurfaceView;
import android.opengl.GLSurfaceView.Renderer;
import android.view.SurfaceHolder;
import android.content.Context;
import android.util.Log;


/*
 * Since a Wallpaper is a service it does not appear in the Activity app drawer.
 * Users must long press the home screen to set a Live Wallpaper.  This wrapper
 * Activity will launch the Wallpaper in preview mode via the standard Android
 * app drawer. It's a GUI window-less Activity that kills itself in onCreate().
 */
public abstract class GLWallpaperService2 extends WallpaperService {
	public class GLEngine extends Engine {
	
		class WallpaperGLSurfaceView extends GLSurfaceView {
		    private static final String TAG = "WallpaperGLSurfaceView";
		 
		    WallpaperGLSurfaceView(Context context) {
		        super(context);
				Log.d("Godot", "WallpaperGLSurfaceView::WallpaperGLSurfaceView()");
		    }
		 
		    @Override
		    public SurfaceHolder getHolder() {
				Log.d("Godot", "WallpaperGLSurfaceView::getHolder()");
		        return getSurfaceHolder();
		    }
		 
		    
		    public void onDestroy() {
				Log.d("Godot", "WallpaperGLSurfaceView::onDestroy()");
		        super.onDetachedFromWindow();
		    }
		}
		
		
		private static final String TAG = "GLEngine";

		private WallpaperGLSurfaceView glSurfaceView;
		public boolean rendererHasBeenSet = false;  // made public so WallpaperService can see
		 
		@Override
		public void onCreate(SurfaceHolder surfaceHolder) {
			Log.d("Godot", "GLEngine::onCreate()");
		    super.onCreate(surfaceHolder);
		    glSurfaceView = new WallpaperGLSurfaceView(GLWallpaperService2.this);
		}
		
		@Override
		public void onVisibilityChanged(boolean visible) {
			Log.d("Godot", "GLEngine::onVisibilityChanged()");
		    super.onVisibilityChanged(visible);
		 
		    if (rendererHasBeenSet) {
		        if (visible) {
		            glSurfaceView.onResume();
		        } else {
		            glSurfaceView.onPause();            
		        }
		    }
		}
		
		@Override
		public void onDestroy() {
			Log.d("Godot", "GLEngine::onDestroy()");
		    super.onDestroy();
		    glSurfaceView.onDestroy();
		}
		
		protected void setRenderer(Renderer renderer) {
			Log.d("Godot", "GLEngine::setRenderer()");
			glSurfaceView.setEGLConfigChooser(8 , 8, 8, 8, 16, 0); // added to get drawing in emulator
			glSurfaceView.setRenderer(renderer);
		    rendererHasBeenSet = true;
		}
		 
		protected void setEGLContextClientVersion(int version) {
			Log.d("Godot", "GLEngine::setEGLContextClientVersion()");
		    glSurfaceView.setEGLContextClientVersion(version);
		}
		 
		protected void setPreserveEGLContextOnPause(boolean preserve) {
			Log.d("Godot", "GLEngine::setPreserveEGLContextOnPause()");
		    glSurfaceView.setPreserveEGLContextOnPause(preserve);
		}
		
	    
	    
	    
	    /////////////////////
	    // GODOT FUNCTIONS //
	    /////////////////////

		
		
	}
}

