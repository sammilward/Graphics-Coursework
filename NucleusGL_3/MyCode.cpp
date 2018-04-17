//--------------------------------------------------------------------------------------------
// File:		MyCode.cpp
// Version:		V2.0
// Author:		Sam Milward
// Description:	Your code goes here
// Notes:		For use with OpenGl 2.0 / DirectX9.0c or higher
//--------------------------------------------------------------------------------------------

// System header files
#include <math.h>

// Windows header files
// --

// Custom header files
#include "MyCode.h"
#include "resource.h"
extern HINSTANCE g_hInstance;

// For dynamic keyboard control
	int m_iMoveX = 200;
	int m_iMoveY;
	int m_iMoveZ;
	//Values of radians of tranformation
	float m_dThetaX;
	float m_dThetaY;
	float m_dThetaZ;

	double m_dViewAngle;
	//debugging control variables
	int iFaceSelection;			// FaceSelection > 0: enables one face (with index selecton - 1)
								// to be drawn on its own to aid debugging.
	bool bDebug;				// Switches on debug output to file when true.
	// number of polygons read in
	int m_iNumOfPolys;
	//Frame Count
	int m_iOurFrameCount;
	// Input filename
	char m_sFilename[30];
	// Viewpoint data, use for parts B/C
	VECTOR m_vDisp, m_vLight;	// Displacement of object coordinates relative
								// to viewing coordinates and light direction
	//Debug File Handle
	FILE *debugfile;

	//Large Screen Constants
	const int LargeScreenWidth = WINDOWWIDTH * 4;
	const int LargeScreenHeight = WINDOWHEIGHT * 4;

	//Zbuffer array
	double ZBuffer[LargeScreenWidth][LargeScreenHeight];

	//Colour array for large screen
	COLOUR colourArray[LargeScreenWidth][LargeScreenHeight];

	//Colour array for smaller screen to average out the colour of pixel
	COLOUR SmallScreenColour[WINDOWWIDTH][WINDOWHEIGHT];




	// Drawing area dimensions: width and height
	int m_iWidth;
	int m_iHeight;

	//Drawing Surface Handle
	float *m_fDrawingSurface;
// Database data, for the polygons
POLYGON polylist[2000];  // Array to store a list of polygons.

COLOUR Default_Colour={0.5,0.5,0.5};//colours are floats these days
//-----------------------------------------------------------------------------
// Name: KeyboardControl
// Desc: Enable Keyboard Control
//-----------------------------------------------------------------------------
void KeyboardControl( WPARAM wParam )
{
	switch( wParam ) 
	{
	case VK_ESCAPE:	// Exit application
		PostQuitMessage( 0 );
		break;
	case VK_LEFT:
		m_iMoveX--;
		break;
	case VK_RIGHT:
		m_iMoveX++;
		break;
	case VK_UP:
		m_iMoveY--;
		break;
	case VK_DOWN:
		m_iMoveY++;
		break;
	case VK_ADD:
		m_iMoveZ++;
		break;
	case VK_SUBTRACT:
		m_iMoveZ--;
		break;
	case VK_D:
		bDebug = !bDebug; // Toggle Debug output
		break;
	case VK_Q: 	DialogBoxParam( g_hInstance, MAKEINTRESOURCE(IDD_INIT),run->m_hWindow, run->DialogMessageHandlerStatic, (LPARAM)(run));
		break;

	//Lab 5 Rotation Keys
	case VK_I: 
		m_dThetaX += 0.01; //Incrases X. Rotates around the X access. Roatates like laptop screen
		break;
	case VK_K: 
		m_dThetaX -= 0.01;
		break;
	case VK_J:
		m_dThetaY += 0.01; //Increase Y
		break;
	case VK_L:
		m_dThetaY -= 0.01;
		break;
	case VK_N: 
		m_dThetaZ += 0.01; //Increase Z
		break;
	case VK_M: 
		m_dThetaZ -= 0.01;
		break;
	}
}

//-----------------------------------------------------------------------------
// Name: KeyboardControl
// Desc: Enable Keyboard Control
//-----------------------------------------------------------------------------
void MouseControl( POINT currentMousePos, POINT oldMousePos )
{
	m_dThetaX -= ( currentMousePos.x - oldMousePos.x )*0.01;
	m_dThetaY -= ( currentMousePos.y - oldMousePos.y )*0.01;
}


//-----------------------------------------------------------------------------
// Name: displayFinal
// Desc: Routine to display useful info after program exit
//-----------------------------------------------------------------------------
void displayFinal( )
{
	char sDispString[50];
	sprintf( sDispString, "Total Framecount %d", m_iOurFrameCount );
	run->Alert( "Finished", sDispString );
}

//-----------------------------------------------------------------------------
// Name: displayReadInfo
// Desc: Routine to display useful info after file read, shows light vector
//		 as an example, modify and use for debugging as required
//-----------------------------------------------------------------------------
void displayReadInfo( )
{
	char sDispString[50];
	sprintf( sDispString, "%d polygons read", m_iNumOfPolys );
	run->Alert( m_sFilename, sDispString );
	sprintf( sDispString , "Light Vector %f  %f  %f", m_vLight.x, m_vLight.y, m_vLight.z );
	run->Alert( "Start Values:", sDispString );
}


