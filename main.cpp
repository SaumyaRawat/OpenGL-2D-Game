#include <fstream>
#include <math.h>
//Include OpenGL header files, so that we can use OpenGL
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <time.h>
#include <unistd.h>
#include <sstream>
#include "imageloader.h"
#include "text3d.h"

using namespace std;

/* Global Variables */
GLfloat angleCircle = 0.0f;     // Rotational angle for cube
static float zoomFactor = 1.0; /* Modified by user input. Initially 1.0 */
static float panFactor = 0.0; /* Modified by user input. Initially 1.0 */
int refreshMills = 1;        // refresh interval in milliseconds
float deltaT = 0.001;
float angle=0.0;
float _cameraangle=30.0;
int WIDTH=600;
int HEIGHT=400;
int InAir = 3; //Check if shell is in air
const float PI= 3.1472;
float Speed = 200.5;
float NewSpeedX = 0.0;
float NewSpeedY = 0.0;
float Gravity = -9.8 ; 
float Direction = 0.0;
int NoOfTargets = 3;
int TargetsLeft;
int NoOfObstacles = 5;
int StopFlag = 0;
const float ShellRadius = 0.3;
const float TargetRadius = 0.3;
const float ObstacleRadius = 0.3;
float Score = 0;
double rotate_by_key=0;
int isPanning = 0;
int mouseAngle = 0;
double rotate_x=0.5;
float startX, startY ; 
string Result;//string which will contain the result
bool mouseLeftDown = false;   // True if mouse LEFT button is down.
                   		           // Saved by mouse.
float mouseX, mouseY;           // Mouse x,y coords, in GLUT format (pixels from upper-left corner).
                              // Only guaranteed to be valid if a mouse button is down.
                              // Saved by mouse, motion.


//Function Prototypes:
void drawCircle(float radius);
GLuint loadTexture(Image* image);
float computeScale(const char* strs[4]);
void setProjectionMatrix (int width, int height);

//Class for shells launched from cannon
class shell
{
public:

	float LX;  //LocalX in the view of canon
	float LY;	 //LocalY in the view of canon
	float WX;	 //World X 
	float WY;  //World Y
	float Theta;
	float Current;
	float Time;
	float World_Origin_X;
	float World_Origin_Y;
	float Canon_Origin_X;
	float Canon_Origin_Y;
	int Rebound; /*"0 means Going -->"  "1 = collided with right wall" "2 = collided with roof 
					"3 = collided with ground" "4 = collided w/ left wall" */
	float NewTheta;
	float vX;
	float aX;
	float aY;
	float vY;
	float NewVX;
	float NewVY;
	float k;
	void initShell()
	{
		this->Current = 0.0;
		//cout<<"Value of angle changed!\n";
		//cout<<"Init A "<<angle<<"\n";
		this->Theta = ((angle))*(PI/180);
		this->Canon_Origin_X=(0.0*cos( this->Theta ) - 1.5*sin( this->Theta )) - 4.5 ; //DEBUG
		this->Canon_Origin_Y=(0.0*sin( this->Theta ) + 1.5*cos( this->Theta )) - 3.2 ; //DEBUG
		this->Theta = ((90+angle))*(PI/180);
		InAir = 1;
		this->LX = Canon_Origin_X;
		this->LY = Canon_Origin_Y;
		this->Rebound = 0;
		this->k = 0.5;
	}

