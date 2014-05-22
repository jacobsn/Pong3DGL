/*
 * This program create a window 400 x 400 pixels and draws a couple
 * of lines using the Bresenham algorithm.
 *
 * To terminate the program, press the esacpe key.  The mouse pointer must 
 * be over the window.
 */

#define MAIN_FILE 1

#include "pong3d.h"


/***************************************************************************
* Function: checkForOpenGLErrors
*
* This function is called to check for OpenGl errors
****************************************************************************/
static void checkForOpenGLErrors(void) {
   GLenum error;
   while ((error = glGetError()) != GL_NO_ERROR)
    fprintf(stderr, "GL error: %s\n", gluErrorString(error));
}

/***************************************************************************
* Function: display_text
*
* This function is used to draw text.
* arguments
*     x, y, z : position for text
*     string:   text to be printed
****************************************************************************/
void display_text(float x, float y, float z, char string[])  {

   int i;

   glDisable(GL_LIGHTING);
     glColor3f(1.0,1.0,1.0);
     glRasterPos3f(x, y, z);
     for (i=0;string[i]!='\0';i++){ 
          glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,string[i]);
     }
     glEnable(GL_LIGHTING);
}

/***************************************************************************
* Function: display
*
* This function is called by OpenGl to draw in the window
****************************************************************************/
void display(void) {
   
   char temp[200];
   
   /* clear the color and depth buffer */
   glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


   if (GameState == START)  {

     /* draw the text */
     display_text(WORLD_SIZE_X - 630,
                  WORLD_SIZE_Y- 100,
                  WORLD_SIZE_Z - 30,
                  "Pong 3D");

     display_text(WORLD_SIZE_X - 650,
                  WORLD_SIZE_Y - 175,
                  WORLD_SIZE_Z - 30,
                  "Written By:");

     display_text(WORLD_SIZE_X - 950,
                  WORLD_SIZE_Y - 900,
                  WORLD_SIZE_Z - 30,
                  "Nathan Jacobs & David Schoemehl");

     initGlobals();
   } else if ( GameState == GAMEOVER ) {
      if (GameType == TARGET_GAME)  {
         if(paddle_hits > 0) {
            hit_percentage = ((float)target_hits / (float)paddle_hits) * 100;
            sprintf(temp,"Your hit_percentage was %4.1f\n",hit_percentage);
            display_text(10,10,WORLD_SIZE_Z - 10,temp);
         } else {
            sprintf(temp,"Try hitting the ball next time... OK?\n");
            display_text(10,10,WORLD_SIZE_Z - 10,temp);
         }
            
         sprintf(temp,"Your final score was %d\n", score);
         display_text(10,70,WORLD_SIZE_Z - 10,temp);         

      } else  {
         sprintf(temp,"Your final score was %d\n",score);
         display_text(10,10,WORLD_SIZE_Z - 10,temp);
      } 
   }

      /* draw the target if it's on */
      if (TargetMode == ON || TargetMode == HIT)  {
      target_angle += 30;
      if (target_angle >= 90) target_angle = -90;
        glTranslatef(target_pos[0],target_pos[1],target_pos[2]);
        if (TargetMode == HIT)
          glRotatef(target_angle, spin_x, spin_y, 0);
          glMaterialfv(GL_FRONT, GL_DIFFUSE, target_diffuse[target_value]);
        glCallList(TARGET);
        glLoadIdentity();
      }

      /* shadow */
      glTranslatef(ball_pos[0],1,ball_pos[2]);   /* move the shadow */
      glCallList(BALL_SHADOW);                   /* draw the shadow */
      glLoadIdentity();                          /* clear transformations */

      /* spot light model 1 */
      glTranslatef(WORLD_SIZE_X/2,
                   WORLD_SIZE_Y,
                   WORLD_SIZE_Z-3500);           /* move the light */
      glCallList(SPOT_LIGHT);                    /* draw the light */
      glLoadIdentity();                          /* clear transformations */

      /* spot light model 2 */
      glTranslatef(WORLD_SIZE_X/2,
                   WORLD_SIZE_Y,
                   WORLD_SIZE_Z-1500);           /* move the light */
      glCallList(SPOT_LIGHT);                    /* draw the light */
      glLoadIdentity();                          /* clear transformations */

      /* ball */
      glTranslatef(ball_pos[0],
                   ball_pos[1],
                   ball_pos[2]);                 /* move the ball */
      glMaterialfv(GL_FRONT, GL_DIFFUSE, ball_diffuse[ball_speed]);
       glCallList(BALL);                          /* draw the ball */
       glLoadIdentity();                          /* clear any transformations */

      /* paddle
      /*    draw the paddle if the game is in speed mode or the ball is
            heading in the positive z-direction in TARGET_GAME         */
      if ( GameType == SPEED_GAME || ball_vel[2] > 0 )  {
        glTranslatef(paddle_pos[MY_PADDLE][0], 
                     paddle_pos[MY_PADDLE][1], 
                     paddle_pos[MY_PADDLE][2]);  /* move the paddle */ 
        glCallList(PADDLE);                      /* draw the paddle */ 
        glLoadIdentity();                        /* clear any transformations */ 
      } 
      else  { 
        glDisable(GL_LIGHTING); 
        glLineWidth(2.0); 
        glBegin(GL_LINES); 
          glColor3f(0,1,0); 
          glVertex3i(WORLD_SIZE_X >> 1, WORLD_SIZE_Y >> 1, WORLD_SIZE_Z - PADDLE_THICKNESS); 
          glVertex3i(mouse_x, mouse_y, WORLD_SIZE_Z - PADDLE_THICKNESS); 
        glEnd(); 
        glEnable(GL_LIGHTING); 
      } 
 
      /* draw the world */
      glCallList(WORLD);

   /* flush the buffer */
   glFlush();

   /* swap the buffers */
   glutSwapBuffers();

   checkForOpenGLErrors();

}

