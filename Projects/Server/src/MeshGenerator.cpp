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

    ofLogVerbose() << "--[MeshGenerator::threadedFunction] " << endl;

    for(int i=0; i<sys_data->totalFreeCores; i++) {
        threads[i].sys_data  = sys_data;
        threads[i].nMTG      = i;
        threads[i].startThread(true, false);
    }

    collector->sys_data  = sys_data;
    collector->threads   = threads;
    collector->startThread(true, false);
    started         = true;

    unsigned long long minMillis = 1000/sys_data->fps;
    unsigned long long currMill, baseMill;

    while(isThreadRunning()) {
        baseMill = ofGetElapsedTimeMillis();
        processFrame();
        currMill = ofGetElapsedTimeMillis();
        if((currMill - baseMill) < minMillis) {
            sleep(minMillis - (currMill - baseMill));
        }
    }
    //ofAddListener(ofEvents().update, this, &MeshGenerator::processFrame);
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
}

void MeshGenerator::processFrame() {

    if(!started) return;
    if(!__idle) {
        ofLogVerbose() << "--[MeshGenerator::processFrame] NO IDLE / FPS ";
        return;
    }
    __idle = false;
    ofLogVerbose() << "--[MeshGenerator::processFrame] IDLE / FPS ";

    int i = 0;
    while(!b_exit && (i<sys_data->totalFreeCores)) {
        if(threads[i].getState() == GENERATOR_IDLE) {
            std::pair <ThreadData *, ThreadData *> frame = buffer->getNextFrame();
            //cout << "Generador " << i << " libre" << endl;
            if((frame.first != NULL) || (frame.second != NULL)) {
            //    cout << "Asignado a Generador " << i << endl;
                threads[i].processMesh(frame, nframe);
                nframe++;
                break;
            } else {
            //    cout << "No hab�a nada para asignar al generador " << i << endl;
            }
        } else {
        //    cout << "Generador " << i << " ocupado" << endl;
        }
        i++;
    }

    __idle = true;
}
