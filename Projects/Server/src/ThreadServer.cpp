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

    pthread_mutex_init(&myMutex, NULL);

    HINSTANCE hGetProcIDDLLPC;
    hGetProcIDDLLPC   =  LoadLibraryA("FrameCompression.dll");
    if (!hGetProcIDDLLPC) {
        std::cout << "No se pudo cargar la libreria: " << std::endl;
    }

	frame_compress     = (f_compress)       GetProcAddress(hGetProcIDDLLPC, "frame_compress");
    frame_uncompress   = (f_uncompress)     GetProcAddress(hGetProcIDDLLPC, "frame_uncompress");

    ofLogVerbose() << "[ThreadServer::threadedFunction] " << this->ip << ":" << this->port;
    TCPCLI.setup(this->ip, this->port, true);

    currBytearray = NULL;
    started = true;

    unsigned long long minMillis = 1000/sys_data->fps;
    unsigned long long currMill, baseMill;

    while(isThreadRunning()) {
        baseMill = ofGetElapsedTimeMillis();
        receiveFrame();
        currMill = ofGetElapsedTimeMillis();
        if((currMill - baseMill) < minMillis) {
            sleep(minMillis - (currMill - baseMill));
        }
    }



    //ofAddListener(ofEvents().update, this, &ThreadServer::receiveFrame);
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

void ThreadServer::receiveFrame(ofEventArgs &e) {}

bool ThreadServer::connError(std::string msj, bool unl) {
    if(checkConnError()) {
        //ofLogVerbose() << ">>[ThreadServer::receiveFrame] connError - Error " << msj;
        //if(unl) unlock();
        return true;
    } else {
        //ofLogVerbose() << ">>[ThreadServer::receiveFrame] connError - Ok " << msj;
    }
    return false;
}

