#include "MeshThreadedGenerator.h"


//typedef void (*f_compartirMalla)(int idMesh, int numberFaces, FaceStruct* faces);
//typedef void (*f_ShareImage)(int* idImage, unsigned char* pixels, int* wPixels, int* hPixels);

void MeshThreadedGenerator::threadedFunction() {
    state   = GENERATOR_IDLE;
    ofLogVerbose() << "MeshThreadedGenerator :: threadedFunction" << endl;
    generarMalla = (f_generarMalla)GetProcAddress(generateMeshLibrary, "generarMalla");
    /*
    while(isThreadRunning()) {
        //ofSleepMillis(1000/sys_data->fps);
        processFrame();
    }
    */

    ofAddListener(ofEvents().update, this, &MeshThreadedGenerator::processFrame);
}

void MeshThreadedGenerator::processFrame(ofEventArgs &e) {
    lock();
    if(state == GENERATOR_LOADED) {

        int idMesh;
        NubePuntos* nbIN = NULL;

        ofLogVerbose() << "MeshThreadedGenerator :: state == GENERATOR_LOADED " << nMTG << endl;
        state           = GENERATOR_BUSY;

        if((frame.first != NULL) && (((ThreadData *) frame.first)->nubeLength >100)) { // En first viene un ThreadData con la nube de puntos.
            //float   wait    = 5000;
            //ofSleepMillis(wait);
            //ofLogVerbose() << "MeshThreadedGenerator :: Waited " << wait << endl;

            ThreadData* td = ((ThreadData *) frame.first);
            time_t now = time(0);
            tm *ltm = localtime(&now);

            ///GENERAR MALLA
            //f_generarMalla generarMalla = (f_generarMalla)GetProcAddress(generateMeshLibrary, "generarMalla");

            nbIN             = new NubePuntos;
            nbIN->largo      = td->nubeLength;
            nbIN->x          = td->xpix;
            nbIN->y          = td->ypix;
            nbIN->z          = td->zpix;
            faces            = new FaceStruct;
            numberFaces      = new int;
            cout << "[MeshThreadedGenerator::processFrame] nubeLength " << td->nubeLength << " " <<  nMTG << endl;
            try {
                generarMalla(nbIN, &faces, numberFaces, nframe);
                /*
                delete nbIN->x;
                delete nbIN->y;
                delete nbIN->z;
                delete nbIN;
                */
                ofLogVerbose() << "[MeshThreadedGenerator::processFrame] Termino de generar " << nframe << endl;
            }  catch (exception& e) {
                cout << "[MeshThreadedGenerator::processFrame] FALLO EL GENERAR MALLA" << e.what();
                frame.first = NULL;
            }

            ///FIN GENERAR MALLA
        }

        result              = new GeneratedResult();
        result->nube        = nbIN;
        result->hasDepth    = (frame.first != NULL);
        result->hasRGB      = (frame.second != NULL);
        result->nframe      = nframe;
        result->idMesh      = 10000 + nframe;
        result->textures    = (ThreadData *) frame.second;
        result->numberFaces = numberFaces;
        result->faces       = faces;
        /**/
        state   = GENERATOR_COMPLETE;
    }

    unlock();
}

void MeshThreadedGenerator::setState(int state) {
    lock();
    this->state = state;
    unlock();
}

int MeshThreadedGenerator::getState() {
    return this->state;
}

void MeshThreadedGenerator::processMesh(std::pair <ThreadData *, ThreadData *> frame, int nframe) {
    lock();
    ofLogVerbose() << "MeshThreadedGenerator :: processMesh " << nMTG << ", nframe " << nframe << endl;
    this->frame     = frame;
    this->nframe    = nframe;
    state           = GENERATOR_LOADED;
    unlock();
}
