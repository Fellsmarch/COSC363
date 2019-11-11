/*========================================================================
* COSC 363  Computer Graphics (2018)
* Ray tracer
* See Lab07.pdf for details.
*=========================================================================
*/
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Sphere.h"
#include "SceneObject.h"
#include "Ray.h"
#include "Plane.h"
#include <GL/glut.h>
#include "TextureBMP.h"
using namespace std;

const float WIDTH = 20.0;
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int NUMDIV = 500;
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;

TextureBMP sphereTexture;
glm::vec3 texturedSphereLocation(-17.0, -5.0, -60.0);

int cameraView = 1;

vector<SceneObject*> sceneObjects;  //A global list containing pointers to objects in the scene


//---The most important function in a ray tracer! ----------------------------------
//   Computes the colour value obtained by tracing a ray and finding its
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
    glm::vec3 backgroundCol(0.3, 0.3, 0.3);
    glm::vec3 light(20, 40, -3);
    glm::vec3 ambientCol(0.2);   //Ambient color of light
    float phongsConstant = 20.0;
    glm::vec3 colourSum(0);

    ray.closestPt(sceneObjects);        //Compute the closest point of intersetion of objects with the ray

    if (ray.xindex == -1) return backgroundCol;      //If there is no intersection return background colour

    glm::vec3 materialCol = sceneObjects[ray.xindex]->getColor();




    //--Lab 7 Task 2--//
    glm::vec3 normalVector = sceneObjects[ray.xindex]->normal(ray.xpt);
    glm::vec3 lightVector = glm::normalize(light - ray.xpt);
    float lDotn = glm::dot(lightVector, normalVector);
    glm::vec3 ambientComponent = ambientCol * materialCol;
    glm::vec3 diffuseComponent = lDotn * materialCol;
//    if (lDotn <= 0) {
//        return ambientComponent;
//    } else {
//        return ambientComponent + diffuseComponent;
//    }

    //--Lab 7 Task 3--//
    glm::vec3 reflectVector = glm::reflect(-lightVector, normalVector);
    float rDotv = glm::dot(reflectVector, normalVector);
    rDotv = max(rDotv, 0.0f);
    glm::vec3 specularComponent(0);

    if (rDotv >= 0) {
        specularComponent = pow(rDotv, phongsConstant) * glm::vec3(1);
    }