	void moveShell() 
	{	
		this->Time = this->Current;
		//cout<<"Angle : "<<this->Theta<<"\n";
		//this->Theta = 30.0 * (PI/180);
		if(this->Rebound == 0)		//has just been launched and is on a front moving projectile
		{
			this->vX = Speed*cos(this->Theta);   //Vx
			this->vY = Speed*sin(this->Theta);	//Vy
			this->aX = -this->k * Speed * this->vX;
			this->aY = (-this->k * Speed * this->vY) - Gravity;
			this->NewVX = this->vX + ( this->aX * deltaT );
			this->NewVY = this->vY + ( this->aY * deltaT );
			this->LX += ( this->NewVX * deltaT ) + ( this->aX * deltaT * deltaT );
			this->LY += ( this->NewVY * deltaT ) + ( this->aY * deltaT * deltaT );
		    //this->LX +=  ( ( cos( this->Theta ) * Speed * this->Time ) );
			//this->LY +=  ( ( Speed * this->Time * (sin( this->Theta )) ) - ( 0.5 * Gravity * this->Time * this->Time) );			
		}
	else if(this->Rebound == 1) //collided with the walls on the right side
		{
			this->NewTheta = 90 + this->Theta;
			this->vX = Speed*cos(this->NewTheta);   //Vx
			this->vY = Speed*sin(this->NewTheta);	//Vy
			this->aX = -this->k * Speed * this->vX;
			this->aY = (-this->k * Speed * this->vY) - Gravity;
			this->NewVX = this->vX + ( this->aX * deltaT );
			this->NewVY = this->vY + ( this->aY * deltaT );
			this->LX += ( this->NewVX * deltaT ) + ( this->aX * deltaT * deltaT );
			this->LY -= ( this->NewVY * deltaT ) + ( this->aY * deltaT * deltaT );

/*			this->NewVX = Speed*cos(this->Theta);   //Vx
			this->NewVY = Speed*sin(this->Theta);	//Vy
//			this->NewTheta = atan2(	)
		    this->LX +=  ( ( cos( this->Theta ) * NewSpeed * this->Time ) );
			this->LY +=  ( ( NewSpeed * this->Time * (sin( this->Theta )) ) - ( 0.5 * Gravity * this->Time * this->Time) );						
*/
		}
		else if(this->Rebound == 2)			//collided with the roof
		{
			this->NewTheta = 90 - this->Theta;
			this->vX = Speed*cos(this->NewTheta);   //Vx
			this->vY = Speed*sin(this->NewTheta);	//Vy
			this->aX = -this->k * Speed * this->vX;
			this->aY = (-this->k * Speed * this->vY) - Gravity;
			this->NewVX = this->vX + ( this->aX * deltaT );
			this->NewVY = this->vY + ( this->aY * deltaT );
			this->LX += ( this->NewVX * deltaT ) + ( this->aX * deltaT * deltaT );
			this->LY -= ( this->NewVY * deltaT ) + ( this->aY * deltaT * deltaT );
		}
		else if(this->Rebound == 3)			//collided with ground
		{
			this->NewTheta = 90 - this->Theta;
			this->vX = Speed*cos(this->NewTheta);   //Vx
			this->aX = -this->k * Speed * this->vX;
			this->NewVX = this->vX + ( this->aX * deltaT );
			this->LX += ( this->NewVX * deltaT ) + ( this->aX * deltaT * deltaT ) - ( 0.35 * 0.1 * Gravity );
			this->LY += 0;		
			if(this->LX>=6.0 || this->LX<=-5.8)					
				InAir = 3;

		}
		else if(this->Rebound == 4)		//Collided with leftwall
		{
			this->NewTheta = 90 - this->Theta;
			this->vX = Speed*cos(this->NewTheta);   //Vx
			this->vY = Speed*sin(this->NewTheta);	//Vy
			this->aX = -this->k * Speed * this->vX;
			this->aY = (-this->k * Speed * this->vY) - Gravity;
			this->NewVX = this->vX + ( this->aX * deltaT );
			this->NewVY = this->vY + ( this->aY * deltaT );
			this->LX += ( this->NewVX * deltaT ) + ( this->aX * deltaT * deltaT );
			this->LY += ( this->NewVY * deltaT ) + ( this->aY * deltaT * deltaT );						
		}

		//cout<<"Speed: "<<Speed<<" Time: "<<this->Time<<" Sin(@): "<<(sin( this->Theta ))<<"\n";
		//cout<<this->LX<<" is the x coordinate\n";
		//cout<<this->LY<<" is the y coordinate\n";	

	}

	void drawShell()
	{
			sleep(deltaT);
			this->Current += deltaT;

			glLoadIdentity();
			moveShell();
			//glLoadIdentity();
			if(this->LX>=6.5 || this->LY>=8.0)
				InAir = 3;
			glTranslatef(this->LX,this->LY,-10.0);
			//glRotatef(angle, -1.0f, 1.0f, 0.0f);  // Rotate about (1,1,0)-axis
			glColor3f(1.0f, 0.0f, 0.0f);
			drawCircle(ShellRadius);

	}
};

