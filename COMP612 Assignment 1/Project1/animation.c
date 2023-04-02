/******************************************************************************
 *
 * Animation v1.0 (23/02/2021)
 *
 * This template provides a basic FPS-limited render loop for an animated scene.
 *
 ******************************************************************************/

#include <Windows.h>
#include <freeglut.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>


 /******************************************************************************
  * Animation & Timing Setup
  ******************************************************************************/

  // Target frame rate (number of Frames Per Second).
#define TARGET_FPS 60				

// Ideal time each frame should be displayed for (in milliseconds).
const unsigned int FRAME_TIME = 1000 / TARGET_FPS;

// Frame time in fractional seconds.
// Note: This is calculated to accurately reflect the truncated integer value of
// FRAME_TIME, which is used for timing, rather than the more accurate fractional
// value we'd get if we simply calculated "FRAME_TIME_SEC = 1.0f / TARGET_FPS".
const float FRAME_TIME_SEC = (1000 / TARGET_FPS) / 1000.0f;

// Time we started preparing the current frame (in milliseconds since GLUT was initialized).
unsigned int frameStartTime = 0;

/******************************************************************************
 * Keyboard Input Handling Setup
 ******************************************************************************/

 // Define all character keys used for input (add any new key definitions here).
 // Note: USE ONLY LOWERCASE CHARACTERS HERE. The keyboard handler provided converts all
 // characters typed by the user to lowercase, so the SHIFT key is ignored.

#define KEY_EXIT				27  // Escape key.
#define KEY_TOGGLE_SNOW			115 // s key.
#define KEY_TOGGLE_DIAGNOSTICS	100 // d key.

/******************************************************************************
 * GLUT Callback Prototypes
 ******************************************************************************/

void display(void);
void reshape(int width, int h);
void keyPressed(unsigned char key, int x, int y);
void idle(void);
void drawCircle(float x, float y, float radius, float color[]);

/******************************************************************************
 * Animation-Specific Function Prototypes (add your own here)
 ******************************************************************************/

void main(int argc, char** argv);
void init(void);
void think(void);

/******************************************************************************
 * Animation-Specific Setup (Add your own definitions, constants, and globals here)
 ******************************************************************************/

#define MAX_PARTICLES 1000

typedef struct {
	float x;
	float y;
} Position2d;

typedef struct {
	Position2d position; // x, y coordiantes of particle
	float size; // GL point size
	float dy; // velocity
	int active; // is active/inactive
} Particle_t;

Particle_t particleSystem[MAX_PARTICLES];

float vertices[10][2];

bool snow;
bool diagnostics;

int particleCount;

 /******************************************************************************
  * Entry Point (don't put anything except the main function here)
  ******************************************************************************/

void main(int argc, char** argv)
{
	// Initialize the OpenGL window.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(900, 900);
	glutCreateWindow("Animation");

	// Set up the scene.
	init();

	// Disable key repeat (keyPressed or specialKeyPressed will only be called once when a key is first pressed).
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

	// Register GLUT callbacks.
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyPressed);
	glutIdleFunc(idle);

	// Record when we started rendering the very first frame (which should happen after we call glutMainLoop).
	frameStartTime = (unsigned int)glutGet(GLUT_ELAPSED_TIME);

	// Enter the main drawing loop (this will never return).
	glutMainLoop();
}

/******************************************************************************
 * GLUT Callbacks (don't add any other functions here)
 ******************************************************************************/

 /*
	 Called when GLUT wants us to (re)draw the current animation frame.

	 Note: This function must not do anything to update the state of our simulated
	 world. Animation (moving or rotating things, responding to keyboard input,
	 etc.) should only be performed within the think() function provided below.
 */
