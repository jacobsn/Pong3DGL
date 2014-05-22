#include "pong3d.h"
 

/***************************************************************************
* Function: display
*
* This function is called by OpenGl to draw in the window
****************************************************************************/
void display(void) {

   /* clear the color and depth buffer */
   glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   if (GameState == START)  {
     /*set ball location to the center of the room */
     ball_pos[0] = WORLD_SIZE_X/2;
     ball_pos[1] = WORLD_SIZE_Y/2;
     ball_pos[2] = WORLD_SIZE_Z/2;

     /* give the ball its starting velocities */
     ball_vel[0] = BALL_START_VEL_X;
     ball_vel[1] = BALL_START_VEL_Y;
     ball_vel[2] = BALL_START_VEL_Z;

     /* clear any transformations */
     glLoadIdentity();

     /* move the ball to the center of the room */
     glTranslatef(ball_pos[0],ball_pos[1],ball_pos[2]);

     /* draw the ball */
     glCallList(BALL);

     /* clear any transformations */
     glLoadIdentity();

     /* move player 1's paddle to its starting location */ 
     glTranslatef(paddle_loc[MY_PADDLE][0],paddle_loc[MY_PADDLE][1],paddle_loc[MY_PADDLE][2]); 


     /* draw the paddle */

     glCallList(PADDLE);
 
     /* clear any transformations */
     glLoadIdentity(); 
 
     /* move player 2's paddle to its starting location */ 
     glTranslatef(paddle_loc[YOUR_PADDLE][0],paddle_loc[YOUR_PADDLE][1],paddle_loc[YOUR_PADDLE][2]); 
 
     /* draw the paddle */ 
     glCallList(PADDLE); 
 
     /* clear any transformations */ 
     glLoadIdentity(); 
 
     /* draw the world */
     glCallList(WORLD);
    
   } else if ( GameState == RUNNING || GameState == PAUSE )  {


      /* move the ball */
      glTranslatef(ball_pos[0],ball_pos[1],ball_pos[2]);

      /* draw the ball */
      glCallList(BALL);
 
      /* draw helper lines 
      glLineWidth(1.); 
      glDisable(GL_LIGHTING); 
      glBegin(GL_LINES); 
         glColor3f(1.,0.0,0.0); 
         glVertex3d(0,0,0); 
         glVertex3d(20*ball_vel[0],20*ball_vel[1], 20*ball_vel[2]); 
      glEnd(); 
      glEnable(GL_LIGHTING); */ 
 
      /* clear any transformations */
      glLoadIdentity();

      /* draw MY paddle */
      glTranslatef(paddle_loc[MY_PADDLE][0],paddle_loc[MY_PADDLE][1],paddle_loc[MY_PADDLE][2]); 
      glCallList(PADDLE); 
 
      /* clear any transformations */ 
      glLoadIdentity(); 
 
      /* draw THE LOSERS paddle */ 
      glTranslatef( paddle_loc[YOUR_PADDLE][0],paddle_loc[YOUR_PADDLE][1],paddle_loc[YOUR_PADDLE][2]); 
      glCallList(PADDLE); 
 
      /* clear any transformations */ 
      glLoadIdentity(); 
       
      /* draw the world */
      glCallList(WORLD);
   }

   /* flush the buffer */
   glFlush(); 

   /* swap the buffers */
   glutSwapBuffers();

   checkForOpenGLErrors();
}

/***************************************************************************
* Function: synchronize
*
****************************************************************************/
void synchronize(int mode) {

   if( mode == LOCAL ) 
      return;
      
   /* send information to the client */
   if( mode == NETWORK_HOST ) {
   
      write(networkFD, ball_pos, sizeof(GLdouble) * 3 ); 
      write(networkFD, paddle_loc[0], sizeof(GLfloat) * 2);
      
      /* get opponents paddle location */
      read(networkFD, paddle_loc[1], sizeof(GLfloat) * 2);
      
      /* swap the x values to make it line up correctly */
      paddle_loc[1][0] = WORLD_SIZE_X - paddle_loc[1][0] - PADDLE_WIDTH;
      
   }
   
   
   if( mode == NETWORK_GUEST ) {
   
      write(networkFD, paddle_loc[0], sizeof(GLfloat) * 2);
   
      read(networkFD, ball_pos, sizeof(GLdouble) * 3);

      /* swap the x and z values */
      ball_pos[2] = WORLD_SIZE_Z - ball_pos[2];
      ball_pos[0] = WORLD_SIZE_X - ball_pos[0];
     
      read(networkFD, paddle_loc[1], sizeof(GLfloat) * 2);
      
      /* swap the x values to make it line up correctly */
      paddle_loc[1][0] = WORLD_SIZE_X - paddle_loc[1][0] - PADDLE_WIDTH;  
      
   }
   
}

