#include "MeshCollector.h"

// comparison, not case sensitive.
bool compare_nframe (GeneratedResult * first, GeneratedResult * second) {
    return (first->nframe < second->nframe);
}

void MeshCollector::threadedFunction() {
    ShareMesh  = (f_compartirMalla)GetProcAddress(memorySharedLibrary, "ShareMesh");
    shareImage = (f_ShareImage)GetProcAddress(memorySharedLibrary, "ShareImage");

    ofAddListener(ofEvents().update, this, &MeshCollector::processFrame);
}

void MeshCollector::exit() {
    ofRemoveListener(ofEvents().update, this, &MeshCollector::processFrame);
    b_exit  = true;
}

void MeshCollector::processFrame(ofEventArgs &e) {
    //Ver en http://www.cplusplus.com/reference/list/list/sort/
    int i = 0;
    for(i=0; ((i<sys_data->totalFreeCores) && !b_exit); i++) {
        if(threads[i].getState() == GENERATOR_COMPLETE) {
            ofLogVerbose() << "[MeshCollector::processFrame]" << endl;
            bool esta = false;
            for (it=list.begin(); it!=list.end(); ++it) {
                if((threads[i].result->nframe == (*it)->nframe)) esta = true;
            }
            if(!esta) {
                threads[i].setState(GENERATOR_IDLE);
                list.push_back(threads[i].result);
                list.sort(compare_nframe);
            }
        }
    }

    shareNextCompleteFrame();
}

void MeshCollector::shareNextCompleteFrame() {
    if(list.size()>0) {
        GeneratedResult * result = list.front();
        if((currFrame + 1) == result->nframe) {
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

    ThreadData * iter = (ThreadData *) gresult->textures;
    /*
    bool descartado = false;
    while(iter != NULL) {
        descartado = descartado || ((iter->img.getWidth() <= 0) || (iter->img.getHeight() <= 0));
        iter = iter->sig;
    }
    ofLogVerbose() << "[MeshCollector::shareFrame] descartado: " << descartado << endl;
    //if(descartado) return;
    */

    if(gresult->hasDepth) {
        int numFaces = *gresult->numberFaces;
        //if(!descartado) {
            ofLogVerbose() << "[MeshCollector::shareFrame] idMesh " << gresult->idMesh << ", numFaces:" << numFaces << endl;
            for(int i = 0; i<numFaces; i++) {
                ofLogVerbose() << "[MeshCollector::shareFrame]" << ", p1_0: " << gresult->faces[i].p1[0] << ", p1_1: " << gresult->faces[i].p1[1]  << ", p1_2: " << gresult->faces[i].p1[2] << ", p2_0: " << gresult->faces[i].p2[0] << ", p2_1: " << gresult->faces[i].p2[1]  << ", p2_2: " << gresult->faces[i].p2[2] << ", p3_0: " << gresult->faces[i].p3[0] << ", p3_1: " << gresult->faces[i].p3[1]  << ", p3_2: " << gresult->faces[i].p3[2] << endl;
            }
            ShareMesh(gresult->idMesh, numFaces, gresult->faces);
        //}
        delete [] gresult->faces;
        delete gresult->numberFaces;
    }

    if(gresult->hasRGB) {

        iter = (ThreadData *) gresult->textures;
        int i = 0;
        do {
            i++;
            ofBuffer imageBuffer;
            ofSaveImage(iter->img.getPixelsRef(), imageBuffer, OF_IMAGE_FORMAT_JPEG);
            iter->img.saveImage("mcollector_share_img_" + ofToString(i) + ".jpg");
            FIMEMORY* stream        = FreeImage_OpenMemory((unsigned char*) imageBuffer.getBinaryBuffer(), imageBuffer.size());
            FREE_IMAGE_FORMAT fif   = FreeImage_GetFileTypeFromMemory( stream, 0 );
            FIBITMAP *dib(0);
            dib                     = FreeImage_LoadFromMemory(fif, stream);

            pixels      = (unsigned char*)FreeImage_GetBits(dib);
            width       = FreeImage_GetWidth(dib);
            height      = FreeImage_GetHeight(dib);
            ofLogVerbose() << "[MeshCollector::shareFrame] dib width: " << width << ", height: " << height << ", img.width: " << iter->img.getWidth() << ", img.height: " << iter->img.getHeight() << endl;
            idMomento   = iter->cliId;
            idMomento   = idMomento*10 + iter->cameraType;
            idMomento   = idMomento*10 + iter->camId;
            idMomento   = idMomento*10000 + gresult->nframe % 10000;

            shareImage(&idMomento, pixels, &width, &height);
            cout << idMomento << endl;

            FreeImage_CloseMemory(stream);
            imageBuffer.clear();
            free(pixels);
            FreeImage_Unload(dib);

            iter = iter->sig;

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
