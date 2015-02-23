#pragma once

#include "ofMain.h"
#include "ThreadData.h"

class IGrabber : public ofBaseApp {

	public:
		virtual void updateThreadData() = 0;
		ThreadData * tData;
		int total2D;
        int total3D;
        int totalONI;
};
