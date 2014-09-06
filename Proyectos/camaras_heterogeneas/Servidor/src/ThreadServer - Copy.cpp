#include "ThreadServer.h"

#include <iostream>
#include <fstream>
using namespace std;

void ThreadServer::threadedFunction() {
    ofLogVerbose() << "[ThreadServer::threadedFunction] " << this->ip << ":" << this->port;
    TCPCLI.setup(this->ip, this->port);

    currBytearray = NULL;
    while(isThreadRunning()) {
        ofSleepMillis(1000/FPS);
        receiveFrame();
    }
    myfile.close();
}

void ThreadServer::update() {
    receiveFrame();
}

void ThreadServer::receiveFrame() {
    try {
        if(TCPCLI.isConnected()) {

            lock();
            int currTotal           = 0;
            int numBytes            = 0;
            bool dataRecd           = false;
            //char * buffer           = NULL;
            unsigned char * buffer;
            ofLogVerbose() << "[ThreadServer::receiveFrame] ";

            //new

            string iniciaDialogo = TCPCLI.receive();
            if(TCPCLI.isConnected()) {
                ofLogVerbose() << "[ThreadServer::receiveFrame] segundo isConnected";
            }
            if(iniciaDialogo.size() > 0) {
                TCPCLI.send("OK");
                ofLogVerbose() << "[ThreadServer::receiveFrame] iniciaDialogo " << iniciaDialogo;
                std::string delimiter   = "|";
                std::size_t found       = iniciaDialogo.find(delimiter);
                std::string v0          = iniciaDialogo.substr(0, found);
                std::string v1          = iniciaDialogo.substr(found+1, iniciaDialogo.size());

                int va0 = atoi(v0.c_str());
                int va1 = atoi(v1.c_str());

                ofLogVerbose() << "[ThreadServer::receiveFrame] " << "va0=" << va0;
                ofLogVerbose() << "[ThreadServer::receiveFrame] " << "va1=" << va1;
                ofLogVerbose() << "[ThreadServer::receiveFrame] frame size " << (va0 * MAX_RECEIVE_SIZE + va1);

                buffer                  = (unsigned char*) malloc (va0 * MAX_RECEIVE_SIZE + va1);
                //buffer                  = new char [va0 * MAX_RECEIVE_SIZE + va1];
                int recd                = va0 * MAX_RECEIVE_SIZE + va1;
                int totalReceived       = 0;
                int messageSize         = MAX_RECEIVE_SIZE;

                while(recd > 0) {
                    if(recd > messageSize) {
                        TCPCLI.receiveRawBytes((char*) &buffer[totalReceived], messageSize);
                        ofLogVerbose() << "[ThreadServer::receiveFrame] recibi mensaje (size)" << messageSize;
                        recd            -= messageSize;
                        totalReceived   += messageSize;
                    } else {
                        TCPCLI.receiveRawBytes((char*) &buffer[totalReceived], recd);
                        ofLogVerbose() << "[ThreadServer::receiveFrame] recibi fin de mensaje (size)" << recd;
                        totalReceived   += recd;
                        recd = 0;
                        dataRecd = true;
                    }
                }

            }
            //end

            unlock();

            if(dataRecd) {
                /*int totCameras  = FrameUtils::getTotalCamerasFromByteArray( (char *)buffer );
                tData           = FrameUtils::getThreadDataFromByteArray( (char *)buffer );

                lock();
                fb.addFrame(tData);
                unlock();*/
                //delete buffer;
                free(buffer);
            }
        }

    } catch (exception& e) {
        ofLogVerbose() << "[ThreadServer::receiveFrame] CATCH " << e.what();
        ofLogVerbose() << "[ThreadServer::receiveFrame] An exception occurred. ";
        //cout << "[PROYECTO] An exception occurred. Exception Nr. " << '\n';
    }
}

char * ThreadServer::getFrame() {
    //lock();
    return currBytearray;
    //unlock();
}

void ThreadServer::exit() {
    lock();

    if(tData != NULL) {
        delete tData;
    }

    TCPCLI.close();

    unlock();
}
