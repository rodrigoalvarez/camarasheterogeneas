#include "ThreadServer.h"

#include <iostream>
#include <fstream>
using namespace std;

void ThreadServer::threadedFunction() {
    ofLogVerbose() << "[ThreadServer::threadedFunction] " << this->ip << ":" << this->port;
    TCPCLI.setup(this->ip, this->port, true);

    currBytearray = NULL;
    while(isThreadRunning()) {
        ofSleepMillis(1000/sys_data->fps);
        receiveFrame();
    }
}

void ThreadServer::update() {
    receiveFrame();
}

void ThreadServer::receiveFrame() {
    try {
        if(TCPCLI.isConnected()) {

            lock();
            int currTotal        = 0;
            int numBytes         = 0;

            if(currBytearray != NULL) {
                delete currBytearray;
                currBytearray = NULL;
            }

            int v0 = 0;
            int v1 = 0;
            TCPCLI.receiveRawBytes((char*) &v0, sizeof(int));
            TCPCLI.receiveRawBytes((char*) &v1, sizeof(int));

            ofLogVerbose() << "" << endl;
            ofLogVerbose() << "[ThreadServer::receiveFrame] RECIBIENDO NUVO FRAME: ";
            do {
                char * recBytearray  = new char [sys_data->maxPackageSize];
                numBytes             = TCPCLI.receiveRawBytes((char*) &recBytearray[0], sys_data->maxPackageSize);
                if(numBytes > 0 ) {
                    currBytearray        = FrameUtils::addToBytearray(recBytearray, numBytes, currBytearray, currTotal);
                    currTotal           += numBytes;
                }
                delete recBytearray;
            } while(currTotal < (v0*sys_data->maxPackageSize + v1));
            ofLogVerbose() << "[ThreadServer::receiveFrame] Se recibio frame de: " << currTotal << " bytes";

            if(currTotal > 0) {
                //int cams    = FrameUtils::getTotalCamerasFromByteArray( currBytearray );
                std::pair <int, ThreadData *>  tPair = FrameUtils::getThreadDataFromByteArray( currBytearray );
                //tData       = FrameUtils::getThreadDataFromByteArray( currBytearray );
                ThreadData * td = tPair.second;
                //td->img.saveImage("desde_thread_server.jpg");
                ofLogVerbose() << "[ThreadServer::receiveFrame] Por agregar frame a buffer con " << tPair.first;
                fb.addFrame(tPair.second, tPair.first);
                //std::pair <int, ThreadData *> head = fb.getHeadFrame();
                //ofLogVerbose() << "[ThreadServer::receiveFrame] prueba. tot cameras " << head.first;
                ofLogVerbose() << "[ThreadServer::receiveFrame] Estado del buffer de este ThreadServer: fb.tope " << fb.tope  << ", fb.base " << fb.base;

             }
            unlock();
        }

    } catch (exception& e) {
        ofLogVerbose() << "[ThreadServer::receiveFrame] CATCH " << e.what();
        ofLogVerbose() << "[ThreadServer::receiveFrame] An exception occurred. ";
        //cout << "[PROYECTO] An exception occurred. Exception Nr. " << '\n';
    }
}

char * ThreadServer::getFrame() {
    ofLogVerbose() << "[ThreadServer::getFrame]";
    lock();
    return currBytearray;
    unlock();
}

void ThreadServer::exit() {
    ofLogVerbose() << "[ThreadServer::exit]";
    lock();

    if(tData != NULL) {
        delete tData;
    }

    TCPCLI.close();

    unlock();
}
