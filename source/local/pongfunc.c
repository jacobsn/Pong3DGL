#include "pong3d.h" 
 
/*************************************************************************** 
* initGlobals
*
* Initialize global variables
****************************************************************************/
 void initGlobals(void)  {

   /* set target mode */
   TargetMode = OFF;
   target_value = 0;
   spin_x = 1;
   spin_y = 0;

   /* scoring variables */
   target_hits = 0;
   paddle_hits = 0;
   hit_percentage = 0.0;
   score = 0;

   /* set game state */
   GameState = START;

   /* target globals */
   target_pos[0] = WORLD_SIZE_X;
   target_pos[1] = WORLD_SIZE_Y;
   target_pos[2] = WORLD_SIZE_Z/6;

   /* paddle globals */
   paddle_pos[MY_PADDLE][0] = WORLD_SIZE_X/2 - PADDLE_WIDTH/2;
   paddle_pos[MY_PADDLE][1] = WORLD_SIZE_Y/2 - PADDLE_WIDTH/2;
   paddle_pos[MY_PADDLE][2] = WORLD_SIZE_Z - PADDLE_THICKNESS;

   /* ball globals */
   ball_pos[0] = WORLD_SIZE_X/2;
   ball_pos[1] = WORLD_SIZE_Y/2;
   ball_pos[2] = WORLD_SIZE_Z/2;

   ball_vel[0] = BALL_START_VEL_X;
   ball_vel[1] = BALL_START_VEL_Y;
   ball_vel[2] = BALL_START_VEL_Z;

   ball_speed = 0;

   /* world globals */
   limit[0] = WORLD_SIZE_X;
   limit[1] = WORLD_SIZE_Y;
   limit[2] = WORLD_SIZE_Z;
} 
 
/*************************************************************************** 
* Function: createDisplayLists()
*
* This function creates the display lists for the various objects in the
* scene.
****************************************************************************/
void createDisplayLists()  { 
    
   int panel_length = WORLD_SIZE_Z;
   GLUquadricObj *qobj; 

   /* create the display list for the ball */
   startList = glGenLists(4);
   qobj = gluNewQuadric();
   gluQuadricDrawStyle(qobj, GLU_FILL);
    
   glNewList(startList, GL_COMPILE); 
      glMaterialfv(GL_FRONT, GL_SPECULAR, ball_specular); 
      glMaterialfv(GL_FRONT, GL_SHININESS, ball_shininess);
      gluSphere(qobj, BALL_RADIUS,20,20); 
      glMaterialfv(GL_FRONT, GL_DIFFUSE, default_diffuse); 
      glMaterialfv(GL_FRONT, GL_SPECULAR, default_specular); 
      glMaterialfv(GL_FRONT, GL_SHININESS, default_shininess); 
 
   glEndList();

   /* create the display list for the paddle */
   glNewList(startList + 1, GL_COMPILE); 
      glLineWidth(2.0); 
      glDisable(GL_LIGHTING); 
      glBegin(GL_LINE_LOOP);
         glColor3f(1.,0.0,0.0);
         glVertex3iv(paddle_vertex[0]);
         glVertex3iv(paddle_vertex[1]);
         glVertex3iv(paddle_vertex[2]);
         glVertex3iv(paddle_vertex[3]); 
      glEnd();
      glBegin(GL_LINES);
         glVertex3iv(paddle_vertex[4]);
         glVertex3iv(paddle_vertex[5]);
         glVertex3iv(paddle_vertex[6]);
         glVertex3iv(paddle_vertex[7]);
         glVertex3iv(paddle_vertex[8]);
         glVertex3iv(paddle_vertex[9]);
         glVertex3iv(paddle_vertex[10]);
         glVertex3iv(paddle_vertex[11]);
      glEnd();
      glEnable(GL_LIGHTING);
      glLineWidth(1.0);
   glEndList();

   /* create the display list for the world */
   glNewList(startList + 2, GL_COMPILE); 

      /* left wall */
      glBegin(GL_QUAD_STRIP);
      while(panel_length >= 0) {
         glVertex3i(0,0,panel_length);
          glVertex3i(0,WORLD_SIZE_Y,panel_length);
          panel_length -= PANEL_LENGTH;
      }
      glEnd();

      /* ceiling */
      panel_length = WORLD_SIZE_Z;
      glBegin(GL_QUAD_STRIP);
      while(panel_length >= 0) {
          glVertex3i(WORLD_SIZE_X,WORLD_SIZE_Y,panel_length);
          glVertex3i(0,WORLD_SIZE_Y,panel_length);
          panel_length -= PANEL_LENGTH;
      }
      glEnd();

      /* right wall */
      panel_length = WORLD_SIZE_Z;
      glBegin(GL_QUAD_STRIP);
      while(panel_length >= 0) {
          glVertex3i(WORLD_SIZE_X,0,panel_length);
          glVertex3i(WORLD_SIZE_X,WORLD_SIZE_Y,panel_length);
          panel_length -= PANEL_LENGTH;
      }
      glEnd();

      /* floor */
      panel_length = WORLD_SIZE_Z;
      glBegin(GL_QUAD_STRIP);
      while(panel_length >= 0) {
          glVertex3i(WORLD_SIZE_X,0,panel_length);
          glVertex3i(0,0,panel_length);
          panel_length -= PANEL_LENGTH;
      }
      glEnd();

      /* back wall */
      glBegin(GL_QUADS);
      glVertex3iv(vertex[0]);
      glVertex3iv(vertex[1]);
      glVertex3iv(vertex[2]);
      glVertex3iv(vertex[3]); 
      glEnd();
   glEndList();

   /* create ball shadow */
   glNewList(startList + 3, GL_COMPILE);
     /* rotate the shadow parallel to the floor */
     glRotatef(90,1,0,0);
     gluDisk(qobj, 0, BALL_RADIUS, 10, 10);
   glEndList();

   /* create 3D light model */
   glNewList(startList + 4, GL_COMPILE);
     glMaterialfv(GL_FRONT, GL_EMISSION, light_emission);
     gluSphere(qobj, LIGHT_RADIUS, 10, 10);
     glMaterialfv(GL_FRONT, GL_EMISSION, default_emission);
   glEndList();

   /* create target model */
   glNewList(startList + 5, GL_COMPILE);
     gluDisk(qobj, 0, TARGET_RADIUS, 10, 10);
     glMaterialfv(GL_FRONT, GL_DIFFUSE, default_diffuse);
   glEndList();
}
 