//    if (lDotn <= 0) {
//        return ambientComponent;
//    } else {
//        return ambientComponent + diffuseComponent + specularComponent;
//    }

    //--Lab 8 Task 1--//
    Ray shadow(ray.xpt, lightVector);
    shadow.closestPt(sceneObjects);
    float lightDistance = glm::length(light - ray.xpt); //Light - ray is just original light vector (un-normalized)

    if (lDotn <= 0 || (shadow.xindex > -1 && shadow.xdist < lightDistance)) {
        colourSum += ambientComponent;
    } else {
        colourSum += ambientComponent + diffuseComponent + specularComponent;
    }


    //--Second light source--//
    glm::vec3 light2(-70, 40, -3);
    glm::vec3 colourSum2(0);


    glm::vec3 lightVector2 = glm::normalize(light2 - ray.xpt);
    float lDotn2 = glm::dot(lightVector2, normalVector);
    glm::vec3 diffuseComponent2 = lDotn2 * materialCol;

    //--Shadows for second light source--//
    glm::vec3 reflectVector2 = glm::reflect(-lightVector2, normalVector);
    float rDotv2 = glm::dot(reflectVector2, normalVector);
    rDotv2 = max(rDotv2, 0.0f);
    glm::vec3 specularComponent2(0);

    if (rDotv2 >= 0) {
        specularComponent2 = pow(rDotv2, phongsConstant) * glm::vec3(1);
    }

    Ray shadow2(ray.xpt, lightVector2);
    shadow2.closestPt(sceneObjects);
    float lightDistance2 = glm::length(light2 - ray.xpt);

    //--Checking if the pixel should be in shadow from the second light source--//
    if (lDotn2 <= 0 || (shadow2.xindex > -1 && shadow2.xdist < lightDistance2)) {
        colourSum2 += ambientComponent;
    } else {
        colourSum2 += ambientComponent + diffuseComponent2 + specularComponent2;
    }

    colourSum = (colourSum + colourSum2) * 0.5f;


    //--Lab 8 Task 2--//
    if (ray.xindex == 0 && step < MAX_STEPS) {
        glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVector);
        Ray reflectedRay(ray.xpt, reflectedDir);
        glm::vec3 reflectedCol = trace(reflectedRay, step + 1);
        colourSum = colourSum + (0.8f * reflectedCol);
    }

    //--Procedurally generated pattern on sphere--//
    if (ray.xindex == 1) {
        int xCurrent = (int) ((ray.xpt.x + 200) * 2) % 3;
        int zCurrent = (int) ((ray.xpt.z + 200) * 2) % 3;

        if ((xCurrent && zCurrent) || (!xCurrent && !zCurrent)) { //If both are the same
            sceneObjects[1]->setColor(glm::vec3(1));
        } else { //Else they are different (one 0 and the other 1)
            sceneObjects[1]->setColor(glm::vec3(0));
        }
    }

    //--Refraction and transparency on sphere--// Similar to reflection from Lab 8 task 2 // lecture notes 9 pg. 21
    //Don't think I need to change for multiple lights?
    if (ray.xindex == 2 && step < MAX_STEPS) {
        float eta = 1.0 / 1.01;
        float transparency = 0.3;

        glm::vec3 incidentDirection = glm::refract(ray.dir, normalVector, eta); //The direction of the incident ray, used to make incident ray
        Ray incidentRay(ray.xpt, incidentDirection); //The incident ray ('inside' the sphere)

        //Same as for original ray
        incidentRay.closestPt(sceneObjects);
        if (incidentRay.xindex == -1) return backgroundCol;

        glm::vec3 refractedNormalVector = sceneObjects[incidentRay.xindex]->normal(incidentRay.xpt);

        glm::vec3 refractedDirection = glm::refract(incidentDirection, -refractedNormalVector, 1.0f / eta); //The direction the ray is refracted to
        Ray refractedRay(incidentRay.xpt, refractedDirection);

        //Same as above
        refractedRay.closestPt(sceneObjects);
        if (refractedRay.xindex == -1) return backgroundCol; //Return this so any part over the background is still drawn

        glm::vec3 refractedColour = trace(refractedRay, step + 1); //Recursively get colour
        colourSum = (colourSum * transparency) + (refractedColour * (1 - transparency));
    }

    //--Transparent Sphere--//
    if (ray.xindex == 5) {
        float transparency = 0.2;

        Ray incidentRay(ray.xpt, ray.dir);
        incidentRay.closestPt(sceneObjects);

        if (incidentRay.xindex == -1) return backgroundCol;

        Ray outwardsRay(ray.xpt, incidentRay.dir);
        outwardsRay.closestPt(sceneObjects);

        glm::vec3 transparentColour = trace(outwardsRay, step + 1);
        colourSum = (colourSum * transparency) + (transparentColour * (1 - transparency));
    }

    //--Textured sphere--//
    if (ray.xindex == 3) {
        glm::vec3 relativePoint = glm::normalize(ray.xpt - texturedSphereLocation);

        float texY = 0.5 + asin(relativePoint.y) / M_PI; //M_PI is just cmath library's pi
        float texX = 0.5 - atan2(relativePoint.z, relativePoint.x) / (2 * M_PI);
        colourSum = sphereTexture.getColorAt(texX, texY);
    }

    //--Chequered floor--//
    if (ray.xindex == 4) { //If the current object is the floor
        int xCurrent = (int) ((ray.xpt.x + 200) / 5) % 2; //10 'checks' on the floor, mod 2 since we only use two colours
        int zCurrent = (int) ((ray.xpt.z + 200) / 5) % 2;

        if ((xCurrent && zCurrent) || (!xCurrent && !zCurrent)) { //If both are the same (either both 0 or both 1)
            sceneObjects[4]->setColor(glm::vec3(1));
        } else { //Else they are different (one 0 and the other 1)
            sceneObjects[4]->setColor(glm::vec3(0));
        }
    }

    return colourSum;
}

