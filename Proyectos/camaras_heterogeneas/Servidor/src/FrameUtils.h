#pragma once

#include <stdio.h>
#include <locale.h>
#include <time.h>
#include <string>
#include <iostream>

#include <sstream>


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
        tData[0].timestamp  = ofGetUnixTime();

        tData[0].img.loadImage("foto0.jpg");
        tData[0].state      = 1; //0-No inited, 1-Only Image, 2-Only Point Cloud, 3-Ambas

        tData[1].cliId      = 1; // ID que identificará a esta instalación de cliente en el servidor.
        tData[1].camId      = 3; // ID que identifica esta cámara dentro de la instalación cliente.
        tData[1].timestamp  = ofGetUnixTime();
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
             - (unsigned int) timestamp
             - (int) imgWidth
             - (int) imgHeight
             - (unsigned char *) imagebytearray
             - (int) pcWidth
             - (int) pcHeight
             - (unsigned short *) pointcloudbytearray
            */

        try {
            int i=0;
            int totSize = sizeof(int); // totalCams
            for(i=0; i<totalCams; i++) {
                totSize += sizeof(int);     //(int) cliId
                totSize += sizeof(int);     //(int) camId
                totSize += sizeof(int);     //(int) state
                totSize += sizeof(char)*10; //(unsigned int) length

                //Si (tData[i].state > 0) = Está inicializado
                if(tData[i].state > 0) {
                    //Si tiene imágen;
                    if((tData[i].state == 1) || (tData[i].state == 3)) {
                        totSize += sizeof(int);     //(int) camWidth
                        totSize += sizeof(int);     //(int) camHeight
                        //Reservo lugar para la imágen.
                        totSize += (tData[i].img.getWidth() * tData[i].img.getHeight() * 3);
                    }

                    //Si tiene nube;
                    if((tData[i].state == 2) || (tData[i].state == 3)) {
                        totSize += sizeof(int);     //(int) pcWidth
                        totSize += sizeof(int);     //(int) pcHeight
                        //Reservo lugar para la nube.
                        totSize += (sizeof(unsigned short)) * tData[i].spix.getWidth()*tData[i].spix.getHeight()*3;
                    }
                }
            }
            return totSize;

        } catch (...) {
          cout << "FrameUtils::getFrameSize: Exception occurred.";
        }

        return -1;
    }

    static ThreadData * getThreadDataFromByteArray(char * bytearray) {
        try {
            int totCameras;// = (int) *bytearray;
            memcpy(&(totCameras), (bytearray),     sizeof(int));
            totCameras = 1;
            cout << "FrameUtils::getThreadDataFromByteArray: totCameras" << totCameras;
            cout << endl;
            if(totCameras > 0) {
                ThreadData * tData  = new ThreadData[totCameras];
                int i;
                char * start = bytearray + sizeof(int);
                char* off_cliId;
                char* off_camId;
                char* off_state;
                char* off_tmstmp_length;
                //char* off_cstr;
                char* off_timestamp;
                char* off_imgWidth;
                char* off_imgHeight;
                char* off_imagebytearray;
                char* off_pcWidth;
                char* off_pcHeight;
                char* off_pcByteArray;

                for(i=0; i<totCameras; i++) {
                    /*
                     - (int) cliId; // ID de la configuración de Cliente. Puede haber N.
                     - (int) camId; // ID de la cámara en la instalación.
                     - (int) state  //0-No inited, 1-Only Image, 2-Only Point Cloud, 3-Ambas
                     - (unsigned int) timestamp
                     - (int) imgWidth
                     - (int) imgHeight
                     - (unsigned char *) imagebytearray
                     - (int) pcWidth
                     - (int) pcHeight
                     - (unsigned short *) pointcloudbytearray
                    */
                    off_cliId           = start;
                    off_camId           = off_cliId + sizeof(int);
                    off_state           = off_camId + sizeof(int);
                    off_timestamp       = off_state + sizeof(int);

                    memcpy(&(tData[i].cliId), (off_cliId),     sizeof(int));
                    memcpy(&(tData[i].camId), (off_camId),     sizeof(int));
                    memcpy(&(tData[i].state), (off_state),     sizeof(int));

                    //Recuperando timestamp
                    //int tmstmp_length   = (int) * off_tmstmp_length;
                    char cstr[11];
                    char * pointer = off_timestamp;
                    for(int j=0; j<10; j++) {
                        cstr[j] = *pointer;
                        pointer += sizeof(char);
                    }
                    cstr[10] = '\0';
                    //fin: Recuperando timestamp

                    tData[i].timestamp  = (unsigned int) atoi(cstr);

                    cout << endl << "recuperado - cliId: " << tData[i].cliId << endl;
                    cout << "recuperado - camId: " << tData[i].camId << endl;
                    cout << "recuperado - state: " << tData[i].state << endl;
                    cout << "recuperado - timestamp: " << tData[i].timestamp << endl;
                    cout << "recuperado - timestampStr: " << asctime(gmtime(&tData[i].timestamp)) << endl;
                    start = pointer;

                    //Si (tData[i].state > 0) = Está inicializado
                    if(tData[i].state > 0) {
                        off_pcWidth = pointer;
                        //Si tiene imágen;
                        if((tData[i].state == 1) || (tData[i].state == 3)) {
                           //Reservo lugar para la imágen.
                           off_imgWidth         = pointer;
                           off_imgHeight        = off_imgWidth  + sizeof(int);
                           off_imagebytearray   = off_imgHeight + sizeof(int);

                           int w;
                           int h;

                           memcpy(&(w),     (off_imgWidth),     sizeof(int));
                           memcpy(&(h),     (off_imgHeight),    sizeof(int));

                           cout << "recuperado - w: " << w << endl;
                           cout << "recuperado - h: " << h << endl;

                            ofImage imgDest3;
                            imgDest3.setFromPixels((unsigned char *) off_imagebytearray, w, h, OF_IMAGE_COLOR, true);
                            imgDest3.saveImage("imgDest3.jpg");
                           /*
                           unsigned char * pixels = new unsigned char[w*h*3];
                           cout << endl << " FIN -1 " << endl;
                           memcpy(&(pixels), (off_imagebytearray), w * h * 3);
                           */

                           cout << endl << " FIN " << endl;

                           /*
                           tData[i].img.setFromPixels( pixels, w, h, OF_IMAGE_COLOR, true ); //Clono la imágen

                           int  mult = sizeof(unsigned char) * tData[i].img.getWidth() * tData[i].img.getHeight();
                           off_pcWidth = off_imagebytearray + mult;
                           */
                           start = off_imagebytearray + w * h * 3;
                        }

                        //Si tiene nube;
                        if((tData[i].state == 2) || (tData[i].state == 3)) {
                            /*
                            off_pcHeight         = off_pcWidth  + sizeof(int);
                            off_pcByteArray      = off_pcHeight + sizeof(int);

                            int w                = (int) *off_pcWidth;
                            int h                = (int) *off_pcHeight;

                            tData[i].spix.setFromPixels((unsigned short *) off_pcByteArray, w, h, OF_IMAGE_COLOR);

                            //Reservo lugar para la nube.
                            start = off_pcByteArray + (sizeof(unsigned short)) * tData[i].spix.getWidth()*tData[i].spix.getHeight();
                            */
                        }
                    }
                    cout << endl;
                }
                return tData;
            }
        } catch (...) {
          cout << "FrameUtils::getThreadDataFromByteArray: Exception occurred.";
        }
        return NULL;
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
         - (unsigned int) timestamp
         - (unsigned char *) imagebytearray
         - (unsigned char *) pointcloudbytearray
        */
        char * combined = NULL;
        try {
            combined        = (char*) malloc (size);
            memcpy(combined,       &totalCams,     sizeof(int));
            char* start     = combined + sizeof(int);
            char* off_cliId;
            char* off_camId;
            char* off_state;
            char* off_tmstmp_length;
            char* off_timestamp;
            char* off_imgWidth;
            char* off_imgHeight;
            char* off_imagebytearray;
            char* off_pcWidth;
            char* off_pcHeight;
            char* off_pcByteArray;

            int i=0, totSize = 0;
            for(i=0; i<totalCams; i++) {
                off_cliId           = start;
                off_camId           = off_cliId + sizeof(int);
                off_state           = off_camId + sizeof(int);
                off_timestamp       = off_state + sizeof(int);// off_tmstmp_length + sizeof(int);

                stringstream stream;
                stream << tData[i].timestamp;
                string tmstmp       = stream.str();
                const char * cstr   = tmstmp.c_str();

                memcpy(off_cliId,           &tData[i].cliId,     sizeof(int));
                memcpy(off_camId,           &tData[i].camId,     sizeof(int));
                memcpy(off_state,           &tData[i].state,     sizeof(int));

                char * pointer      = off_timestamp;
                for(int j=0; j<10; j++) { //caracteres timestamp
                    memcpy(pointer, &cstr[j], sizeof(char));
                    pointer += sizeof(char);
                }

                cout << "guardado - cliId: "        << tData[i].cliId   << endl;
                cout << "guardado - camId: "        << tData[i].camId   << endl;
                cout << "guardado - state: "        << tData[i].state   << endl;
                cout << "guardado - timestamp: "    << cstr             << endl;

                start = pointer;

                //Si (tData[i].state > 0) = Está inicializado
                if(tData[i].state > 0) {
                    off_pcWidth = pointer;
                    //Si tiene imágen;
                    if((tData[i].state == 1) || (tData[i].state == 3)) {
                       //Reservo lugar para la imágen.
                       off_imgWidth         = pointer;
                       off_imgHeight        = off_imgWidth  + sizeof(int);
                       off_imagebytearray   = off_imgHeight + sizeof(int);

                       int w = tData[i].img.getWidth();
                       int h = tData[i].img.getHeight();

                       cout << "guardado - w: " << w << endl;
                       cout << "guardado - h: " << h << endl;

                       memcpy(off_imgWidth,       &w,    sizeof(int));
                       memcpy(off_imgHeight,      &h,    sizeof(int));
                       memcpy(off_imagebytearray, tData[i].img.getPixels(), w*h*3);

                       start    = off_imagebytearray + (w*h*3);
                    }

                    //Si tiene nube;
                    if((tData[i].state == 2) || (tData[i].state == 3)) {
                       /*
                       off_pcHeight         = off_pcWidth  + sizeof(int);
                       off_pcByteArray      = off_pcHeight + sizeof(int);
                       //Reservo lugar para la nube.

                       int w = tData[i].spix.getWidth();
                       int h = tData[i].spix.getHeight();

                       memcpy(off_pcWidth,       &w,    sizeof(int));
                       memcpy(off_pcHeight,      &h,    sizeof(int));
                       memcpy(off_pcByteArray,   (unsigned short *)tData[i].spix.getPixels(), (sizeof(unsigned short)) * w * h);
                       start = off_pcByteArray + (sizeof(unsigned short)) * w * h;
                       */
                    }
                }
            }

        } catch (...) {
          cout << "FrameUtils::getFrameByteArray: Exception occurred.";
        }

        return combined;
    }

};
