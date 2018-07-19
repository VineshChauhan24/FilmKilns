/*
 * Copyright (c) 2018-present, lmyooyo@gmail.com.
 *
 * This source code is licensed under the GPL license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.lmy.codec

import android.graphics.SurfaceTexture
import android.os.Environment
import com.lmy.codec.encoder.Encoder
import com.lmy.codec.encoder.impl.AudioEncoderImpl
import com.lmy.codec.entity.CodecContext
import com.lmy.codec.helper.CodecFactory
import com.lmy.codec.muxer.Muxer
import com.lmy.codec.muxer.impl.MuxerImpl
import com.lmy.codec.pipeline.SingleEventPipeline
import com.lmy.codec.render.Render
import com.lmy.codec.render.impl.DefaultRenderImpl
import com.lmy.codec.texture.impl.filter.BaseFilter
import com.lmy.codec.wrapper.CameraWrapper

/**
 * Created by lmyooyo@gmail.com on 2018/3/21.
 */
class CameraPreviewPresenter(var context: CodecContext,
                             var encoder: Encoder? = null,
                             var audioEncoder: Encoder? = null,
                             private var cameraWrapper: CameraWrapper? = null,
                             private var render: Render? = null,
                             private var muxer: Muxer? = null)
    : SurfaceTexture.OnFrameAvailableListener {

    private var onStateListener: OnStateListener? = null

    init {
        SingleEventPipeline.instance.start()
        cameraWrapper = CameraWrapper.open(context, this)
                .post(Runnable {
                    render = DefaultRenderImpl(context, cameraWrapper!!.textureWrapper)
                })
    }

    fun setFilter(filter: Class<*>) {
        render?.setFilter(filter)
    }

    fun getFilter(): BaseFilter? {
        return render?.getFilter()
    }

    /**
     * Camera有数据生成时回调
     * For CameraWrapper
     */
    override fun onFrameAvailable(cameraTexture: SurfaceTexture?) {
        render?.onFrameAvailable()
        render?.post(Runnable {
            encoder?.onFrameAvailable(cameraTexture)
        })
    }

    fun startPreview(screenTexture: SurfaceTexture, width: Int, height: Int) {
        cameraWrapper?.post(Runnable {
            cameraWrapper!!.startPreview()
            render?.start(screenTexture, width, height)
            render?.post(Runnable {
                start()
            })
        })
    }

    fun updatePreview(width: Int, height: Int) {
//        mRender?.updatePreview(width, height)
    }

    private fun start() {
        muxer = MuxerImpl("${Environment.getExternalStorageDirectory().absolutePath}/test.mp4")
        encoder = CodecFactory.getEncoder(context, render!!.getFrameBufferTexture(),
                cameraWrapper!!.textureWrapper.egl!!.eglContext!!)
        audioEncoder = AudioEncoderImpl(context)
        if (null != muxer) {
            encoder!!.setOnSampleListener(muxer!!)
            audioEncoder!!.setOnSampleListener(muxer!!)
        }
    }

    fun updateSize(width: Int, height: Int) {
        render?.updateSize(width, height)
        SingleEventPipeline.instance.queueEvent(Runnable {
            stop()
            start()
        })
    }

    fun stopPreview() {
        release()
        SingleEventPipeline.instance.quit()
    }

    private fun release() {
        SingleEventPipeline.instance.queueEvent(Runnable {
            stop()
        })
        try {
            cameraWrapper?.release()
            cameraWrapper = null
            render?.release()
            render = null
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }

    private fun stop() {
        encoder?.stop()
        audioEncoder?.stop()
        muxer?.release()
        onStateListener?.onStop()
    }

    fun setOnStateListener(listener: OnStateListener) {
        onStateListener = listener
    }

    interface OnStateListener {
        fun onStop()
    }
}