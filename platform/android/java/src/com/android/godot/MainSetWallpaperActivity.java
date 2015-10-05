package com.android.godot;


import android.app.Activity;
import android.app.WallpaperManager;
import android.content.ComponentName;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;

/*
 * Since a Wallpaper is a service it does not appear in the Activity app drawer.
 * Users must long press the home screen to set a Live Wallpaper.  This wrapper
 * Activity will launch the Wallpaper in preview mode via the standard Android
 * app drawer. It's a GUI window-less Activity that kills itself in onCreate().
 */
public class MainSetWallpaperActivity extends Activity {
	
	  @Override
	  protected void onCreate(Bundle savedInstanceState) {
	    super.onCreate(savedInstanceState);
	    this.launchWallpaperPreview();
	    this.finish();
	  }
	  public void launchWallpaperPreview() {
		    Intent intent = new Intent(WallpaperManager.ACTION_CHANGE_LIVE_WALLPAPER);
		    intent.putExtra(WallpaperManager.EXTRA_LIVE_WALLPAPER_COMPONENT,
		        new ComponentName(this, GodotWallpaperService.class));
		    startActivity(intent);
	  }
}

