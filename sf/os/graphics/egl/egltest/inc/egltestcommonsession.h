// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
*/
#ifndef EGLTESTCOMMONSESSION_H
#define EGLTESTCOMMONSESSION_H

#include <e32base.h>
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
#include <sgresource/sgimage.h>
#else
#include <graphics/sgimage.h>
#endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
#include <EGL/egl.h>


#include <test/egltestcommonutils.h>

//pointer to function for eglimage
typedef EGLImageKHR (*TFPtrEglCreateImageKhr) (EGLDisplay dpy, EGLContext ctx, EGLenum aTarget, EGLClientBuffer buffer, EGLint*attrib_list);
typedef EGLBoolean (*TFPtrEglDestroyImageKhr) (EGLDisplay dpy, EGLImageKHR image);
typedef EGLBoolean (*TFPtrVgCreateEglImageTargetKhr) (VGeglImageKHR image);
typedef EGLBoolean (*TFPtrEglSwapBuffersRegionNok) (EGLDisplay dpy, EGLSurface surface, EGLint numRects, const EGLint* rects);

//pointer to function for sync object
typedef EGLSyncKHR (*TFPtrEglCreateSyncKhr) (EGLDisplay dpy, EGLenum type, const EGLint *attrib_list);
typedef EGLBoolean (*TFPtrEglDestroySyncKhr) (EGLDisplay dpy, EGLSyncKHR sync);
typedef EGLint (*TFPtrEglClientWaitSyncKhr) (EGLDisplay dpy, EGLSyncKHR sync, EGLint flags, EGLTimeKHR timeout);
typedef EGLBoolean (*TFPtrEglSignalSyncKhr) (EGLDisplay dpy, EGLSyncKHR sync, EGLenum mode);
typedef EGLint (*TFPtrEglSignalSyncImpl) (EGLDisplay dpy, EGLSyncKHR sync, EGLenum mode);
typedef EGLBoolean (*TFPtrEglGetSyncAttribKhr) (EGLDisplay dpy, EGLSyncKHR sync, EGLint attribute, EGLint *value);
typedef EGLint (*TFPtrEglPrivateSignalSyncNok) (EGLDisplay dpy, EGLSyncKHR sync, EGLenum mode);

//pointer to function for surface scaling
typedef EGLBoolean (*TFPtrEglQuerySurfaceScalingCapabilityNok) (EGLDisplay dpy, EGLConfig config, EGLint surface_width, EGLint surface_height, EGLint target_width, EGLint target_height, EGLint *value);
typedef EGLBoolean (*TFPtrEglSetSurfaceScalingNok) (EGLDisplay dpy, EGLSurface surface, EGLint target_offset_x, EGLint target_offset_y, EGLint target_width, EGLint target_height);

const TUint KImagesArrayGranularity = 4;

// forward declarations
class CFbsBitmap;
class TRgb;
class CTestExecuteLogger;
class TMapEglConfigToPixelFormat;

// consts used by this class - use #define to avoid global temporary constructors
#define KPixmapSize 		TSize(100,100)
#define KPixmapSizeBigger	TSize(200,200)
#define KPixmapSquare 		TRect(25,25, 75,75)

#define KRgbReddish 	TRgb(200, 100, 50, 255)	// 0xC8, 0x64, 0x32 0xFF
#define KRgbGreenish 	TRgb(70, 220, 120, 255)	// 0x46, 0xDC, 0x78 0xFF
#define KRgbBlueish 	TRgb(150, 30, 240, 255)	// 0x96, 0x1E, 0xF0 0xFF
#define KRgbDummy	 	TRgb(0, 0, 0)			// 0x00, 0x00, 0x00


NONSHARABLE_CLASS(TMapEglConfigToPixelFormat)
	{
public:
	EGLint iBufferSize;
	EGLint iAlphaSize;
	EGLint iRedSize;
	EGLint iGreenSize;
	EGLint iBlueSize;
	EGLint iSurfaceTypeFlags;
	EGLint iColorBufferType;
	TUidPixelFormat iPixelFormat;
	TDisplayMode iDisplayMode;
	};

