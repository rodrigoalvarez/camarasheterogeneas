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

        ofVec3f xyz;
        ofVec3f abc;

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
            if(td->nubeLength > 0) {
                ofLogVerbose() << "[ThreadData::mergePointClouds] - Mergeando 2 Frames - " << nubeLength << " - " << td->nubeLength;

                int curLength   = nubeLength;

                char * tmpzPix = (char*) malloc ( sizeof(float) * (curLength + td->nubeLength) );
                char * tmpyPix = (char*) malloc ( sizeof(float) * (curLength + td->nubeLength) );
                char * tmpxPix = (char*) malloc ( sizeof(float) * (curLength + td->nubeLength) );
                char * offx    = tmpxPix + curLength * sizeof(float);
                char * offy    = tmpyPix + curLength * sizeof(float);
                char * offz    = tmpzPix + curLength * sizeof(float);

                memcpy(tmpxPix,  xpix,      sizeof(float) * curLength);
                memcpy(offx,     td->xpix,  sizeof(float) * td->nubeLength);

                memcpy(tmpyPix,  ypix,      sizeof(float) * curLength);
                memcpy(offy,     td->ypix,  sizeof(float) * td->nubeLength);

                memcpy(tmpzPix,  zpix,      sizeof(float) * curLength);
                memcpy(offz,     td->zpix,  sizeof(float) * td->nubeLength);

                int w;
//                for(w=0; w<nubeLength; w++) {
//                    cout << xpix[w] << " " << ypix[w] << " " << zpix[w] << endl;
//                }
//                for(w=0; w<td->nubeLength; w++) {
//                    cout << td->xpix[w] << " " << td->ypix[w] << " " << td->zpix[w] << endl;
//                }

                free(xpix);
                free(ypix);
                free(zpix);

                xpix = (float *) tmpxPix;
                ypix = (float *) tmpyPix;
                zpix = (float *) tmpzPix;

                nubeLength = curLength + td->nubeLength;

//                for(w=0; w<nubeLength; w++) {
//                    //cout << xpix[w] << " " << ypix[w] << " " /*<< zpix[w] */<< endl;
//                    cout << xpix[w] << " " << ypix[w] << " " << zpix[w]<< endl;
//                }
            }
        }
};
