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
				int useComp                     = settings.getValue("useRGBCompression", 0);
				currCam->useCompression         = ((useComp == 0) ? false : true);
				currCam->rgbCompressionQuality  = settings.getValue("RGBCompressionQuality", 90);
				currCam->pcDownSample           = settings.getValue("pcDownSample", 4);
				currCam->resolutionDownSample   = settings.getValue("resolutionDownSample", 1.0f);
				currCam->fps                    = settings.getValue("FPS", 24);
				currCam->use2D                  = settings.getValue("use2D", true);
				currCam->use3D                  = settings.getValue("use3D", true);

                if(settings.pushTag("matrix2D")) {
                    currCam->imgrow1.set(settings.getValue("m00", 1.0f), settings.getValue("m01", 1.0f), settings.getValue("m02", 1.0f), settings.getValue("m03", 1.0f));
                    currCam->imgrow2.set(settings.getValue("m10", 1.0f), settings.getValue("m11", 1.0f), settings.getValue("m12", 1.0f), settings.getValue("m13", 1.0f));
                    currCam->imgrow3.set(settings.getValue("m20", 1.0f), settings.getValue("m21", 1.0f), settings.getValue("m22", 1.0f), settings.getValue("m23", 1.0f));
                    currCam->imgrow4.set(settings.getValue("m30", 1.0f), settings.getValue("m31", 1.0f), settings.getValue("m32", 1.0f), settings.getValue("m33", 1.0f));

                    cout << "[GlobalData::loadCalibData] - imgrow1.x: " << currCam->imgrow1.x << ", imgrow1.y: " << currCam->imgrow1.y << ", imgrow1.z: " << currCam->imgrow1.z << ", imgrow1.w: " << currCam->imgrow1.w << endl;
                    cout << "[GlobalData::loadCalibData] - imgrow2.x: " << currCam->imgrow2.x << ", imgrow2.y: " << currCam->imgrow2.y << ", imgrow2.z: " << currCam->imgrow2.z << ", imgrow2.w: " << currCam->imgrow2.w << endl;
                    cout << "[GlobalData::loadCalibData] - imgrow3.x: " << currCam->imgrow3.x << ", imgrow3.y: " << currCam->imgrow3.y << ", imgrow3.z: " << currCam->imgrow3.z << ", imgrow3.w: " << currCam->imgrow3.w << endl;
                    cout << "[GlobalData::loadCalibData] - imgrow4.x: " << currCam->imgrow4.x << ", imgrow4.y: " << currCam->imgrow4.y << ", imgrow4.z: " << currCam->imgrow4.z << ", imgrow4.w: " << currCam->imgrow4.w << endl;

                    settings.popTag();
                }

                if(settings.pushTag("matrix3D")) {
                    currCam->row1.set(settings.getValue("m00", 1.0f), settings.getValue("m01", 1.0f), settings.getValue("m02", 1.0f), settings.getValue("m03", 1.0f));
                    currCam->row2.set(settings.getValue("m10", 1.0f), settings.getValue("m11", 1.0f), settings.getValue("m12", 1.0f), settings.getValue("m13", 1.0f));
                    currCam->row3.set(settings.getValue("m20", 1.0f), settings.getValue("m21", 1.0f), settings.getValue("m22", 1.0f), settings.getValue("m23", 1.0f));
                    currCam->row4.set(settings.getValue("m30", 1.0f), settings.getValue("m31", 1.0f), settings.getValue("m32", 1.0f), settings.getValue("m33", 1.0f));

                    cout << "[GlobalData::loadCalibData] - row1.x: " << currCam->row1.x << ", row1.y: " << currCam->row1.y << ", row1.z: " << currCam->row1.z << ", row1.w: " << currCam->row1.w << endl;
                    cout << "[GlobalData::loadCalibData] - row2.x: " << currCam->row2.x << ", row2.y: " << currCam->row2.y << ", row2.z: " << currCam->row2.z << ", row2.w: " << currCam->row2.w << endl;
                    cout << "[GlobalData::loadCalibData] - row3.x: " << currCam->row3.x << ", row3.y: " << currCam->row3.y << ", row3.z: " << currCam->row3.z << ", row3.w: " << currCam->row3.w << endl;
                    cout << "[GlobalData::loadCalibData] - row4.x: " << currCam->row4.x << ", row4.y: " << currCam->row4.y << ", row4.z: " << currCam->row4.z << ", row4.w: " << currCam->row4.w << endl;

                    settings.popTag();
                }
                if(settings.pushTag("dataContext")) {
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
