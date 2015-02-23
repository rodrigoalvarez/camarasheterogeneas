#include "Transmitter.h"
#include "Base64.h"
#include "ToHex.h"
#include "Grabber.h"

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

void Transmitter::threadedFunction() {

    HINSTANCE hGetProcIDDLL;
    hGetProcIDDLL   =  LoadLibraryA("imageCompression.dll");
    if (!hGetProcIDDLL) {
        std::cout << "No se pudo cargar la libreria: " << std::endl;
    }
    compress_img    = (f_compress_img)   GetProcAddress(hGetProcIDDLL, "compress_img");

    state       = 0;

    std::string str =  ofToString(sys_data->cliId) + "|" + ofToString(sys_data->cliPort);
    cliId           = str;

    started = true;

    /*
    while(isThreadRunning()) {
        ofLogVerbose()  << endl << "[Transmitter::threadedFunction] while(isThreadRunning())";
        ofSleepMillis(1000/sys_data->fps);

        process();
    }
    */

    ofAddListener(ofEvents().update, this, &Transmitter::process);
}

bool Transmitter::checkConnError() {
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

void Transmitter::process(ofEventArgs &e) {
    if(connectionClosed) return;
    if(!started) return;
    if(!idle) {
        ofLogVerbose() << "[Transmitter::process] :: NO IDLE / FPS " << ofToString(ofGetFrameRate()) << endl;
        return;
    }
    idle = false;

    ofLogVerbose() << "[Transmitter::process] :: IDLE / FPS " << ofToString(ofGetFrameRate()) << endl;

    if(state == 0) { // 0 - Todavía no se conectó al servidor
        try {
            ofLogVerbose() << "[Transmitter::threadedFunction] state=0, conectando a " << sys_data->serverIp << "-" << sys_data->serverPort;
            TCP.setup( sys_data->serverIp, sys_data->serverPort );
            TCPSVR.setup(sys_data->cliPort, true);
            TCP.send( cliId );
            TCP.close();
            state = 2;
        } catch (int e) {
            ofLogVerbose() << "[Transmitter::threadedFunction] An exception occurred. Exception Nr. " << e;
            state       = 3;
        }
    } else if(state == 2) { // 2 - Tiene cliente asignado.
        try {
            if(TCPSVR.getNumClients() > 0) {
                if(TCPSVR.isClientConnected(0)) {
                    ofLogVerbose()  << endl << "[Transmitter::threadedFunction] Actualizando ultima informacion:";
                    grabber->updateThreadData();
                    ofLogVerbose()  << endl << "[Transmitter::threadedFunction] Saliendo de actualizar";
                    sendFrame((grabber->total2D + grabber->total3D + grabber->totalONI), grabber->tData);
                } else {
                    ofLogVerbose() << "[Transmitter::threadedFunction] El servidor no está conectado.";
                }
            }
        } catch (int e) {
            ofLogVerbose() << "[Transmitter::threadedFunction] An exception occurred. Exception Nr. " << e;
            state       = 3;
        }
    } else {
        ofLogVerbose() << "[Transmitter::threadedFunction] state=3";
    }

    idle = true;
}

void Transmitter::sendFrame(int totalCams, ThreadData * tData) {
    //ofLogVerbose() << " al entrar a serdFrame " << tData[0].nubeH << endl;
    /*
    La idea es simular ahora el envío de un frame completo incluyendo sus imágenes y nubes de punto.
     1) - Formar gran bytearray // Pronto.
     2) - Dado el tamaño de los bloques en los que voy a tener que partir el gran bytearray,
          separarlo agregando marcas para poder verificar y reordenarlo del otro lado.
          Para control pensaba agregar por c/bloque: id(No de Frame), total(Total de bloques en los que se partió el bytearray), index (Número de ese bloque en la secuencia)
     3) Calcular algún tipo de checksum de verificación del frame completo para poder verificar que los datos están más o menos bien.
    */

    int imageBytesToSend    = 0;
    int totalBytesSent      = 0;
    int messageSize         = 0;


    ofLogVerbose()  << "[Transmitter::sendFrame] Por entrar a hacer compressImages " << totalCams;
    FrameUtils::compressImages(tData, totalCams, compress_img);


    ofLogVerbose()  << "[Transmitter::sendFrame] Saliendo de hacer compressImages " << totalCams;


    int frameSize       = FrameUtils::getFrameSize(tData, totalCams);
    ofLogVerbose()  << "[Transmitter::frameSize] frameSize:" << frameSize;

    //cout << " al entrar a serdFrame2 " << tData[0].nubeH << endl;
    char * bytearray    = FrameUtils::getFrameByteArray(tData, totalCams, frameSize);

    int val0  = floor(frameSize / sys_data->maxPackageSize);   //totMaxRecSize
    int val1  = frameSize - val0 * sys_data->maxPackageSize; //resto

    if(checkConnError()) return;
    TCPSVR.sendRawBytesToAll((char*) &val0, sizeof(int));
    if(checkConnError()) return;
    TCPSVR.sendRawBytesToAll((char*) &val1, sizeof(int));

    ofLogVerbose()  << endl;
    ofLogVerbose()  << "[Transmitter::sendFrame] ENVIANDO NUEVO FRAME:";
    ofLogVerbose()  << "[Transmitter::sendFrame] totalCams " << totalCams;
    ofLogVerbose()  << "[Transmitter::sendFrame] frameSize " << frameSize;
    ofLogVerbose()  << "[Transmitter::sendFrame] cantidad de paquetes " << (val0 + val1);
    if(checkConnError()) return;
    ofLogVerbose()  << "[Transmitter::sendFrame] conexiones " << TCPSVR.getNumClients();
    ofLogVerbose()  << endl;

    // FIN DE INTENTO ENVIAR FOTO A VER SI FALLA

    imageBytesToSend    = frameSize;
    totalBytesSent      = 0;
    messageSize         = sys_data->maxPackageSize;
    while( imageBytesToSend > 1 ) {
        if(imageBytesToSend > messageSize) {
            if(checkConnError()) return;
            TCPSVR.sendRawBytesToAll((const char*) &bytearray[totalBytesSent], messageSize);
            imageBytesToSend    -= messageSize;
            totalBytesSent      += messageSize;
        } else {
            if(checkConnError()) return;
            TCPSVR.sendRawBytesToAll((char*) &bytearray[totalBytesSent], imageBytesToSend);
            totalBytesSent += imageBytesToSend;
            imageBytesToSend = 0;
        }
    }

    free(bytearray);
    int i = 0;
    for(i=0; i<totalCams; i++) {
        if(tData[i].state > 0) {
            if((tData[i].state == 1) || (tData[i].state == 3)) {
                free(tData[i].compImg);
            }
        }
    }
}

void Transmitter::exit() {
    ofLogVerbose() << "[Transmitter::exit]";

    lock();

    TCPSVR.close();

    unlock();

    stopThread();
}