//-----------------------------------------------------------------------------
// Name: LoadPolys
// Desc: Read polygon info from file
//-----------------------------------------------------------------------------
int LoadPolys( FILE *infile )
{
	char cInString[1000];
	int iNumPolys = 0;
	float fLength;
	float fR, fG, fB;		// red, green, blue values

	do
	{
		fgets( cInString, 1000, infile);						 // Read first/next line of file
		sscanf( cInString, "%d", &polylist[iNumPolys].nv);   // Get number of vertices
		fprintf( debugfile, "number of vertices: %d\n", polylist[iNumPolys].nv);   // print number of vertices to debug file
		if  (polylist[iNumPolys].nv == 0)
			break;	// Get out if terminating zero found

		// Only allocate the memory we need - ALWAYS remember to delete on shutdown

		for (int i = 0; i < polylist[iNumPolys].nv; i++)
		{
			// Read next line of file
			fgets(cInString, 1000, infile);
			//Get Coordinates
			sscanf(cInString, "%f%f%f", &( polylist[iNumPolys].vert[i].x ),
										&( polylist[iNumPolys].vert[i].y ),
										&( polylist[iNumPolys].vert[i].z ) );
		}
	
		polylist[iNumPolys].normal = Cross( VectorDiff( polylist[iNumPolys].vert[0], polylist[iNumPolys].vert[1] ),
									 VectorDiff( polylist[iNumPolys].vert[0],polylist[iNumPolys].vert[2] ) );
		fLength = (float)sqrt( Dot( polylist[iNumPolys].normal, polylist[iNumPolys].normal ) ); // Calculate length of vector

	    polylist[iNumPolys].normal.x /= fLength;	// Normalise
	    polylist[iNumPolys].normal.y /= fLength;	// each
	    polylist[iNumPolys].normal.z /= fLength;	// component
	    fgets(cInString, 1000, infile);		// Read  next line of file
	    sscanf( cInString, "%f%f%f", &fR, &fG, &fB );	// Get Colour, texture
		if (fR>1.0 ||fG>1.0||fB>1.0)//cope with either Open Gll 0-1.0 colours or old style 0-255 colours
		{
			polylist[iNumPolys].colour.r = fR/255.0;
			polylist[iNumPolys].colour.g = fG/255.0;
			polylist[iNumPolys].colour.b = fB/255.0;	
		}
		else
		{
			polylist[iNumPolys].colour.r = fR;
			polylist[iNumPolys].colour.g = fG;
			polylist[iNumPolys].colour.b = fB;	
		}
		iNumPolys++;
	} while( 1 );

	return iNumPolys;  //Return number of polygons read
}


//-----------------------------------------------------------------------------
// Name: ReadFile
// Desc: Read polygon info from file
//-----------------------------------------------------------------------------
void ReadFile()
{
	FILE *flInFile;
	flInFile = fopen( m_sFilename, "r" );
	m_iNumOfPolys = LoadPolys( flInFile );
	displayReadInfo();
	fclose(flInFile);	
}


void MemPlotPix(DWORD dwX, DWORD dwY, float fR, float fG, float fB)
{
	colourArray[dwX][dwY].r = fR;
	colourArray[dwX][dwY].g = fG;
	colourArray[dwX][dwY].b = fB;
}

//-----------------------------------------------------------------------------
// Name: Plotpix
// Desc: Draw a pixel - Calls nRGBAImage::SetColour(...),
//		 m_kImage MUST be initialised before use!
//		 Example usage: Plotpix( x, y, 255, 0, 0 );
//-----------------------------------------------------------------------------
inline void Plotpix( DWORD dwX, DWORD dwY, float fR, float fG, float fB )
{
	DWORD dwYtemp;

	// If using OpenGL we need to mirror the Y coordinates,
	// as OpenGL uses the opposite coordinate system to us and DirectX
#ifdef 	DIRECTX 
	dwYtemp = dwY;
#else  //OPENGL should be defined
	dwYtemp = ( m_iHeight - 1 ) - dwY;
#endif

	int index = 4 * ( dwYtemp * m_iWidth + dwX );
	m_fDrawingSurface[index]	 = fR;	// Red Channel
	m_fDrawingSurface[index + 1] = fG;	// Green Channel
	m_fDrawingSurface[index + 2] = fB;	// Blue Channel
	m_fDrawingSurface[index + 3] = 0.0; // Alpha Channel
}


