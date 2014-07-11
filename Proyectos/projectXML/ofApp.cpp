#include "ofApp.h"
#include "ofxXmlSettings.h"
#include <array>

bool REAL_TIME = true;
int REAL_TIME_FPS = 10;
int REAL_TIME_PORT = 3232;

bool imageIsFrame = false;
float imagePosition = 0;

struct DepthSettings {
   double Near;
   double Far;
   double PointsDownSample;
};

struct Camera {
   int ResolutionX;
   int ResolutionY;
   double ResolutionDownSample;
   int FPS;
   bool ColorRGB;
   bool Use2D;
   bool Use3D;
   double Matrix[16];
   string DataContext;
   DepthSettings DepthSettings;
};

array<Camera,3> cameras;
array<ofxUISuperCanvas*,3> allCameras;
array<ofxUISuperCanvas*,3> allMatrix;

void ofApp::setup() {

	for (int i = 0; i < allCameras.size(); i++) {
		allCameras[i] = NULL;
		allMatrix[i] = NULL;
	}

	ofSetVerticalSync(true); 
	ofEnableSmoothing(); 
    
    gui = new ofxUISuperCanvas("XML GENERATOR");
	gui->setHeight(500);

    gui->addSpacer();
	gui->addLabel("General Options:");

	gui->addToggle("realTime", true);

	gui->addLabel("realTimeFPS");
	gui->addTextInput("realTimeFPSValue", "10");
	
	gui->addLabel("realTimePort");
	gui->addTextInput("realTimePortValue", "3232");

    gui->addSpacer();
	gui->addLabelButton("ADD CAMERA", false);
	gui->addLabelButton("SAVE", false);

    gui->autoSizeToFitWidgets();
    ofAddListener(gui->newGUIEvent,this,&ofApp::guiEvent);
    gui->loadSettings("guiSettings.xml");

}

void ofApp::update() {

}

void ofApp::draw() {

}

void saveXmlFile();