shell CanonBall;

class Target
{
public:

	float y;
	float x;
	int flag;
	void drawTarget(int i)
	{
		glPushMatrix();
/*		glEnable(GL_NORMALIZE);
		glEnable(GL_COLOR_MATERIAL);
		GLuint ballTextureId;
		Image *ballimage = loadBMP("/home/levicorpus/Downloads/Trial/beachball.bmp");
		ballTextureId = loadTexture(ballimage);
		delete ballimage;
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, ballTextureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); */
		glColor3f(1.0f, 1.0f, 1.0f);
    	glTranslatef(0.0 + 2*i,this->y-2.0,-10.0);
/*		glTexCoord2f(0.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f);
		glTexCoord2f(0.0f, 1.0f); */
  	  	glutSolidSphere(TargetRadius,20,20);
//   	    glColor3f(1.0,1.0,1.0);
    	this->x = i;
    	glPopMatrix();
    	glDisable(GL_TEXTURE_2D);
	}

};
Target ball[10];

//const char* STRS[4] ={"a","b","c","d"};
class Obstacle
{
public:
	//float x = -0.5;
	//float y = -0.9;

	float x;
	float y;

	void initObstacle()
	{
		this->x = -0.5;
		this->y = -0.9;
	}

/*	void drawObstacle(int i)
	{
		glPushMatrix();
		this->x = -0.5 + i;
		this->y = -0.9 + i;
		glTranslatef(-0.5+i,-0.9+i,-5.0);
		glColor3f(0.0,1.0,0.0);
		drawCircle(0.25);
		glRectf(0.25, 0.25, 1.0, 0.50);
		glPopMatrix();
	}
*/
	void drawObstacle(int i)
	{
		glPushMatrix();
			this->x = -0.5 + i;
			this->y = -0.9 + i;
			glTranslatef(this->x,this->y-1.0,-10.0);	
			glScalef(2.0,2.0,2.0);
			glRotatef(90,0.0,0.0,1.0);
			glColor3f(1.0,0.0,0.0);
			drawCircle(0.25);
			
			/* draw eyebrows */
			glPushMatrix();
				glTranslatef(0.22,-0.0,0.2);
				glRotatef(20,0.0,0.0,1.0);
				glColor3f(0.0,0.0,0.0);
				glBegin(GL_QUADS);
					glVertex3f(-0.05f,0.0f,0.0);
					glVertex3f(0.05f,0.0f,0.0);
					glVertex3f(0.05f,0.2f,0.0);
					glVertex3f(-0.05f,0.2f,0.0);
				glEnd();
			glPopMatrix();

			/* draw beak */
			glPushMatrix();
				glTranslatef(0.0,0.20,0.0);
				glRotatef(90, -1.0, 0.0, 0.0);
				//glScalef(0.5,0.0,0.0);
				glColor3f(1.0,1.0,0.0);
				glutSolidCone(0.10, 0.25, 50, 5);
			glPopMatrix();
			
			/* draw tail */
			glPushMatrix();
				glTranslatef(0.0,-0.35,0.0);
				glRotatef(140, -1.0, 0.0, 0.0);
				//glScalef(0.5,0.0,0.0);
				glColor3f(0.0,0.0,0.0);
				glutSolidCone(0.10, 0.25, 50, 5);
			glPopMatrix();

			
			/* draw eyes */
			glPushMatrix();
				glTranslatef(0.1,0.07,0.0);
				//glScalef(0.5,0.0,0.0);
				glColor3f(1.0,1.0,1.0);
				glutSolidSphere(0.08, 50, 5);
			glPopMatrix();


		glPopMatrix();
	}
};

Obstacle obs[10];
void drawCircle(float radius){
	//glPushMatrix();
    //glTranslatef(-2.0f, 0.0f,-5.0f);
    int i;
    float twicePi = 2.0 * 3.142;
    //x = 0, y = 0;
    glBegin(GL_TRIANGLE_FAN); //BEGIN CIRCLE
    //glVertex2f(x, y); // center of circle
    for (i = 0; i < 360; i++)   {
        glVertex2f (
            (radius * float(cos(i * twicePi / 360))), (radius * float(sin(i * twicePi / 360)))
        );
    }
    glEnd(); //END
    //glPopMatrix();
}

