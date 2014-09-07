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
        cliId             = settings.getValue("cliId",          1);
        port              = settings.getValue("port",           15000);
		goLive            = settings.getValue("realTime",       1);
		realtimeFPS       = settings.getValue("realTimeFPS",    10);
		realtimePORT      = settings.getValue("realTimePort",   3232);

        if(settings.pushTag("cameras")) {
            //int i = 0;
            cout << " camara " << settings.getNumTags("camera") << endl;
            //while(settings.pushTag("camera", i)) {

		    //for (int i = 0; settings.pushTag("camera", i); i++) {
		    int totCams = settings.getNumTags("camera");
		    for (int i = 0; i < totCams; i++) {
		        cout << " camara " << i << endl;
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
				currCam->colorRGB               = settings.getValue("colorRGB", true);
				currCam->use2D                  = settings.getValue("use2D", true);
				currCam->use3D                  = settings.getValue("use3D", true);

                if(settings.pushTag("dataContext")) {
                    if(settings.pushTag("depthSettings")) {
                        currCam->near3D                 = settings.getValue("near", 0);
                        currCam->far3D                  = settings.getValue("far", 1);
                        currCam->points3DDownSample     = settings.getValue("pointsDownSample", 0.5);
                        settings.popTag();
                    }
                    if(settings.pushTag("matrix")) {
                        currCam->matrix.set(settings.getValue("m000", 1),
                                           settings.getValue("m001", 0),
                                           settings.getValue("m002", 0),
                                           settings.getValue("m003", 0),
                                           settings.getValue("m100", 0),
                                           settings.getValue("m101", 1),
                                           settings.getValue("m102", 0),
                                           settings.getValue("m103", 0),
                                           settings.getValue("m200", 0),
                                           settings.getValue("m201", 0),
                                           settings.getValue("m202", 1),
                                           settings.getValue("m203", 0),
                                           settings.getValue("m300", 0),
                                           settings.getValue("m301", 0),
                                           settings.getValue("m302", 0),
                                           settings.getValue("m303", 1)
                                           );
                        settings.popTag();
                    }
                    if(settings.pushTag("imgTransform")) {
                        currCam->xyz.set(settings.getValue("x", 1), settings.getValue("y", 1), settings.getValue("z", 1));
                        currCam->abc.set(settings.getValue("a", 1), settings.getValue("b", 1), settings.getValue("c", 1));
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
