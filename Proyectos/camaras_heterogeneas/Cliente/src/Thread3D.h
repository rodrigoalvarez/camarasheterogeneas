#pragma once

#include "ofxOpenNI.h"
#include "GlobalData.h"
#include "ofMain.h"

class Thread3D : public ofThread {

	public:
		void	threadedFunction();
		void    updateData();
		bool	isDeviceInitted();
        bool	isDataAllocated();

		ofxOpenNI openNIRecorder;
        bool    dataAllocated;
        ofImage         img;
        ofShortPixels  spix;
        t_camera  * context;
};
