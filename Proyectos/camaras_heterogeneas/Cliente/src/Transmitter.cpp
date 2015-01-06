#include "Transmitter.h"
#include "Base64.h"
#include "ToHex.h"
#include "Grabber.h"

#define  CLI_PORT 15000

void Transmitter::threadedFunction() {

    HINSTANCE hGetProcIDDLL;
    hGetProcIDDLL   =  LoadLibraryA("imageCompression.dll");
    if (!hGetProcIDDLL) {
        std::cout << "No se pudo cargar la libreria: " << std::endl;
    }
    compress_img    = (f_compress_img)   GetProcAddress(hGetProcIDDLL, "compress_img");
    //decompress_img  = (f_decompress_img) GetProcAddress(hGetProcIDDLL, "decompress_img");

    state       = 0;

    std::string str =  ofToString(sys_data->cliId) + "|" + ofToString(sys_data->cliPort);
    cliId           = str;

    started = true;
    //ofAddListener(ofEvents().update, this, &Transmitter::process);

    while(isThreadRunning()) {
        ofLogVerbose()  << endl << "[Transmitter::threadedFunction] while(isThreadRunning())";
        ofSleepMillis(1000/sys_data->fps);

        process();
    }
}

void Transmitter::process() {
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
                    sendFrame((grabber->total2D + grabber->total3D), grabber->tData);
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
    int i = 0;
    for(i=0; i<totalCams; i++) {
        if(tData[i].state > 0) {
            if((tData[i].state == 1) || (tData[i].state == 3)) {
                free(tData[i].compImg);
            }
        }
    }
}
