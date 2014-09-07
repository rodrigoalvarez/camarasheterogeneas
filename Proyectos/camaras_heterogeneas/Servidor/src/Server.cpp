#include "Server.h"
#include "ofxNetwork.h"
#include <sys/time.h>
//--------------------------------------------------------------

int procesar = 0;
void Server::setup() {

    ofLogToFile("server_log.txt", false);
    ofSetLogLevel(OF_LOG_VERBOSE);

    mb    = new MainBuffer();

    ofShortPixels  spix;
    //ofSetFrameRate(FPS);
    TCP.setup(PORT_0);

	tData2              = NULL;
	currCliPort         = PORT_0 + 1;
	totThreadedServers  = 0;
    buffLastIndex       = 0;
    buffCurrIndex       = 0;

    int i;
	for(i = 0; i < MAX_THREADED_SERVERS; i++) {
        tservers[i] = NULL;
	}

    //generator.buffer = mb;
    //generator.startThread(true, false);

}

//Dejo abierto el puerto PORT_0
//  Si recibo un cliente conectado,
        //Abro un thread con un puerto libre PUERTO_X
        //Le retorno al cliente el puerto (PUERTO_X) asignado.

//--------------------------------------------------------------
void Server::update() {

    for(int i = 0; i < TCP.getLastID(); i++) { // getLastID is UID of all clients
        if( TCP.isClientConnected(i) ) { // check and see if it's still around
            string str = TCP.receive(i);
            if(str.length() > 0 ) {

                std::string delimiter   = "|";
                std::size_t found       = str.find(delimiter);
                std::string cli         = str.substr(0, found);
                std::string port        = str.substr(found+1, str.size());

                ofLogVerbose() << "[Server::update] NUEVO CLIENTE id:" << str;
                ofLogVerbose() << "[Server::update] cli:" << cli;
                ofLogVerbose() << "[Server::update] port:" << port;
                ofLogVerbose() << "[Server::update] ip:" << TCP.getClientIP(i);

                ostringstream convert;
                convert << currCliPort;

                //NOTA: Además del puerto del thread que lo atiende debería pasarle hora actual del servidor (para que sincronice) y fps.

                tservers[totThreadedServers]          = new ThreadServer();
                tservers[totThreadedServers]->cliId   = atoi(cli.c_str());
                tservers[totThreadedServers]->ip      = TCP.getClientIP(i);
                tservers[totThreadedServers]->port    = atoi(port.c_str());
                tservers[totThreadedServers]->startThread(true, false);

                currCliPort         ++;
                totThreadedServers  ++;
            }
        }
    }

    computeFrames();

}

void Server::computeFrames() {
    ofSleepMillis(1000/FPS);
    ofLogVerbose() << "[Server::computeFrames] - Total de Threads activos: " << totThreadedServers;
    for(int i = 0; i < totThreadedServers; i++) {

        tservers[i]->lock();
        std::pair <int, ThreadData *> head = tservers[i]->fb.getHeadFrame();
        ofLogVerbose() << "[Server::computeFrames] - Server[" << i << "] : Tope del buffer tiene " << head.first << " vistas de camaras.";
        for(int c = 0; c < head.first; c++) {
            mb->addFrame(&head.second[c], c, i);
            ofLogVerbose() << "[Server::computeFrames] - head.second[c].nubeLength " << head.second[c].nubeLength;
            if((head.second[c].state == 3) && (head.second[c].nubeW > 0) && (procesar == 1)) {
                procesar = 0;
                generarMalla(head.second[c]);
            }
        }

        tservers[i]->unlock();
    }
}

//--------------------------------------------------------------
void Server::draw() {
//    if(tData2 != NULL) {
//      cout << "width: " << tData2[0].img.getWidth() << endl;
//      cout << "height: " << tData2[0].img.getHeight() << endl;
//      tData2[0].img.setUseTexture(TRUE);
//      tData2[0].img.reloadTexture();
//      //tData2[0].img.draw(0,0);
//    }
}

void Server::exit() {
    int i = 0;
    for(i = 0; i < totThreadedServers; i++) {
        tservers[i]->exit();
        tservers[i]->stopThread();
        delete tservers[i];
    }
    TCP.close();
    delete mb;
}

void Server::generarMalla(ThreadData data) {
    ofLogVerbose() << "[Server::generarMalla] - generarMalla()";
    //ThreadData* data;
    //data = FrameUtils::getDummyFrame();//Obtengo el frame vacio
    int downsampling = 4;

    char* fileName      = "frame_i.xyz";
    char* outputName    = "mallaFrame_i.ply";
    char* script        = "ScriptProyecto.mlx";

    //Creo el archivo b,n de la nube unida
    FILE * pFile;
    pFile = fopen (fileName,"w");
    //Recorro los frames de cada camara y me quedo solo con los 3D
    //for (int i = 0; i<5; i++){
    //    if (data[i].state == 2 || data[i].state == 3){//Me fijo si el frame es 3D

    //for(int i=0; i < data->nubeLength; i += downsampling) {
    ofLogVerbose() << "[Server::generarMalla] - generarMalla() - data->nubeLength: " << data.nubeLength;
    for(int i=0; i < data.nubeLength; i ++) {
        //if((data->xpix[i] != -1) && (data->ypix[y * data->nubeW + x] != -1) && (data->zpix[y * data->nubeW + x] != -1)) {
        fprintf (pFile, "%f %f %f\n", data.xpix[i], data.ypix[i], data.zpix[i]);
        //}
    }

//    for(int y=0; y < data->nubeH; y += downsampling) {
//        for(int x=0; x < data->nubeW; x += downsampling) {
//            if((data->xpix[y * data->nubeW + x] != -1) && (data->ypix[y * data->nubeW + x] != -1) && (data->zpix[y * data->nubeW + x] != -1)) {
//                fprintf (pFile, "%f %f %f\n", data->xpix[y * data->nubeW + x], data->ypix[y * data->nubeW + x], data->zpix[y * data->nubeW + x]);
//            }
//        }
//    }

    //    }
    //}
    fclose (pFile);
    char linea[500];

    sprintf (linea, "\"C:\\Program Files\\VCG\\MeshLab\\meshlabserver\" -i %s -o %s -s %s -om vc vn", fileName, outputName, script);
    printf(linea);
    system(linea);/**/
}

//--------------------------------------------------------------
void Server::keyPressed(int key) {
    procesar = 1;
}

//--------------------------------------------------------------
void Server::keyReleased(int key) {

}

//--------------------------------------------------------------
void Server::mouseMoved(int x, int y ) {

}

//--------------------------------------------------------------
void Server::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void Server::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void Server::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void Server::windowResized(int w, int h){

}

//--------------------------------------------------------------
void Server::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void Server::dragEvent(ofDragInfo dragInfo){

}
