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

    total2D = 0;
    total3D = 0;

	if(settings.pushTag("settings")) {
	    nClientes = settings.getNumTags("cliente");
        cout << "Numero de clientes: " << nClientes << endl;
        sys_data = new t_data[nClientes];

	    for(int k = 0; k < nClientes; k++){



            if(settings.pushTag("cliente",k)) {
                sys_data[k].cliId           = settings.getValue("cliId",          0);
                sys_data[k].cliPort         = settings.getValue("cliPort",        15000);
                sys_data[k].serverIp        = settings.getValue("serverIp",       "127.0.0.1");
                sys_data[k].serverPort      = settings.getValue("serverPort",     11969);
                sys_data[k].goLive          = settings.getValue("realTime",       0);
                sys_data[k].persistence     = settings.getValue("persistence",    1);
                sys_data[k].logLevel        = settings.getValue("logLevel",                 0);
                sys_data[k].fps             = settings.getValue("fps",            10);
                sys_data[k].maxPackageSize  = settings.getValue("maxPackageSize", 60000);

                if(settings.pushTag("cameras")) {
                    sys_data[k].nCamaras = settings.getNumTags("camera");
                    sys_data[k].camera = new t_camera[sys_data[k].nCamaras];
                    for (int i = 0; i < sys_data[k].nCamaras; i++) {
                        settings.pushTag("camera", i);

                        sys_data[k].camera[i].id                     = settings.getValue("id", 0);
                        sys_data[k].camera[i].resolutionX            = settings.getValue("resolutionX", 640);
                        sys_data[k].camera[i].resolutionY            = settings.getValue("resolutionY", 480);
                        sys_data[k].camera[i].resolutionDownSample   = settings.getValue("resolutionDownSample", 1);
                        sys_data[k].camera[i].fps                    = settings.getValue("FPS", 24);
                        sys_data[k].camera[i].use2D                  = settings.getValue("use2D", true);
                        sys_data[k].camera[i].use3D                  = settings.getValue("use3D", true);

                        if(settings.pushTag("dataContext")) {
                            if(settings.pushTag("depthSettings")) {
                                sys_data[k].camera[i].near3D                 = settings.getValue("near", 0);
                                sys_data[k].camera[i].far3D                  = settings.getValue("far", 1);
                                sys_data[k].camera[i].points3DDownSample     = settings.getValue("pointsDownSample", 0.5);
                                settings.popTag();
                            }
                            if(settings.pushTag("matrix")) {
                                sys_data[k].camera[i].row1.set(settings.getValue("m000", 1.0f), settings.getValue("m001", 1.0f), settings.getValue("m002", 1.0f), settings.getValue("m003", 1.0f));
                                sys_data[k].camera[i].row2.set(settings.getValue("m100", 1.0f), settings.getValue("m101", 1.0f), settings.getValue("m102", 1.0f), settings.getValue("m103", 1.0f));
                                sys_data[k].camera[i].row3.set(settings.getValue("m200", 1.0f), settings.getValue("m201", 1.0f), settings.getValue("m202", 1.0f), settings.getValue("m203", 1.0f));
                                sys_data[k].camera[i].row4.set(settings.getValue("m300", 1.0f), settings.getValue("m301", 1.0f), settings.getValue("m302", 1.0f), settings.getValue("m303", 1.0f));

                                cout << "[GlobalData::loadCalibData] - row1.x: " << sys_data[k].camera[i].row1.x << ", row1.y: " << sys_data[k].camera[i].row1.y << ", row1.z: " << sys_data[k].camera[i].row1.z << ", row1.w: " << sys_data[k].camera[i].row1.w << endl;
                                cout << "[GlobalData::loadCalibData] - row2.x: " << sys_data[k].camera[i].row2.x << ", row2.y: " << sys_data[k].camera[i].row2.y << ", row2.z: " << sys_data[k].camera[i].row2.z << ", row2.w: " << sys_data[k].camera[i].row2.w << endl;
                                cout << "[GlobalData::loadCalibData] - row3.x: " << sys_data[k].camera[i].row3.x << ", row3.y: " << sys_data[k].camera[i].row3.y << ", row3.z: " << sys_data[k].camera[i].row3.z << ", row3.w: " << sys_data[k].camera[i].row3.w << endl;
                                cout << "[GlobalData::loadCalibData] - row4.x: " << sys_data[k].camera[i].row4.x << ", row4.y: " << sys_data[k].camera[i].row4.y << ", row4.z: " << sys_data[k].camera[i].row4.z << ", row4.w: " << sys_data[k].camera[i].row4.w << endl;

                                settings.popTag();
                            }
                            settings.popTag();
                        }

                        if (sys_data[k].camera[i].use2D)
                            total2D ++;
                        if (sys_data[k].camera[i].use3D)
                            total2D ++;

                        settings.popTag();
                    }
                    settings.popTag();
                }
                settings.popTag();
            }
	    }
	}
}