void ofApp::guiEvent(ofxUIEventArgs &e) {
	
	string name = e.widget->getName();
	ofxUISuperCanvas* canvas = (ofxUISuperCanvas*)e.widget->getCanvasParent();
	string canvasName = canvas->getCanvasTitle()->getLabel();

	if(name == "realTime")
	{
		ofxUIToggle* widget = (ofxUIToggle*) e.widget;
		REAL_TIME = widget->getValue() == 1;
	}
	if(name == "realTimeFPSValue")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		REAL_TIME_FPS = ofToInt(widget->getTextString());
	}
	if(name == "realTimePort")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		REAL_TIME_PORT = ofToInt(widget->getTextString());
	}
	
	if(name == "resolutionXValue") {
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "CAMERA0") {
			cameras[0].ResolutionX = ofToInt(widget->getTextString());
		}
		if (canvasName == "CAMERA1") {
			cameras[1].ResolutionX = ofToInt(widget->getTextString());
		}
		if (canvasName == "CAMERA2") {
			cameras[2].ResolutionX = ofToInt(widget->getTextString());
		}
	}
	if(name == "resolutionYValue")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "CAMERA0") {
			cameras[0].ResolutionY = ofToInt(widget->getTextString());
		}
		if (canvasName == "CAMERA1") {
			cameras[1].ResolutionY = ofToInt(widget->getTextString());
		}
		if (canvasName == "CAMERA2") {
			cameras[2].ResolutionY = ofToInt(widget->getTextString());
		}
	}
	if(name == "resolutionDownSampleValue")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "CAMERA0") {
			cameras[0].ResolutionDownSample = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA1") {
			cameras[1].ResolutionDownSample = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA2") {
			cameras[2].ResolutionDownSample = ofToDouble(widget->getTextString());
		}
	}
	if(name == "colorRGB")
	{
		ofxUIToggle* widget = (ofxUIToggle*) e.widget;
		if (canvasName == "CAMERA0") {
			cameras[0].ColorRGB = widget->getValue() == 1;
		}
		if (canvasName == "CAMERA1") {
			cameras[1].ColorRGB = widget->getValue() == 1;
		}
		if (canvasName == "CAMERA2") {
			cameras[2].ColorRGB = widget->getValue() == 1;
		}
	}
	if(name == "FPSValue")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "CAMERA0") {
			cameras[0].FPS = ofToInt(widget->getTextString());
		}
		if (canvasName == "CAMERA1") {
			cameras[0].FPS = ofToInt(widget->getTextString());
		}
		if (canvasName == "CAMERA2") {
			cameras[0].FPS = ofToInt(widget->getTextString());
		}
	}

	if(name == "3D")
	{
		ofxUIToggle* widget = (ofxUIToggle*) e.widget;
		if (canvasName == "CAMERA0") {
			cameras[0].Use3D = widget->getValue() == 1;
		}
		if (canvasName == "CAMERA1") {
			cameras[1].Use3D = widget->getValue() == 1;
		}
		if (canvasName == "CAMERA2") {
			cameras[2].Use3D = widget->getValue() == 1;
		}
	}
	if(name == "2D")
	{
		ofxUIToggle* widget = (ofxUIToggle*) e.widget;
		if (canvasName == "CAMERA0") {
			cameras[0].Use2D = widget->getValue() == 1;
		}
		if (canvasName == "CAMERA1") {
			cameras[1].Use2D = widget->getValue() == 1;
		}
		if (canvasName == "CAMERA2") {
			cameras[2].Use2D = widget->getValue() == 1;
		}
	}

	if(name == "dataContextValue")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "CAMERA0") {
			cameras[0].DataContext = widget->getTextString();
		}
		if (canvasName == "CAMERA1") {
			cameras[1].DataContext = widget->getTextString();
		}
		if (canvasName == "CAMERA2") {
			cameras[2].DataContext = widget->getTextString();
		}
	}
	if(name == "nearValue")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "CAMERA0") {
			cameras[0].DepthSettings.Near = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA1") {
			cameras[1].DepthSettings.Near = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA2") {
			cameras[2].DepthSettings.Near = ofToDouble(widget->getTextString());
		}
	}
	if(name == "farValue")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "CAMERA0") {
			cameras[0].DepthSettings.Far = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA1") {
			cameras[1].DepthSettings.Far = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA2") {
			cameras[2].DepthSettings.Far = ofToDouble(widget->getTextString());
		}
	}
	if(name == "pointsDownSampleValue")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "CAMERA0") {
			cameras[0].DepthSettings.PointsDownSample = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA1") {
			cameras[1].DepthSettings.PointsDownSample = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA2") {
			cameras[2].DepthSettings.PointsDownSample = ofToDouble(widget->getTextString());
		}
	}
	
	if(name == "m[0,0]")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "CAMERA0") {
			cameras[0].Matrix[0] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA1") {
			cameras[1].Matrix[0] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA2") {
			cameras[2].Matrix[0] = ofToDouble(widget->getTextString());
		}
	}
	if(name == "m[0,1]")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "CAMERA0") {
			cameras[0].Matrix[1] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA1") {
			cameras[1].Matrix[1] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA2") {
			cameras[2].Matrix[1] = ofToDouble(widget->getTextString());
		}
	}
	if(name == "m[0,2]")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "CAMERA0") {
			cameras[0].Matrix[2] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA1") {
			cameras[1].Matrix[2] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA2") {
			cameras[2].Matrix[2] = ofToDouble(widget->getTextString());
		}
	}
	if(name == "m[0,3]")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "CAMERA0") {
			cameras[0].Matrix[3] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA1") {
			cameras[1].Matrix[3] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA2") {
			cameras[2].Matrix[3] = ofToDouble(widget->getTextString());
		}
	}
	if(name == "m[1,0]")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "CAMERA0") {
			cameras[0].Matrix[4] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA1") {
			cameras[1].Matrix[4] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA2") {
			cameras[2].Matrix[4] = ofToDouble(widget->getTextString());
		}
	}
	if(name == "m[1,1]")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "CAMERA0") {
			cameras[0].Matrix[5] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA1") {
			cameras[1].Matrix[5] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA2") {
			cameras[2].Matrix[5] = ofToDouble(widget->getTextString());
		}
	}
	if(name == "m[1,2]")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "CAMERA0") {
			cameras[0].Matrix[6] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA1") {
			cameras[1].Matrix[6] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA2") {
			cameras[2].Matrix[6] = ofToDouble(widget->getTextString());
		}
	}
	if(name == "m[1,3]")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "CAMERA0") {
			cameras[0].Matrix[7] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA1") {
			cameras[1].Matrix[7] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA2") {
			cameras[2].Matrix[7] = ofToDouble(widget->getTextString());
		}
	}
	if(name == "m[2,0]")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "CAMERA0") {
			cameras[0].Matrix[8] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA1") {
			cameras[1].Matrix[8] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA2") {
			cameras[2].Matrix[8] = ofToDouble(widget->getTextString());
		}
	}
	if(name == "m[2,1]")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "CAMERA0") {
			cameras[0].Matrix[9] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA1") {
			cameras[1].Matrix[9] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA2") {
			cameras[2].Matrix[9] = ofToDouble(widget->getTextString());
		}
	}
	if(name == "m[2,2]")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "CAMERA0") {
			cameras[0].Matrix[10] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA1") {
			cameras[1].Matrix[10] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA2") {
			cameras[2].Matrix[10] = ofToDouble(widget->getTextString());
		}
	}
	if(name == "m[2,3]")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "CAMERA0") {
			cameras[0].Matrix[11] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA1") {
			cameras[1].Matrix[11] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA2") {
			cameras[2].Matrix[11] = ofToDouble(widget->getTextString());
		}
	}
	if(name == "m[3,0]")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "CAMERA0") {
			cameras[0].Matrix[12] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA1") {
			cameras[1].Matrix[12] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA2") {
			cameras[2].Matrix[12] = ofToDouble(widget->getTextString());
		}
	}
	if(name == "m[3,1]")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "CAMERA0") {
			cameras[0].Matrix[13] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA1") {
			cameras[1].Matrix[13] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA2") {
			cameras[2].Matrix[13] = ofToDouble(widget->getTextString());
		}
	}
	if(name == "m[3,2]")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "CAMERA0") {
			cameras[0].Matrix[14] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA1") {
			cameras[1].Matrix[14] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA2") {
			cameras[2].Matrix[14] = ofToDouble(widget->getTextString());
		}
	}
	if(name == "m[3,3]")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "CAMERA0") {
			cameras[0].Matrix[15] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA1") {
			cameras[1].Matrix[15] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA2") {
			cameras[2].Matrix[15] = ofToDouble(widget->getTextString());
		}
	}

	if(name == "SAVE")
	{
		ofxUILabelButton* widget = (ofxUILabelButton*) e.widget;
		if (widget->getValue() == 1) {
			saveXmlFile();
		}
	}
	if(name == "ADD CAMERA")
	{
		ofxUILabelButton* widget = (ofxUILabelButton*) e.widget;
		if (widget->getValue() == 1) {
			ofxUISuperCanvas* gui1;
			ofxUISuperCanvas* gui2;
			int i = 0;
			for (; i < allCameras.size(); i++) {
				if (allCameras[i] == NULL) {
					gui1 = new ofxUISuperCanvas("CAMERA " + ofToString(i));
					gui1->setPosition(250 + i * 500, 0);
					allCameras[i] = gui1;

					gui1->autoSizeToFitWidgets();
					ofAddListener(gui1->newGUIEvent,this,&ofApp::guiEvent);
					gui1->loadSettings("guiSettings.xml");

					gui2 = new ofxUISuperCanvas("MATRIX " + ofToString(i));
					gui2->setPosition(480 + i * 500, 0);
					allMatrix[i] = gui2;
					
					gui2->autoSizeToFitWidgets();
					ofAddListener(gui2->newGUIEvent,this,&ofApp::guiEvent);
					gui2->loadSettings("guiSettings.xml");
					break;
				}
			}
			gui1->setHeight(500);
			gui2->setHeight(500);

			gui1->addSpacer();
			gui1->addLabel("Camera Options:");

			gui1->addLabel("resolutionX");
			gui1->addTextInput("resolutionXValue", "800");

			gui1->addLabel("resolutionY");
			gui1->addTextInput("resolutionYValue", "600");

			gui1->addLabel("resolutionDownSample");
			gui1->addTextInput("resolutionDownSampleValue", "1");
	
			gui1->addToggle("colorRGB", true);

			gui1->addLabel("FPS");
			gui1->addTextInput("FPSValue", "24");

			vector<string> radioItems;
			radioItems.push_back("3D");
			radioItems.push_back("2D");
			gui1->addRadio("type", radioItems, OFX_UI_ORIENTATION_HORIZONTAL);

			gui1->addLabel("dataContext");
			gui1->addTextInput("dataContextValue", "");
	
			gui1->addSpacer();
			gui1->addLabel("Depth Settings:");

			gui1->addLabel("near");
			gui1->addTextInput("nearValue", "0");

			gui1->addLabel("far");
			gui1->addTextInput("farValue", "0");

			gui1->addLabel("pointsDownSample");
			gui1->addTextInput("pointsDownSampleValue", "0");
	
			gui2->addSpacer();
			gui2->addLabel("Matrix Settings:");
	
			gui2->addLabel("m[0,0]");
			gui2->addTextInput("m00", "0");
			gui2->addLabel("m[0,1]");
			gui2->addTextInput("m01", "0");
			gui2->addLabel("m[0,2]");
			gui2->addTextInput("m02", "0");
			gui2->addLabel("m[1,0]");
			gui2->addTextInput("m10", "0");
			gui2->addLabel("m[1,1]");
			gui2->addTextInput("m11", "0");
			gui2->addLabel("m[1,2]");
			gui2->addTextInput("m12", "0");
			gui2->addLabel("m[2,0]");
			gui2->addTextInput("m20", "0");
			gui2->addLabel("m[2,1]");
			gui2->addTextInput("m21", "0");
			gui2->addLabel("m[2,2]");
			gui2->addTextInput("m22", "0");
		}
	}
}

