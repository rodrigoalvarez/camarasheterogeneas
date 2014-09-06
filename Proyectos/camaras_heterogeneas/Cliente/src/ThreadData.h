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
};
