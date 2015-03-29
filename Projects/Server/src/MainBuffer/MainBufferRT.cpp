#include "MainBufferRT.h"

MainBufferRT::MainBufferRT() {
    for(int i=0; i<50; i++) {
        for(int j=0; j<50; j++) {
            tdatabusy[i][j] = false;
        }
    }
    iniData = NULL;
    pthread_mutex_init(&myMutex, NULL);
}

void MainBufferRT::startBuffer() {
    for(int i=0; i<50; i++) {
        for(int j=0; j<50; j++) {
            tdata[i][j] = NULL;
        }
    }
}

MainBufferRT::~MainBufferRT() {
    ofLogVerbose() << "[MainBufferRT::~MainBufferRT]";

    for(int i=0; i<50; i++) {
        for(int j=0; j<50; j++) {
            delete tdata[i][j];
            tdata[i][j] = NULL;
        }
    }

    pthread_mutex_destroy(&myMutex);

}

int MainBufferRT::buffLength() {
    ThreadData * iter = iniData;
    int length = 0;
    while(iter != NULL) {
        length++;
        iter = iter->sig;
    }
    return length;
}

/**
* Agrega un Frame al buffer principal.
* Un frame est� compuesto por informaci�n de varias c�maras.
* Esta funci�n descompone esa informaci�n y agrupa la info de c/c�mara seg�n un redondeo
* del tiempo en el que haya sido usado.
* En caso que el buffer est� lleno, inserta igualmente y desplaza los indicadores.
* Nunca deja de almacenar, siempre guarda y descarta lo m�s viejo.
* Recibe por par�metro el frame y la totalidad de C�maras en ese momento.
* Asumo que en frame viene una �nica c�mara.
*/
void MainBufferRT::addFrame( ThreadData * frame , int cam, int cli) {

    pthread_mutex_lock(&myMutex);
    frame->used = false;
    if(iniData == NULL) {
        iniData = frame;
        iniData->sig = NULL;
    } else {
        ThreadData * iter = iniData;
        ThreadData * prev = iter;
        while((iter!=NULL) && !((iter->cliId == cli) &&(iter->camId == cam ))) {
            ofLogVerbose() << "[MainBufferRT::addFrame] iter-cli: " << iter->cliId << ", cli: " << cli << ", camId: " << iter->camId << ", cam: " << cam;
            prev = iter;
            iter = iter->sig;
        }

        if(iter == NULL) {
            prev->sig = frame;
            ofLogVerbose() << "[MainBufferRT::addFrame] iter == NULL ";
        } else {
            if(prev == iter) {
                ofLogVerbose() << "[MainBufferRT::addFrame] " << prev << " == " << iter;
                iniData = frame;
                iniData->sig = iter->sig;
                ThreadData * tmp = iter;
                tmp->releaseResources();
            } else {
                ofLogVerbose() << "[MainBufferRT::addFrame] Else ";
                prev->sig = frame;
                frame->sig = iter->sig;
                ThreadData * tmp = iter;
                iter = prev;
                tmp->releaseResources();
            }
        }
    }

    pthread_mutex_unlock(&myMutex);
}

bool MainBufferRT::hasNewData(ThreadData * data) {
    ThreadData * it = data;
    while(it!=NULL) {
        if(!it->used) {
            return true;
        }
        it = it->sig;
    }
    return false;
}