void drawRect()
{
	//glPushMatrix();

	//glTranslatef(-2.0f, 0.0f, -5.0f);
	if(mouseAngle == 1)
	{
		mouseX = double(mouseX/WIDTH);
		mouseY = double(mouseY/HEIGHT);
		if(mouseX >= 0.21) 
		{
			angle = -(atan2(mouseY,mouseX)) * 180/PI;
			cout<<"The angle is :"<<angle<<"\n";
		}
		else if(mouseX < 0.21)
		{
			angle = +(atan2(mouseY,mouseX) * 180/PI);
			cout<<"The angle in the left side is :"<<angle<<"\n";
		}

		mouseAngle = 0;

	}	
	glRotatef(angle,0.0f,0.0f,1.0f); //rotating object continuously by angle degree
	//glScalef(2.0f,2.0f,2.0f);
	
		glBegin(GL_QUADS);

			glVertex3f(-0.5f,0.0f,0.0);

			glVertex3f(0.5f,0.0f,0.0);

			glVertex3f(0.3f,1.5f,0.0);

			glVertex3f(-0.3f,1.5f,0.0);

		glEnd();
	//glScalef(0.5f,0.5f,0.5f); //Nullify the effect of scaling
	//glPopMatrix();
	              

}

 
void drawCanon()
{
//Circle
	//glPushMatrix();
		//glLoadIdentity(); //Reset the drawing perspective
		glTranslatef(-4.5f,-3.2f,-5.0f);
//		glTranslatef(0.0f,0.0f,-5.0f);
		//glScalef(0.3f,0.3f,0.3f);
			glColor3f(0.6f, 0.4f, 0.12f);
			drawCircle(0.3);
//Rectangle

/*	glPushMatrix();
		glLoadIdentity();
		glRotatef(-50.0, 1.0, 1.0, 1.0);
    	glTranslatef(-4.0f, 0.0f,-5.0f); */
    		glColor3f(0.0f, 0.0f, 0.0f);
			drawRect();
		//glScalef(1.0/0.3,1.0/0.3,1.0/0.3);

	//glPopMatrix();
/*	glPopMatrix(); 
*/

}


void drawTargets()
{
	for(int i=0;i<NoOfTargets;i++)
		{
			if(ball[i].flag == 1 || ball[i].flag == 0)
				ball[i].drawTarget(i);
		}
}

void drawObstacles()
{
	for(int i=0;i<NoOfObstacles;i++)
	{
		obs[i].drawObstacle(i+0.5);
	}
}

const char* winline = {"You Won!"};
const char* loseline = {"Game Over!"};

