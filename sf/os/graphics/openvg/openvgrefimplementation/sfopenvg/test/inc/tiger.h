/*
* Copyright (c) 2009 Symbian Foundation Ltd
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Symbian Foundation Ltd - initial contribution.
* 
* Contributors:
*
* Description:
* CTiger class declaration to render OpenVG tiger image
*/

#ifndef TIGER_H
#define TIGER_H

//  INCLUDES
#include <e32base.h> // for CBase definition
#include <egl.h>
#include <openvg.h>
//#include <vgu.h>
#include <eglosnativewindowtype.h>

////////////////////////////////////////////
//Copied from ri_package_11\samples\tiger\tiger.h
extern const int KtigerCommandCount;
extern const char KtigerCommands[];
extern const float KtigerMinX;
extern const float KtigerMaxX;
extern const float KtigerMinY;
extern const float KtigerMaxY;
extern const int KtigerPointCount;
extern const float KtigerPoints[];

// CLASS DECLARATION

/**
 * Class that does the actual OpenGL ES rendering.
 */
typedef struct
{
	VGFillRule		m_fillRule;
	VGPaintMode		m_paintMode;
	VGCapStyle		m_capStyle;
	VGJoinStyle		m_joinStyle;
	float			m_miterLimit;
	float			m_strokeWidth;
	VGPaint			m_fillPaint;
	VGPaint			m_strokePaint;
	VGPath			m_path;
} PathData;

typedef struct
{
	PathData*			m_paths;
	int					m_numPaths;
} PS;

/////////////////////////////////////////////////////////////

class RWindow;

class CTiger : public CBase
    {
    public:  // Constructors and destructor

        /**
         * Factory method for creating a new CTiger object.
         */
        static CTiger* NewL( TUint aWidth, TUint aHeight);

        /**
         * Destructor. Does nothing.
         */
        virtual ~CTiger();

    public: // New functions

        /**
         * Initializes OpenGL ES, sets the vertex and color
         * arrays and pointers. Also selects the shading mode.
         */
        void AppInit( RWindow& aWindow, CFbsBitmap* aBitmapToDraw=NULL );
        
        /**
         * Called upon application exit. Does nothing.
         */
        void AppExit( void );

        /**
         * Notifies that the screen size has dynamically changed during execution of
         * this program. Resets the viewport to this new size.
         * @param aWidth New width of the screen.
         * @param aHeight New height of the screen.
         */
        void SetScreenSize( TUint aWidth, TUint aHeight );
        
        void AppRender( const TRect& aRect, CFbsBitmap* aDst=NULL);

    protected: // New functions

        /**
         * Standard constructor that must never Leave.
         * Stores the given screen width and height.
         * @param aWidth Width of the screen.
         * @param aHeight Height of the screen.
         */
        CTiger(TUint aWidth, TUint aHeight);

        /**
         * Second phase contructor. Does nothing.
         */
        void ConstructL( void );
        
        //fucntions copied from ri_package_11\samples\tiger\main.c - no attempt to Symbianise!!
        void PS_destruct(PS* ps);
        void render(int w, int h);
        void PS_render(PS* ps);
        PS* PS_construct(const char* commands, int commandCount, const float* points, int pointCount);
        void init(NativeWindowType window, CFbsBitmap* aBitmapToDraw=NULL);
        void CreateImage(CFbsBitmap* aBitmapToDraw);
        void renderFromBitmap(int w, int h);
        

    private: // Data

        /** Width of the screen */
        TUint           iScreenWidth;

        /** Height of the screen */
        TUint           iScreenHeight;

    private: //OpenVG structures
    	
    	//copied from ri_package_11\samples\tiger\main.c - no attempt to symbianise
    	PS* 				iTiger;
    	int					iRenderWidth;
    	int					iRenderHeight;
    	EGLDisplay			iEgldisplay;
    	EGLConfig			iEglconfig;
    	EGLSurface			iEglsurface;
    	EGLContext			iEglcontext;
    	TNativeWindowType iNativeWindowType;
    	VGImage iImage;
    	CFbsBitmap* iBitmapToDraw;
   	
    };
#endif // TIGER_H

// End of File
