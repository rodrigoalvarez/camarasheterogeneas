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