void CheckEndGame()
{
	//cout<<"Targets Left: "<<TargetsLeft<<"\n";
	if(TargetsLeft == 0)
	{
		glPushMatrix();
		glLoadIdentity();
		glTranslatef(0.0,0.0,-3.5);
		glScalef(0.6,0.6,0);
		t3dDraw3D(winline, 0, 0, 0.2f);
		glPopMatrix();
//		exit(0);
	}
}
void checkCollision()
{
	/* Check collision with walls */
	float shellX = CanonBall.LX;
	float shellY = CanonBall.LY;
	if(shellX>=6.1)					//Collided with right wall
		CanonBall.Rebound = 1;
	else if(shellY>=3.5)			//collided with roof
		CanonBall.Rebound = 2;
	else if(shellY<=-3.0)			//collided with ground
		CanonBall.Rebound = 3;
	else if(shellX<=-5.8)			//collided with left wall
		CanonBall.Rebound = 4;

	float ballX,ballY;
	float length = 100;
	/* Check collision with obstacles */
	for(int i=0;i<NoOfObstacles;i++)
	{
		ballX = obs[i].x;
		ballY = obs[i].y;
		/*cout<<"target ball X: "<<ballX<<"\n";
		cout<<"target ball Y: "<<ballY<<"\n";
		cout<<"shell X: "<<shellX<<"\n";
		cout<<"shell Y: "<<shellY<<"\n"; */
		if (shellX + ShellRadius + ObstacleRadius > ballX  //check for AABB collision
		&& shellX < ballX + ShellRadius + ObstacleRadius
		&& shellY + ShellRadius + ObstacleRadius > ballY 
		&& shellY < ballY + ShellRadius + ObstacleRadius
		&& ball[i].flag != 3)
		{		
			length = sqrt( ( (shellX-ballX) * (shellX-ballX) ) + ( (shellY-ballY) * (shellY-ballY) ) );
			//cout<<"D: "<<d<<"\n";
			//cout<<"Sum of radii: "<<ShellRadius+TargetRadius<<"\n";
			if( length - (ShellRadius + ObstacleRadius) <= 0.01 )
			{
				//cout<<"collision detected!\n";			
				CanonBall.Rebound = 1;
			}
		}
	}


	/* Check collision with targets */
	float d = 100.0;

	for(int i=0;i<NoOfTargets;i++)
	{
		ballX = ball[i].x;
		ballY = ball[i].y-2.0;
		/*cout<<"target ball X: "<<ballX<<"\n";
		cout<<"target ball Y: "<<ballY<<"\n";
		cout<<"shell X: "<<shellX<<"\n";
		cout<<"shell Y: "<<shellY<<"\n"; */
		if (shellX + ShellRadius + TargetRadius > ballX  //check for AABB collision
		&& shellX < ballX + ShellRadius + TargetRadius
		&& shellY + ShellRadius + TargetRadius > ballY 
		&& shellY < ballY + ShellRadius + TargetRadius
		&& ball[i].flag != 3)
		{		
			d = sqrt( ( (shellX-ballX) * (shellX-ballX) ) + ( (shellY-ballY) * (shellY-ballY) ) );
			//cout<<"D: "<<d<<"\n";
			//cout<<"Sum of radii: "<<ShellRadius+TargetRadius<<"\n";
			if( d - (ShellRadius + TargetRadius) <= 0.01 )
			{
				//cout<<"collision detected!\n";			
				//Increase Score					ballX+1 is the depth factor
				Score += ( ( ballX + 1 ) + 10.0 );
				TargetsLeft-=1;
				//Delete the ball
				ball[i].flag = 3;
				//exit(0);
			}
		}
	}
}

void printScore()
{
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(-6.0,3.0,-5.0);
	glScalef(0.6,0.6,0);
	glColor3f(0.0f, 0.0f, 0.0f);
	stringstream convert; // stringstream used for the conversion
	convert << Score;//add the value of Number to the characters in the stream
	Result = convert.str();//set Result to the content of the stream
	//Result now is equal to "123" 
	t3dDraw3D(Result, 0, 0, 0.2f);
	glPopMatrix();
}
//Called when the window is resized
void handleResize(int w, int h) {
	//Tell OpenGL how to convert from coordinates to pixel values
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION); //Switch to setting the camera perspective
	//Set the camera perspective
	glLoadIdentity(); //Reset the camera
	gluPerspective(45.0 * zoomFactor,                  //The camera angle
				   (double)w / (double)h, //The width-to-height ratio
				   1.0,                   //The near z clipping coordinate
				   200.0);                //The far z clipping coordinate
}

//Computes a scaling value so that the strings
float computeScale(const char* strs[4]) {
	float maxWidth = 0;
	for(int i = 0; i < 4; i++) {
		float width = t3dDrawWidth(strs[i]);
		if (width > maxWidth) {
			maxWidth = width;
		}
	}
	
	return 2.6f / maxWidth;
}

//BACKGROUND IMAGE
// load an image file as a texture

GLuint loadTexture(Image* image) {
	GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D,
				 0,
				 GL_RGB,
				 image->width, image->height,
				 0,
				 GL_RGB,
				 GL_UNSIGNED_BYTE,
				 image->pixels);
	return textureId;
}

GLuint _textureId;
void initRendering() {
/*
//Initializes 3D rendering
void initRendering() {
	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
}
*/

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	TargetsLeft = NoOfTargets;	
	Image *image = loadBMP("/home/levicorpus/Downloads/Trial/rb.bmp");
	_textureId = loadTexture(image);
	delete image;
	t3dInit();

}

