/* include files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <glut.h>

/* defines */
#define WORLD_SIZE_X 1000
#define WORLD_SIZE_Y 1000
#define WORLD_SIZE_Z 4000
#define PANEL_LENGTH 500

#define START   0
#define RUNNING 1
#define PAUSE   2
#define GAMEOVER 3

#define FRAME_RATE 30

#ifdef MAIN_FILE
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN int WINDOW_X_SIZE = 400;
EXTERN int WINDOW_Y_SIZE = 400;
#define CAMERA_DISTANCE 1000

EXTERN int control_vector[2];
EXTERN int mouse_x;
EXTERN int mouse_y;

/*****************************************************************************************/
/*                        the target                                                     */
/*****************************************************************************************/
#define ON  1
#define OFF 0
#define HIT 2
#define TARGET_RADIUS 100

/* state of target */
EXTERN int TargetMode;
EXTERN int target_angle;
EXTERN int spin_x;
EXTERN int spin_y;
EXTERN int target_pos[3];
EXTERN int target_value;
EXTERN int target_values[] = {5,10,15};

EXTERN int target_hits;
EXTERN int paddle_hits;
EXTERN float hit_percentage;
EXTERN int score;
/*****************************************************************************************/
/*                        the lights                                                     */
/*****************************************************************************************/
#define LIGHT_RADIUS 75

/*****************************************************************************************/
/*                        the ball                                                       */
/*****************************************************************************************/
#define BALL_RADIUS 40
#define BALL_START_VEL_X 0
#define BALL_START_VEL_Y 0
#define BALL_START_VEL_Z 80
#define BALL_SPEED_INC 3
#define SPEED_INC_TARGET 10

EXTERN int ball_speed;
EXTERN int ball_speeds[3] = {50, 75, 100};

/*****************************************************************************************/
/*                        the paddles                                                    */
/*****************************************************************************************/
#define PADDLE_WIDTH 200
#define PADDLE_THICKNESS 5
#define X_BOUNCE 10
#define Y_BOUNCE 10

#define PI 3.1415
#define TRUE 1
#define FALSE 0

EXTERN GLint vertex[8][3] = {{0,0,0},{WORLD_SIZE_X,0,0},{WORLD_SIZE_X,WORLD_SIZE_Y,0},{0,WORLD_SIZE_Y,0},{0,0,WORLD_SIZE_Z},{WORLD_SIZE_X,0,WORLD_SIZE_Z},{WORLD_SIZE_X,WORLD_SIZE_Y,WORLD_SIZE_Z},{0,WORLD_SIZE_Y,WORLD_SIZE_Z}};
EXTERN GLint paddle_vertex[12][3] = {{0,0,PADDLE_THICKNESS},{PADDLE_WIDTH,0,PADDLE_THICKNESS},{PADDLE_WIDTH,PADDLE_WIDTH,PADDLE_THICKNESS},{0,PADDLE_WIDTH,PADDLE_THICKNESS},

                                    {PADDLE_WIDTH/3,0,PADDLE_THICKNESS},{PADDLE_WIDTH/3,PADDLE_WIDTH,PADDLE_THICKNESS},{2*PADDLE_WIDTH/3,0,PADDLE_THICKNESS},

                                    {2*PADDLE_WIDTH/3,PADDLE_WIDTH,PADDLE_THICKNESS},{0,PADDLE_WIDTH/3,PADDLE_THICKNESS},{PADDLE_WIDTH,PADDLE_WIDTH/3,PADDLE_THICKNESS},

                                    {0,2*PADDLE_WIDTH/3,PADDLE_THICKNESS},{PADDLE_WIDTH,2*PADDLE_WIDTH/3,PADDLE_THICKNESS}};

/* lower center of paddle */
#define MY_PADDLE 0
#define YOUR_PADDLE 1
EXTERN int paddle_pos[2][3];