void display(void)
{
	/*
		TEMPLATE: REPLACE THIS COMMENT WITH YOUR DRAWING CODE

		Separate reusable pieces of drawing code into functions, which you can add
		to the "Animation-Specific Functions" section below.

		Remember to add prototypes for any new functions to the "Animation-Specific
		Function Prototypes" section near the top of this template.
	*/
	glClear(GL_COLOR_BUFFER_BIT);


	// Background
	glBegin(GL_QUADS);

	glColor3f(0.878f, 0.947f, 0.802f);
	glVertex2f(-1.0f, -1.0f);
	glVertex2f(1.0f, -1.0f);
	glColor3f(0.678f, 0.847f, 0.902f);
	glVertex2f(1.0f, 1.0f);
	glVertex2f(-1.0f, 1.0f);
	
	glEnd();

	// Terrain
	float totalX = 0;
	
	while (totalX < 2.0f) {
		for (int i = 1; i < 11; i++)
		{
			if (i == 0) {
				glBegin(GL_POLYGON);

				glColor3f(0.35f, 0.35f, 0.35f);
				glVertex2f(-1.0f, -1.0f);
				glVertex2f(-1.0f + vertices[i][0], -1.0f);
				glColor3f(0.98f, 0.98f, 0.98f);
				glVertex2f(-1.0f + vertices[i][0], vertices[i][1]);
				glVertex2f(-1.0f, vertices[i-1][1]);

				glEnd();

				totalX += vertices[i][0];
			}

			else {
				glBegin(GL_POLYGON);

				glColor3f(0.35f, 0.35f, 0.35f);
				glVertex2f(-1.0f + totalX, -1.0f);
				glVertex2f(-1.0f + vertices[i][0] + totalX, -1.0f);
				glColor3f(0.98f, 0.98f, 0.98f);
				glVertex2f(-1.0f + vertices[i][0] + totalX, vertices[i][1]);
				glVertex2f(-1.0f + totalX, vertices[i-1][1]);

				glEnd();

				totalX += vertices[i][0];
			}
		}
	}

	// Snowman
	float body[4] = { 0.95f, 0.93f, 0.93f, 1.0f };
	float eyes[4] = { 0.0f, 0.0f, 0.0f, 1.0f};
	float nose[4] = { 1.0f, 0.647f, 0.0f, 1.0f };
	// Bottom circle
	drawCircle(0.3f, -0.5f, 0.15f, body);
	// Top circle
	drawCircle(0.3f, -0.35f, 0.1f, body);
	// Left eye
	drawCircle(0.275f, -0.30f, 0.009f, eyes);
	// Right eye
	drawCircle(0.325f, -0.30f, 0.009f, eyes);
	// Nose
	glColor4f(1.0f, 0.647f, 0.0f, 1.0f);
	glBegin(GL_TRIANGLES);
	glVertex2f(0.3f, -0.335f);
	glVertex2f(0.4f, -0.325f);
	glVertex2f(0.3f, -0.315f);
	glEnd();


	// Snow
	glEnable(GL_BLEND);

	for (int i = 0; i < MAX_PARTICLES; i++)
	{	
		glColor4f(0.753f, 0.753f, 0.753f, particleSystem[i].dy - 0.2f);
		glPointSize(particleSystem[i].size);

		glBegin(GL_POINTS);
		glVertex2f(particleSystem[i].position.x, particleSystem[i].position.y);

		glEnd();
	}
	
	glDisable(GL_BLEND);

	// Diagnostics

	if (diagnostics) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glColor3f(1.0f, 0.0f, 0.0f);

		// Quit
		glRasterPos2f(-0.95f, 0.95f);
		glutBitmapString(GLUT_BITMAP_HELVETICA_12, "Press [ESC] to quit");

		// Snow toggle
		glRasterPos2f(-0.95f, 0.90f);
		glutBitmapString(GLUT_BITMAP_HELVETICA_12, "Press [s] to toggle snow");

		// Active number of particles on screen
		glRasterPos2f(-0.95f, 0.85f);
		char particleCountDisplay[40];
		sprintf_s(particleCountDisplay, 40, "Number of particles on screen: %d", particleCount);
		glutBitmapString(GLUT_BITMAP_HELVETICA_12, particleCountDisplay);
	}

	glutSwapBuffers();
}

/*
	Called when the OpenGL window has been resized.
*/
void reshape(int width, int h)
{
}

/*
	Called each time a character key (e.g. a letter, number, or symbol) is pressed.
*/
void keyPressed(unsigned char key, int x, int y)
{
	switch (tolower(key)) {
		/*
			TEMPLATE: Add any new character key controls here.

			Rather than using literals (e.g. "d" for diagnostics), create a new KEY_
			definition in the "Keyboard Input Handling Setup" section of this file.
		*/
	case KEY_EXIT:
		exit(0);
		break;
	
	case KEY_TOGGLE_SNOW:
		snow = !snow;
		break;

	case KEY_TOGGLE_DIAGNOSTICS:
		diagnostics = !diagnostics;
		break;
	}
}

/*
	Called by GLUT when it's not rendering a frame.

	Note: We use this to handle animation and timing. You shouldn't need to modify
	this callback at all. Instead, place your animation logic (e.g. moving or rotating
	things) within the think() method provided with this template.
*/
void idle(void)
{
	// Wait until it's time to render the next frame.

	unsigned int frameTimeElapsed = (unsigned int)glutGet(GLUT_ELAPSED_TIME) - frameStartTime;
	if (frameTimeElapsed < FRAME_TIME)
	{
		// This frame took less time to render than the ideal FRAME_TIME: we'll suspend this thread for the remaining time,
		// so we're not taking up the CPU until we need to render another frame.
		unsigned int timeLeft = FRAME_TIME - frameTimeElapsed;
		Sleep(timeLeft);
	}

	// Begin processing the next frame.
	
	frameStartTime = glutGet(GLUT_ELAPSED_TIME); // Record when we started work on the new frame.

	think(); // Update our simulated world before the next call to display().

	glutPostRedisplay(); // Tell OpenGL there's a new frame ready to be drawn.
}

/******************************************************************************
 * Animation-Specific Functions (Add your own functions at the end of this section)
 ******************************************************************************/

