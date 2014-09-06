#include "Thread2D.h"

void Thread2D::threadedFunction() {
    ofSetFrameRate(1);
    isAllocated = false;

    /*ofFile file( "camara2d.txt", ofFile::WriteOnly );

    file << "context-id: " << context->id << " \n";
    file << "context-resolutionX: " << context->resolutionX << " \n";
    file << "context-resolutionY: " << context->resolutionY << " \n";
    file << "context-colorRGB: " << context->colorRGB << " \n";

    file.close();*/

    bool first = true;
    float mTimestamp;
    int snapCounter = 1;
    vidGrabber.setVerbose(true);
    vidGrabber.setDeviceID(context->id);
    vidGrabber.initGrabber(context->resolutionX, context->resolutionY);
    //return;
    string path = "cameras/2D/" + ofToString(context->id);
    ofDirectory::createDirectory(path, true, true);

    if(context->colorRGB == 1) {
        img.allocate(context->resolutionX, context->resolutionX, OF_IMAGE_COLOR);
    } else {
        //@TODO: El grayscale no está funcionando. Queda todo negro.
        img.allocate(context->resolutionX, context->resolutionY, OF_IMAGE_GRAYSCALE);
    }

    //nuevo QT
    video.setCodecQualityLevel( OF_QT_SAVER_CODEC_QUALITY_NORMAL );
    video.setup( context->resolutionX, context->resolutionY, "cameras/2D/" + ofToString(context->id) + "/rec.mov" );
    //fin:nuevo QT

    //return;
    while(isThreadRunning()) {
        vidGrabber.grabFrame();
        if (vidGrabber.isFrameNew()) {
            lock();
            isAllocated = false;

            if(context->colorRGB == 1) {
                img.setFromPixels(vidGrabber.getPixels(), context->resolutionX, context->resolutionY, OF_IMAGE_COLOR, true);
            } else {
                //@TODO: El grayscale no está funcionando. Queda todo negro.
                img.setFromPixels(vidGrabber.getPixels(), context->resolutionX, context->resolutionY, OF_IMAGE_GRAYSCALE, true);
            }
            //img.saveImage("cameras/2D/" + ofToString(context->id) + "/img-"+ofToString(snapCounter)+".png");

            //nuevo QT
            float time  = ofGetElapsedTimef() - mTimestamp;
            video.addFrame(vidGrabber.getPixels(), first ? 0 : time);
            mTimestamp  = ofGetElapsedTimef();
            first       = false;
            //fin:nuevo QT

            isAllocated = true;
            unlock();
            snapCounter++;
        }
        //sleep(40);
    }
    img.clear();
    video.finishMovie();
}

bool Thread2D::isDeviceInitted() {
    return vidGrabber.isInitialized();
}

bool Thread2D::isDataAllocated() {
    return isDeviceInitted() && isAllocated;
}
