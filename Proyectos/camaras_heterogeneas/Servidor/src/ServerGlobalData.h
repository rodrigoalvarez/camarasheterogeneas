#pragma once
#include "ofxXmlSettings.h"
#include "ofMain.h"
#include "ofMatrix4x4.h"

struct t_data {
    string   serverIp;
    int      serverPort;
    int      fps;
    int      maxPackageSize;
};

class ServerGlobalData {
	public:
        void    loadCalibData(char * xml);
        t_data      * sys_data;
};
