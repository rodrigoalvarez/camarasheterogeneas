#pragma once

#include <stdio.h>
#include <locale.h>
#include <time.h>
#include <string>
#include <iostream>
#include <sys/time.h>
#include <sstream>
#include "ofxCvShortImage.h"
/*
unsigned int  timestamp;
ofImage img;
int     inited;

void    updateData();
ofShortPixels  spix;
*/
//struct tm *newtime;
//time_t aclock;
class FrameUtils {

	public:
    static ThreadData * getDummyFrame() {

        //time( &aclock );                 /* Get time in seconds */
        //newtime = localtime( &aclock );  /* Convert time to struct */
        /* Print local time as a string */
        //printf( "The current date and time are: %s", asctime( newtime ) );

        ThreadData * tData  = new ThreadData[2];
        tData[0].cliId      = 519; // ID que identificará a esta instalación de cliente en el servidor.
        tData[0].camId      = 2; // ID que identifica esta cámara dentro de la instalación cliente.

        gettimeofday(&tData[0].curTime, NULL);

        tData[0].img.loadImage("foto0.jpg");

        ofxCvShortImage img;
        img.allocate(300, 200);
        tData[0].spix       = img.getShortPixelsRef();

        tData[0].state      = 3; //0-No inited, 1-Only Image, 2-Only Point Cloud, 3-Ambas

        tData[1].cliId      = 1; // ID que identificará a esta instalación de cliente en el servidor.
        tData[1].camId      = 3; // ID que identifica esta cámara dentro de la instalación cliente.
        gettimeofday(&tData[1].curTime, NULL);
        //tData[1].timestamp  = ofGetUnixTime();
        tData[1].img.loadImage("foto1.jpg");
        tData[1].state      = 1; //0-No inited, 1-Only Image, 2-Only Point Cloud, 3-Ambas

        return tData;
    }

    static int getFrameSize(ThreadData * tData, int totalCams) {
            /*
            Aquí tengo que calcular el tamaño total del gran bytearray.
            Cada ThreadData lo voy a transformar en un elemento bien controlado en tamaño para poder castearlo sin problema del otro lado.
            Dicho elemento va a estar compuesto de:
            p/c/thread data
             - (int) cliId; // ID de la configuración de Cliente. Puede haber N.
             - (int) camId; // ID de la cámara en la instalación.
             - (int) state  //0-No inited, 1-Only Image, 2-Only Point Cloud, 3-Ambas
             - (struct timeval) curTime
             - (int) imgWidth
             - (int) imgHeight
             - (unsigned char *) imagebytearray
             - (int) pcWidth
             - (int) pcHeight
             - (ofFloatPixels *) sXpix
             - (ofFloatPixels *) sYpix
             - (ofFloatPixels *) sZpix
            */

        try {
            int i=0;
            int totSize = sizeof(int); // totalCams
            for(i=0; i<totalCams; i++) {
                totSize += sizeof(int);     //(int) cliId
                totSize += sizeof(int);     //(int) camId
                totSize += sizeof(int);     //(int) state
                totSize += sizeof(timeval); //(struct timeval) [long + long]

                //Si (tData[i].state > 0) = Está inicializado
                if(tData[i].state > 0) {
                    //Si tiene imágen;
                    if((tData[i].state == 1) || (tData[i].state == 3)) {
                        totSize += sizeof(int);     //(int) camWidth
                        totSize += sizeof(int);     //(int) camHeight
                        //Reservo lugar para la imágen.

                        ofPixels p = tData[i].img.getPixelsRef();
                        totSize += p.size();
                    }

                    //Si tiene nube;
                    if((tData[i].state == 2) || (tData[i].state == 3)) {
                        totSize += sizeof(int);     //(int) pcWidth
                        totSize += sizeof(int);     //(int) pcHeight
                        totSize += sizeof(int);     //(int) pcLength
                        //Reservo lugar para la nube.

                        totSize += (sizeof(float)) * tData[i].nubeLength * 3;
                    }
                }
            }
            return totSize;

        } catch (...) {
          ofLogWarning() << "[FrameUtils::getFrameSize] - Exception occurred.";
        }

        return -1;
    }

    static int getTotalCamerasFromByteArray(char * bytearray) {
        int totCameras; // = (int) *bytearray;
        try {
            memcpy(&(totCameras), (bytearray),     sizeof(int));
        } catch (...) {
            ofLogWarning() << "[FrameUtils::getTotalCamerasFromByteArray] - Exception occurred.";
        }
        return NULL;
    }

