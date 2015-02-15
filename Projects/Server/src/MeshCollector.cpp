#include "MeshCollector.h"

// comparison, not case sensitive.
bool compare_nframe (GeneratedResult * first, GeneratedResult * second) {
    return (first->nframe < second->nframe);
}

void MeshCollector::threadedFunction() {
    ShareMesh  = (f_compartirMalla)GetProcAddress(memorySharedLibrary, "ShareMesh");
    shareImage = (f_ShareImage)GetProcAddress(memorySharedLibrary, "ShareImage");
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
                if((threads[i].result->nframe == (*it)->nframe)) esta = true;
            }
            if(!esta) {
                threads[i].setState(GENERATOR_IDLE);

                list.push_back(threads[i].result);
                list.sort(compare_nframe);

                /*
                GeneratedResult * result = threads[i].result;
                delete [] result->faces;
                delete result->numberFaces;
                if(result->textures != NULL) {
                    while(result->textures != NULL){
                        ThreadData * curr = result->textures;
                        result->textures = result->textures->sig;
                        delete curr;
                    }
                    result->textures = NULL;
                }
                */
            }
        }
    }

    shareNextCompleteFrame();
}

void MeshCollector::shareNextCompleteFrame() {
    if(list.size()>0) {
        GeneratedResult * result = list.front();
        if((currFrame + 1) == result->nframe) {
            ofLogVerbose() << "SHARE FRAME " << (currFrame + 1) << endl;
            currFrame++;
            list.remove(result);

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
        int numFaces = *gresult->numberFaces;
        ShareMesh(gresult->idMesh, numFaces, gresult->faces);
        delete [] gresult->faces;
        delete gresult->numberFaces;
    }

    if(gresult->hasRGB) {
        ThreadData * iter = (ThreadData *) gresult->textures;

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

            FreeImage_CloseMemory(stream);
            imageBuffer.clear();
            free(pixels);
            FreeImage_Unload(dib);
        } while(iter != NULL);
    }

    if(gresult->hasRGB) {
        if(gresult->textures != NULL) {
            while(gresult->textures != NULL){
                ThreadData * curr = gresult->textures;
                gresult->textures = gresult->textures->sig;
                delete curr;
            }
            gresult->textures = NULL;
        }
    }
    delete gresult;
}
