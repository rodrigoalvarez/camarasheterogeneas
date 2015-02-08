#include "GlobalData.h"

//--------------------------------------------------------------
int GlobalData::getFPS() {

}

bool GlobalData::getGoLive() {

}

int GlobalData::getTotal3D() {
    return total3D;
}

int GlobalData::getTotal2D() {
    return total2D;
}

int GlobalData::getTotalDevices() {

}

void GlobalData::loadCalibData(char * xml) {
    ofxXmlSettings settings;
    settings.loadFile(xml);

    total2D = 0;
    total3D = 0;
    if (settings.pushTag("settings")) {
        nClientes = settings.getNumTags("client");
        sys_data = new t_data[nClientes];

        for (int k = 0; k < nClientes; k++) {
            if (settings.pushTag("client", k)) {
                sys_data[k].cliId           = settings.getValue("cliId",          0);
                sys_data[k].cliPort         = settings.getValue("cliPort",        15000);
                sys_data[k].serverIp        = settings.getValue("serverIp",       "127.0.0.1");
                sys_data[k].serverPort      = settings.getValue("serverPort",     11969);
                sys_data[k].goLive          = settings.getValue("realTime",       0);
                sys_data[k].persistence     = settings.getValue("persistence",    1);
                sys_data[k].logLevel        = settings.getValue("logLevel",       0);
                sys_data[k].fps             = settings.getValue("fps",            10);
                sys_data[k].maxPackageSize  = settings.getValue("maxPackageSize", 60000);
                sys_data[k].alfaCoord       = settings.getValue("alfaCoord", 0.0);

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

                        if(settings.pushTag("depthSettings")) {
                            sys_data[k].camera[i].near3D                 = settings.getValue("near", 0);
                            sys_data[k].camera[i].far3D                  = settings.getValue("far", 1);
                            sys_data[k].camera[i].points3DDownSample     = settings.getValue("pointsDownSample", 0.5);
                            settings.popTag();
                        }
                        if(settings.pushTag("matrixA2D")) {
                            sys_data[k].camera[i].imgrowA1.set(settings.getValue("m00", 1.0f), settings.getValue("m01", 1.0f), settings.getValue("m02", 1.0f), settings.getValue("m03", 1.0f));
                            sys_data[k].camera[i].imgrowA2.set(settings.getValue("m10", 1.0f), settings.getValue("m11", 1.0f), settings.getValue("m12", 1.0f), settings.getValue("m13", 1.0f));
                            sys_data[k].camera[i].imgrowA3.set(settings.getValue("m20", 1.0f), settings.getValue("m21", 1.0f), settings.getValue("m22", 1.0f), settings.getValue("m23", 1.0f));
                            sys_data[k].camera[i].imgrowA4.set(settings.getValue("m30", 1.0f), settings.getValue("m31", 1.0f), settings.getValue("m32", 1.0f), settings.getValue("m33", 1.0f));
                            settings.popTag();
                        }
                        if(settings.pushTag("matrixB2D")) {
                            sys_data[k].camera[i].imgrowB1.set(settings.getValue("m00", 1.0f), settings.getValue("m01", 1.0f), settings.getValue("m02", 1.0f), settings.getValue("m03", 1.0f));
                            sys_data[k].camera[i].imgrowB2.set(settings.getValue("m10", 1.0f), settings.getValue("m11", 1.0f), settings.getValue("m12", 1.0f), settings.getValue("m13", 1.0f));
                            sys_data[k].camera[i].imgrowB3.set(settings.getValue("m20", 1.0f), settings.getValue("m21", 1.0f), settings.getValue("m22", 1.0f), settings.getValue("m23", 1.0f));
                            sys_data[k].camera[i].imgrowB4.set(settings.getValue("m30", 1.0f), settings.getValue("m31", 1.0f), settings.getValue("m32", 1.0f), settings.getValue("m33", 1.0f));
                            settings.popTag();
                        }
                        if(settings.pushTag("matrix3D")) {
                            sys_data[k].camera[i].row1.set(settings.getValue("m00", 1.0f), settings.getValue("m01", 1.0f), settings.getValue("m02", 1.0f), settings.getValue("m03", 1.0f));
                            sys_data[k].camera[i].row2.set(settings.getValue("m10", 1.0f), settings.getValue("m11", 1.0f), settings.getValue("m12", 1.0f), settings.getValue("m13", 1.0f));
                            sys_data[k].camera[i].row3.set(settings.getValue("m20", 1.0f), settings.getValue("m21", 1.0f), settings.getValue("m22", 1.0f), settings.getValue("m23", 1.0f));
                            sys_data[k].camera[i].row4.set(settings.getValue("m30", 1.0f), settings.getValue("m31", 1.0f), settings.getValue("m32", 1.0f), settings.getValue("m33", 1.0f));
                            settings.popTag();
                        }

                        if (sys_data[k].camera[i].use2D)
                            total2D ++;
                        if (sys_data[k].camera[i].use3D)
                            total3D ++;

                        settings.popTag();
                    }
                    settings.popTag();
                }
                settings.popTag();
            }
        }
    }
}

/*void GlobalData::loadCalibData(char * xml) {
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
                        sys_data[k].camera[i].use2D                  = settings.getValue("use2D", false);
                        sys_data[k].camera[i].use3D                  = settings.getValue("use3D", false);

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

                                settings.popTag();
                            }
                            settings.popTag();
                        }

                        if (sys_data[k].camera[i].use2D)
                            total2D ++;
                        if (sys_data[k].camera[i].use3D)
                            total3D ++;

                        settings.popTag();
                    }
                    settings.popTag();
                }
                settings.popTag();
            }
	    }
	}
}*/
