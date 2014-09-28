#include "testApp.h"


vector<string> getImageFiles() {
    vector<string> files;
    ofDirectory directory("C:\\of_v0073_win_cb_release\\apps\\myApps\\reproductorUnido\\img");
    directory.allowExt("png");
    directory.allowExt("bmp");
    directory.allowExt("jpg");
    directory.listDir();
    for(int i = 0; i < directory.numFiles(); i++) {
        files.push_back(directory.getPath(i));
    }
    return files;
}

vector<string> getMeshFiles() {
    vector<string> files;
    ofDirectory directory("C:\\of_v0073_win_cb_release\\apps\\myApps\\reproductorUnido\\mesh");
    directory.allowExt("ply");
    directory.listDir();
    for(int i = 0; i < directory.numFiles(); i++) {
        files.push_back(directory.getPath(i));
    }
    return files;
}

void testApp::setup() {

    ofSetFrameRate(60);
    ofEnableAlphaBlending();
    ofSetLogLevel(OF_LOG_VERBOSE);

    vector<string> files3D = getMeshFiles();
    ShareMesh* mesh = new ShareMesh(files3D[0]);
    meshes.push_back(mesh);

    vector<string> files2D = getImageFiles();
    for (int i = 0; i < files2D.size(); i++) {
        cout << files2D[i] << endl;
        ShareImage* image = new ShareImage(files2D[i]);
        images.push_back(image);
    }
}

void testApp::update() {

    if (ofGetFrameNum() % 300 == 0) {
        system("cls");
        cout << "MESH..." << (meshes[0]->isConnectedMeshId ? "YES" : "NO") << endl;
        if(!meshes[0]->isConnectedMeshId) {
            meshes[0]->isConnectedMeshId = meshes[0]->memoryMappedMeshId.connect();
            meshes[0]->isConnectedNFaces = meshes[0]->memoryMappedMeshSize.connect();
            meshes[0]->isConnectedFaces = meshes[0]->memoryMappedMesh.connect();
        }
        for (int i = 0; i < images.size(); i++) {
            cout << "IMAGE " << i << "..." << (images[i]->isConnectedImageId ? "YES" : "NO") << endl;
            if(!images[i]->isConnectedImageId) {
                images[i]->isConnectedImageId = images[i]->memoryMappedImageId.connect();
                images[i]->isConnectedWPixels = images[i]->memoryMappedImageSizeW.connect();
                images[i]->isConnectedHPixels = images[i]->memoryMappedImageSizeH.connect();
                images[i]->isConnectedPixels = images[i]->memoryMappedImage.connect();
            }
        }
	}
}

void testApp::draw() {

}