/**
* Retorna el siguiente frame a ser procesado.
* Retorna un tipo pair conteniendo:
* pair.first: Nube de punto concatenada.
* pair.second: Array de ThreadData conteniendo las texturas de todas las c�maras
* de todos los clientes para ese frame (a�n no implementado).
*/
std::pair <ThreadData *, ThreadData *> MainBufferRT::getNextFrame() {
    std::pair <ThreadData *, ThreadData *> ret;
    ret.first   = NULL;
    ret.second  = NULL;

    if(!hasNewData(iniData)) {
        return ret;
    }

    ThreadData * it = iniData;
    while(it!=NULL) {
        ofLogVerbose() << "[MainBufferRT::getNextFrame] entro " << it << endl;
        pthread_mutex_lock(&myMutex);
        it->used = true;
        ThreadData * curr = ThreadData::Clone(it);
        pthread_mutex_unlock(&myMutex);
        ofLogVerbose() << "[MainBufferRT::getNextFrame] clono " << curr << endl;
        if( (curr != NULL) && ((curr->state == 2 ) || (curr->state == 3 ))) {
            if(curr->nubeLength > 0) {
                if(ret.first == NULL) {
                    ret.first = ThreadData::Clone(curr);
                } else {
                    ofLogVerbose() << "[MainBufferRT::getNextFrame] Mergeando puntos.";
                    ret.first->mergePointClouds(curr);
                }
            }
        }

        if( (curr != NULL) && ((curr->state == 1 ) || (curr->state == 3 )) ) {
            ThreadData * td = ThreadData::Clone(curr);
            if(ret.second == NULL) {
                ofLogVerbose() << "[MainBufferRT::getNextFrame] primera imagen RGB.";
                td->sig     = NULL;
            } else {
                ofLogVerbose() << "[MainBufferRT::getNextFrame] agrego otra imagen imagen RGB. " << td;
                td->sig     = ret.second;
            }
            ret.second  = td;
        }
        delete curr;
        ofLogVerbose() << "[MainBufferRT::getNextFrame] ACA a " << it << endl;
        ofLogVerbose() << "[MainBufferRT::getNextFrame] ACA b " << it->sig << endl;
        it = it->sig;
        ofLogVerbose() << "[MainBufferRT::getNextFrame] ACA c " << it << endl;
    }

    ofLogVerbose() << "[MainBufferRT::getNextFrame] ACA " << endl;
    ThreadData * iter = ret.second;
    bool descartado = false;
    while(iter != NULL) {
        descartado = descartado || ((iter->img.getWidth() <= 0) || (iter->img.getHeight() <= 0));
        iter = iter->sig;
    }

    if((ret.first == NULL) || (((ThreadData *) ret.first)->nubeLength <100)) {
        descartado = true;
    }

    if(descartado) {
        ret.first   = NULL;
        ret.second  = NULL;
    }

    /*
    ret.first   = NULL;
    ret.second  = NULL;
    */

    return ret;
}

/*Field * fi = fm->getNextFilledField();
    std::pair <ThreadData *, ThreadData *> ret;
    ret.first   = NULL;
    ret.second  = NULL;

    if(fi == NULL) {
        ofLogVerbose() << "[MainBuffer::getNextFrame] No se encontr� un frame sin procesar para devolver. Retorno NULL.";
        return ret;
    }

    for (map< long int, ThreadData * >::iterator it = fi->frame_map.begin(); it != fi->frame_map.end(); ++it) {
        //ofLogVerbose() << "[MainBuffer::getNextFrame] for.";
        if( (((ThreadData *) it->second)->state == 2 ) || (((ThreadData *) it->second)->state == 3 )) {
            ((ThreadData *) it->second)->cameraType = 2;
            if(((ThreadData *) it->second)->nubeLength > 0) {
                if(ret.first == NULL) {
                    ofLogVerbose() << "[MainBuffer::getNextFrame] first==NULL.";
                    ret.first = (ThreadData *) it->second;
                } else {
                    ofLogVerbose() << "[MainBuffer::getNextFrame] Mergeando puntos.";
                    ret.first->mergePointClouds((ThreadData *) it->second);
                }
            }
        }

        if( (((ThreadData *) it->second)->state == 1) || (((ThreadData *) it->second)->state == 3) ) {
            ThreadData * td = new ThreadData();
            if(((ThreadData *) it->second)->state == 3) {
                td->cameraType  = 2;
            } else {
                td->cameraType  = 1;
            }

            ThreadData * re = ((ThreadData *) it->second);

            td->cliId   = re->cliId;
            td->camId   = re->camId;

            td->img.setFromPixels(((ThreadData *) it->second)->img.getPixelsRef());
            td->row1     = re->row1;
            td->row2     = re->row2;
            td->row3     = re->row3;
            td->row4     = re->row4;

            if(ret.second == NULL) {
                ofLogVerbose() << "[MainBuffer::getNextFrame] primera imagen RGB.";
                td->sig     = NULL;
            } else {
                ofLogVerbose() << "[MainBuffer::getNextFrame] agrego otra imagen imagen RGB.";
                td->sig     = ret.second;
            }
            ret.second  = td;
        }
    }
    //fm->removeField(fi);
    ofLogVerbose() << "[MainBuffer::getNextFrame] Luego de mergear";
    return ret;*/
