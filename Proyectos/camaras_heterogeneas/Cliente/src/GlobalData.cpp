#include "GlobalData.h"

//--------------------------------------------------------------
int GlobalData::getFPS() {

}

bool GlobalData::getGoLive() {

}

int GlobalData::getTotal3D() {

}

int GlobalData::getTotal2D() {

}

int GlobalData::getTotalDevices() {

}

void GlobalData::loadCalibData(char * xml) {
    ofxXmlSettings settings;
    settings.loadFile(xml);
    t_camera * currCam = NULL;

    total2D = 0;
    total3D = 0;

	if(settings.pushTag("settings")) {

	    sys_data                  = new t_data();
	    sys_data->cliId           = settings.getValue("cliId",          1);
        sys_data->cliPort         = settings.getValue("cliPort",        15000);
		sys_data->serverIp        = settings.getValue("serverIp",       "127.0.0.1");
		sys_data->serverPort      = settings.getValue("serverPort",     11969);
		sys_data->goLive          = settings.getValue("realTime",       0);
		sys_data->persistence     = settings.getValue("persistence",    1);
		sys_data->logLevel        = settings.getValue("logLevel",                 0);
		sys_data->fps             = settings.getValue("fps",            10);
		sys_data->maxPackageSize  = settings.getValue("maxPackageSize", 60000);

        if(settings.pushTag("cameras")) {

		    int totCams = settings.getNumTags("camera");
		    for (int i = 0; i < totCams; i++) {
		        settings.pushTag("camera", i);

                if(camera == NULL) {
		            camera          = new t_camera();
		            currCam         = camera;
		        } else {
                    currCam->sig    = new t_camera();
                    currCam         = currCam->sig;
		        }

                currCam->sig                    = NULL;
                currCam->id                     = settings.getValue("id", 0);
				currCam->resolutionX            = settings.getValue("resolutionX", 640);
				currCam->resolutionY            = settings.getValue("resolutionY", 480);
				currCam->resolutionDownSample   = settings.getValue("resolutionDownSample", 1);
				currCam->fps                    = settings.getValue("FPS", 24);
				currCam->use2D                  = settings.getValue("use2D", true);
				currCam->use3D                  = settings.getValue("use3D", true);

                if(settings.pushTag("dataContext")) {
                    /*if(settings.pushTag("depthSettings")) {
                        currCam->near3D                 = settings.getValue("near", 0);
                        currCam->far3D                  = settings.getValue("far", 1);
                        currCam->points3DDownSample     = settings.getValue("pointsDownSample", 0.5);
                        settings.popTag();
                    }*/
                    if(settings.pushTag("matrix")) {
                        currCam->row1.set(settings.getValue("m000", 1.0f), settings.getValue("m001", 1.0f), settings.getValue("m002", 1.0f), settings.getValue("m003", 1.0f));
                        currCam->row2.set(settings.getValue("m100", 1.0f), settings.getValue("m101", 1.0f), settings.getValue("m102", 1.0f), settings.getValue("m103", 1.0f));
                        currCam->row3.set(settings.getValue("m200", 1.0f), settings.getValue("m201", 1.0f), settings.getValue("m202", 1.0f), settings.getValue("m203", 1.0f));
                        currCam->row4.set(settings.getValue("m300", 1.0f), settings.getValue("m301", 1.0f), settings.getValue("m302", 1.0f), settings.getValue("m303", 1.0f));

                        cout << "[GlobalData::loadCalibData] - row1.x: " << currCam->row1.x << ", row1.y: " << currCam->row1.y << ", row1.z: " << currCam->row1.z << ", row1.w: " << currCam->row1.w << endl;
                        cout << "[GlobalData::loadCalibData] - row2.x: " << currCam->row2.x << ", row2.y: " << currCam->row2.y << ", row2.z: " << currCam->row2.z << ", row2.w: " << currCam->row2.w << endl;
                        cout << "[GlobalData::loadCalibData] - row3.x: " << currCam->row3.x << ", row3.y: " << currCam->row3.y << ", row3.z: " << currCam->row3.z << ", row3.w: " << currCam->row3.w << endl;
                        cout << "[GlobalData::loadCalibData] - row4.x: " << currCam->row4.x << ", row4.y: " << currCam->row4.y << ", row4.z: " << currCam->row4.z << ", row4.w: " << currCam->row4.w << endl;

                        settings.popTag();
                    }
                    settings.popTag();
                }

                if(currCam->use3D) {
                    total3D ++;
				} else {
				    total2D ++;
				}

				settings.popTag();
			}
			settings.popTag();
		}
		settings.popTag();
	}
}