//---------Anti Aliasing---------//
glm::vec3 antiAliasing(Ray ray, glm::vec3 eye, float xp, float yp) {
    float pixelSize = (XMAX - XMIN) / NUMDIV;

    Ray firstQuarter = Ray(eye, glm::vec3(xp + pixelSize * 0.25, yp + pixelSize * 0.25, -EDIST));
    Ray secondQuarter = Ray(eye, glm::vec3(xp + pixelSize * 0.25, yp + pixelSize * 0.75, -EDIST));
    Ray thirdQuarter = Ray(eye, glm::vec3(xp + pixelSize * 0.75, yp + pixelSize * 0.25, -EDIST));
    Ray fourthQuarter = Ray(eye, glm::vec3(xp + pixelSize * 0.75, yp + pixelSize * 0.75, -EDIST));

    firstQuarter.normalize();
    secondQuarter.normalize();
    thirdQuarter.normalize();
    fourthQuarter.normalize();

    //Add up all colours and average them
    glm::vec3 average(0.25);
    return (trace(firstQuarter, 1) + trace(secondQuarter, 1) + trace(thirdQuarter, 1) + trace(fourthQuarter, 1)) * average;
}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
    float xp, yp;  //grid point
    float cellX = (XMAX-XMIN)/NUMDIV;  //cell width
    float cellY = (YMAX-YMIN)/NUMDIV;  //cell height

    glm::vec3 eye;

    if (cameraView) {
        eye = glm::vec3(0., 0., 35.);  //The eye position (source of primary rays) is the origin
    } else {
        eye = glm::vec3(0, -10, -10.);  //The eye position (source of primary rays) is the origin
    }

    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBegin(GL_QUADS);  //Each cell is a quad.

    for(int i = 0; i < NUMDIV; i++)     //For each grid point xp, yp
    {
        xp = XMIN + i*cellX;
        for(int j = 0; j < NUMDIV; j++)
        {
            yp = YMIN + j*cellY;

            glm::vec3 dir(xp+0.5*cellX, yp+0.5*cellY, -EDIST);  //direction of the primary ray

            Ray ray = Ray(eye, dir);        //Create a ray originating from the camera in the direction 'dir'
            ray.normalize();                //Normalize the direction of the ray to a unit vector
            glm::vec3 col = antiAliasing(ray, eye, xp, yp);
//            glm::vec3 col = trace (ray, 1); //Trace the primary ray and get the colour value

            glColor3f(col.r, col.g, col.b);
            glVertex2f(xp, yp);             //Draw each cell with its color value
            glVertex2f(xp+cellX, yp);
            glVertex2f(xp+cellX, yp+cellY);
            glVertex2f(xp, yp+cellY);
        }
    }

    glEnd();
    glFlush();
}

//---------Keyboard function to change camera---------//
void keyboardFunction(unsigned char key, int x, int y) {
    if (key == 'c') {
        cameraView = !cameraView;
        glutPostRedisplay();
    }
}

//---------Create a cube, can move the cube automatically with transform parameters---------//
void createCube(float xTransform, float yTransform, float zTransform) {
    float angle = glm::radians(25.0f);
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, glm::normalize(glm::vec3(0, 1, 1)));

    glm::vec3 cubeTopA = glm::vec3(-5 + xTransform, -10 + yTransform, -45 + zTransform);
    glm::vec3 cubeTopB = glm::vec3(5 + xTransform, -10 + yTransform, -45 + zTransform);
    glm::vec3 cubeTopC = glm::vec3(5 + xTransform, -10 + yTransform, -55 + zTransform);
    glm::vec3 cubeTopD = glm::vec3(-5 + xTransform, -10 + yTransform, -55 + zTransform);

    glm::vec3 cubeBottomA = glm::vec3(-5 + xTransform, -20 + yTransform, -45 + zTransform);
    glm::vec3 cubeBottomB = glm::vec3(5 + xTransform, -20 + yTransform, -45 + zTransform);
    glm::vec3 cubeBottomC = glm::vec3(5 + xTransform, -20 + yTransform, -55 + zTransform);
    glm::vec3 cubeBottomD = glm::vec3(-5 + xTransform, -20 + yTransform, -55 + zTransform);

    //--Each plane made in counter-clockwise order (from the normal), always in the same declaration order as the topPlane--//

    Plane *topPlane = new Plane(glm::vec3(rotation * glm::vec4(cubeTopA, 1)), //Point A
                                glm::vec3(rotation * glm::vec4(cubeTopB, 1)), //Point B
                                glm::vec3(rotation * glm::vec4(cubeTopC, 1)), //Point C
                                glm::vec3(rotation * glm::vec4(cubeTopD, 1)), //Point D
                                glm::vec3(1, 0, 0.5)); //Colour

    Plane *bottomPlane = new Plane(glm::vec3(rotation * glm::vec4(cubeBottomA, 1)),
                                   glm::vec3(rotation * glm::vec4(cubeBottomB, 1)),
                                   glm::vec3(rotation * glm::vec4(cubeBottomC, 1)),
                                   glm::vec3(rotation * glm::vec4(cubeBottomD, 1)),
                                   glm::vec3(1, 0, 0.5));

    Plane *rightPlane = new Plane(glm::vec3(rotation * glm::vec4(cubeBottomC, 1)),
                                  glm::vec3(rotation * glm::vec4(cubeBottomB, 1)),
                                  glm::vec3(rotation * glm::vec4(cubeTopB, 1)),
                                  glm::vec3(rotation * glm::vec4(cubeTopC, 1)),
                                  glm::vec3(1, 0, 0.5));

    Plane *leftPlane = new Plane(glm::vec3(rotation * glm::vec4(cubeBottomD, 1)),
                                 glm::vec3(rotation * glm::vec4(cubeBottomA, 1)),
                                 glm::vec3(rotation * glm::vec4(cubeTopA, 1)),
                                 glm::vec3(rotation * glm::vec4(cubeTopD, 1)),
                                 glm::vec3(1, 0, 0.5));

    Plane *backPlane = new Plane(glm::vec3(rotation * glm::vec4(cubeBottomD, 1)),
                                 glm::vec3(rotation * glm::vec4(cubeBottomC, 1)),
                                 glm::vec3(rotation * glm::vec4(cubeTopC, 1)),
                                 glm::vec3(rotation * glm::vec4(cubeTopD, 1)),
                                 glm::vec3(1, 0, 0.5));

    Plane *frontPlane = new Plane(glm::vec3(rotation * glm::vec4(cubeBottomB, 1)),
                                  glm::vec3(rotation * glm::vec4(cubeBottomA, 1)),
                                  glm::vec3(rotation * glm::vec4(cubeTopA, 1)),
                                  glm::vec3(rotation * glm::vec4(cubeTopB, 1)),
                                  glm::vec3(1, 0, 0.5));

    sceneObjects.push_back(bottomPlane);
    sceneObjects.push_back(topPlane);
    sceneObjects.push_back(rightPlane);
    sceneObjects.push_back(leftPlane);
    sceneObjects.push_back(backPlane);
    sceneObjects.push_back(frontPlane);

    //Other ways of doing it:
    //--No rotation (with cube corners)--//
