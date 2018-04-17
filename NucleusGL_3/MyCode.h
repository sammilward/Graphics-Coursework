//--------------------------------------------------------------------------------------------
// File:		MyCode.cpp
// Version:		V2.0
// Author:		Daniel Rhodes
// Description:	Your code goes here
// Notes:		For use with OpenGl 2.0 / DirectX9.0c or higher
//--------------------------------------------------------------------------------------------

// System header files
//#pragma warning(disable: 4996)
#include <fstream>

// Windows header files
#include <windows.h>
// Custom header files
#include "Trans.h"

// Only include this file once, may cause problems with some compilers.
#pragma once
// Same effect as:
// #ifndef ND3D_H
// #define ND3D_H
//
// main body of code
//
// #endif // !defined(ND3D_H)

//-----------------------------------------------------------------------------
// Key definitions
// Desc:	Allow NON CASE SENSITIVE key controls
//-----------------------------------------------------------------------------
#define	KEYDOWN	0x8000

#define	VK_SHIFT	0x10	// SHIFT key  
#define	VK_CONTROL	0x11	// CTRL key  
#define	VK_MENU		0x12	// ALT key 
#define	VK_LEFT		0x25	// LEFT ARROW key  
#define	VK_UP		0x26	// UP ARROW key  
#define	VK_RIGHT	0x27	// RIGHT ARROW key  
#define	VK_DOWN		0x28	// DOWN ARROW key  
#define VK_ESCAPE	0x1B	// ESC key 
#define	VK_0		0x30
#define	VK_1		0x31
#define	VK_2		0x32
#define	VK_3		0x33
#define	VK_4		0x34
#define	VK_5		0x35
#define	VK_6		0x36
#define	VK_7		0x37
#define	VK_8		0x38
#define	VK_9		0x39
#define	VK_A		0x41
#define	VK_B		0x42
#define	VK_C		0x43
#define	VK_D		0x44
#define	VK_E		0x45
#define	VK_F		0x46
#define	VK_G		0x47
#define	VK_H		0x48
#define	VK_I		0x49
#define	VK_J		0x4A
#define	VK_K		0x4B
#define	VK_L		0x4C
#define	VK_M		0x4D
#define	VK_N		0x4E
#define	VK_O		0x4F
#define	VK_P		0x50
#define	VK_Q		0x51
#define	VK_R		0x52
#define	VK_S		0x53
#define	VK_T		0x54
#define	VK_U		0x55
#define	VK_V		0x56
#define	VK_W		0x57
#define	VK_X		0x58
#define	VK_Y		0x59
#define	VK_Z		0x5A
#define	VK_NUMPAD0  0x60	// Numeric keypad 0 key  
#define	VK_NUMPAD1  0x61	// Numeric keypad 1 key  
#define	VK_NUMPAD2  0x62	// Numeric keypad 2 key  
#define	VK_NUMPAD3  0x63	// Numeric keypad 3 key  
#define	VK_NUMPAD4  0x64	// Numeric keypad 4 key  
#define	VK_NUMPAD5  0x65	// Numeric keypad 5 key  
#define	VK_NUMPAD6  0x66	// Numeric keypad 6 key  
#define	VK_NUMPAD7  0x67	// Numeric keypad 7 key  
#define	VK_NUMPAD8  0x68	// Numeric keypad 8 key  
#define	VK_NUMPAD9  0x69	// Numeric keypad 9 key  
#define	VK_SUBTRACT	0x6D	// Minus key (numpad)
#define	VK_ADD		0x6B	// Plus key (numpad)
#define	VK_OEM_4	0xDB	// [{ key
#define	VK_OEM_6	0xDD	// ]} key
#define	VK_OEM_COMMA 0xBC	// , key
#define	VK_OEM_PERIOD 0xBE	// . key
#define	VK_OEM_2	0xBF	// /? key


//-----------------------------------------------------------------------------
// Name: texCoord & texMap
// Desc: Some handy definitions for texture mapping - if you get that far!
//-----------------------------------------------------------------------------
typedef struct texCoord
{
   float u, v;	// x & y, called u & v under texture mapping conventions
} TEXCOORD;

typedef struct textureMap
{
   char *texMap;
   int sx, sy;
} TEXMAP;

typedef struct colour
{
	float r, g, b;
} COLOUR;

