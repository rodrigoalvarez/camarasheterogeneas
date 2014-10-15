#include "Transmitter.h"
#include "Base64.h"
#include "ToHex.h"
#include "Grabber.h"

#define  CLI_PORT 15000

void Transmitter::threadedFunction() {
    state       = 0;

    std::string str =  ofToString(sys_data->cliId) + "|" + ofToString(sys_data->cliPort);
    cliId           = str;

    while(isThreadRunning()) {
        ofLogVerbose()  << endl << "[Transmitter::threadedFunction] while(isThreadRunning())";
        ofSleepMillis(1000/sys_data->fps);

        if(state == 0) { // 0 - Todav�a no se conect� al servidor
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
                        sendFrame((grabber->total2D + grabber->total3D), grabber->tData);
                    } else {
                        ofLogVerbose() << "[Transmitter::threadedFunction] El servidor no est� conectado.";
                    }
                }
            } catch (int e) {
                ofLogVerbose() << "[Transmitter::threadedFunction] An exception occurred. Exception Nr. " << e;
                state       = 3;
            }
        } else {
            ofLogVerbose() << "[Transmitter::threadedFunction] state=3";
        }
    }
}

void Transmitter::sendFrame(int totalCams, ThreadData * tData) {
    //ofLogVerbose() << " al entrar a serdFrame " << tData[0].nubeH << endl;
    /*
    La idea es simular ahora el env�o de un frame completo incluyendo sus im�genes y nubes de punto.
     1) - Formar gran bytearray // Pronto.
     2) - Dado el tama�o de los bloques en los que voy a tener que partir el gran bytearray,
          separarlo agregando marcas para poder verificar y reordenarlo del otro lado.
          Para control pensaba agregar por c/bloque: id(No de Frame), total(Total de bloques en los que se parti� el bytearray), index (N�mero de ese bloque en la secuencia)
     3) Calcular alg�n tipo de checksum de verificaci�n del frame completo para poder verificar que los datos est�n m�s o menos bien.
    */
    int imageBytesToSend    = 0;
    int totalBytesSent      = 0;
    int messageSize         = 0;


    ofLogVerbose()  << "[Transmitter::sendFrame] Por entrar a hacer compressImages " << totalCams;
    FrameUtils::compressImages(tData, totalCams);
    ofLogVerbose()  << "[Transmitter::sendFrame] Saliendo de hacer compressImages " << totalCams;

    int frameSize       = FrameUtils::getFrameSize(tData, totalCams);

    //cout << " al entrar a serdFrame2 " << tData[0].nubeH << endl;
    char * bytearray    = FrameUtils::getFrameByteArray(tData, totalCams, frameSize);

    int val0  = floor(frameSize / sys_data->maxPackageSize);   //totMaxRecSize
    int val1  = frameSize - val0 * sys_data->maxPackageSize; //resto
    TCPSVR.sendRawBytesToAll((char*) &val0, sizeof(int));
    TCPSVR.sendRawBytesToAll((char*) &val1, sizeof(int));

    ofLogVerbose()  << endl;
    ofLogVerbose()  << "[Transmitter::sendFrame] ENVIANDO NUEVO FRAME:";
    ofLogVerbose()  << "[Transmitter::sendFrame] totalCams " << totalCams;
    ofLogVerbose()  << "[Transmitter::sendFrame] frameSize " << frameSize;
    ofLogVerbose()  << "[Transmitter::sendFrame] cantidad de paquetes " << (val0 + val1);
    ofLogVerbose()  << "[Transmitter::sendFrame] conexiones " << TCPSVR.getNumClients();
    ofLogVerbose()  << endl;

    // FIN DE INTENTO ENVIAR FOTO A VER SI FALLA

    imageBytesToSend    = frameSize;
    totalBytesSent      = 0;
    messageSize         = sys_data->maxPackageSize;
    while( imageBytesToSend > 1 ) {
        if(imageBytesToSend > messageSize) {
            TCPSVR.sendRawBytesToAll((const char*) &bytearray[totalBytesSent], messageSize);
            imageBytesToSend    -= messageSize;
            totalBytesSent      += messageSize;
        } else {
            TCPSVR.sendRawBytesToAll((char*) &bytearray[totalBytesSent], imageBytesToSend);
            totalBytesSent += imageBytesToSend;
            imageBytesToSend = 0;
        }
    }

    free(bytearray);
}
