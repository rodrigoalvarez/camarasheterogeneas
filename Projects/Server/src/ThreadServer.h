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
            closed              = false;
            b_exit              = false;
        }

        ~ThreadServer() {
            if(!started) return;
            //ofRemoveListener(ofEvents().update, this, &ThreadServer::receiveFrame);
        }

		void threadedFunction();
		bool closed;
		int port;
		int cliId;
		string ip;
		ofxTCPClient TCPCLI;
		void startFoo();
		void update();
		void receiveFrame(ofEventArgs &e);
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
		bool b_exit;
		bool connectionClosed;
		bool checkConnError();
};
