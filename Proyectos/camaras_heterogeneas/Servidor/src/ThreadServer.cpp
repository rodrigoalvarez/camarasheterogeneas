#include "ThreadServer.h"

#include <iostream>
#include <fstream>
#include <cerrno>

#ifdef TARGET_WIN32
#define ENOTCONN        WSAENOTCONN
#define EWOULDBLOCK     WSAEWOULDBLOCK
#define ENOBUFS         WSAENOBUFS
#define ECONNRESET      WSAECONNRESET
#define ESHUTDOWN       WSAESHUTDOWN
#define EAFNOSUPPORT    WSAEAFNOSUPPORT
#define EPROTONOSUPPORT WSAEPROTONOSUPPORT
#define EINPROGRESS     WSAEINPROGRESS
#define EISCONN         WSAEISCONN
#define ENOTSOCK        WSAENOTSOCK
#define EOPNOTSUPP      WSAEOPNOTSUPP
#define ETIMEDOUT       WSAETIMEDOUT
#define EADDRNOTAVAIL   WSAEADDRNOTAVAIL
#define ECONNREFUSED    WSAECONNREFUSED
#define ENETUNREACH     WSAENETUNREACH
#define EADDRINUSE      WSAEADDRINUSE
#define EADDRINUSE      WSAEADDRINUSE
#define EALREADY        WSAEALREADY
#define ENOPROTOOPT     WSAENOPROTOOPT
#define EMSGSIZE        WSAEMSGSIZE
#define ECONNABORTED    WSAECONNABORTED
#endif


using namespace std;

void ThreadServer::threadedFunction() {

    HINSTANCE hGetProcIDDLL;
    hGetProcIDDLL   =  LoadLibraryA("imageCompression.dll");
    if (!hGetProcIDDLL) {
        std::cout << "No se pudo cargar la libreria: " << std::endl;
    }
    decompress_img  = (f_decompress_img) GetProcAddress(hGetProcIDDLL, "decompress_img");

    ofLogVerbose() << "[ThreadServer::threadedFunction] " << this->ip << ":" << this->port;
    TCPCLI.setup(this->ip, this->port, true);

    currBytearray = NULL;
    started = true;
    //ofAddListener(ofEvents().update, this, &ThreadServer::receiveFrame);

    while(isThreadRunning()) {

        /*timeval curTime;
        gettimeofday(&curTime, NULL);
        int milli = curTime.tv_usec / 1000;
        char buffer [80];
        strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&curTime.tv_sec));
        char currentTime[84] = "";
        sprintf(currentTime, "%s:%d", buffer, milli);*/

        ofLogVerbose() << "[ThreadServer::threadedFunction] Hago polling - (FPS) " << ofToString(ofGetFrameRate());
        receiveFrame();
    }
}

void ThreadServer::update() {
    //receiveFrame();
}

bool ThreadServer::checkConnError() {
    #ifdef TARGET_WIN32
    int	err	= WSAGetLastError();
    #else
    int err = errno;
    #endif
    if(err == ECONNRESET) {
        connectionClosed = true;
        return true;
    }
    return false;
}

void ThreadServer::receiveFrame() {
    if(connectionClosed) return;
    if(!started) return;
    if(!idle) {
        ofLogVerbose() << "[ThreadServer::receiveFrame] :: NO IDLE / FPS " << ofToString(ofGetFrameRate()) << endl;
        return;
    }
    idle = false;

    ofLogVerbose() << "[ThreadServer::receiveFrame] :: IDLE / FPS " << ofToString(ofGetFrameRate()) << endl;
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
            int recSize = 0;
            int	err = 0;
            recSize = TCPCLI.receiveRawBytes((char*) &v0, sizeof(int));
            if(checkConnError()) return;
            recSize = TCPCLI.receiveRawBytes((char*) &v1, sizeof(int));
            if(checkConnError()) return;

            ofLogVerbose() << "" << endl;

            timeval curTime;
            gettimeofday(&curTime, NULL);
            int milli = curTime.tv_usec / 1000;
            char buffer [80];
            strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&curTime.tv_sec));
            char currentTime[84] = "";
            sprintf(currentTime, "%s:%d", buffer, milli);

            float millisNow = ofGetElapsedTimeMillis();
            ofLogVerbose() << "[ThreadServer::receiveFrame] RECIBIENDO NUVO FRAME: " << currentTime;
            do {
                char * recBytearray  = new char [sys_data->maxPackageSize];
                numBytes             = TCPCLI.receiveRawBytes((char*) &recBytearray[0], sys_data->maxPackageSize);
                if(numBytes > 0 ) {
                    currBytearray        = FrameUtils::addToBytearray(recBytearray, numBytes, currBytearray, currTotal);
                    currTotal           += numBytes;
                } else {
                    if(checkConnError()) return;
                }
                delete recBytearray;
            } while(currTotal < (v0*sys_data->maxPackageSize + v1));
            ofLogVerbose() << "[ThreadServer::receiveFrame] Se recibio frame de: " << currTotal << " bytes";

            float millisThen = ofGetElapsedTimeMillis();
            float p = (float)(millisThen) - (float)(millisNow);
            ofLogVerbose() << "[ThreadServer::receiveFrame] time fraction: " << p << endl;

            if(currTotal > 0) {

                std::pair <int, ThreadData *>  tPair = FrameUtils::getThreadDataFromByteArray( currBytearray );

                /*
                ThreadData * prevFrame  = tPair.second;
                while(prevFrame != NULL) {
                    ThreadData * td = prevFrame;
                    prevFrame = prevFrame->sig;
                    if(td->state > 0) {
                        //PROBLEMA:
                        td->releaseResources();
                    }
                }
                delete prevFrame;
                */

                ofLogVerbose() << "[ThreadServer::receiveFrame] Por agregar frame a buffer con " << tPair.first;

                FrameUtils::decompressImages(tPair.second, tPair.first, decompress_img);

                fb.addFrame(tPair.second, tPair.first);

                ofLogVerbose() << "[ThreadServer::receiveFrame] Estado del buffer de este ThreadServer: fb.tope " << fb.tope  << ", fb.base " << fb.base;
            }
            unlock();
        }

    } catch (exception& e) {
        ofLogVerbose() << "[ThreadServer::receiveFrame] CATCH " << e.what();
        ofLogVerbose() << "[ThreadServer::receiveFrame] An exception occurred. ";
    }
    idle = true;
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