void initBG()
{
	/* Coloured Background: */
	
/*	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Set background color to white and opaque
	glClearDepth(1.0f);                   // Set background depth to farthest
*/
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -5.0f);
//	
//	GLfloat ambientLight[] = {0.2f, 0.2f, 0.2f, 1.0f};
//	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
	
//	GLfloat directedLight[] = {0.7f, 0.7f, 0.7f, 1.0f};
//	GLfloat directedLightPos[] = {-10.0f, 15.0f, 20.0f, 0.0f};
//	glLightfv(GL_LIGHT0, GL_DIFFUSE, directedLight);
//	glLightfv(GL_LIGHT0, GL_POSITION, directedLightPos);
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _textureId);
	
	//Bottom
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glColor3f(1.0f, 1.0f, 1.0f);
//	glRotatef(90.0,1.0,0.0,0.0);
	glTranslatef(0.0f,0.0f,-6.0f);

	glBegin(GL_QUADS);
	
//	glNormal3f(0.0, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-8.5f, -4.5f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(8.5f, -4.5f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(8.5f, 4.5f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-8.5f, 4.5f, 0.0f);
	
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void cleanup() {
	t3dCleanup();
}

void drawScene() {
	//Clear information from last draw
	//cout<<Time<<"\n";
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setProjectionMatrix (WIDTH,HEIGHT);
	initBG();
	glMatrixMode(GL_MODELVIEW); //Switch to the drawing perspective
	glPushMatrix();
		glLoadIdentity();
		//glRotatef(_cameraangle,0.0f,1.0f,0.0f); //rotate object by 30 degree with respect to y-axis
		drawObstacles();
		drawTargets();	
		glTranslatef(0.0f, 0.0f, -5.0f);
		drawCanon();
	glPopMatrix(); //DEBUG
	glPushMatrix();
	if(TargetsLeft>0)
	{	
		if (InAir == 1)
		{
			CanonBall.drawShell();
			//cout<<"IN AIR"<<"\n";
			checkCollision();

		}
		else if (InAir == 0)
		{	

			CanonBall.initShell();
			//cout<<"LAUNCHED!"<<"\n";
			//cout<<CanonBall.Start<<"\n";
		}
	}
	glPopMatrix();
	/* Print Scores: */
	printScore();
	CheckEndGame();
	glutSwapBuffers(); //Send the 3D scene to the screen
	angleCircle -= 0.15f;

}


/* A routine for setting the projection matrix. May be called from a resize
   event handler in a typical application. Takes integer width and height 
   dimensions of the drawing area. Creates a projection matrix with correct
   aspect ratio and zoom factor. */
void setProjectionMatrix (int width, int height)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
   //gluPerspective (50.0*zoomFactor, (float)width/(float)height, 1.0, 200.0);
   	glOrtho(-8.0 * zoomFactor + panFactor, 8.0 * zoomFactor + panFactor, -4.0, 4.0, 1.0, 200.0);


}

/* Mouse */

void pan(int x, int y)
{
	if ( isPanning == 1)
	{
		if(x > startX)			//move right
			panFactor -=0.3;
		else if(x < startX)
			panFactor +=0.3;
	}
}


void onMouseClick(int button, int state, int x, int y)
{
	if(state == GLUT_UP)
		isPanning = 0;
	if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)  // PAN
		{
			if(InAir == 3)
			{
				/* Start tracking */
				startX = x;
				startY = y;
				mouseX = x;
				mouseY = y;
				isPanning = 1;
				mouseAngle = 1;				
			}
		}
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if(InAir == 3)
		{
				mouseX = x;
				mouseY = y;
				mouseAngle = 1;				
		}	

	}

	if(button == 3)
	{
		if(zoomFactor >= 0.7)
			zoomFactor -= 0.3;
	}

	if(button == 4)
	{
		if(zoomFactor <= 1.3)
			zoomFactor += 0.3;
			panFactor = 0.0;
	}
	glutPostRedisplay();
}

