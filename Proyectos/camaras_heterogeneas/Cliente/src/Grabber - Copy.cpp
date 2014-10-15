#include "Grabber.h"
#include "Thread2D.h"

//--------------------------------------------------------------
void Grabber::setup() {

    gdata       = new GlobalData();
    gdata->loadCalibData("settings.xml");

    switch(gdata->sys_data->logLevel) {
        case 0: ofSetLogLevel(OF_LOG_VERBOSE); break;
        case 1: ofSetLogLevel(OF_LOG_NOTICE); break;
        case 2: ofSetLogLevel(OF_LOG_WARNING); break;
        case 3: ofSetLogLevel(OF_LOG_ERROR); break;
        case 4: ofSetLogLevel(OF_LOG_FATAL_ERROR); break;
        case 5: ofSetLogLevel(OF_LOG_SILENT); break;
        default:ofSetLogLevel(OF_LOG_VERBOSE); break;
    }

    FrameUtils::init();

    ofLogToFile("client_log.txt", false);

    total2D     = gdata->total2D;    //Hacer que se cargue dinámico.
    total3D     = gdata->total3D;    //Hacer que se cargue dinámico.

    ofSetFrameRate(gdata->sys_data->fps);

    if((gdata->total2D + gdata->total3D) > 0) {
        tData = new ThreadData[gdata->total2D + gdata->total3D];
        for(int w = 0; w < (gdata->total2D + gdata->total3D); w++) {
            tData[w].cliId = gdata->sys_data->cliId;
        }
    } else {
        tData = NULL;
    }

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

    if(gdata->sys_data->goLive == 1) { //Si desde el Calibrator se indicó que se debe trasmitir en vivo.
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

    if(gdata->sys_data->goLive == 1) {
        transmitter.stopThread();
    }

    ofSleepMillis(1500);

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

//Operación invocada por Transmitter para refrezcar la información a trasmitir.
void Grabber::updateThreadData() {
    ofLogVerbose() << "[Grabber::updateThreadData] " << " entrando.";
    int di      = 0;
    int i       = 0;
    //tData[i].state = //0-No inited, 1-Only Image, 2-Only Point Cloud, 3-Ambas
    /**
    * ACTUALIZO LA INFO DE LAS CÁMARAS 2D
    */
    for(i; i<gdata->total2D; i++) {
        t2D[i].lock();
        tData[di].state    = 0;
        if(t2D[i].isDeviceInitted() && t2D[i].isDataAllocated()) { //Si la cámara está inicializada.
            tData[di].state  = DEVICE_2D; // 2D
            tData[di].img.setFromPixels(t2D[i].img.getPixels(), t2D[i].img.getWidth(), t2D[i].img.getHeight(), OF_IMAGE_COLOR, true);
            tData[di].abc.set(t2D[i].context->abc.x, t2D[i].context->abc.y, t2D[i].context->abc.z);
            tData[di].xyz.set(t2D[i].context->xyz.x, t2D[i].context->xyz.y, t2D[i].context->xyz.z);
            //tData[di].img.saveImage("desde_imagen.jpg");
        }
        tData[di].nubeW = tData[di].nubeH = 0;
        gettimeofday(&tData[di].curTime, NULL);
        t2D[i].unlock();
        di++;
    }

    /**
    * ACTUALIZO LA INFO DE LAS CÁMARAS 3D
    */
    i = 0;
    for(i; i<gdata->total3D; i++) {

        t3D[i].lock();
        tData[di].state    = 0;

        if(t3D[i].isDeviceInitted()) { //Si la cámara está inicializada.

            if(t3D[i].context->use2D == 1) {
                tData[di].state    = DEVICE_2D;
                //Clono la imágen

                tData[di].img.setFromPixels(t3D[i].img.getPixels(), t3D[i].img.getWidth(), t3D[i].img.getHeight(), OF_IMAGE_COLOR, true);
                tData[di].img.saveImage("desde_kinect.jpg");
                tData[di].nubeW = tData[di].nubeH = 0;
            }

            if(t3D[i].context->use3D == 1) {
                ((t3D[i].context->use2D == 1) ? tData[di].state = DEVICE_2D_3D : tData[di].state = DEVICE_3D);
                //Hacer que esta nube de puntos, cuando tela de, ya te la de transformada.

                tData[di].nubeW         = t3D[i].spix.getWidth();
                tData[di].nubeH         = t3D[i].spix.getHeight();
                XnDepthPixel*  rawPix   = t3D[i].spix.getPixels();

                xn::DepthGenerator& Xn_depth = t3D[i].openNIRecorder->getDepthGenerator();
                ofxOpenNI oni;
                xn::DepthGenerator& Xn_depth2 = oni.getDepthGenerator();

                int downsampling = 2;
                XnPoint3D Point2D, Point3D;

                int y   = 0;
                int x   = 0;
                float d = 0;
                tData[di].nubeLength    = 0;
                ofVec3f v1;
                ofVec3f * vt;
                ofVec3f * vtmp;
                ofLogVerbose() << di << " tData[di].nubeH " << tData[di].nubeH << " tData[di].nubeW " << tData[di].nubeW;

                float * tmpX = NULL;
                float * tmpY = NULL;
                float * tmpZ = NULL;

                if((tData[di].nubeW > 0) && (tData[di].nubeH > 0)) {
                    tmpX = new float[tData[di].nubeW * tData[di].nubeH];
                    tmpY = new float[tData[di].nubeW * tData[di].nubeH];
                    tmpZ = new float[tData[di].nubeW * tData[di].nubeH];
                }

                ofLogVerbose() << t3D[i].context->matrix << endl;

                Point2D.X   = 1.0f;
                Point2D.Y   = 1.0f;
                Point2D.Z   = 1.0f;
                Xn_depth2.ConvertProjectiveToRealWorld(1, &Point2D, &Point3D);

                ofVec3f vtrans;
                vtrans.set(Point3D.X, Point3D.Y, Point3D.Z);

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

                                        //Xn_depth2.ConvertProjectiveToRealWorld(1, &Point2D, &Point3D);
                                        v1.set(x, y, (float)d);
                                        vtmp    =   transformPointDepth(v1, vtrans);
                                        vt      =   transformPoint(*vtmp, t3D[i].context->matrix);
                                        tmpX[tData[di].nubeLength] = vt->x;
                                        tmpY[tData[di].nubeLength] = vt->y;
                                        tmpZ[tData[di].nubeLength] = vt->z;
                                        tData[di].nubeLength ++;
                                        delete vtmp;
                                        delete vt;

                                    } catch(...) {
                                        ofLogVerbose() << "[Grabber::updateThreadData] " << "Excepción al transformar los puntos.";
                                    }
                                }
                            }
                        }
                    }
                }

                //
                char nombre2[50];
                sprintf(nombre2, "frame1.xyz", i);

                //Creo el archivo b,n de la nube unida
                FILE * pFile;
                pFile = fopen (nombre2,"w");
                //Recorro los frames de cada camara y me quedo solo con los 3D
                for(int i=0; i < tData[di].nubeLength; i ++) {
                    fprintf (pFile, "%f %f %f\n", tmpX[i], tmpY[i], tmpZ[i]);
                }

                fclose (pFile);
                //

                if(tData[di].nubeLength > 0) {
                    tData[di].xpix = new float[tData[di].nubeLength];
                    tData[di].ypix = new float[tData[di].nubeLength];
                    tData[di].zpix = new float[tData[di].nubeLength];

                    memcpy(tData[di].xpix,     tmpX,     sizeof(float) * tData[di].nubeLength);
                    memcpy(tData[di].ypix,     tmpY,     sizeof(float) * tData[di].nubeLength);
                    memcpy(tData[di].zpix,     tmpZ,     sizeof(float) * tData[di].nubeLength);

                }
                if(tmpX != NULL) {
                    delete tmpX;
                    delete tmpY;
                    delete tmpZ;
                }

                ofLogVerbose() << "[Grabber::updateThreadData] " << " saliendo del for for.";

            }
        }
        gettimeofday(&tData[di].curTime, NULL);
        t3D[i].unlock();
        di++;

    }

}

ofVec3f * Grabber::transformPointDepth(ofVec3f point, ofVec3f vtrans) {
    ofVec3f v;
    v.set(point.x, point.y, point.z);
    v            = vtrans * v;
    ofVec3f * v3 = new ofVec3f(v.x, v.y, v.z);
    return v3;
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