void ofApp::keyPressed(int key) {

}

void ofApp::keyReleased(int key) {

}

void saveXmlFile() {
	ofxXmlSettings settings;

	settings.addTag("settings");
	settings.pushTag("settings");

	settings.setValue("realTime", REAL_TIME);
	settings.setValue("realTimeFPS", REAL_TIME_FPS);
	settings.setValue("realTimePort", REAL_TIME_PORT);
	
	settings.addTag("cameras");
	settings.pushTag("cameras");

	for(int i = 0; i < cameras.size(); i++) {
		settings.addTag("camera");
		settings.pushTag("camera", i);

		settings.addValue("id", i);
		settings.addValue("resolutionX", cameras[i].ResolutionX);
		settings.addValue("resolutionY", cameras[i].ResolutionY);
		settings.addValue("resolutionDownSample", cameras[i].ResolutionDownSample);
		settings.addValue("FPS", cameras[i].FPS);
		settings.addValue("colorRGB", cameras[i].ColorRGB);
		settings.addValue("use2D", cameras[i].Use2D);
		settings.addValue("use3D", cameras[i].Use3D);
		settings.addValue("dataContext", cameras[i].DataContext);
		
		settings.addTag("depthSettings");
		settings.pushTag("depthSettings");
		settings.addValue("near", cameras[i].DepthSettings.Near);
		settings.addValue("far", cameras[i].DepthSettings.Far);
		settings.addValue("pointsDownSample", cameras[i].DepthSettings.PointsDownSample);

		settings.popTag();
		
		settings.addTag("matrix");
		settings.pushTag("matrix");
		for(int k = 0; k < sizeof(cameras[i].Matrix)/sizeof(*cameras[i].Matrix); k++) {
			std::stringstream cellM;
			cellM << "r" << k / 3 << k % 3;
			settings.addValue(cellM.str(), cameras[i].Matrix[i]);
		}

		settings.popTag();
		settings.popTag();
	}

	settings.popTag();
	settings.popTag();

	settings.saveFile("settings.xml");
}