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
* Un frame está compuesto por información de varias cámaras.
* Esta función descompone esa información y agrupa la info de c/cámara según un redondeo
* del tiempo en el que haya sido usado.
* En caso que el buffer esté lleno, inserta igualmente y desplaza los indicadores.
* Nunca deja de almacenar, siempre guarda y descarta lo más viejo.
* Recibe por parámetro el frame y la totalidad de Cámaras en ese momento.
* Asumo que en frame viene una única cámara.
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