void DrawImage()
{
	DrawBigImage();

	//Initalise the small screen array to 0	
	for (int i = 0; i < WINDOWWIDTH; i++)
	{
		for (int j = 0; j < WINDOWHEIGHT; j++)
		{
			SmallScreenColour[i][j].r = 0;
			SmallScreenColour[i][j].g = 0;
			SmallScreenColour[i][j].b = 0;
		}
	}

	//Loop through columns of sub pixels
	//WHY DOES IT ONLY RUN ON ONE SMALL QUDRANT (QUATER)
	for (int i = 0; i <= 3; i++)
	{
		//Loop through each window height in stages of 4
		for (int j = i; j <= LargeScreenWidth; j = j + 4)
		{
			//Loop through rows of sub pixels
			for (int k = 0; k <= 3; k++)
			{
				//Loop through each window width in stages of 4
				for (int l = k; l <= LargeScreenHeight; l = l + 4)
				{
					int inX = int(floor(l / 4));
					int inY = int(floor(j/4));
					SmallScreenColour[inX][inY].r += colourArray[l][j].r;
					SmallScreenColour[inX][inY].b += colourArray[l][j].b;
					SmallScreenColour[inX][inY].g += colourArray[l][j].g;

				}
			}
		}
	}

	//Eds algorithm
	//int smallY, smallX;
	//for (int j = 0; j < LargeScreenHeight; j++) {
	//	smallY = int(floor(j / 4));
	//	for (int i = 0; i < LargeScreenWidth; i++) {
	//		smallX = int(floor(i / 4));
	//		SmallScreenColour[smallX][smallY].r += colourArray[i][j].r;
	//		SmallScreenColour[smallX][smallY].g += colourArray[i][j].g;
	//		SmallScreenColour[smallX][smallY].b += colourArray[i][j].b;
	//	}
	//}

	
	//For dividing (Averaging)
	for (int i = 0; i < WINDOWWIDTH; i++)
	{
		for (int j = 0; j < WINDOWHEIGHT; j++)
		{
			SmallScreenColour[i][j].r = SmallScreenColour[i][j].r / 16;
			SmallScreenColour[i][j].b = SmallScreenColour[i][j].b / 16;
			SmallScreenColour[i][j].g = SmallScreenColour[i][j].g / 16;
		}
	}

	//For displaying, calling plotpix
	for (int i = 0; i < WINDOWWIDTH; i++)
	{
		for (int j = 0; j < WINDOWHEIGHT; j++)
		{
			Plotpix(i, j, SmallScreenColour[i][j].r, SmallScreenColour[i][j].g, SmallScreenColour[i][j].b);
		}
	}

	//Loop through all of super pixels, add the colours 
	//to the new small colour array, with the average of all sub pixels





}



//-----------------------------------------------------------------------------
// Name: DrawImage
// Desc: Draws the image
//-----------------------------------------------------------------------------
void DrawBigImage( )
{
	POLYGON polyTempP, polyTempQ, polyTempQT, LargePoly;	// Temporary polygons for copying transformed, projected / clipped
												// versions of scene polys before drawing them. 
	int iCurrentPoly;							// Current polygon in process
	TRANSFORM object_transformation; // Structure to hold transformation

	//Lab 5 Rotation Work
    //Initalise object transformation with angles to roatate. 
	object_transformation = BuildTrans(m_dThetaX, m_dThetaY, m_dThetaZ, m_vDisp);
	
	//Lab 6 New vector
	VECTOR view_in_object_coordinate, temp3;
	view_in_object_coordinate = MOnV(InverseRotationOnly(object_transformation), object_transformation.shift);
	
	//Initalise Z buffer to 0
	memset(ZBuffer, 0, sizeof(ZBuffer));

	for (int i = 0; i < LargeScreenWidth; i++)
	{
		for (int j = 0; j < LargeScreenHeight; j++)
		{
			colourArray[i][j].r = 0;
			colourArray[i][j].g = 0;
			colourArray[i][j].b = 0;
		}
	}

	if ( m_iNumOfPolys > 0 )
	//	DrawTrapezium(Default_Colour, 300, 300, -5, 0.1, 400, 430);		// This draws the square you see, replace with Trapezium, and later Polygon
//	else
		m_iOurFrameCount++;	// Increment frame counter if we have a polygon to draw

	for (iCurrentPoly = 0; iCurrentPoly < m_iNumOfPolys; iCurrentPoly++)	// for each polygon
	{
		//Lab 6 work
		//Stores value of VectorSum  in a temporary variable
		temp3 = VectorSum(view_in_object_coordinate, polylist[iCurrentPoly].vert[0]);
		if (Dot(temp3, polylist[iCurrentPoly].normal) < 0)
		{
			//If the dot product is negative then terminate this iteration 
			//then continues to the next iteration.
			continue;
		}

		if ( iFaceSelection > m_iNumOfPolys )
			iFaceSelection = m_iNumOfPolys;    //Keep debug selector in range
		
		
		if ( iFaceSelection && ( iCurrentPoly + 1 ) != iFaceSelection)
			continue; // Reject unselected polygons if debug selection active.
		
	
		polyTempP = polylist[iCurrentPoly];             //copy static data into temp poly structure
		
		// Copy each vertex in polygon, add displacement to allow shift

		//Lab 5
		//Creates temprary vectors to store Vector points 
		VECTOR temp1, temp2;
		for (int i = 0; i < polylist[iCurrentPoly].nv; i++)
		{
			//Assign transformaion to temp1. This roatates the shape.
			//Pass it a vertex, and the transformation we created and then does the roatation.
			temp1 = DoTransform(polylist[iCurrentPoly].vert[i], object_transformation);
			
			//Pass vector stored in temp1, and pass the viewing angle
			temp2 = Project(temp1, m_dViewAngle);

			//To turn coordinates from screen coords to pixel. Add 1 then mulitply by half the screen resoloution
			temp2.x = (temp2.x + 1) * (WINDOWWIDTH / 2);
			temp2.y = (temp2.y + 1) * (WINDOWHEIGHT / 2);
			temp2.x += m_iMoveX;
			temp2.y += m_iMoveY;
			//temp2.z += m_iMoveZ;

			polyTempP.vert[i] = temp2;



		}
				
		if ( bDebug )
			fprintf( debugfile, " number of vertices: %d\n", polyTempP.nv);   // print number of vertices

		fflush( debugfile );
		
		if ( bDebug )	// Print out current poly specs if debug active
		{
			for (int i = 0; i < polyTempP.nv; i++)
			{
				fprintf( debugfile, "before clipping Polygon %d, Vertex %d values: %7.2f, %7.2f, %11.6f\n",
						 iCurrentPoly, i, polyTempP.vert[i].x, polyTempP.vert[i].y, polyTempP.vert[i].z ); 
			}
			fflush( debugfile );
			
		}
		
		// The section below calls clipping and polygon draw routines, commented out to allow the 
		// program to work without them. You may re-instate once you have appropriate routines,
		// or replace with your own code.
		

		//Call function to clip polygon with original and temp, Window width is highest x val allowed
		ClipPolyXHigh( &polyTempP, &polyTempQT, WINDOWWIDTH );	// Clip against upper x boundary
		
		
		ClipPolyYHigh( &polyTempQT, &polyTempQ, WINDOWHEIGHT );	// Clip against upper y boundary (bottom of screen)
		
		ClipPolyXLow( &polyTempQ, &polyTempQT, 1);				// Clip against lower x boundary
		
		
		ClipPolyYLow( &polyTempQT, &polyTempQ, 1);				// Clip against lower y boundary (bottom of screen)
		/*
		if ( bDebug )	// Print out current poly specs if debug active
		{
			for ( int i = 0; i < polyTempQ.nv; i++ )
				fprintf( debugfile, "after clipping Polygon %d Vertex %d values:y %7.2f  %7.2f %11.6f\n",
						 iCurrentPoly, i, polyTempQ.vert[i].x, polyTempQ.vert[i].y, polyTempQ.vert[i].z ); 

			fflush(debugfile);
		}
		*/

		//Make a larger poly multiplied by 4 for the larger screen
		//Array for loop, set all coords.

		for (int i = 0; i < polyTempQ.nv; i++)
		{
			LargePoly.vert[i].x = polyTempQ.vert[i].x * 4;
			LargePoly.vert[i].y = polyTempQ.vert[i].y * 4;
			LargePoly.vert[i].z = polyTempQ.vert[i].z * 4;
		}
		LargePoly.normal = polyTempQ.normal;
		LargePoly.nv = polyTempQ.nv;
		LargePoly.colour = polyTempQ.colour;
		LargePoly.texMap = polyTempQ.texMap;




		DrawPolygon( &LargePoly );	// Call the drawing routine
		
	}
	if ( m_iNumOfPolys > 0 )
		bDebug = false;	// Switch debug off after first run - switch on again via keyboard control if needed

	
}