    static std::pair <int, ThreadData *> getThreadDataFromByteArray(char * bytearray) {
        try {
            int totCameras;
            memcpy(&(totCameras), (bytearray),     sizeof(int));
            ofLogVerbose() << "[FrameUtils::getThreadDataFromByteArray] - totCameras " << totCameras;
            if(totCameras > 0) {
                ThreadData * tData  = new ThreadData[totCameras];
                int i;
                char * start = bytearray + sizeof(int);
                char* off_cliId;
                char* off_camId;
                char* off_state;
                char* off_curTime;
                char* off_imgWidth;
                char* off_imgHeight;
                char* off_imagebytearray;
                char* off_pcWidth;
                char* off_pcHeight;
                char* off_pcLength;
                char* off_nubeByteArray;

                for(i=0; i<totCameras; i++) {
                    /*
                     - (int) cliId; // ID de la configuración de Cliente. Puede haber N.
                     - (int) camId; // ID de la cámara en la instalación.
                     - (int) state  //0-No inited, 1-Only Image, 2-Only Point Cloud, 3-Ambas
                     - (timeval) curTime
                     - (int) imgWidth
                     - (int) imgHeight
                     - (unsigned char *) imagebytearray
                     - (int) pcWidth
                     - (int) pcHeight
                     - ofFloatPixels  sXpix;
                     - ofFloatPixels  sYpix;
                     - ofFloatPixels  sZpix;
                    */
                    off_cliId           = start;
                    off_camId           = off_cliId     + sizeof(int);
                    off_state           = off_camId     + sizeof(int);
                    off_curTime         = off_state     + sizeof(int);

                    memcpy(&(tData[i].cliId),   (off_cliId),     sizeof(int));
                    memcpy(&(tData[i].camId),   (off_camId),     sizeof(int));
                    memcpy(&(tData[i].state),   (off_state),     sizeof(int));
                    memcpy(&(tData[i].curTime), (off_curTime),   sizeof(timeval));

                    ofLogVerbose() << "[FrameUtils::getThreadDataFromByteArray] - recuperado - cliId: " << tData[i].cliId;
                    ofLogVerbose() << "[FrameUtils::getThreadDataFromByteArray] - recuperado - camId: " << tData[i].camId;
                    ofLogVerbose() << "[FrameUtils::getThreadDataFromByteArray] - recuperado - state: " << tData[i].state;
                    ofLogVerbose() << "[FrameUtils::getThreadDataFromByteArray] - recuperado - curTime: " << tData[i].curTime.tv_usec << " " << tData[i].curTime.tv_sec;

                    start = off_curTime   + sizeof(timeval);

                    ofLogVerbose() << "tData[i].state " << tData[i].state;
                    //Si (tData[i].state > 0) = Está inicializado
                    if(tData[i].state > 0) {
                        ofLogVerbose() << "[FrameUtils::getThreadDataFromByteArray] - (tData[i].state > 0) ";
                        //Si tiene imágen;
                        if((tData[i].state == 1) || (tData[i].state == 3)) {

                            //Reservo lugar para la imágen.
                            off_imgWidth         = start;
                            off_imgHeight        = off_imgWidth  + sizeof(int);
                            off_imagebytearray   = off_imgHeight + sizeof(int);

                            int w;
                            int h;

                            memcpy(&(w),     (off_imgWidth),     sizeof(int));
                            memcpy(&(h),     (off_imgHeight),    sizeof(int));

                            ofLogVerbose() << "w || h - " << w << " || " << h;

                            ofImage imgDest3;
                            //imgDest3.loadImage("foto0.jpg");
                            try {
                                imgDest3.setFromPixels((unsigned char *) off_imagebytearray, w, h, OF_IMAGE_COLOR, true);
                            }catch(std::bad_alloc& ba) {}

                            ofLogVerbose() << "imgDest3.setFromPixels";
                            char dig = (char)(((int)'0')+i);

                            imgDest3.saveImage("imgDest" + ofToString(i) + ".jpg");

                            start = off_imagebytearray + w * h * 3;
                        }

                        off_pcWidth = start;

                        //Si tiene nube;
                        if((tData[i].state == 2) || (tData[i].state == 3)) {

                            off_pcHeight         = off_pcWidth  + sizeof(int);
                            off_pcLength         = off_pcHeight + sizeof(int);
                            off_nubeByteArray    = off_pcLength + sizeof(int);

                            int w                = (int) *off_pcWidth;
                            int h                = (int) *off_pcHeight;
                            int nubeLength       = (int) *off_pcLength;

                            memcpy(&(w),            (off_pcWidth),     sizeof(int));
                            memcpy(&(h),            (off_pcHeight),    sizeof(int));
                            memcpy(&(nubeLength),   (off_pcLength),    sizeof(int));

                            ofLogVerbose() << "[FrameUtils::getFrameByteArray] - nube recuperado - w: " << w;
                            ofLogVerbose() << "[FrameUtils::getFrameByteArray] - nube recuperado - h: " << h;
                            ofLogVerbose() << "[FrameUtils::getFrameByteArray] - nube recuperado - nubeLength: " << nubeLength;

                            tData[i].nubeW  = w;
                            tData[i].nubeH  = h;
                            tData[i].nubeLength = nubeLength;

                            if(nubeLength > 0) {
                                tData[i].xpix   = new float[nubeLength];
                                tData[i].ypix   = new float[nubeLength];
                                tData[i].zpix   = new float[nubeLength];

                                //tData[i].sXpix  = (ofFloatPixels &) off_nubeByteArray;//imgx.getFloatPixelsRef();
                                memcpy((tData[i].xpix),     (off_nubeByteArray),     sizeof(float)*nubeLength);
                                off_nubeByteArray   = off_nubeByteArray + nubeLength*sizeof(float);

                                memcpy((tData[i].ypix),     (off_nubeByteArray),     sizeof(float)*nubeLength);
                                off_nubeByteArray   = off_nubeByteArray + nubeLength*sizeof(float);

                                memcpy((tData[i].zpix),     (off_nubeByteArray),     sizeof(float)*nubeLength);
                                off_nubeByteArray   = off_nubeByteArray + nubeLength*sizeof(float);
                            }

                            ofLogVerbose() << "[FrameUtils::getFrameByteArray] - nubeLength: " << tData[i].nubeLength;
                            //Reservo lugar para la nube.
                            start = off_nubeByteArray;

                        }
                    }
                    cout << endl;
                }
                std::pair <int, ThreadData *> retVal;
                retVal.first    = totCameras;
                retVal.second   = tData;
                return retVal;
            }
        } catch (...) {
            ofLogWarning() << "[FrameUtils::getThreadDataFromByteArray] - Exception occurred.";
        }
        std::pair <int, ThreadData *> retVal;
        retVal.first    = 0;
        retVal.second   = NULL;
        return retVal;
    }

