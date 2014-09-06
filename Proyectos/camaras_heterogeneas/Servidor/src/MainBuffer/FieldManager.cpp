#include "FieldManager.h"
#include <sys/time.h>

FieldManager::FieldManager() {

}

FieldManager::~FieldManager() {
    ofLogVerbose() << "[FieldManager::~FieldManager]";
    for (std::map< std::pair <time_t, int>, Field * >::iterator it = field_map.begin(); it != field_map.end(); ++it) {
        delete ((Field *) it->second);
    }
}

/**
* Dado el timestamp del frame, retorna el tiempo correspondiente al field m�s cercano.
*/
void FieldManager::calcSyncTime(ThreadData * frame) {
    ofLogVerbose() << "[FieldManager::calcSyncTime] " << frame->curTime.tv_usec << " " << frame->curTime.tv_sec;
    float minUnit   = (1000/SERVER_BUFF_FPS);
    int milli       = frame->curTime.tv_usec / 1000;
    int step        = (milli / minUnit);

    frame->key.first    = frame->curTime.tv_sec;
    frame->key.second   = step;

    //printf("current time: %d %d \n", frame->key.first, frame->key.second);
}

/**
* Dado un tiempo timestamp, hace la conversi�n seg�n SYNC_FACTOR_VALUE y retorna
* el field para ese tiempo.
*/
Field * FieldManager::getFieldForTime(std::pair <time_t, int> timestamp) {
    std::pair <time_t, int> search = timestamp;
    ofLogVerbose() << "[FieldManager::getFieldForTime] - Buscando " << timestamp.first << "-" << timestamp.second;
    if(field_map.count(search) == 1) {
        ofLogVerbose() << "[FieldManager::getFieldForTime] - " << "ENCONTRO";
        return field_map[search];
    } else {

        Field * fi  = new Field();
        fi->id = field_map.size();
        field_map.insert ( std::pair< std::pair <time_t, int>, Field * > (search, fi) );

        ofLogVerbose() << "[FieldManager::getFieldForTime] - " << "NO ENCONTRO";
        ofLogVerbose() << "[FieldManager::getFieldForTime] - " << "NUEVO FIELD " << fi->id;

        return fi;
    }
    return NULL;
}

/**
* Chequea si este Field tiene ya un frame proveniente de esa camara.
* En caso de que sea true, entonces no se deber�a poner otro frame
* proveniente de esa c�mara.
*/
bool FieldManager::hasFrameFromCam(Field * fi, long int camId) {
    //return false;
    return fi->hasCam(camId);
}

/**
* Agrega a Field el frame para esa c�mara.
*/
void FieldManager::addFrameFromCam(ThreadData * frame, long int camId, Field * fi) {
    ofLogVerbose() << "[FieldManager::addFrameFromCam] addFrameFromCam " << camId << endl;
    fi->addFrame(frame, camId);
}

/**
* Retorna el Frame completo m�s antiguo registrado.
*/
Field * FieldManager::getOlderCompleteFrame() {
    return NULL;
}

/**
* Remueve de memoria el Field pasado por par�metro.
*/
void FieldManager::removeField(Field *) {

}
