package com.android.godot;

import net.rbgrn.android.glwallpaperservice.*;

// Original code provided by Robert Green
// http://www.rbgrn.net/content/354-glsurfaceview-adapted-3d-live-wallpapers
public class GodotWallpaperTestService extends GLWallpaperService {
	public GodotWallpaperTestService() {
		super();
	}
	public Engine onCreateEngine() {
		GodotWallpaperTestEngine engine = new GodotWallpaperTestEngine();
		return engine;
	}

	class GodotWallpaperTestEngine extends GLEngine {
		GodotWallpaperTestRenderer renderer;
		public GodotWallpaperTestEngine() {
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
