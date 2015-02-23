#include "ThreadONI.h"

void ThreadONI::threadedFunction() {

    openNIRecorder = new ofxOpenNI(context->id);
    openNIRecorder->setup();
	openNIRecorder->start();

    openNIRecorder->startPlayer(context->file);
    openNIRecorder->setPaused(false);
    openNIRecorder->setLooped(true);
    dataAllocated = false;

	if((context->use2D == 1) && (sys_data->goLive == 1)) {
        img.allocate(context->resolutionX, context->resolutionX, OF_IMAGE_COLOR);
	}

    deviceInited    = true;

    started = true;
    ofAddListener(ofEvents().update, this, &ThreadONI::process);

}

void ThreadONI::process(ofEventArgs &e) {
    if(!started) return;
    if(!idle) {
        ofLogVerbose() << "[ThreadONI::process] :: NO IDLE / FPS " << ofToString(ofGetFrameRate()) << endl;
        return;
    }
    idle = false;

    ofLogVerbose() << "[ThreadONI::process] :: IDLE / FPS " << ofToString(ofGetFrameRate()) << endl;

    openNIRecorder->update();

	updateData();

    idle = true;
}

void ThreadONI::updateData() {

    dataAllocated = false;

    if(deviceInited) {
        lock();
        if(context->use2D) {
            ofPixels&    ipixels    = openNIRecorder->getImagePixels();
            if((context->use2D == 1) && (sys_data->goLive == 1)) {
                img.setFromPixels(ipixels.getPixels(), context->resolutionX, context->resolutionY, OF_IMAGE_COLOR, true);
                if(context->resolutionDownSample < 1) {
                    img.resize(img.width * context->resolutionDownSample, img.height * context->resolutionDownSample);
                }
                //img.saveImage("player3.jpg");
            }
        }
        if(context->use3D) {
            spix    = openNIRecorder->getDepthRawPixels();
        }
        dataAllocated = true;
        unlock();
    }
}

bool ThreadONI::isDeviceInitted() {
    return deviceInited;
}

bool ThreadONI::isDataAllocated() {
    return dataAllocated;
}

void ThreadONI::exit() {
    ofLogVerbose() << "[ThreadONI::exit]";
    stopThread();
}
