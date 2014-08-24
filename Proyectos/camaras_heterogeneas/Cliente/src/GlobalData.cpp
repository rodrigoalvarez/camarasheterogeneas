#include "GlobalData.h"


int GlobalData::getTotal3D() {
	return total3D;
}

int GlobalData::getTotal2D() {
	return total2D;
}

int GlobalData::getTotalDevices() {
	return total2D + total3D;
}

void GlobalData::loadCalibData(char * xml) {

    ofxXmlSettings settings;
    settings.loadFile(xml);
    total2D = 0;
    total3D = 0;

	if (settings.pushTag("settings")) {

		REAL_TIME           = settings.getValue("realTime",       true);
		REAL_TIME_FPS       = settings.getValue("realTimeFPS",    10);
		REAL_TIME_PORT      = settings.getValue("realTimePort",   3232);

        if (settings.pushTag("cameras")) {
		    for (int i = 0; settings.pushTag("camera", i) && i < 3; i++) {

				Camera currCam							= cameras[i];
                currCam.Id								= settings.getValue("id", 0);
				currCam.ResolutionX						= settings.getValue("resolutionX", 640);
				currCam.ResolutionY						= settings.getValue("resolutionY", 480);
				currCam.ResolutionDownSample			= settings.getValue("resolutionDownSample", 1);
				currCam.FPS								= settings.getValue("FPS", 24);
				currCam.ColorRGB						= settings.getValue("colorRGB", true);
				currCam.Use2D							= settings.getValue("use2D", true);
				currCam.Use3D							= settings.getValue("use", true);
				currCam.DepthSettings.Near				= settings.getValue("near", 0);
				currCam.DepthSettings.Far				= settings.getValue("far", 1);
				currCam.DepthSettings.PointsDownSample  = settings.getValue("pointsDownSample", 0.5);
				currCam.DataContext						= settings.getValue("dataContext", "");

				if (settings.pushTag("matrix")) {
					for (int k = 0; k < 16; k++) {
						std::stringstream cellM;
						cellM << "m" << k / 4 << k % 4;
						currCam.Matrix[k] = settings.getValue("m00", 0);
					}
					settings.popTag();
				}
				settings.popTag();

				if (currCam.Use3D) {
                    total3D++;
				} if (currCam.Use2D) {
				    total2D++;
				}
			}
			settings.popTag();
		}
		settings.popTag();
	}
}
