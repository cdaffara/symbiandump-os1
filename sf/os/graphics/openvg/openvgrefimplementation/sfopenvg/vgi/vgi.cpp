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
* Implementation of VGI interface
*/

#include <e32std.h>
#include <vg\vgcontext.h>
#include <vg\vgcontext_symbian.h>
#include <egl.h>

#define MAX_WIDTH 		640 //*4 // in bytes
#define MAX_HEIGHT 		640 //*4 // in bytes
class TEgl
	{
public:
	TEgl() :
	iEgldisplay(0),
	iEglsurface(0),
	iEglcontext(0),
	iInitialised(EFalse),
	iPixmap(NULL)
	{};

public: //data
	EGLDisplay iEgldisplay;
	EGLSurface iEglsurface;
	EGLContext iEglcontext;
	TBool iInitialised;
	CFbsBitmap* iPixmap;
	};


TEgl& GetEglInstance()
	{
	//use TLS
	//use TLS to store static global egl
	TEgl* pEgl=NULL; 
	if((pEgl = static_cast<TEgl*>(Dll::Tls()))==NULL)
		{
		//create TLS instance
		pEgl = new(ELeave)TEgl;
		Dll::SetTls(pEgl);
		}
	return *pEgl;
	}

void ReleaseTls()
	{
	TEgl* pEgl = static_cast<TEgl*>(Dll::Tls());
	if (pEgl)
		delete pEgl; 
	Dll::SetTls(NULL);
	}

VGI_API_CALL int VGIInitialize( int /*width*/, int /*height*/, VGIColorSpace /*colorSpace*/ )
	{
	return VGI_OK;
	}

VGI_API_CALL int VGIInitializeEx( int /*width*/, int /*height*/, VGIColorSpace /*colorSpace*/, int /*premultiplied*/, int /*conformant*/ )
	{
	return VGI_OK;
	}

VGI_API_CALL int VGICopyToTarget( VGIColorBufferFormat /*format*/, int /*bufferStride*/, void */*buffer*/, int /*maskStride*/, void */*mask*/, VGICopyToTargetHint /*hint*/ )
	{
	return VGI_OK;
	}

VGI_API_CALL void VGITerminate( void )
	{

	}

VGI_API_CALL int VGIResize( int /*width*/, int /*height*/ )
	{
	return VGI_OK;
	}

VGI_API_CALL int VGIBindToImage( VGImage /*image*/ )
	{
	return VGI_OK;
	}	

VGI_API_CALL int VGIUnBindImage( void )
	{
	return VGI_OK;
	}



VGI_API_CALL TInt VGISymbianInitialize( TSize aSize, VGIColorSpace /*aColorSpace*/ )
	{
	TEgl& egl = GetEglInstance();
	//only init once
	if(!egl.iInitialised)
		{
		egl.iInitialised = ETrue;
		static const EGLint s_configAttribs[] =
			{
					EGL_RED_SIZE,
					8,
					EGL_GREEN_SIZE,
					8,
					EGL_BLUE_SIZE,
					8,
					EGL_ALPHA_SIZE,
					8,
					EGL_LUMINANCE_SIZE,
					EGL_DONT_CARE, //EGL_DONT_CARE
					EGL_SURFACE_TYPE,
					EGL_WINDOW_BIT,
					EGL_SAMPLES,
					1,
					EGL_NONE
			};
		EGLint numconfigs;
		
		egl.iEgldisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
		eglInitialize(egl.iEgldisplay, NULL, NULL);
		__ASSERT_ALWAYS(eglGetError() == EGL_SUCCESS,User::Invariant());
		eglBindAPI(EGL_OPENVG_API);
		
		EGLConfig  eglconfig;
		
		eglChooseConfig(egl.iEgldisplay, s_configAttribs, &eglconfig, 1, &numconfigs);
		__ASSERT_ALWAYS(eglGetError() == EGL_SUCCESS,User::Invariant());
		__ASSERT_ALWAYS(numconfigs == 1,User::Invariant());
		
		TSize maxSize(MAX_WIDTH,MAX_HEIGHT);
		egl.iPixmap = new(ELeave) CFbsBitmap();
		egl.iPixmap->Create( maxSize, EColor16MA );
		 
		egl.iEglsurface = eglCreatePixmapSurface(egl.iEgldisplay, eglconfig, (EGLNativePixmapType)egl.iPixmap, NULL);
		__ASSERT_ALWAYS(eglGetError() == EGL_SUCCESS,User::Invariant());
				
		egl.iEglcontext = eglCreateContext(egl.iEgldisplay, eglconfig, NULL, NULL);
		__ASSERT_ALWAYS(eglGetError() == EGL_SUCCESS,User::Invariant());
		eglMakeCurrent(egl.iEgldisplay, egl.iEglsurface, egl.iEglsurface, egl.iEglcontext);
		__ASSERT_ALWAYS(eglGetError() == EGL_SUCCESS,User::Invariant());
	
		}
		return VGI_OK;
	}

VGI_API_CALL TInt VGISymbianInitializeEx( TSize /*aSize*/, VGIColorSpace /*aColorSpace*/, TBool /*aPremultiplied*/, TBool /*aConformant*/ )
	{
	return VGI_OK;
	}

VGI_API_CALL TInt VGISymbianCopyToBitmap( CFbsBitmap *aBitmap, CFbsBitmap *aMaskBitmap, VGICopyToTargetHint /*aHint*/ )
	{
	TEgl& egl = GetEglInstance();	
	
	eglCopyBuffers(egl.iEgldisplay, egl.iEglsurface,(EGLNativePixmapType)aBitmap);
	
	if(aMaskBitmap)
		{
		eglCopyBuffers(egl.iEgldisplay, egl.iEglsurface,(EGLNativePixmapType)aMaskBitmap);		
		}
	
	__ASSERT_ALWAYS(eglGetError() == EGL_SUCCESS,User::Invariant());
	return VGI_OK;
	}

VGI_API_CALL void VGISymbianTerminate()
{
	TEgl& egl = GetEglInstance();
	eglDestroyContext(egl.iEgldisplay, egl.iEglcontext);
	eglDestroySurface(egl.iEgldisplay, egl.iEglsurface);
	delete egl.iPixmap;
	ReleaseTls();
}

VGI_API_CALL TInt VGISymbianResize( TSize aSize )
	{
	return VGI_OK;
	}

VGI_API_CALL TInt VGISymbianBindToImage( VGImage /*aImage*/ )
	{
	return VGI_OK;
	}

VGI_API_CALL TInt VGISymbianUnBindImage()
	{
	return VGI_OK;
	}