void ThreadServer::receiveFrame() {
    if(connectionClosed) return;
    if(!started) return;
    if(!idle) {
        ofLogVerbose() << ">>[ThreadServer::receiveFrame] :: NO IDLE / FPS ";
        return;
    }
    idle = false;

    ofLogVerbose() << ">>[ThreadServer::receiveFrame] :: IDLE / FPS ";
    try {
        if(connError("1", false)) return;
        if(TCPCLI.isConnected()) {

            //lock();
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
                TCPCLI.send("OK");
            }

            if(connError("2", true)) return;
            if(TCPCLI.isConnected()) {
                if(connError("3", true)) return;
                recSize = TCPCLI.receiveRawBytes((char*) &v0, sizeof(int));
                if(connError("4", true)) {
                    TCPCLI.send("ERROR");
                    return;
                }
            } else {
                //unlock();
                return;
            }

            if(v0 == -10) {
                ofLogVerbose() << ">>[ThreadServer::receiveFrame] Conexi�n cerrada por el cliente"<< endl;
                cout << ">>[ThreadServer::receiveFrame] Conexi�n cerrada por el cliente"<< endl;

                if(connError("5", true)) return;
                if(TCPCLI.isConnected()) {
                    if(connError("6", true)) return;

                    TCPCLI.send("OK");
                    TCPCLI.close();
                } else {
                    //unlock();
                    return;
                }

                connectionClosed    = true;
                idle                = true;
                closed              = true;
                pthread_mutex_lock(&myMutex);
                fb.~FrameBuffer();
                pthread_mutex_unlock(&myMutex);
                //unlock();
                return;
            }
            if(connError("7", true)) return;
            if(TCPCLI.isConnected()) {
                if(connError("8", true)) return;
                recSize = TCPCLI.receiveRawBytes((char*) &v1, sizeof(int));
            } else {
                //unlock();
                return;
            }

            timeval curTime;
            gettimeofday(&curTime, NULL);
            int milli = curTime.tv_usec / 1000;
            char buffer [80];
            strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&curTime.tv_sec));
            char currentTime[84] = "";
            sprintf(currentTime, "%s:%d", buffer, milli);

            float millisNow = ofGetElapsedTimeMillis();
            ofLogVerbose() << ">>[ThreadServer::receiveFrame] RECIBIENDO NUEVO FRAME: " << currentTime << ", v0: " << v0 << ", v1: " << v1;
            int guarda = 5000;
            do {
                char * recBytearray  = new char [sys_data->maxPackageSize];
                //ofLogVerbose() << ">>[ThreadServer::receiveFrame] Inicio del DO";
                //ofLogVerbose() << ">>[ThreadServer::receiveFrame] MAX_PACKAGE_SIZE " << sys_data->maxPackageSize;
                if(connError("9", true)) return;
                if(TCPCLI.isConnected()) {
                    //ofLogVerbose() << ">>[ThreadServer::receiveFrame] isConnected";
                    if(connError("10", true)) return;
                    numBytes             = TCPCLI.receiveRawBytes((char*) &recBytearray[0], sys_data->maxPackageSize);
                } else {
                    ofLogVerbose() << ">>[ThreadServer::receiveFrame] !isConnected - delete y return";
                    delete recBytearray;
                    //unlock();
                    return;
                }

                if(numBytes > 0 ) {
                    //ofLogVerbose() << ">>[ThreadServer::receiveFrame] numBytes > 0";
                    currBytearray        = FrameUtils::addToBytearray(recBytearray, numBytes, currBytearray, currTotal);
                    currTotal           += numBytes;
                } else {
                    //ofLogVerbose() << ">>[ThreadServer::receiveFrame] numBytes < 0";
                    if(checkConnError()) {
                        ofLogVerbose() << ">>[ThreadServer::receiveFrame] connError";

                        if(connError("11", true)) return;
                        if(TCPCLI.isConnected()) {
                            TCPCLI.send("ERROR");
                        }

                        exit();
                        return;
                    }
                }
                delete recBytearray;
                ofLogVerbose() << ">>[ThreadServer::receiveFrame] RECIBIENDO currTotal: " << currTotal << ", total esperado: " << (v0*sys_data->maxPackageSize + v1);
                guarda--;
            } while((currTotal < (v0*sys_data->maxPackageSize + v1)) && (guarda>0) && !(b_exit));

            if(b_exit) {
                //unlock();
                return;
            }

            if(guarda > 0) {
                if(connError("12", true)) return;
                if(TCPCLI.isConnected()) {
                    TCPCLI.send("OK");
                }

                ofLogVerbose() << ">>[ThreadServer::receiveFrame] Se recibio frame de: " << currTotal << " bytes";

                if(currTotal > 0) {
                    vector< unsigned char > uncompressed;

                    if(sys_data->allowCompression) {
                        std::vector<unsigned char> result(currBytearray, currBytearray + currTotal);
                        uncompressed    = frame_uncompress(result);
                        delete currBytearray;
                        currBytearray   = new char[uncompressed.size()];
                        memcpy(currBytearray, (char *) &uncompressed[0], uncompressed.size());
                        //currBytearray   = (char *) &uncompressed[0];
                    }

                    //ofLogVerbose() << ">>[ThreadServer::receiveFrame] por hacer el getThreadDataFromByteArray";

                    std::pair <int, ThreadData *>  tPair = FrameUtils::getThreadDataFromByteArray( currBytearray );

                    //ofLogVerbose() << ">>[ThreadServer::receiveFrame] Por agregar frame a buffer con " << tPair.first;

                    FrameUtils::decompressImages(tPair.second, tPair.first, decompress_img);

                    pthread_mutex_lock(&myMutex);
                    fb.addFrame(tPair.second, tPair.first);
                    ofLogVerbose() << ">>[ThreadServer::receiveFrame] BUFFER LENGHT " << fb.length();
                    ofLogVerbose() << ">>[ThreadServer::receiveFrame] Estado del buffer de este ThreadServer: fb.tope " << fb.tope  << ", fb.base " << fb.base;
                    pthread_mutex_unlock(&myMutex);
                }
            } else {
                if(connError("13", true)) return;
                if(TCPCLI.isConnected()) {
                    TCPCLI.send("ERROR");
                }
            }

            //unlock();
        }

    } catch (exception& e) {
        ofLogVerbose() << ">>[ThreadServer::receiveFrame] CATCH " << e.what();
        ofLogVerbose() << ">>[ThreadServer::receiveFrame] An exception occurred. ";
    }
    ofLogVerbose() << ">>[ThreadServer::receiveFrame] saliendo. ";
    idle = true;
}

char * ThreadServer::getFrame() {
    ofLogVerbose() << "[ThreadServer::getFrame]";
    lock();
    return currBytearray;
    //unlock();
}

void ThreadServer::exit() {
    ofLogVerbose() << "[ThreadServer::exit]";
    lock();
    b_exit = true;
    if(tData != NULL) {
        delete tData;
    }

    if(TCPCLI.isConnected()) {
        TCPCLI.close();
    }

    pthread_mutex_destroy(&myMutex);
    //unlock();
    stopThread();
}
