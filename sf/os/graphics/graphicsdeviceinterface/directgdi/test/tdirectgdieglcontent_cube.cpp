// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "tdirectgdieglcontent_cube.h"
#include "tdisplaymode_mapping.h"
#include <fbs.h>
#include <graphics/sgimage_sw.h>
#include <graphics/fbsdefs.h>

// CONSTANTS
/** Camera parameters */
const TInt KCameraDistance = 100;
const TReal32 KFrustumLeft = -1.f;	//left vertical clipping plane
const TReal32 KFrustumRight = 1.f;	//right vertical clipping plane
const TReal32 KFrustumBottom = -1.f;//bottom horizontal clipping plane
const TReal32 KFrustumTop = 1.f;	//top horizontal clipping plane
const TReal32 KFrustumNear = 3.f;	//near depth clipping plane
const TReal32 KFrustumFar = 1000.f;	//far depth clipping plane

/* Define vertice coordinates for the cube
   Duplication of vertices needed for texturing every surface of the cube */
static const GLbyte KVertices[24 * 3] =
	{
	/* top */
	-1,  1,  1,
	1,  1,  1,
	1, -1,  1,
	-1, -1,  1,

	/* front */
	1,  1,  1,
	1,  1, -1,
	1, -1, -1,
	1, -1,  1,

	/* right */
	-1,  1,  1,
	-1,  1, -1,
	1,  1, -1,
	1,  1,  1,

	/* left */
	1, -1,  1,
	1, -1, -1,
	-1, -1, -1,
	-1, -1,  1,

	/* back */
	-1, -1,  1,
	-1, -1, -1,
	-1,  1, -1,
	-1,  1,  1,

	/* bottom */
	-1,  1, -1,
	1,  1, -1,
	1, -1, -1,
	-1, -1, -1
	};

/**
Indices for drawing the triangles.
The color of the triangle is determined by 
the color of the last vertex of the triangle.
*/
static const GLubyte KTriangles[12 * 3] =
	{
	/* top */
	1,0,3,
	1,3,2,

	/* front */
	5,4,7,
	5,7,6,

	/* right */
	9,8,11,
	9,11,10,

	/* left */
	13,12,15,
	13,15,14,

	/* back */
	17,16,19,
	17,19,18,

	/* bottom */
	21,22,23,
	21,23,20
	};

/* Macro for changing the input texture coordinate values from
   GLubyte [0,255] to GLbyte [-128,127]. See more info below. */
#define tex(u,v) (GLbyte)( (u) - 128 ) , (GLbyte)( (v) - 128 )

/* Input texture coordinates for each of the object vertices.
   The coordinates are given in GLbyte [-128,127] format.
   Since the texture picture coordinates are between values
   [0,255] for both width and height, we have defined a macro
   to change the input coordinates into a appropriate form.
   It is easier to think the texture coordinates as corresponding
   image pixel coordinates. This alone is not enough because
   if texture coordinates are not given between values [0,1],
   texture wrapping will occur. Therefore we need to
   scale the texture coordinates with appropriate texture matrix,
   which is defined in AppInit(). */
static const GLbyte KTexCoords[24 * 2] = 
	{
	/* top */
	tex(0,0), 
	tex(255,0),
	tex(255,255), 
	tex(0,255), 

	/* front */
	tex(0,255),
	tex(127,255),
	tex(127,127),
	tex(0,127),

	/* right */
	tex(127,255),
	tex(255,255),
	tex(255,127),
	tex(127,127),

	/* left */
	tex(0,127),
	tex(127,127),
	tex(127,0),
	tex(0,0),

	/* back */
	tex(127,127),
	tex(255,127),
	tex(255,0),
	tex(127,0),

	/* bottom */
	tex(255,255),
	tex(255,0),
	tex(0,0),
	tex(0,255)
	};

/**
Static constructor.
@param aPixelFormat Pixel format of pixmap buffer.
@param aSize Size of pixmap buffer.
*/
CGLCube* CGLCube::NewL(TUidPixelFormat aPixelFormat, const TSize& aSize)
	{
	CGLCube* self = NewLC(aPixelFormat, aSize);
	CleanupStack::Pop(self);
	return self;
	}

CGLCube* CGLCube::NewLC(TUidPixelFormat aPixelFormat, const TSize& aSize)
	{
	CGLCube* self = new(ELeave) CGLCube();
	CleanupStack::PushL(self);
	self->ConstructL(aPixelFormat, aSize);
	return self;
	}