/*****************************************************************************************/
/*                        materials                                                      */
/*****************************************************************************************/
EXTERN GLfloat default_specular[] = {0.,0.,0.,1.};
EXTERN GLfloat default_ambient[] = {.2,.2,.2,1.};
EXTERN GLfloat default_diffuse[] = {.8,.8,.8,1.};
EXTERN GLfloat default_shininess[] = {0.0};
EXTERN GLfloat default_emission[] ={0.0,0.0,0.0,1.0};

EXTERN GLfloat ball_specular[] = {1.,1.,1., 1.};
EXTERN GLfloat ball_diff_amb[] = {.3,.3,.3, 1.};
EXTERN GLfloat ball_shininess[] = {90.};
EXTERN GLfloat ball_diffuse[3][4] = {{1.0, 0.3, 0.3, 0.9},{0.9, 0.6, 0.0, 0.9},{0.2, 0.7, 0.2, 0.9}};

EXTERN GLfloat floor_diffuse[] = {.5, .2, .9, 1};

EXTERN GLfloat light_emission[] ={.2,.2,.2,1.0};

EXTERN GLfloat target_specular[] = {.5,.5,.5, 1.};
EXTERN GLfloat target_shininess[] = {90.};
EXTERN GLfloat target_diffuse[3][4] =  {{1.0, 0.2, 0.2, 0.9},{0.2, 1.0, 0.2, 0.9},{0.2, 0.2, 1.0, 0.9}};

/*****************************************************************************************/
/*                        lighting                                                       */
/*****************************************************************************************/
#define LIGHTING_DEFAULT 0
#define LIGHTING_DISCO 1

EXTERN GLfloat light_position0[] = {WORLD_SIZE_X-20,WORLD_SIZE_Y-20,WORLD_SIZE_Z-20,1};
EXTERN GLfloat light_direction0[] = {0,-1,0};
EXTERN GLfloat light_ambient0[] = {.1,.1,.1,1.0};

EXTERN GLfloat light_direction1[] = {0,-1,0};
EXTERN GLfloat light_cutoff1 = 90.;
EXTERN GLfloat light_position1[] = {20, WORLD_SIZE_Y - 20, WORLD_SIZE_Z-20,1};
EXTERN GLfloat light_color1[] = {0.9,.0,.4,1.};

EXTERN GLfloat light_position2[] = {WORLD_SIZE_X/2,WORLD_SIZE_Y-20,WORLD_SIZE_Z-3500 ,1};
EXTERN GLfloat light_direction2[] = {0,-1,0};
EXTERN GLfloat light_cutoff2 = 45.;
EXTERN GLfloat light_specular2[] = {1,1,1,1};
EXTERN GLfloat light_diffuse2[] = {1.,1.,1.,1.};

EXTERN GLfloat light_position3[] = {WORLD_SIZE_X/2,WORLD_SIZE_Y-20,WORLD_SIZE_Z-1500 ,1};
EXTERN GLfloat light_direction3[] = {0,-1,0};
EXTERN GLfloat light_cutoff3 =45.;
EXTERN GLfloat light_specular3[] = {1,1,1,1};
EXTERN GLfloat light_diffuse3[] = {1.,1.,1.,1.};

EXTERN int limit[3];

/* starting location for the display list */
EXTERN int startList;

/* handy macros */
#define BALL startList
#define PADDLE startList + 1
#define WORLD startList + 2
#define BALL_SHADOW startList + 3
#define SPOT_LIGHT startList + 4
#define TARGET startList + 5  

/* State of game */
EXTERN int GameState;

/* type of game */
EXTERN int GameType;
#define SPEED_GAME  0
#define TARGET_GAME 1

/* sphere location and motion information */
EXTERN int ball_pos[3];
EXTERN int ball_vel[3];



/*****************************************************************************************/
/*                        function prototypes                                            */
/*****************************************************************************************/
void display(void);
void init(void);
void initLighting(int model);
void myReshape(GLsizei w, GLsizei h);
void myKeyboard(unsigned char c, int x, int y);
void myPassiveMotion(int x, int y);
void myMouse(int button, int state, int x, int y);
void myMotion(int x, int y);
void createDisplayLists();
void initLighting(int model);
void initGlobals(void);