//-----------------------------------------------------------------------------
// Name: DrawTrapezium
// Desc: Draw a Trapezium
//-----------------------------------------------------------------------------
void DrawTrapezium(COLOUR c, InterpStruct *xVals, InterpStruct *zVals, float y_start, float y_end)//x_start and end set to refference parameters as DrawPoly 
	//Needs to know the values, as the bottom of one Trapezium is the top of the next one
{

	//Note no protection to keep in screen bounds...
	float zCurrent, zChange;

	//For the highest y, plus any movement, to the lowest y plus or minus movement
	for (int j = y_start; j < y_end; j++)
	{
		zCurrent = zVals->start;
		zChange = SlopeCalculation(zVals->start, zVals->end, xVals->start, xVals->end);
		//From furthest left, to furthest right plus any movement
		for (int i = xVals->start; i < xVals->end; i++)
		{
			//Plots the pixel
			if (zCurrent >= ZBuffer[i][j])
			{
				MemPlotPix(i, j, c.r, c.g, c.b);
				ZBuffer[i][j] = zCurrent;
			}
			zCurrent += zChange;
			
		}

		xVals->end += xVals->rSlope;
		xVals->start += xVals->lSlope;

		zVals->end += zVals->rSlope;
		zVals->start += zVals->lSlope;
	}


}

//Calculate Slope Function
float SlopeCalculation(float CurTop, float NxtTop, float CurBot, float NxtBot)
{
	float slope;
	if (CurBot == NxtBot)
	{
		slope = 0;
	}
	else
	{
		slope = ((NxtTop - CurTop) / (NxtBot - CurBot));
	}
	return slope;
}

