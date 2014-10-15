#pragma once
#include "ofxXmlSettings.h"
#include "ofMain.h"
#include "ofMatrix4x4.h"

struct t_data {
    string   serverIp;
    int      serverPort;
    int      fps;
    int      persistToPly;
    int      logLevel;
    int      maxPackageSize;
    int      syncFactorValue;
    int      maxThreadedServers;
    int      maxReceiveFrameBuffer;
    int      processMostRecent;
};

class ServerGlobalData {
	public:
        void    loadCalibData(char * xml);
        t_data      * sys_data;
};
