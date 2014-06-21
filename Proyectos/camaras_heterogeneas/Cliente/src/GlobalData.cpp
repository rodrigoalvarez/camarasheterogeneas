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

		goLive            = settings.getValue("realTime",       1);
		realtimeFPS       = settings.getValue("realTimeFPS",    10);
		realtimePORT      = settings.getValue("realTimePort",   3232);

        if(settings.pushTag("cameras2D")) {
		    for (int i = 0; settings.pushTag("camera2D", i); i++) {

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
				currCam->fps                    = settings.getValue("realTime", 24);
				currCam->colorRGB               = settings.getValue("colorRGB", true);
				currCam->use2D                  = settings.getValue("use2D", true);
				currCam->use3D                  = settings.getValue("use3D", true);
				currCam->near3D                 = settings.getValue("near3D", 0);
				currCam->far3D                  = settings.getValue("far3D", 1);
				currCam->points3DDownSample     = settings.getValue("points3DDownSample", 0.5);
				//currCam->dataContext            = settings.getValue("dataContext", "");

				if(settings.pushTag("translation")) {
				    currCam->translation                = new t_translation();
					currCam->translation->translationX  = settings.getValue("x", 0);
					currCam->translation->translationY  = settings.getValue("y", 0);
					currCam->translation->translationZ  = settings.getValue("z", 0);
					settings.popTag();
				}

				if(settings.pushTag("rotation")) {
				    currCam->rotation   = new t_rotation();
					currCam->rotation->rotation00   = settings.getValue("r00", 1);
					currCam->rotation->rotation01   = settings.getValue("r01", 0);
					currCam->rotation->rotation02   = settings.getValue("r02", 0);
					currCam->rotation->rotation10   = settings.getValue("r10", 0);
					currCam->rotation->rotation11   = settings.getValue("r11", 1);
					currCam->rotation->rotation12   = settings.getValue("r12", 0);
					currCam->rotation->rotation20   = settings.getValue("r20", 0);
					currCam->rotation->rotation21   = settings.getValue("r21", 0);
					currCam->rotation->rotation22   = settings.getValue("r22", 1);
					settings.popTag();
				}
				settings.popTag();

				if(currCam->use3D) {
                    total3D ++;
				} else {
				    total2D ++;
				}
			}
			settings.popTag();
		}
		settings.popTag();
	}
}
