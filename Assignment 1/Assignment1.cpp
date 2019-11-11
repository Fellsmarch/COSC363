//  ========================================================================
//  COSC363: Computer Graphics (2019); CSSE  University of Canterbury.
//
//  FILE NAME: Assignment1.cpp
//  Author: Harrison Cook (hgc25)
//  ========================================================================

#include <iostream>
#include <GL/freeglut.h>
#include <cmath>
#include <fstream>
#include <climits>
#include <math.h>
#include "loadTGA.h"
using namespace std;

#define FLOOR_HEIGHT 499 //The base height of the floor
#define GL_CLAMP_TO_EDGE 0x812F   //To get rid of seams between textures
#define NUM_TEXTURES 10
#define MAX_PARTICLES 1000

//Spaceship variables
float movingTexLge = 1.5;
float movingTexZro = 0.5;
float spaceShipHeight = 0.0;
float spaceShipRotateX = 0;
float spaceShipRotateY = 0;
float spaceShipRotateZ = 0;
float spaceShipRotateEnabled = 0;
int spaceShipTakenOff = 0;

//Cannon variables
float *x, *y, *z;  //vertex coordinate arrays
int *t1, *t2, *t3; //triangles
int nvrt, ntri;    //total number of vertices and triangles
float cannonBallX = 0, cannonBallY = 0;

//Floating Robot Variables
float roboForward = 90;
float roboY = 0.0;
float roboZ = 0.0;

//Camera parameters
float lookAngle=0;
float eye_x = 5 * sin(lookAngle), eye_z = 5 * cos(lookAngle);
float look_x = eye_x + 100 * sin(lookAngle), look_z = eye_z - 100 * cos(lookAngle);
float cameraHeight = 505;
int spaceShipView = 0;
float degToRad = 3.14159265 / 180.0; //Conversion from degrees to radians

GLuint texId[NUM_TEXTURES];
float limbsTheta = 20.0;
float patrolTheta = 0.0;

//Particles
typedef struct {
    float life, deathSpeed;
    float x, y, z;
    float dirX, dirY, dirZ;
    int constrained;
} Particle;
Particle particles[MAX_PARTICLES]; //Array of particles
int cannonFired = 0;
int stopSmoke = 0;