/***************************************************************************
* Function: gameMotion
*
* This function controls the motion of the game
****************************************************************************/
void gameMotion(int message)  {
   int i, x_ball, y_ball, seed;

   if(GameType == TARGET_GAME && ball_vel[2] < 0 && TargetMode != HIT)  {
     /* check to see if the ball has hit the target */
     if ( ball_pos[2] >= target_pos[2] - TARGET_RADIUS &&
          ball_pos[2] <= target_pos[2] + TARGET_RADIUS &&
          ball_pos[0] >= target_pos[0] - TARGET_RADIUS &&
          ball_pos[0] <= target_pos[0] + TARGET_RADIUS &&
          ball_pos[1] >= target_pos[1] - TARGET_RADIUS &&
          ball_pos[1] <= target_pos[1] + TARGET_RADIUS)  {
       /* increment score */
       score += target_values[target_value];
       target_hits++;
       ball_speed--;
       if(ball_speed < 0) ball_speed = 0;
       ball_vel[2] = ball_speeds[ball_speed];
       TargetMode = HIT;
       target_angle = 0;
     }
   }
   
   /* check the x and y position of the ball, if they exceed the wall limit
      then reverse their direction */
   for(i = 0; i < 2; i++)  {
      if(TargetMode == ON && ball_vel[2] < 0)
        ball_vel[i] += control_vector[i] >> 6;
      ball_pos[i] += ball_vel[i];
      if (ball_pos[i] + BALL_RADIUS >= limit[i] || ball_pos[i] - BALL_RADIUS <= 0) {
         ball_vel[i] = -ball_vel[i]; 
         ball_pos[i] += ball_vel[i]; 
      }
   }

   /* move the ball's position in the z direction */
   ball_pos[2] += ball_vel[2];

   /* check to see if the ball has exceed the z limit */
   if (ball_pos[2] + BALL_RADIUS >= limit[2]) {

     /* find ball pos relative to paddle location */
     x_ball = ball_pos[0] - paddle_pos[MY_PADDLE][0];
     y_ball = ball_pos[1] - paddle_pos[MY_PADDLE][1];

       /* check to see if the ball is within the paddle's location */
       if (x_ball >= 0 && x_ball <= PADDLE_WIDTH && y_ball >= 0 && y_ball <= PADDLE_WIDTH) {

       /* increment the number of paddle hits */
       paddle_hits++;

       /* turn on new target if the ball has hit the paddle */
       if(GameType == TARGET_GAME)  {
         seed = time(NULL);
         srand(seed);
         spin_x = (spin_x == 1) ? 0:1;
         spin_y = (spin_x == 1) ? 0:1;
         target_pos[0] =  (rand() % (WORLD_SIZE_X - (TARGET_RADIUS << 1))) + TARGET_RADIUS;
         target_pos[1] =  (rand() % (WORLD_SIZE_Y - (TARGET_RADIUS << 1))) + TARGET_RADIUS;
         if(TargetMode == HIT)  {
           target_value++;
           if (target_value > 2) target_value = 2;
         }
         TargetMode = ON;

         /* change z direction of the ball */
         ball_vel[2] = -ball_vel[2];
         ball_pos[2] += ball_vel[2];
         
       }

         if (GameType == SPEED_GAME)  {

           /* increment the z vector speed */
           ball_vel[2] = -(ball_vel[2] + BALL_SPEED_INC);
           score += BALL_SPEED_INC;

           /* x sectors */
           if (x_ball > paddle_vertex[6][0]) {
             ball_vel[0] += X_BOUNCE;
           }
           else if (x_ball < paddle_vertex[4][0]) {
             ball_vel[0] -= X_BOUNCE;
           }

           /* y sectors */
           if (y_ball  > paddle_vertex[10][1]) {
             ball_vel[1] += Y_BOUNCE;
           }
           else if (y_ball  < paddle_vertex[8][1]) {
             ball_vel[1] -= Y_BOUNCE;
           }
         }
       }
         else  {
          /* paddle missed */
          GameState = GAMEOVER;
         }
   }
   /* check to see if the ball has hit the back wall */
   else if (ball_pos[2] - BALL_RADIUS <= 0)  {

         /* turn target off if it hasn't been hit */
         if(GameType == TARGET_GAME) {
           if (TargetMode != HIT)  {
             target_value = 0;
             TargetMode = OFF;
             ball_speed++;
             if(ball_speed > 2) ball_speed = 2;
             ball_vel[2] = -ball_speeds[ball_speed];
           }
         }
         /* reverse the direction of the ball */
         ball_vel[2] = -ball_vel[2];
         ball_pos[2] += ball_vel[2];
   }

   /* display the new information */
   glutPostRedisplay();

   /* call this function again if the game is running */
   if (GameState == RUNNING)
     glutTimerFunc(FRAME_RATE, gameMotion, 0);

}



