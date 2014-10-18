#pragma once

#include <stdio.h>
#include <locale.h>
#include <time.h>
#include <string>
#include <iostream>
#include <sys/time.h>
#include <sstream>
#include "ofxCvShortImage.h"

#include <windows.h>


typedef void (*f_compress_img) (void ** srcBuff, int width, int height, void ** destBuff, int * comp_size);
typedef void (*f_decompress_img) (void * srcBuff, int comp_size, int* width, int* height, void ** destBuff);

//f_compress_img    compress_img;
//f_decompress_img  decompress_img;



class FrameUtils {

	public:

    //static f_compress_img    compress_img;
    //static f_decompress_img  decompress_img;

    static void init() {
//        HINSTANCE hGetProcIDDLL;
//        hGetProcIDDLL =  LoadLibraryA("imageCompression.dll");
//
//        if (!hGetProcIDDLL) {
//            std::cout << "No se pudo cargar la libreria: " << std::endl;
//        } else {
//            //FrameUtils::compress_img = (f_compress_img)   GetProcAddress(hGetProcIDDLL, "compress_img");
//            compress_img    = (f_compress_img)   GetProcAddress(hGetProcIDDLL, "compress_img");
//            decompress_img  = (f_decompress_img) GetProcAddress(hGetProcIDDLL, "decompress_img");
//        }
    }