    static char * getFrameByteArray(ThreadData * tData, int totalCams, int size) {
        ofLogVerbose() << " getFrameByteArray tData[0].nubeH " << tData[0].nubeH;
        /*
        Aquí tengo que formar el gran bytearray del frame (el tamaño ya está calculado en size).
        Cada ThreadData lo voy a transformar en un elemento bien controlado en tamaño para poder castearlo sin problema del otro lado.
        Dicho elemento va a estar compuesto de:
        p/c/thread data
         - (int) cliId; // ID de la configuración de Cliente. Puede haber N.
         - (int) camId; // ID de la cámara en la instalación.
         - (int) state  //0-No inited, 1-Only Image, 2-Only Point Cloud, 3-Ambas
         - (timeval) curTime
         - (unsigned char *) imagebytearray
         - ofFloatPixels  sXpix;
         - ofFloatPixels  sYpix;
         - ofFloatPixels  sZpix;
        */
        char * combined = NULL;
        try {
            combined        = (char*) malloc (size);
            memcpy(combined,       &totalCams,     sizeof(int));
            char* start     = combined + sizeof(int);
            char* off_cliId;
            char* off_camId;
            char* off_state;
            char* off_curTime;
            char* off_imgWidth;
            char* off_imgHeight;
            char* off_imagebytearray;
            char* off_pcWidth;
            char* off_pcHeight;
            char* off_pcLength;
            char* off_nubeByteArray;

            int i=0, totSize = 0;
            for(i=0; i<totalCams; i++) {
                off_cliId           = start;
                off_camId           = off_cliId     + sizeof(int);
                off_state           = off_camId     + sizeof(int);
                off_curTime         = off_state     + sizeof(int);

                memcpy(off_cliId,           &tData[i].cliId,     sizeof(int));
                memcpy(off_camId,           &tData[i].camId,     sizeof(int));
                memcpy(off_state,           &tData[i].state,     sizeof(int));
                memcpy(off_curTime,         &tData[i].curTime,   sizeof(timeval));

                ofLogVerbose() << "[FrameUtils::getFrameByteArray] - guardado - cliId: "        << tData[i].cliId;
                ofLogVerbose() << "[FrameUtils::getFrameByteArray] - guardado - camId: "        << tData[i].camId;
                ofLogVerbose() << "[FrameUtils::getFrameByteArray] - guardado - state: "        << tData[i].state;

                start = off_curTime + sizeof(timeval);

                //Si (tData[i].state > 0) = Está inicializado
                if(tData[i].state > 0) {

                    //Si tiene imágen;
                    if((tData[i].state == 1) || (tData[i].state == 3)) {
                        //Reservo lugar para la imágen.
                        off_imgWidth         = start;//off_curTime + sizeof(timeval);;
                        off_imgHeight        = off_imgWidth  + sizeof(int);
                        off_imagebytearray   = off_imgHeight + sizeof(int);

                        int w = tData[i].img.getWidth();
                        int h = tData[i].img.getHeight();

                        ofLogVerbose() << "[FrameUtils::getFrameByteArray] - guardado - w: " << w;
                        ofLogVerbose() << "[FrameUtils::getFrameByteArray] - guardado - h: " << h;

                        memcpy(off_imgWidth,       &w,    sizeof(int));
                        memcpy(off_imgHeight,      &h,    sizeof(int));
                        memcpy(off_imagebytearray, tData[i].img.getPixels(), tData[i].img.getPixelsRef().size());

                        start    = off_imagebytearray + (w*h*3);
                    }

                    off_pcWidth = start; //off_curTime + sizeof(timeval);
                    //Si tiene nube;
                    if((tData[i].state == 2) || (tData[i].state == 3)) {
                        off_pcHeight         = off_pcWidth  + sizeof(int);
                        off_pcLength         = off_pcHeight + sizeof(int);
                        off_nubeByteArray    = off_pcLength + sizeof(int);

                        int w           = tData[i].nubeW;
                        int h           = tData[i].nubeH;
                        int nubeLength  = tData[i].nubeLength;

                        ofLogVerbose() << "[FrameUtils::getFrameByteArray] - nube guardado - w: " << tData[i].nubeW;
                        ofLogVerbose() << "[FrameUtils::getFrameByteArray] - nube guardado - h: " << tData[i].nubeH;
                        ofLogVerbose() << "[FrameUtils::getFrameByteArray] - nube guardado - nubeLength: " << nubeLength;

                        memcpy(off_pcWidth,       &w,           sizeof(int));
                        memcpy(off_pcHeight,      &h,           sizeof(int));
                        memcpy(off_pcLength,      &nubeLength,  sizeof(int));

                        if(nubeLength > 0) {
                            memcpy(off_nubeByteArray,   tData[i].xpix, (sizeof(float) * nubeLength));
                            off_nubeByteArray = off_nubeByteArray + (sizeof(float)) * nubeLength;

                            memcpy(off_nubeByteArray,   tData[i].ypix, (sizeof(float)) * nubeLength);
                            off_nubeByteArray = off_nubeByteArray + (sizeof(float)) * nubeLength;

                            memcpy(off_nubeByteArray,   tData[i].zpix, (sizeof(float)) * nubeLength);
                            off_nubeByteArray = off_nubeByteArray + (sizeof(float)) * nubeLength;
                        }

                        start = off_nubeByteArray;
                    }
                }
            }

        } catch (...) {
            ofLogWarning() << "[FrameUtils::getFrameByteArray] - Exception occurred.";
        }

        return combined;
    }

    static char * addToBytearray(char * receiveBytes, int numBytes, char * currBytearray, int currTotal) {
        try {
            if(numBytes <= 0) return currBytearray;
            char * retBytearray = new char[currTotal + numBytes];
            char * pointer;
            if(currBytearray != NULL) {
                memcpy(retBytearray, currBytearray, currTotal);
                delete currBytearray;
                pointer = retBytearray + currTotal;
                memcpy(pointer, receiveBytes, numBytes);
            } else {
                memcpy(retBytearray, receiveBytes, numBytes);
            }
            return retBytearray;
        } catch (...) {
            ofLogWarning() << "[FrameUtils::addToBytearray] - Exception occurred.";
        }
        return NULL;
    }

};
