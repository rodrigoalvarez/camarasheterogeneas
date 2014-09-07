#include "Grabber.h"
#include "Thread2D.h"

//--------------------------------------------------------------
void Grabber::setup() {
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofLogToFile("client_log.txt", false);
    ofSetFrameRate(1);
    gdata       = new GlobalData();
    gdata->loadCalibData("settings.xml");

    total2D     = gdata->total2D;    //Hacer que se cargue din�mico.
    total3D     = gdata->total3D;    //Hacer que se cargue din�mico.
    goLive      = gdata->sys_data->goLive;     //Hacer que se cargue din�mico.


    if((gdata->total2D + gdata->total3D) > 0) {
        tData = new ThreadData[gdata->total2D + gdata->total3D];
        for(int w = 0; w < (gdata->total2D + gdata->total3D); w++) {
            tData[w].cliId = 1;
        }
    } else {
        tData = NULL;
    }

    printf("total2D, %d \n", gdata->total2D);

    t_camera * camera = gdata->camera;

    if(gdata->total2D > 0) {
        t2D   = new Thread2D[gdata->total2D];
    }

    if(gdata->total3D > 0) {
        t3D   = new Thread3D[gdata->total3D];
    }

    int i2D = 0;
    int i3D = 0;

    while(camera != NULL) {
        if(camera->use3D == 1) {
            t3D[i3D].sys_data                   = gdata->sys_data;
            t3D[i3D].context                    = camera;
            t3D[i3D].startThread(true, false);
            i3D ++;
        } else {
            t2D[i2D].sys_data                   = gdata->sys_data;
            t2D[i2D].context                    = camera;
            t2D[i2D].startThread(true, false);
            i2D ++;
        }
        camera = camera->sig;
    }

    if(goLive) { //Si desde el Calibrator se indic� que se debe trasmitir en vivo.
        transmitter.grabber     = this;
        transmitter.sys_data    = gdata->sys_data;
        transmitter.startThread(true, false);
    }
}

//--------------------------------------------------------------
void Grabber::update() {

}

//--------------------------------------------------------------
void Grabber::draw() {

}

void Grabber::exit() {
    //Stop the thread
    int i       = 0;
    for(i; i<gdata->total2D; i++) {
        t2D[i].stopThread();
    }

    i = 0;
    for(i; i<gdata->total3D; i++) {
        t3D[i].stopThread();
    }

    if(goLive) {
        transmitter.stopThread();
    }

    if(t2D != NULL) {
        delete t2D;
    }

    if(t3D != NULL) {
        delete t3D;
    }

    if(tData != NULL) {
        delete tData;
    }

    if(gdata != NULL) {
        delete gdata;
    }
}