//-----------------------------------------------------------------------------
// Name: DrawPolygon
// Desc: Draw a polygon
//-----------------------------------------------------------------------------
void DrawPolygon( POLYGON *p )
{

	//Lab 2 Task 5
	int top_vertex, current_left, current_right, next_left, next_right, ytop, ybottom, tempYIndex;
	float x_start, x_finish, slope_left, slope_right, tempY;

	InterpStruct xVals, zVals;

	if (p->nv > 2) //If 2 or less points then program gets stuck in a loop. 
	{
		
		
		//Structures to hold values for x and z
		
		top_vertex = 0; //Initialise
		//Lab 2 Task 6
		tempY = p->vert[0].y; //Tempy holds y coordinate of first vertex in array
		tempYIndex = 0; //Index is initialised to the first vertex

		for (int i = 0; i < p->nv; i++) //Loop through all verticies in the array
		{
			if (p->vert[i].y < tempY) //If the current vertex y coord is smaller than the tempY Y coord
			{
				tempY = p->vert[i].y; //Tempy is set to the new, lowest Y coord
				tempYIndex = i; //The index is equal to the i value in the loop
			}
		}

		top_vertex = tempYIndex; //top_vertex is equal to the vertex with the smallest Y cord, which is the highest vertex

		//Lab 2 Task 7
		current_left = top_vertex; //Assign current left and right to the highest vertex
		current_right = top_vertex;

		//Lab 2 Task 8
		if (current_left - 1 < 0) //As array cant be smaller 0, so we wrap the points around
		{
			next_left = (p->nv - 1); //Assign next left to the wrap around
		}
		else {
			next_left = current_left - 1; //Assign normal vertex - 1
		}

		if (current_right + 1 > p->nv - 1) //If bigger than bounds of the array, we need to wrap around
		{
			next_right = 0; //Wrap around to the first vertex in the array
		}
		else {
			next_right = current_right + 1; //If not out of bounds, assign just the next boundary
		}

		//Lab 2 Task 9
		if (p->vert[next_left].y - p->vert[current_left].y != 0) //If difference in current and next left not equal 0
		{
			slope_left = ((p->vert[next_left].x - p->vert[current_left].x) / (p->vert[next_left].y - p->vert[current_left].y));
			//Calculate left slope as difference in X divided by difference in Y
		}
		else {
			slope_left = 0; //If the difference is equal 0 then no slope, slope = 0
		}

		if (p->vert[next_right].y - p->vert[current_right].y != 0) //If left difference in current right and next right not equal = 0
		{
			slope_right = ((p->vert[next_right].x - p->vert[current_right].x) / (p->vert[next_right].y - p->vert[current_right].y));
			//Calculate right slope as difference in X divided by difference in Y
		}
		else {
			slope_right = 0;
		}

		//Lab 2 Task 10

		bool ChangeLeft;
		if (p->vert[next_right].y < p->vert[next_left].y) //if next right has a larger value of y (so lower point) than next left
		{
			ybottom = p->vert[next_right].y; //then bottom = next right y coordinate
			ChangeLeft = false; //As right is the end 
		}
		else {
			ybottom = p->vert[next_left].y; //if not bottom is equal to next left y coordinate  //In our example right is the one that ends
			ChangeLeft = true; //As left is the end vertex of the last trapizum
		}

		ytop = p->vert[top_vertex].y; //ytop = top vertex y coordinate 

		//Lab 3 Task 1 Define x_start and x_finish as x coords so they can be passed by reference
		x_start = p->vert[current_left].x;
		x_finish = p->vert[current_right].x;


		xVals.start = p->vert[current_left].x;
		xVals.end = p->vert[current_left].x;
		xVals.lSlope = SlopeCalculation(p->vert[current_left].x, p->vert[next_left].x, p->vert[current_left].y, p->vert[next_left].y);
		xVals.rSlope = SlopeCalculation(p->vert[current_right].x, p->vert[next_right].x, p->vert[current_right].y, p->vert[next_right].y);

		zVals.start = p->vert[current_left].z;
		zVals.end = p->vert[current_right].z;
		zVals.lSlope = SlopeCalculation(p->vert[current_left].z, p->vert[next_left].z, p->vert[current_left].y, p->vert[next_left].y);
		zVals.rSlope = SlopeCalculation(p->vert[current_right].z, p->vert[next_right].z, p->vert[current_right].y, p->vert[next_right].y);
		//Lab 3 Task 2a Find vertex which terminates the previous trapezium if on left or right


		//DrawTrapezium(p->colour, p->vert[current_left].x, p->vert[current_right].x, p->vert[top_vertex].y, INSERT VARIABLE , slope_right, slope_left);
		//void DrawTrapezium(COLOUR c, float x_start, float x_end, float slope_left, float slope_right, float y_start, float y_end)
		//Lab 2 Call
		//DrawTrapezium(p->colour, p->vert[current_left].x, p->vert[current_right].x, slope_left, slope_right, ytop, ybottom);
		//Lab 3 Task 1 call with reference parameters
		DrawTrapezium(p->colour, &xVals, &zVals, ytop, ybottom);

		//Lab 3 Task 4 Loop
		while (next_left != next_right)
		{
			//LeftSIDE
			//Lab 3 Task 2b and 2c
			if (ChangeLeft == true) //If left is the vertex to terminate previous
			{
				current_left = next_left; //Update current left to next left

				//Find the Next left
				if (current_left - 1 < 0) //As array cant be smaller 0, so we wrap the points around
				{
					next_left = (p->nv - 1); //Assign next left to the wrap around
				}
				else {
					next_left = current_left - 1; //Assign normal vertex - 1
				}
				//Find the next Left slope
				if (p->vert[next_left].y - p->vert[current_left].y != 0) //If difference in current and next left not equal 0
				{
					slope_left = ((p->vert[next_left].x - p->vert[current_left].x) / (p->vert[next_left].y - p->vert[current_left].y));
					//Calculate left slope as difference in X divided by difference in Y
				}
				else {
					slope_left = 0; //If the difference is equal 0 then no slope, slope = 0
				}

				//Do part D here
				x_start = p->vert[current_left].x;
				xVals.lSlope = slope_left;
				xVals.start = p->vert[current_left].x;

				zVals.lSlope = SlopeCalculation(p->vert[current_left].z, p->vert[next_left].z, p->vert[current_left].y, p->vert[next_left].y);
				zVals.start = p->vert[current_left].z;


			}
			else
			{
				current_right = next_right; //Update current right to next right

				//Find the Next Right
				if (current_right + 1 > p->nv - 1) //If bigger than bounds of the array, we need to wrap around
				{
					next_right = 0; //Wrap around to the first vertex in the array
				}
				else {
					next_right = current_right + 1; //If not out of bounds, assign just the next boundary
				}

				//find the next right slope
				if (p->vert[next_right].y - p->vert[current_right].y != 0) //If left difference in current right and next right not equal = 0
				{
					slope_right = ((p->vert[next_right].x - p->vert[current_right].x) / (p->vert[next_right].y - p->vert[current_right].y));
					//Calculate right slope as difference in X divided by difference in Y
				}
				else {
					slope_right = 0;
				}

				//Do part D here
				x_finish = p->vert[current_right].x;

				xVals.end = p->vert[current_right].x;
				xVals.rSlope = slope_right;
				
				zVals.end = p->vert[current_right].z;
				zVals.rSlope = SlopeCalculation(p->vert[current_right].z, p->vert[next_right].z, p->vert[current_right].y, p->vert[next_right].y);

			}
			//Lab 3 Task 2e

			ytop = ybottom; //The top of the new trap is = to the bottom of the previous

			if (p->vert[next_right].y < p->vert[next_left].y) //if next right has a larger value of y (so lower point) than next left
			{
				ybottom = p->vert[next_right].y; //then bottom = next right y coordinate
				ChangeLeft = false; //As right is the end 
			}
			else {
				ybottom = p->vert[next_left].y; //if not bottom is equal to next left y coordinate  //In our example right is the one that ends
				ChangeLeft = true; //As left is the end vertex of the last trapizum
			}

			DrawTrapezium(p->colour, &xVals, &zVals, ytop, ybottom); //Call second draw trap function
		}
	}
}


