#include "MainBufferRT.h"

MainBufferRT::MainBufferRT() {
    for(int i=0; i<50; i++) {
        for(int j=0; j<50; j++) {
            tdatabusy[i][j] = false;
        }
    }
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
    //long int camId  = getCamId(cam, cli);
    while(tdatabusy[cli][cam]);
    tdatabusy[cli][cam] = true;

    if(tdata[cli][cam] != NULL) {
        delete tdata[cli][cam];
    }
    tdata[cli][cam] = frame;

    tdatabusy[cli][cam] = false;
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

    for(int i=0; i<50; i++) {
        for(int j=0; j<50; j++) {

            while(tdatabusy[i][j]);
            tdatabusy[i][j] = true;
            ThreadData * curr = ThreadData::Clone(tdata[i][j]);
            tdatabusy[i][j] = false;

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

            if( (curr != NULL) && ((curr->state == 1 ) || (curr->state == 3 ))) {
                ThreadData * td = ThreadData::Clone(curr);

                if(ret.second == NULL) {
                    ofLogVerbose() << "[MainBuffer::getNextFrame] primera imagen RGB.";
                    td->sig     = NULL;
                } else {
                    ofLogVerbose() << "[MainBuffer::getNextFrame] agrego otra imagen imagen RGB.";
                    td->sig     = ret.second;
                }
                ret.second  = td;
            }
            delete curr;

        }
    }

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
