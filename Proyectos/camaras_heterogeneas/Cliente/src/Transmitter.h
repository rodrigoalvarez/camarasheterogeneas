#pragma once

#include "ofMain.h"
#include "IGrabber.h"
#include "ofxNetwork.h"
#include "ThreadData.h"
#include "FrameUtils.h"

class Transmitter : public ofThread {

	public:
		void threadedFunction();
        int camWidth;
        int camHeight;
        IGrabber * grabber;

        ofxTCPClient TCP;
        ofxTCPServer TCPSVR;

		int state; // 0 - todavía no se conectó al servidor, 1 - todavía no tiene cliente asignado, 2 - tiene cliente asignado.
		string cliId;
		int serverPort;
		int go;

		void sendFrame(int totalCams, ThreadData * tData);

};