//Operaci�n invocada por Transmitter para refrezcar la informaci�n a trasmitir.
void Grabber::updateThreadData() {
    ofLogVerbose() << "[Grabber::updateThreadData] " << " entrando.";
    //return;
    int di      = 0;
    int i       = 0;
    //tData[i].state = //0-No inited, 1-Only Image, 2-Only Point Cloud, 3-Ambas
    /**
    * ACTUALIZO LA INFO DE LAS C�MARAS 2D
    */
    for(i; i<gdata->total2D; i++) {
        t2D[i].lock();
        tData[di].state    = 0;
        if(t2D[i].isDeviceInitted() && t2D[i].isDataAllocated()) { //Si la c�mara est� inicializada.
            tData[di].state  = DEVICE_2D; // 2D
            if(t2D[i].context->colorRGB == 1) {
                tData[di].img.setFromPixels(t2D[i].img.getPixels(), t2D[i].img.width, t2D[i].img.height, OF_IMAGE_COLOR, true); //Clono la im�gen
            } else {
                tData[di].img.setFromPixels(t2D[i].img.getPixels(), t2D[i].img.width, t2D[i].img.height, OF_IMAGE_GRAYSCALE, true); //Clono la im�gen
            }
        }
        gettimeofday(&tData[di].curTime, NULL);
        t2D[i].unlock();
        di++;
    }

    /**
    * ACTUALIZO LA INFO DE LAS C�MARAS 3D
    */
    i = 0;
    for(i; i<gdata->total3D; i++) {
        t3D[i].lock();
        tData[di].state    = 0;

        if(t3D[i].isDeviceInitted()) { //Si la c�mara est� inicializada.

            if(t3D[i].context->use2D == 1) {
                tData[di].state    = DEVICE_2D;
                //Clono la im�gen
                if(t3D[i].context->colorRGB == 1) {
                    tData[di].img.setFromPixels(t3D[i].img.getPixels(), t3D[i].img.width, t3D[i].img.height, OF_IMAGE_COLOR, true);
                } else {
                    tData[di].img.setFromPixels(t3D[i].img.getPixels(), t3D[i].img.width, t3D[i].img.height, OF_IMAGE_GRAYSCALE, true);
                }
            }

            if(t3D[i].context->use3D == 1) {
                ((t3D[i].context->use2D == 1) ? tData[di].state = DEVICE_2D_3D : tData[di].state = DEVICE_3D);
                //Hacer que esta nube de puntos, cuando tela de, ya te la de transformada.

                tData[di].nubeW         = t3D[i].spix.getWidth();
                tData[di].nubeH         = t3D[i].spix.getHeight();
                XnDepthPixel*  rawPix   = t3D[i].spix.getPixels();

                xn::DepthGenerator& Xn_depth = t3D[i].openNIRecorder->getDepthGenerator();

                int downsampling = 2;
                XnPoint3D Point2D, Point3D;

                /*tData[di].xpix = new float[tData[di].nubeW * tData[di].nubeH];
                tData[di].ypix = new float[tData[di].nubeW * tData[di].nubeH];
                tData[di].zpix = new float[tData[di].nubeW * tData[di].nubeH];*/

                int y   = 0;
                int x   = 0;
                float d = 0;
                tData[di].nubeLength    = 0;
                ofVec3f v1;
                ofVec3f * vt;
                ofLogVerbose() << di << " tData[di].nubeH " << tData[di].nubeH << " tData[di].nubeW " << tData[di].nubeW;

                float * tmpX = NULL;
                float * tmpY = NULL;
                float * tmpZ = NULL;

                if((tData[di].nubeW > 0) && (tData[di].nubeH > 0)) {
                    tmpX = new float[tData[di].nubeW * tData[di].nubeH];
                    tmpY = new float[tData[di].nubeW * tData[di].nubeH];
                    tmpZ = new float[tData[di].nubeW * tData[di].nubeH];
                }

                for(y=0; y < tData[di].nubeH; y += downsampling) {
                    if(y < tData[di].nubeH) {
                        for(x=0; x < tData[di].nubeW; x += downsampling) {
                            if(x < tData[di].nubeW) {
                                d = rawPix[y * tData[di].nubeW + x];
                                if(d != 0) {
                                    Point2D.X   = x;
                                    Point2D.Y   = y;
                                    Point2D.Z   = (float)d;
                                    try {
                                        Xn_depth.ConvertProjectiveToRealWorld(1, &Point2D, &Point3D);
                                        v1.set(Point3D.X, Point3D.Y, Point3D.Z);
                                        vt = transformPoint(v1, t3D[i].context->matrix);
                                        //if(! ((roundf(vt->x) == 0.0) && (roundf(vt->y) == 0.0) && (round(vt->z) == 0.0)) ) {
                                            ofLogVerbose() << "[Grabber::updateThreadData] " << "vt->x: " << vt->x  << "vt->y: " << vt->y << "vt->z: " << vt->z;
                                            tmpX[tData[di].nubeLength] = vt->x;
                                            tmpY[tData[di].nubeLength] = vt->y;
                                            tmpZ[tData[di].nubeLength] = vt->z;
                                            tData[di].nubeLength ++;
                                        //}

                                        delete vt;

                                    } catch(...) {
                                        ofLogVerbose() << "[Grabber::updateThreadData] " << "Excepci�n al transformar los puntos.";
                                    }
                                }
                            }
                        }
                    }
                }

                if(tData[di].nubeLength > 0) {
                    tData[di].xpix = new float[tData[di].nubeLength];
                    tData[di].ypix = new float[tData[di].nubeLength];
                    tData[di].zpix = new float[tData[di].nubeLength];

                    memcpy(tData[di].xpix,     tmpX,     sizeof(float) * tData[di].nubeLength);
                    memcpy(tData[di].ypix,     tmpY,     sizeof(float) * tData[di].nubeLength);
                    memcpy(tData[di].zpix,     tmpZ,     sizeof(float) * tData[di].nubeLength);

                    /*
                    char* fileName      = "frame_i.xyz";
                    FILE * pFile;
                    pFile = fopen (fileName,"w");

                    for(int i=0; i < tData[di].nubeLength; i ++) {
                        fprintf (pFile, "%f %f %f\n", tData[di].xpix[i], tData[di].ypix[i], tData[di].zpix[i]);
                    }*/

                }
                if(tmpX != NULL) {
                    delete tmpX;
                    delete tmpY;
                    delete tmpZ;
                }

                ofLogVerbose() << "[Grabber::updateThreadData] " << " saliendo del for for.";
                /**/
            }
        }
        gettimeofday(&tData[di].curTime, NULL);
        t3D[i].unlock();
        di++;
    }
    cout << " al salir " << tData[0].nubeH << endl;
}

ofVec3f * Grabber::transformPoint(ofVec3f point, ofMatrix4x4 transform) {
    ofVec4f v;
    v.set(point.x, point.y, point.z, 1);
    v            = transform * v;
    ofVec3f * v3 = new ofVec3f(v.x, v.y, v.z);
    return v3;
}

//--------------------------------------------------------------
void Grabber::keyPressed(int key) {
    exit();
}

//--------------------------------------------------------------
void Grabber::keyReleased(int key) {

}

//--------------------------------------------------------------
void Grabber::mouseMoved(int x, int y ) {

}

//--------------------------------------------------------------
void Grabber::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void Grabber::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void Grabber::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void Grabber::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void Grabber::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void Grabber::dragEvent(ofDragInfo dragInfo) {

}
