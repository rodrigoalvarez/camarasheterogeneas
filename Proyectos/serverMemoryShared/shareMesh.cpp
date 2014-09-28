#include "shareMesh.h"

bool isServer = true;

ShareMesh::ShareMesh(string filename) {

    meshId = new int;
    *meshId = 100001;

    mPly = new MasterPly();
    mPly->Sampling(filename.c_str());

    numberFaces = new int;
    *numberFaces = mPly->totalFaces();
    faces = mPly->getFaces();

    std::stringstream key1;
    key1 << "MeshId" << *meshId / 100000;
	memoryMappedMeshId.setup(key1.str(), sizeof(int), isServer);
    isConnectedMeshId = memoryMappedMeshId.connect();
    memoryMappedMeshId.setData(meshId);

    std::stringstream key2;
    key2 << "MeshNumberFaces" << *meshId / 100000;
	memoryMappedMeshSize.setup(key2.str(), sizeof(int), isServer);
    isConnectedNFaces = memoryMappedMeshSize.connect();
    memoryMappedMeshSize.setData(numberFaces);

    std::stringstream key3;
    key3 << "MeshFaces" << *meshId / 100000;
	memoryMappedMesh.setup(key3.str(), sizeof(FaceStruct) * (*numberFaces), isServer);
    isConnectedFaces = memoryMappedMesh.connect();
    memoryMappedMesh.setData(faces);
}
