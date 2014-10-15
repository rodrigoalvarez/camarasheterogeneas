#include "Thread2D.h"

void Thread2D::threadedFunction() {
    ofSetFrameRate(1);
    isAllocated = false;

    bool first = true;
    float mTimestamp;
    int snapCounter = 1;

    vidGrabber.setVerbose(true);
    vidGrabber.setDeviceID(context->id);
    vidGrabber.initGrabber(context->resolutionX, context->resolutionY);
    //return;
    string path = "cameras/2D/" + ofToString(context->id);


    if(sys_data->goLive == 1) {
        img.allocate(context->resolutionX, context->resolutionX, OF_IMAGE_COLOR);
    }

    if(sys_data->persistence == 1) {
        ofDirectory::createDirectory(path, true, true);
        //nuevo QT
        video.setCodecQualityLevel( OF_QT_SAVER_CODEC_QUALITY_NORMAL );
        video.setup( context->resolutionX, context->resolutionY, "cameras/2D/" + ofToString(context->id) + "/rec.mov" );
        //fin:nuevo QT
    }

    //return;
    while(isThreadRunning()) {
        ofSleepMillis(1000/sys_data->fps);
        vidGrabber.grabFrame();
        if (vidGrabber.isFrameNew()) {
            lock();
            isAllocated = false;

            if(sys_data->goLive == 1) {
                img.setFromPixels(vidGrabber.getPixels(), context->resolutionX, context->resolutionY, OF_IMAGE_COLOR, true);
            }

            if(sys_data->persistence == 1) {
                //nuevo QT
                float time  = ofGetElapsedTimef() - mTimestamp;
                video.addFrame(vidGrabber.getPixels(), first ? 0 : time);
                mTimestamp  = ofGetElapsedTimef();
                first       = false;
                //fin:nuevo QT
            }

            isAllocated = true;
            unlock();
            snapCounter++;
        }
        //sleep(40);
    }

    if(sys_data->goLive == 1) {
        img.clear();
    }

    if(sys_data->persistence == 1) {
        video.finishMovie();
    }
}

bool Thread2D::isDeviceInitted() {
    return vidGrabber.isInitialized();
}

bool Thread2D::isDataAllocated() {
    return isDeviceInitted() && isAllocated;
}
