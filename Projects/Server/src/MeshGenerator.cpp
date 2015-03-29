#include "MeshGenerator.h"

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <windows.h>
#include <iostream>
#include <sstream>

#include <stdio.h>
#include "FreeImage.h"

using namespace std;

int proc = 0;
void MeshGenerator::threadedFunction() {
    if(buffer == NULL) return;
    currTProcesor   = 0;
    //idMesh          = 0;
    threads     = new MeshThreadedGenerator[sys_data->totalFreeCores];
    collector   = new MeshCollector();

    ofLogVerbose() << "MeshGenerator :: threadedFunction " << endl;

    for(int i=0; i<sys_data->totalFreeCores; i++) {
        threads[i].sys_data  = sys_data;
        threads[i].nMTG      = i;
        threads[i].startThread(true, false);
    }

    collector->sys_data  = sys_data;
    collector->threads   = threads;
    collector->startThread(true, false);
    started         = true;

    ofAddListener(ofEvents().update, this, &MeshGenerator::processFrame);
}

void MeshGenerator::exit() {
    b_exit          = true;
    collector->exit();
    collector->stopThread();
    int i = 0, proc = 0;
    for(i=0; (i<sys_data->totalFreeCores); i++) {
        proc ++;
        proc = proc % sys_data->totalFreeCores;
        threads[proc].exit();
        threads[proc].stopThread();
    }
}

void MeshGenerator::processFrame(ofEventArgs &e) {

    if(!started) return;
    ofLogVerbose() << "MeshGenerator :: FPS " << ofToString(ofGetFrameRate()) << endl;

    if(!__idle) {
        ofLogVerbose() << "MeshGenerator :: NO IDLE / FPS " << ofToString(ofGetFrameRate()) << endl;
        return;
    }
    __idle = false;
    ofLogVerbose() << "MeshGenerator :: IDLE / FPS " << ofToString(ofGetFrameRate()) << endl;
    int i = 0;
    for(i=0; (i<sys_data->totalFreeCores && !b_exit); i++) {
        proc ++;
        proc = proc % sys_data->totalFreeCores;
        if(threads[proc].getState() == GENERATOR_IDLE) {
            std::pair <ThreadData *, ThreadData *> frame = buffer->getNextFrame();
            if((frame.first != NULL) || (frame.second != NULL)) {
                threads[proc].processMesh(frame, nframe);
                nframe++;
                break;
            }
        }
    }
    __idle = true;
}
