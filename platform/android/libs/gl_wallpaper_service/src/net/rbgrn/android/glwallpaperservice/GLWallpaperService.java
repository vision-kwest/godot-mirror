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

import android.graphics.Rect;
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
    public final static int RENDERMODE_WHEN_DIRTY = 0;
    public final static int RENDERMODE_CONTINUOUSLY = 1;
    private Object lock = new Object();

    protected GLSurfaceView mGLSurfaceView = null;
    protected SurfaceHolder mSurfaceHolder = null;

    private static int mEngineCount = 0;
    protected int mCurrentId = 0;
    protected int mFormat;

    private int debugFlags;
    private int renderMode;

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
    protected List<Runnable> pendingOperations = new ArrayList<Runnable>();

    public void setGLWrapper(final GLSurfaceView.GLWrapper glWrapper) {
        Log.v(LOG_TAG, "GLEngine.setGLWrapper()");
        synchronized (lock) {
            if (mGLSurfaceView != null) {
                mGLSurfaceView.setGLWrapper(glWrapper);
            } else {
                pendingOperations.add(new Runnable() {
                    public void run() {
                        setGLWrapper(glWrapper);
                    }
                });
            }
        }
    }

    public void setDebugFlags(final int debugFlags) {
        Log.v(LOG_TAG, "GLEngine.setDebugFlags()");
        synchronized (lock) {
            if (mGLSurfaceView != null) {
                mGLSurfaceView.setDebugFlags(debugFlags);
            } else {
                this.debugFlags = debugFlags;
                pendingOperations.add(new Runnable() {
                    public void run() {
                        setDebugFlags(debugFlags);
                    }
                });
            }
        }
    }

    public int getDebugFlags() {
        Log.v(LOG_TAG, "GLEngine.getDebugFlags()");
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
                Log.v(LOG_TAG, "GLEngine.setRenderer()");
                mGLSurfaceView.setRenderer(renderer);
                boolean isVisible = (mCurrentId != 0);
                if (!isVisible) {
                	mGLSurfaceView.onPause();
                }
            } else {
                Log.v(LOG_TAG, "PENDING: GLEngine.setRenderer()");
                pendingOperations.add(new Runnable() {
                    public void run() {
                        setRenderer(renderer);
                    }
                });
            }
        }
    }

    public void queueEvent(final Runnable r) {
        Log.v(LOG_TAG, "GLEngine.queueEvent()");
        synchronized (lock) {
            if (mGLSurfaceView != null) {
                mGLSurfaceView.queueEvent(r);
            } else {
                pendingOperations.add(new Runnable() {
                    public void run() {
                        queueEvent(r);
                    }
                });
            }
        }
    }

    public void setEGLContextFactory(final GLSurfaceView.EGLContextFactory factory) {
        Log.v(LOG_TAG, "GLEngine.setEGLContextFactory()");
        synchronized (lock) {
            if (mGLSurfaceView != null) {
                mGLSurfaceView.setEGLContextFactory(factory);
            } else {
                pendingOperations.add(new Runnable() {
                    public void run() {
                        setEGLContextFactory(factory);
                    }
                });
            }
        }
    }

    public void setEGLWindowSurfaceFactory(final GLSurfaceView.EGLWindowSurfaceFactory factory) {
        Log.v(LOG_TAG, "GLEngine.setEGLContextFactory()");
        synchronized (lock) {
            if (mGLSurfaceView != null) {
                mGLSurfaceView.setEGLWindowSurfaceFactory(factory);
            } else {
                pendingOperations.add(new Runnable() {
                    public void run() {
                        setEGLWindowSurfaceFactory(factory);
                    }
                });
            }
        }
    }

    public void setEGLConfigChooser(final GLSurfaceView.EGLConfigChooser configChooser) {
        Log.v(LOG_TAG, "GLEngine.setEGLConfigChooser(EGLConfigChooser)");
        synchronized (lock) {
            if (mGLSurfaceView != null) {
                mGLSurfaceView.setEGLConfigChooser(configChooser);
            } else {
                pendingOperations.add(new Runnable() {
                    public void run() {
                        setEGLConfigChooser(configChooser);
                    }
                });
            }
        }
    }

    public void setEGLConfigChooser(final boolean needDepth) {
         Log.v(LOG_TAG, "GLEngine.setEGLConfigChooser(boolean)");
        synchronized (lock) {
            if (mGLSurfaceView != null) {
                mGLSurfaceView.setEGLConfigChooser(needDepth);
            } else {
                pendingOperations.add(new Runnable() {
                    public void run() {
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
                Log.v(LOG_TAG, "GLEngine.setEGLConfigChooser(int,int,int,int,int,int)");
                mGLSurfaceView.setEGLConfigChooser(redSize, greenSize, blueSize,
                        alphaSize, depthSize, stencilSize);
            } else {
                Log.v(LOG_TAG, "PENDING: GLEngine.setEGLConfigChooser(int,int,int,int,int,int)");
                pendingOperations.add(new Runnable() {
                    public void run() {
                        setEGLConfigChooser(redSize, greenSize, blueSize, alphaSize, depthSize, stencilSize);
                    }
                });
            }
        }
    }

    public void setEGLContextClientVersion(final int version) {
        Log.v(LOG_TAG, "GLEngine.setEGLContextClientVersion()");
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
                        setEGLContextClientVersion(version);
                    }
                });
            }
        }
    }

    public void setRenderMode(final int renderMode) {
        synchronized (lock) {
            if (mGLSurfaceView != null) {
                Log.v(LOG_TAG, "GLEngine.setRenderMode(int)");
                mGLSurfaceView.setRenderMode(renderMode);
            } else {
                Log.v(LOG_TAG, "PENDING: GLEngine.setRenderMode(int)");
                this.renderMode = renderMode;
                pendingOperations.add(new Runnable() {
                    public void run() {
                        setRenderMode(renderMode);
                    }
                });
            }
        }
    }

    public int getRenderMode() {
        Log.v(LOG_TAG, "GLEngine.getRenderMode()");
        synchronized (lock) {
            if (mGLSurfaceView != null) {
                return mGLSurfaceView.getRenderMode();
            } else {
                return renderMode;
            }
        }
    }

    public void requestRender() {
        Log.v(LOG_TAG, "GLEngine.requestRender()");
        if (mGLSurfaceView != null) {
            mGLSurfaceView.requestRender();
        }
    }

    @Override
    public void onDestroy() {
        Log.v(LOG_TAG, "GLWallpaperService.onDestroy()");
        super.onDestroy();
    }

    public void resumeCallBack() {}
    public void pauseCallBack() {}
    public void surfaceCreatedCallBack() {}


    public interface Renderer extends GLSurfaceView.Renderer {
    }

    public class GLEngine extends WallpaperService.Engine {

        protected int mId;

        public GLEngine() {
            Log.v(LOG_TAG, "GLEngine.GLEngine()");

            mEngineCount += 1;
            mId = mEngineCount;
        }

        public GLSurfaceView getGLSurfaceView() {
            Log.v(LOG_TAG, "GLEngine.getGLSurfaceView()");
           // Sub-classes that need a special version of GLSurfaceView can override this method.
            return new GLSurfaceView(GLWallpaperService.this){
                @Override
                public SurfaceHolder getHolder() {
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

        @Override
        public void onVisibilityChanged(final boolean visible) {
            super.onVisibilityChanged(visible);
            final SurfaceHolder holder = getSurfaceHolder();
            final boolean aquireLock = true;
            if (isAnotherEngineRendering()){
                Log.v(LOG_TAG, mId+": PENDING: GLEngine.onVisibilityChanged("+visible+")");
                pendingOperations.add(new Runnable() {
                    public void run() {
                        if (visible) {
                            renderSetup(holder, aquireLock);
                        } else {
                            renderTearDown(holder, 0);
                        }
                    }
                });
                return;
            }

            Log.v(LOG_TAG, mId+": GLEngine.onVisibilityChanged("+visible+")");
            if (visible) {
                renderSetup(holder, aquireLock);
            } else {
                renderTearDown(holder, 0);
                runPendingOperations();
            }
        }

        @Override
        public void onSurfaceChanged(SurfaceHolder holder, int format, int width, int height) {
            Log.v(LOG_TAG, "GLEngine.onSurfaceChanged("+format+", "+width+", "+height+")");
            // No need to store the width and height. We can get that info from
            // the SurfaceHolder.getSurfaceFrame() method.
            mFormat = format;
        }

        @Override
        public void onSurfaceCreated(SurfaceHolder holder) {
            Log.v(LOG_TAG, "GLEngine.onSurfaceCreated()");
            synchronized (lock) {
                if (mGLSurfaceView == null) {
                    mSurfaceHolder = holder;
                    surfaceCreatedCallBack(); // Init Godot first to set use_32 boolean
                    mGLSurfaceView = getGLSurfaceView();
                    runPendingOperations();
                }
            }
        }

        @Override
        public void onSurfaceDestroyed(SurfaceHolder holder) {
            Log.v(LOG_TAG, "GLEngine.onSurfaceDestroyed()");
            if (isAnotherEngineRendering()){
                Log.v(LOG_TAG, "REBOOT!!!");
                renderTearDown(mSurfaceHolder, mCurrentId);
            }
        }

        @Override
        public void onDestroy() {
            Log.v(LOG_TAG, "GLEngine.onDestroy()");
            if (isAnotherEngineRendering()){
                Log.v(LOG_TAG, "REBOOT!!!");
                boolean aquireLock = false;
                renderSetup(mSurfaceHolder, aquireLock);
            }
            super.onDestroy();
        }

        public boolean isAnotherEngineRendering(){
            boolean isCurentEngineNotMe = mCurrentId != mId;
            boolean isSurfaceInUse = mCurrentId != 0;
            return (isSurfaceInUse && isCurentEngineNotMe);
        }

        public void renderSetup(SurfaceHolder holder, boolean aquireLock){
            Log.v(LOG_TAG, "GLEngine.renderSetup()");
            if (aquireLock) {
                mCurrentId = mId;
                mSurfaceHolder = holder;
            }
            mGLSurfaceView.surfaceCreated(holder);
            Rect rect = holder.getSurfaceFrame();
            mGLSurfaceView.surfaceChanged(holder, mFormat, rect.width(), rect.height());
            mGLSurfaceView.onResume();
            resumeCallBack();
        }

        public void renderTearDown( SurfaceHolder holder, int id){
            Log.v(LOG_TAG, "GLEngine.renderTearDown()");
            mGLSurfaceView.onPause();
            pauseCallBack();
            mGLSurfaceView.surfaceDestroyed(holder);
            mCurrentId = id;
        }

        public void runPendingOperations(){
            Log.v(LOG_TAG, "GLEngine.runPendingOperations()");
            for (Runnable pendingOperation: pendingOperations) {
                pendingOperation.run();
            }
            Log.v(LOG_TAG, "Done.");
            pendingOperations.clear();
        }
    }
}