//-----------------------------------------------------------------------------
// Name: POLYGON
// Desc: Polygon structure - compatible with Box.dat, ignore z coordinates
//		 for part A. Surface Normal is calculated by read_polys
//-----------------------------------------------------------------------------
typedef struct polygon
{
	// Basic information for 2D/3D Part A/B
	int nv;           // Number of vertices
	VECTOR vert[20];  // Array of vertices - position vectors
	COLOUR colour;       // Colour, struct of 3 BYTE's R, G, B

	// Extra information for back face removal, part B
	VECTOR normal;    //Surface Normal Vector

	// Extra information for shading and texturing, part C
	int vertVecNo[20];		// holds vertex vector number
	TEXMAP *texMap;			// pointer to texture map
	TEXCOORD texPos[20];	//Position of corresponding numbered vertex in texture map
} POLYGON;

// Enables you to use a different ??Main.cpp without changing your MyCode files


//#define	DIRECTX 
#define	OPENGL	
#define FULLSCREENQUAD	1
#define TEXWIDTH		700
#define TEXHEIGHT		700
#define MAXTEX			2		// Maximum number of textures
#define iTexID0			0		// Texture ID's, for multitexturing in OpenGL
#define iTexID1			1
#if FULLSCREENQUAD	// Need to use texture size as window size for fullscreen quad
	#define WINDOWWIDTH		TEXWIDTH	// Window width
	#define WINDOWHEIGHT	TEXHEIGHT	// Window height
#else
	#define WINDOWWIDTH		1024	// Window width
	#define WINDOWHEIGHT	768		// Window height
#endif
extern FILE *debugfile;

typedef struct InterpStruct {
	float start;
	float end;
	float lSlope;
	float rSlope;
};


	// Pixel drawing demo prototype
void DrawTrapezium(COLOUR c, InterpStruct *xVals, InterpStruct *zVals, float y_start, float y_end); //x_start and end set to refference parameters as DrawPoly 
	//Needs to know the values, as the bottom of one Trapezium is the top of the next one
	// Pixel drawing routine
	inline void Plotpix( DWORD dwX, DWORD dwY, float byR, float byG, float byB );

	void MemPlotPix(DWORD dwX, DWORD dwY, float byR, float byG, float byB);

	float SlopeCalculation(float CurTop, float NxtTop, float CurBot, float NxtBot);

	void DrawBigImage();

	// Prototypes for routines you must supply
	int ClipPolyXLow( POLYGON *pIinput, POLYGON *pOutput, int iXBound );
	int ClipPolyYLow( POLYGON *pIinput, POLYGON *pOutput, int iYBound );
	int ClipPolyXHigh( POLYGON *pIinput, POLYGON *pOutput, int iXBound );
	int ClipPolyYHigh( POLYGON *pIinput, POLYGON *pOutput, int iYBound );
	// Prototype for DrawPolygon, commented out because it is incomplete, you must define your own version
	void DrawPolygon( POLYGON *p);
	// Prototype for DrawTrapezium, commented out because it is incomplete, you must define your own version
	//void DrawTrapezium( float fR, float fG, float fB, int iStartY, int iEndY...Something to specify x endpoints and slopes );
	int LoadPolys( FILE *flInFile );
	void ReadFile( );	
	void displayFinal( );		// Creates final message, called on exit, useful for performance logging
	void displayReadInfo( );	// Routine to display useful info after file read, shows light vector as an example,
								// modify and use for debugging as required
	void DrawImage( );
	// Enable Keybord and mouse commands
	void KeyboardControl( WPARAM wParam );
	void MouseControl( POINT currentMousePos, POINT oldMousePos );
//-----------------------------------------------------------------------------
// Name: Interface
// Desc: Derived Window Class, add extra variables required to by used within
//		 MyCode here
//-----------------------------------------------------------------------------
class Interface
{




public:
	HWND m_hWindow;

	// Variables
	int m_iX, m_iY, m_iZ;
	// For dynamic keyboard control
	int m_iXShift;
	int m_iYShift;
	int m_iZShiftObj;

	// Frame count
	int m_iGLFrameCount;	
	// Utility functions	
	
	// Dialog Box Message Handler
	static INT_PTR CALLBACK DialogMessageHandlerStatic( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
	INT_PTR CALLBACK DialogMessageHandler( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
void Alert( char *Head, char *Inner );

	// Constructor
	Interface(	HWND p_hWindow, float *p_fDrawingSurface, int p_iWidth, int p_iHeight );
	// Destructor
	~Interface( void ) { fclose(debugfile); };

	// Utility functions
	void Init( HINSTANCE hInstance );			 // Runs before rendering begins, use for any initialisation requierd
	void Shutdown( );

	// Exposed main function, call all other drawing routines from this one

};
extern Interface *run;

