#include "Thread3D.h"

Thread3D::Thread3D() {
    openNIRecorder = NULL;
}

Thread3D::~Thread3D() {
    if(openNIRecorder != NULL) {
        delete openNIRecorder;
    }
}

void Thread3D::threadedFunction() {
    dataAllocated = false;

	char buff[30];
	openNIRecorder = new ofxOpenNI(context->id);
	openNIRecorder->setup();

	cout << "openNIRecorder->getNumDevices() " << openNIRecorder->getNumDevices() << endl;

	if(context->use2D == 1) {
	    openNIRecorder->addImageGenerator();
	}

	if(context->use3D == 1) {
	    openNIRecorder->addDepthGenerator();
	}
	ofLogVerbose() << "[Thread3D::threadedFunction] - context->use3D: " << context->use3D;

    openNIRecorder->setRegister(true);
    openNIRecorder->setMirror(true);
    openNIRecorder->setUseDepthRawPixels(true);
    openNIRecorder->start();

    if(context->colorRGB == 1) {
        img.allocate(context->resolutionX, context->resolutionX, OF_IMAGE_COLOR);
    } else {
        //@TODO: El grayscale no está funcionando. Queda todo negro.
        img.allocate(context->resolutionX, context->resolutionY, OF_IMAGE_GRAYSCALE);
    }

	string path = "cameras/3D/" + ofToString(context->id);
	ofDirectory::createDirectory(path, true, true);

	sprintf( buff, "%u", ofGetSystemTimeMicros() );

    openNIRecorder->startRecording( ofToDataPath( path  + "/oni_" + ofToString(ofGetUnixTime()) + "_" + buff + ".oni") );

	while(isThreadRunning()) {
	    ofSleepMillis(1000/FPS);
	    openNIRecorder->update();
	    updateData();
	}

	openNIRecorder->stop();
    // done
    img.clear();
}

void Thread3D::updateData() {
    dataAllocated = false;

    if(openNIRecorder->isRecording()) {
        if(context->use2D) {
            ofPixels&    ipixels    = openNIRecorder->getImagePixels();
            if(context->colorRGB == 1) {
                img.setFromPixels(ipixels.getPixels(), context->resolutionX, context->resolutionY, OF_IMAGE_COLOR, true);
            } else {
                //@TODO: El grayscale no está funcionando. Queda todo negro.
                img.setFromPixels(ipixels.getPixels(), context->resolutionX, context->resolutionY, OF_IMAGE_GRAYSCALE, true);
            }
            //img.saveImage("desde_kinect.jpg");
            if(context->resolutionDownSample < 1) {
                img.resize(img.width * context->resolutionDownSample, img.height * context->resolutionDownSample);
            }
            //img.saveImage("desde_kinect_res.jpg");
        }
        if(context->use3D) {
            //openNIRecorder->
            spix    = openNIRecorder->getDepthRawPixels();
            //Falta aplicarle la transformación según lo que venga en la matriz.
            //Falta aplicarle el downsample.
        }
        dataAllocated = true;
    }
}

bool Thread3D::isDeviceInitted() {
    return openNIRecorder->isRecording();
}

bool Thread3D::isDataAllocated() {
    return dataAllocated;
}
