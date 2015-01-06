#include "MeshCollector.h"

// comparison, not case sensitive.
bool compare_nframe (GeneratedResult * first, GeneratedResult * second) {
    return (first->nframe < second->nframe);
}

void MeshCollector::threadedFunction() {
    ShareMesh  = (f_compartirMalla)GetProcAddress(memorySharedLibrary, "ShareMesh");
    /*
    while(isThreadRunning()) {
        //ofSleepMillis(1000/sys_data->fps);
        processFrame();
    }*/

    ofAddListener(ofEvents().update, this, &MeshCollector::processFrame);
}

void MeshCollector::processFrame(ofEventArgs &e) {
    //Ver en http://www.cplusplus.com/reference/list/list/sort/
    int i = 0;
    for(i=0; i<sys_data->totalFreeCores; i++) {
        if((threads[i].getState() == GENERATOR_COMPLETE) /*&& (threads[i].nframe != -1)*/) {

            bool esta = false;
            for (it=list.begin(); it!=list.end(); ++it) {
                if(threads[i].result->nframe == (*it)->nframe) esta = true;
            }
            if(!esta) {
                threads[i].setState(GENERATOR_IDLE);
                //threads[i].nframe = -1;
                list.push_back(threads[i].result);
                list.sort(compare_nframe);
            }
        }
    }

    //for (it=list.begin(); it!=list.end(); ++it)
    //    ofLogVerbose() << "MeshCollector :: Ordered nframe " << (*it)->nframe << endl;
    shareNextCompleteFrame();
}

int sss = 0;
void MeshCollector::shareNextCompleteFrame() {
    sss ++;
    if(list.size()>0) {
        GeneratedResult * result = list.front();
        if((currFrame + 1) == result->nframe) {
            ofLogVerbose() << "SHARE FRAME " << (currFrame + 1) << "  " << sss << endl;
            currFrame++;
            list.remove(result);
            if(result->hasDepth) {
                delete [] result->faces;
                delete result->numberFaces;
                /*
                delete result->nube->x;
                delete result->nube->y;
                delete result->nube->z;
                */
                delete result->nube;
            }
            //return;
            shareFrame(result);
        }
    }
}

void MeshCollector::shareFrame(GeneratedResult * gresult) {
    int idMomento;
    unsigned char* pixels;
    int width;
    int height;

    if(gresult->hasDepth) {
        ofLogVerbose() << "POR SHEREAR LA MAYA GENERADA " << (currFrame) << endl;
        ofLogVerbose() << "DATOS MAYA GENERADA, idMesh: " << (gresult->idMesh) << ", idnumberFaces: " << (*gresult->numberFaces) << ", faces: " << (gresult->faces) << endl;
        ShareMesh(gresult->idMesh, *gresult->numberFaces, gresult->faces);
        delete [] gresult->faces;
        delete gresult->numberFaces;
    }

    if(gresult->hasRGB) { // En first viene un array de ThreadData con las texturas.
        ThreadData * iter = (ThreadData *) gresult->textures;
        f_ShareImage shareImage = (f_ShareImage)GetProcAddress(memorySharedLibrary, "ShareImage");
        do {
            ofBuffer imageBuffer;
            ofSaveImage(iter->img.getPixelsRef(), imageBuffer, OF_IMAGE_FORMAT_JPEG);

            FIMEMORY* stream        = FreeImage_OpenMemory((unsigned char*) imageBuffer.getBinaryBuffer(), imageBuffer.size());
            FREE_IMAGE_FORMAT fif   = FreeImage_GetFileTypeFromMemory( stream, 0 );
            FIBITMAP *dib(0);
            dib                     = FreeImage_LoadFromMemory(fif, stream);

            pixels      = (unsigned char*)FreeImage_GetBits(dib);
            width       = FreeImage_GetWidth(dib);
            height      = FreeImage_GetHeight(dib);

            idMomento   = iter->cliId;
            idMomento   = idMomento*10 + iter->cameraType;
            idMomento   = idMomento*10 + iter->camId;
            idMomento   = idMomento*10000 + gresult->nframe % 10000;

            shareImage(&idMomento, pixels, &width, &height);
            iter = iter->sig;
        } while(iter != NULL);
    }
}
