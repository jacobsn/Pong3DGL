/*
 * This program create a window 400 x 400 pixels and draws a couple
 * of lines using the Bresenham algorithm.
 *
 * To terminate the program, press the esacpe key.  The mouse pointer must 
 * be over the window.
 */

#define MAINFILE 1
#include "pong3d.h"


/***************************************************************************
* Function: gameMotion
*
* This function controls the motion of the game
****************************************************************************/
void gameMotion(int message)  {
   int i, x_ball, y_ball;


   if( GameMode != NETWORK_GUEST ) {

       /* check the x and y position of the ball, if they exceed the wall limit
          then reverse their direction */
       for(i = 0; i < 2; i++)  {
          ball_pos[i] += ball_vel[i];
          if (ball_pos[i] + BALL_RADIUS >= limit[i] || ball_pos[i] - BALL_RADIUS <= 0) {
             ball_vel[i] = -ball_vel[i]; 
             ball_pos[i] += ball_vel[i]; 
          }
       }

       /* move the ball's position in the z direction */
       ball_pos[2] += ball_vel[2];

       /* check to see if the ball has exceed the z limit */
       if (ball_pos[2] + BALL_RADIUS >= limit[2] ) {

         /* find ball pos relative to paddle location */
         x_ball = ball_pos[0] - paddle_loc[MY_PADDLE][0];
         y_ball = ball_pos[1] - paddle_loc[MY_PADDLE][1];

           /* check to see if the ball is within the paddle's location */
           if (x_ball >= 0 && x_ball <= PADDLE_WIDTH && y_ball >= 0 && y_ball <= PADDLE_WIDTH) {

             /* increment the z vector speed */
             ball_vel[2] = -(ball_vel[2] + BALL_SPEED_INC);
             ball_pos[2] += ball_vel[2];

             /* x sectors */
             if (x_ball > paddle_vertex[6][0]) {
               ball_vel[0] += X_BOUNCE;
             } else if (x_ball < paddle_vertex[4][0]) {
               ball_vel[0] -= X_BOUNCE;
             }

             /* y sectors */
             if (y_ball  > paddle_vertex[10][1]) {
               ball_vel[1] += Y_BOUNCE;
             } else if (y_ball  < paddle_vertex[8][1]) {
               ball_vel[1] -= Y_BOUNCE;
             }
           } else  {
              /* paddle missed */
              GameState = START;
           }
       } else if (ball_pos[2] - BALL_RADIUS <= 0)  {
          /* handle your opponents paddle */
          if ( GameMode == LOCAL || ball_pos[0] >= paddle_loc[YOUR_PADDLE][0]
             && ball_pos[0] <= (paddle_loc[YOUR_PADDLE][0] + PADDLE_WIDTH)
             && ball_pos[1] >= paddle_loc[YOUR_PADDLE][1]
             && ball_pos[1] <= (paddle_loc[YOUR_PADDLE][1] + PADDLE_WIDTH))  { 
             ball_vel[2] = -ball_vel[2];
             ball_pos[2] += ball_vel[2];
          } else  {
             GameState = START;
          }
      }
   
   }
   
   synchronize(GameMode);
   
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

   /* as a guest you cannot control game */
   if( GameMode == NETWORK_GUEST)
      return;

   switch (key)
   {
      case 27:       /* esc key, pause the program */
         if (GameState == RUNNING) {
            GameState = PAUSE;
         } else {
            GameState = RUNNING;
            gameMotion(0);
         }
         break;
     case 32:     /* space bar, take input from keyboard */
         if (GameState == START)  {
            GameState = RUNNING;
            
                gameMotion(0);
         }
         else if (GameState == RUNNING)
            GameState = START;
         break;
   }
} 
 
/*************************************************************************** 
* Function: myMotion 
* 
****************************************************************************/
void myMotion(int x, int y)  { 

   /* set paddle x */
   paddle_loc[MY_PADDLE][0] = (x * (float)WORLD_SIZE_X / (float)WINDOW_X_SIZE) - (float)PADDLE_WIDTH/2; 
   if (paddle_loc[MY_PADDLE][0] > WORLD_SIZE_X - PADDLE_WIDTH) 
      paddle_loc[MY_PADDLE][0] = WORLD_SIZE_X - PADDLE_WIDTH; 
   else if (paddle_loc[MY_PADDLE][0] < 0) 
      paddle_loc[MY_PADDLE][0] = 0; 
      
   /* set paddle y */
   paddle_loc[MY_PADDLE][1] = WORLD_SIZE_Y - y * ((float)WORLD_SIZE_Y / (float)WINDOW_Y_SIZE) - (float)PADDLE_WIDTH/2; 
   if (paddle_loc[MY_PADDLE][1] > WORLD_SIZE_Y - PADDLE_WIDTH) 
      paddle_loc[MY_PADDLE][1] = WORLD_SIZE_Y - PADDLE_WIDTH; 
   else if (paddle_loc[MY_PADDLE][1] < 0) 
      paddle_loc[MY_PADDLE][1] = 0; 
}

/***************************************************************************
* selectMenu
*
****************************************************************************/
void selectMenu(int num ) {
   int fd;

   switch( num )
   {
      case MENU_HOST:
         GameMode = NETWORK_HOST;
         
         printf("Setting up to host the game -- waiting for connection...\n");
         
         if( 0 > ( networkFD = initServerSocket() ) ) {
            printf("Not able to host a game -- default to local mode\n");
            GameMode = LOCAL;
         } else {
            printf("Lets get ready to rumble\n");
         }
         
         break;
      
      case MENU_JOIN:
         GameMode = NETWORK_GUEST;
         
         if( 0 > ( networkFD = initClientSocket() ) ) {
            printf("Unable to connect to host -- default to local mode\n");
            GameMode = LOCAL;
         } else {
            printf("Lets get ready to rumble\n");
            GameState = RUNNING;
            gameMotion(0);
         }
         
         break;
   }
}


/***************************************************************************
* MAIN LOOP
*
* Initialize variables, menus, etc... and enter the main gl loop
****************************************************************************/
int main(int argc, char** argv) {

   /* use glut to create a 400x400 window with double buffering, rgb color */
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
   glutInitWindowSize (WINDOW_X_SIZE, WINDOW_Y_SIZE);
   glutInitWindowPosition (100, 100);
   glutCreateWindow(argv[0]);
   glutSetCursor(GLUT_CURSOR_NONE); 
   
   init();

   glutDisplayFunc(display);

   glutReshapeFunc (myReshape);

   glutKeyboardFunc(myKeyboard);
 
   glutPassiveMotionFunc(myMotion); 

   /* define menus */
   glutCreateMenu(selectMenu);
   glutAddMenuEntry("Host Game", MENU_HOST);
   glutAddMenuEntry("Join Game", MENU_JOIN);
   
   glutAttachMenu(GLUT_RIGHT_BUTTON);
   
   glutMainLoop();

   return(0);
}
