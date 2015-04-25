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
    closed           = false;
    connectionClosed = false;
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
    ofAddListener(ofEvents().update, this, &ThreadServer::receiveFrame);
}

void ThreadServer::update() {
}

bool ThreadServer::checkConnError() {
    #ifdef TARGET_WIN32
    int	err	= WSAGetLastError();
    #else
    int err = errno;
    #endif

    if(connectionClosed) return true;
    switch(err) {
        case EBADF:
        case ECONNRESET:
        case EINTR: //EINTR: receive interrupted by a signal, before any data available");
		case ENOTCONN: //ENOTCONN: trying to receive before establishing a connection");
        case ENOTSOCK: //ENOTSOCK: socket argument is not a socket");
        case EOPNOTSUPP: //EOPNOTSUPP: specified flags not valid for this socket");
        case ETIMEDOUT: //ETIMEDOUT: timeout");
        case EIO: //EIO: io error");
        case ENOBUFS: //ENOBUFS: insufficient buffers to complete the operation");
        case ENOMEM: //ENOMEM: insufficient memory to complete the request");
        case EADDRNOTAVAIL: //EADDRNOTAVAIL: the specified address is not available on the remote machine");
        case EAFNOSUPPORT: //EAFNOSUPPORT: the namespace of the addr is not supported by this socket");
        case EISCONN: //EISCONN: the socket is already connected");
        case ECONNREFUSED: //ECONNREFUSED: the server has actively refused to establish the connection");
        case ENETUNREACH: //ENETUNREACH: the network of the given addr isn't reachable from this host");
        case EADDRINUSE: //EADDRINUSE: the socket address of the given addr is already in use");
        case EINPROGRESS: //EINPROGRESS: the socket is non-blocking and the connection could not be established immediately" );
        case EALREADY: //EALREADY: the socket is non-blocking and already has a pending connection in progress");
        case ENOPROTOOPT: //ENOPROTOOPT: The optname doesn't make sense for the given level.");
        case EPROTONOSUPPORT: //EPROTONOSUPPORT: The protocol or style is not supported by the namespace specified.");
        case EMFILE: //EMFILE: The process already has too many file descriptors open.");
        case ENFILE: //ENFILE: The system already has too many file descriptors open.");
        case EACCES: //EACCES: The process does not have the privilege to create a socket of the specified 	 style or protocol.");
        case EMSGSIZE: //EMSGSIZE: The socket type requires that the message be sent atomically, but the message is too large for this to be possible.");
        case EPIPE:         connectionClosed = true;
                            break;
    }
    return connectionClosed;
}

void ThreadServer::receiveFrame(ofEventArgs &e) {
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

            if(TCPCLI.isConnected()) {
                recSize = TCPCLI.receiveRawBytes((char*) &v0, sizeof(int));
                if(checkConnError()) {
                    exit();
                    return;
                }
            } else {
                return;
            }

            if(v0 == -10) {
                ofLogVerbose() << "[ThreadServer::receiveFrame] Conexión cerrada por el cliente"<< endl;
                cout << "[ThreadServer::receiveFrame] Conexión cerrada por el cliente"<< endl;
                if(TCPCLI.isConnected()) {
                    TCPCLI.close();
                } else {
                    return;
                }
                connectionClosed    = true;
                idle                = true;
                closed              = true;
                unlock();
                return;
            }

            if(TCPCLI.isConnected()) {
                recSize = TCPCLI.receiveRawBytes((char*) &v1, sizeof(int));
            } else {
                return;
            }

            if(!((v0 >= 0) && (v0 < 1000) && (v1 >= 0) && (v1 <= 60000))) {
                ofLogVerbose() << "[ThreadServer::receiveFrame] Error en la conexión con el cliente. Cerrando conexión."<< endl;
                cout << "[ThreadServer::receiveFrame] Error en la conexión con el cliente. Cerrando conexión."<< endl;
                if(TCPCLI.isConnected()) {
                    TCPCLI.close();
                } else {
                    return;
                }
                connectionClosed    = true;
                idle                = true;
                closed              = true;
                unlock();
                return;
            }

            if(checkConnError()) {
                exit();
                return;
            }

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

                if(TCPCLI.isConnected()) {
                    numBytes             = TCPCLI.receiveRawBytes((char*) &recBytearray[0], sys_data->maxPackageSize);
                } else {
                    delete recBytearray;
                    return;
                }
                if(numBytes > 0 ) {
                    currBytearray        = FrameUtils::addToBytearray(recBytearray, numBytes, currBytearray, currTotal);
                    currTotal           += numBytes;
                } else {
                    if(checkConnError()) {
                        exit();
                        return;
                    }
                }
                delete recBytearray;
            } while((currTotal < (v0*sys_data->maxPackageSize + v1)) && !(b_exit));

            if(b_exit) {
                return;
            }
            ofLogVerbose() << "[ThreadServer::receiveFrame] Se recibio frame de: " << currTotal << " bytes";

            float millisThen = ofGetElapsedTimeMillis();
            float p = (float)(millisThen) - (float)(millisNow);
            ofLogVerbose() << "[ThreadServer::receiveFrame] time fraction: " << p << endl;

            if(currTotal > 0) {

                std::pair <int, ThreadData *>  tPair = FrameUtils::getThreadDataFromByteArray( currBytearray );

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
    b_exit = true;
    if(tData != NULL) {
        delete tData;
    }

    TCPCLI.close();

    unlock();
    stopThread();
}
