#include<stdlib.h>
#include<windows.h>
#include<GL/glut.h>
#include "Model_PLY.h"
#include<iostream>

#include "FreeImage.h"
#include "matrix4x4.h"

using namespace std;

Model_PLY* model = NULL;
GLuint texObject, projTexture;
GLfloat vertices[][3]={{-1.0,-1.0,-1.0},{1.0,-1.0,-1.0},
          {1.0,1.0,-1.0},{-1.0,1.0,-1.0},{-1.0,-1.0,1.0},
           {1.0,-1.0,1.0},{1.0,1.0,1.0},{-1.0,1.0,1.0}};
GLfloat colors[][3]={{0.0,0.0,0.0},{1.0,0.0,0.0},
              {1.0,1.0,0.0},{0.0,1.0,0.0},{0.0,0.0,1.0},
              {1.0,0.0,1.0},{1.0,1.0,1.0},{0.0,1.0,1.0}};


void setVertex(int index) {
    GLfloat vert[3] = {model->Faces_Triangles[index * 3], model->Faces_Triangles[index * 3 + 1], model->Faces_Triangles[index * 3 + 2]};
    glVertex3fv(vert);
}

void triangle(int index)
{
    glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
		//glColor3fv(colors[0]);
		setVertex(index * 3);
		//glColor3fv(colors[1]);
		setVertex(index * 3 + 1);
		//glColor3fv(colors[2]);
		setVertex(index * 3 + 2);
	glEnd();
    glBegin(GL_LINE_LOOP);
        glColor3f(0.5f, 0.5f, 0.5f);
        setVertex(index * 3);
        glColor3f(0.5f, 0.5f, 0.5f);
        setVertex(index * 3 + 1);
        glColor3f(0.5f, 0.5f, 0.5f);
        setVertex(index * 3 + 2);
    glEnd();
}

void drawMesh()
{
    for (int i = 0; i < model->TotalFaces; i++) {
        triangle(i);
    }
}

static GLdouble viewer[] = {0.0,0.0,0.0};
static GLdouble oldViewer[] = {0.0,0.0,5.0};
Matrix4x4f TextureTransform;

float MVmatrix[16];

float rotateObjectX = 0.0f;
float rotateObjectY = 0.0f;
float rotateObjectZ = 0.0f;
float rotateTextureX = 0.0f;
float rotateTextureY = 0.0f;
float rotateTextureZ = 0.0f;

int cameraAxis = -1;
int cameraMove = -1;

bool modeTexture = true;

int TotalConnectedTriangles = 0;
int TotalConnectedPoints = 0;
int TotalFaces = 0;

void writeText()
{
    system("cls");
    cout << "Triangles: " << TotalConnectedTriangles << endl;
    cout << "Points: " << TotalConnectedPoints << endl;
    cout << "Faces: " << TotalFaces << endl << endl;
	cout << "Origin position..." << endl << viewer[0]  << " " << viewer[1]  << " " << viewer[2] << endl;
	cout << "Object rotate..." << endl << rotateObjectX  << " " << rotateObjectY  << " " << rotateObjectZ << endl;
	cout << "Texture rotate..." << endl << rotateTextureX  << " " << rotateTextureY  << " " << rotateTextureZ << endl;
}

void textureProjection(Matrix4x4f &mv)
{
	Matrix4x4f inverseMV = Matrix4x4f::invertMatrix(mv);

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glTranslatef(0.5f,0.5f,0.0f); //Bias
	glScalef(0.5f,0.5f,1.0f);		//Scale
	glFrustum(-0.035,0.035,-0.035,0.035,0.1,1.9); //MV for light map
	glTranslatef(0.0f,0.0f,-1.0f);
	glMultMatrixf(inverseMV.getMatrix());	//Inverse ModelView
	glMatrixMode(GL_MODELVIEW);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glLoadIdentity();
	if (modeTexture) {
        glTranslatef(viewer[0], viewer[1], viewer[2]);
        glRotatef(0, 1.0f,0.0f,0.0f);
        glRotatef(0, 0.0f,1.0f,0.0f);
        glRotatef(0, 0.0f,0.0f,1.0f);
	} else {
        glTranslatef(viewer[0], viewer[1], viewer[2] - 20);
        glRotatef(-rotateObjectX + rotateTextureX, 1.0f,0.0f,0.0f);
        glRotatef(-rotateObjectY + rotateTextureY, 0.0f,1.0f,0.0f);
        glRotatef(-rotateObjectZ + rotateTextureZ, 0.0f,0.0f,1.0f);
        glTranslatef(-viewer[0], -viewer[1], -viewer[2] + 20);
	}
	glGetFloatv(GL_MODELVIEW_MATRIX,MVmatrix);
	TextureTransform.setMatrix(MVmatrix);
	glPopMatrix();
	textureProjection(TextureTransform);

	glLoadIdentity();
	glTranslatef(viewer[0], viewer[1], viewer[2] - 20);
	glRotatef(rotateObjectX, -1.0f,0.0f,0.0f);
    glRotatef(rotateObjectY, 0.0f,-1.0f,0.0f);
    glRotatef(rotateObjectZ, 0.0f,0.0f,-1.0f);
	drawMesh();

	glFlush();
	glutSwapBuffers();

    writeText();
}