/**
1st phase constructor
*/
CGLCube::CGLCube()
	{
	}

/**
2nd phase constructor
@param aPixelFormat Pixel format of pixmap buffer.
@param aSize Size of pixmap buffer.
*/
void CGLCube::ConstructL(TUidPixelFormat aPixelFormat, const TSize& aSize)
	{
	// init graphic environment
	User::LeaveIfError(SgDriver::Open());
	FbsStartup();
	User::LeaveIfError(RFbsSession::Connect());	
	InitEglL(aPixelFormat, aSize);
	}

/**
Destructor
*/
CGLCube::~CGLCube()
	{
	// deinit gfx environment
	DeInitEgl();
	SgDriver::Close();
	RFbsSession::Disconnect();
	}

/**
Egl environment initialisation for pixmap surface rendering.
@param aPixelFormat Pixel format of pixmap buffer.
@param aSize Size of pixmap buffer.
*/
void CGLCube::InitEglL(TUidPixelFormat aPixelFormat, const TSize& aSize)
	{
	// Get the display for drawing graphics
	iEglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if(iEglDisplay == NULL)
		{
		_LIT(KGetDisplayFailed, "eglGetDisplay failed");
		User::Panic(KGetDisplayFailed, 0);
		}

	// Initialize display
	if(eglInitialize(iEglDisplay, NULL, NULL) == EGL_FALSE)
		{
		_LIT(KInitializeFailed, "eglInitialize failed");
		User::Panic(KInitializeFailed, 0);
		}

	// switch api to GLES
	eglBindAPI(EGL_OPENGL_ES_API);
	
	iImageInfo.iSizeInPixels = aSize;
	iImageInfo.iPixelFormat =  aPixelFormat;
	iImageInfo.iCpuAccess = ESgCpuAccessNone;
	iImageInfo.iUsage = ESgUsageOpenGlesTarget|ESgUsageDirectGdiSource;
	iImageInfo.iShareable = ETrue;
	iImageInfo.iScreenId = KSgScreenIdMain;

	for(TInt i=0; i<KEglContentBuffers; i++)
		{
		User::LeaveIfError(iImages[i].Create(iImageInfo, NULL, 0));

		EGLint numOfConfigs = 0; 

		// Define properties for the wanted EGLSurface 
		const EGLint attribList[] =
			{
			EGL_MATCH_NATIVE_PIXMAP, (EGLint)&iImages[i],
			EGL_SURFACE_TYPE, EGL_PIXMAP_BIT,
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
			EGL_NONE
			};

		// Choose an EGLConfig that best matches to the properties in attribList
		if(eglChooseConfig(iEglDisplay, attribList, &iConfig, 1, &numOfConfigs) == EGL_FALSE)
			{
			_LIT(KChooseConfigFailed, "eglChooseConfig failed");
			User::Panic(KChooseConfigFailed, 0);
			}

		const EGLint ppixmapAttribs[] = { EGL_NONE };
		iEglSurfaces[i] = eglCreatePixmapSurface(iEglDisplay, iConfig, &iImages[i], ppixmapAttribs);
		if(iEglSurfaces[i] == NULL)
			{
			_LIT(KCreatePixmapSurfaceFailed, "eglCreatePixmapSurface failed");
			User::Panic(KCreatePixmapSurfaceFailed, 0);
			}
		}
	iEglContext = eglCreateContext(iEglDisplay, iConfig, EGL_NO_CONTEXT, NULL);
	if(iEglContext == NULL)
		{
		_LIT(KCreateContextFailed, "eglCreateContext failed");
		User::Panic(KCreateContextFailed, 0);
		}

	if(eglMakeCurrent(iEglDisplay, iEglSurfaces[iCurrentImage], iEglSurfaces[iCurrentImage], iEglContext) == EGL_FALSE)
		{
		_LIT(KMakeCurrentFailed, "eglMakeCurrent failed");
		User::Panic(KMakeCurrentFailed, 0);
		}

	// Prepare texture map (shaded chessboard)
	GLubyte* texData = new(ELeave) GLubyte[64*64*4];
	for(TInt i=0; i<64; i++)
		{
		for(TInt j=0; j<64; j++)
			{
			if((i&8)^(j&8)) // switch 'white' and 'black' fields
				{
				texData[i*64*4+j*4+0] = i*4; // r
				texData[i*64*4+j*4+1] = j*4; // g
				texData[i*64*4+j*4+2] = (i+j)*2; // b
				}
			else
				{
				texData[i*64*4+j*4+0] = 255-i*4; // r
				texData[i*64*4+j*4+1] = 255-j*4; // g
				texData[i*64*4+j*4+2] = 255-(i+j)*2; // b
				}
			texData[i*64*4+j*4+3] = 255; // alpha
			}
		}

	// Generate texture
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &iTexId);
	glBindTexture(GL_TEXTURE_2D, iTexId);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
	delete [] texData;
	}

