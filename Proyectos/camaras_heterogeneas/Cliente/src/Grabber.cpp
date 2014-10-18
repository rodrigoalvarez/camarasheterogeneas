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
        tData[di].camId    = t2D[i].context->id;
        tData[di].state    = 0;
        if(t2D[i].isDeviceInitted() && t2D[i].isDataAllocated()) { //Si la cámara está inicializada.
            tData[di].state  = DEVICE_2D; // 2D
            tData[di].img.setFromPixels(t2D[i].img.getPixels(), t2D[i].img.getWidth(), t2D[i].img.getHeight(), OF_IMAGE_COLOR, true);

            tData[di].row1.set(t3D[i].context->row1.x, t3D[i].context->row1.y, t3D[i].context->row1.z, t3D[i].context->row1.w);
            tData[di].row2.set(t3D[i].context->row2.x, t3D[i].context->row2.y, t3D[i].context->row2.z, t3D[i].context->row2.w);
            tData[di].row3.set(t3D[i].context->row3.x, t3D[i].context->row3.y, t3D[i].context->row3.z, t3D[i].context->row3.w);
            tData[di].row4.set(t3D[i].context->row4.x, t3D[i].context->row4.y, t3D[i].context->row4.z, t3D[i].context->row4.w);

            ofLogVerbose() << "[Grabber::updateThreadData] 2D - row1.x: " << tData[di].row1.x << ", row1.y: " << tData[di].row1.y << ", row1.z: " << tData[di].row1.z << ", row1.w: " << tData[di].row1.w;
            ofLogVerbose() << "[Grabber::updateThreadData] 2D - row2.x: " << tData[di].row2.x << ", row2.y: " << tData[di].row2.y << ", row2.z: " << tData[di].row2.z << ", row2.w: " << tData[di].row2.w;
            ofLogVerbose() << "[Grabber::updateThreadData] 2D - row3.x: " << tData[di].row3.x << ", row3.y: " << tData[di].row3.y << ", row3.z: " << tData[di].row3.z << ", row3.w: " << tData[di].row3.w;
            ofLogVerbose() << "[Grabber::updateThreadData] 2D - row4.x: " << tData[di].row4.x << ", row4.y: " << tData[di].row4.y << ", row4.z: " << tData[di].row4.z << ", row4.w: " << tData[di].row4.w;
        }
        tData[di].nubeW = tData[di].nubeH = 0;
        gettimeofday(&tData[di].curTime, NULL);
        t2D[i].unlock();
        di++;
    }

    /**
    * ACTUALIZO LA INFO DE LAS CÁMARAS 3D
    */
    ofLogVerbose() << "ACTUALIZO LA INFO DE LAS CÁMARAS 3D" << endl;
    i = 0;
    for(i; i<gdata->total3D; i++) {
        t3D[i].lock();
        tData[di].state    = 0;
        tData[di].camId    = t3D[i].context->id;

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

                int downsampling = 2;
                XnPoint3D Point2D, Point3D;

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

                tData[di].row1.set(t3D[i].context->row1.x, t3D[i].context->row1.y, t3D[i].context->row1.z, t3D[i].context->row1.w);
                tData[di].row2.set(t3D[i].context->row2.x, t3D[i].context->row2.y, t3D[i].context->row2.z, t3D[i].context->row2.w);
                tData[di].row3.set(t3D[i].context->row3.x, t3D[i].context->row3.y, t3D[i].context->row3.z, t3D[i].context->row3.w);
                tData[di].row4.set(t3D[i].context->row4.x, t3D[i].context->row4.y, t3D[i].context->row4.z, t3D[i].context->row4.w);

                ofLogVerbose() << "[Grabber::updateThreadData] 3D - row1.x: " << tData[di].row1.x << ", row1.y: " << tData[di].row1.y << ", row1.z: " << tData[di].row1.z << ", row1.w: " << tData[di].row1.w;
                ofLogVerbose() << "[Grabber::updateThreadData] 3D - row2.x: " << tData[di].row2.x << ", row2.y: " << tData[di].row2.y << ", row2.z: " << tData[di].row2.z << ", row2.w: " << tData[di].row2.w;
                ofLogVerbose() << "[Grabber::updateThreadData] 3D - row3.x: " << tData[di].row3.x << ", row3.y: " << tData[di].row3.y << ", row3.z: " << tData[di].row3.z << ", row3.w: " << tData[di].row3.w;
                ofLogVerbose() << "[Grabber::updateThreadData] 3D - row4.x: " << tData[di].row4.x << ", row4.y: " << tData[di].row4.y << ", row4.z: " << tData[di].row4.z << ", row4.w: " << tData[di].row4.w;

                ofMatrix4x4 matrix;
                matrix.set( t3D[i].context->row1.x, t3D[i].context->row1.y, t3D[i].context->row1.z, t3D[i].context->row1.w,
                            t3D[i].context->row2.x, t3D[i].context->row2.y, t3D[i].context->row2.z, t3D[i].context->row2.w,
                            t3D[i].context->row3.x, t3D[i].context->row3.y, t3D[i].context->row3.z, t3D[i].context->row3.w,
                            t3D[i].context->row4.x, t3D[i].context->row4.y, t3D[i].context->row4.z, t3D[i].context->row4.w);

                ofLogVerbose() << matrix << endl;

                for(y=0; y < tData[di].nubeH; y += downsampling) {
                    for(x=0; x < tData[di].nubeW; x += downsampling) {
                        d = rawPix[y * tData[di].nubeW + x];
                        if(d != 0) {
                            Point2D.X   = x;
                            Point2D.Y   = y;
                            Point2D.Z   = (float)d;
                            try {

                                Xn_depth.ConvertProjectiveToRealWorld(1, &Point2D, &Point3D);
                                v1.set(Point3D.X, Point3D.Y, Point3D.Z);

                                vt = transformPoint(v1, matrix);
                                if(((int)vt->x == 0) && ((int)vt->x == 0) && ((int)vt->x == 0)) {

                                } else {
                                    tmpX[tData[di].nubeLength] = vt->x;
                                    tmpY[tData[di].nubeLength] = vt->y;
                                    tmpZ[tData[di].nubeLength] = vt->z;
                                    tData[di].nubeLength ++;
                                }
                                delete vt;

                            } catch(...) {
                                ofLogVerbose() << "[Grabber::updateThreadData] " << "Excepción al transformar los puntos.";
                            }

                        }/* else {
                            tmpX[tData[di].nubeLength] = 0;
                            tmpY[tData[di].nubeLength] = 0;
                            tmpZ[tData[di].nubeLength] = 0;
                        }*/
                    }
                }

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
                /**/
            }
        }
        gettimeofday(&tData[di].curTime, NULL);
        t3D[i].unlock();
        di++;
    }

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
