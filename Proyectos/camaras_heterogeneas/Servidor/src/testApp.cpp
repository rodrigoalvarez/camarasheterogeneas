#include "testApp.h"
#include "ofxNetwork.h"
//--------------------------------------------------------------
void testApp::setup(){
    ofSetFrameRate(1);
    TCP.setup(11998);
    /*udpConnection.Create();
	udpConnection.Bind(11998);
	udpConnection.SetNonBlocking(true);*/
	tData2 = NULL;
}

//--------------------------------------------------------------
void testApp::update() {
    for(int i = 0; i < TCP.getLastID(); i++) { // getLastID is UID of all clients
        if( TCP.isClientConnected(i) ) { // check and see if it's still around
          // maybe the client is sending something
          string str      = TCP.receive(i);
          if(str.size() > 0) {
              //char *bytearray = new char[str.size()+1];
              //strcpy(bytearray, str.c_str());
              if(tData2 != NULL) {
                  delete tData2;
              }
              //char * bytearray  = str.c_str();
              tData2 = FrameUtils::getThreadDataFromByteArray( (char *)str.c_str() );
              //delete bytearray;
          }
        }
    }
    /*
    int bufferSize = 756360;
    char bytearray[bufferSize];
    int size = udpConnection.Receive(bytearray, bufferSize);

    if (size > 0) {

        ThreadData * tData2  = FrameUtils::getThreadDataFromByteArray(bytearray);
        delete tData2;*/
        /*t_completeFrame * obj = (t_completeFrame*) udpMessage;
        printf("recibimos datos : \n\r");
	    //string message  =   udpMessage;
        printf("Se recibió: totTrans:%d\n\r",   obj->totTrans);
        printf("Se recibió: totRot:%d\n\r",     obj->totRot);

        char udpMessageTrans[obj->totTrans * sizeof(t_translation)];
        udpConnection.Receive(udpMessageTrans, obj->totTrans * sizeof(t_translation));
        obj->arrTranslation = (t_translation *) udpMessageTrans;
        //obj->arrTranslation = (t_translation *) &udpMessage[sizeof(t_completeFrame)];
        //obj->arrTranslation = (t_translation*) udpMessage[sizeof(t_completeFrame)];
        printf("Se recibió: obj->arrTranslation[0].translationX:%d\n\r", obj->arrTranslation[0].translationX);*/
	/*} else {
	  //  printf("pasa naranja");
	}*/
}

//--------------------------------------------------------------
void testApp::draw(){
    if(tData2 != NULL) {
      cout << "width: " << tData2[0].img.getWidth() << endl;
      cout << "height: " << tData2[0].img.getHeight() << endl;
      tData2[0].img.setUseTexture(TRUE);
      tData2[0].img.reloadTexture();
      //tData2[0].img.draw(0,0);
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){

}
