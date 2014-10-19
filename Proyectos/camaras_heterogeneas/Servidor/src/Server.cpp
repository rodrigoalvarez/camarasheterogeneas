#include "Server.h"
#include "ofxNetwork.h"
#include <sys/time.h>

#include "ofxSimpleGuiToo.h"

#include <stdio.h>
#include <sys/types.h>

#include <string.h>
//--------------------------------------------------------------
int      box2;
int procesar = 0;

ofTexture			videoTexture1;
ofTexture			videoTexture2;
ofTexture			videoTexture3;
ofTexture			videoTexture4;
ofTexture			videoTexture5;
ofTexture			videoTexture6;
ofTexture			videoTexture7;
ofTexture			videoTexture8;
ofTexture			videoTexture9;
ofImage tmp;
ofVideoGrabber		vidGrabber;

void Server::setupGui(string ip) {

    gui.addTitle("IP: " + ip);
    gui.addTitle("Port: " + ofToString(gdata->sys_data->serverPort));
    gui.addTitle("FPS: " + ofToString(gdata->sys_data->fps));
    gui.addTitle("Max Package Size: " + ofToString(gdata->sys_data->maxPackageSize));
    gui.addTitle("Sync Factor: " + ofToString(gdata->sys_data->syncFactorValue));
    gui.addTitle("Max Threads: " + ofToString(gdata->sys_data->maxThreadedServers));
    gui.addTitle("Max Cli Buffer: " + ofToString(gdata->sys_data->maxReceiveFrameBuffer));

    videoTexture1.allocate(320, 240, GL_RGB);
    videoTexture2.allocate(320, 240, GL_RGB);
    videoTexture3.allocate(320, 240, GL_RGB);
    videoTexture4.allocate(320, 240, GL_RGB);
    videoTexture5.allocate(320, 240, GL_RGB);
    videoTexture6.allocate(320, 240, GL_RGB);
    videoTexture7.allocate(320, 240, GL_RGB);
    videoTexture8.allocate(320, 240, GL_RGB);
    videoTexture9.allocate(320, 240, GL_RGB);

    gui.addContent("Camera 1", videoTexture1);
    gui.addContent("Camera 2", videoTexture2);
    gui.addContent("Camera 3", videoTexture3);
    gui.addContent("Camera 4", videoTexture4);
    gui.addContent("Camera 5", videoTexture5);
    gui.addContent("Camera 6", videoTexture6);
    gui.addContent("Camera 7", videoTexture7);
    gui.addContent("Camera 8", videoTexture8);
    gui.addContent("Camera 9", videoTexture9);

    gui.show();
}

void Server::setVideoPreview(int cli, int cam, ofImage img) {

    string search = ofToString(cli) + "_" + ofToString(cam);
    ofLogVerbose() << "[Server::setVideoPreview] search: " << search;

    if(!img.isAllocated()) return;
    //img.saveImage("desde_save_video_preview.jpg");
    ofImage tmpImg;
    tmpImg.allocate(img.getWidth(), img.getHeight(), OF_IMAGE_COLOR);
    tmpImg.setFromPixels(img.getPixelsRef());
    tmpImg.resize(320, 240);


    ofTexture txt;
    //txt = videoTexture1;

    //cout << texture_map.size()+1 << endl ;
    if(texture_map.count(search) == 1) {
        txt = texture_map[search];
    } else {
        switch(texture_map.size()+1) {
            case 1: txt = videoTexture1;break;
            case 2: txt = videoTexture2;break;
            case 3: txt = videoTexture3;break;
            case 4: txt = videoTexture4;break;
            case 5: txt = videoTexture5;break;
            case 6: txt = videoTexture6;break;
            case 7: txt = videoTexture7;break;
            case 8: txt = videoTexture8;break;
            case 9: txt = videoTexture9;break;
            default:txt = videoTexture1;break;
        }
        texture_map.insert ( std::pair< string, ofTexture > (search, txt) );
    }

    txt.loadData((unsigned char *)tmpImg.getPixels(), 320, 240, GL_RGB);

}

