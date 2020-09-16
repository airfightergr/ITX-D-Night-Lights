// Downloaded from https://developer.x-plane.com/code-sample/hello-world-sdk-3/

#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMPlugin.h"
#include "XPLMProcessing.h"
#include "XPLMDataAccess.h"
#include "XPLMMenus.h"
#include "XPLMUtilities.h"
#include "XPWidgets.h"
#include "XPStandardWidgets.h"
#include "XPLMScenery.h"

#include <string.h>
#include <iostream>
#if IBM
	#include <windows.h>
#endif
#if LIN
	#include <GL/gl.h>
#elif __GNUC__
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif

#ifndef XPLM300
	#error This is made to be compiled against the XPLM300 SDK
#endif


//GLOBALS
#define MPS_TO_KNOTS	1.94384		//m/s to knots

XPLMDataRef	groundSpeed = NULL;

//float getGroundSpeed(void* inRefcon);

// An opaque handle to the window we will create
static XPLMWindowID	g_window;

// Callbacks we will register when we create our window
void				draw_hello_world(XPLMWindowID in_window_id, void * in_refcon);
int					dummy_mouse_handler(XPLMWindowID in_window_id, int x, int y, int is_down, void * in_refcon) { return 0; }
XPLMCursorStatus	dummy_cursor_status_handler(XPLMWindowID in_window_id, int x, int y, void * in_refcon) { return xplm_CursorDefault; }
int					dummy_wheel_handler(XPLMWindowID in_window_id, int x, int y, int wheel, int clicks, void * in_refcon) { return 0; }
void				dummy_key_handler(XPLMWindowID in_window_id, char key, XPLMKeyFlags flags, char virtual_key, void * in_refcon, int losing_focus) { }

PLUGIN_API int XPluginStart(
							char *		outName,
							char *		outSig,
							char *		outDesc)
{
	strcpy(outName, "HelloWorld_myPlugin");
	strcpy(outSig, "xpsdk.examples.itselios");
	strcpy(outDesc, "My HelloWorld Plugin!");
	
	//Get GS and stuff
	groundSpeed = XPLMFindDataRef("sim/flightmodel/position/groundspeed");


	XPLMCreateWindow_t params;
	params.structSize = sizeof(params);
	params.visible = 1;
	params.drawWindowFunc = draw_hello_world;
	// Note on "dummy" handlers:
	// Even if we don't want to handle these events, we have to register a "do-nothing" callback for them
	params.handleMouseClickFunc = dummy_mouse_handler;
	params.handleRightClickFunc = dummy_mouse_handler;
	params.handleMouseWheelFunc = dummy_wheel_handler;
	params.handleKeyFunc = dummy_key_handler;
	params.handleCursorFunc = dummy_cursor_status_handler;
	params.refcon = NULL;
	params.layer = xplm_WindowLayerFloatingWindows;
	// Opt-in to styling our window like an X-Plane 11 native window
	// If you're on XPLM300, not XPLM301, swap this enum for the literal value 1.
	params.decorateAsFloatingWindow = xplm_WindowDecorationRoundRectangle;
	
	// Set the window's initial bounds
	// Note that we're not guaranteed that the main monitor's lower left is at (0, 0)...
	// We'll need to query for the global desktop bounds!
	int left, bottom, right, top;
	XPLMGetScreenBoundsGlobal(&left, &top, &right, &bottom);
	params.left = left + 25;
	params.bottom = bottom + 800;
	params.right = params.left + 400;
	params.top = params.bottom + 200;
	
	g_window = XPLMCreateWindowEx(&params);
	
	// Position the window as a "free" floating window, which the user can drag around
	XPLMSetWindowPositioningMode(g_window, xplm_WindowPositionFree, -1);
	// Limit resizing our window: maintain a minimum width/height of 100 boxels and a max width/height of 300 boxels
	XPLMSetWindowResizingLimits(g_window, 700, 150, 700, 150);
	XPLMSetWindowTitle(g_window, "ITX-D Night Lighting");
	
	return g_window != NULL;
}

PLUGIN_API void	XPluginStop(void)
{
	// Since we created the window, we'll be good citizens and clean it up
	XPLMDestroyWindow(g_window);
	g_window = NULL;
}

PLUGIN_API void XPluginDisable(void) { }
PLUGIN_API int  XPluginEnable(void)  { return 1; }
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, int inMsg, void * inParam) { }

void	draw_hello_world(XPLMWindowID in_window_id, void * in_refcon)
{
	// Mandatory: We *must* set the OpenGL state before drawing
	// (we can't make any assumptions about it)
	XPLMSetGraphicsState(
						 0 /* no fog */,
						 0 /* 0 texture units */,
						 0 /* no lighting */,
						 0 /* no alpha testing */,
						 1 /* do alpha blend */,
						 1 /* do depth testing */,
						 0 /* no depth writing */
						 );
	
	int l, t, r, b;
	XPLMGetWindowGeometry(in_window_id, &l, &t, &r, &b);
	
	//get the ground speed and convert it to knots and char
	float gsGet = XPLMGetDataf(groundSpeed);
	char gsVal[32];
	sprintf(gsVal, "GROUND SPEED: %4.2f KTS", gsGet * MPS_TO_KNOTS);

	float col_white[] = {1.0, 1.0, 1.0}; // red, green, blue
	float col_red[] = {1.0, 0.0, 0.0}; // red, green, blue
	float col_green[] = {0.0, 1.0, 0.0}; // red, green, blue
	float col_purple[] = {1.0, 0.0, 1.0}; // red, green, blue
	
	XPLMDrawString(col_white, l + 10, t - 20, "Hello world!", NULL, xplmFont_Proportional);
	XPLMDrawString(col_red, l + 10, t - 40, "Ilias Tselios!", NULL, xplmFont_Proportional);
	XPLMDrawString(col_green, l + 10, t - 60, "New colors!", NULL, xplmFont_Proportional);
	XPLMDrawString(col_purple, l + 10, t - 80, "This is magenta!", NULL, xplmFont_Proportional);
	XPLMDrawString(col_purple, l + 10, t - 100, gsVal, NULL, xplmFont_Proportional);
}