/*************************************************************************** 
* Function: init
*
* This function initializes variables for OpenGl
****************************************************************************/ 

void init(void) {
 
   /* set game type */
   GameType = SPEED_GAME;

   initGlobals();

   /* set clear color to black */
   glClearColor (0.0, 0.0, 0.0, 0.0);
   
   /* flat shading */
   glShadeModel (GL_SMOOTH);
   glEnable(GL_DEPTH_TEST); 

   /* create the game objects */
   createDisplayLists();

   glMatrixMode(GL_PROJECTION);

   glLoadIdentity();

   glFrustum(-WORLD_SIZE_X/2,WORLD_SIZE_X/2,-WORLD_SIZE_Y/2,WORLD_SIZE_Y/2, CAMERA_DISTANCE , WORLD_SIZE_Z + CAMERA_DISTANCE + 1);
   glTranslated(-WORLD_SIZE_X/2,-WORLD_SIZE_Y/2,-(WORLD_SIZE_Z + CAMERA_DISTANCE ));
   
   printf("GL_VENDOR is %s\n", glGetString(GL_VENDOR));
   printf("GL_RENDERER is %s\n", glGetString(GL_RENDERER));
   printf("GL_EXTENSIONS is %s\n", glGetString(GL_EXTENSIONS));

   glMatrixMode(GL_MODELVIEW); 
 
   glLoadIdentity(); 
 
   /* setup the lighting */ 
   initLighting(LIGHTING_DEFAULT);
}
  
/*************************************************************************** 
* Function: initLighting
*
* This function initializes variables for the lights in the game 
****************************************************************************/ 
void initLighting(int model) { 
 
   switch( model ) { 
      case LIGHTING_DEFAULT: 
         glEnable(GL_LIGHTING); 
        /* glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);*/ 
 
         glEnable(GL_LIGHT0); 
         glLightfv(GL_LIGHT0, GL_POSITION, light_position0); 
         glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient0); 
         glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_direction0); 
         
         glEnable(GL_LIGHT2); 
         glLightfv(GL_LIGHT2, GL_POSITION, light_position2); 
         glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular2); 
         glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse2); 
         glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, light_direction2); 
         glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, light_cutoff2); 
 
         glEnable(GL_LIGHT3); 
         glLightfv(GL_LIGHT3, GL_POSITION, light_position3); 
         glLightfv(GL_LIGHT3, GL_SPECULAR, light_specular3); 
         glLightfv(GL_LIGHT3, GL_DIFFUSE, light_diffuse3); 
         glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, light_direction3); 
         glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, light_cutoff3); 
          
         /*glEnable(GL_LIGHT1); 
         glLightfv(GL_LIGHT1, GL_POSITION, light_position1); 
         glLightfv(GL_LIGHT1, GL_SPECULAR, light_color1); 
         break; 
         /* 
         glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light_direction1); 
         glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, light_cutoff1); 
         glLightfv(GL_LIGHT1, GL_POSITION, light_position1); 
         glLightfv(GL_LIGHT1, GL_SPECULAR, light_color1); 
         */ 
   } 
} 