//-----------------------------------------------------------------------------
// Name: ClipPolyXLow
// Desc: Clipping Routine for lower x boundary
//-----------------------------------------------------------------------------
int ClipPolyXLow(POLYGON *pIinput, POLYGON *pOutput, int iXBound)
{
	//Lab 4 Task 2, Define indicies into the vertex array
	//Current vertex set to First vertex, previous set to last
	int CurVer = 0, PreVer = pIinput->nv - 1; //Amount of verticies - 1 to get index of last vertex
															   //Lab 4 Task 3, copy input vector details to output
															   //Input Poly Details are copied over to Output
	pOutput->colour = pIinput->colour;
	pOutput->normal = pIinput->normal;
	pOutput->texMap = pIinput->texMap;

	//Lab 4 Task 4
	//As we are unsure of how many verticies the new clipped poly will have
	pOutput->nv = 0;


	//Lab 4 Task 5 Structured english notes code

	//for each vertex
	for (int i = 0; i < pIinput->nv; i++)
	{
		pOutput->nv += 1; //Every loop add an extra vertex to new poly
						  //If previous vertex is on the screen
		if (pIinput->vert[PreVer].x >= iXBound)
		{
			//If current vertex is on the screen
			if (pIinput->vert[CurVer].x >= iXBound)
			{
				pOutput->vert[pOutput->nv - 1] = pIinput->vert[CurVer];
				//Set first vertex of Output to first of Input
			}
			else
			{
				//As new vertex is inline with the x boundary
				pOutput->vert[pOutput->nv - 1].x = iXBound;
				//Y value is calculated between two verticies and x bound point. Maths on lecture notes
				pOutput->vert[pOutput->nv - 1].y = (((iXBound - pIinput->vert[CurVer].x)*(pIinput->vert[PreVer].y - pIinput->vert[CurVer].y)) / (pIinput->vert[PreVer].x - pIinput->vert[CurVer].x)) + pIinput->vert[CurVer].y;
				//Z value is the same as the input Z value
				pOutput->vert[pOutput->nv - 1].z = pIinput->vert[i].z;
			}
		}
		else //If previous vertex is not on screen 
		{
			if (pIinput->vert[CurVer].x >= iXBound)
			{
				//As new vertex is inline with the x boundary
				pOutput->vert[pOutput->nv - 1].x = iXBound;
				//Y value is calculated between two verticies and x bound point. Maths on lecture notes
				pOutput->vert[pOutput->nv - 1].y = (((iXBound - pIinput->vert[CurVer].x)*(pIinput->vert[PreVer].y - pIinput->vert[CurVer].y)) / (pIinput->vert[PreVer].x - pIinput->vert[CurVer].x)) + pIinput->vert[CurVer].y;
				//Z value is the same as the input Z value
				pOutput->vert[pOutput->nv - 1].z = pIinput->vert[i].z;
				pOutput->nv += 1;
				pOutput->vert[pOutput->nv - 1] = pIinput->vert[CurVer];
			}
			else
			{
				pOutput->nv -= 1;
			}
		}
		CurVer += 1;
		PreVer = CurVer - 1;
	}

	// Tell calling routine how many vertices in pOutput array
	// Tell calling routine how many vertices in pOutput array
	return pOutput->nv;
}

