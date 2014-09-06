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
array<ofxUILabelButton*,3> allTabs;

void getCameraTabs (ofxUIWidget* i) {
	if (i->getName() == "CAMERA 0") {
		allTabs[0] = (ofxUILabelButton*)i;
	}
	if (i->getName() == "CAMERA 1") {
		allTabs[1] = (ofxUILabelButton*)i;
	}
	if (i->getName() == "CAMERA 2") {
		allTabs[2] = (ofxUILabelButton*)i;
	}
}

void loadXmlFile();

void ofApp::setup() {

	for (int i = 0; i < allCameras.size(); i++) {
		allCameras[i] = NULL;
		allMatrix[i] = NULL;
		cameras[i].ResolutionX = 800;
		cameras[i].ResolutionY = 600;
		cameras[i].ResolutionDownSample = 1;
		cameras[i].ColorRGB = true;
		cameras[i].FPS = 24;
		cameras[i].Use2D = false;
		cameras[i].Use3D = false;
		cameras[i].DataContext = "";
		cameras[i].DepthSettings.Near = 0;
		cameras[i].DepthSettings.Far = 0;
		cameras[i].DepthSettings.PointsDownSample = 0;
		for (int k = 0; k < 16; k++) {
			cameras[i].Matrix[k] = 0;
		}
	}

	ofSetVerticalSync(true); 
	ofEnableSmoothing(); 
    
    gui = new ofxUISuperCanvas("XML GENERATOR");
	gui->setHeight(720);

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
	
    gui->addSpacer();
	gui->addLabelButton("CAMERA 0", false);
	gui->addLabelButton("CAMERA 1", false);
	gui->addLabelButton("CAMERA 2", false);

	vector<ofxUIWidget*> widgets = gui->getWidgets();
	for_each (widgets.begin(), widgets.end(), getCameraTabs);

	allTabs[0]->setVisible(false);
	allTabs[1]->setVisible(false);
	allTabs[2]->setVisible(false);

    gui->autoSizeToFitWidgets();
    ofAddListener(gui->newGUIEvent,this,&ofApp::guiEvent);
    gui->loadSettings("guiSettings.xml");
	gui->setHeight(720);

	loadXmlFile();

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
		if (canvasName == "CAMERA 0") {
			cameras[0].ResolutionX = ofToInt(widget->getTextString());
		}
		if (canvasName == "CAMERA 1") {
			cameras[1].ResolutionX = ofToInt(widget->getTextString());
		}
		if (canvasName == "CAMERA 2") {
			cameras[2].ResolutionX = ofToInt(widget->getTextString());
		}
	}
	if(name == "resolutionYValue")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "CAMERA 0") {
			cameras[0].ResolutionY = ofToInt(widget->getTextString());
		}
		if (canvasName == "CAMERA 1") {
			cameras[1].ResolutionY = ofToInt(widget->getTextString());
		}
		if (canvasName == "CAMERA 2") {
			cameras[2].ResolutionY = ofToInt(widget->getTextString());
		}
	}
	if(name == "resolutionDownSampleValue")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "CAMERA 0") {
			cameras[0].ResolutionDownSample = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA 1") {
			cameras[1].ResolutionDownSample = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA 2") {
			cameras[2].ResolutionDownSample = ofToDouble(widget->getTextString());
		}
	}
	if(name == "colorRGB")
	{
		ofxUIToggle* widget = (ofxUIToggle*) e.widget;
		if (canvasName == "CAMERA 0") {
			cameras[0].ColorRGB = widget->getValue() == 1;
		}
		if (canvasName == "CAMERA 1") {
			cameras[1].ColorRGB = widget->getValue() == 1;
		}
		if (canvasName == "CAMERA 2") {
			cameras[2].ColorRGB = widget->getValue() == 1;
		}
	}
	if(name == "FPSValue")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "CAMERA 0") {
			cameras[0].FPS = ofToInt(widget->getTextString());
		}
		if (canvasName == "CAMERA 1") {
			cameras[0].FPS = ofToInt(widget->getTextString());
		}
		if (canvasName == "CAMERA 2") {
			cameras[0].FPS = ofToInt(widget->getTextString());
		}
	}

	if(name == "3D")
	{
		ofxUIToggle* widget = (ofxUIToggle*) e.widget;
		if (canvasName == "CAMERA 0") {
			cameras[0].Use3D = widget->getValue() == 1;
		}
		if (canvasName == "CAMERA 1") {
			cameras[1].Use3D = widget->getValue() == 1;
		}
		if (canvasName == "CAMERA 2") {
			cameras[2].Use3D = widget->getValue() == 1;
		}
	}
	if(name == "2D")
	{
		ofxUIToggle* widget = (ofxUIToggle*) e.widget;
		if (canvasName == "CAMERA 0") {
			cameras[0].Use2D = widget->getValue() == 1;
		}
		if (canvasName == "CAMERA 1") {
			cameras[1].Use2D = widget->getValue() == 1;
		}
		if (canvasName == "CAMERA 2") {
			cameras[2].Use2D = widget->getValue() == 1;
		}
	}

	if(name == "dataContextValue")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "CAMERA 0") {
			cameras[0].DataContext = widget->getTextString();
		}
		if (canvasName == "CAMERA 1") {
			cameras[1].DataContext = widget->getTextString();
		}
		if (canvasName == "CAMERA 2") {
			cameras[2].DataContext = widget->getTextString();
		}
	}
	if(name == "nearValue")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "CAMERA 0") {
			cameras[0].DepthSettings.Near = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA 1") {
			cameras[1].DepthSettings.Near = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA 2") {
			cameras[2].DepthSettings.Near = ofToDouble(widget->getTextString());
		}
	}
	if(name == "farValue")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "CAMERA 0") {
			cameras[0].DepthSettings.Far = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA 1") {
			cameras[1].DepthSettings.Far = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA 2") {
			cameras[2].DepthSettings.Far = ofToDouble(widget->getTextString());
		}
	}
	if(name == "pointsDownSampleValue")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "CAMERA 0") {
			cameras[0].DepthSettings.PointsDownSample = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA 1") {
			cameras[1].DepthSettings.PointsDownSample = ofToDouble(widget->getTextString());
		}
		if (canvasName == "CAMERA 2") {
			cameras[2].DepthSettings.PointsDownSample = ofToDouble(widget->getTextString());
		}
	}
	
	if(name == "m00")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "MATRIX 0") {
			cameras[0].Matrix[0] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "MATRIX 1") {
			cameras[1].Matrix[0] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "MATRIX 2") {
			cameras[2].Matrix[0] = ofToDouble(widget->getTextString());
		}
	}
	if(name == "m01")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "MATRIX 0") {
			cameras[0].Matrix[1] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "MATRIX 1") {
			cameras[1].Matrix[1] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "MATRIX 2") {
			cameras[2].Matrix[1] = ofToDouble(widget->getTextString());
		}
	}
	if(name == "m02")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "MATRIX 0") {
			cameras[0].Matrix[2] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "MATRIX 1") {
			cameras[1].Matrix[2] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "MATRIX 2") {
			cameras[2].Matrix[2] = ofToDouble(widget->getTextString());
		}
	}
	if(name == "m03")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "MATRIX 0") {
			cameras[0].Matrix[3] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "MATRIX 1") {
			cameras[1].Matrix[3] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "MATRIX 2") {
			cameras[2].Matrix[3] = ofToDouble(widget->getTextString());
		}
	}
	if(name == "m10")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "MATRIX 0") {
			cameras[0].Matrix[4] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "MATRIX 1") {
			cameras[1].Matrix[4] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "MATRIX 2") {
			cameras[2].Matrix[4] = ofToDouble(widget->getTextString());
		}
	}
	if(name == "m11")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "MATRIX 0") {
			cameras[0].Matrix[5] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "MATRIX 1") {
			cameras[1].Matrix[5] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "MATRIX 2") {
			cameras[2].Matrix[5] = ofToDouble(widget->getTextString());
		}
	}
	if(name == "m12")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "MATRIX 0") {
			cameras[0].Matrix[6] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "MATRIX 1") {
			cameras[1].Matrix[6] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "MATRIX 2") {
			cameras[2].Matrix[6] = ofToDouble(widget->getTextString());
		}
	}
	if(name == "m13")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "MATRIX 0") {
			cameras[0].Matrix[7] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "MATRIX 1") {
			cameras[1].Matrix[7] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "MATRIX 2") {
			cameras[2].Matrix[7] = ofToDouble(widget->getTextString());
		}
	}
	if(name == "m20")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "MATRIX 0") {
			cameras[0].Matrix[8] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "MATRIX 1") {
			cameras[1].Matrix[8] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "MATRIX 2") {
			cameras[2].Matrix[8] = ofToDouble(widget->getTextString());
		}
	}
	if(name == "m21")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "MATRIX 0") {
			cameras[0].Matrix[9] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "MATRIX 1") {
			cameras[1].Matrix[9] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "MATRIX 2") {
			cameras[2].Matrix[9] = ofToDouble(widget->getTextString());
		}
	}
	if(name == "m22")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "MATRIX 0") {
			cameras[0].Matrix[10] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "MATRIX 1") {
			cameras[1].Matrix[10] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "MATRIX 2") {
			cameras[2].Matrix[10] = ofToDouble(widget->getTextString());
		}
	}
	if(name == "m23")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "MATRIX 0") {
			cameras[0].Matrix[11] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "MATRIX 1") {
			cameras[1].Matrix[11] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "MATRIX 2") {
			cameras[2].Matrix[11] = ofToDouble(widget->getTextString());
		}
	}
	if(name == "m30")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "MATRIX 0") {
			cameras[0].Matrix[12] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "MATRIX 1") {
			cameras[1].Matrix[12] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "MATRIX 2") {
			cameras[2].Matrix[12] = ofToDouble(widget->getTextString());
		}
	}
	if(name == "m31")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "MATRIX 0") {
			cameras[0].Matrix[13] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "MATRIX 1") {
			cameras[1].Matrix[13] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "MATRIX 2") {
			cameras[2].Matrix[13] = ofToDouble(widget->getTextString());
		}
	}
	if(name == "m32")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "MATRIX 0") {
			cameras[0].Matrix[14] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "MATRIX 1") {
			cameras[1].Matrix[14] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "MATRIX 2") {
			cameras[2].Matrix[14] = ofToDouble(widget->getTextString());
		}
	}
	if(name == "m33")
	{
		ofxUITextInput* widget = (ofxUITextInput*) e.widget;
		if (canvasName == "MATRIX 0") {
			cameras[0].Matrix[15] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "MATRIX 1") {
			cameras[1].Matrix[15] = ofToDouble(widget->getTextString());
		}
		if (canvasName == "MATRIX 2") {
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
			ofxUISuperCanvas* gui1 = NULL;
			ofxUISuperCanvas* gui2 = NULL;
			int i = 0;
			for (; i < allCameras.size(); i++) {
				if (allCameras[i] == NULL) {
					gui1 = new ofxUISuperCanvas("CAMERA " + ofToString(i));
					gui1->setPosition(250, 0);
					gui1->setVisible(false);
					allCameras[i] = gui1;

					gui1->autoSizeToFitWidgets();
					ofAddListener(gui1->newGUIEvent,this,&ofApp::guiEvent);
					gui1->loadSettings("guiSettings.xml");

					gui2 = new ofxUISuperCanvas("MATRIX " + ofToString(i));
					gui2->setPosition(480, 0);
					gui2->setVisible(false);
					allMatrix[i] = gui2;
					
					gui2->autoSizeToFitWidgets();
					ofAddListener(gui2->newGUIEvent,this,&ofApp::guiEvent);
					gui2->loadSettings("guiSettings.xml");

					allTabs[i]->setVisible(true);
					break;
				}
			}
			if (gui1 == NULL) {
				return;
			}
			gui->setHeight(720);
			gui1->setHeight(720);
			gui2->setHeight(720);

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
			gui2->addTextInput("m00", ofToString(cameras[i].Matrix[0]));
			gui2->addLabel("m[0,1]");
			gui2->addTextInput("m01", ofToString(cameras[i].Matrix[1]));
			gui2->addLabel("m[0,2]");
			gui2->addTextInput("m02", ofToString(cameras[i].Matrix[2]));
			gui2->addLabel("m[0,3]");
			gui2->addTextInput("m03", ofToString(cameras[i].Matrix[3]));
			gui2->addLabel("m[1,0]");
			gui2->addTextInput("m10", ofToString(cameras[i].Matrix[4]));
			gui2->addLabel("m[1,1]");
			gui2->addTextInput("m11", ofToString(cameras[i].Matrix[5]));
			gui2->addLabel("m[1,2]");
			gui2->addTextInput("m12", ofToString(cameras[i].Matrix[6]));
			gui2->addLabel("m[1,3]");
			gui2->addTextInput("m13", ofToString(cameras[i].Matrix[7]));
			gui2->addLabel("m[2,0]");
			gui2->addTextInput("m20", ofToString(cameras[i].Matrix[8]));
			gui2->addLabel("m[2,1]");
			gui2->addTextInput("m21", ofToString(cameras[i].Matrix[9]));
			gui2->addLabel("m[2,2]");
			gui2->addTextInput("m22", ofToString(cameras[i].Matrix[10]));
			gui2->addLabel("m[2,3]");
			gui2->addTextInput("m23", ofToString(cameras[i].Matrix[11]));
			gui2->addLabel("m[3,0]");
			gui2->addTextInput("m30", ofToString(cameras[i].Matrix[12]));
			gui2->addLabel("m[3,1]");
			gui2->addTextInput("m31", ofToString(cameras[i].Matrix[13]));
			gui2->addLabel("m[3,2]");
			gui2->addTextInput("m32", ofToString(cameras[i].Matrix[14]));
			gui2->addLabel("m[3,3]");
			gui2->addTextInput("m33", ofToString(cameras[i].Matrix[15]));
		}
	}
	
	if(name == "CAMERA 0")
	{
		ofxUILabelButton* widget = (ofxUILabelButton*) e.widget;
		if (widget->getValue() == 1) {
			ofxUISuperCanvas* gui1 = allCameras[0];
			ofxUISuperCanvas* gui2 = allMatrix[0];
			gui1->setVisible(true);
			gui2->setVisible(true);
			gui1 = allCameras[1];
			gui2 = allMatrix[1];
			if (gui1 != NULL) {
				gui1->setVisible(false);
				gui2->setVisible(false);
			}
			gui1 = allCameras[2];
			gui2 = allMatrix[2];
			if (gui1 != NULL) {
				gui1->setVisible(false);
				gui2->setVisible(false);
			}
		}
	}
	if(name == "CAMERA 1")
	{
		ofxUILabelButton* widget = (ofxUILabelButton*) e.widget;
		if (widget->getValue() == 1) {
			ofxUISuperCanvas* gui1 = allCameras[0];
			ofxUISuperCanvas* gui2 = allMatrix[0];
			gui1->setVisible(false);
			gui2->setVisible(false);
			gui1 = allCameras[1];
			gui2 = allMatrix[1];
			gui1->setVisible(true);
			gui2->setVisible(true);
			gui1 = allCameras[2];
			gui2 = allMatrix[2];
			if (gui1 != NULL) {
				gui1->setVisible(false);
				gui2->setVisible(false);
			}
		}
	}
	if(name == "CAMERA 2")
	{
		ofxUILabelButton* widget = (ofxUILabelButton*) e.widget;
		if (widget->getValue() == 1) {
			ofxUISuperCanvas* gui1 = allCameras[0];
			ofxUISuperCanvas* gui2 = allMatrix[0];
			gui1->setVisible(false);
			gui2->setVisible(false);
			gui1 = allCameras[1];
			gui2 = allMatrix[1];
			gui1->setVisible(false);
			gui2->setVisible(false);
			gui1 = allCameras[2];
			gui2 = allMatrix[2];
			gui1->setVisible(true);
			gui2->setVisible(true);
		}
	}
}

void ofApp::keyPressed(int key) {

}

void ofApp::keyReleased(int key) {

}

void loadXmlFile() {

	string path = ofFilePath::getCurrentWorkingDirectory() + "data";
	ofDirectory dir2D(path);
	dir2D.allowExt("matrix");
	dir2D.listDir();
	int count = min(3, dir2D.numFiles());
	for (int i = 0; i < count; i++) {
		ofBuffer buffer = ofBufferFromFile(dir2D.getPath(i));
		vector<string> lines = ofSplitString(buffer.getText(), "\n");
		for (int k1 = 0; k1 < 4; k1++) {
			for (int k2 = 0; k2 < 4; k2++) {
				vector<string> numbers = ofSplitString(lines[k1], " ");
				cameras[i].Matrix[k1 * 4 + k2] = ofToDouble(numbers[k2]);
			}
		}
	}
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

	for(int i = 0; i < cameras.size() && allCameras[i] != NULL; i++) {
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
			cellM << "r" << k / 4 << k % 4;
			settings.addValue(cellM.str(), cameras[i].Matrix[k]);
		}

		settings.popTag();
		settings.popTag();
	}

	settings.popTag();
	settings.popTag();

	settings.saveFile("settings.xml");
}