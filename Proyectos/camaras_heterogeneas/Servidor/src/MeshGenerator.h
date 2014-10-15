#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
#include "Constants.h"
#include "ThreadData.h"
#include "FrameUtils.h"
#include "MainBuffer/MainBuffer.h"
#include "ServerGlobalData.h"

class MeshGenerator : public ofThread {
	public:
        MeshGenerator() {
            buffer = NULL;
        }

		void threadedFunction();
		void processFrame();
		MainBuffer * buffer;
		t_data * sys_data;
};
