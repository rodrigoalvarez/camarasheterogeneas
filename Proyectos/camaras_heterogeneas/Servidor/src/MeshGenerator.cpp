#include "MeshGenerator.h"

#include <iostream>
#include <fstream>
using namespace std;

void MeshGenerator::threadedFunction() {
    if(buffer == NULL) return;

    while(isThreadRunning()) {
        ofSleepMillis(1000/FPS);
        processFrame();
    }
}

void MeshGenerator::processFrame() {
    std::pair <ThreadData *, ThreadData *> frame = buffer->getNextFrame();
    if(frame.first != NULL) { // En first viene un ThreadData con la nube de puntos.
        ofLogVerbose() << " [MeshGenerator::processFrame] - Obtuve un frame mergeado con nube de puntos largo: " << ((ThreadData *) frame.first)->nubeLength;
    }

    if(frame.second != NULL) { // En first viene un array de ThreadData con las texturas.

    }
}