/***************************************************************************
* Function: keyboard
*
* This function reads input from the keyboard
****************************************************************************/
void myKeyboard(unsigned char key, int x, int y) {

  switch (key)
     {
     case 27:       /* esc key, pause the program */
       if (GameState == RUNNING)
         GameState = PAUSE;
       else {
       	GameState = RUNNING;
         gameMotion(0);
       }
       break;
     case 32:     /* space bar, take input from keyboard */
       if ( GameState == START )  {
       	 GameState = RUNNING;
         gameMotion(0);
       } else if (GameState == RUNNING )
         GameState = START;
       else if( GameState == GAMEOVER ) {
         GameState = START;
         glutPostRedisplay();
       }
      
       
       break;
     }

} 
 
/*************************************************************************** 
* Function: myMotion 
* 
****************************************************************************/
void myPassiveMotion(int x, int y)  { 
 
   /* map the mouse to world coord. */ 
   mouse_x = x * WORLD_SIZE_X / WINDOW_X_SIZE; 
   mouse_y = WORLD_SIZE_Y - y * WORLD_SIZE_Y / WINDOW_Y_SIZE; 
 
   /* set the paddle location in the x direction */ 
   paddle_pos[MY_PADDLE][0] = mouse_x - (PADDLE_WIDTH >> 1); 
   if (paddle_pos[MY_PADDLE][0] > WORLD_SIZE_X - PADDLE_WIDTH) 
      paddle_pos[MY_PADDLE][0] = WORLD_SIZE_X - PADDLE_WIDTH; 
   else if (paddle_pos[MY_PADDLE][0] < 0) 
      paddle_pos[MY_PADDLE][0] = 0; 
 
   /* set the paddle location in the y direction */ 
   paddle_pos[MY_PADDLE][1] = mouse_y - (PADDLE_WIDTH >> 1); 
   if (paddle_pos[MY_PADDLE][1] > WORLD_SIZE_Y - PADDLE_WIDTH) 
      paddle_pos[MY_PADDLE][1] = WORLD_SIZE_Y - PADDLE_WIDTH; 
   else if (paddle_pos[MY_PADDLE][1] < 0) 
      paddle_pos[MY_PADDLE][1] = 0; 
}

