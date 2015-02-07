#pragma once

#include "ofMain.h"
#include "IGrabber.h"
#include "ofxNetwork.h"
#include "ThreadData.h"
#include "GlobalData.h"
#include "FrameUtils.h"

class Transmitter : public ofThread {

	public:
		void threadedFunction();
		void process(ofEventArgs &e);
		bool checkConnError();
		void exit();
        int camWidth;
        int camHeight;
        IGrabber * grabber;
        f_compress_img      compress_img;
        ofxTCPClient TCP;
        ofxTCPServer TCPSVR;

		int state; // 0 - todav�a no se conect� al servidor, 1 - todav�a no tiene cliente asignado, 2 - tiene cliente asignado.
		string cliId;
		int serverPort;
		int go;
        t_data * sys_data;
		void sendFrame(int totalCams, ThreadData * tData);
        bool connectionClosed;
        bool started;
        bool idle;

        Transmitter() {
            idle                = true;
            started             = false;
            connectionClosed    = false;
        }

        ~Transmitter() {
            if(!started) return;
            //ofRemoveListener(ofEvents().update, this, &Transmitter::process);
        }
};