/**
Egl environment destroying.
*/
void CGLCube::DeInitEgl()
	{
	glDeleteTextures(1, &iTexId);
	eglMakeCurrent(iEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroyContext(iEglDisplay, iEglContext);
	for(TInt i=0; i<KEglContentBuffers; i++)
		{
		eglDestroySurface(iEglDisplay, iEglSurfaces[i]);
		iImages[i].Close();
		}
	eglTerminate(iEglDisplay);
	eglReleaseThread();
	}

/**
Render frame of spinning cube.
@param aFrame Number of frame to render.
*/
void CGLCube::Render(TInt aFrame)
	{
	if(eglMakeCurrent(iEglDisplay, iEglSurfaces[iCurrentImage], iEglSurfaces[iCurrentImage], iEglContext) == EGL_FALSE)
		{
		_LIT(KMakeCurrentFailed, "eglMakeCurrent failed");
		User::Panic(KMakeCurrentFailed, 0);
		}

	// Set the screen background color. 
	glClearColor(0.f, 0.f, 0.f, 1.f);

	// Enable back face culling, texturing, and normalization.
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_NORMALIZE);

	// Initialize viewport and projection. 
	glViewport(0, 0, iImageInfo.iSizeInPixels.iWidth, iImageInfo.iSizeInPixels.iHeight);

	// Calculate the view frustrum
	GLfloat aspectRatio = (GLfloat)(iImageInfo.iSizeInPixels.iWidth) / (GLfloat)(iImageInfo.iSizeInPixels.iHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustumf(KFrustumLeft * aspectRatio, KFrustumRight * aspectRatio,
			KFrustumBottom, KFrustumTop,
			KFrustumNear, KFrustumFar);

	/* Initialize appropriate texture matrix. First we have to translate the
       input texture coordinate values to be within a range of [0,255]. Hence
       we translate the x- and y-coordinate values by 128. Recall that the 
       values in nokTexCoords are between [-128,127], now they are in a range 
       of [0,255]. After that we scale the values by 1/255 to make the values 
       to be in range [0,1]. */
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glScalef(1.0f/255.0f, 1.0f/255.0f, 1.0f);
	glTranslatef(128.0f, 128.0f, 0.0f);

	// Remember to change the matrix mode to modelview.
	glMatrixMode(GL_MODELVIEW);

	// Enable vertex and texture arrays.
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	// Set array pointers. 
	glVertexPointer(3, GL_BYTE, 0, KVertices);
	glTexCoordPointer(2, GL_BYTE, 0, KTexCoords);

	// Set the initial shading mode 
	glShadeModel(GL_FLAT);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_DONT_CARE);

	glClear(GL_COLOR_BUFFER_BIT);

	// Animate and draw box 
	glLoadIdentity();
	glTranslatex(0 , 0 , -KCameraDistance << 16);
	glRotatex(aFrame << 18, 1 << 16,    0   ,    0   );
	glRotatex(aFrame << 17,    0   , 1 << 16,    0   );
	glRotatex(aFrame << 16,    0   ,    0   , 1 << 16);
	glScalex(20 << 16, 20 << 16, 20 << 16);

	glBindTexture(GL_TEXTURE_2D, iTexId);
	glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_BYTE, KTriangles);

	iLastImage = iCurrentImage;
	}

/**
Get image id of current frame. Current image to render is switch to next.
@param aId Reference to drawable id class to store image id.
*/
void CGLCube::GetImage(TSgDrawableId& aId)
	{
	// finish rendering
	glFinish();

	aId = iImages[iLastImage].Id();
	// switch to next buffer to prevent overdraw of image by asynchronous rendering
	if(iLastImage == iCurrentImage)
		{
		iCurrentImage = (iCurrentImage+1)%KEglContentBuffers;
		}
	}