//-----------------------------------------------------------------------------
// Name: ClipPolyYLow
// Desc: Clipping Routine for lower y boundary
//-----------------------------------------------------------------------------
int ClipPolyYLow(POLYGON *pIinput, POLYGON *pOutput, int iYBound) //The bottom has highest y coords
{//Clipped at top of page
	
	//Lab 4 Task 2, Define indicies into the vertex array
	//Current vertex set to First vertex, previous set to last
	int CurVer = 0, PreVer = pIinput->nv - 1; //Amount of verticies - 1 to get index of last vertex
											  //Lab 4 Task 3, copy input vector details to output
											  //Input Poly Details are copied over to Output
	pOutput->colour = pIinput->colour;
	pOutput->normal = pIinput->normal;
	pOutput->texMap = pIinput->texMap;

	//Lab 4 Task 4
	//As we are unsure of how many verticies the new clipped poly will have
	pOutput->nv = 0;

	//Lab 4 Task 5 Structured english notes code

	//for each vertex
	for (int i = 0; i < pIinput->nv; i++)
	{
		pOutput->nv += 1; //Every loop add an extra vertex to new poly
						  //If previous vertex is on the screen
		if (pIinput->vert[PreVer].y >= iYBound)
		{
			//If current vertex is on the screen
			if (pIinput->vert[CurVer].y >= iYBound)
			{
				pOutput->vert[pOutput->nv - 1] = pIinput->vert[CurVer];
				//Set first vertex of Output to first of Input
			}
			else
			{
				//As new vertex is inline with the x boundary
				pOutput->vert[pOutput->nv - 1].y = iYBound;
				//Y value is calculated between two verticies and x bound point. Maths on lecture notes
				pOutput->vert[pOutput->nv - 1].x = (((iYBound - pIinput->vert[CurVer].y)*(pIinput->vert[PreVer].x - pIinput->vert[CurVer].x)) / (pIinput->vert[PreVer].y - pIinput->vert[CurVer].y)) + pIinput->vert[CurVer].x;
				//Z value is the same as the input Z value
				pOutput->vert[pOutput->nv - 1].z = pIinput->vert[i].z;
			}
		}
		else //If previous vertex is not on screen 
		{
			if (pIinput->vert[CurVer].y >= iYBound)
			{
				//As new vertex is inline with the x boundary
				pOutput->vert[pOutput->nv - 1].y = iYBound;
				//Y value is calculated between two verticies and x bound point. Maths on lecture notes
				pOutput->vert[pOutput->nv - 1].x = (((iYBound - pIinput->vert[CurVer].y)*(pIinput->vert[PreVer].x - pIinput->vert[CurVer].x)) / (pIinput->vert[PreVer].y - pIinput->vert[CurVer].y)) + pIinput->vert[CurVer].x;
				//Z value is the same as the input Z value
				pOutput->vert[pOutput->nv - 1].z = pIinput->vert[i].z;
				pOutput->nv += 1;
				pOutput->vert[pOutput->nv - 1] = pIinput->vert[CurVer];
			}
			else
			{
				pOutput->nv -= 1;
			}
		}
		CurVer += 1;
		PreVer = CurVer - 1;
	}
	// Tell calling routine how many vertices in pOutput array
	return pOutput->nv;
}

