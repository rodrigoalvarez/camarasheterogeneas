#ifndef TEXTUREMAIN_H
#define TEXTUREMAIN_H

#include "modelPly.h"
#include "modelImg.h"
#include <stdlib.h>
#include <windows.h>
#include "masterSettings.h"
#include "modelXYZ.h"
#include "ofxXmlSettings.h"

class TextureMain
{
    public:

        GLuint *textures;

        MasterSettings* settings;

        /* Texture */

        Model_IMG* textureImage;
        Model_PLY* textureModel;
        MasterTexture* textureMaster;
        bool textureViewMode;
        bool drawFast;
        bool textureWire;

        int textureCount;
        int textureIndex;

        int facesCount;
        int** faces;
        float frustum[6][4];

        /* Camera */

        int cameraAxis;
        int cameraMove;

        TextureMain();
        virtual ~TextureMain();

        void display(void);
        void keys(unsigned char key, int x, int y);
        void mouse(int btn, int state, int x, int y);
        void mouseMove(int x, int y);

        void loadLightMapTexture(Model_IMG* model, string file);

    protected:
    private:

        void writeText();
        void setFaceVertex(int index);
        void setPointVertex(int index);

        void draw2DElement(int index);
        void draw3D();
        void draw2DView();

        void ExtractFrustum();
        bool PointInFrustum(float x, float y, float z);

        void draw2DCalibrationFull();
        void draw2DCalibrationFast();

        void textureProjection(Matrix4x4f &mv);
        void stepTransformTexture();
        void stepTexture();
        void stepClearTexture();

        void IncludeMesh (Model_XYZ* model, Model_XYZ* newModel, MasterMesh master);

};

#endif // TEXTUREMAIN_H