/** 
Provides a single thread with the fields and methods necessary to
create and use an EGL surface and context
*/
class CTestEglSession : public CBase
	{
public:
	enum TResourceCloseRule
		{
		EResourceCloseSgImageLate,
		EResourceCloseSgImageEarly, 
		EResourceCloseSgDriverAndImageEarly
		};	

public:
	inline CTestExecuteLogger& Logger() { return iLogger; }

	IMPORT_C static CTestEglSession* NewL(CTestExecuteLogger& aLogger, EGLDisplay& aDisplay, TInt aThreadIdx);
	IMPORT_C static CTestEglSession* NewLC(CTestExecuteLogger& aLogger, EGLDisplay& aDisplay, TInt aThreadIdx);
	IMPORT_C ~CTestEglSession();

public:
	//
	//Compound functions that construct surface 
	//
	IMPORT_C void CreateWindowSurfaceAndMakeCurrentL(EGLConfig aConfig, RWindow& aWindow, TBool aVgAlphaFormatPre = EFalse, EGLenum aBindAPI = EGL_OPENVG_API, TInt aRenderVersionNumber = 1, EGLint* aAttribList = NULL);	
	IMPORT_C void CreatePixmapSurfaceAndMakeCurrentAndMatchL(const TSgImageInfo& aImageInfo, TResourceCloseRule aResourceCloseRule, EGLenum aBindAPI = EGL_OPENVG_API, TInt aRenderVersionNumber = 1);
	IMPORT_C void CreatePixmapSurfaceAndMakeCurrentAndMatchL(const TSize& aSize, TDisplayMode aDisplayMode, EGLenum aBindAPI = EGL_OPENVG_API, TInt aRenderVersionNumber = 1);
    IMPORT_C void CreatePbufferSurfaceAndMakeCurrentL(EGLConfig aConfig, const TSize& aSize, EGLenum aBindAPI = EGL_OPENVG_API, TInt aRenderVersionNumber = 1);

    //
    //the following two functions are not recommended to use
    //
    IMPORT_C void CreatePixmapSurfaceAndMakeCurrentL(EGLConfig aConfig, const TSgImageInfo& aImageInfo, TResourceCloseRule aResourceCloseRule, EGLenum aBindAPI = EGL_OPENVG_API, TInt aRenderVersionNumber = 1);
    IMPORT_C void CreatePixmapSurfaceAndMakeCurrentL(EGLConfig aConfig, const TSize& aSize, TDisplayMode displayMode, EGLenum aBindAPI = EGL_OPENVG_API, TInt aRenderVersionNumber = 1);
	IMPORT_C EGLConfig GetConfigExactMatchL(TEglTestConfig aConfigAttribIndex, TEglConfigMatchRule aMatchRule = EConfigMatchStandard);

    //
    //function to check whether a particular extension is supported
    //
	IMPORT_C TBool CheckNeededExtensionL(TInt aExtension, const TDesC& aExtensionName);

	//
	// TODO: delete these and don't use them again
	//
	IMPORT_C void SetExpectedError(EGLint aExpectedErrorCode);
	IMPORT_C TBool CheckExpectedError(EGLint aFunctionReturnValue);
	IMPORT_C void CheckExpectedErrorL(EGLint aExpectedErrorCode);
	IMPORT_C void ResetExpectedError();
	
	
	IMPORT_C void TryUsePixmapCFbsBitmapL();
	IMPORT_C void TryUsePixmapCFbsBitmapOpenVgL(EGLConfig aConfig, const TSize& aSize, TDisplayMode aDisplayMode);
	IMPORT_C void TryUsePixmapCFbsBitmapOpenGlesL(EGLConfig aConfig, const TSize& aSize, TDisplayMode aDisplayMode, TInt aRenderVersion);
	IMPORT_C void TryUsePixmapRSgImageL();
	IMPORT_C TBool TryUsePixmapRSgImageOpenVgL(EGLConfig aConfig, const TSgImageInfo& aImageInfo, TResourceCloseRule aResourceCloseRule);
	IMPORT_C TBool TryUsePixmapRSgImageOpenGlesL(EGLConfig aConfig, const TSgImageInfo& aImageInfo, TResourceCloseRule aResourceCloseRule, TInt aRenderVersion);
	
	IMPORT_C const TMapEglConfigToPixelFormat* GetPixelFormatFromEglConfigL(EGLConfig aConfig);

	IMPORT_C void ViewConfigsL();
	IMPORT_C TBool IsACompatibleConfig(EGLConfig aConfig,RSgImage& aImage,TBool aLog);
	IMPORT_C CFbsBitmap* NativeFbsBitmap();
	IMPORT_C RSgImage& NativeSgImage();
	IMPORT_C EGLSurface Surface() const;
	IMPORT_C EGLContext Context() const;
	
	IMPORT_C TInt CreateBitmap(CFbsBitmap* aFbsBitmap, const TSize& aSize, TDisplayMode aDisplayMode);
	IMPORT_C TBool CongfigSupportsOpenVgL(EGLConfig aConfig);
	
	
	//
	//image extension utils
	//
	IMPORT_C TBool FetchProcEglCreateImageKhr();
	IMPORT_C TBool FetchProcEglDestroyImageKhr();
	IMPORT_C TBool FetchProcvgCreateImageTargetKhr();
	IMPORT_C EGLImageKHR eglCreateImageKhrL(EGLDisplay aDisplay,EGLContext aContext,EGLenum aTarget,RSgImage* aSgImage,const EGLint *aAttr_List);
	IMPORT_C EGLImageKHR eglCreateImageKhrL(EGLDisplay aDisplay,EGLContext aContext,EGLenum aTarget,CFbsBitmap &aCFbsBitmap,const EGLint *aAttr_List);
	IMPORT_C TBool DestroyEGLImage();
	IMPORT_C TBool DestroyEGLImage(EGLDisplay aDisplay, EGLImageKHR aEGLImageKHR);
	IMPORT_C VGImage vgCreateImageTargetKHR(VGeglImageKHR aImage);
		
	IMPORT_C void DrawOpenVgL();	
	IMPORT_C void DrawOpenGLesL();

	IMPORT_C void CheckImageDataL(CFbsBitmap* aFbsBitmap);
	IMPORT_C void CheckImageDataFullRedishL(CFbsBitmap* aFbsBitmap);	
	IMPORT_C void CheckImageDataVgL(VGImageFormat aDataFormat);
	IMPORT_C void CheckImageDataGLesL();
	
	IMPORT_C void CleanupSurfaceAndContextL();
	IMPORT_C void CleanupSurfaceFbsBitmapL();
	IMPORT_C void CleanupSurfaceSgImageL();
	IMPORT_C void ResetSurfaceAndContextSgImageL();
	
	IMPORT_C void SwapChannels(TUint32 &aSwapMe);
	
	IMPORT_C static void ConvertColor(const TRgb& aSource, VGfloat* aTarget);
	IMPORT_C TBool PixelsMatch(const TRgb& aExpected, const TRgb& aActual, TBool aCheckAlpha);
	
	IMPORT_C void CheckVgDrawingL(VGImageFormat aDataFormat, const CFbsBitmap* aReferenceBitmap);
	
	IMPORT_C CFbsBitmap* CreateReferenceBitmapL(TDisplayMode aMode);
	IMPORT_C CFbsBitmap* CreateReferenceBitmapL(TDisplayMode aMode, const TRgb& aColour);
	IMPORT_C CFbsBitmap* CreateReferenceBitmapL(TDisplayMode aMode, const TSize& aSize, const TInt aIndex);
	IMPORT_C CFbsBitmap* CreateReferenceMaskedBitmapL(TDisplayMode aRefBitmapMode, const TRgb& aPenBitmapColor, const CFbsBitmap* aMaskBitmap);
	
	IMPORT_C void InitializeL(TBool aTerminateDisplay = EFalse);
	IMPORT_C void TerminateDisplayL();
	
	IMPORT_C void OpenSgDriverL();
	IMPORT_C void CloseSgDriver();
	
	IMPORT_C void OpenFbsSessionL();
	IMPORT_C void CloseFbsSession();	

	IMPORT_C TBool IsOpenGLESSupported(); 
    IMPORT_C TBool IsOpenGLES2Supported();
    IMPORT_C TBool IsOpenVGSupported();

protected:
	IMPORT_C CTestEglSession(CTestExecuteLogger& aLogger, EGLDisplay& aDisplay, TInt aThreadIdx);
	
	void QueryExtensionsL(TExtensionsGroups aExtensionBelongsTo);	
	TBool FindExtensionStringL(TExtensionsGroups aExtensionBelongsTo, const TDesC8& aExtensionString);

	TInt GetFullMatchConfigIndex(EGLDisplay aDisplay, EGLConfig *aConfigs, TInt aNumConfigs, const EGLint aWantedAttribs[], TEglConfigMatchRule aMatchRule);

	const TMapEglConfigToPixelFormat& ConfigToPixelFormatTable(TInt aIndex) const;
	TInt ConfigToPixelFormatTableLength() const;
    
    void CheckAllAvailableRenders(); 
	
protected:
	CTestExecuteLogger& iLogger;
	TBool iVerboseLogging;

	/**
	 Logical thread index, used for outputing logging info for multi threaded tests
	 Each thread in a multi threaded test will use its own instance of this
	 CTestEglSession class, and pass in a unique index identifier
	 */
	TInt iThreadIdx;
	
	/**
	 Holds the parsed result of calling eglQueryString() for extensions
	 */
	RArray<TPtrC8> iExtensionStrings;
	TExtensionsGroups iExtensionGroupCached;

	// Extension Function pointer
	TFPtrEglCreateImageKhr 	ipfnEglCreateImageKHR;
	TFPtrEglDestroyImageKhr ipfnEglDestroyImageKHR;
	TFPtrVgCreateEglImageTargetKhr	ipfnvgCreateImageTargetKHR;
	TFPtrEglSwapBuffersRegionNok   ipfnEglSwapBuffersRegionNok;

	/** 
	 Copy of the display passed in during construction.  
	 Not owned by this object.
	*/
	EGLDisplay& iDisplay;
	EGLSurface iSurface;
	EGLContext iContext;

	/** 
	 When performing negative tests, we need to specify which error code we are expecting to produce.
	 The next call to an egl API method will check for this value.
	 If the value is different, then the test will leave with a failure code.
	 If the value is the same then the test may continue.  The value of iExpectedError will be reset to 
	 its initial value of EGL_SUCCESS;  	
	*/
	EGLint iExpectedErrorCode;
	VGErrorCode iExpectedErrorCodeVG;
		
	// Native image - only use one of these for any test
	CFbsBitmap* iFbsBitmap;
	RSgImage	iSgImage;
	
	TBool       iTerminateDisplay;
	TBool 		iSgDriverOpen;
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	RSgDriver   iSgDriver;
#endif	
	TBool 		iFbsSessionOpen;

	//we will use a "lazy" initialization for iIsOpenGLESSupported, iIsOpenGLES2Supported and iIsOpenVGSupportedvariable members, 
	//i.e. they will be initialized upon the first request
	TBool       iIsSupportedRenderInitialized; //signify that iIsOpenGLESSupported, iIsOpenGLES2Supported and iIsOpenVGSupported members have been initialized   
	TBool       iIsOpenGLESSupported;  
    TBool       iIsOpenGLES2Supported; 
    TBool       iIsOpenVGSupported;
	};

#endif // EGLTESTCOMMONSESSION_H