//-----------------------------------------------------------------------------
// Name: ClipPolyXHi
// Desc: Clipping Routine for upper x boundary
//-----------------------------------------------------------------------------
int ClipPolyXHigh(POLYGON *pIinput, POLYGON *pOutput, int iXBound)
{


	//Lab 4 Task 2, Define indicies into the vertex array
	//Current vertex set to First vertex, previous set to last
	int CurVer = 0, PreVer = pIinput->nv - 1; //Amount of verticies - 1 to get index of last vertex
	//Lab 4 Task 3, copy input vector details to output
	//Input Poly Details are copied over to Output
	pOutput->colour = pIinput->colour;
	pOutput->normal = pIinput->normal;
	pOutput->texMap = pIinput->texMap;

	//Lab 4 Task 4
	//As we are unsure of how many verticies the new clipped poly will have
	pOutput->nv = 0;


	//Lab 4 Task 5 Structured english notes code

	//for each vertex
	for (int i = 0; i < pIinput->nv; i++)
	{
		pOutput->nv += 1; //Every loop add an extra vertex to new poly
		//If previous vertex is on the screen
		if (pIinput->vert[PreVer].x < iXBound)
		{
			//If current vertex is on the screen
			if (pIinput->vert[CurVer].x < iXBound)
			{
				pOutput->vert[pOutput->nv - 1] = pIinput->vert[CurVer];
				//Set first vertex of Output to first of Input
			}
			else
			{
				//As new vertex is inline with the x boundary
				pOutput->vert[pOutput->nv - 1].x = iXBound;
				//Y value is calculated between two verticies and x bound point. Maths on lecture notes
				pOutput->vert[pOutput->nv - 1].y = (((iXBound - pIinput->vert[CurVer].x)*(pIinput->vert[PreVer].y - pIinput->vert[CurVer].y)) / (pIinput->vert[PreVer].x - pIinput->vert[CurVer].x)) + pIinput->vert[CurVer].y;
				//Z value is the same as the input Z value
				pOutput->vert[pOutput->nv - 1].z = pIinput->vert[i].z;
			}
		}
		else //If previous vertex is not on screen 
		{
			if (pIinput->vert[CurVer].x < iXBound)
			{
				//As new vertex is inline with the x boundary
				pOutput->vert[pOutput->nv - 1].x = iXBound;
				//Y value is calculated between two verticies and x bound point. Maths on lecture notes
				pOutput->vert[pOutput->nv-1].y = (((iXBound - pIinput->vert[CurVer].x)*(pIinput->vert[PreVer].y - pIinput->vert[CurVer].y)) / (pIinput->vert[PreVer].x - pIinput->vert[CurVer].x)) + pIinput->vert[CurVer].y;
				//Z value is the same as the input Z value
				pOutput->vert[pOutput->nv - 1].z = pIinput->vert[i].z;
				pOutput->nv += 1;
				pOutput->vert[pOutput->nv - 1] = pIinput->vert[CurVer];
			}
			else
			{
				pOutput->nv -= 1;
			}
		}
		CurVer += 1;
		PreVer = CurVer - 1;
	}

	// Tell calling routine how many vertices in pOutput array
	return pOutput->nv;

}

//-----------------------------------------------------------------------------
// Name: ClipPolyYHi
// Desc: Clipping Routine for upper y boundary
//-----------------------------------------------------------------------------
int ClipPolyYHigh(POLYGON *pIinput, POLYGON *pOutput, int iYBound)
{//clipped at bottom of page

	//lab 4 task 2, define indicies into the vertex array
	//current vertex set to first vertex, previous set to last
	int curver = 0, prever = pIinput->nv - 1; //amount of verticies - 1 to get index of last vertex
											  //lab 4 task 3, copy input vector details to output
											  //input poly details are copied over to output
	pOutput->colour = pIinput->colour;
	pOutput->normal = pIinput->normal;
	pOutput->texMap = pIinput->texMap;

	//lab 4 task 4
	//as we are unsure of how many verticies the new clipped poly will have
	pOutput->nv = 0;

	//lab 4 task 5 structured english notes code

	//for each vertex
	for (int i = 0; i < pIinput->nv; i++)
	{
		pOutput->nv += 1; //every loop add an extra vertex to new poly
						  //if previous vertex is on the screen
		if (pIinput->vert[prever].y < iYBound)
		{
			//if current vertex is on the screen
			if (pIinput->vert[curver].y < iYBound)
			{
				pOutput->vert[pOutput->nv - 1] = pIinput->vert[curver];
				//set first vertex of output to first of input
			}
			else
			{
				//as new vertex is inline with the x boundary
				pOutput->vert[pOutput->nv - 1].y = iYBound;
				//y value is calculated between two verticies and x bound point. maths on lecture notes
				pOutput->vert[pOutput->nv - 1].x = (((iYBound - pIinput->vert[curver].y)*(pIinput->vert[prever].x - pIinput->vert[curver].x)) / (pIinput->vert[prever].y - pIinput->vert[curver].y)) + pIinput->vert[curver].x;
				//z value is the same as the input z value
				pOutput->vert[pOutput->nv - 1].z = pIinput->vert[i].z;
			}
		}
		else //if previous vertex is not on screen 
		{
			if (pIinput->vert[curver].y < iYBound)
			{
				//as new vertex is inline with the x boundary
				pOutput->vert[pOutput->nv - 1].y = iYBound;
				//y value is calculated between two verticies and x bound point. maths on lecture notes
				pOutput->vert[pOutput->nv - 1].x = (((iYBound - pIinput->vert[curver].y)*(pIinput->vert[prever].x - pIinput->vert[curver].x)) / (pIinput->vert[prever].y - pIinput->vert[curver].y)) + pIinput->vert[curver].x;
				//z value is the same as the input z value
				pOutput->vert[pOutput->nv - 1].z = pIinput->vert[i].z;
				pOutput->nv += 1;
				pOutput->vert[pOutput->nv - 1] = pIinput->vert[curver];
			}
			else
			{
				pOutput->nv -= 1;
			}
		}
		curver += 1;
		prever = curver - 1;
	}
	// Tell calling routine how many vertices in pOutput array
	return pOutput->nv;
}

//-----------------------------------------------------------------------------
// Name: Init
// Desc: Initialises Direct3D etc.
//		 This is called before switch to graphics mode,
//		 example of z buffer initialisation shown in comments,
//		 ignore for parts a/b.
//-----------------------------------------------------------------------------
