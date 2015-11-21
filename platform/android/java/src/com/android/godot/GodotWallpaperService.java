package com.android.godot;

import net.rbgrn.android.glwallpaperservice.*;

// Original code provided by Robert Green
// http://www.rbgrn.net/content/354-glsurfaceview-adapted-3d-live-wallpapers
public class GodotWallpaperService extends GLWallpaperService {
	public GodotWallpaperService() {
		super();
	}
	public Engine onCreateEngine() {
		GodotWallpaperEngine engine = new GodotWallpaperEngine();
		return engine;
	}

	class GodotWallpaperEngine extends GLEngine {
		GodotWallpaperTestRenderer renderer;
		public GodotWallpaperEngine() {
			super();
			// handle prefs, other initialization
			renderer = new GodotWallpaperTestRenderer();
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
	}
}