/***************************************************************************
* Function: initServerSocket
*
****************************************************************************/
int initServerSocket() { 
   struct sockaddr_in address; 
    
   int sock, conn, i; 
   size_t addrLength = sizeof(struct sockaddr_in); 
    
   if( (sock = socket(PF_INET, SOCK_STREAM, 0)) < 0 ) { 
      printf("Could not create socket\n");
      return -1;
   } 
   
   i = 1;
   setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));
   
   /* initialize the socket address */ 
   address.sin_family = AF_INET; 
   address.sin_port = htons(PORT); 
   memset(&address.sin_addr,0,sizeof(address.sin_addr)); 
    
   if( bind(sock, (struct sockaddr *) &address, sizeof(address))) { 
      printf("Could not bind to address\n");
      return -1; 
   } 
    
   if( listen( sock, 1) ) { 
      printf("Not allowed to listen\n"); 
      return -1;
   } 
    
   if( 0 >= (conn = accept(sock,(struct sockaddr *) &address, (int *)&addrLength))) { 
      printf("Could not accept connections\n"); 
      return -1;
   } 
 
   return conn;
} 


/***************************************************************************
* Function: initClientSocket
*
****************************************************************************/
int initClientSocket() { 
   struct sockaddr_in address; 
   struct in_addr inaddr;
   struct hostent * host;
   int sock;
   
   char hostname[512];
   
   /* read in the host name */
   printf("\nEnter host name: ");
   scanf("%s", hostname);

   printf("Attempting to connect to %s\n",hostname);

   if( inet_aton(hostname, &inaddr))
      host = gethostbyaddr((void *) &inaddr, sizeof(inaddr), AF_INET);
   else
      host = gethostbyname((char *) hostname);
      
   if( !host ) {
      /* we can't find the IP number */
      printf("Could not open the address you specified");
      return -1;
   } else {
      printf("Connecting to host\n");
   }
   
   if((sock = socket(PF_INET, SOCK_STREAM ,0)) < 0 ) {
      printf("Could not open connection.\n");
      return -1;
   }
   
   address.sin_family = AF_INET;
   address.sin_port = PORT;
   
   memcpy(&address.sin_addr, host->h_addr, sizeof(address.sin_addr));
   
   if( connect( sock,  (struct sockaddr *) &address, sizeof(address))) {
      printf("Could not create a connection to the host.\n");
      return -1;
   }
   
   return sock;

} 



/***************************************************************************
* Function: init
*
* This function initializes variables for OpenGl
****************************************************************************/ 

void init(void) {
 

   /* set game state */
   GameState = START;
   GameMode = LOCAL;
 
   /* set clear color to black */
   glClearColor (0.0, 0.0, 0.0, 0.0);
   
   /* flat shading */
   glShadeModel (GL_SMOOTH);
   glEnable(GL_DEPTH_TEST); 

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

void initLighting(int model) { 
 
   switch( model ) { 
      case LIGHTING_DEFAULT: 
         glEnable(GL_LIGHTING); 

         glEnable(GL_LIGHT0); 
         glLightfv(GL_LIGHT0, GL_POSITION, light_position0); 
         glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular0); 
 
         glEnable(GL_LIGHT1); 
         glLightfv(GL_LIGHT1, GL_POSITION, light_position1); 
         glLightfv(GL_LIGHT1, GL_SPECULAR, light_color1); 
         break; 
   } 
} 


/***************************************************************************
* Function: createDisplayLists()
*
* This function creates the display lists for the various objects in the
* scene.
****************************************************************************/
void createDisplayLists()  { 
    
  
   GLUquadricObj *qobj; 

   /* create the display list for the ball */
   startList = glGenLists(3);
   qobj = gluNewQuadric();
   gluQuadricDrawStyle(qobj, GLU_FILL);
    
   glNewList(startList, GL_COMPILE); 
 
      glMaterialfv(GL_FRONT, GL_SPECULAR, ball_specular); 
      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ball_diff_amb); 
      glMaterialfv(GL_FRONT, GL_SHININESS, ball_shininess);
      gluSphere(qobj, BALL_RADIUS,10,10); 
      glMaterialfv(GL_FRONT, GL_SPECULAR, default_specular); 
      glMaterialfv(GL_FRONT, GL_AMBIENT, default_ambient); 
      glMaterialfv(GL_FRONT, GL_DIFFUSE, default_diffuse); 
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
      glBegin(GL_QUADS); 

      /* left */
      glVertex3iv(vertex[4]);
      glVertex3iv(vertex[0]);
      glVertex3iv(vertex[3]);
      glVertex3iv(vertex[7]);

      /* top */
      glVertex3iv(vertex[3]);
      glVertex3iv(vertex[2]);
      glVertex3iv(vertex[6]);
      glVertex3iv(vertex[7]);

      /* right */
      glVertex3iv(vertex[1]);
      glVertex3iv(vertex[5]);
      glVertex3iv(vertex[6]);
      glVertex3iv(vertex[2]);

      /* bottom */

      glVertex3iv(vertex[0]);
      glVertex3iv(vertex[4]);
      glVertex3iv(vertex[5]);
      glVertex3iv(vertex[1]);

      /* back */
      glVertex3iv(vertex[0]);
      glVertex3iv(vertex[1]);
      glVertex3iv(vertex[2]);
      glVertex3iv(vertex[3]); 
      glEnd();
   glEndList();
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
* Function: checkForOpenGLErrors
*
* This function is called to check for OpenGl errors
****************************************************************************/
void checkForOpenGLErrors(void) {
   GLenum error;
   while ((error = glGetError()) != GL_NO_ERROR)
    fprintf(stderr, "GL error: %s\n", gluErrorString(error));
}