void loadGLTextures()
{
    glGenTextures(NUM_TEXTURES, texId); // Create texture ids

    // *** left ***
    glBindTexture(GL_TEXTURE_2D, texId[0]);
    loadTGA("Textures/Skybox/left.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // *** front ***
    glBindTexture(GL_TEXTURE_2D, texId[1]);
    loadTGA("Textures/Skybox/front.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // *** right ***
    glBindTexture(GL_TEXTURE_2D, texId[2]);
    loadTGA("Textures/Skybox/right.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // *** back***
    glBindTexture(GL_TEXTURE_2D, texId[3]);
    loadTGA("Textures/Skybox/back.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // *** top ***
    glBindTexture(GL_TEXTURE_2D, texId[4]);
    loadTGA("Textures/Skybox/up.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // *** down ***
    glBindTexture(GL_TEXTURE_2D, texId[5]);
    loadTGA("Textures/Skybox/down.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // *** floor ***
    glBindTexture(GL_TEXTURE_2D, texId[6]);
    //loadTGA("Floor.tga");
    loadTGA("Textures/Floor-Sand.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    // *** Castle walls ***
    glBindTexture(GL_TEXTURE_2D, texId[7]);
    loadTGA("Textures/Desert-Castle-Wall.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    // *** Crenellation ***
    glBindTexture(GL_TEXTURE_2D, texId[8]);
    loadTGA("Textures/Desert-Castle-Wall.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    // *** Spaceship ***
    glBindTexture(GL_TEXTURE_2D, texId[9]);
    loadTGA("Textures/Metal-Texture.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
}

//-- Loads mesh data in OFF format    -------------------------------------
void loadMeshFile(const char* fname)
{
    ifstream fp_in;
    int num, ne;

    fp_in.open(fname, ios::in);
    if(!fp_in.is_open())
    {
        cout << "Error opening mesh file" << endl;
        exit(1);
    }

    fp_in.ignore(INT_MAX, '\n');                //ignore first line
    fp_in >> nvrt >> ntri >> ne;                // read number of vertices, polygons, edges

    x = new float[nvrt];                        //create arrays
    y = new float[nvrt];
    z = new float[nvrt];

    t1 = new int[ntri];
    t2 = new int[ntri];
    t3 = new int[ntri];

    for(int i=0; i < nvrt; i++)                         //read vertex list
        fp_in >> x[i] >> y[i] >> z[i];

    for(int i=0; i < ntri; i++)                         //read polygon list
    {
        fp_in >> num >> t1[i] >> t2[i] >> t3[i];
        if(num != 3)
        {
            cout << "ERROR: Polygon with index " << i  << " is not a triangle." << endl;  //not a triangle!!
            exit(1);
        }
    }

    fp_in.close();
    cout << " File successfully read." << endl;
}

//---------------------------------------------------------------------
void patrolTimer(int value)
{
    patrolTheta -= 0.2; //Walking robots

    //Floating robots:

    float speed = 0.2;
    static int up = 1;
    static int rotating = 0;
    static int forwards = 0;

    if (roboForward > 0 && !rotating) {
        roboZ += speed;
        if (roboZ >= 27) {
            rotating = !rotating;
        }
    } else if (roboForward < 0 && !rotating) {
        roboZ -= speed;
        if (roboZ <= 0) {
            rotating = !rotating;
        }
    } else {
        if (forwards) {
            roboForward += 5;
        } else {
            roboForward -= 5;
        }

        if (roboForward == 90 || roboForward == -90) {
            rotating = !rotating;
            forwards = !forwards;
        }
    }

    if (up) {
        roboY += speed / 7;
        if (roboY >= 1) {
            up = !up;
        }
    } else {
        roboY -= speed / 7;
        if (roboY <= 0) {
            up = !up;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(50, patrolTimer, 0);
}

//---------------------------------------------------------------------
void textureTimer(int value)
{
    static int inwards = 1;
    float increment = 0.006;

    if (movingTexLge > 3.0 || movingTexLge < 2.0) { //Slow down when coming to switch
        increment /= 1.5;
    }

    if (movingTexLge > 3.4 || movingTexLge < 1.6) { //Slow down even more when coming to switch
        increment /= 1.5;
    }

    if (inwards) {
        movingTexLge += increment;
        movingTexZro -= increment;
    } else {
        movingTexLge -= increment;
        movingTexZro += increment;
    }


    if (movingTexLge >= 3.5) {
        inwards = !inwards;
    } else if (movingTexLge <= 1.5) {
        inwards = !inwards;
    }

    glutPostRedisplay();
    glutTimerFunc(50, textureTimer, 0);
}

//---------------------------------------------------------------------
void limbsMovementTimer(int value)
{
    static int movingForward = 0;

    if (movingForward == 1) {
        limbsTheta++;
        if (limbsTheta == 20) {
            movingForward = 0;
        }
    } else {
        limbsTheta--;
        if(limbsTheta == -20) {
            movingForward = 1;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(50, limbsMovementTimer, 0);
}

//---------------------------------------------------------------------
void fireCannonTimer(int value)
{
    static int iteration = 0;

    cannonBallX += 2.5;

    if (iteration < 100) cannonBallY += 1;
    else cannonBallY -= 1;

    iteration++;

    glutPostRedisplay();
    glutTimerFunc(50, fireCannonTimer, 0);
}

//---------------------------------------------------------------------
void spaceShipTakeOffTimer(int value)
{
    static int iteration = 0;
    static float speedX = (rand() % 360) / 200;
    static float speedY = (rand() % 360) / 200;
    static float speedZ = (rand() % 360) / 200;
    float slowSpeed = 0.1;

    if (iteration < 300) {
        spaceShipHeight += slowSpeed;
    }

    if (iteration >= 295 && spaceShipRotateEnabled) {
        spaceShipRotateX += speedX;
        spaceShipRotateY += speedY;
        spaceShipRotateZ += speedZ;
    }

    if (iteration >= 450) {
        spaceShipHeight *= 1.05;
    }

    iteration++;

    glutPostRedisplay();
    glutTimerFunc(20, spaceShipTakeOffTimer, 0);
}

//---------------------------------------------------------------------
void stopSmokeFunc(int value) {
    stopSmoke = 1;
}

//---------------------------------------------------------------------
void keyboardFunction(unsigned char key, int x, int y)
{
    switch(key) {
        case 's': glutTimerFunc(20, spaceShipTakeOffTimer, 0); spaceShipTakenOff = 1; break;
        case 'c': glutTimerFunc(50, fireCannonTimer, 0); cannonFired = 1; glutTimerFunc(1000, stopSmokeFunc, 0); break;
        case 'r': spaceShipRotateEnabled = !spaceShipRotateEnabled; break; //Toggle the spaceship rotating
        case 'e': spaceShipRotateX = 0; spaceShipRotateY = 0; spaceShipRotateZ = 0; break; //Set the rotation of the spaceship back to 0, 0, 0
        case 'p': stopSmoke = !stopSmoke; break;
    }
}

//=======================Particles======================================
float randomDirection()
{
    return float((rand() % 60) - 30) / 2000;
}

Particle generateParticle() {
    Particle particle;
    particle.life = 1.0;
    particle.deathSpeed = (rand() % 90) / 1000.0 + 0.01;
    particle.x = 0;
    particle.y = 0;
    particle.z = 0;
    particle.dirX = randomDirection();
    particle.dirY = float(rand() % 71) / 700;   //Different since I want them to move up faster
    particle.dirZ = randomDirection();
    particle.constrained = !(rand() % 10);      //Whether the particle should be constrained to a certain area

    return particle;
}

void drawParticles()
{
    glColor3f(0.4, 0.4, 0.4);
        for (int i = 0; i < MAX_PARTICLES; i++) {
            if (particles[i].life > 0) {
                glPushMatrix();
                    glTranslatef(particles[i].x, particles[i].y, particles[i].z);
                    glutSolidSphere(0.05, 5, 5);
                glPopMatrix();
            }
        }
    glColor3f(1, 1, 1);
}

void updateParticles()
{
    static float speedMod = 0.4;
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].life > 0) {
            particles[i].x += particles[i].dirX * speedMod;
            particles[i].y += particles[i].dirY * speedMod;
            particles[i].z += particles[i].dirZ * speedMod;
            particles[i].life -= particles[i].deathSpeed * speedMod;

            if (particles[i].constrained) {
                if (particles[i].x > 1 || particles[i].x < -1) {
                    particles[i].dirX = -particles[i].dirX;
                }

                if (particles[i].z > 1 || particles[i].z < -1) {
                    particles[i].dirZ = -particles[i].dirZ;
                }
            }
        }
    }
}

void generateNewParticles() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].life <= 0) {
            if (!(rand() % 10)) { //One in ten chance
                particles[i] = generateParticle();
            }
        }
    }
}

//======================================================================

void skybox()
{
    glEnable(GL_TEXTURE_2D);

    ////////////////////// LEFT WALL ///////////////////////
    glBindTexture(GL_TEXTURE_2D, texId[0]);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex3f(-1000,  0, 1000);
        glTexCoord2f(1.0, 0.0); glVertex3f(-1000, 0., -1000);
        glTexCoord2f(1.0, 1.0); glVertex3f(-1000, 1000., -1000);
        glTexCoord2f(0.0, 1.0); glVertex3f(-1000, 1000, 1000);
    glEnd();

    ////////////////////// FRONT WALL ///////////////////////
    glBindTexture(GL_TEXTURE_2D, texId[1]);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex3f(-1000,  0, -1000);
        glTexCoord2f(1.0, 0.0); glVertex3f(1000, 0., -1000);
        glTexCoord2f(1.0, 1.0); glVertex3f(1000, 1000, -1000);
        glTexCoord2f(0.0, 1.0); glVertex3f(-1000,  1000, -1000);
    glEnd();

    ////////////////////// RIGHT WALL ///////////////////////
    glBindTexture(GL_TEXTURE_2D, texId[2]);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex3f(1000,  0, -1000);
        glTexCoord2f(1.0, 0.0); glVertex3f(1000, 0, 1000);
        glTexCoord2f(1.0, 1.0); glVertex3f(1000, 1000,  1000);
        glTexCoord2f(0.0, 1.0); glVertex3f(1000,  1000,  -1000);
    glEnd();


    ////////////////////// REAR WALL ////////////////////////
    glBindTexture(GL_TEXTURE_2D, texId[3]);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex3f( 1000, 0, 1000);
        glTexCoord2f(1.0, 0.0); glVertex3f(-1000, 0,  1000);
        glTexCoord2f(1.0, 1.0); glVertex3f(-1000, 1000,  1000);
        glTexCoord2f(0.0, 1.0); glVertex3f( 1000, 1000, 1000);
    glEnd();

    /////////////////////// TOP //////////////////////////
    glBindTexture(GL_TEXTURE_2D, texId[4]);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex3f(1000,  1000,  -1000);
        glTexCoord2f(1.0, 0.0); glVertex3f(1000, 1000,  1000);
        glTexCoord2f(1.0, 1.0); glVertex3f(-1000, 1000, 1000);
        glTexCoord2f(0.0, 1.0); glVertex3f(-1000, 1000, -1000);

    glEnd();

    /////////////////////// FLOOR //////////////////////////
    glBindTexture(GL_TEXTURE_2D, texId[5]);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex3f(-1000, 0., -1000);
        glTexCoord2f(1.0, 0.0); glVertex3f(-1000, 0.,  1000);
        glTexCoord2f(1.0, 1.0); glVertex3f(1000, 0., 1000);
        glTexCoord2f(0.0, 1.0); glVertex3f(1000,  0., -1000);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    /////////////////////// TOP COVERS //////////////////////////
    glColor3f(0, 0, 0);
    glBegin(GL_QUADS);
        glVertex3f(2000,  1025,  -2000);
        glVertex3f(2000, 1025,  2000);
        glVertex3f(-2000, 1025, 2000);
        glVertex3f(-2000, 1025, -2000);
    glEnd();

    glBegin(GL_QUADS);
        glVertex3f(2000,  1001,  -2000);
        glVertex3f(2000, 1001,  2000);
        glVertex3f(-2000, 1001, 2000);
        glVertex3f(-2000, 1001, -2000);
    glEnd();
    glColor3f(1, 1, 1);
}

//---------------------------------------------------------------------
void floor()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texId[6]);

    glBegin(GL_QUADS);
         glTexCoord2f(0.0, 500.0); glVertex3f(-1000, FLOOR_HEIGHT, -1000);
         glTexCoord2f(0.0, 0.0); glVertex3f(-1000, FLOOR_HEIGHT, 1000);
         glTexCoord2f(500.0, 0.0); glVertex3f(1000, FLOOR_HEIGHT, 1000);
         glTexCoord2f(500.0, 500.0); glVertex3f(1000, FLOOR_HEIGHT, -1000);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void createBox(float objectHeight, float boxLength,
                float boxDepth, int textureIndex, float texScale,
                float texTopXScale, float texTopYScale, int gate)
{
    glEnable(GL_TEXTURE_2D);
    float texLge = 5.0 * texScale;
    float texSml = 1.0 * texScale;
    float texZro = 0.0; //Just put in so tex coords are more readable
    float texTopY = 5.0 * texTopYScale;
    float texTopX = 1.0 * texTopXScale;

    glBindTexture(GL_TEXTURE_2D, texId[textureIndex]);

    if (!gate) {
        //Back
        glBegin(GL_QUADS);
            glTexCoord2f(texZro,texZro); glVertex3f(0.0, 0.0, 0.0);
            glTexCoord2f(texZro,texLge); glVertex3f(0.0, objectHeight, 0.0);
            glTexCoord2f(texLge,texLge); glVertex3f(boxLength, objectHeight, 0.0);
            glTexCoord2f(texLge,texZro); glVertex3f(boxLength, 0.0, 0.0);
        glEnd();

        //Front
        glBegin(GL_QUADS);
            glTexCoord2f(texZro,texZro); glVertex3f(0.0, 0.0, boxDepth);
            glTexCoord2f(texZro,texLge); glVertex3f(0.0, objectHeight, boxDepth);
            glTexCoord2f(texLge,texLge); glVertex3f(boxLength, objectHeight, boxDepth);
            glTexCoord2f(texLge,texZro); glVertex3f(boxLength, 0.0, boxDepth);
        glEnd();
    } else {
        //Back Left
        glBegin(GL_QUADS);
            glTexCoord2f(texZro,texZro); glVertex3f(0.0, 0.0, 0.0);
            glTexCoord2f(texZro,texLge); glVertex3f(0.0, objectHeight, 0.0);
            glTexCoord2f(texLge,texLge); glVertex3f(boxLength / 3, objectHeight, 0.0);
            glTexCoord2f(texLge,texZro); glVertex3f(boxLength / 3, 0.0, 0.0);
        glEnd();

         //Back Right
        glBegin(GL_QUADS);
            glTexCoord2f(texZro,texZro); glVertex3f(boxLength / 3 * 2, 0.0, 0.0);
            glTexCoord2f(texZro,texLge); glVertex3f(boxLength / 3 * 2, objectHeight, 0.0);
            glTexCoord2f(texLge,texLge); glVertex3f(boxLength, objectHeight, 0.0);
            glTexCoord2f(texLge,texZro); glVertex3f(boxLength, 0.0, 0.0);
        glEnd();

        //Front Left
        glBegin(GL_QUADS);
            glTexCoord2f(texZro,texZro); glVertex3f(0.0, 0.0, boxDepth);
            glTexCoord2f(texZro,texLge); glVertex3f(0.0, objectHeight, boxDepth);
            glTexCoord2f(texLge,texLge); glVertex3f(boxLength / 3, objectHeight, boxDepth);
            glTexCoord2f(texLge,texZro); glVertex3f(boxLength / 3, 0.0, boxDepth);
        glEnd();

         //Front Right
        glBegin(GL_QUADS);
            glTexCoord2f(texZro,texZro); glVertex3f(boxLength / 3 * 2, 0.0, boxDepth);
            glTexCoord2f(texZro,texLge); glVertex3f(boxLength / 3 * 2, objectHeight, boxDepth);
            glTexCoord2f(texLge,texLge); glVertex3f(boxLength, objectHeight, boxDepth);
            glTexCoord2f(texLge,texZro); glVertex3f(boxLength, 0.0, boxDepth);
        glEnd();

        //Inside Left
        glBegin(GL_QUADS);
            glTexCoord2f(texZro,texZro); glVertex3f(boxLength / 3, 0.0, 0.0);
            glTexCoord2f(texSml,texZro); glVertex3f(boxLength / 3, 0.0, boxDepth);
            glTexCoord2f(texSml,texLge); glVertex3f(boxLength / 3, objectHeight, boxDepth);
            glTexCoord2f(texZro,texLge); glVertex3f(boxLength / 3, objectHeight, 0.0);
        glEnd();

        //Inside Right
        glBegin(GL_QUADS);
            glTexCoord2f(texZro,texZro); glVertex3f(boxLength / 3 * 2, 0.0, 0.0);
            glTexCoord2f(texSml,texZro); glVertex3f(boxLength / 3 * 2, 0.0, boxDepth);
            glTexCoord2f(texSml,texLge); glVertex3f(boxLength / 3 * 2, objectHeight, boxDepth);
            glTexCoord2f(texZro,texLge); glVertex3f(boxLength / 3 * 2, objectHeight, 0.0);
        glEnd();

        //Back Above Doorway
        glBegin(GL_QUADS);
            glTexCoord2f(texZro,texZro); glVertex3f(boxLength / 3 * 2, (objectHeight / 3 * 2), 0.0);
            glTexCoord2f(texZro,texLge); glVertex3f(boxLength / 3 * 2, objectHeight, 0.0);
            glTexCoord2f(texLge,texLge); glVertex3f(boxLength / 3, objectHeight, 0.0);
            glTexCoord2f(texLge,texZro); glVertex3f(boxLength / 3, (objectHeight / 3 * 2), 0.0);
        glEnd();

        //Front Above Doorway
        glBegin(GL_QUADS);
            glTexCoord2f(texZro,texZro); glVertex3f(boxLength / 3 * 2, (objectHeight / 3 * 2), boxDepth);
            glTexCoord2f(texZro,texLge); glVertex3f(boxLength / 3 * 2, objectHeight, boxDepth);
            glTexCoord2f(texLge,texLge); glVertex3f(boxLength / 3, objectHeight, boxDepth);
            glTexCoord2f(texLge,texZro); glVertex3f(boxLength / 3, (objectHeight / 3 * 2), boxDepth);
        glEnd();

        //Bottom Above Doorway
        glBegin(GL_QUADS);
            glTexCoord2f(texZro,texZro); glVertex3f(boxLength / 3 * 2, (objectHeight / 3 * 2), 0.0);
            glTexCoord2f(texTopX,texZro); glVertex3f(boxLength / 3 * 2, (objectHeight / 3 * 2), boxDepth);
            glTexCoord2f(texTopX,texTopY); glVertex3f(boxLength / 3, (objectHeight / 3 * 2), boxDepth);
            glTexCoord2f(texZro,texTopY); glVertex3f(boxLength / 3, (objectHeight / 3 * 2), 0.0);
        glEnd();
    }

    //Left
    glBegin(GL_QUADS);
        glTexCoord2f(texZro,texZro); glVertex3f(0.0, 0.0, 0.0);
        glTexCoord2f(texSml,texZro); glVertex3f(0.0, 0.0, boxDepth);
        glTexCoord2f(texSml,texLge); glVertex3f(0.0, objectHeight, boxDepth);
        glTexCoord2f(texZro,texLge); glVertex3f(0.0, objectHeight, 0.0);
    glEnd();

    //Right
    glBegin(GL_QUADS);
        glTexCoord2f(texZro,texZro); glVertex3f(boxLength, 0.0, 0.0);
        glTexCoord2f(texSml,texZro); glVertex3f(boxLength, 0.0, boxDepth);
        glTexCoord2f(texSml,texLge); glVertex3f(boxLength, objectHeight, boxDepth);
        glTexCoord2f(texZro,texLge); glVertex3f(boxLength, objectHeight, 0.0);
    glEnd();

    //Top
    glBegin(GL_QUADS);
        glTexCoord2f(texZro,texZro); glVertex3f(0.0, objectHeight, 0.0);
        glTexCoord2f(texTopX,texZro); glVertex3f(0.0, objectHeight, boxDepth);
        glTexCoord2f(texTopX,texTopY); glVertex3f(boxLength, objectHeight, boxDepth);
        glTexCoord2f(texZro,texTopY); glVertex3f(boxLength, objectHeight, 0.0);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

//---------------------------------------------------------------------
void castleWall(float wallLength, float wallHeight, float wallDepth, int gate)
{
    createBox(wallHeight, wallLength, wallDepth, 7, 1.0, 1.0, 2.0, gate);

    float crenHeight = 0.75;
    float crenLength = 1.0;
    float crenDepth = 0.5;

    for (int i = 0; i < wallLength; i += (crenLength * 2)) {
        glPushMatrix();
            glTranslatef(i, wallHeight, 0);
            createBox(crenHeight, crenLength, crenDepth, 7, 0.05, 0.05, 0.1, 0);
        glPopMatrix();
    }
}

//---------------------------------------------------------------------
void castle()
{
    float wallHeight = 12.5; //y
    float wallLength = 30.0; //x
    float wallDepth = 2.5;   //z

    glPushMatrix();
        glTranslatef(-wallLength / 2, 0, -wallLength / 2);
        //Back Wall
        glPushMatrix();
            glTranslatef(0, 0, -0.1);
            castleWall(wallLength, wallHeight, wallDepth, 0);
        glPopMatrix();

        //Front Wall (Gate Wall)
        glPushMatrix();
            glTranslatef(wallLength, 0, wallLength + 0.1);
            glRotatef(180, 0, 1, 0);
            castleWall(wallLength, wallHeight, wallDepth, 1);
        glPopMatrix();

        //Left Wall
        glPushMatrix();
            glRotatef(90, 0, 1, 0);
            glTranslatef(-wallLength, 0, 0);
            castleWall(wallLength, wallHeight, wallDepth, 0);
        glPopMatrix();

        //Right Wall
        glPushMatrix();
            glRotatef(-90, 0, 1, 0);
            glTranslatef(0, 0, -wallLength);
            castleWall(wallLength, wallHeight, wallDepth, 0);
        glPopMatrix();
    glPopMatrix();

}

//---------------------------------------------------------------------
void normal(int tindx)
{
    float x1 = x[t1[tindx]], x2 = x[t2[tindx]], x3 = x[t3[tindx]];
    float y1 = y[t1[tindx]], y2 = y[t2[tindx]], y3 = y[t3[tindx]];
    float z1 = z[t1[tindx]], z2 = z[t2[tindx]], z3 = z[t3[tindx]];
    float nx, ny, nz;
    nx = y1*(z2-z3) + y2*(z3-z1) + y3*(z1-z2);
    ny = z1*(x2-x3) + z2*(x3-x1) + z3*(x1-x2);
    nz = x1*(y2-y3) + x2*(y3-y1) + x3*(y1-y2);
    glNormal3f(nx, ny, nz);
}

//---------------------------------------------------------------------
void cannon()
{

    glPushMatrix();
    glTranslatef(15, 15, 0);
    glRotatef(30, 0, 0, 1);
        glColor3f(0.1, 0.1, 0.1);
        //Construct the object model here using triangles read from OFF file
        glBegin(GL_TRIANGLES);

            for(int tindx = 0; tindx < ntri; tindx++)
            {
               normal(tindx);
               glVertex3d(x[t1[tindx]], y[t1[tindx]], z[t1[tindx]]);
               glVertex3d(x[t2[tindx]], y[t2[tindx]], z[t2[tindx]]);
               glVertex3d(x[t3[tindx]], y[t3[tindx]], z[t3[tindx]]);
            }
        glEnd();


    glPopMatrix();

    //Cannon Ball
    glColor3f(0, 0, 0);
    glPushMatrix();
        glTranslatef(cannonBallX, cannonBallY, 0);
        glTranslatef(38.88, 64, 0);
        glutSolidSphere(5, 36, 18);
    glPopMatrix();

    glColor3f(0.4, 0.2, 0.0);
    glPushMatrix();
        glTranslatef(-10, 5, 17);
        glScalef(80, 10, 6);
        glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-20, 25, 17);
        glScalef(40, 30, 6);
        glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-10, 5, -17);
        glScalef(80, 10, 6);
        glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-20, 25, -17);
        glScalef(40, 30, 6);
        glutSolidCube(1);
    glPopMatrix();

    glColor3f(1.0, 1.0, 1.0);
}

//---------------------------------------------------------------------
void generateSmoke()
{
    if (cannonFired) {
        updateParticles();
        if (!stopSmoke) {
            generateNewParticles();
        }
        drawParticles();
    }
}

//---------------------------------------------------------------------
void drawQuadripedRobot()
{
    glColor3f(1., 0.78, 0.06);      //Scanner Body
    glPushMatrix();
        glTranslatef(0, 7.7, 1.5);
        glScalef(1, 2, 1);
        glutSolidCube(1.4);
    glPopMatrix();

    glPushMatrix();                 //Scanner sphere/head
        glTranslatef(0, 8.7, 1.5);
        glutSolidSphere(1.4, 10, 10);
    glPopMatrix();

    glPushMatrix();                 //Tail
        glTranslatef(0, 7.5, -3.5);
        glRotatef(-45, 1, 0, 0);
        glTranslatef(0, -7.5, 3.5);
        glTranslatef(0, 7.5, -3.5);
        glScalef(0.8, 15, 0.8);
        glutSolidCube(0.2);
    glPopMatrix();

    glPushMatrix();                 //Tail end
        glTranslatef(0, 8.5, -4.5);
        glutSolidSphere(0.2, 10, 10);
    glPopMatrix();

    glColor3f(1., 0., 0.);          //Torso
    glPushMatrix();
        glTranslatef(0, 5.5, 0);
        glRotatef(90, 1, 0, 0);
        glScalef(3.75, 6.2, 2);
        glutSolidCube(1);
    glPopMatrix();

    glColor3f(0., 0., 1.);          //Front Right Leg
    glPushMatrix();
        glTranslatef(-1.05, 4.4, 2.3);
        glRotatef(-limbsTheta, 1, 0, 0);
        glTranslatef(1.05, -4.4, -2.3);
        glTranslatef(-1.05, 2.6, 2.3);
        //glTranslatef(-0.8, 2.2, 0);
        glScalef(1, 4.4, 1);
        glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();                 //Front Right Leg Socket
        glTranslatef(-1.05, 4.6, 2.3);
        glutSolidSphere(0.8, 10, 10);
    glPopMatrix();

    glColor3f(0., 0., 1.);          //Front Left Leg
    glPushMatrix();
        glTranslatef(1.05, 4.4, 2.3);
        glRotatef(limbsTheta, 1, 0, 0);
        glTranslatef(-1.05, -4.4, -2.3);
        glTranslatef(1.05, 2.6, 2.3);
        glScalef(1, 4.4, 1);
        glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();                 //Front Left Leg Socket
        glTranslatef(1.05, 4.6, 2.3);
        glutSolidSphere(0.8, 10, 10);
    glPopMatrix();

    glColor3f(0., 0., 1.);          //Back Right Leg
    glPushMatrix();
        glTranslatef(-1.05, 4.4, -2.3);
        glRotatef(-limbsTheta, 1, 0, 0);
        glTranslatef(1.05, -4.4, 2.3);
        glTranslatef(-1.05, 2.6, -2.3);
        glScalef(1, 4.4, 1);
        glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();                 //Back Right Leg Socket
        glTranslatef(-1.05, 4.6, -2.3);
        glutSolidSphere(0.8, 10, 10);
    glPopMatrix();

    glColor3f(0., 0., 1.);          //Back Left Leg
    glPushMatrix();
        glTranslatef(1.05, 4.4, -2.3);
        glRotatef(limbsTheta, 1, 0, 0);
        glTranslatef(-1.05, -4.4, 2.3);
        glTranslatef(1.05, 2.6, -2.3);
        glScalef(1, 4.4, 1);
        glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();                 //Back Left Leg Socket
        glTranslatef(1.05, 4.6, -2.3);
        glutSolidSphere(0.8, 10, 10);
    glPopMatrix();

    glColor3f(1.0, 1.0, 1.0); //Reset colour values for scene
}

//---------------------------------------------------------------------
void drawFloatingRobot()
{
    //Main Body
    glColor3f(0.1, 0.1, 0.1);
    glPushMatrix();
        glScalef(0.8, 2, 0.8);
        glutSolidSphere(2, 50, 50);
    glPopMatrix();

    //Eye - White
    glColor3f(1, 1, 1);
    glPushMatrix();
        glTranslatef(1.2, 1, 0);
        glScalef(0.5, 1, 1);
        glutSolidSphere(1, 20, 20);
    glPopMatrix();

    //Eye - Iris
    glColor3f(1, 0, 0);
    glPushMatrix();
        glTranslatef(1.5, 1, 0);
        glScalef(0.5, 1, 1);
        glutSolidSphere(0.5, 10, 10);
    glPopMatrix();

}

//--------------------------------------------------------------
void spaceShip()
{
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, texId[9]);
    float cubeSize = 5.0;

    glPushMatrix();
        //Front
        glBegin(GL_QUADS);
            glTexCoord2f(-movingTexZro, -movingTexZro); glVertex3f(0.0, 0.0, cubeSize);
            glTexCoord2f(-movingTexZro, -movingTexLge); glVertex3f(0.0, cubeSize, cubeSize);
            glTexCoord2f(-movingTexLge, -movingTexLge); glVertex3f(cubeSize, cubeSize, cubeSize);
            glTexCoord2f(-movingTexLge, -movingTexZro); glVertex3f(cubeSize, 0.0, cubeSize);
        glEnd();

        //Back
        glBegin(GL_QUADS);
            glTexCoord2f(movingTexZro,movingTexZro); glVertex3f(0.0, 0.0, 0.0);
            glTexCoord2f(movingTexZro,movingTexLge); glVertex3f(0.0, cubeSize, 0.0);
            glTexCoord2f(movingTexLge,movingTexLge); glVertex3f(cubeSize, cubeSize, 0.0);
            glTexCoord2f(movingTexLge,movingTexZro); glVertex3f(cubeSize, 0.0, 0.0);
        glEnd();

        //Left
        glBegin(GL_QUADS);
            glTexCoord2f(-movingTexZro,-movingTexZro); glVertex3f(0.0, 0.0, 0.0);
            glTexCoord2f(-movingTexLge,-movingTexZro); glVertex3f(0.0, 0.0, cubeSize);
            glTexCoord2f(-movingTexLge,-movingTexLge); glVertex3f(0.0, cubeSize, cubeSize);
            glTexCoord2f(-movingTexZro,-movingTexLge); glVertex3f(0.0, cubeSize, 0.0);
        glEnd();

        //Right
        glBegin(GL_QUADS);
            glTexCoord2f(movingTexZro,movingTexZro); glVertex3f(cubeSize, 0.0, 0.0);
            glTexCoord2f(movingTexLge,movingTexZro); glVertex3f(cubeSize, 0.0, cubeSize);
            glTexCoord2f(movingTexLge,movingTexLge); glVertex3f(cubeSize, cubeSize, cubeSize);
            glTexCoord2f(movingTexZro,movingTexLge); glVertex3f(cubeSize, cubeSize, 0.0);
        glEnd();

        //Top
        glBegin(GL_QUADS);
            glTexCoord2f(-movingTexZro,-movingTexZro); glVertex3f(0.0, cubeSize, 0.0);
            glTexCoord2f(-movingTexLge,-movingTexZro); glVertex3f(0.0, cubeSize, cubeSize);
            glTexCoord2f(-movingTexLge,-movingTexLge); glVertex3f(cubeSize, cubeSize, cubeSize);
            glTexCoord2f(-movingTexZro,-movingTexLge); glVertex3f(cubeSize, cubeSize, 0.0);
        glEnd();

        //Bottom
        glBegin(GL_QUADS);
            glTexCoord2f(-movingTexZro,-movingTexZro); glVertex3f(0.0, 0.0, 0.0);
            glTexCoord2f(-movingTexLge,-movingTexZro); glVertex3f(0.0, 0.0, cubeSize);
            glTexCoord2f(-movingTexLge,-movingTexLge); glVertex3f(cubeSize, 0.0, cubeSize);
            glTexCoord2f(-movingTexZro,-movingTexLge); glVertex3f(cubeSize, 0.0, 0.0);
        glEnd();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}

//--------------------------------------------------------------
int collisionDetection(int forwards, float moveSpeed, int vertical) {
    typedef struct {
        float x1, x2;
        float z1, z2;
        float y1, y2;
    } Object;

    Object spaceShip = {2.5, -2.5, 2.5, -2.5, 0, 5};
    Object castleWall1 = {15, 12.5, 15, -15, 0, 13.25}; //Right wall, 13.25 height accounts for crenelation as well
    Object castleWall2 = {-12.5, -15, 15, -15, 0, 13.25}; //Left Wall
    Object castleWall3 = {15, -15, -12.5, -15, 0, 13.25}; //Back Wall
    Object castleGateRight = {15, 5, 15, 12.5, 0, 13.25};
    Object castleGateLeft = {-5, -15, 15, 12.5, 0, 13.25};
    Object castleGateTop = {5, -5, 15, 12.5, 12.5 / 3 * 2, 13.25};
    Object cannon1 = {12, 8, 24, 16, 0, 6};
    Object cannon2 = {-8, -12, 24, 16, 0, 6};

    int numObjects = 9;
    Object objects[numObjects] = {spaceShip, castleWall1, castleWall2,
        castleWall3, castleGateLeft, castleGateRight, castleGateTop,
        cannon1, cannon2};

    moveSpeed *= 1.5; //To reduce clipping

    float newEye_x = eye_x - moveSpeed * sin(lookAngle * degToRad);
    float newEye_z = eye_z + moveSpeed * cos(lookAngle * degToRad);

    if (forwards) {
        newEye_x = eye_x + moveSpeed * sin(lookAngle * degToRad);
        newEye_z = eye_z - moveSpeed * cos(lookAngle * degToRad);
    }

    for (int i = 0; i < numObjects; i++) {
        Object o = objects[i];
        o.z1 -= 50; //Account for whole scene being moved
        o.z2 -= 50;
        o.y1 += FLOOR_HEIGHT; //Account for objects being moved up to floor level
        o.y2 += FLOOR_HEIGHT;
        float new_y = cameraHeight;

        if (vertical) { //If camera moving up or down
            if (forwards) new_y += moveSpeed; //Going up
            if (!forwards) new_y -= moveSpeed; //Going down
        }

        if (newEye_x >= 1000 || newEye_x <= -1000    //Check skybox
            || newEye_z >= 1000 || newEye_z <= -1000) {
                return 1; //Skybox collision found
            }

        if ((newEye_x <= o.x1 && newEye_x >= o.x2)     //Check x direction
            && (newEye_z <= o.z1 && newEye_z >= o.z2)   //Check z direction
            && (new_y >= o.y1 && new_y <= o.y2)) {      //Check y direction
                return 1; //Collision found
        }
    }

    return 0; //No collisions found
}

//--------------------------------------------------------------
void cameraMovement(int key, int x, int y)
{
    float cameraSpeed = 0.5;
    float maxCameraHeight = 20;

    if (key == GLUT_KEY_HOME) {
        spaceShipView = !spaceShipView;
    }

    if (!spaceShipView) {
        if(key==GLUT_KEY_LEFT) lookAngle-=5;         //Turn left
        else if(key==GLUT_KEY_RIGHT) lookAngle+=5;   //Turn right
        else if(key == GLUT_KEY_DOWN) {  //Move backward
            if (!collisionDetection(0, cameraSpeed, 0)) {
                eye_x -= cameraSpeed * sin(lookAngle * degToRad);
                eye_z += cameraSpeed * cos(lookAngle * degToRad);
            }
        } else if (key == GLUT_KEY_UP) { //Move forward
            if (!collisionDetection(1, cameraSpeed, 0)) {
                eye_x += cameraSpeed * sin(lookAngle * degToRad);
                eye_z -= cameraSpeed * cos(lookAngle * degToRad);
            }
        } else if (key == GLUT_KEY_PAGE_UP && cameraHeight < FLOOR_HEIGHT + maxCameraHeight) { //Move up
            if (!collisionDetection(1, cameraSpeed, 1)) {
                cameraHeight += cameraSpeed;
            }
        } else if (key == GLUT_KEY_PAGE_DOWN && cameraHeight > FLOOR_HEIGHT + 1) { //Move down
            if (!collisionDetection(0, cameraSpeed, 1)) {
                cameraHeight -= cameraSpeed;
            }
        }

        look_x = eye_x + 100*sin(lookAngle * degToRad);
        look_z = eye_z - 100*cos(lookAngle * degToRad);
    }

    glutPostRedisplay();
}

//---------------------------------------------------------------------
void initialise(void)
{
    loadGLTextures();
    loadMeshFile("Cannon.off");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_NORMALIZE);
    glClearColor(0., 0., 0., 0.);    //Background colour
}

//---------------------------------------------------------------------
void display(void)
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective(80., 1., 0.1, 5000.);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (!spaceShipView) {
        gluLookAt(eye_x, cameraHeight, eye_z, look_x, 500, look_z, 0, 1, 0);
    } else {
        gluLookAt(0, spaceShipHeight + FLOOR_HEIGHT + 1, -47, 0, 500, -45.5, 0, 1, 0);
    }


    //Display scene
    skybox();
    floor();

    glPushMatrix();
        glTranslatef(0, FLOOR_HEIGHT, -50); //Translate all objects to just above the floor plane

        castle();

        //Robot Quadriped Patroller 1
        glPushMatrix();
            glRotatef(patrolTheta, 0, 1, 0);
            glTranslatef(30, 0, 0);
            //glTranslatef(0.0, -1.0, 0.0);
            glScalef(0.4, 0.4, 0.4);
            drawQuadripedRobot();
        glPopMatrix();

        //Robot Quadriped Patroller 2
        glPushMatrix();
            glRotatef(-patrolTheta, 0, 1, 0);
            glTranslatef(-27, 0, 0);
            //glTranslatef(0.0, -1.0, 0.0);
            glScalef(0.4, 0.4, 0.4);
            //glRotatef(180, 0, 1, 0);
            drawQuadripedRobot();
        glPopMatrix();

        //Floating robot 1
        glPushMatrix();
            glTranslatef(0, roboY, roboZ);
            glTranslatef(-13.5, 14, -13.5);
            glRotatef(-roboForward, 0, 1, 0);
            glScalef(0.3, 0.3, 0.3);
            drawFloatingRobot();
        glPopMatrix();

        //Floating robot 2
        glPushMatrix();
            glTranslatef(0, roboY, -roboZ);
            glTranslatef(13.5, 14, 13.5);
            glRotatef(roboForward, 0, 1, 0);
            glScalef(0.3, 0.3, 0.3);
            drawFloatingRobot();
        glPopMatrix();

        //Cannon 1
        glPushMatrix();
            glTranslatef(10.0, 0.0, 20.0);
            glRotatef(-90, 0, 1, 0);
            glScalef(0.08, 0.08, 0.08);
            cannon();
        glPopMatrix();

        //Cannon 2
        glPushMatrix();
            glTranslatef(-10.0, 0.0, 20.0);
            glRotatef(-90, 0, 1, 0);
            glScalef(0.08, 0.08, 0.08);
            cannon();
        glPopMatrix();

        glPushMatrix();
            glTranslatef(0.0, spaceShipHeight + 2.5, 0.0);
            glRotatef(spaceShipRotateX, 1, 0, 0);
            glRotatef(spaceShipRotateY, 0, 1, 0);
            glRotatef(spaceShipRotateZ, 0, 0, 1);
            glTranslatef(-2.5, -2.5, -2.5);
            spaceShip();
        glPopMatrix();

        //Cannon smoke particles
        glPushMatrix();
            glTranslatef(10, 5.1, 23.4);
            generateSmoke();
        glPopMatrix();

        glPushMatrix();
            glTranslatef(-10, 5.1, 23.4);
            generateSmoke();
        glPopMatrix();

    glPopMatrix();

    glutSwapBuffers();
    glFlush();
}

//-------------------------------------------------------------------
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_DEPTH );
    glutInitWindowSize (700, 700);
    glutInitWindowPosition (50, 50);

    glutCreateWindow ("Alien Fort");
    initialise();
    glutDisplayFunc(display);
    glutSpecialFunc(cameraMovement);
    glutKeyboardFunc(keyboardFunction);
    glutTimerFunc(50, limbsMovementTimer, 0);
    glutTimerFunc(50, patrolTimer, 0);
    glutTimerFunc(50, textureTimer, 0);

    glutMainLoop();
    return 0;
}
