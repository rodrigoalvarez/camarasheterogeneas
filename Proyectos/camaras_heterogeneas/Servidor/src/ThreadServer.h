#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
#include "Constants.h"
#include "ThreadData.h"
#include "FrameUtils.h"
#include "ServerGlobalData.h"
#include "FrameBuffer.h"

class ThreadServer : public ofThread {

	public:
        f_decompress_img    decompress_img;

        ThreadServer() {
            idle                = true;
            started             = false;
            connectionClosed    = false;
        }

        ~ThreadServer() {
            if(!started) return;
            //ofRemoveListener(ofEvents().update, this, &ThreadServer::receiveFrame);
        }

		void threadedFunction();
		int port;
		int cliId;
		string ip;
		ofxTCPClient TCPCLI;
		void startFoo();
		void update();
		void receiveFrame();
		void exit();
		//int totCameras;
		ThreadData * tData;
		char * getFrame();
		char * currBytearray;
		ofstream myfile;
		FrameBuffer fb;
		t_data * sys_data;
		bool idle;
		bool started;
		bool connectionClosed;
		bool checkConnError();
};