    static ThreadData * getDummyFrame() {

        //time( &aclock );                 /* Get time in seconds */
        //newtime = localtime( &aclock );  /* Convert time to struct */
        /* Print local time as a string */
        //printf( "The current date and time are: %s", asctime( newtime ) );

        ThreadData * tData  = new ThreadData[2];
        tData[0].cliId      = 519; // ID que identificará a esta instalación de cliente en el servidor.
        tData[0].camId      = 2; // ID que identifica esta cámara dentro de la instalación cliente.

        gettimeofday(&tData[0].curTime, NULL);

        tData[0].img.loadImage("foto0.png");

        ofxCvShortImage img;
        img.allocate(100, 100);
        tData[0].spix       = img.getShortPixelsRef();
        tData[0].nubeW      = img.getWidth();
        tData[0].nubeH      = img.getHeight();
        tData[0].nubeLength = img.getWidth() * img.getHeight();
        tData[0].xpix       = (float*) malloc (sizeof(float) *tData[0].nubeLength);
        tData[0].ypix       = (float*) malloc (sizeof(float) *tData[0].nubeLength);
        tData[0].zpix       = (float*) malloc (sizeof(float) *tData[0].nubeLength);
        tData[0].state      = 3; //0-No inited, 1-Only Image, 2-Only Point Cloud, 3-Ambas
        tData[0].abc.set(1, 1, 1);
        tData[0].xyz.set(1, 1, 1);

        tData[1].cliId      = 1; // ID que identificará a esta instalación de cliente en el servidor.
        tData[1].camId      = 3; // ID que identifica esta cámara dentro de la instalación cliente.
        gettimeofday(&tData[1].curTime, NULL);
        //tData[1].timestamp  = ofGetUnixTime();
        ofxCvShortImage img2;
        img2.allocate(100, 200);

        tData[1].img.loadImage("foto1.png");

        tData[1].spix       = img2.getShortPixelsRef();
        tData[1].nubeW      = img2.getWidth();
        tData[1].nubeH      = img2.getHeight();
        tData[1].nubeLength = img2.getWidth() * img2.getHeight();
        tData[1].xpix       = (float*) malloc (sizeof(float) *tData[1].nubeLength);
        tData[1].ypix       = (float*) malloc (sizeof(float) *tData[1].nubeLength);
        tData[1].zpix       = (float*) malloc (sizeof(float) *tData[1].nubeLength);
        tData[1].state      = 3; //0-No inited, 1-Only Image, 2-Only Point Cloud, 3-Ambas
        tData[1].abc.set(1, 1, 1);
        tData[1].xyz.set(1, 1, 1);

        int w;
        for(w=0; w<tData[1].nubeLength; w++) {
            tData[1].xpix[w] = 0.0f;
            tData[1].ypix[w] = 0.0f;
            tData[1].zpix[w] = 0.0f;
            //cout << tData[1].xpix[w] << " " << tData[1].ypix[w] << " " << tData[1].zpix[w] << endl;
        }
        for(w=0; w<tData[0].nubeLength; w++) {
            tData[0].xpix[w] = 1.0f;
            tData[0].ypix[w] = 1.0f;
            tData[0].zpix[w] = 1.0f;
            //cout << tData[0].xpix[w] << " " << tData[0].ypix[w] << " " << tData[0].zpix[w] << endl;
        }

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
                        totSize += sizeof(int);     //(int) camBArrSize
                        //totSize += sizeof(int);     //(int) camHeight
                        totSize += sizeof(float)*6; //(int) transformación matriz
                        //Reservo lugar para la imágen.

                        //ofPixels p = tData[i].img.getPixelsRef();
                        //totSize += p.size();
                        ofLogVerbose() << "[FrameUtils::getFrameSize] sumando el size " << tData[i].compSize << endl;
                        totSize += tData[i].compSize;
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
                char* off_imgBArrSize;
                char* off_imgWidth;
                char* off_imgHeight;
                char* off_imgXYZ;
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

                    ofLogVerbose() << "[FrameUtils::getThreadDataFromByteArray] - recuperado - cliId: " << tData[i].cliId << " - camId: " << tData[i].camId << " - state: " << tData[i].state ;

                    start = off_curTime   + sizeof(timeval);

                    //Si (tData[i].state > 0) = Está inicializado
                    if(tData[i].state > 0) {
                        //Si tiene imágen;
                        if((tData[i].state == 1) || (tData[i].state == 3)) {

                            //Reservo lugar para la imágen.
                            off_imgBArrSize      = start;
                            //off_imgWidth         = start;
                            //off_imgHeight        = off_imgWidth  + sizeof(int);
                            off_imgXYZ           = off_imgBArrSize + sizeof(int);
                            off_imagebytearray   = off_imgXYZ    + sizeof(float)*6;

                            int w;
                            int h;

                            memcpy(&(tData[i].compSize),     (off_imgBArrSize),     sizeof(int));

                            ofLogVerbose() << "[FrameUtils::getThreadDataFromByteArray] - Recibida imagen de size: " << tData[i].compSize;

                            memcpy(&(tData[i].xyz.x),   (off_imgXYZ),                       sizeof(float));
                            memcpy(&(tData[i].xyz.y),   (off_imgXYZ + sizeof(float)),       sizeof(float));
                            memcpy(&(tData[i].xyz.z),   (off_imgXYZ + sizeof(float) * 2),   sizeof(float));

                            memcpy(&(tData[i].abc.x),   (off_imgXYZ + sizeof(float) * 3),   sizeof(float));
                            memcpy(&(tData[i].abc.y),   (off_imgXYZ + sizeof(float) * 4),   sizeof(float));
                            memcpy(&(tData[i].abc.z),   (off_imgXYZ + sizeof(float) * 5),   sizeof(float));

                            ofLogVerbose() << "[FrameUtils::getThreadDataFromByteArray] - xyz.x: " << tData[i].xyz.x;
                            ofLogVerbose() << "[FrameUtils::getThreadDataFromByteArray] - xyz.y: " << tData[i].xyz.y;
                            ofLogVerbose() << "[FrameUtils::getThreadDataFromByteArray] - xyz.z: " << tData[i].xyz.z;

                            ofLogVerbose() << "[FrameUtils::getThreadDataFromByteArray] - abc.x: " << tData[i].abc.x;
                            ofLogVerbose() << "[FrameUtils::getThreadDataFromByteArray] - abc.y: " << tData[i].abc.y;
                            ofLogVerbose() << "[FrameUtils::getThreadDataFromByteArray] - abc.z: " << tData[i].abc.z;

                            tData[i].compImg    = new char[tData[i].compSize];
                            memcpy((tData[i].compImg), ((unsigned char *) off_imagebytearray), tData[i].compSize);

                            /*
                            ofImage imgDest3;
                            //imgDest3.loadImage("foto0.jpg");
                            try {
                                imgDest3.setFromPixels((unsigned char *) off_imagebytearray, w, h, OF_IMAGE_COLOR, true);
                            }catch(std::bad_alloc& ba) {}
                            */

                            //char dig = (char)(((int)'0')+i);
                            //tData[i].img.setFromPixels((unsigned char *) off_imagebytearray, w, h, OF_IMAGE_COLOR, true);
                            //imgDest3.saveImage("imgDest" + ofToString(i) + ".jpg");

                            //start = off_imagebytearray + w * h * 3;
                            start    = off_imagebytearray + tData[i].compSize;
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

                            ofLogVerbose() << "Recibida nube de: " << w << "x" << h << " - total puntos: " << nubeLength;

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

                            //Reservo lugar para la nube.
                            start = off_nubeByteArray;

                        }
                    }
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
            char* off_imgBArrSize;
            //char* off_imgWidth;
            //char* off_imgHeight;
            char* off_imgXYZ;
            char* off_imagebytearray;
            char* off_pcWidth;
            char* off_pcHeight;
            char* off_pcLength;
            char* off_nubeByteArray;

            int i=0, totSize = 0;
            for(i=0; i<totalCams; i++) {
                ofLogVerbose() << "off_imagebytearray size " << tData[i].compSize << endl;

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
                        off_imgBArrSize      = start;
                        //off_imgWidth         = start;//off_curTime + sizeof(timeval);;
                        //off_imgHeight        = off_imgWidth  + sizeof(int);
                        off_imgXYZ           = off_imgBArrSize + sizeof(int);
                        off_imagebytearray   = off_imgXYZ    + sizeof(float)*6;

                        int w = tData[i].img.getWidth();
                        int h = tData[i].img.getHeight();

                        ofLogVerbose() << "[FrameUtils::getFrameByteArray] - guardado - w: " << w;
                        ofLogVerbose() << "[FrameUtils::getFrameByteArray] - guardado - h: " << h;

                        memcpy(off_imgBArrSize,    &tData[i].compSize,    sizeof(int));
                        //memcpy(off_imgWidth,       &w,    sizeof(int));
                        //memcpy(off_imgHeight,      &h,    sizeof(int));
                        ofLogVerbose() << "[FrameUtils::getFrameByteArray] - Recibida imagen de size: " << tData[i].compSize;

                        memcpy(off_imgXYZ,                   &tData[i].xyz.x,    sizeof(float));
                        memcpy(off_imgXYZ + sizeof(float),   &tData[i].xyz.y,    sizeof(float));
                        memcpy(off_imgXYZ + sizeof(float)*2, &tData[i].xyz.z,    sizeof(float));
                        memcpy(off_imgXYZ + sizeof(float)*3, &tData[i].abc.x,    sizeof(float));
                        memcpy(off_imgXYZ + sizeof(float)*4, &tData[i].abc.y,    sizeof(float));
                        memcpy(off_imgXYZ + sizeof(float)*5, &tData[i].abc.z,    sizeof(float));

                        ofLogVerbose() << "[FrameUtils::getFrameByteArray] - xyz.x: " << tData[i].xyz.x;
                        ofLogVerbose() << "[FrameUtils::getFrameByteArray] - xyz.y: " << tData[i].xyz.y;
                        ofLogVerbose() << "[FrameUtils::getFrameByteArray] - xyz.z: " << tData[i].xyz.z;

                        ofLogVerbose() << "[FrameUtils::getFrameByteArray] - abc.x: " << tData[i].abc.x;
                        ofLogVerbose() << "[FrameUtils::getFrameByteArray] - abc.y: " << tData[i].abc.y;
                        ofLogVerbose() << "[FrameUtils::getFrameByteArray] - abc.z: " << tData[i].abc.z;

                        //&tData[i].compImg, &tData[i].compSize
                        memcpy(off_imagebytearray, tData[i].compImg, tData[i].compSize);

                        /*
                        totSize += sizeof(int);     //(int) camBArrSize
                        totSize += sizeof(float)*6; //(int) transformación matriz
                        //Reservo lugar para la imágen.

                        ofLogVerbose() << "[FrameUtils::getFrameSize] sumando el size " << tData[i].compSize << endl;
                        totSize += tData[i].compSize;
                        */
                        //

                        start    = off_imagebytearray + tData[i].compSize;
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

    /**
    * Recorre los totalCams ThreadData y comprime todas las imágenes.
    */
    static void compressImages(ThreadData * tData, int totalCams) {
        HINSTANCE hGetProcIDDLL;
        hGetProcIDDLL                    =  LoadLibraryA("imageCompression.dll");
        f_compress_img compress_img      = (f_compress_img)   GetProcAddress(hGetProcIDDLL, "compress_img");
        //f_decompress_img decompress_img  = (f_decompress_img) GetProcAddress(hGetProcIDDLL, "decompress_img");

        try {
            void * srcBuff;
            int i;
            for(i=0; i<totalCams; i++) {
                if(tData[i].state > 0) {
                    if((tData[i].state == 1) || (tData[i].state == 3)) {
                        srcBuff           = (void *) tData[i].img.getPixels();

                        if (!hGetProcIDDLL) {
                            std::cout << "No se pudo cargar la libreria: " << std::endl;
                        } else {

                            compress_img(&srcBuff, tData[i].img.getWidth(), tData[i].img.getHeight(), &tData[i].compImg, &tData[i].compSize);

                            /*if(tData[i].compSize == -1) {
                                std::cout << "Error al comprimir la imagen. " << std::endl;
                            } else {
                                int unc_width   = 0;
                                int unc_height  = 0;

                                const unsigned char * unc_Buff = NULL;
                                decompress_img(tData[i].compImg, tData[i].compSize, &unc_width, &unc_height, (void **)&unc_Buff);
                            }*/
                        }
                        ofLogWarning() << "[FrameUtils::compressImages] - tData[i].compSize " << tData[i].compSize;
                    }
                    if((tData[i].state == 2) || (tData[i].state == 3)) {
                        //@TODO: Acá falta integrar la compresión de la nube.
                    }
                }
            }
        } catch (...) {
            ofLogWarning() << "[FrameUtils::compressImages] - Exception occurred.";
        }
    }

    static void compressSample() {

        HINSTANCE hGetProcIDDLL;

        hGetProcIDDLL =  LoadLibraryA("imageCompression.dll");

        if (!hGetProcIDDLL) {
            std::cout << "No se pudo cargar la libreria: " << std::endl;
        } else {
            f_compress_img      compress_img    = (f_compress_img)   GetProcAddress(hGetProcIDDLL, "compress_img");
            f_decompress_img    decompress_img  = (f_decompress_img) GetProcAddress(hGetProcIDDLL, "decompress_img");

            ofImage img;
            img.loadImage("img1.jpg");

            void * srcBuff           = (void *) img.getPixels();
            void * destBuff          = NULL;
            int width                = img.getWidth();
            int height               = img.getHeight();
            int comp_size;

            compress_img(&srcBuff, width, height, &destBuff, &comp_size);

            if(comp_size == -1) {
                std::cout << "Error al comprimir la imagen. " << std::endl;
            } else {
                int unc_width   = 0;
                int unc_height  = 0;

                const unsigned char * unc_Buff = NULL;
                decompress_img(destBuff, comp_size, &unc_width, &unc_height, (void **)&unc_Buff);

                ofImage img2;
                img2.setFromPixels(unc_Buff, unc_width, unc_height, OF_IMAGE_COLOR, true);
                img2.saveImage("salida_decompress_img.jpg");

            }
        }
    }

    /**
    * Recorre los totalCams ThreadData y descomprime todas las imágenes.
    */
    static void decompressImages(ThreadData * tData, int totalCams) {
        HINSTANCE hGetProcIDDLL;
        //HINSTANCE hGetProcPCIDDLL;

        hGetProcIDDLL                    =  LoadLibraryA("imageCompression.dll");
        //hGetProcPCIDDLL                     =  LoadLibraryA("pointCloudCompression.dll");

        f_decompress_img    decompress_img  = (f_decompress_img) GetProcAddress(hGetProcIDDLL,      "decompress_img");
        //f_decompress_pc     decompress_pc   = (f_decompress_img) GetProcAddress(hGetProcPCIDDLL,    "decompress_pc");

        try {
            int i;
            for(i=0; i<totalCams; i++) {
                if(tData[i].state > 0) {
                    if((tData[i].state == 1) || (tData[i].state == 3)) {
                        if(tData[i].compSize == -1) {
                            std::cout << "Error al comprimir la imagen. " << std::endl;
                        } else {
                            int unc_width   = 0;
                            int unc_height  = 0;

                            const unsigned char * unc_Buff = NULL;
                            decompress_img(tData[i].compImg, tData[i].compSize, &unc_width, &unc_height, (void **)&unc_Buff);
                            tData[i].img.setFromPixels(unc_Buff, unc_width, unc_height, OF_IMAGE_COLOR, true);
                            tData[i].img.saveImage("decompress_debug.jpg");
                        }
                    }
                    if((tData[i].state == 2) || (tData[i].state == 3)) {
                        //@TODO: Acá falta integrar la compresion de la nube.
//                        if(tData[i].pcCompSize == -1) {
//                            std::cout << "Error al comprimir la nube. " << std::endl;
//                        } else {
//                            int unc_width   = 0;
//                            int unc_height  = 0;
//
//                            const unsigned char * unc_Buff = NULL;
//                            decompress_img(tData[i].compImg, tData[i].compSize, &unc_width, &unc_height, (void **)&unc_Buff);
//                            tData[i].img.setFromPixels(unc_Buff, unc_width, unc_height, OF_IMAGE_COLOR, true);
//                            tData[i].img.saveImage("decompress_debug.jpg");
//                        }
                    }
                }
            }
        } catch (...) {
            ofLogWarning() << "[FrameUtils::decompressImages] - Exception occurred.";
        }
    }

    /**
    * Retorna un pair con <bytearray, size>
    */
    static std::pair <char *, int> compressData(char * data, int size) {

    }

    /**
    * Retorna un pair con <bytearray, size>
    */
    static std::pair <char *, int> decompressData(char * data, int size) {

    }
};
