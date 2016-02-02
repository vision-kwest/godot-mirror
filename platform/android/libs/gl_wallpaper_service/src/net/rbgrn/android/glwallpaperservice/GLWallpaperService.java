/*
 * Copyright (c) 2011 Ben Gruver
 * All rights reserved.
 *
 * You may use this code at your option under the following BSD license
 * or Apache 2.0 license terms
 *
 * [The "BSD license"]
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * [The "Apache 2.0 license"]
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package net.rbgrn.android.glwallpaperservice;

import android.opengl.GLSurfaceView;
import android.service.wallpaper.WallpaperService;
import android.util.Log;
import android.view.SurfaceHolder;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;

public abstract class GLWallpaperService extends WallpaperService {
    private static final String LOG_TAG = "GLWallpaperService";

    private GLSurfaceView mGLSurfaceView = null;
    protected SurfaceHolder mSurfaceHolder = null;
    protected boolean mIsInitialized = false;
    private int mFocusCount = 0;
    private static int lastId = 0;

    @Override
    public void onDestroy() {
        Log.v(LOG_TAG, "GLWallpaperService.onDestroy()");
        mGLSurfaceView.surfaceDestroyed(mSurfaceHolder);
        super.onDestroy();
    }

    public interface Renderer extends GLSurfaceView.Renderer {
    }

    public class GLEngine extends WallpaperService.Engine {
	public final static int RENDERMODE_WHEN_DIRTY = 0;
	public final static int RENDERMODE_CONTINUOUSLY = 1;

        private Object lock = new Object();

        private int debugFlags;
        private int renderMode;
        protected int myId;

        /**
         * If we don't have a GLSurfaceView yet, then we queue up any operations that are requested, until the
         * GLSurfaceView is created.
         *
         * Initially, we created the glSurfaceView in the GLEngine constructor, and things seemed to work. However,
         * it turns out a few devices aren't set up to handle the surface related events at this point, and crash.
         *
         * This is a work around so that we can delay the creation of the GLSurfaceView until the surface is actually
         * created, so that the underlying code should be in a state to be able to handle the surface related events
         * that get fired when GLSurfaceView is created.
         */
        private List<Runnable> pendingOperations = new ArrayList<Runnable>();

        public GLEngine() {
            lastId += 1;
            myId = lastId;
            Log.v(LOG_TAG, "GLEngine.GLEngine("+myId+")");
        }

        public GLSurfaceView getGLSurfaceView() {
            Log.v(LOG_TAG, "GLEngine.getGLSurfaceView("+myId+")");
    		// Sub-classes that need a special version of GLSurfaceView can override this method.
        	return new GLSurfaceView(GLWallpaperService.this){
                @Override
                public SurfaceHolder getHolder() {
                    Log.v(LOG_TAG, "GLSurfaceViewSubClass.getHolder("+myId+")");
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
                    return GLWallpaperService.this.mSurfaceHolder;
                }
        	};
        }
        
        public void setGLWrapper(final GLSurfaceView.GLWrapper glWrapper) {
            synchronized (lock) {
                if (mGLSurfaceView != null) {
                    Log.v(LOG_TAG, "GLEngine.setGLWrapper("+myId+")");
                    mGLSurfaceView.setGLWrapper(glWrapper);
                } else {
                    pendingOperations.add(new Runnable() {
                        public void run() {
                            Log.v(LOG_TAG, "GLEngine.setGLWrapper("+myId+")");
                            setGLWrapper(glWrapper);
                        }
                    });
                }
            }
        }

        public void setDebugFlags(final int debugFlags) {
            synchronized (lock) {
                if (mGLSurfaceView != null) {
                    Log.v(LOG_TAG, "GLEngine.setDebugFlags("+myId+")");
                    mGLSurfaceView.setDebugFlags(debugFlags);
                } else {
                    this.debugFlags = debugFlags;
                    pendingOperations.add(new Runnable() {
                        public void run() {
                            Log.v(LOG_TAG, "GLEngine.setDebugFlags("+myId+")");
                            setDebugFlags(debugFlags);
                        }
                    });
                }
            }
        }

        public int getDebugFlags() {
            Log.v(LOG_TAG, "GLEngine.getDebugFlags("+myId+")");
            synchronized (lock) {
                if (mGLSurfaceView != null) {
                    return mGLSurfaceView.getDebugFlags();
                } else {
                    return debugFlags;
                }
            }
        }

        public void setRenderer(final GLSurfaceView.Renderer renderer) {
            synchronized (lock) {
                if (mGLSurfaceView != null) {
                    Log.v(LOG_TAG, "GLEngine.setRenderer("+myId+")");
                    mGLSurfaceView.setRenderer(renderer);
                    if (!isVisible()) {
                        mGLSurfaceView.onPause();
                    }
                } else {
                    pendingOperations.add(new Runnable() {
                        public void run() {
                            Log.v(LOG_TAG, "GLEngine.setRenderer("+myId+")*");
                            setRenderer(renderer);
                        }
                    });
                }
            }
        }

        public void queueEvent(final Runnable r) {
            synchronized (lock) {
                if (mGLSurfaceView != null) {
                    Log.v(LOG_TAG, "GLEngine.queueEvent("+myId+")");
                    mGLSurfaceView.queueEvent(r);
                } else {
                    pendingOperations.add(new Runnable() {
                        public void run() {
                            Log.v(LOG_TAG, "GLEngine.queueEvent("+myId+")");
                            queueEvent(r);
                        }
                    });
                }
            }
        }

        public void setEGLContextFactory(final GLSurfaceView.EGLContextFactory factory) {
            synchronized (lock) {
                if (mGLSurfaceView != null) {
                    Log.v(LOG_TAG, "GLEngine.setEGLContextFactory("+myId+")");
                    mGLSurfaceView.setEGLContextFactory(factory);
                } else {
                    pendingOperations.add(new Runnable() {
                        public void run() {
                            Log.v(LOG_TAG, "GLEngine.setEGLContextFactory("+myId+")");
                            setEGLContextFactory(factory);
                        }
                    });
                }
            }
        }

        public void setEGLWindowSurfaceFactory(final GLSurfaceView.EGLWindowSurfaceFactory factory) {
            synchronized (lock) {
                if (mGLSurfaceView != null) {
                    Log.v(LOG_TAG, "GLEngine.setEGLContextFactory("+myId+")");
                    mGLSurfaceView.setEGLWindowSurfaceFactory(factory);
                } else {
                    pendingOperations.add(new Runnable() {
                        public void run() {
                            Log.v(LOG_TAG, "GLEngine.setEGLContextFactory("+myId+")");
                            setEGLWindowSurfaceFactory(factory);
                        }
                    });
                }
            }
        }

        public void setEGLConfigChooser(final GLSurfaceView.EGLConfigChooser configChooser) {
            synchronized (lock) {
                if (mGLSurfaceView != null) {
                    Log.v(LOG_TAG, "GLEngine.setEGLConfigChooser(EGLConfigChooser "+myId+")");
                    mGLSurfaceView.setEGLConfigChooser(configChooser);
                } else {
                    pendingOperations.add(new Runnable() {
                        public void run() {
                            Log.v(LOG_TAG, "GLEngine.setEGLConfigChooser(EGLConfigChooser "+myId+")");
                            setEGLConfigChooser(configChooser);
                        }
                    });
                }
            }
        }

        public void setEGLConfigChooser(final boolean needDepth) {
            synchronized (lock) {
                if (mGLSurfaceView != null) {
                    Log.v(LOG_TAG, "GLEngine.setEGLConfigChooser(boolean "+myId+")");
                    mGLSurfaceView.setEGLConfigChooser(needDepth);
                } else {
                    pendingOperations.add(new Runnable() {
                        public void run() {
                            Log.v(LOG_TAG, "GLEngine.setEGLConfigChooser(boolean "+myId+")");
                            setEGLConfigChooser(needDepth);
                        }
                    });
                }
            }
        }

        public void setEGLConfigChooser(final int redSize, final int greenSize, final int blueSize,
            final int alphaSize, final int depthSize, final int stencilSize) {
            synchronized (lock) {
                if (mGLSurfaceView != null) {
                    Log.v(LOG_TAG, "GLEngine.setEGLConfigChooser(int,int,int,int,int,int "+myId+")");
                    mGLSurfaceView.setEGLConfigChooser(redSize, greenSize, blueSize,
                        alphaSize, depthSize, stencilSize);
                } else {
                    pendingOperations.add(new Runnable() {
                        public void run() {
                            Log.v(LOG_TAG, "GLEngine.setEGLConfigChooser(int,int,int,int,int,int "+myId+")");
                            setEGLConfigChooser(redSize, greenSize, blueSize, alphaSize, depthSize, stencilSize);
                        }
                    });
                }
            }
        }

        public void setEGLContextClientVersion(final int version) {
            synchronized (lock) {
                Method method = null;

                try {
                    //the setEGLContextClientVersion method is first available in api level 8, but we would
                    //like to support compiling against api level 7
                    method = GLSurfaceView.class.getMethod("setEGLContextClientVersion", int.class);
                } catch (NoSuchMethodException ex) {
                    return;
                }

                if (mGLSurfaceView != null) {
                    Log.v(LOG_TAG, "GLEngine.setEGLContextClientVersion("+myId+")");
                    try {
                        method.invoke(mGLSurfaceView, version);
                    } catch (IllegalAccessException ex) {
                        return;
                    } catch (InvocationTargetException ex) {
                        return;
                    }
                } else {
                    pendingOperations.add(new Runnable() {
                        public void run() {
                            Log.v(LOG_TAG, "GLEngine.setEGLContextClientVersion("+myId+")");
                            setEGLContextClientVersion(version);
                        }
                    });
                }
            }
        }

        public void setRenderMode(final int renderMode) {
            synchronized (lock) {
                if (mGLSurfaceView != null) {
                    Log.v(LOG_TAG, "GLEngine.setRenderMode(int "+myId+")");
                    mGLSurfaceView.setRenderMode(renderMode);
                } else {
                    this.renderMode = renderMode;
                    pendingOperations.add(new Runnable() {
                        public void run() {
                           Log.v(LOG_TAG, "GLEngine.setRenderMode(int "+myId+")*");
                            setRenderMode(renderMode);
                        }
                    });
                }
            }
        }

        public int getRenderMode() {
            Log.v(LOG_TAG, "GLEngine.getRenderMode("+myId+")");
            synchronized (lock) {
                if (mGLSurfaceView != null) {
                    return mGLSurfaceView.getRenderMode();
                } else {
                    return renderMode;
                }
            }
        }

        public void requestRender() {
            if (mGLSurfaceView != null) {
                Log.v(LOG_TAG, "GLEngine.requestRender("+myId+")");
                mGLSurfaceView.requestRender();
            }
        }

        @Override
        public void onVisibilityChanged(final boolean visible) {
            super.onVisibilityChanged(visible);

            synchronized (lock) {
                if (mGLSurfaceView != null) {
                	Log.v(LOG_TAG, "GLEngine.onVisibilityChanged("+visible+", "+myId+")");
                    if (visible) {
                        Log.v(LOG_TAG, "SET("+myId+"): mSurfaceHolder, "+myId);
                        GLWallpaperService.this.mSurfaceHolder = getSurfaceHolder(); //MAINT: I neeed to know why this works.
                        if (mFocusCount == 0){
                            mGLSurfaceView.onResume();
                            resumeCallBack();
                        }
                        mFocusCount++;
                    } else {
                        mFocusCount--;
                        if (mFocusCount == 0){
                            mGLSurfaceView.onPause();
                            pauseCallBack();
                        }
                    }
                } else {
                    pendingOperations.add(new Runnable() {
                        public void run() {
                        	Log.v(LOG_TAG, "GLEngine.onVisibilityChanged("+visible+","+myId+")");
                            if (visible) {
                                mGLSurfaceView.onResume();
                                resumeCallBack();
                            } else {
                                mGLSurfaceView.onPause();
                                pauseCallBack();
                            }
                        }
                    });
                }
            }
        }
        public void resumeCallBack() {}
        public void pauseCallBack() {}

        @Override
        public void onSurfaceChanged(final SurfaceHolder holder, final int format, final int width, final int height) {
            synchronized (lock) {
                Log.v(LOG_TAG, "SET("+myId+"): mSurfaceHolder, "+myId);
                GLWallpaperService.this.mSurfaceHolder = holder; //MAINT: I neeed to know why this works.
                if (mGLSurfaceView != null) {
                	Log.v(LOG_TAG, "GLEngine.onSurfaceChanged("+myId+")");
                    mGLSurfaceView.surfaceChanged(holder, format, width, height);
                } else {
                    pendingOperations.add(new Runnable() {
                        public void run() {
                        	Log.v(LOG_TAG, "GLEngine.onSurfaceChanged("+myId+")");
                            onSurfaceChanged(holder, format, width, height);
                        }
                    });
                }
            }
        }

        @Override
        public void onSurfaceCreated(SurfaceHolder holder) {
            Log.v(LOG_TAG, "GLEngine.onSurfaceCreated("+myId+")");
            synchronized (lock) {
                Log.v(LOG_TAG, "SET("+myId+"): mSurfaceHolder, "+myId);
                GLWallpaperService.this.mSurfaceHolder = holder; //MAINT: I neeed to know why this works.
                mIsInitialized = true;  // Do we need this? Just check if mSurfaceHolder != null
                if (mGLSurfaceView == null) {
                    surfaceCreatedCallBack(); // Init Godot first to set use_32 boolean
                    mGLSurfaceView = getGLSurfaceView();
                    for (Runnable pendingOperation: pendingOperations) {
                        pendingOperation.run();
                    }
                    pendingOperations.clear();
                }
                mGLSurfaceView.surfaceCreated(holder);
            }
        }
        public void surfaceCreatedCallBack() {}

        @Override
        public void onSurfaceDestroyed(SurfaceHolder holder) {
            Log.v(LOG_TAG, "GLEngine.onSurfaceDestroyed("+myId+")");
            /*
             * Shouldnt we be able to destroy the surface? We dont need this one 
             * anymore...
             * synchronized (lock) {
             *   if (mGLSurfaceView != null) {
             *       mGLSurfaceView.surfaceDestroyed(holder);
             *   }
            }*/
        }
    }
}