/*
// mouse
// The GLUT mouse function
void mouse(int button, int state, int x, int y)
{
   // Save the left button state
   if (button == GLUT_LEFT_BUTTON)
   {
      mouseLeftDown = (state == GLUT_DOWN);
//      glutPostRedisplay();  // Left button has changed; redisplay!
   }
   // Save the mouse position   
}


// motion
// The GLUT motion function
void motion(int x, int y)
{
   // We only do anything if the left button is down
   if (mouseLeftDown)
   {
		mouseX = x;
		mouseY = y;
		angle = atan2(mouseY,mouseX);
   }
}

*/


void SpecialInput(int key, int x, int y)
{
	switch(key)
	{
		case GLUT_KEY_UP:
			if(zoomFactor >= 0.7)
				zoomFactor -= 0.3;
		break;
		case GLUT_KEY_DOWN:
			if(zoomFactor <= 1.3)
				zoomFactor += 0.3;
				panFactor = 0.0;

		break;
		case GLUT_KEY_LEFT:
			if( -8.0 * zoomFactor + panFactor >= -8.0)
				panFactor -=0.1;
			break;
		case GLUT_KEY_RIGHT:
			if( 8.0 * zoomFactor + panFactor <= 8.0)
				panFactor +=0.1;

		break;
	}

	glutPostRedisplay();
} 
//Called when a key is pressed 
void handleKeypress(unsigned char key, int x, int y)

{

	switch(key)

	{

		case 27:
			cleanup();
			exit(0);

		case 'a':
		case 'A':
		if(InAir == 3)
		{
			if(angle + 10.0 <= 90.0)
			{
				angle+=10.0f;

				if(angle>360.f)
				{

					angle-=360;

				}
			}
		}
			break;

		case 'b':
		case 'B':
		if(InAir == 3)
		{
			if( angle - 10.0 >= -90.0)
			{				
				angle-=10.0f;

				if(angle>360.f)
				{

					angle-=360;

				}
			}
		}
			break;

		case 32:  //SpaceBar
			InAir = 0;
			break;

		case 'F':
		case 'f': //Increase speed of firing
			Speed += 15;
			break;

		case 'S':
		case 's': //Decrease speed of firing
			Speed -= 15;
			break;
		
		case 'H': //TO MAKE MOVING TARGETS STOP
		case 'h':
			if( StopFlag == 0)
				StopFlag = 1;
			else 
				StopFlag = 0;
			break;
		/*
		case 'z':
		WIDTH=WIDTH-20;
		gluPerspective(45.0,                  //The camera angle
				   (double)WIDTH / (double)HEIGHT, //The width-to-height ratio
				   1.0,                   //The near z clipping coordinate
				   200.0);                //The far z clipping coordinate
				break;

*/
	}

}

/* Called back when timer expired   */
void timer(int value) {

	if( StopFlag == 0)
	{
		for(int i=0;i<NoOfTargets;i++)
		{		
			if(ball[i].flag == 1) //GO DOWN
		        {
					srand (time(NULL));
		            ball[i].y-=0.005 + (0.01 * ( rand() % NoOfTargets ) );
		            ball[i].x-=0.005;
		            if(ball[i].y<-0.0)
		                ball[i].flag=0;
		        }
		        if(ball[i].flag == 0)  //GO UP
		    	{
		      //  	cout<<"Going up\n";
			        ball[i].y+=0.005 + (0.01 * ( rand() % NoOfTargets ) );
			        if(ball[i].y>5.5)
			            ball[i].flag=1;
		    	}
		}
	}
   glutPostRedisplay();      // Post re-paint request to activate display()
   glutTimerFunc(refreshMills, timer, 0); // next timer call milliseconds later
}

int main(int argc, char** argv) {
	//Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 400); //Set the window size

	//Create the window
	glutCreateWindow("CentralPark");

	initRendering(); //Initialize rendering
	//_scale = computeScale(Result);
	//Set handler functions for drawing, keypresses, and window resizes
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutSpecialFunc(SpecialInput);
	glutReshapeFunc(handleResize);
	glutTimerFunc(refreshMills, timer, 0);     // First timer call immediately
	glutMouseFunc(onMouseClick);
	glutMotionFunc(pan);
	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
	return 0; //This line is never reached
}
