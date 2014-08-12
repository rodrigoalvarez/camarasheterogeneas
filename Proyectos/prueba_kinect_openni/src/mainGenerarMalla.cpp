#include "mainGenerarMalla.h"


void mainGenerarMalla::setup() {
    ThreadData* data;
    data = FrameUtils::getDummyFrame();//Obtengo el frame vacio
    int downsampling = 1;

    char* fileName      = "frame_i.xyz";
    char* outputName    = "mallaFrame_i.ply";
    char* script        = "ScriptProyecto.mlx";

    //Creo el archivo b,n de la nube unida
    FILE * pFile;
    pFile = fopen (fileName,"w");
    //Recorro los frames de cada camara y me quedo solo con los 3D
    for (int i = 0; i<5; i++){
        if (data[i].state == 2 || data[i].state == 3){//Me fijo si el frame es 3D
            ofShortPixels& pixels = data[i].spix;
            XnDepthPixel* rawPix    = pixels.getPixels();
            xn::DepthGenerator & Xn_depth = data[i].Xn_depth;

            int step = 4;
            for(int y=0; y < pixels.getHeight(); y += downsampling) {
                for(int x=0; x < pixels.getWidth(); x += downsampling) {
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
        }
    }
    fclose (pFile);
    char linea[500];
    sprintf (linea, "\"C:\\Program Files\\VCG\\MeshLab\\meshlabserver\" -i %s -o %s -s %s -om vc vn", fileName, outputName, script);
    printf(linea);
    system(linea);/**/
}

//--------------------------------------------------------------
void mainGenerarMalla::update() {
    //printf("Hola");
    /*ofMesh mesh;
    for(int i=0; i<numONIFiles; i++) {
        oniData[i].update();
        if(oniData[i].isOn) {
            oniData[i].getMesh(mesh);
            ofImage img;
            ofPixels* pix       = oniData[i].getPixels();
            int downsampling = 1;
            if (guardarNube)
                oniData[i].getPointCloud("cloud2.xyz", downsampling);
            printf("%f\n",oniData[i].getPosition());
            unsigned char * p   = pix->getPixels();
            img.setFromPixels(p, pix->getWidth(), pix->getHeight(), OF_IMAGE_COLOR, true);
            if(cantImgs < 5) {
                cantImgs++;
                char buf[50];
                sprintf(buf, "imagen_%d.jpg", cantImgs); // puts string into buffer
                img.saveImage(ofToDataPath( buf ));
            }

        //oniData[i].getMeshLab("nube_A.xyz","nube_A_meshed.ply","ScriptProyecto.mlx");
        }
    }

    vbo.setMesh(mesh, GL_STATIC_DRAW);*/
}

//--------------------------------------------------------------
void mainGenerarMalla::draw() {

}

//--------------------------------------------------------------
void mainGenerarMalla::exit() {
}

//--------------------------------------------------------------
void mainGenerarMalla::keyPressed(int key) {
}

//--------------------------------------------------------------
void mainGenerarMalla::keyReleased(int key){

}

//--------------------------------------------------------------
void mainGenerarMalla::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void mainGenerarMalla::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void mainGenerarMalla::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void mainGenerarMalla::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void mainGenerarMalla::windowResized(int w, int h){

}
