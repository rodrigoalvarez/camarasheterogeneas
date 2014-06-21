#ifndef _CH_ONI_DATA
#define _CH_ONI_DATA

#include "ofxOpenNI.h"
//#include "ofxDelaunay.h"

class chONIData {
    public:
        char * oniName;
        float rotX;
        float rotY;
        float rotZ;
        float transX;
        float transY;
        float transZ;
        ofFloatColor    color;
        bool   playing;
        bool   isOn;
        ofxOpenNI player;
        //ofxDelaunay triangulator;

    void setup(char * name) {
        oniName = name;
        player.setup();
        player.start();
        player.startPlayer(oniName);
        player.setPaused(true);
        playing = false;
        isOn    = false;
        player.setUseDepthRawPixels(true);
        player.setMirror(true);
        //player.setUserSmoothing(true);
        //player.
        //Pongo valores por defecto
        rotX    = 0;
        rotY    = 0;
        rotZ    = 0;
        transX  = 0;
        transY  = 0;
        transZ  = 0;
        color   = ofFloatColor(256, 0, 0);
    }

    void update() {
        player.update();
        player.drawDebug();
        //printf(" player.isPaused(): %s, playing: %s", (player.isPaused()? "true": "false"), (playing? "true": "false"));
        if(!player.isPaused() && !playing) {
            player.setPaused(true);
        }

        if(player.isPaused() && playing) {
            player.setPaused(false);
        }
    }

    ofPixels getImagePixels() {
        return player.getImagePixels();
    }

    ofShortPixels& getDepthRawPixels() {
        return player.getDepthRawPixels();
    }

    void stop() {
        player.stop();
    }

    void setPaused(bool paused) {
        player.setPaused(paused);
    }

    bool isPaused() {
        return player.isPaused();
    }

    void setPosition(float pos){
        player.setPosition(pos);
    }

    float getPosition(){
        return player.getPosition();
    }

    void getMesh(ofMesh &mesh) {
        //triangulator.reset();
        if(isPaused()) {
            printf(" is paused ");
        }
        //ofMesh mesh;
        ofPixels cpixels        = player.getImagePixels();
        ofPixels rpix           = cpixels.getChannel(0);
        ofPixels gpix           = cpixels.getChannel(1);
        ofPixels bpix           = cpixels.getChannel(2);

        ofShortPixels& pixels   = player.getDepthRawPixels();
        ofPixels&    ipixels    = player.getImagePixels();
        XnDepthPixel* rawPix    = pixels.getPixels();

        int step = 4;
        float max = 3000;
        float min = 500;
        for(int y=0; y < 480; y += step) {
            for(int x=0; x < 640; x += step) {
                float d = rawPix[y * 640 + x];

                if((d > min) && (d < max)) {
                    ofVec3f vec = ofVec3f( x, y, (float)d);
                    vec.rotate(rotX, ofVec3f(1, 0, 0));
                    vec.rotate(rotY, ofVec3f(0, 1, 0));
                    vec.rotate(rotZ, ofVec3f(0, 0, 1));
                    vec.x += transX;
                    vec.y += transY;
                    vec.z += transZ;
                    mesh.addVertex(vec);
                    mesh.addColor(ipixels.getColor(x, y));
                    //triangulator.addPoint(x, y, d, &ipixels.getColor(x, y));
                } else {
                }
            }
        }
        //printf(" max %f ", max);
        //return mesh;
        //triangulator.triangulate();
    }

    void getPointCloud(char* file, int downsampling) {
        //triangulator.reset();
        if(isPaused()) {
            printf(" is paused ");
        }

        ofShortPixels& pixels   = player.getDepthRawPixels();
        ofPixels&    ipixels    = player.getImagePixels();
        XnDepthPixel* rawPix    = pixels.getPixels();
        xn::DepthGenerator & Xn_depth = player.getDepthGenerator();

        int step = 4;
        FILE * pFile;

        pFile = fopen (file,"w");
        for(int y=0; y < 480; y += downsampling) {
            for(int x=0; x < 640; x += downsampling) {
                float d = rawPix[y * 640 + x];
                if (d != 0.0){
                    ofVec3f vec = ofVec3f( x, y, (float)d);
                    XnPoint3D Point2D, Point3D;
                    Point2D.X = vec.x;
                    Point2D.Y = vec.y;
                    Point2D.Z = vec.z;
                    Xn_depth.ConvertProjectiveToRealWorld(1, &Point2D, &Point3D);
                    fprintf (pFile, "%f %f %f\n",Point3D.X,Point3D.Y,Point3D.Z);
                }
            }
        }
        fprintf (pFile, "Termino-%f",player.getTotalNumFrames());
        fclose (pFile);
    }

    void getMeshLab(char* input, char* output, char* script){
        char linea[500];
        sprintf (linea, "\"C:\\Program Files\\VCG\\MeshLab\\meshlabserver\" -i %s -o %s -s %s -om vc vn", input, output, script);
        printf(linea);
        system(linea);
    }

    ofPixels* getPixels() {
        ofPixels&    ipixels    = player.getImagePixels();
        return &ipixels;
    }

    void draw() {
        //triangulator.drawTriangles();
    }

};
#endif // RECORDONIVIDEOS_H