/***************************************************************************
* Function: myMouse 
* 
****************************************************************************/
void myMouse(int button, int state, int x, int y)  {

   if (state == GLUT_DOWN) {
     /* map the mouse to world coord. */
     mouse_x = x * WORLD_SIZE_X / WINDOW_X_SIZE; 
     mouse_y = WORLD_SIZE_Y - y * WORLD_SIZE_Y / WINDOW_Y_SIZE;

     /* change the control vector */
     control_vector[0] = mouse_x - (WORLD_SIZE_X >> 1);
     control_vector[1] = mouse_y - (WORLD_SIZE_Y >> 1);
   }
   else if (state == GLUT_UP) {
     control_vector[0] = 0;
     control_vector[1] = 0;
   }

}

/***************************************************************************
* Function: myButtonMotion 
* 
****************************************************************************/
void myMotion(int x, int y)  {

     /* map the mouse to world coord. */
     mouse_x = x * WORLD_SIZE_X / WINDOW_X_SIZE; 
     mouse_y = WORLD_SIZE_Y - y * WORLD_SIZE_Y / WINDOW_Y_SIZE;

     /* change the control vector */
     control_vector[0] = mouse_x - (WORLD_SIZE_X >> 1);
     control_vector[1] = mouse_y - (WORLD_SIZE_Y >> 1);

}
/***************************************************************************
* Function: myReshape
*
* This function is by OpenGl when the window is reshaped
****************************************************************************/
void myReshape(GLsizei w, GLsizei h) {

   /* set the viewport to the size of the X-window */
   glViewport(0, 0, w, h); 
 
   WINDOW_X_SIZE = w; 
   WINDOW_Y_SIZE = h;

}

 /***************************************************************************
* selectGameType
*
* Initialize variables, menus, etc... and enter the main gl loop
****************************************************************************/
void selectGameType(int selection)  {

  switch(selection)  {
    case SPEED_GAME:
      GameType = SPEED_GAME;
      break;
    case TARGET_GAME:
      GameType = TARGET_GAME;
      break;
  }
}

/***************************************************************************
* selectMainMenu
*
* Initialize variables, menus, etc... and enter the main gl loop
****************************************************************************/
void selectMainMenu(int selection)  {
}

/***************************************************************************
* MAIN LOOP
*
* Initialize variables, menus, etc... and enter the main gl loop
****************************************************************************/
int main(int argc, char** argv) {

   int gameTypeMenu;

   /* use glut to create a 400x400 window with double buffering, rgb color */
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
   glutInitWindowSize (WINDOW_X_SIZE, WINDOW_Y_SIZE);
   glutInitWindowPosition (100, 100);
   glutCreateWindow(argv[0]);
   glutSetCursor(GLUT_CURSOR_NONE); 
   glutSetWindowTitle("Pong 3D");
   
   /* initialize variables for the game */
   init();

   /* callback functions */
   glutDisplayFunc(display);
   glutReshapeFunc (myReshape);
   glutKeyboardFunc(myKeyboard);
    glutMouseFunc(myMouse);
    glutMotionFunc(myMotion);
    glutPassiveMotionFunc(myPassiveMotion); 

   /* create the game type selection menu */
   gameTypeMenu = glutCreateMenu(selectGameType);
   glutAddMenuEntry("Speed Game", SPEED_GAME);
   glutAddMenuEntry("Target Game", TARGET_GAME);

   /* create the main menu */
   glutCreateMenu(selectMainMenu);

   /* add the sub menu for game type selection */
   glutAddSubMenu("Game Types", gameTypeMenu);

   /* when right mouse button is pressed, menu will appear */
   glutAttachMenu(GLUT_RIGHT_BUTTON);

   glutMainLoop();

   return(0);
}
