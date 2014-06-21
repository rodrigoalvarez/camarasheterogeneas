#pragma once

#include "ofMain.h"
#include "IGrabber.h"

class Transmitter : public ofThread {

	public:
		void threadedFunction();
        int camWidth;
        int camHeight;
        IGrabber * grabber;
};
