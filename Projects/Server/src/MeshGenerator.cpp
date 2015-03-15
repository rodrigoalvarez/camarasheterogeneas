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

/*
struct NubePuntos{
    float* x;
    float* y;
    float* z;
    int largo;
};
*/

//string facesMemoryKey   = "Faces";
//string nFacesMemoryKey  = "NumberFaces";
//int nFacesMemorySize    = sizeof(int);
//int facesMemorySize;

//typedef void (*f_generarMalla)(NubePuntos* nbIN, FaceStruct** faces, int* numberFaces, int nroFrame);
//typedef void (*f_compartirMalla)(int idMesh, int numberFaces, FaceStruct* faces);
//typedef void (*f_ShareImage)(int* idImage, unsigned char* pixels, int* wPixels, int* hPixels);

//int nframe          = 0;
//int currTProcesor   = 0;
//int idMesh          = 0;
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

    /*
    while(isThreadRunning()) {
        ofLogVerbose() << "MeshGenerator :: FPS " << ofToString(ofGetFrameRate()) << endl;
        //ofSleepMillis(1000/sys_data->fps);
        //processFrame();
    }*/

}

void MeshGenerator::processFrame(ofEventArgs &e) {

    if(!started) return;
    ofLogVerbose() << "MeshGenerator :: FPS " << ofToString(ofGetFrameRate()) << endl;

    if(!__idle) {
        ofLogVerbose() << "MeshGenerator :: NO IDLE / FPS " << ofToString(ofGetFrameRate()) << endl;
        return;
    }
    //buffer->getNextFrame();
    //return;
    __idle = false;
    ofLogVerbose() << "MeshGenerator :: IDLE / FPS " << ofToString(ofGetFrameRate()) << endl;
    int i = 0;
    for(i=0; i<sys_data->totalFreeCores; i++) {
        proc ++;
        proc = proc % sys_data->totalFreeCores;
        if(threads[proc].getState() == GENERATOR_IDLE) {
            std::pair <ThreadData *, ThreadData *> frame = buffer->getNextFrame();
            if((frame.first != NULL) || (frame.second != NULL)) {
                threads[proc].processMesh(frame, nframe);
                nframe++;
                break;
            }
            /*if(frame.first != NULL) {
                delete frame.first;
                frame.first = NULL;
            }
            if(frame.second != NULL) {
                while(frame.second != NULL){
                    ThreadData * curr = frame.second;
                    frame.second = frame.second->sig;
                    delete curr;
                }
            }*/
        }
    }
    __idle = true;
    /*return;

    ofLogVerbose() << "MeshGenerator :: FPS " << ofToString(ofGetFrameRate()) << endl;

    if(!__idle) {
        ofLogVerbose() << "MeshGenerator :: NO IDLE / FPS " << ofToString(ofGetFrameRate()) << endl;
        return;
    }
    __idle = false;
    ofLogVerbose() << "MeshGenerator :: IDLE / FPS " << ofToString(ofGetFrameRate()) << endl;

    int i = 0;
    for(i=0; i<sys_data->totalFreeCores; i++) {
        proc ++;
        proc = proc % sys_data->totalFreeCores;
        if(threads[proc].getState() == GENERATOR_IDLE) {

            if(frame.first != NULL) {
                delete frame.first;
                frame.first = NULL;
            }
            if(frame.second != NULL) {
                while(frame.second != NULL){
                    ThreadData * curr = frame.second;
                    frame.second = frame.second->sig;
                    delete curr;
                }
            }
        }
    }
    __idle = true;*/
}
