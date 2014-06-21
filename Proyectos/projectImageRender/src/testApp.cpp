#include "testApp.h"

float marginLeft = 0;
float marginTop = 0;
bool imageIsFrame = false;
float imagePosition = 0;


void testApp::setup() {

	ofDirectory dir2D("C:/of_v0.8.1_vs_release/apps/myApps/projectRecorder3/bin/data");
	dir2D.allowExt("mov");
	dir2D.listDir();
	movCant = min(PLAYER_CANT, dir2D.numFiles());
	for(int i = 0; i < movCant; i++) {
		movPlayer[i].loadMovie(dir2D.getPath(i));
		//movPlayer[i].play();
	}


	ofSetVerticalSync(true); 
	ofEnableSmoothing(); 
    
    gui = new ofxUISuperCanvas("FRAME MANAGER");

    gui->addSpacer();
	vector<string> radioItems;
	radioItems.push_back("SECOND");
	radioItems.push_back("FRAME");
	gui->addRadio("TYPE", radioItems, OFX_UI_ORIENTATION_HORIZONTAL);
	
    gui->addSpacer();
	gui->addTextInput("TEXTVALUE", "0");

    gui->addSpacer();
	gui->addLabelButton("SAVE", false);

    gui->autoSizeToFitWidgets();
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
    gui->loadSettings("guiSettings.xml");

}


void testApp::update() {

	for(int i = 0; i < movCant; i++) {
		
		if (imageIsFrame) {
			movPlayer[i].setFrame(imagePosition);
		} else if (movPlayer[i].getDuration() > 0) {
			movPlayer[i].setPosition(imagePosition / movPlayer[i].getDuration());
		}
		movPlayer[i].update();
	}
}


void testApp::draw() {
	
	marginLeft = 0;
	marginTop = 0;

	ofSetHexColor(0xffffff);
	for(int i = 0; i < movCant; i++) {
		movPlayer[i].draw(marginLeft, 200, movPlayer[i].getWidth() / 2.0f, movPlayer[i].getHeight() / 2.0f);
		
		std::string pos = std::to_string(movPlayer[i].getCurrentFrame());
		std::string tim = std::to_string(movPlayer[i].getPosition() * movPlayer[i].getDuration());
		std::string dur = std::to_string(movPlayer[i].getDuration());
		std::string tfra = std::to_string(movPlayer[i].getTotalNumFrames());
		
		ofDrawBitmapString("Frame: " + pos, marginLeft, 220);
		ofDrawBitmapString("Time: " + tim, marginLeft, 240);
		ofDrawBitmapString("Duration: " + dur, marginLeft, 260);
		ofDrawBitmapString("TotalFrames: " + tfra, marginLeft, 280);

		marginLeft += movPlayer[i].getWidth() / 2.0;
		marginTop = max(marginTop, movPlayer[i].getHeight() / 2.0f);
	}
}


void testApp::keyPressed(int key) {

}


void testApp::guiEvent(ofxUIEventArgs &e) {
	
	string name = e.widget->getName();
	if(name == "TEXTVALUE")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		imagePosition = std::atof(widget->getTextString().c_str());
		cout << "TEXT " << imagePosition << endl;

	}
	if(name == "TYPE")
	{
		ofxUIRadio* widget = (ofxUIRadio*) e.widget;
		imageIsFrame = widget->getValue() == 1;
		cout << "TYPE " << imageIsFrame << endl;

	}
	if(name == "SAVE")
	{
		ofxUILabelButton* widget = (ofxUILabelButton*) e.widget;
		cout << "SAVE " << widget->getValue() << endl;

		if (widget->getValue() == 1) {
			for(int i = 0; i < movCant; i++) {
				ofImage img;
				img.allocate(movPlayer[i].getWidth(), movPlayer[i].getHeight(), OF_IMAGE_COLOR);
				img.setFromPixels(movPlayer[i].getPixels(), movPlayer[i].getWidth(), movPlayer[i].getHeight(), OF_IMAGE_COLOR, true);
				img.saveImage("image" + ofToString(i) + ".png");
			}
		}

	}


	/*string name = e.widget->getName();
	int kind = e.widget->getKind();

	if(name == "TEXTVALUE")
	{
		ofxUITextInput* slider = (ofxUITextInput*) e.widget;
		string text = slider->getTextString();
		imageSecond = std::atof(text.c_str());
		cout << imageSecond << endl;

	} else if (name == "TEXTFRAME") {

		ofxUITextInput* slider = (ofxUITextInput*) e.widget;
		string text = slider->getTextString();
		imageFrame = std::atof(text.c_str());
		cout << imageFrame << endl;
	}*/
}