void drawCircle(float x, float y, float radius, float color[])
{
	glBegin(GL_TRIANGLE_FAN);
	glColor4f(color[0], color[1], color[2], color[3]);
	glVertex2f(x, y);

	for (int i = 0; i <= 360; i++)
	{
		float angle = (float)(i * 3.1415 / 180.0);
		float localX = cos(angle) * radius;
		float localY = sin(angle) * radius;
		glVertex2f(localX + x, localY + y);
	}

	glEnd();
}


 /*
	 Initialise OpenGL and set up our scene before we begin the render loop.
 */
void init(void)
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	srand(time(NULL));
	snow = true;
	diagnostics = true;

	// init the snow
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		particleSystem[i].position.x = ((float)rand() / RAND_MAX * 3.5f) - 0.5f;

		particleSystem[i].position.y = ((float)rand() / RAND_MAX) * 2 + 2.0f;
		
		particleSystem[i].dy = (((float)rand() / RAND_MAX) + 0.2);
		particleSystem[i].size = ((float)rand() / RAND_MAX) * 9.0f + 1.0f;
		particleSystem[i].active = 1;
		particleCount++;
	}

	// init the landscape
	for (int i = 0; i < 10; i++)
	{
		vertices[i][0] = (float)rand() / RAND_MAX * 0.3f + 0.2f;
		vertices[i][1] = (float)rand() / RAND_MAX * 0.4f - 0.3f;
	}	
}

/*
	Advance our animation by FRAME_TIME milliseconds.

	Note: Our template's GLUT idle() callback calls this once before each new
	frame is drawn, EXCEPT the very first frame drawn after our application
	starts. Any setup required before the first frame is drawn should be placed
	in init().
*/
void think(void)
{
	/*
		TEMPLATE: REPLACE THIS COMMENT WITH YOUR ANIMATION/SIMULATION CODE

		In this function, we update all the variables that control the animated
		parts of our simulated world. For example: if you have a moving box, this is
		where you update its coordinates to make it move. If you have something that
		spins around, here's where you update its angle.

		NOTHING CAN BE DRAWN IN HERE: you can only update the variables that control
		how everything will be drawn later in display().

		How much do we move or rotate things? Because we use a fixed frame rate, we
		assume there's always FRAME_TIME milliseconds between drawing each frame. So,
		every time think() is called, we need to work out how far things should have
		moved, rotated, or otherwise changed in that period of time.

		Movement example:
		* Let's assume a distance of 1.0 GL units is 1 metre.
		* Let's assume we want something to move 2 metres per second on the x axis
		* Each frame, we'd need to update its position like this:
			x += 2 * (FRAME_TIME / 1000.0f)
		* Note that we have to convert FRAME_TIME to seconds. We can skip this by
		  using a constant defined earlier in this template:
			x += 2 * FRAME_TIME_SEC;

		Rotation example:
		* Let's assume we want something to do one complete 360-degree rotation every
		  second (i.e. 60 Revolutions Per Minute, or RPM).
		* Each frame, we'd need to update our object's angle like this (we'll use the
		  FRAME_TIME_SEC constant as per the example above):
			a += 360 * FRAME_TIME_SEC;

		This works for any type of "per second" change: just multiply the amount you'd
		want to move in a full second by FRAME_TIME_SEC, and add or subtract that
		from whatever variable you're updating.

		You can use this same approach to animate other things like color, opacity,
		brightness of lights, etc.
	*/


	// Update snow
	int activeCount = 1000;
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		
		// Deactivate particles
		if (!snow && particleSystem[i].position.y < -1.0
			|| !snow && particleSystem[i].position.y > 1.0
			|| !snow && particleSystem[i].position.x < -1.0
			|| !snow && particleSystem[i].position.x > 1.0)
		{
			particleSystem[i].active = 0;
			particleSystem[i].dy = 0;
			particleSystem[i].position.x = ((float)rand() / RAND_MAX * 3.5f) - 0.5f;
			particleSystem[i].position.y = ((float)rand() / RAND_MAX) * 2 + 2.0f;
		}

		// Recycle particles to the top of the screen
		if (particleSystem[i].position.y <= -1.0 && particleSystem[i].active == 1)
		{

			particleSystem[i].position.x = ((float)rand() / RAND_MAX * 3.5f) - 0.5f;
			particleSystem[i].position.y = ((float)rand() / RAND_MAX) * 2 + 2.0f;
			particleSystem[i].dy = (((float)rand() / RAND_MAX) + 0.2);
		}

		// Reactivate particles
		if (snow && particleSystem[i].dy == 0)
		{
			particleSystem[i].dy = (((float)rand() / RAND_MAX) + 0.2);
			particleSystem[i].active = 1;
		}

		// Update particle position
		particleSystem[i].position.x -= 0.35f * FRAME_TIME_SEC;
		particleSystem[i].position.y -= particleSystem[i].dy * FRAME_TIME_SEC;

		// Count number of active particles
		if (particleSystem[i].position.y > -1.0
			&& particleSystem[i].position.y < 1.0
			&& particleSystem[i].position.x > -1.0
			&& particleSystem[i].position.x < 1.0)
			activeCount++;
		else
			activeCount--;
	}
	particleCount = activeCount;
}

/******************************************************************************/