void keys(unsigned char key, int x, int y)
{
    if(key == 't') {
        modeTexture = true;
        rotateTextureX = 0;
        rotateTextureY = 0;
        rotateTextureZ = 0;
    }
    if(key == 'v') {
        modeTexture = false;
        rotateTextureX = rotateObjectX;
        rotateTextureY = rotateObjectY;
        rotateTextureZ = rotateObjectZ;
    }

	if(key == 'w') rotateObjectX += 2.0;
	if(key == 's') rotateObjectX -= 2.0;
	if(key == 'a') rotateObjectY += 2.0;
	if(key == 'd') rotateObjectY -= 2.0;
	if(key == 'e') rotateObjectZ += 2.0;
	if(key == 'q') rotateObjectZ -= 2.0;

	display();
}

void mouse(int btn, int state, int x, int y)
{
    cameraAxis = state == GLUT_DOWN ? btn : -1;
    if (state == GLUT_DOWN) {
        cameraMove = y;
        oldViewer[0] = viewer[0];
        oldViewer[1] = viewer[1];
        oldViewer[2] = viewer[2];
    }
    if (state == GLUT_UP) {
        cameraMove = -1;
    }

	display();
}

void mouseMove(int x, int y)
{
	if (cameraAxis != -1)
	{
		float deltaMove = (y - cameraMove) * 0.1f;
        if (cameraAxis == GLUT_LEFT_BUTTON) {
            viewer[0] = oldViewer[0] + deltaMove;
        } else if (cameraAxis == GLUT_RIGHT_BUTTON) {
            viewer[1] = oldViewer[1] + deltaMove;
        } else if (cameraAxis == GLUT_MIDDLE_BUTTON) {
            viewer[2] = oldViewer[2] + deltaMove;
        }
		display();
	}
}


void myReshape(int w, int h)
{
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(w<=h)
        glFrustum(-2.0,2.0,-2.0*(GLfloat)h/(GLfloat)w,2.0*(GLfloat)h/(GLfloat)w, 2.0,20.0);
	else
        glFrustum(-2.0,2.0,-2.0*(GLfloat)w/(GLfloat)h,2.0*(GLfloat)w/(GLfloat)h, 2.0,20.0);
	glMatrixMode(GL_MODELVIEW);
}

void loadLightMapTexture(const char *name)
{
	GLfloat eyePlaneS[] =  {1.0f, 0.0f, 0.0f, 0.0f};
	GLfloat eyePlaneT[] =  {0.0f, 1.0f, 0.0f, 0.0f};
	GLfloat eyePlaneR[] =  {0.0f, 0.0f, 1.0f, 0.0f};
	GLfloat eyePlaneQ[] =  {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat borderColor[4] = {0.6f, 0.6f, 0.6f, 1.0f};

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

	//set up texture generation mode and set the corresponding planes
	glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
	glTexGenfv(GL_S,GL_EYE_PLANE,eyePlaneS);
	glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
	glTexGenfv(GL_T,GL_EYE_PLANE,eyePlaneT);
	glTexGeni(GL_R,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
	glTexGenfv(GL_R,GL_EYE_PLANE,eyePlaneR);
	glTexGeni(GL_Q,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
	glTexGenfv(GL_Q,GL_EYE_PLANE,eyePlaneQ);

	glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glEnable(GL_TEXTURE_GEN_R);
    glEnable(GL_TEXTURE_GEN_Q);

    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
    FIBITMAP *dib(0);
    BYTE* bits(0);
    unsigned int width(0), height(0);
    GLuint gl_texID;

    fif = FreeImage_GetFileType(name, 0);
    if(fif == FIF_UNKNOWN)
        fif = FreeImage_GetFIFFromFilename(name);
    if(fif == FIF_UNKNOWN)
        printf("Formato de imagen no reconocido.");

    if(FreeImage_FIFSupportsReading(fif))
        dib = FreeImage_Load(fif, name);
    if(!dib)
        printf("Error al cargar la imagen.");

    bits = FreeImage_GetBits(dib);
    width = FreeImage_GetWidth(dib);
    height = FreeImage_GetHeight(dib);
    if((bits == 0) || (width == 0) || (height == 0))
        printf("Error en la imagen.");

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812D);
	glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,borderColor);
    gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGB,width,height,GL_RGB,GL_UNSIGNED_BYTE,bits);

	FreeImage_Unload(dib);
}

int main(int argc, char **argv)
{
    model = new Model_PLY();
    model->Load("test3.ply");
    TotalConnectedTriangles = model->TotalConnectedTriangles;
    TotalConnectedPoints = model->TotalConnectedPoints;
    TotalFaces = model->TotalFaces;

    writeText();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
	glutInitWindowSize(500,500);
	glutCreateWindow("Texture project");
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMove);
	//glutMouseWheelFunc(mouseWheel);
	glutKeyboardFunc(keys);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_TEXTURE_2D);

	loadLightMapTexture("lightmap.bmp");

	glutMainLoop();
}