//    Plane *bottomPlane = new Plane (cubeBottomA, cubeBottomB, cubeBottomC, cubeBottomD, glm::vec3(1, 0, 0.5)); //Colour
//    Plane *topPlane = new Plane (cubeTopA, cubeTopB, cubeTopC, cubeTopD, glm::vec3(1, 0, 0.5)); //Colour


    //--Old way of doing it (without cube corners & with rotation)--//
//    Plane *topPlane = new Plane (glm::vec3(rotation * glm::vec4(-5 + xTransform, -10 + yTransform, -45 + zTransform, 1)), //Point A
//                                  glm::vec3(rotation * glm::vec4(5 + xTransform, -10 + yTransform, -45 + zTransform, 1)), //Point B
//                                  glm::vec3(rotation * glm::vec4(5 + xTransform, -10 + yTransform, -55 + zTransform, 1)), //Point C
//                                  glm::vec3(rotation * glm::vec4(-5 + xTransform, -10 + yTransform, -55 + zTransform, 1)), //Point D
//                                  glm::vec3(1, 0, 0.5)); //Colour
}


//---This function initializes the scene -------------------------------------------
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);
    glClearColor(0, 0, 0, 1);

    sphereTexture = TextureBMP((char*) "earth.bmp");



    //-- Create a pointer to a sphere object
    Sphere *sphere1 = new Sphere(glm::vec3(-5.0, -5.0, -110.0), 15.0, glm::vec3(0, 0, 1));
    Sphere *sphere2 = new Sphere(glm::vec3(5.0, 5.0, -90.0), 5.0, glm::vec3(1, 0, 0));
    Sphere *sphere3 = new Sphere(glm::vec3(5.0, -10.0, -90.0), 7, glm::vec3(0.8, 0.8, 0.8));
    Sphere *sphere4 = new Sphere(texturedSphereLocation, 5, glm::vec3(1));
    Sphere *sphere5 = new Sphere(glm::vec3(-5.0, -10.0, -50.0), 6, glm::vec3(0, 1, 0));


    //--Add the above to the list of scene objects.
    sceneObjects.push_back(sphere1);
    sceneObjects.push_back(sphere2);
    sceneObjects.push_back(sphere3);
    sceneObjects.push_back(sphere4);

    Plane *plane = new Plane (glm::vec3(-40., -20, -40), //Point A
                              glm::vec3(40., -20, -40), //Point B
                              glm::vec3(40., -20, -200), //Point C
                              glm::vec3(-40., -20, -200), //Point D
                              glm::vec3(0.5, 0.5, 0)); //Colour

    sceneObjects.push_back(plane);
    sceneObjects.push_back(sphere5);

    createCube(30, 5, 0);
}



int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracer");

    glutKeyboardFunc(keyboardFunction);

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
