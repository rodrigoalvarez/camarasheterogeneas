#include "ServerGlobalData.h"

//--------------------------------------------------------------

void ServerGlobalData::loadCalibData(char * xml) {
    ofxXmlSettings settings;
    settings.loadFile(xml);
    if(settings.pushTag("settings")) {
	    sys_data                        = new t_data();
	    sys_data->serverPort            = settings.getValue("serverPort",               11969);
		sys_data->fps                   = settings.getValue("fps",                      10);
		sys_data->maxPackageSize        = settings.getValue("maxPackageSize",           60000);
		sys_data->syncFactorValue       = settings.getValue("syncFactorValue",          10);
		sys_data->maxThreadedServers    = settings.getValue("maxThreadedServers",       100);
		sys_data->maxReceiveFrameBuffer = settings.getValue("maxReceiveFrameBuffer",    10);
	}
}