void Server::setup() {

    gdata   = new ServerGlobalData();
    gdata->loadCalibData("settings.xml");

    string line;
    string ip;
    ifstream IPFile;
    int offset;
    char* search0 = "IPv4 Address. . . . . . . . . . . :";      // search pattern

    system("ipconfig > ip.txt");

    IPFile.open ("ip.txt");
    if(IPFile.is_open()) {
       while(!IPFile.eof()) {
           getline(IPFile,line);
           if ((offset = line.find(search0, 0)) != string::npos) {
               line.erase(0,39);
               cout << line << endl;
               ip = line.c_str();
               IPFile.close();
           }
        }
    }
    //
    setupGui(ip);

    switch(gdata->sys_data->logLevel) {
        case 0: ofSetLogLevel(OF_LOG_VERBOSE); break;
        case 1: ofSetLogLevel(OF_LOG_NOTICE); break;
        case 2: ofSetLogLevel(OF_LOG_WARNING); break;
        case 3: ofSetLogLevel(OF_LOG_ERROR); break;
        case 4: ofSetLogLevel(OF_LOG_FATAL_ERROR); break;
        case 5: ofSetLogLevel(OF_LOG_SILENT); break;
        default:ofSetLogLevel(OF_LOG_VERBOSE); break;
    }

    ofLogToFile("server_log.txt", false);

    mb              = new MainBuffer();
    mb->sys_data    = gdata->sys_data;
    mb->startBuffer();

    ofShortPixels  spix;
    //ofSetFrameRate(gdata->sys_data->fps);
    TCP.setup(gdata->sys_data->serverPort);

	tData2              = NULL;
	currCliPort         = gdata->sys_data->serverPort + 1;
	totThreadedServers  = 0;
    buffLastIndex       = 0;
    buffCurrIndex       = 0;

    for(int i = 0; i < gdata->sys_data->maxThreadedServers; i++) {
        tservers[i] = NULL;
	}

    generator.sys_data  = gdata->sys_data;
    generator.buffer    = mb;
    generator.startThread(true, false);

}

//Dejo abierto el puerto PORT_0
//  Si recibo un cliente conectado,
        //Abro un thread con un puerto libre PUERTO_X
        //Le retorno al cliente el puerto (PUERTO_X) asignado.

//--------------------------------------------------------------
void Server::update() {
    ofSleepMillis(1000/gdata->sys_data->fps);
    vidGrabber.update();
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


                tservers[totThreadedServers]            = new ThreadServer();
                tservers[totThreadedServers]->sys_data  = gdata->sys_data;
                tservers[totThreadedServers]->cliId     = atoi(cli.c_str());
                tservers[totThreadedServers]->ip        = TCP.getClientIP(i);
                tservers[totThreadedServers]->port      = atoi(port.c_str());
                tservers[totThreadedServers]->startThread(true, false);


                currCliPort         ++;
                totThreadedServers  ++;
            }
        }
    }
    computeFrames();
}

void Server::computeFrames() {
    ofLogVerbose() << "[Server::computeFrames] - Total de Threads activos: " << totThreadedServers;
    for(int i = 0; i < totThreadedServers; i++) {
        int currCam = 1;
        tservers[i]->lock();
        std::pair <int, ThreadData *> head = tservers[i]->fb.getHeadFrame();
        ofLogVerbose() << "[Server::computeFrames] - Server[" << i << "] : Tope del buffer tiene " << head.first << " vistas de camaras.";
        for(int c = 0; c < head.first; c++) {
            //cout << "head.second[" << c << "].cliId: " << head.second[c].cliId << " , currCam " << endl;
            if(gui.isOn()) {
                setVideoPreview(head.second[c].cliId, currCam, head.second[c].img);
            }

            currCam ++;
            mb->addFrame(&head.second[c], c, i);
            ofLogVerbose() << "[Server::computeFrames] - head.second[c].nubeLength " << head.second[c].nubeLength;

            if((head.second[c].state == 3) && (head.second[c].nubeW > 0) && (procesar == 1)) {
                procesar = 0;
                generarMalla(head.second[c]);
            }/**/
        }

        tservers[i]->unlock();
    }
}

//--------------------------------------------------------------
void Server::draw() {

    if(gui.isOn()) {
        gui.draw();
    }

    //gui.hide();
    //setVideoPreview(1, tmp);
    //tmp.draw(0, 0);

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

    int downsampling = 4;

    char* fileName      = "frame_i.xyz";
    char* outputName    = "mallaFrame_i.ply";
    char* script        = "ScriptProyecto.mlx";

    //Creo el archivo b,n de la nube unida
    FILE * pFile;
    pFile = fopen (fileName,"w");
    //Recorro los frames de cada camara y me quedo solo con los 3D
    ofLogVerbose() << "[Server::generarMalla] - generarMalla() - data->nubeLength: " << data.nubeLength;
    for(int i=0; i < data.nubeLength; i ++) {
        fprintf (pFile, "%f %f %f\n", data.xpix[i], data.ypix[i], data.zpix[i]);
    }

    fclose (pFile);
    char linea[500];

    sprintf (linea, "\"C:\\Program Files\\VCG\\MeshLab\\meshlabserver\" -i %s -o %s -s %s -om vc vn", fileName, outputName, script);
    printf(linea);
    system(linea);/**/
}

//--------------------------------------------------------------
void Server::keyPressed(int key) {
    //procesar = 1;
    if(gui.isOn()) {
        gui.hide();
    } else {
        gui.show();
    }
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
