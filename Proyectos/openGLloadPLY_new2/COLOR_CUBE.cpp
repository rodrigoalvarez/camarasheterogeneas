
//#include <GL/glew.h>
#include<stdlib.h>
#include<windows.h>
#include<GL/glut.h>
#include <GL/glext.h>      // Sample's header file
#include "Model_PLY.h"

#include<iostream>

#include "FreeImage.h"
#include "matrix4x4.h"

using namespace std;

GLuint *textures = new GLuint[2];
PFNGLACTIVETEXTUREARBPROC       glActiveTextureARB       = NULL;
PFNGLMULTITEXCOORD2FARBPROC     glMultiTexCoord2fARB     = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB = NULL;

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
    //glBindTexture(GL_TEXTURE_2D, textures[0]);
    glColor3f(255.0f, 255.0f, 255.0f);
   //glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

	glBegin(GL_POLYGON);
		setVertex(index * 3);
		setVertex(index * 3 + 1);
		setVertex(index * 3 + 2);
	glEnd();
	//glBindTexture(GL_TEXTURE_2D, textures[1]);
	glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINE_LOOP);
		setVertex(index * 3);
		//glColor3f(0.0f, 0.0f, 1.0f);
		setVertex(index * 3 + 1);
		//glColor3f(0.0f, 0.0f, 1.0f);
		setVertex(index * 3 + 2);
	glEnd();
}

void colorcube()
{
    glActiveTextureARB(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures[0]); // your texture 1 here
    //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glActiveTextureARB(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures[1]); // your texture 0 here


    glActiveTextureARB(GL_TEXTURE2);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures[2]); // your texture 0 here
    /*glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD); // not standard, must be an extension
*/
    for (int i = 0; i < model->TotalFaces; i++) {
        triangle(i);
    }
    glActiveTextureARB(GL_TEXTURE0);
    glDisable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE1);
    glDisable(GL_TEXTURE_2D);
}

static GLfloat theta[]={0.0,0.0,0.0};
static GLint axis=2;
static GLdouble viewer[]={0.0,0.0,5.0};
static GLdouble oldViewer[]={0.0,0.0,5.0};
Matrix4x4f TextureTransform;
Matrix4x4f TextureTransform2;
float rotateObjectX = 0.0f;
float rotateObjectY = 0.0f;
float rotateTextureX = 0.0f;
float rotateTextureY = 0.0f;
float moveEyePoint = 0.0f;

float MVmatrix[16];
float MVmatrix2[16];

bool objectMove = false;
bool textureMove = false;
int lastX = -1;
int lastY = -1;

void textureProjection(Matrix4x4f &mv)
{
	Matrix4x4f inverseMV = Matrix4x4f::invertMatrix(mv);

	//here is where we do the transformations on the texture matrix for the lightmap
	//the basic equation here is M = Bias * Scale * ModelView for light map * Inverse Modelview
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
	glRotatef(rotateTextureX - rotateObjectX,1.0f,0.0f,0.0f);
	glRotatef(rotateTextureY - rotateObjectY,0.0f,1.0f,0.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX,MVmatrix);
	TextureTransform.setMatrix(MVmatrix);
	glPopMatrix();
	textureProjection(TextureTransform);

	glLoadIdentity();
	gluLookAt(viewer[0], viewer[1],viewer[2],0.0,0.0,0.0,0.0,1.0,0.0);
	glRotatef(theta[0],1.0,0.0,0.0);
	glRotatef(theta[1],0.0,1.0,0.0);
	glRotatef(theta[2],0.0,0.0,1.0);

	colorcube();

	glFlush();
	glutSwapBuffers();
}

void mouse(int btn, int state, int x, int y)
{
    if (btn == GLUT_LEFT_BUTTON) {
        objectMove = true;
        textureMove = false;
    }
    if (btn == GLUT_RIGHT_BUTTON) {
        objectMove = false;
        textureMove = true;
    }
    if (state == GLUT_DOWN) {
        lastX = x;
        lastY = y;
        oldViewer[0] = viewer[0];
        oldViewer[1] = viewer[1];
        oldViewer[2] = viewer[2];
        //cout << lastX << "  " << lastY << endl;
    }
    if (state == GLUT_UP) {
        lastX = -1;
        lastY = -1;
    }
	/*if(btn==GLUT_LEFT_BUTTON && state==GLUT_DOWN) axis=0;
	if(btn==GLUT_MIDDLE_BUTTON&&state==GLUT_DOWN) axis=1;
	if(btn==GLUT_RIGHT_BUTTON&&state==GLUT_DOWN) axis=2;
	theta[axis]+=2.0;
	if(theta[axis]>360.0) theta[axis]-=360.0;*/
	display();
}

void mouseMove(int x, int y)
{
	if (objectMove && lastX >= 0 && lastY >= 0)
	{
		float deltaX = (x - lastX) * 0.1f;
		float deltaY = (y - lastY) * 0.1f;
        viewer[0] = oldViewer[0] + deltaX;
        viewer[1] = oldViewer[1] + deltaY;
        viewer[2] = oldViewer[2];
		display();
	}
}

void keys(unsigned char key, int x, int y)
{

	if(key == 'a') rotateTextureY += 1.0;
	if(key == 'd') rotateTextureY -= 1.0;
	if(key == 's') rotateTextureX -= 1.0;
	if(key == 'w') rotateTextureX += 1.0;

	if(key == 'y')     glActiveTextureARB(GL_TEXTURE0);
	if(key == 'u')     glActiveTextureARB(GL_TEXTURE1);
	if(key == 'i')     glActiveTextureARB(GL_TEXTURE2);

	if(key == '6') viewer[0] += 1.0;
	if(key == '4') viewer[0] -= 1.0;
	if(key == '8') viewer[1] += 1.0;
	if(key == '2') viewer[1] -= 1.0;
	if(key == '-') viewer[2] += 1.0;
	if(key == '+') viewer[2] -= 1.0;

	display();
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
    GLfloat borderColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};

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
    int TotalConnectedTriangles = model->TotalConnectedTriangles;
    int TotalConnectedPoints = model->TotalConnectedPoints;
    int TotalFaces = model->TotalFaces;

    cout << TotalConnectedTriangles << " - " << TotalConnectedPoints << " - " << TotalFaces << endl;


	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
	glutInitWindowSize(500,500);
	glutCreateWindow("Colorcube Viewer");
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMove);
	glutKeyboardFunc(keys);
	glEnable(GL_DEPTH_TEST);

    /*glewInit();GLenum err = glewInit();
    if (GLEW_OK != err)
    {
      /* Problem: glewInit failed, something is seriously wrong.
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));*/

    glGenTextures(3, textures);

    glActiveTextureARB       = (PFNGLCLIENTACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
    glMultiTexCoord2fARB     = (PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress("glMultiTexCoord2fARB");
    glClientActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glClientActiveTextureARB");

    if( !glActiveTextureARB || !glMultiTexCoord2fARB || !glClientActiveTextureARB )
    {
        MessageBox(NULL,"One or more GL_ARB_multitexture functions were not found",
            "ERROR",MB_OK|MB_ICONEXCLAMATION);
        return -1;
    }


    glActiveTextureARB(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
	loadLightMapTexture("lightmap.bmp");

    glActiveTextureARB(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
	loadLightMapTexture("lightmap2.JPG");

    glActiveTextureARB(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textures[2]);
	loadLightMapTexture("lightmap3.JPG");


	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glEnable(GL_TEXTURE_2D);



	glutMainLoop();
}
/* Tengo que ver como seleccionar la imagen a mover y que se mueva esa y no otra*/
