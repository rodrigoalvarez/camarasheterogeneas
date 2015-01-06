#include "Server.h"
#include "ofxNetwork.h"
#include <sys/time.h>

#include "ofxSimpleGuiToo.h"

#include <stdio.h>
#include <sys/types.h>

#include <string.h>

bool compare_texture(DebugTexture * first, DebugTexture * second) {
    if(first->cli == second->cli) {
        return (first->cam < second->cam);
    }
    return (first->cli < second->cli);
}

void Server::setupGui(string ip) {
    ServerGlobalData::debug = false;
    gui.addFPSCounter();
    gui.addTitle("IP: " + ip);
    gui.addTitle("Port: " + ofToString(gdata->sys_data->serverPort));
    gui.addTitle("Desired FPS: " + ofToString(gdata->sys_data->fps));
    gui.addTitle("Max Package Size: " + ofToString(gdata->sys_data->maxPackageSize));
    gui.addTitle("Sync Factor: " + ofToString(gdata->sys_data->syncFactorValue));
    gui.addTitle("Max Threads: " + ofToString(gdata->sys_data->maxThreadedServers));
    gui.addTitle("Max Cli Buffer: " + ofToString(gdata->sys_data->maxReceiveFrameBuffer));
    gui.show();
}

void Server::setVideoPreview(int cli, int cam, ofImage img) {

    if(!img.isAllocated()) return;
    ofImage tmpImg;
    tmpImg.allocate(img.getWidth(), img.getHeight(), OF_IMAGE_COLOR);
    tmpImg.setFromPixels(img.getPixelsRef());
    tmpImg.resize(320, 240);

    ofTexture * txt = NULL;
    DebugTexture * dt= NULL;

    for (it=list.begin(); it!=list.end(); ++it) {
        if( ((*it)->cli == cli) && ((*it)->cam == cam) ) {
            dt  = (*it);
        }
    }

    if(dt == NULL) {
        dt                  = new DebugTexture();
        dt->videoTexture    = new ofTexture();
        dt->videoTexture->allocate(320, 240, GL_RGB);
        dt->cli             = cli;
        dt->cam             = cam;
        std::string title   = "Cli: " + ofToString(cli) + ", Cam: " + ofToString(cam);
        gui.addContent(title, *dt->videoTexture);
        list.push_back(dt);
        list.sort(compare_texture);
    }

    dt->videoTexture->loadData((unsigned char *)tmpImg.getPixels(), 320, 240, GL_RGB);
    tmpImg.clear();
    ofLogVerbose() << "[Server::setVideoPreview] end search" << endl;
}

void Server::exit() {
    int i = 0;
    for(i = 0; i < totThreadedServers; i++) {
        tservers[i]->exit();
        tservers[i]->stopThread();
        delete tservers[i];
    }

    TCP.close();

    for (it=list.begin(); it!=list.end(); ++it) {
        DebugTexture * result = list.front();
        delete result->videoTexture;
        list.remove(result);
        delete result;
    }

    if(gdata != NULL) {
        delete gdata;
    }

    delete mb;
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

    TCP.setup(gdata->sys_data->serverPort);

	currCliPort         = gdata->sys_data->serverPort + 1;
	totThreadedServers  = 0;
    buffLastIndex       = 0;
    buffCurrIndex       = 0;

    ofSetFrameRate(gdata->sys_data->fps);

    mb              = new MainBuffer();
    mb->sys_data    = gdata->sys_data;
    mb->startBuffer();

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
    ofLogVerbose() << "Server :: FPS " << ofToString(ofGetFrameRate()) << endl;
    for(int i = 0; i < TCP.getLastID(); i++) { // getLastID is UID of all clients
        if( TCP.isClientConnected(i) ) { // check and see if it's still around
            string str = TCP.receive(i);
            if(str.length() > 0 ) {
                std::string delimiter   = "|";
                std::size_t found       = str.find(delimiter);
                std::string cli         = str.substr(0, found);
                std::string port        = str.substr(found+1, str.size());

                ofLogVerbose() << "[Server::update] NUEVO CLIENTE id:" << str;
                ofLogVerbose() << "[Server::update] cli:"   << cli;
                ofLogVerbose() << "[Server::update] port:"  << port;
                ofLogVerbose() << "[Server::update] ip:"    << TCP.getClientIP(i);

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
        ofLogVerbose() << "[Server::computeFrames] - Server[" << i << "] : por hacer lock " << endl;
        tservers[i]->lock();
        ofLogVerbose() << "[Server::computeFrames] - Server[" << i << "] : por hacer getHeadFrame " << endl;
        std::pair <int, ThreadData *> head = tservers[i]->fb.getHeadFrame();
        ofLogVerbose() << "[Server::computeFrames] - Server[" << i << "] : Tope del buffer tiene " << head.first << " vistas de camaras.";

        for(int c = 0; c < head.first; c++) {
            if(gui.isOn()) {
                setVideoPreview(head.second[c].cliId, currCam, head.second[c].img);
            }

            currCam ++;
            mb->addFrame(&head.second[c], c, i);
        }

        /*
        ThreadData * prevFrame  = head.second;
        while(prevFrame != NULL) {
            ThreadData * td = prevFrame;
            prevFrame = prevFrame->sig;
            if(td->state > 0) {
                //PROBLEMA:
                td->releaseResources();
            }
        }
        delete prevFrame;*/

        tservers[i]->unlock();
    }
}

//--------------------------------------------------------------
void Server::draw() {

    if(gui.isOn()) {
        gui.draw();
    }
}

//--------------------------------------------------------------
void Server::keyPressed(int key) {
    if(key == 'g') {
        if(gui.isOn()) {
            gui.hide();
        } else {
            gui.show();
        }
    } else if(key == 'd') {
        ServerGlobalData::debug = !ServerGlobalData::debug;
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
