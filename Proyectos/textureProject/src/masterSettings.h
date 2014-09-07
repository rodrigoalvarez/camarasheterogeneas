#ifndef MASTERSETTINGS_H
#define MASTERSETTINGS_H
#include "matrix4x4.h"

using namespace std;

struct MasterTexture {
    float viewer[3];
    float rotate[3];

    Matrix4x4f TextureTransform;
    float MVmatrix[16];
};

struct MasterMesh {
    float viewer[3];
    float rotate[3];
};


class MasterSettings
{
    public:
        MasterSettings(int textureCount, MasterTexture* textureMaster, int meshCount, MasterMesh* meshMaster);
        void loadTextureCalibration();
        void loadMeshCalibration();
        void saveTextureCalibration();
        void saveMeshCalibration();
    protected:
    private:
        int textureCount;
        MasterTexture* textureMaster;
        int meshCount;
        MasterMesh* meshMaster;
};

#endif // MASTERSETTINGS_H
