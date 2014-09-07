#include "MainBuffer.h"

MainBuffer::MainBuffer() {
    fm = new FieldManager();
}

MainBuffer::~MainBuffer() {
    ofLogVerbose() << "[MainBuffer::~MainBuffer]";
    delete fm;
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
void MainBuffer::addFrame( ThreadData * frame , int cam, int cli) {
    long int camId  = getCamId(cam, cli);
    ofLogVerbose() << "[MainBuffer::addFrame] camId " << camId;

    fm->calcSyncTime(frame);
    ofLogVerbose() << "[MainBuffer::addFrame] time " << frame->key.first << "-" << frame->key.second;

    Field * fi      = fm->getFieldForTime(frame->key);

    if(!fm->hasFrameFromCam(fi, camId)) {
        fm->addFrameFromCam(frame, camId, fi);
        return;
    }

    ofLogVerbose() << endl;
}

/**
* Private
*/
long int MainBuffer::getCamId(int cam, int cli) {
    return ((cli + 1) * 10000 + cam);
}

/**
* Retorna el siguiente frame a ser procesado.
* Retorna un tipo pair conteniendo:
* pair.first: Nube de punto concatenada.
* pair.second: Array de ThreadData conteniendo las texturas de todas las c�maras
* de todos los clientes para ese frame (a�n no implementado).
*/
std::pair <ThreadData *, ThreadData *> MainBuffer::getNextFrame() {
    Field * fi = fm->getNextFilledField();
    std::pair <ThreadData *, ThreadData *> ret;
    ret.first   = NULL;
    ret.second  = NULL;

    if(fi == NULL) {
        ofLogVerbose() << "[MainBuffer::getNextFrame] No se encontr� un frame sin procesar para devolver. Retorno NULL.";
        return ret;
    }

    for (map< long int, ThreadData * >::iterator it = fi->frame_map.begin(); it != fi->frame_map.end(); ++it) {
        if( (((ThreadData *) it->second)->state == 2 ) || (((ThreadData *) it->second)->state == 3 )) {
            if(((ThreadData *) it->second)->nubeLength > 0) {
                if(ret.first == NULL) {
                    ret.first = (ThreadData *) it->second;
                } else {
                    ret.first->mergePointClouds((ThreadData *) it->second);
                }
            }
        }
    }

    return ret;
}
