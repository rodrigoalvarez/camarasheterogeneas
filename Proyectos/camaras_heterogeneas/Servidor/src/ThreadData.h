#pragma once
#include <sys/time.h>

//template <time_t, int> struct fm_key;

class ThreadData {
	public:
        std::pair <time_t, int> key;
        //fm_key<time_t, int> * key;
        timeval curTime;
        //time_t  timestamp;
        char *  timestampStr;
        int     cliId;   // ID de la configuración de Cliente. Puede haber N
        int     camId;   // ID de la cámara en la instalación.
        ofImage img;
        int     state; //0-No inited, 1-Only Image, 2-Only Point Cloud, 3-Ambas
        //void    updateData();
        ofShortPixels  spix;
        int nubeLength;
        int nubeW;
        int nubeH;
        float *         xpix;
        float *         ypix;
        float *         zpix;

        ofFloatPixels  sXpix;
        ofFloatPixels  sYpix;
        ofFloatPixels  sZpix;

        ThreadData() {
            xpix = NULL;
            ypix = NULL;
            zpix = NULL;
        }

        ~ThreadData() {
            ofLogVerbose() << "[ThreadData::~ThreadData]";
        }

        void mergePointClouds(ThreadData * td) {
            if(td->nubeLength <= 0) {
                int curLength   = nubeLength;

                float * tmpxPix = new float[curLength + td->nubeLength];
                float * tmpyPix = new float[curLength + td->nubeLength];
                float * tmpzPix = new float[curLength + td->nubeLength];

                memcpy(tmpxPix,     &xpix,     curLength);
                memcpy(tmpxPix + sizeof(float) * curLength,  &td->xpix,  td->nubeLength);

                memcpy(tmpyPix,     &ypix,     curLength);
                memcpy(tmpyPix + sizeof(float) * curLength,  &td->ypix,  td->nubeLength);

                memcpy(tmpzPix,     &zpix,     curLength);
                memcpy(tmpzPix + sizeof(float) * curLength,  &td->zpix,  td->nubeLength);

                delete xpix;
                delete ypix;
                delete zpix;

                xpix = tmpxPix;
                ypix = tmpyPix;
                zpix = tmpzPix;

                nubeLength += td->nubeLength;
            }
        }
};
