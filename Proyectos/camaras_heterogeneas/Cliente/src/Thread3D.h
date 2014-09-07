#pragma once

#include "ofxOpenNI.h"
#include "GlobalData.h"
#include "Constants.h"
#include "ofMain.h"

class Thread3D : public ofThread {

	public:
        Thread3D();
        ~Thread3D();

		void	threadedFunction();
		void    updateData();
		bool	isDeviceInitted();
        bool	isDataAllocated();

		ofxOpenNI * openNIRecorder;
        bool    dataAllocated;
        ofImage         img;
        ofFloatPixels  fXpix;
        ofFloatPixels  fYpix;
        ofFloatPixels  fZpix;
        ofShortPixels  spix;
        t_camera  * context;
        t_data * sys_data;
};
