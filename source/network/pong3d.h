#ifndef __PONG_3D_H__
#define __PONG_3D_H__

/* include files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> 

#ifdef WIN32 
#include <windows.h> 
#include <io.h> 
#include <winsock.h> 
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#endif 

#include <glut.h>

#ifdef MAINFILE
#define EXTERN
#else
#define EXTERN extern
#endif



 
/*****************************************************************************************/ 
/*                        the network                                                    */ 
/*****************************************************************************************/ 
EXTERN const int PORT = 5309; 
EXTERN int networkFD = 0; 

/* defines */
#define WORLD_SIZE_X 1000
#define WORLD_SIZE_Y 1000
#define WORLD_SIZE_Z 4000

EXTERN const int FRAME_RATE = 1;

EXTERN int WINDOW_X_SIZE = 300; 
EXTERN int WINDOW_Y_SIZE = 300;
EXTERN const int CAMERA_DISTANCE = 1000;
 
/*****************************************************************************************/ 
/*                        the ball                                                       */ 
/*****************************************************************************************/
#define BALL_RADIUS 50
#define BALL_START_VEL_X 0
#define BALL_START_VEL_Y 0
#define BALL_START_VEL_Z 100

#define BALL_SPEED_INC 5

/*****************************************************************************************/ 
/*                        the paddles                                                    */ 
/*****************************************************************************************/
#define PADDLE_WIDTH 200
#define PADDLE_THICKNESS 5
#define X_BOUNCE 10
#define Y_BOUNCE 10

EXTERN const float PI = 3.1415;

EXTERN const int TRUE = 1;
EXTERN const int FALSE = 0;

EXTERN GLint vertex[8][3] = {{0,0,0},{WORLD_SIZE_X,0,0},{WORLD_SIZE_X,WORLD_SIZE_Y,0},{0,WORLD_SIZE_Y,0},{0,0,WORLD_SIZE_Z},{WORLD_SIZE_X,0,WORLD_SIZE_Z},{WORLD_SIZE_X,WORLD_SIZE_Y,WORLD_SIZE_Z},{0,WORLD_SIZE_Y,WORLD_SIZE_Z}};
EXTERN GLint paddle_vertex[12][3] = {{0,0,PADDLE_THICKNESS},{PADDLE_WIDTH,0,PADDLE_THICKNESS},{PADDLE_WIDTH,PADDLE_WIDTH,PADDLE_THICKNESS},{0,PADDLE_WIDTH,PADDLE_THICKNESS},

                                    {PADDLE_WIDTH/3,0,PADDLE_THICKNESS},{PADDLE_WIDTH/3,PADDLE_WIDTH,PADDLE_THICKNESS},{2*PADDLE_WIDTH/3,0,PADDLE_THICKNESS},

                                    {2*PADDLE_WIDTH/3,PADDLE_WIDTH,PADDLE_THICKNESS},{0,PADDLE_WIDTH/3,PADDLE_THICKNESS},{PADDLE_WIDTH,PADDLE_WIDTH/3,PADDLE_THICKNESS},

                                    {0,2*PADDLE_WIDTH/3,PADDLE_THICKNESS},{PADDLE_WIDTH,2*PADDLE_WIDTH/3,PADDLE_THICKNESS}};
 
/* lower center of paddle */ 
#define MY_PADDLE 0 
#define YOUR_PADDLE 1 
EXTERN GLfloat paddle_loc[2][3] = {{WORLD_SIZE_X/2,WORLD_SIZE_Y/2,WORLD_SIZE_Z - PADDLE_THICKNESS }, 
                                 {WORLD_SIZE_X/2,WORLD_SIZE_Y/2,PADDLE_THICKNESS}};   
 
 
/*****************************************************************************************/ 
/*                        material                                                       */ 
/*****************************************************************************************/ 
EXTERN const GLfloat default_specular[] = {0.0,0.0,0.0,1.0}; 
EXTERN const GLfloat default_ambient[] = {0.2,0.2,0.2,1.0}; 
EXTERN const GLfloat default_diffuse[] = {0.8,0.8,0.8,1.0}; 
EXTERN const GLfloat default_shininess[] = {0.0}; 
 
EXTERN const GLfloat ball_specular[] = {1.0,1.0,1.0, 1.0}; 
EXTERN const GLfloat ball_diff_amb[] = {0.3,0.3,0.3, 1.0}; 
EXTERN const GLfloat ball_shininess[] = {90.0}; 
 
/*****************************************************************************************/ 
/*                        lighting                                                       */ 
/*****************************************************************************************/ 
#define LIGHTING_DEFAULT 0 
#define LIGHTING_DISCO 1 
 
EXTERN const GLfloat light_position0[] = {WORLD_SIZE_X-20,WORLD_SIZE_Y-20,WORLD_SIZE_Z-20 ,1}; 
EXTERN const GLfloat light_specular0[] = {1.0,1.0,1.0,1.0}; 
 
EXTERN const GLfloat light_direction1[] = {0.0,-1.0,0.0}; 
EXTERN const GLfloat light_cutoff1 = 90.0; 
EXTERN const GLfloat light_position1[] = {20.0, WORLD_SIZE_Y - 20.0, WORLD_SIZE_Z-20.0,1.0}; 
EXTERN const GLfloat light_color1[] = {0.9,0.0,0.4,1.0}; 
 

EXTERN const int limit[] = {WORLD_SIZE_X, WORLD_SIZE_Y, WORLD_SIZE_Z};

/* starting location for the display list */
EXTERN GLuint startList;
 
/* handy macros */ 
#define BALL startList 
#define PADDLE startList + 1 
#define WORLD startList + 2

/**********************************************************************************/
/*                       State of game                                            */ 
/**********************************************************************************/
#define START   0
#define RUNNING 1
#define PAUSE   2
#define END     3

#define LOCAL 0
#define NETWORK_GUEST 1
#define NETWORK_HOST 2

#define MENU_HOST 0
#define MENU_JOIN 1

EXTERN int GameState;
EXTERN int GameMode;


/* sphere location and motion information */
EXTERN GLdouble ball_pos[3] = {WORLD_SIZE_X/2,WORLD_SIZE_Y/2,WORLD_SIZE_Z/2};
EXTERN GLdouble ball_vel[3] = {BALL_START_VEL_X, BALL_START_VEL_Y,BALL_START_VEL_Z};
 
 
 
/*****************************************************************************************/
/*                        function prototypes                                            */ 
/*****************************************************************************************/

/* graphics */
void display(void);
void init(void);
void initLighting(int model); 
void createDisplayLists();
void initLighting(int model);

void createDisplayLists(void);

/* user interface */
void selectMenu(int num);
void myReshape(GLsizei w, GLsizei h);
void myKeyboard(unsigned char c, int x, int y);
void myMotion(int x, int y);

/* network */
void synchronize(int mode);
int initServerSocket(void );
int initClientSocket(void );

/* misc */
void checkForOpenGLErrors(void);


typedef struct clientstate {
   GLfloat ballLoc[3] ;
   GLint paddleLoc[6][2];  /* array for six players */
                           /* contains distance from the lower left of their side */
} ClientState;

typedef struct serverstate {
   GLfloat ballLoc[3];
   GLfloat ballVel[3];
   GLint paddleLoc[6][2];  /* array for six players */
                           /* contains distance from the lower left of their side */
} ServerState;

#endif
