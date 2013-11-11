// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent - Internal Symbian test code
*/

#include <test/graphicsfontutils.h>
#include <EGL/egl.h>
#include <VG/openvg.h>
#include <graphics/fbsglyphmetricsarray.h> 
#include <graphics/fbsglyphdataiterator.h>
#include <sgresource/sgimage.h>
#include <sgresource/sgdriver_test.h>
#include <sgresource/sgdriver_profiling.h>
#include "FbsMessage.h"
#include "tfbsglyphdata.h"

_LIT(KTypefaceName, "DejaVu Sans Condensed");
//_LIT(KMonoTypefaceName, "DejaVu Sans Mono");
const TInt KNumGlyphCodesLatin = 96;
const TUint KDejaVuInvalidGlyphCode = 0;

// Currently only used in debug. When TestMultithreadStressAtlas() test is enabled, #ifdef to be removed. 
#ifdef _DEBUG
const TInt KTestThreadMinHeapSize = 0x20000;
const TInt KTestThreadMaxHeapSize = 0x20000;
#endif

// 'most significant bit' flag to ensure value is interpreted as a glyph code rather than an ascii code
const TUint KGlyphCodeFlag = 0x80000000;      


// Please note the following macros which enable helper functions, and are declared in the header.
// SAVEGLYPHSTOMBMDURINGCOMPARISON and
// SAVEGLYPHSTOMBMDEBUGFUNCTION

// Utility function declarations - utilities used by the tests
static TFontSpec GenerateDejaVuFontSpec(TInt aSeed);
static void CopyCharLine(TUint32*& aBinaryDataPtr,TInt aBufferWords,const TUint8* aData,TInt aBitShift,TInt aCharWidth, TInt16 aRepeatCount);
static void DecodeBinaryData(const TSize& aDataSize, const TUint8* aData, TInt aStride,	TUint32* aBinaryData);
static TInt CreateSgImageFromCharacterData(const TUint8* aData, const TSize& aSize, TGlyphBitmapType aType, RSgImage& aImage);
static TInt CreateSgImageFromCharacterData(const TUint8* aData, const TSize& aSize, TGlyphBitmapType aType, RSgImage& aImage, TUint8* aBuffer1, TUint8* aBuffer2);
// Following functions commented out because the tests which use these functions 
// are currently commented out due to Broadcom defect 
// ESLM-85LDV7 - TB10.1 Closing of RSgImage with duplicate handle used in same thread does not release GPU RAM
//static TInt FillGraphicsMemoryWithImages(const TSize& aSize, RArray<RSgImage>& aImages);
//static TInt NearlyFillGraphicsMemoryWithImages(const TSize& aSize, RArray<RSgImage>& aImages);

#if defined (SAVEGLYPHSTOMBMDEBUGFUNCTION) || defined (SAVEGLYPHSTOMBMDURINGCOMPARISON)
/**
Static utility function. Converts an A8 RSgImage into a CFbsBitmap.
To do this, the RSgImage is converted to an EGLImage, then to a VGImage,
where the image memory is read into a CFbsBitmap.

@param aEGL The EGL helper object that will read the SgImage into a memory buffer.
@param aSgImage The RSgImage to convert.
@param aRect A rectangular region of the RSgImage to convert.
@param aBitmap On success, holds a pointer to a CFbsBitmap which contains the image
	data of the RSgImage.
@return One of the system-wide error codes.
*/
static TInt CreateBitmapFromSgImage(CEGLHelper* aEGL, const RSgImage& aSgImage, const TRect& aRect, CFbsBitmap*& aBitmap)
	{
	TInt err = KErrNone;
	const TSize bufferSize = aRect.Size();
	const TInt dataStride = bufferSize.iWidth;

	TUint8* imageBuffer = reinterpret_cast<TUint8*>(User::AllocZ(bufferSize.iHeight * dataStride));
	if (!imageBuffer)
		{
		return KErrNoMemory;
		}
	err = aEGL->GetSgImageData(aSgImage, aRect, imageBuffer);
	if (err != KErrNone)
		{
		User::Free(imageBuffer);
		return err;
		}
	aBitmap = new CFbsBitmap();
	if (!aBitmap)
		{
		User::Free(imageBuffer);
		return KErrNoMemory;
		}

	err = aBitmap->Create(bufferSize, EGray256);
	if (KErrNone == err)
		{
		TUint8* buf = imageBuffer;
		aBitmap->BeginDataAccess();
		TUint8* dataAddress = reinterpret_cast<TUint8*>(aBitmap->DataAddress());
		const TInt dataStride = aBitmap->DataStride();	
		for (TInt scanline = 0; scanline < bufferSize.iHeight; scanline++)
			{
			Mem::Copy(dataAddress, buf, bufferSize.iWidth);
			dataAddress += dataStride;
			buf += bufferSize.iWidth;
			}
		aBitmap->EndDataAccess(EFalse);
		}
	else
		{
		delete aBitmap;
		aBitmap = NULL;
		}

	User::Free(imageBuffer);
	return err;
	}

/**
Utility function to aid with debugging.
Saves a bitmap to file.

@param aBmp Bitmap to save
@param aMeta Optional. If specified, it is added to the name of the bitmap file.
@param aRef Flag to show whether bitmap is a reference bitmap (ETrue) or test bitmap (EFalse).
*/
static void SaveBmp(CFbsBitmap* aBmp, TPtrC* aMeta, TBool aRef)
	{
	if (!aBmp)
		{
		return;
		}
	
	TBuf<256> testFileName;
	if (aRef)
		{
		testFileName.Append(_L("Ref"));
		}
	else
		{
		testFileName.Append(_L("Test"));
		}
	if (aMeta)
		{
		testFileName.Append(*aMeta);
		}

	TFileName mbmFile;
	TBuf<20> testPathName;
	#ifdef __WINS__
		testPathName.Append(_L("c:\\%S.mbm"));
	#else
		testPathName.Append(_L("e:\\%S.mbm"));
	#endif
	mbmFile.Format(testPathName, &testFileName);

	// As this is for debugging purposes only, doesn't matter reporting whether
	// saving succeeded or not.
	aBmp->Save(mbmFile);
	}
#endif // SAVEGLYPHSTOMBMDEBUGFUNCTION OR SAVEGLYPHSTOMBMDURINGCOMPARISON


#ifdef SAVEGLYPHSTOMBMDEBUGFUNCTION
void CTFbsGlyphData::SaveRSgImagesAsMbms(CEGLHelper* aEGL, const RSgImage& aImageA, const TRect& aRectA, const RSgImage& aImageB, const TRect& aRectB )
	{
	static TInt countToAppend = 0;

	CFbsBitmap* bitmap = NULL;
	if (KErrNone == CreateBitmapFromSgImage(aEGL, aImageA, aRectA, bitmap))
		{
		TBuf<KMaxFileName> buf( _L("String") );
		buf.AppendNum( countToAppend );
		TPtrC nameAppend( buf );

		SaveBmp(bitmap, &nameAppend, EFalse);
		}
	delete bitmap;  
	bitmap = NULL;

	if (KErrNone == CreateBitmapFromSgImage(aEGL, aImageB, aRectB, bitmap))
		{
		TBuf<KMaxFileName> buf( _L("String") );
		buf.AppendNum( countToAppend );
		TPtrC nameAppend( buf );

		SaveBmp(bitmap, &nameAppend, ETrue);
		}
	delete bitmap;
	bitmap = NULL;

	countToAppend++;
	}

/**
Static debug utility method that outputs the glyph images of the given glyph
codes for the given font to a file.
 */
static void DumpFontGlyphs(CEGLHelper* aEGL, CFont* aFont, TInt aCodesCount)
	{
	TFontSpec fontSpec = aFont->FontSpecInTwips();
	TOpenFontCharMetrics charMetrics;
	TSize bitmapSize;
	const TUint8* bitmapData = NULL;
	
	for (TInt glyphCode = 0; glyphCode < aCodesCount; glyphCode++)
		{
		CFont::TCharacterDataAvailability availability = aFont->GetCharacterData(glyphCode | KGlyphCodeFlag, charMetrics, bitmapData, bitmapSize);
		if (availability == CFont::EAllCharacterData)
			{
			RSgImage characterDataImage;
			TInt err = CreateSgImageFromCharacterData(bitmapData, bitmapSize, fontSpec.iFontStyle.BitmapType(), characterDataImage);
			if (err == KErrNone)
				{
				CFbsBitmap* bitmap = NULL;
				err = CreateBitmapFromSgImage(aEGL, characterDataImage, TRect(TPoint(0, 0), bitmapSize), bitmap);
				if (err == KErrNone)
					{
					TBuf<256> bitmapName;
					bitmapName.AppendFormat(_L("%S-%i"), &(fontSpec.iTypeface.Name()), glyphCode);
					TPtrC bitmapNamePtr(bitmapName);
					SaveBmp(bitmap, &bitmapNamePtr, EFalse);
					delete bitmap;
					}
				}
			characterDataImage.Close();
			}
		}
	}
#endif // SAVEGLYPHSTOMBMDEBUGFUNCTION


/**
Utility to return a fontspec such that the font created from it will
not match any other font generated by a different seed. The font
will be useable by RFbsGlyphDataIterator and RFbsGlyphMetricsArray.
It will always return a font based on the DejaVu fontspec, this is 
so that the glyphcodes in DejaVuASCIIToGlyphCode are guaranteed to
work.

@param aSeed Specifies a variant of the fontspec to create. Passing the
	same seed will cause the same TFontSpec to be returned.
@return The generated fontspec. 
 */
static TFontSpec GenerateDejaVuFontSpec(TInt aSeed)
	{
	const TInt KFontHeightStep = 4;
	const TInt KFontInitialHeight = 8;
	
	const TInt KNumFontTypefaces = 3;
	const TInt KNumFontBitmapTypes = 2;
	const TInt KNumFontStyles = 4;
	
	TInt fontBitmapTypeVariant = aSeed % KNumFontBitmapTypes;
	TInt fontStyleVariant = (aSeed / KNumFontBitmapTypes) % KNumFontStyles;
	TInt fontTypefaceVariant = (aSeed / ( KNumFontStyles * KNumFontBitmapTypes)) % KNumFontTypefaces;
	TInt fontHeightVariant = aSeed / (KNumFontStyles * KNumFontTypefaces * KNumFontBitmapTypes);
	
	TFontSpec fontSpec;	
	fontSpec.iHeight = KFontInitialHeight + (fontHeightVariant * KFontHeightStep);
	// Set the typeface name
	// Set the style.
	switch (fontStyleVariant)
		{
		case 1: // italic
			fontSpec.iFontStyle.SetPosture(EPostureItalic);
			fontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightNormal);
			break;
		case 2: // bold
			fontSpec.iFontStyle.SetPosture(EPostureUpright);
			fontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
			break;
		case 3: // bold italic
			fontSpec.iFontStyle.SetPosture(EPostureItalic);
			fontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
			break;
		default: // normal 
			fontSpec.iFontStyle.SetPosture(EPostureUpright);
			fontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightNormal);
			break;
		}
	switch (fontTypefaceVariant)
		{
		case 1:
			fontSpec.iTypeface.SetName(_L("DejaVu Sans Mono"));
			break;
		case 2:
			fontSpec.iTypeface.SetName(_L("DejaVu Serif Condensed"));
			break;
		case 3:
			fontSpec.iTypeface.SetName(_L("DejaVu Sans Condensed"));
			break;
		}
	switch(fontBitmapTypeVariant)
		{
		case 1:
			fontSpec.iFontStyle.SetBitmapType(EMonochromeGlyphBitmap);
			break;
		default:
			fontSpec.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
			break;
		}

	return fontSpec;
	}


/**
 * 
 EGL helper class to retrieve image data from an SgImage into a memory buffer.
 */
CEGLHelper::CEGLHelper() :
	iDisplay(EGL_NO_DISPLAY),
	iContext(EGL_NO_CONTEXT),
	iSurface(EGL_NO_SURFACE)
	{
	}
CEGLHelper::~CEGLHelper()
	{
	iMutex.Close();
	eglMakeCurrent(iDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroyContext(iDisplay, iContext);
	eglDestroySurface(iDisplay, iSurface);
	eglTerminate(iDisplay);
	eglReleaseThread();
	iSgDriver.Close();
	}

/**
Factory method to create CEGLHelper.
@return A pointer to an instance of CEGLHelper.
 */
CEGLHelper* CEGLHelper::NewL()
	{
	CEGLHelper* self = new CEGLHelper();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(1); // self
	return self;
	}

/**
Opens handle to the process-wide synchronisation semaphore,
loads EGL and VG extension function pointers,
sets up EGL resources so that EGLImages can be constructed. 
 */
void CEGLHelper::ConstructL()
	{
	_LIT(KEGLMutex, "TFbsGlyphDataEGLMutex");
	User::LeaveIfError(iMutex.CreateGlobal(KEGLMutex, EOwnerProcess));
	User::LeaveIfError(iSgDriver.Open());

	iDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (iDisplay == EGL_NO_DISPLAY)
		{
		User::Leave(KErrNotSupported);
		}
	if (EGL_TRUE != eglInitialize(iDisplay, NULL, NULL))
		{
		User::Leave(KErrNotSupported);
		}
	eglBindAPI(EGL_OPENVG_API);

	// Load the necessary EGL extensions...
	eglCreateImageKHR = reinterpret_cast<PFNEGLCREATEIMAGEKHRPROC>(eglGetProcAddress("eglCreateImageKHR"));
	eglDestroyImageKHR = reinterpret_cast<PFNEGLDESTROYIMAGEKHRPROC>(eglGetProcAddress("eglDestroyImageKHR"));
	vgCreateImageTargetKHR = reinterpret_cast<TvgCreateEGLImageTargetKHRTypefPtr>(eglGetProcAddress("vgCreateEGLImageTargetKHR"));
	if (!eglCreateImageKHR || !eglDestroyImageKHR || !vgCreateImageTargetKHR)
		{
		User::Leave(KErrExtensionNotSupported);
		}

	// In order to create VGImages from EGLImages, a context must be current.
	// Therefore create an EGLContext and EGLSurface to make current, using
	// a dummy RSgImage.

	RSgImage dummySurface;
	TSgImageInfo dummySurfaceInfo(TSize(1, 1), ESgPixelFormatRGB_565, ESgUsageBitOpenVgSurface);
	User::LeaveIfError(dummySurface.Create(dummySurfaceInfo, NULL, 0));
	CleanupClosePushL(dummySurface);

	EGLint configAttribs[] = 
		{
		EGL_MATCH_NATIVE_PIXMAP, (EGLint)&dummySurface,
		EGL_RENDERABLE_TYPE, EGL_OPENVG_BIT,
		EGL_NONE
		};

	EGLint configId = 0;
	EGLint numConfigs = 0;
	if (EGL_FALSE == eglChooseConfig(iDisplay, configAttribs, &configId, 1, &numConfigs) || numConfigs == 0)
		{
		User::Leave(KErrGeneral);
		}
	iContext = eglCreateContext(iDisplay, configId, EGL_NO_CONTEXT, NULL);
	if (iContext == EGL_NO_CONTEXT)
		{
		User::Leave(KErrGeneral);
		}
	iSurface = eglCreatePixmapSurface(iDisplay, configId, &dummySurface, NULL);
	if (iSurface == EGL_NO_SURFACE)
		{
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(1); // dummySurface
	}

/**
Retrieves the data from an A8 RSgImage into a buffer.
To do this, the RSgImage is converted to an EGLImage, then to a VGImage,
where the image memory is read into the given buffer.
The function can be called from multiple threads and synchronisation
with EGL is controlled via a mutex.

@param aSgImage The RSgImage to convert.
@param aRect A rectangular region of the RSgImage to convert.
@param aBuf On success, contains the image data of the RSgImage.
@return One of the system-wide error codes.
 */
TInt CEGLHelper::GetSgImageData(const RSgImage& aSgImage, const TRect& aRect, TUint8*& aBuf)
	{
	const TSize bufferSize = aRect.Size();
	const TInt dataStride = bufferSize.iWidth;

	if (bufferSize == TSize(0,0))
		{
		return KErrNone;
		}
	iMutex.Wait();

	TInt err = KErrNone;
	EGLImageKHR eglImage;
	if (EGL_FALSE == eglBindAPI(EGL_OPENVG_API))
		{
		err = KErrGeneral;
		}
	else if (EGL_FALSE == eglMakeCurrent(iDisplay, iSurface, iSurface, iContext))
		{
		err = KErrGeneral;
		}
	else
		{
		// Create EGLImages from the RSgImage.
		EGLint imageAttribs[] =
			{
			EGL_IMAGE_PRESERVED_KHR, EGL_TRUE, 
			EGL_NONE
			};
		eglImage = eglCreateImageKHR(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, reinterpret_cast<EGLClientBuffer>(&aSgImage), imageAttribs);
		if (eglImage == EGL_NO_IMAGE_KHR)
			{
			err = KErrArgument;
			}
		}
	if (err == KErrNone)
		{
		// Create VGImages from the EGLImage.
		VGImage vgImage = vgCreateImageTargetKHR(eglImage);
		eglDestroyImageKHR(iDisplay, eglImage);
		if (vgImage == VG_INVALID_HANDLE)
			{
			err = KErrArgument;
			}
		else
			{
			// Get the image data in 8bpp format
			vgGetImageSubData(vgImage, aBuf, dataStride, VG_A_8, aRect.iTl.iX, aRect.iTl.iY, bufferSize.iWidth, bufferSize.iHeight);
			vgDestroyImage(vgImage);
			}
		}
	eglReleaseThread();
	iMutex.Signal();
	return err;
	}

CTFbsGlyphData::CTFbsGlyphData(CTestStep* aStep):
	CTGraphicsBase(aStep)
	{
	}

void CTFbsGlyphData::ConstructL()
	{
	User::LeaveIfError(Logger().ShareAuto());
	User::LeaveIfError(RFbsSession::Connect());
	iFbs = RFbsSession::GetSession();
	iTs = (CFbsTypefaceStore*)CFbsTypefaceStore::NewL(NULL);
	User::LeaveIfError(iTs->GetNearestFontToDesignHeightInPixels((CFont*&)iFont, TFontSpec(KTypefaceName, 15)));
	User::LeaveIfError(iTs->GetNearestFontToDesignHeightInPixels((CFont*&)iFont2, TFontSpec(KTypefaceName, 8)));
	
	CCharCodeConverter* converter = CCharCodeConverter::NewLC();
	converter->UseFontL(iFont);
	iGlyphCodesLatin = new(ELeave) TUint[KNumGlyphCodesLatin];
	for (TInt ii = 0; ii < KNumGlyphCodesLatin; ++ii)
		{
		TUint asciiCode = ii+0x20; // ASCII characters from 0020 to 007F
		iGlyphCodesLatin[ii] = converter->GlyphCodeL(asciiCode);
		}
	CleanupStack::PopAndDestroy(1); // converter
	
	User::LeaveIfError(iSgDriver.Open());
	iEGL = CEGLHelper::NewL();
	
	// Creating a CFbsBitmap will force the RFbsSession to allocate a scanline buffer
	// now rather than in the middle of a test, thus avoiding heap check failure. 
	CFbsBitmap* dummyBitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(dummyBitmap);
	User::LeaveIfError(dummyBitmap->Create(TSize(512, 1), EGray256));
	CleanupStack::PopAndDestroy(dummyBitmap);

	INFO_PRINTF1(_L("FBSERV Glyph Data Testing"));
	}




CTFbsGlyphData::~CTFbsGlyphData()
	{
	delete iEGL;
	iSgDriver.Close();
	if (iTs)
		{
		iTs->ReleaseFont(iFont);
		iTs->ReleaseFont(iFont2);
		}
	delete iTs;
	delete[] iGlyphCodesLatin;
	User::Free(iTempBuf1);
	User::Free(iTempBuf2);
	RFbsSession::Disconnect();
	}

void CTFbsGlyphData::RunTestCaseL(TInt aCurTestCase)
	{
	((CTFbsGlyphDataStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	
	TRAPD(leave, 

	switch(aCurTestCase)
		{
	case 1:
		((CTFbsGlyphDataStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0624"));
		TestConsistencyWithGetCharacterData();
		break;
	case 2:
		((CTFbsGlyphDataStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0625"));
		TestInvalidGlyphCode();
		break;
	case 3:
		((CTFbsGlyphDataStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0626"));
		TestGlyphMetricsArrayParameters();
		break;
	case 4:
		((CTFbsGlyphDataStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0627"));
		TestGlyphMetricsArrayReuse();
		break;
	case 5:
		((CTFbsGlyphDataStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0628"));
		TestGlyphDataIteratorClose();
		break;
	case 6:
		((CTFbsGlyphDataStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0629"));
		TestGlyphDataIteratorSequence();
		break;	
	case 7:
		((CTFbsGlyphDataStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0632"));
		TestGlyphDataIteratorMultipleUsesOnMultipleFonts();
		break;
	case 8:
		((CTFbsGlyphDataStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0633"));
		TestGlyphDataIteratorImageValidity();
		break;
	case 9:
		((CTFbsGlyphDataStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0634"));
		TestGlyphDataIteratorOpenInvalidCode();
		break;
	case 10:
		((CTFbsGlyphDataStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0636"));
		TestGlyphDataIteratorOpenTwice();
		break;
	case 11:
		((CTFbsGlyphDataStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0637"));
		TestGlyphDataIteratorOpenTwiceWithDifferentFonts();
		break;
	case 12:
		((CTFbsGlyphDataStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0638"));
		TestGlyphDataIteratorOpenTooBigFont();
		break;
	case 13:
		((CTFbsGlyphDataStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0640"));
		TestGlyphDataIteratorOpenWithWrongArgument();
		break;
	case 14:
		((CTFbsGlyphDataStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0641"));
		TestGlyphDataIteratorImageMemoryLeak();
		break;
	case 15:
		((CTFbsGlyphDataStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0662"));
		TestGlyphDataIteratorNoGraphicsMemory();
		break;
	case 16: 
		((CTFbsGlyphDataStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0659"));
		TestGlyphDataIteratorLargeFontStress();
		break;
	case 17: 
		((CTFbsGlyphDataStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0660"));
		TestGlyphDataIteratorManyFontsStressL();
		break;
	case 18:
		((CTFbsGlyphDataStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0666"));
		TestGlyphDataIteratorNextIsAtomic();
		break;
	case 19:
		((CTFbsGlyphDataStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0665"));
		TestGlyphDataIteratorSameGlyphCodes();
		break;
	case 20:
		((CTFbsGlyphDataStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0668"));
		TestGlyphDataIteratorManyArraySizes();
		break;
	case 21:
		((CTFbsGlyphDataStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0669"));
		TestBitmapFontSupport();
		break;
	case 22:
		((CTFbsGlyphDataStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0671"));
		TestMultithreadShareSingleFont();
		break;
	case 23:
		((CTFbsGlyphDataStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0672"));
		TestMultithreadStressAtlas();
		break;
    case 24:
        ((CTFbsGlyphDataStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0673"));
        TestGlyphMetricsArrayHeapOOML();
        break;
    case 25:
        ((CTFbsGlyphDataStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0674"));
        TestGlyphDataIteratorHeapOOML();
        break;
	default:
		((CTFbsGlyphDataStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTFbsGlyphDataStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	
	); // TRAPD

	if (leave != KErrNone)
		{
		ERR_PRINTF2(_L("Leave %d occurred during test"), leave);
		iStep->SetTestStepResult(EFail);
		}

	((CTFbsGlyphDataStep*)iStep)->RecordTestResultL();
	}


/**
@SYMTestCaseID		GRAPHICS-FBSERV-0624
@SYMTestPriority	High
@SYMTestType		UT
@SYMTestStatus		Implemented
@SYMPREQ			PREQ2678

@SYMTestCaseDesc
	Shows that RFbsGlyphMetricsArray::Get() and CFont::GetCharacterData() all 
	provide the same metrics for the same set of glyph codes when using a CFbsFont.
	Shows that RFbsGlyphDataIterator::Metrics() and CFont::GetCharacterData()
	provide the same metrics for the same set of glyph codes.

@SYMTestActions
	i. Call RFbsGlyphMetricsArray::Get() for a set of glyph codes with 1 glyph code per call.
	ii. Call RFbsGlyphMetricsArray::Get() for a set of glyph codes all in 1 call.
	iii. Call RFbsGlyphDataIterator::Open() for a set of glyph codes.
	iv. Call CFont::GetCharacterData() for the same set of glyph codes.
	v. Compare the metrics for each glyph code from all calls.

@SYMTestExpectedResults
	For each glyph code, metrics received from RFbsGlyphMetricsArray::Get() and
	CFont::GetCharacterData() and RFbsGlyphDataIterator are all the same.
*/
void CTFbsGlyphData::TestConsistencyWithGetCharacterData()
	{
	INFO_PRINTF1(_L("Test RFbsGlyphMetricsArray::Get() with GetCharacterData()"));
	
	__UHEAP_MARK;

	RFbsGlyphDataIterator iter;
	RFbsGlyphMetricsArray glyphMetricsArray;
	RFbsGlyphMetricsArray glyphMetricsArraySingle;
	
	TInt numMismatches = 0;
	TOpenFontCharMetrics charMetrics;
	TSize bitmapSize;
	const TUint8* bitmapData = NULL;
	
	// Retrieve list of metrics for all glyph codes in one call
	TInt err = glyphMetricsArray.Get(*iFont, iGlyphCodesLatin, KNumGlyphCodesLatin);
	TESTNOERROR(err);
	if (err == KErrNone)
		{
		TEST(KNumGlyphCodesLatin == glyphMetricsArray.Count());
		
		TInt index = 0;
		TInt iterErr = iter.Open(*iFont, iGlyphCodesLatin, KNumGlyphCodesLatin);
		TESTNOERROR(iterErr);
		for (; iterErr == KErrNone; iterErr = iter.Next(), index++)
			{
			iFont->GetCharacterData(iGlyphCodesLatin[index] | KGlyphCodeFlag, charMetrics, bitmapData, bitmapSize);
			
			// Retrieve the metrics for each glyph code, one at a time
			TESTNOERROR(err = glyphMetricsArraySingle.Get(*iFont, &iGlyphCodesLatin[index], 1));
			if (KErrNone == err)
				{
				// Compare GetCharacterData() metrics with single RFbsGlyphMetricsArray.
				TUint32 comparison1 = CompareMetrics(charMetrics, glyphMetricsArraySingle[0]); 
				// Compare GetCharacterData() metrics with large RFbsGlyphMetricsArray.
				TUint32 comparison2 = CompareMetrics(charMetrics, glyphMetricsArray[index]);
				// Compare GetCharacterData() metrics with RFbsGlyphDataIterator.
				TUint32 comparison3 = CompareMetrics(charMetrics, iter.Metrics());
				if (comparison1 != 0 || comparison2 != 0 || comparison3 != 0)
					{
					ERR_PRINTF5(_L("Glyphcode %i : Metrics mismatch: %d/%d/%d"), iGlyphCodesLatin[index], comparison1, comparison2, comparison3);
					++numMismatches;
					}
				}
			}
			iter.Close();
			glyphMetricsArray.Close();
			glyphMetricsArraySingle.Close();
			TESTE(iterErr == KErrNotFound, iterErr);
			TEST(numMismatches == 0);
			TEST(index == KNumGlyphCodesLatin);
		}		
	
	__UHEAP_MARKEND;
	}

/**
@return A series of success/fail booleans as a bitmask. A return value of zero
	indicates all tests passed, a result of 1 indicates the first test case failed, 
	a return of 3 indicates the first and second test failed, and so on.
*/
TUint32 CTFbsGlyphData::CompareMetrics(const TOpenFontCharMetrics& aMetrics1, const TOpenFontCharMetrics& aMetrics2)
	{
	TUint32 result = 0;
	result |= (aMetrics1.Width() == aMetrics2.Width()) ? 0 : (1 << 0);
	result |= (aMetrics1.Height() == aMetrics2.Height()) ? 0 : (1 << 1);
	result |= (aMetrics1.HorizBearingX() == aMetrics2.HorizBearingX()) ? 0 : (1 << 2);
	result |= (aMetrics1.HorizBearingY() == aMetrics2.HorizBearingY()) ? 0 : (1 << 3);
	result |= (aMetrics1.HorizAdvance() == aMetrics2.HorizAdvance()) ? 0 : (1 << 4);
	result |= (aMetrics1.VertBearingX() == aMetrics2.VertBearingX()) ? 0 : (1 << 5);
	result |= (aMetrics1.VertBearingY() == aMetrics2.VertBearingY()) ? 0 : (1 << 6);
	result |= (aMetrics1.VertAdvance() == aMetrics2.VertAdvance()) ? 0 : (1 << 7);
	TRect rect1;
	aMetrics1.GetHorizBounds(rect1);
	TRect rect2;
	aMetrics2.GetHorizBounds(rect2);
	result |= (rect1 == rect2) ? 0 : (1 << 8);
	aMetrics1.GetVertBounds(rect1);
	aMetrics2.GetVertBounds(rect2);
	result |= (rect1 == rect2) ? 0 : (1 << 9);
	return result;
	}


/**
@SYMTestCaseID		GRAPHICS-FBSERV-0625
@SYMTestPriority	High
@SYMTestType		UT
@SYMTestStatus		Implemented
@SYMPREQ			PREQ2678

@SYMTestCaseDesc
	Shows that RFbsGlyphMetricsArray::Get(), and CFont::GetCharacterData() show the same 
	behaviour when asked for metrics for an invalid glyph code when using a	CFbsFont. 
	An invalid glyph code is one for which there is no character equivalent, such as 
	0.

@SYMTestActions
	i. Call CFont::GetCharacterData() for an invalid glyph code.
	ii. Call RFbsGlyphMetricsArray::Get() for the same invalid glyph code, and either 
		compare the metrics if i. was successful, or check an error code was returned
	
@SYMTestExpectedResults
	If GetCharacterData() is successful, the metrics received from
	RFbsGlyphMetricsArray::Get() and CFont::GetCharacterData()	are the same, otherwise
	RFbsGlyphMetricsArray::Get() should return an error code.
*/

void CTFbsGlyphData::TestInvalidGlyphCode()
	{
	INFO_PRINTF1(_L("Test behaviour of RFbsGlyphMetricsArray::Get() with invalid glyph code is consistent with GetCharacterData"));
	
	__UHEAP_MARK;
	TInt arrayErr = KErrNone;
	RFbsGlyphMetricsArray glyphMetricsArray;
	TOpenFontCharMetrics charMetrics;
	TSize bitmapSize;
	const TUint8* bitmapData = NULL;
	
	CFont::TCharacterDataAvailability availability = iFont->GetCharacterData(KDejaVuInvalidGlyphCode | KGlyphCodeFlag, charMetrics, bitmapData, bitmapSize);
	if (availability == CFont::ENoCharacterData)
		{
		// Some rasterizers fail to return any data for KDejaVuInvalidGlyphCode, therefore
		// rather than compare metrics, make sure RFbsGlyphDataIterator returns an error code.
		WARN_PRINTF1(_L("Rasterizer failed to return data for invalid glyph code; not comparing glyph metrics"));
		arrayErr = glyphMetricsArray.Get(*iFont, &KDejaVuInvalidGlyphCode, 1);
		TESTE(arrayErr != KErrNone, arrayErr);
		}
	else
		{
		TESTNOERROR(arrayErr = glyphMetricsArray.Get(*iFont, &KDejaVuInvalidGlyphCode, 1));
		if (KErrNone == arrayErr)
			{
			iFont->GetCharacterData(KDejaVuInvalidGlyphCode | KGlyphCodeFlag, charMetrics, bitmapData, bitmapSize);
			TUint comparisonResult = CompareMetrics(charMetrics, glyphMetricsArray[0]);
			TESTNOERROR( comparisonResult );
			}
		}
	glyphMetricsArray.Close();

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID		GRAPHICS-FBSERV-0626
@SYMTestPriority	High
@SYMTestType		UT
@SYMTestStatus		Implemented
@SYMPREQ			PREQ2678

@SYMTestCaseDesc
	Shows that RFbsGlyphMetricsArray::Get() returns with the correct error code when passed
	various combinations of parameters, and preserves the state of the array.

@SYMTestActions
	Populate the array with a single metrics entry.
	Call RFbsGlyphMetricsArray::Get with the following parameter combinations:
		1. A negative count
		2. A positive count and null glyph code array pointer
		3. A zero count and non-null glyph code array pointer
		4. A zero count and null glyph code array pointer

@SYMTestExpectedResults
	The following return codes are expected for each call:
		1. KErrArgument
		2. KErrArgument
		3. KErrArgument
		4. KErrArgument	
	For each case the glyph metrics array remains unchanged.
*/
void CTFbsGlyphData::TestGlyphMetricsArrayParameters()
	{
	INFO_PRINTF1(_L("Test the return values of GetGlyphMetrics with different parameters"));
	__UHEAP_MARK;
	TInt arrayErr = KErrNone;
	TOpenFontCharMetrics dummyMetrics;
	
	RFbsGlyphMetricsArray glyphMetricsArray;
	arrayErr = glyphMetricsArray.Get(*iFont, iGlyphCodesLatin, 1);
	TESTNOERROR(arrayErr);
	TEST(1 == glyphMetricsArray.Count());
	
	// 1. Negative Count
	arrayErr = glyphMetricsArray.Get(*iFont, iGlyphCodesLatin, -1);
	TESTE(KErrArgument == arrayErr, arrayErr);
	TEST(1 == glyphMetricsArray.Count());
	
	// 2. Positive Count and NULL Array Pointer
	arrayErr = glyphMetricsArray.Get(*iFont, NULL, 1);
	TESTE(KErrArgument == arrayErr, arrayErr);
	TEST(1 == glyphMetricsArray.Count());

	// 3. Zero Count & Valid Array Pointer
	arrayErr = glyphMetricsArray.Get(*iFont, iGlyphCodesLatin, 0);
	TESTE(KErrArgument == arrayErr, arrayErr);

	// 4. Zero Count & NULL Array Pointer
	arrayErr = glyphMetricsArray.Get(*iFont, NULL, 0);
	TESTE(KErrArgument == arrayErr, arrayErr);

	glyphMetricsArray.Close();
	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID		GRAPHICS-FBSERV-0627
@SYMTestPriority	High
@SYMTestType		UT
@SYMTestStatus		Implemented
@SYMPREQ			PREQ2678

@SYMTestCaseDesc
	Shows that reusing an RFbsGlyphMetricsArray works correctly.
	In particular when the array is reused and filled with fewer entries
	and when the array is reused and filled with more entries than previously.
	It also shows that when re-using an array that has been populated, memory 
	is not de-allocated if the new array of glyphs is smaller.

@SYMTestActions
	i. Call RFbsGlyphMetricsArray::Get() for a set of 10 glyph codes.
	ii. Check that the RFbsGlyphMetricsArray has 10 entries.
	iii. Find the size of the heap-cell allocated to the array.
	iii. Call RFbsGlyphMetricsArray::Get() for a set of 5 glyph codes.
	iv. Check that the RFbsGlyphMetricsArray has 5 entries.
	v. Call RFbsGlyphMetricsArray::Get() for a set of 20 glyph codes.
	vi. Check that the RFbsGlyphMetricsArray has 20 entries.
	vii. Call RFbsGlyphMetricsArray::Get() for a set of 0 glyph codes.
	viii. Check that the RFbsGlyphMetricsArray has 0 entries.
	ix. Call RFbsGlyphMetricsArray::Get() for 1 glyph code.
	x. Check that the RFbsGlyphMetricsArray has 1 entries.
	xi. Close the RFbsGlyphMetricsArray.
	xii. Check that the RFbsGlyphMetricsArray has 0 entries.
	During the test check that the size of the heap cell allocated to the array
	does not shrink.

@SYMTestExpectedResults
	After each call to RFbsGlyphMetricsArray::Get(), the array has the expected number of entries.
*/
void CTFbsGlyphData::TestGlyphMetricsArrayReuse()
	{
	INFO_PRINTF1(_L("Test reuse of array with RFbsGlyphMetricsArray"));
	__UHEAP_MARK;
	
	RFbsGlyphMetricsArray glyphMetricsArray;

	// Retrieve list of metrics for 10 glyph codes
	TESTNOERROR(glyphMetricsArray.Get(*iFont, iGlyphCodesLatin, 10));
	TEST(10 == glyphMetricsArray.Count());
	
	// Find the size of the heap cell allocated for the array.
	TInt arrayHeapCellSize = User::Heap().AllocLen(&glyphMetricsArray[0]);
	
	// Retrieve list of metrics for 5 glyph codes.
	// To ensure that different metrics are returned, use different glyph codes
	TESTNOERROR(glyphMetricsArray.Get(*iFont, &iGlyphCodesLatin[10], 5));
	TEST(5 == glyphMetricsArray.Count());
	// Check that memory has not been de-allocated for a smaller array.
	TEST(User::Heap().AllocLen(&glyphMetricsArray[0]) == arrayHeapCellSize);

	// Retrieve list of metrics for 20 glyph codes.
	// To ensure that different metrics are returned, use different glyph codes
	TESTNOERROR(glyphMetricsArray.Get(*iFont, &iGlyphCodesLatin[15], 20));
	TEST(20 == glyphMetricsArray.Count());
	arrayHeapCellSize = User::Heap().AllocLen(&glyphMetricsArray[0]);
		
	// Retrieve list of metrics for 0 glyph codes.
	TEST(KErrArgument == glyphMetricsArray.Get(*iFont, &iGlyphCodesLatin[35], 0));
	// We can't check whether memory has been de-allocated as glyphMetricsArray[0]
	// is null, therefore dereferencing it causes a panic.

	// Retrieve list of metrics for 1 glyph code.
	// To ensure that different metrics are returned, use different glyph code
	TESTNOERROR(glyphMetricsArray.Get(*iFont, &iGlyphCodesLatin[35], 1));
	TEST(1 == glyphMetricsArray.Count());
	TEST(User::Heap().AllocLen(&glyphMetricsArray[0]) == arrayHeapCellSize);
	
	// Test that after closing a non-empty array, the array has 0 size.
	glyphMetricsArray.Close();
	TEST(0 == glyphMetricsArray.Count());
	
	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID		GRAPHICS-FBSERV-0628
@SYMTestPriority	High
@SYMTestType		UT
@SYMTestStatus		Implemented
@SYMPREQ			PREQ2678

@SYMTestCaseDesc
	Validates the behaviour of RFbsGlyphDataIterator::Close() in the following use cases:
		1. When called on an iterator instance which has not been opened, has no effect.
		2. When called on an open iterator closes the iterator 

@SYMTestActions
	Use case 1:
		i. Create an RFbsGlyphDataIterator instance but do not open.
		ii. Call RFbsGlyphDataIterator::Close().
		
	Use case 2:
		i. Create an RFbsGlyphDataIterator instance and call RFbsGlyphDataIterator::Open().
		ii. Call RFbsGlyphDataIterator::Next() to prove the iterator is open.
		iii. Call RFbsGlyphDataIterator::Close().
		iv. Check that RFbsGlyphDataIterator::IsOpen() returns false.
	
@SYMTestExpectedResults
	Each call to RFbsGlyphDataIterator::IsOpen() returns the expected value.
*/
void CTFbsGlyphData::TestGlyphDataIteratorClose()
	{
	INFO_PRINTF1(_L("Test closing an RFbsGlyphDataIterator"));
	__UHEAP_MARK;

	// Use case 1
	RFbsGlyphDataIterator iter1;
	iter1.Close();

	// Use case 2
	RFbsGlyphDataIterator iter2;
	TESTNOERROR(iter2.Open(*iFont, iGlyphCodesLatin, 1));
	TInt iterErr = iter2.Next();
	TESTE(KErrNotFound == iterErr, iterErr);
	iter2.Close();
	
	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID		GRAPHICS-FBSERV-0629
@SYMTestPriority	High
@SYMTestType		UT
@SYMTestStatus		Implemented
@SYMPREQ			PREQ2678

@SYMTestCaseDesc
	Show that the sequence of iterations when calling RFbsGlyphDataIterator::Next()
	matches the order of the array of glyph codes.

@SYMTestActions
	i. Create an RFbsGlyphDataIterator instance and call RFbsGlyphDataIterator::Open()
		with an array of different glyph codes.
	ii. Iterate through all the glyph data.
		For each iteration check that the glyph code returned from 
		RFbsGlyphDataIterator::GlyphCode() matches the corresponding glyph code
		passed into Open().

@SYMTestExpectedResults
	Each comparison of glyph code should match.
*/
void CTFbsGlyphData::TestGlyphDataIteratorSequence()
	{
	INFO_PRINTF1(_L("Test the iterator sequence of RFbsGlyphDataIterator"));
	__UHEAP_MARK;
	
	TBool matches = ETrue;
	TInt index = 0;

	RFbsGlyphDataIterator iter;
	TInt iterErr = iter.Open(*iFont, iGlyphCodesLatin, KNumGlyphCodesLatin);
	TESTNOERROR(iterErr);
	for (; index < KNumGlyphCodesLatin && matches && (iterErr == KErrNone); iterErr = iter.Next(), index++)
		{
		if (iter.GlyphCode() != iGlyphCodesLatin[index])
			{
			ERR_PRINTF4(_L("Failed at iteration %d: wanted %d, got %d"), index, iGlyphCodesLatin[index], iter.GlyphCode());
			matches = EFalse;
			}
		}
	iter.Close();
	TESTE(iterErr == KErrNotFound, iterErr);
	TEST(matches);
	TEST(index == KNumGlyphCodesLatin);
	iter.Close();

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID		GRAPHICS-FBSERV-0632
@SYMTestPriority	High
@SYMTestType		UT
@SYMTestStatus		Implemented
@SYMPREQ			PREQ2678

@SYMTestCaseDesc
	Ensure it is possible to reuse a closed iterator on another CFbsFont.
	
@SYMTestActions
	i. Open an RFbsGlyphDataIterator with sample data.
	ii. Iterate through until the end of the iterator has been reached by calling 
		Next() on the final element.
	iii. Re-open the same RFbsGlyphDataIterator with sample data on a different CFbsFont.
	iv. Iterate through a second time until the end has been reached by calling Next()
		on the final element.
	v. Close the iterator.
	vi. During both iterations the bitmap data returned and metrics are compared with
		the equivalent from GetCharacterData().

@SYMTestExpectedResults
	The iterator should be opened successfully for both fonts and the data returned
	should match the data from GetCharacterData().
*/
void CTFbsGlyphData::TestGlyphDataIteratorMultipleUsesOnMultipleFonts()
	{
	INFO_PRINTF1(_L("Reuse a closed iterator on a second CFbsFont"));
	__UHEAP_MARK;
	
	const TUint8* bitmapData;
	TSize bitmapSize;
	TOpenFontCharMetrics charMetrics;
	RFbsGlyphDataIterator iter;
	
	// Array of fonts to iterate through.
	CFbsFont* font[2] = {iFont, iFont2};
	
	for (TInt fontId = 0; fontId < 2; fontId++)
		{
		// On the first iteration, open and use a font until all glyphs have been iterated through.
		// On second iteration, use the same iterator on a different font and repeat.
        CFbsFont* currentFont = font[fontId];

		//Open the iterator on the first font and compare the returned bitmaps against GetCharacterData
		TInt iterErr = iter.Open(*currentFont, iGlyphCodesLatin, KNumGlyphCodesLatin);
		TESTNOERROR(iterErr);
		TFontSpec fontSpec = currentFont->FontSpecInTwips();
		
		TInt index = 0;
		for (; (iterErr == KErrNone) && (index < KNumGlyphCodesLatin); iterErr = iter.Next(), ++index)
			{
			currentFont->GetCharacterData(iGlyphCodesLatin[index] | KGlyphCodeFlag, charMetrics, bitmapData, bitmapSize);
			
			TESTNOERROR(CompareMetrics(charMetrics, iter.Metrics()));
			if (bitmapSize == TSize(0, 0))
				{
				TEST(bitmapSize == iter.Rect().Size());
				}
			else
				{
				// Compare images.
				TBool match = EFalse;
				RSgImage characterDataImage;
				TInt err = CreateSgImageFromCharacterData(bitmapData, bitmapSize, fontSpec.iFontStyle.BitmapType(), characterDataImage);
				if (err == KErrNone)
					{
					err = CompareSgImages(iEGL, iter.Image(), iter.Rect(), characterDataImage, TRect(bitmapSize), match);
					}
				characterDataImage.Close();
				if (err != KErrNone)
					{
					TESTNOERROR(err);
					break;
					}
				TEST(match);
				}		
			}
		iter.Close();
		TESTE(iterErr == KErrNotFound, iterErr);
		TEST(index == KNumGlyphCodesLatin);
		}
	
	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID		GRAPHICS-FBSERV-0633
@SYMTestPriority	High
@SYMTestType		UT
@SYMTestStatus		Implemented
@SYMPREQ			PREQ2678

@SYMTestCaseDesc
	Check that for various Latin fonts, the images of the glyphs stored on the 
	RSgImage matches those provided by GetCharacterData().

@SYMTestActions
	Create a selection of fonts, using various typefaces, sizes and bitmap types.
	For each font:
	i. Open the RFbsGlyphDataIterator and iterate each glyph.
	ii. For each glyph, call GetCharacterData() with the expected glyph code.
	iii. Convert the character data to an RSgImage.
	iv. Perform a comparison between the character RSgImage and the iterator 
		image. 
	v. After all iterations, close the iterator and check all expected glyphs
		were iterated through.

@SYMTestExpectedResults
	All glyph images should match.
*/
void CTFbsGlyphData::TestGlyphDataIteratorImageValidity()
	{
	INFO_PRINTF1(_L("Test the glyph images of the iterator match GetCharacterData()"));
	__UHEAP_MARK;
	
	const TInt KNumFonts = 20;
	
	// Create a new typeface store for this test so that heap checking will not
	// be affected by cached CFbsFonts.
	CFbsTypefaceStore* typefaceStore = NULL;
	TRAPD(err, typefaceStore = CFbsTypefaceStore::NewL(NULL));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Failed to construct typeface store. Test aborted."));
		__UHEAP_RESET;
		iStep->SetTestStepResult(EFail);
		return;
		}
	
	for (TInt font = 0; font < KNumFonts; ++font)
		{
		// Use either a pre-created bitmap-font TFontSpec, or generate a Deja-vu one.
		TFontSpec fontSpec = GenerateDejaVuFontSpec(font);
		CFbsFont* latinFont = NULL;
		TESTNOERROR(typefaceStore->GetNearestFontToDesignHeightInPixels((CFont*&)latinFont, fontSpec));	
		
		fontSpec = latinFont->FontSpecInTwips();
		InfoPrintFontSpec(*latinFont);
				
		RFbsGlyphDataIterator iter;
		TInt iterErr = iter.Open(*latinFont, iGlyphCodesLatin, KNumGlyphCodesLatin);
		TESTNOERROR(iterErr);

		err = KErrNone;
		TInt index = 0;
		TInt numMismatches = 0;
		// For each iteration, get the character data of the expected glyph.
		// Create RSgImage from character data, and compare iter image with constructed image.
		for (; (iterErr == KErrNone) && (err == KErrNone) && (index < KNumGlyphCodesLatin); (iterErr = iter.Next()), ++index)
			{
			TBool glyphMatches = ETrue;
			const RSgImage& iteratorImage = iter.Image();

			const TUint8* bitmapData = NULL;
			TSize bitmapSize;
			TOpenFontCharMetrics metrics;
			TInt characterDataAvailability = latinFont->GetCharacterData(iGlyphCodesLatin[index] | KGlyphCodeFlag, metrics, bitmapData, bitmapSize);
			if (bitmapSize == TSize(0, 0))
				{
				glyphMatches = (bitmapSize == iter.Rect().Size());
				}
			else
				{
				RSgImage characterDataImage;
				TESTNOERROR(CreateSgImageFromCharacterData(bitmapData, bitmapSize, fontSpec.iFontStyle.BitmapType(), characterDataImage));
				err = CompareSgImages(iEGL, iteratorImage, iter.Rect(), characterDataImage, TRect(bitmapSize), glyphMatches);
				characterDataImage.Close();
				}
			if (err == KErrNone && !glyphMatches)
				{
				ERR_PRINTF2(_L("Glyphcode %i : Image mismatch"), iGlyphCodesLatin[index]);
				++numMismatches;
				}
			}
		iter.Close();
		TESTNOERROR(err);
		TESTE(iterErr == KErrNotFound, iterErr);
		TEST(index == KNumGlyphCodesLatin);	
		TEST(numMismatches == 0);
		typefaceStore->ReleaseFont(latinFont);
		}
	delete typefaceStore;
	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID      GRAPHICS-FBSERV-0634
@SYMTestPriority    High
@SYMTestType        UT
@SYMTestStatus      Implemented
@SYMPREQ            PREQ2678

@SYMTestCaseDesc
	To ensure that if the glyph image iterator has a current invalid 
	character code, the SgImage returned by the iterator will match 
	to the image obtained from the GetCharacterData() function
@SYMTestActions
	i. Retrieve bitmap data and metrics by using GetCharacterData().
	ii. Open a glyph data iterator passing an invalid character code.
	iii. If i. was unsuccessful, check that opening the iterator returned
		an error code and skip to ix.
	iv. Create SgImage from bitmap data.
	v. Get SgImage from the glyph data iterator.
	vi. Compare SgImages obtained on iv and v steps.
	vii. Get font metrics from the glyph data iterator.
	viii. Compare metrics obtained on i and vii steps.
	vii. Close the iterator.

@SYMTestExpectedResults
	If the request to get the character data failed, the return value of 
	RFbsGlyphDataIterator::Open() must not be KErrNone.
	Otherwise, images obtained from the iterator and GetCharacterData() should
	match.
*/
void CTFbsGlyphData::TestGlyphDataIteratorOpenInvalidCode()
	{
	INFO_PRINTF1(_L("Ensure that the image returned by the iterator will \
match to the image obtained from GetCharacterData() if character code is invalid"));
	__UHEAP_MARK;

	const TUint8* bitmapData = NULL;
	TSize bitmapSize;
	TOpenFontCharMetrics metrics;
	const TFontSpec fontSpec = iFont->FontSpecInTwips();
	CFont::TCharacterDataAvailability availability = iFont->GetCharacterData(KDejaVuInvalidGlyphCode | KGlyphCodeFlag, metrics, bitmapData, bitmapSize);

	RFbsGlyphDataIterator iter;
	TInt err = iter.Open(*iFont, &KDejaVuInvalidGlyphCode, 1);
	if (availability == CFont::ENoCharacterData)
		{
		// Some rasterizers fail to return any data for KDejaVuInvalidGlyphCode, therefore
		// rather than compare image contents, make sure RFbsGlyphDataIterator returns an error code.
		WARN_PRINTF1(_L("Rasterizer failed to return data for invalid glyph code; not comparing image contents"));
		TESTE(err != KErrNone, err);
		}
	else
		{
		TESTNOERROR(err);
		if (err == KErrNone)
			{
			TBool glyphMatches = EFalse;
			if (bitmapSize == TSize(0, 0))
				{
				glyphMatches = (bitmapSize == iter.Rect().Size());
				}
			else
				{
				RSgImage characterDataImage;
				TESTNOERROR(CreateSgImageFromCharacterData(bitmapData, bitmapSize, fontSpec.iFontStyle.BitmapType(), characterDataImage));
				TESTNOERROR(CompareSgImages(iEGL, iter.Image(), iter.Rect(), characterDataImage, TRect(bitmapSize), glyphMatches));
				characterDataImage.Close();
				}
			TESTNOERROR(CompareMetrics(metrics, iter.Metrics()));
			TEST(glyphMatches);
			}
		}
	iter.Close();

	__UHEAP_MARKEND;
	}


/**
@SYMTestCaseID      GRAPHICS-FBSERV-0636
@SYMTestPriority    High
@SYMTestType        UT
@SYMTestStatus      Implemented
@SYMPREQ            PREQ2678

@SYMTestCaseDesc
	To ensure that opening the glyph data iterator which has already been opened with the same font
	has no effect on the state of the iterator.
@SYMTestActions
	i. Open glyph data iterator on 2 glyph codes.
	ii. Try to open the glyph data iterator again on the same font.
	iii. Call RFbsGlyphDataIterator::Next() on the iterator and check error code, making the last
		glyph code the current iteration.
	iv. Call RFbsGlyphDataIterator::Next() again.
@SYMTestExpectedResults
	The second attempt to open the glyph data iterator will result an error with code KErrInUse.
	The last two calls to RFbsGlyphDataIterator::Next() should return KErrNone and KErrNotFound
	respectively, showing the iterator was not modified when the call to Open() failed.
*/
void CTFbsGlyphData::TestGlyphDataIteratorOpenTwice()
	{
	INFO_PRINTF1(_L("Ensure that opening the glyph data iterator which has already been opened with the same font has no effect"));
	__UHEAP_MARK;

	RFbsGlyphDataIterator iter;
	TInt iterErr = iter.Open(*iFont, iGlyphCodesLatin, 2);
	TESTNOERROR(iterErr);
	
	iterErr = iter.Open(*iFont, iGlyphCodesLatin, KNumGlyphCodesLatin);
	TESTE(iterErr == KErrInUse, iterErr);
	iterErr = iter.Next();
	TESTNOERROR(iterErr);
	iterErr = iter.Next();
	TESTE(iterErr == KErrNotFound, iterErr);
	iter.Close();

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID      GRAPHICS-FBSERV-0637
@SYMTestPriority    High
@SYMTestType        UT
@SYMTestStatus      Implemented
@SYMPREQ            PREQ2678

@SYMTestCaseDesc
	To ensure that opening the glyph data iterator which has already been opened with different font
	has no effect on the state of the iterator.
@SYMTestActions
	i. Open glyph data iterator on an 2 glyph codes
	ii. Try to open the glyph data iterator again with a different font.
	iii. Call RFbsGlyphDataIterator::Next() on the iterator and check error code, making the last
		glyph code the current iteration.
	iv. Call RFbsGlyphDataIterator::Next() again.
@SYMTestExpectedResults
	The second attempt to open the glyph data iterator will result an error with code KErrInUse.
	The Next() call after this should return KErrNone, signifying the iterator is still open.
	The last Next() call should return KErrNotFound, signifying the iterator has iterated 
	through the two original glyph codes.
*/
void CTFbsGlyphData::TestGlyphDataIteratorOpenTwiceWithDifferentFonts()
	{
	INFO_PRINTF1(_L("Ensure that opening the glyph data iterator which has already been opened with different font has no effect"));
	__UHEAP_MARK;

	RFbsGlyphDataIterator iter;
	TInt iterErr = iter.Open(*iFont, iGlyphCodesLatin, 2);
	TESTNOERROR(iterErr);

	iterErr = iter.Open(*iFont2, iGlyphCodesLatin, 2);
	TESTE(iterErr == KErrInUse, iterErr);
	iterErr = iter.Next();
	TESTNOERROR(iterErr);
	iterErr = iter.Next();
	TESTE(iterErr == KErrNotFound, iterErr);
	iter.Close();

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID      GRAPHICS-FBSERV-0638
@SYMTestPriority    High
@SYMTestType        UT
@SYMTestStatus      Implemented
@SYMPREQ            PREQ2678

@SYMTestCaseDesc
	To ensure that opening of glyph data iterator with the font greater than 
	2048 by 2048 will not be supported 
@SYMTestActions
	i. Create font with the height greater than 2048
	ii. Try to open the glyph data iterator with the font created on previous step
	iii Release the font
@SYMTestExpectedResults
	Must fail with error code KErrTooBig
*/
void CTFbsGlyphData::TestGlyphDataIteratorOpenTooBigFont()
	{
	INFO_PRINTF1(_L("To ensure that opening of glyph data iterator with the font greater than 2048X2048 will not be supported"));
	__UHEAP_MARK;

	CFbsFont* bigFont;
	const TInt maxHeight = 2048;
	const TInt maxHeightLimit = maxHeight + 20; //max size after we stop trying to create the font
	// the loop below will guarantee that if the font with the size greater than 2048 is available it will be created
	for(TInt height = maxHeight + 1; height < maxHeightLimit; height++)
		{
		TESTNOERROR(iTs->GetNearestFontToDesignHeightInPixels((CFont*&)bigFont, TFontSpec(KTypefaceName, height)));
		TInt realHeight = bigFont->FontMaxHeight();
		if(realHeight > maxHeight)
			{
			break;
			}
		iTs->ReleaseFont(bigFont);
		bigFont = NULL;
		}

	if (bigFont)
		{
		RFbsGlyphDataIterator iter;
		TInt iterErr = iter.Open(*bigFont, iGlyphCodesLatin, KNumGlyphCodesLatin);
		TESTE(iterErr == KErrTooBig, iterErr);
		iTs->ReleaseFont(bigFont); 
		}
	else
		{
		//It is legitimate to fail to create the font, as there are no requirements for the rasterizer here to support such big font. 
		//In this case we will skip the test.
		WARN_PRINTF1(_L("Failed to create font with height greater than 2048"));
		}

	__UHEAP_MARKEND;
	}




/**
@SYMTestCaseID      GRAPHICS-FBSERV-0640
@SYMTestPriority    High
@SYMTestType        UT
@SYMTestStatus      Implemented
@SYMPREQ            PREQ2678

@SYMTestCaseDesc
	To ensure that the glyph data iterator processes wrong arguments correctly
@SYMTestActions
	i. Try to open the glyph data iterator with the negative count passed in
	ii. Try to open the glyph data iterator with the positive count and NULL 
	glyph code array pointer passed in
	iii. Try to open the glyph data iterator with a valid glyph code array and 
	count equal to zero
@SYMTestExpectedResults
	At all steps the returned value is set to KErrArgument.
*/
void CTFbsGlyphData::TestGlyphDataIteratorOpenWithWrongArgument()
	{
	INFO_PRINTF1(_L("To ensure that the glyph data iterator processes wrong arguments correctly"));
	__UHEAP_MARK;

	RFbsGlyphDataIterator iter;
	TInt iterErr = iter.Open(*iFont, iGlyphCodesLatin, -1);
	TESTE(iterErr == KErrArgument, iterErr);
	
	iterErr = iter.Open(*iFont, NULL, 1);
	TESTE(iterErr == KErrArgument, iterErr);
	
	iterErr = iter.Open(*iFont, iGlyphCodesLatin, 0);
	TESTE(iterErr == KErrArgument, iterErr);
	
	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID      GRAPHICS-FBSERV-0641
@SYMTestPriority    High
@SYMTestType        UT
@SYMTestStatus      Implemented
@SYMPREQ            PREQ2678

@SYMTestCaseDesc
	To ensure that all allocated RSgImages were released after the
	glyph data iterator has been opened and closed multiple times.

@SYMTestActions
	i. Retrieve MSgDriver_Test interface from the SgDriver
	ii. Mark alloc start and obtain resorce count from the interface
	iii. Iterate through glyph data by calling RFbsGlyphDataIterator::Next() 
	iv. Retrieve SgImage from the glyph data iterator instance
	v. Repeate steps iii and iv multiple times
	vi. Release font
	vii.  Mark alloc end and obtain resorce count from the interface

@SYMTestExpectedResults
	Resorce count at the end matches resorce count at the beginning. 
*/
void CTFbsGlyphData::TestGlyphDataIteratorImageMemoryLeak()
	{
	__UHEAP_MARK;
	
	MSgDriver_Test* sgDriverTestInterface = NULL; 
	TInt err = iSgDriver.GetInterface(sgDriverTestInterface);
	if(err != KErrNone)
		{
		__UHEAP_MARKEND;
		WARN_PRINTF2(_L("Failed to obtain MSgDriver_Test interface with error code: %d, the test will be skipped"), err);
		return;
		}

	TEST(sgDriverTestInterface != NULL);
	sgDriverTestInterface->AllocMarkStart();

	MSgDriver_Profiling* sgDriverProfilInterface = NULL;
	err = iSgDriver.GetInterface(sgDriverProfilInterface);
	if(err != KErrNone)
		{
		sgDriverTestInterface->AllocMarkEnd(0);
		__UHEAP_MARKEND;
		WARN_PRINTF2(_L("Failed to obtain MSgDriver_Profiling interface with error code: %d, the test will be skipped"), err);
		return;
		}
	const TInt resCount = sgDriverProfilInterface->LocalResourceCount();

	CFbsFont* font = NULL;
	err = iTs->GetNearestFontToDesignHeightInPixels((CFont*&)font, TFontSpec(KTypefaceName, 15));
	TESTNOERROR(err);
	if(err != KErrNone)
		{
		__UHEAP_MARKEND;
		return;
		}

	for (TInt ii = 0; ii < 10; ii++)
		{
		TInt index = 0;
		RFbsGlyphDataIterator iter;
		TInt iterErr = iter.Open(*font, iGlyphCodesLatin, KNumGlyphCodesLatin);
		TESTNOERROR(iterErr);
		for (; (iterErr == KErrNone) && (index < KNumGlyphCodesLatin); iterErr = iter.Next(), ++index)
			{
			const RSgImage& image = iter.Image();
			}
		iter.Close();
		TEST(index == KNumGlyphCodesLatin);
		TESTE(iterErr == KErrNotFound, iterErr);
		}
	iTs->ReleaseFont(font);
	const TInt resCountEnd = sgDriverProfilInterface->LocalResourceCount();
	TEST(resCountEnd == resCount);
	sgDriverTestInterface->AllocMarkEnd(0);

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID      GRAPHICS-FBSERV-0659
@SYMTestPriority    Med
@SYMTestType        UT
@SYMTestStatus      Implemented
@SYMPREQ            PREQ2678

@SYMTestCaseDesc
	Uses the RFbsGlyphDataIterator to render a large amount of unique glyphs, at a very large 
	size, to ensure that if graphics memory runs out while the iterator is in use, eviction 
	takes place and does not corrupt the glyph images in any way.

@SYMTestActions
	i. Create a large CFbsFont from the typeface store (size 100+)
	ii. Simulate a low graphics-memory situation by creating enough RSgImages to fill the memory,
		releasing one image in order to allow some small amount for the test.
	iii. Open a RFbsGlyphDataIterator on the font, using a large array of unique glyph codes.
	iv. Iterate through the glyphs, comparing each returned SgImage against the system-memory
		representation of the glyph as returned by CFont::GetCharacterData().
	v. Check for errors and mismatches, and release all images created by ii.
	
@SYMTestExpectedResults
	At each iteration, each glyph should match in size and contents. 
*/
void CTFbsGlyphData::TestGlyphDataIteratorLargeFontStress()
	{
	INFO_PRINTF1(_L("Stress test using a RFbsGlyphDataIterator with a large font"));
#ifdef __WINS__
	// Cannot run test on emulator reliably - this is because on emulator
	// system-memory is used for RSgImages, so using up RSgImage memory may 
	// cause heap-allocation failures unrelated to the area being tested. 
	// This test is specifically testing the behaviour when running out of
	// RSgImage-based memory (i.e. graphics memory), but on emulator this 
	// will cause a failed allocation anywhere.
	INFO_PRINTF1(_L("Skipping test on emulator..."));
#else
	WARN_PRINTF1(_L("---Stress test TO BE REVISITED due to Broadcom defect ESLM-85LDV7 - TB10.1 Closing of RSgImage with duplicate handle used in same thread does not release GPU RAM"));
	TEST(EFalse);
/*	__UHEAP_MARK;

	const TInt KFontSize = 128;

	CFbsFont* font;
	TInt err = iTs->GetNearestFontToDesignHeightInPixels((CFont*&)font, TFontSpec(KMonoTypefaceName, KFontSize));
	TESTNOERROR(err);
	// Output the actual fontspec used in the test.
	InfoPrintFontSpec(*font);
	
	// Create 2 buffers for use in comparing SgImages so that we don't run out 
	// of system memory through allocating memory in the test
	TInt maxFontWidth = font->MaxCharWidthInPixels();
	TInt maxFontHeight = font->HeightInPixels();
	iTempBuf1 = (TUint8*) User::AllocZ(maxFontWidth * maxFontHeight);
	iTempBuf2 = (TUint8*) User::AllocZ(maxFontWidth * maxFontHeight);

	// In order for the image comparisons to have enough memory to perform, keep 
	// one large RSgImage which is created before the rest of the graphics memory 
	// is filled.  This image can then be closed before doing the image comparison 
	// and recreated after the image comparison to ensure that the graphics 
	// memory is full.  Without this image, the image comparison could fail with 
	// out of memory and the test would fail. 
	RSgImage tempImage;
	TESTNOERROR(tempImage.Create(TSgImageInfo(TSize(1000, 1000), ESgPixelFormatA_8, ESgUsageBitOpenVgImage)));

	TFontSpec actualFontSpec;
	actualFontSpec = font->FontSpecInTwips();
	
	// Create RSgImages from character data independently from using iterator.
	// These will be used for comparing with RSgImages retrieved from iterator.
	RArray <RSgImage> sgImageFromCharDataArray;
	TInt index = 0;
	for(; (index < KNumGlyphCodesLatin) && (err == KErrNone); ++index)
		{
		RSgImage characterDataSgImage;
		TInt err = KErrNone;
		const TUint8* bitmapData = NULL;
		TSize bitmapSize;
		TOpenFontCharMetrics metrics;
		font->GetCharacterData(iGlyphCodesLatin[index] | KGlyphCodeFlag, metrics, bitmapData, bitmapSize);

		if (bitmapSize != TSize(0, 0))
			{
			err = CreateSgImageFromCharacterData(bitmapData, bitmapSize, actualFontSpec.iFontStyle.BitmapType(), characterDataSgImage, iTempBuf1, iTempBuf2);
			}
		if (KErrNone == err)
			{
			err = sgImageFromCharDataArray.Append(characterDataSgImage);
			}
		}
	TESTNOERROR(err);
	TEST(index == KNumGlyphCodesLatin);

	// Simulate low OOGM situation by creating many RSgImages until out of memory.
	RArray <RSgImage> sgImageArray;
	if (err == KErrNone)
		{
		TESTNOERROR(NearlyFillGraphicsMemoryWithImages(TSize(256, 256), sgImageArray));
		}
	
	// Open Iterator on long string of data...
	RFbsGlyphDataIterator iter;
	TInt iterErr = KErrNone;
	if (err == KErrNone)
		{
		iterErr = iter.Open(*font, iGlyphCodesLatin, KNumGlyphCodesLatin);
		TESTNOERROR(iterErr);
		}

	// For each glyph, compare it to the system-memory version from GetCharacterData().
	TInt numMismatches = 0;
	for(index = 0; (iterErr == KErrNone) && (index < sgImageFromCharDataArray.Count()) && (err == KErrNone); iterErr = iter.Next(), ++index)
		{
		const TUint8* bitmapData = NULL;
		TSize bitmapSize;
		TOpenFontCharMetrics metrics;
		font->GetCharacterData(iter.GlyphCode() | KGlyphCodeFlag, metrics, bitmapData, bitmapSize);

		if (iter.Rect().Size() == TSize(0, 0))
			{
			numMismatches += (bitmapSize != TSize(0, 0)) ? 1 : 0;
			}
		else
			{
			// Free up memory so that the image compariso succeeds
			// Release all the images used to simulate OOGM.
			for (TInt i = sgImageArray.Count() - 1; i >= 0; --i)
				{
				sgImageArray[i].Close();
				sgImageArray.Remove(i);
				}
			
			TBool match = ETrue;
			err = CompareSgImages(iEGL, sgImageFromCharDataArray[index], TRect(bitmapSize), iTempBuf1, iter.Image(), iter.Rect(), iTempBuf2, match);
			if (err == KErrNone && !match)
				{
				++numMismatches;
				}
			TInt result = FillGraphicsMemoryWithImages(TSize(256, 256), sgImageArray);
			TESTE(result == KErrNoMemory || result == KErrNoGraphicsMemory, result);
			}
		}
	iter.Close();

	// Release all images created from character data.
	for (TInt i = sgImageFromCharDataArray.Count()-1; i >= 0; --i)
		{
		sgImageFromCharDataArray[i].Close();
		}
	sgImageFromCharDataArray.Close();

	// Release all the images used to simulate OOGM.
	for (TInt i = sgImageArray.Count() - 1; i >= 0; --i)
		{
		sgImageArray[i].Close();
		}
	sgImageArray.Close();
	tempImage.Close();
	iTs->ReleaseFont(font);
	User::Free(iTempBuf1);
	User::Free(iTempBuf2);
	iTempBuf1 = NULL;
	iTempBuf2 = NULL;

	// Log any errors only after memory is freed - this ensures there is enough
	// memory for the logger.
	TESTNOERROR(err);
	TESTE(iterErr == KErrNotFound, iterErr);
	TEST(index == KNumGlyphCodesLatin);
	TEST(numMismatches == 0);

	__UHEAP_MARKEND;*/
#endif
	}

/**
@SYMTestCaseID      GRAPHICS-FBSERV-0660
@SYMTestPriority    Med
@SYMTestType        UT
@SYMTestStatus      Implemented
@SYMPREQ            PREQ2678

@SYMTestCaseDesc
	Opens an RFbsGlyphDataIterator on many different fonts of different sizes and typefaces
	and uses many fonts, in order to test that the iterator can cope with being used on many
	fonts with many glyphs.

@SYMTestActions
	i. Perform test of 100 iterations, where:
		1. A new Latin font is created every iteration in order to force the Typeface Store
			to create a brand-new server-side font at each iteration. 
		2. For this font, open an RFbsGlyphDataIterator and cycle through all Latin glyphs.
		3. For each glyph, compare against the glyph image returned by CFont::GetCharacterData().
		4. Keep a record of the number of mismatches, and carry on to next font.
	ii. Perform i. again, but using the existing fonts.
	iii. Check that there are no mismatches, all glyphs and fonts were successfully checked, 
		and no error codes returned during the test.
	iv. Clean up all resources.
	
@SYMTestExpectedResults
	The glyphs provided by the iterator should match that returned by GetCharacterData()
	for every font and every iteration. 
*/
void CTFbsGlyphData::TestGlyphDataIteratorManyFontsStressL()
	{
	INFO_PRINTF1(_L("Stress test using a RFbsGlyphDataIterator with hundreds of fonts"));
	WARN_PRINTF1(_L("---Stress test TO BE REVISITED due to Broadcom defect ESLM-85LDV7 - TB10.1 Closing of RSgImage with duplicate handle used in same thread does not release GPU RAM"));
	TEST(EFalse);
	/*__UHEAP_MARK;

	const TInt KNumFonts = 100;
	const TInt KNumRepeatsPerFont = 2;
	TInt err = KErrNone;
	TInt numGlyphMismatches = 0;

	CFbsFont** font = new (ELeave) CFbsFont*[KNumFonts];
	Mem::FillZ(font, sizeof(CFbsFont*) * KNumFonts);

	// Do the whole thing KNumRepeatsPerFont times. The second+ repeats will 
	// re-use the fonts created in the first repeat, to ensure that fonts that 
	// may have been evicted are able to be re-used with the iterator.
	for (TInt rep = 0; (rep < KNumRepeatsPerFont) && (err == KErrNone); ++rep)
		{
		// Iterate through all the font variants:
		// Iterate all font styles, for all latin typefaces, at increasing sizes.
		TInt i = 0;
		for (; (i < KNumFonts) && (err == KErrNone); ++i)
			{
			// Only create this font if this font isn't already valid (i.e. when this is the 
			// first rep) otherwise re-use it.
			if (!font[i])
				{
				TFontSpec requestedFontSpec = GenerateDejaVuFontSpec(i);
				err = iTs->GetNearestFontToDesignHeightInPixels((CFont*&)font[i], requestedFontSpec);
				}
			if (err == KErrNone)
				{
				RFbsGlyphDataIterator iter;
				TInt iterErr = iter.Open(*(font[i]), iGlyphCodesLatin, KNumGlyphCodesLatin);
				if (iterErr != KErrNone)
					{
					ERR_PRINTF2(_L("Failed to open RFbsGlyphDataIterator [err=%d]"), iterErr);
					InfoPrintFontSpec(*(font[i]));
					iStep->SetTestStepResult(EFail);
					}
				else
					{
					TInt index = 0;
					for(; (iterErr == KErrNone) && (index < KNumGlyphCodesLatin) && (err == KErrNone) ; iterErr = iter.Next(), index++)
						{
						const TUint8* bitmapData = NULL;
						TSize bitmapSize;
						TOpenFontCharMetrics metrics;
						font[i]->GetCharacterData(iter.GlyphCode() | KGlyphCodeFlag, metrics, bitmapData, bitmapSize);
						if (iter.Rect().Size() == TSize(0, 0))
							{
							numGlyphMismatches += (bitmapSize != TSize(0, 0)) ? 1 : 0;
							}
						else
							{
							TBool match = EFalse;
							const TFontSpec fontSpec = font[i]->FontSpecInTwips();							
							// Compare to system-memory version of glyph
							RSgImage characterDataImage;
							err = CreateSgImageFromCharacterData(bitmapData, bitmapSize, fontSpec.iFontStyle.BitmapType(), characterDataImage);
							if (err == KErrNone) 
								{
								err = CompareSgImages(iEGL, iter.Image(), iter.Rect(), characterDataImage, TRect(bitmapSize), match);
								}
							if (err == KErrNone && !match)
								{
								++numGlyphMismatches;
								}
							characterDataImage.Close();
							}					
						}
					iter.Close();
					TESTE(iterErr == KErrNotFound, iterErr);
					TEST(index == KNumGlyphCodesLatin);					
					}
				}
			}
		// Check all the fonts were iterated through.
		TEST(i == KNumFonts);
		}
	TESTNOERROR(err);
	TEST(numGlyphMismatches == 0);

	// Cleanup
	for (TInt ii = 0; ii < KNumFonts; ii++)
		{
		iTs->ReleaseFont(font[ii]);
		}
	delete [] font;
	__UHEAP_MARKEND;*/
	}

/**
@SYMTestCaseID      GRAPHICS-FBSERV-0662
@SYMTestPriority    Low
@SYMTestType        UT
@SYMTestStatus      Implemented
@SYMPREQ            PREQ2678

@SYMTestCaseDesc
	Uses a RFbsGlyphDataIterator when there is no graphics memory available in the system.
	It shows that when under low graphics memory, Next() returns the correct error code
	as per the API (either KErrNoMemory or KErrNoGraphicsMemory, depending on the implementation 
	of Graphics Resource being used).

@SYMTestActions
	i. Create a CFbsFont from the typeface store.
	ii. Simulate a low graphics-memory situation by creating enough RSgImages to fill the memory,
	iii. Open a RFbsGlyphDataIterator on the font.
	iv. Attempt to use the iterator, calling Next(), checking the returned code.
	v. Close the iterator and release all graphics memory from ii.

@SYMTestExpectedResults
	Next() should return either KErrNoMemory or KErrNoGraphicsMemory depending on the implmentation
	of Graphics Resource used. It should return the same error as is returned when filling
	the graphics memory reaches the limit.
*/
void CTFbsGlyphData::TestGlyphDataIteratorNoGraphicsMemory()
	{
	INFO_PRINTF1(_L("Test that when there is no GPU memory available, Next() returns correct error"));
	WARN_PRINTF1(_L("---Stress test TO BE REVISITED due to Broadcom defect ESLM-85LDV7 - TB10.1 Closing of RSgImage with duplicate handle used in same thread does not release GPU RAM"));
	TEST(EFalse);
	/*__UHEAP_MARK;
	
	const TInt KFontSize = 128;
	CFbsFont* font = NULL;
	RFbsGlyphDataIterator iter;
	
	TInt err = iTs->GetNearestFontToDesignHeightInPixels((CFont*&)font, TFontSpec(KMonoTypefaceName, KFontSize));
	TESTNOERROR(err);
	
	// Simulate low OOGM situation by creating many RSgImages until out of memory.
	if (err == KErrNone)
		{
		InfoPrintFontSpec(*font);
		RArray <RSgImage> sgImageArray;
		TInt iterErr = KErrNone;
		TInt gfxMemErr = FillGraphicsMemoryWithImages(TSize(KFontSize, KFontSize), sgImageArray);
		TESTE(gfxMemErr == KErrNoMemory || gfxMemErr == KErrNoGraphicsMemory, gfxMemErr);
		if (gfxMemErr == KErrNoMemory || gfxMemErr == KErrNoGraphicsMemory)
			{
			// Next() could either fail with KErrNoMemory or KErrNoGraphicsMemory, but should
			// be the same error code as the last attempted creation of an SgImage, done in 
			// FillGraphicsMemoryWithImages() so compare against that code.
			iterErr = iter.Open(*font, iGlyphCodesLatin, KNumGlyphCodesLatin);
			for (; iterErr == KErrNone; iterErr = iter.Next())
				{
				// no operation
				}
			iter.Close();
			}
	
		// Release all the images used to simulate OOGM.
		for (TInt i = sgImageArray.Count() - 1; i >= 0; --i)
			{
			sgImageArray[i].Close();
			}
		sgImageArray.Close();
		
		// Log any errors only after memory is freed - this ensures there is enough
		// memory for the logger.
		TESTE(iterErr == gfxMemErr, iterErr);
		}

	iTs->ReleaseFont(font);
	
	__UHEAP_MARKEND;*/
	}

/**
@SYMTestCaseID      GRAPHICS-FBSERV-0666
@SYMTestPriority    Low
@SYMTestType        UT
@SYMTestStatus      Implemented
@SYMPREQ            PREQ2678

@SYMTestCaseDesc
	Uses a RFbsGlyphDataIterator after Next() returns an error, in order to show that 
	an error does not invalidate the state of the iterator and it is still usable. 

@SYMTestActions
	i. Open the RFbsGlyphDataIterator on 1 glyph code.
	ii. Store the data of the iterator and call Next() to reach the end of the iterator
	iii. Access the glyph data repeatedly and check that the iterator members
		 still match those in ii.

@SYMTestExpectedResults
	The calls to Next() should cause KErrNotFound since it is past the final glyph.
	The iterator data should match at all times since the iterator is never moved.
*/
void CTFbsGlyphData::TestGlyphDataIteratorNextIsAtomic()
	{
	INFO_PRINTF1(_L("To ensure that Next() is atomic, if it returns an error it is still useable"));
	__UHEAP_MARK;

	RFbsGlyphDataIterator iter;
	TInt iterErr = iter.Open(*iFont, iGlyphCodesLatin, 1);
	TESTNOERROR(iterErr);
	
	TSgDrawableId id = iter.Image().Id();
	TOpenFontCharMetrics metrics = iter.Metrics();
	TUint glyphCode = iter.GlyphCode();
	TRect rect = iter.Rect();
	
	for (TInt i = 0; i < 2; i++)
		{
		iterErr = iter.Next();
		TESTE(iterErr == KErrNotFound, iterErr);
		
		TEST(id == iter.Image().Id());
		TEST(glyphCode == iter.GlyphCode());
		TEST(rect == iter.Rect());
		TEST(CompareMetrics(metrics, iter.Metrics()) == 0);
		}
	iter.Close();
		
	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID      GRAPHICS-FBSERV-0665
@SYMTestPriority    High
@SYMTestType        UT
@SYMTestStatus      Implemented
@SYMPREQ            PREQ2678

@SYMTestCaseDesc
	Glyph Atlas white-box test.
	To ensure that the same RSgImage is used for repeated requests for the 
	same glyph in the same call to RFbsGlyphDataIterator:Open().
@SYMTestActions
	i Open the glyph data iterator with a list of glyph codes which are all the same
	ii Retrieve the drawable id of each iteration 
	iii Check that the same drawable id is retrieved in each iteration  
@SYMTestExpectedResults
	Each iteration returns the same drawable id. 
*/
void CTFbsGlyphData::TestGlyphDataIteratorSameGlyphCodes()
	{
	INFO_PRINTF1(_L("White box test - Ensure that the same RSgImage is used for repeated requests for the same glyph in the same call to Open()"));
	__UHEAP_MARK;

	const TUint KSameRepeatedGlyphCode = iGlyphCodesLatin['A' - 0x20];
	const TInt KNumGlyphs = 10;
	TUint* sameRepeatedGlyphCodes = new TUint[KNumGlyphs];
	for (TInt ii = 0; ii < KNumGlyphs; ++ii)
		{
		sameRepeatedGlyphCodes[ii] = KSameRepeatedGlyphCode;
		}
	RFbsGlyphDataIterator iter;
	TInt err = iter.Open(*iFont, sameRepeatedGlyphCodes, KNumGlyphs);
	
	TESTNOERROR(err);
	if (KErrNone == err)
		{
		// get the drawable id of the first glyph and check that the id is valid
		TSgDrawableId referenceId = iter.Image().Id();
		RSgImage image;
		TESTNOERROR(image.Open(referenceId));
		image.Close();
		TESTNOERROR(iter.Next());

		for (;KErrNone == err; err = iter.Next())
			{
			TEST(referenceId == iter.Image().Id());
			}
		TESTE(KErrNotFound == err, err);
		}

	iter.Close();
	delete[] sameRepeatedGlyphCodes;

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID      GRAPHICS-FBSERV-0668
@SYMTestPriority    High
@SYMTestType        UT
@SYMTestStatus      Implemented
@SYMPREQ            PREQ2678

@SYMTestCaseDesc
	To ensure that the iterator can successfully be opened on an array
	of glyph codes of various array sizes. 
@SYMTestActions
	Perform many iterations of opening an array and cycling through the glyphs,
	increasing the size of the array after each iteration. Some simple sanity-checking
	of the glyphs is performed.
@SYMTestExpectedResults
	KErrNone should be returned at all times. 
*/
void CTFbsGlyphData::TestGlyphDataIteratorManyArraySizes()
	{
	INFO_PRINTF1(_L("Ensure that the RFbsGlyphDataIterator successfully opens glyph code arrays of many sizes"));
	__UHEAP_MARK;
	
	RFbsGlyphMetricsArray glyphMetricsArray;
	
	TESTNOERROR(glyphMetricsArray.Get(*iFont, iGlyphCodesLatin, KNumGlyphCodesLatin));
	TInt iterErr = KErrNone;
	
	for (TInt arraySize = 1; (arraySize < KNumGlyphCodesLatin) && (iterErr == KErrNone); ++arraySize)
		{
		RFbsGlyphDataIterator iter;
		TInt iterErr = iter.Open(*iFont, iGlyphCodesLatin, arraySize);
		TESTNOERROR(iterErr);
		
		for (TInt index = 0; iterErr == KErrNone; iterErr = iter.Next(), ++index)
			{
			// sanity checking...
			if (iter.GlyphCode() != iGlyphCodesLatin[index])
				{
				ERR_PRINTF4(_L("Test failed at array size %d - Wanted glyphcode %d, got %d"), arraySize, iGlyphCodesLatin[index], iter.GlyphCode());
				iStep->SetTestStepResult(EFail);
				}
			if (CompareMetrics(iter.Metrics(), glyphMetricsArray[index]) != 0)
				{
				ERR_PRINTF3(_L("Test failed at array size %d, metrics check failed at glyphcode %d"), arraySize, iGlyphCodesLatin[index]);
				iStep->SetTestStepResult(EFail);
				}			
			}
		iter.Close();
		}

	glyphMetricsArray.Close();
	TESTNOERROR(iterErr);

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID      GRAPHICS-FBSERV-0669
@SYMTestPriority    Low
@SYMTestType        UT
@SYMTestStatus      Implemented
@SYMPREQ            PREQ2678

@SYMTestCaseDesc
	Negative test case to show that RFbsGlyphDataIterator and RFbsGlyphMetricsArray
	do not support bitmap fonts.
@SYMTestActions
	i. Load a bitmap font.
	ii. Attempt to open an RFbsGlyphDataIterator and RFbsGlyphMetricsArray with the font.
@SYMTestExpectedResults
	KErrNotSupported should be returned in both instances. 
*/
 void CTFbsGlyphData::TestBitmapFontSupport()
	{
	INFO_PRINTF1(_L("Test bitmap font not supported"));
	__UHEAP_MARK;
	
	CFbsFont* bitmapFont = NULL;
	TInt err = iTs->GetNearestFontToDesignHeightInPixels((CFont*&)bitmapFont, TFontSpec(_L("Digital"), 14));
	TESTNOERROR(err);
	TEST(!bitmapFont->IsOpenFont());
	
	RFbsGlyphDataIterator iter;
	err = iter.Open(*bitmapFont, iGlyphCodesLatin, KNumGlyphCodesLatin);
	TEST(err == KErrNotSupported);
	iter.Close();
	
	RFbsGlyphMetricsArray array;
	err = array.Get(*bitmapFont, iGlyphCodesLatin, KNumGlyphCodesLatin);
	TEST(err == KErrNotSupported);
	array.Close();
	
	iTs->ReleaseFont(bitmapFont);
	__UHEAP_MARKEND;
	}
 
/**
@SYMTestCaseID		GRAPHICS-FBSERV-0671
@SYMTestPriority	High
@SYMTestType		UT
@SYMTestStatus		Implemented
@SYMPREQ			PREQ2678

@SYMTestCaseDesc
	Shows that different threads (and therefore RFbsSessions) using fonts with the same
	TFontSpec share the same glyphs	in the atlas and do not create duplicate entries
	in the Glyph Atlas, and that releasing a font clears all associated glyphs in the
	atlas.

@SYMTestActions
	i. Create a handle to a test font in the current process.
	ii. Spawn a test thread and wait for it to complete. Within the thread :
		1. Create a font with the same fontspec as the parent process.
		2. Use the RFbsGlyphDataIterator API to force rasterization into the glyph atlas.
		3. Release the iterator.
	iii. Check there were no leaves from the thread.
	iv. Repeat ii. and iii. several times. Before using RFbsGlyphDataIterator,
		the thread checks that the glyphs are still in the atlas from the first thread.
	v. Check that the number of fonts in the atlas has increased by one only.
	vi. Check that the number of glyphs in the atlas has increased by the size of the 
		glyph code array.
	vii. Release the font in the parent process, thereby releasing the font and glyphs
		in the glyph atlas, and check that the state of the atlas is the same as before
		the test is run.

@SYMTestExpectedResults
	All threads should return no errors or leaves or panics.
	After all threads have finished:
		The glyph count should have increased by the size of the glyph code array used
		in the RFbsGlyphDataIterator, showing that	glyphs are only being added to the atlas
		once. The number of fonts in the atlas should have increased by one, showing
		that only the single underlying font object is being added to the atlas, despite
		different RFbsSessions and CFbsFont instances used.
	After the test font is released in the main process:
		The glyph count and font count return to the pre-test value, showing that when
		the last handle to a TFontSpec is released, the atlas frees its associated data.
*/
void CTFbsGlyphData::TestMultithreadShareSingleFont()
	{
	INFO_PRINTF1(_L("Test glyphs shared between RFbsSessions/threads/processes"));
#ifndef _DEBUG
	// Test relies on debug-only FBS messages EFbsMessAtlasGlyphCount and EFbsMessAtlasFontCount
	INFO_PRINTF1(_L("Skipping test in release mode"));
#else	
	__UHEAP_MARK;

	_LIT(KThreadName, "GlyphDataTestThread");
	const TInt KNumTestThreads = 5;
	const TFontSpec KTestFontSpec(KTypefaceName, 50);	
	const TInt atlasFontCountStart = iFbs->SendCommand(EFbsMessAtlasFontCount);
	const TInt atlasGlyphCountStart = iFbs->SendCommand(EFbsMessAtlasGlyphCount);

	CFbsFont* testFont;
	TInt err = iTs->GetNearestFontToDesignHeightInPixels( (CFont*&)testFont, KTestFontSpec);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not load font, err = %d"), err);
		iStep->SetTestStepResult(EFail);
		return;
		}
	
	// Check there are no glyphs belonging to the test font before the test starts.
	TInt atlasFontGlyphCount = iFbs->SendCommand(EFbsMessAtlasGlyphCount, testFont->Handle());
	TEST(atlasFontGlyphCount == 0);

	TGlyphDataMultithreadParams params = {KTestFontSpec, iGlyphCodesLatin, KNumGlyphCodesLatin, NULL};

	// Run the test threads sequentially, and check its exit status.
	RThread testThread;
	TInt numThreadsPassed = 0;
	for (TInt i = 0; i < KNumTestThreads; i++)
		{
		TBool threadPassed = ETrue;
		TGlyphDataThreadInfo info = {EGlyphDataMultiSessionTestShareGlyphs, params, i, iStep};
		err = testThread.Create(KThreadName, CTFbsGlyphData::ThreadFunction, KDefaultStackSize, KTestThreadMinHeapSize, KTestThreadMaxHeapSize, &info);
		TESTNOERROR(err);

		TRequestStatus statusThread;
		testThread.Logon(statusThread);
		testThread.Resume();
		
		User::WaitForRequest(statusThread);
		TInt threadResult = testThread.ExitReason();
		if (threadResult != KErrNone)
			{
			ERR_PRINTF3(_L("Thread %i: Terminated with reason %d"), i, threadResult);
			threadPassed = EFalse; 
			}
		TExitCategoryName exitCategory = testThread.ExitCategory();
		if (exitCategory.Compare(_L("Kill")) != 0)
			{
			ERR_PRINTF3(_L("Thread %i: Terminated with reason category '%S'"), i, &exitCategory);
			threadPassed = EFalse;
			}
		testThread.Close();
		numThreadsPassed += (threadPassed) ? 1 : 0;
		}
	TEST(numThreadsPassed == KNumTestThreads);

	// Check that the atlas still contains the glyphs and the font created by the threads
	// after they have died, since the font is still open in this process.
	atlasFontGlyphCount = iFbs->SendCommand(EFbsMessAtlasGlyphCount, testFont->Handle());
	TEST(atlasFontGlyphCount == params.iGlyphCodesCount);
	TInt atlasFontCount = iFbs->SendCommand(EFbsMessAtlasFontCount);
	TEST(atlasFontCount == (atlasFontCountStart + 1));

	iTs->ReleaseFont(testFont);
	testFont = NULL;

	// Check the atlas state is now the same as it was before the test started,
	// now that the last remaining handle to the font used in the threads is released.
	TInt atlasGlyphCountEnd = iFbs->SendCommand(EFbsMessAtlasGlyphCount);
	TEST(atlasGlyphCountStart == atlasGlyphCountEnd);
	TInt atlasFontCountEnd = iFbs->SendCommand(EFbsMessAtlasFontCount);
	TEST(atlasFontCountStart == atlasFontCountEnd);
	__UHEAP_MARKEND;
#endif
	}

/**
Worker thread for TestMultithreadShareSingleFont().
The thread uses RFbsGlyphDataIterator on a CFbsFont of the given TFontSpec.
Once complete the atlas is queried for the number of associated glyphs.
 */
void CTFbsGlyphData::ThreadShareGlyphsL(TInt aThreadNum, TGlyphDataMultithreadParams& aParam, CTestStep* aStep)
	{
	User::LeaveIfError(RFbsSession::Connect());
	CFbsTypefaceStore* ts = CFbsTypefaceStore::NewL(NULL);
	CleanupStack::PushL(ts);
	RFbsSession* fbs = RFbsSession::GetSession();

	CFbsFont* font;
	TInt err = ts->GetNearestFontToDesignHeightInPixels((CFont*&)font, aParam.iFontSpec);
	User::LeaveIfError(err);

	if (aThreadNum > 0)
		{
		// If this is not the first thread, it means the first thread has already executed and 
		// populated the glyph atlas with the glyphs. The font created by this thread 
		// should already have its glyphs in the atlas.
		TInt fontGlyphCount = fbs->SendCommand(EFbsMessAtlasGlyphCount, font->Handle());
		if (fontGlyphCount != aParam.iGlyphCodesCount)
			{
			aStep->ERR_PRINTF4(_L("Thread %d: Only %d glyphs in atlas before first iteration, expected %d"), aThreadNum, fontGlyphCount, aParam.iGlyphCodesCount);
			aStep->SetTestStepResult(EFail);
			}
		}

	RFbsGlyphDataIterator iter;
	for (err = iter.Open(*font, aParam.iGlyphCodes, aParam.iGlyphCodesCount); err == KErrNone; err = iter.Next())
		{
		// no-op
		}
	iter.Close();

	// Check that the glyphs of this font have been added to the atlas
	TInt fontGlyphCount = fbs->SendCommand(EFbsMessAtlasGlyphCount, font->Handle());
	if (fontGlyphCount != aParam.iGlyphCodesCount)
		{
		aStep->ERR_PRINTF5(_L("Thread %d: Only %d glyphs in atlas after last iteration, expected %d (err=%d)"), aThreadNum, fontGlyphCount, aParam.iGlyphCodesCount, err);
		aStep->SetTestStepResult(EFail);
		}
	if (err != KErrNotFound)
		{
		aStep->ERR_PRINTF3(_L("Thread %d: Error during test = %d"), aThreadNum, err);
		aStep->SetTestStepResult(EFail);
		}

	ts->ReleaseFont(font);
	CleanupStack::PopAndDestroy(1); // ts
	RFbsSession::Disconnect();
	}


/**
@SYMTestCaseID		GRAPHICS-FBSERV-0672
@SYMTestPriority	Medium
@SYMTestType		UT
@SYMTestStatus		Implemented
@SYMPREQ			PREQ2678

@SYMTestCaseDesc
	Tests that with many concurrent sessions connected to Fbserv, the atlas successfully
	returns the correct glyph images even if the atlas becomes full and has to evict glyphs.

@SYMTestActions
	i. Create 25 threads, each a unique session with Fbserv.
	ii. Launch the threads simultaneously. In each thread:
		1. Create a FBS typeface store and create a font which is unique in the process.
		2. Use RFbsGlyphDataIterator to iterate through the latin glyph codes.
		3. Check the image is correct for each glyph against the image returned by 
			CFont::GetCharacterData().
		4. Close the iterator.
		5. Release the font and close the typeface store. 
	iii. Once all threads have finished, check the exit status of each thread

@SYMTestExpectedResults
	Every glyph for every thread should match the image returned by GetCharacterData()
	All threads should exit normally with no Leave code.
*/
_LIT(KTestMultithreadStressFinishSemaphore, "TestMultithreadStressAtlasFinish");

void CTFbsGlyphData::TestMultithreadStressAtlas()
	{
	INFO_PRINTF1(_L("Stress test glyph atlas with multiple RFbsSessions"));
	WARN_PRINTF1(_L("---Stress test TO BE REVISITED due to Broadcom defect ESLM-85NEFT - TB10.1 eglCreateImageKHR hangs during multithreading"));
	TEST(EFalse);
	/*__UHEAP_MARK;

	TInt err = KErrNone;
	const TInt KNumTestThreads = 25;
	_LIT(KThreadNameFormat, "GlyphDataTestThread%i");

	// Create a semaphore that is signalled by each test thread when it has finished.
	RSemaphore threadFinishSemaphore;
	err = threadFinishSemaphore.CreateGlobal(KTestMultithreadStressFinishSemaphore, 0, EOwnerThread);
	TESTNOERROR(err);

	// Prepare the testdata for the threads
	// Each thread will have a TFontSpec which will cause unique CFbsFonts
	// to be created in the server, and therefore the atlas.
	RThread testThread[KNumTestThreads];
	TGlyphDataThreadInfo testInfo[KNumTestThreads];	
	for (TInt i = 0; i < KNumTestThreads; ++i)
		{
		testInfo[i].iStep = iStep;
		testInfo[i].iTest = EGlyphDataMultiSessionTestStressAtlas;
		testInfo[i].iParams.iFontSpec = GenerateDejaVuFontSpec(i);
		testInfo[i].iParams.iGlyphCodes = iGlyphCodesLatin;
		testInfo[i].iParams.iGlyphCodesCount = KNumGlyphCodesLatin;
		testInfo[i].iParams.iEGL = iEGL;
		testInfo[i].iThreadNum = i;	
		TBuf<128> threadName;
		threadName.AppendFormat(KThreadNameFormat, i);
		err = testThread[i].Create(threadName, CTFbsGlyphData::ThreadFunction, KDefaultStackSize, KTestThreadMinHeapSize, KTestThreadMaxHeapSize, &testInfo[i]);
		TESTNOERROR(err);
		}

	// All threads are created, start them simultaneously.
	for (TInt i = 0; i < KNumTestThreads; ++i)
		{
		testThread[i].Resume();
		}
	// Wait for all threads to finish.
	for (TInt i = 0; i < KNumTestThreads; ++i)
		{
		threadFinishSemaphore.Wait();
		}
	// Allow some time for remaining threads to finish tidy-up.
	User::After(100000);
	threadFinishSemaphore.Close();

	TInt numThreadsPassed = 0;
	for (TInt i = 0; i < KNumTestThreads; ++i)
		{
		TBool threadPassed = ETrue;
		TInt threadResult = testThread[i].ExitReason();
		if (threadResult != KErrNone)
			{
			ERR_PRINTF3(_L("Thread %i: Terminated with reason %d"), i, threadResult);
			threadPassed = EFalse; 
			}
		TExitCategoryName exitCategory = testThread[i].ExitCategory();
		if (exitCategory.Compare(_L("Kill")) != 0)
			{
			ERR_PRINTF3(_L("Thread %i: Terminated with reason category '%S'"), i, &exitCategory);
			threadPassed = EFalse;
			}
		testThread[i].Close();
		numThreadsPassed += (threadPassed) ? 1 : 0;
		}
	TEST(numThreadsPassed == KNumTestThreads);

	__UHEAP_MARKEND;*/
	}
/**
Worker thread for TestMultithreadStressAtlas().
The thread uses RFbsGlyphDataIterator on a CFbsFont of the given TFontSpec.
For each glyph, the image returned by the iterator is compared to the image 
returned from CFont::GetCharacterData().
Once complete, the semaphore is signalled to tell the parent process it has
finished.
 */
void CleanupFinishSemaphore(TAny* aItem)
    {
    RSemaphore* semaphore = reinterpret_cast<RSemaphore*>(aItem);
    semaphore->Signal();
    semaphore->Close();
    }
void CTFbsGlyphData::ThreadStressAtlasL(TInt aThreadNum, TGlyphDataMultithreadParams& aParam, CTestStep* aStep)
	{
	TOpenFontCharMetrics charMetrics;
	const TUint8* bitmapData;
	TSize bitmapSize;
	RSgImage charDataImage;

	RSemaphore threadFinishSemaphore;
	User::LeaveIfError(threadFinishSemaphore.OpenGlobal(KTestMultithreadStressFinishSemaphore));
	CleanupStack::PushL(TCleanupItem(CleanupFinishSemaphore, &threadFinishSemaphore));

	User::LeaveIfError(RFbsSession::Connect());
	CFbsTypefaceStore* ts = CFbsTypefaceStore::NewL(NULL);
	CleanupStack::PushL(ts);
	
	CFbsFont* font;
	User::LeaveIfError(ts->GetNearestFontToDesignHeightInPixels((CFont*&)font, aParam.iFontSpec));

	TInt numGlyphMatches = 0;
	TInt index = 0;
	TInt err = KErrNone;
	RFbsGlyphDataIterator iter;
	for (err = iter.Open(*font, aParam.iGlyphCodes, aParam.iGlyphCodesCount); err == KErrNone; err = iter.Next(), ++index)
		{
		TBool glyphMatch = EFalse;
		font->GetCharacterData(aParam.iGlyphCodes[index] | KGlyphCodeFlag, charMetrics, bitmapData, bitmapSize);
		if (bitmapSize == TSize(0, 0))
			{
			glyphMatch = (bitmapSize == iter.Rect().Size());
			}
		else
			{
			err = CreateSgImageFromCharacterData(bitmapData, bitmapSize, font->FontSpecInTwips().iFontStyle.BitmapType(), charDataImage);
			if (err == KErrNone)
				{
				err = CompareSgImages(aParam.iEGL, iter.Image(), iter.Rect(), charDataImage, TRect(bitmapSize), glyphMatch);
				}
			charDataImage.Close();
			}
		if (err != KErrNone)
			{
			break;
			}
		numGlyphMatches += (glyphMatch) ? 1 : 0;
		}
	iter.Close();
	
	if (index != aParam.iGlyphCodesCount)
		{
		aStep->ERR_PRINTF5(_L("Thread %d: Iterator terminated early - %d out of %d glyphs (err=%d)"), aThreadNum, index, aParam.iGlyphCodesCount, err);
		aStep->SetTestStepResult(EFail);
		}
	if (index != numGlyphMatches)
		{
		aStep->ERR_PRINTF4(_L("Thread %d: Matched %d out of %d glyphs"), aThreadNum, numGlyphMatches, aParam.iGlyphCodesCount);
		aStep->SetTestStepResult(EFail);
		}

	ts->ReleaseFont(font);
	CleanupStack::PopAndDestroy(2); // ts, threadFinishSemaphore
	RFbsSession::Disconnect();
	}

/**
@SYMTestCaseID      GRAPHICS-FBSERV-0673
@SYMTestPriority    Medium
@SYMTestType        UT
@SYMTestStatus      Implemented
@SYMPREQ            PREQ2678

@SYMTestCaseDesc
    Tests the robustness of using RFbsGlyphMetricsArray when the client heap and the
    FbServ private heap experience failures allocating memory, causing no panics 
    or leaves.

@SYMTestActions
    i. Set the default heap failure for the next heap allocation.
    ii. Create a new CFbsFont using a TFontSpec not already in the glyph atlas.
    iii. Call RFbsGlyphMetricsArray::Get(), and close the array.
    iv. Release the font so that nothing is left in the cache as a result of
        attempting to use it, and reset the heap failure state.
    v. While iii returns KErrNoMemory, increment the failure count and repeat
        step ii.
    vi. Using a separate font so that the test is not affected by the earlier
        run, repeat ii. to v., but rather than setting the default heap to 
        fail, the FbServ private heap is set to fail, via IPC messages to Fbs.

@SYMTestExpectedResults
    If no errors occur, KErrNone should be returned after a certain number of
    repetitions. Any other error code denotes a problem handling low-memory 
    situtations.
*/
void CTFbsGlyphData::TestGlyphMetricsArrayHeapOOML()
    {
    INFO_PRINTF1(_L("Test RFbsGlyphMetricsArray during heap alloc failure"));
    __UHEAP_MARK;

    // Create a font that wont be in the cache already...
    TInt rep = 0;
    TInt err = KErrNoMemory;
    CFbsFont* font = NULL;
    
    while (err == KErrNoMemory)
        {
        User::LeaveIfError(iTs->GetNearestFontInPixels((CFont*&)font, GenerateDejaVuFontSpec(10)));
        __UHEAP_FAILNEXT(rep);
        RFbsGlyphMetricsArray array;
        err = array.Get(*font, iGlyphCodesLatin, KNumGlyphCodesLatin);
        array.Close();
        __UHEAP_RESET;
        iTs->ReleaseFont(font);
        font = NULL;
        ++rep;
        }

	TESTE(err == KErrNone, err);
    if (err == KErrNone)
        {
        INFO_PRINTF2(_L("Client Heap OOM : Test passed after rep %d"), rep);
        }
    else
        {
        ERR_PRINTF3(_L("Client Heap OOM : Test failed with err=%d, after rep %d"), err, rep);
        }

    // Now test when the server-side FbServ heap fails...
    rep = 0;
    err = KErrNoMemory;
    
    while (err == KErrNoMemory)
        {
        User::LeaveIfError(iTs->GetNearestFontInPixels((CFont*&)font, GenerateDejaVuFontSpec(11)));
        iFbs->SendCommand(EFbsMessSetHeapFail, RFbsSession::EHeapFailTypeServerMemory, rep);
        RFbsGlyphMetricsArray array;
        err = array.Get(*font, iGlyphCodesLatin, KNumGlyphCodesLatin);
        array.Close();
        iFbs->SendCommand(EFbsMessSetHeapReset, RFbsSession::EHeapFailTypeServerMemory);
        iTs->ReleaseFont(font);
        font = NULL;
        ++rep;
        }

	TESTE(err == KErrNone, err);
    if (err == KErrNone)
        {
        INFO_PRINTF2(_L("FBServ Heap OOM : Test passed after rep %d"), rep);
        }
    else
        {
        ERR_PRINTF3(_L("FBServ Heap OOM : Test failed with err=%d, after rep %d"), err, rep);      
        }
    __UHEAP_MARKEND;
    }

/**
@SYMTestCaseID      GRAPHICS-FBSERV-0674
@SYMTestPriority    Medium
@SYMTestType        UT
@SYMTestStatus      Implemented
@SYMPREQ            PREQ2678

@SYMTestCaseDesc
    Tests the robustness of using RFbsGlyphDataIterator when the client heap and the
    FbServ private heap experience failures allocating memory, causing no panics 
    or leaves.

@SYMTestActions
    i. Set the default heap failure for the next heap allocation.
    ii. Create a new CFbsFont using a TFontSpec not already in the glyph atlas.
    iii. Call RFbsGlyphDataIterator::Open(), and close the array.
    iv. Release the font so that nothing is left in the cache as a result of
        attempting to use it, and reset the heap failure state.
    v. While iii returns KErrNoMemory, increment the failure count and repeat
        step ii.
    vi. Using a separate font so that the test is not affected by the earlier
        run, repeat ii. to v., but rather than setting the default heap to 
        fail, the FbServ private heap is set to fail, via IPC messages to Fbs.

@SYMTestExpectedResults
    If no errors occur, KErrNone should be returned after a certain number of
    repetitions. Any other error code denotes a problem handling low-memory 
    situtations.
*/
void CTFbsGlyphData::TestGlyphDataIteratorHeapOOML()
    {
    INFO_PRINTF1(_L("Test RFbsGlyphDataIterator during heap alloc failure"));    
    __UHEAP_MARK;

    // Create a font that wont be in the cache already...
    TInt rep = 0;
    TInt err = KErrNoMemory;
    CFbsFont* font = NULL;
    
    while (err == KErrNoMemory)
        {
        User::LeaveIfError(iTs->GetNearestFontInPixels((CFont*&)font, GenerateDejaVuFontSpec(10)));
        __UHEAP_FAILNEXT(rep);
        RFbsGlyphDataIterator iter;
        err = iter.Open(*font, iGlyphCodesLatin, KNumGlyphCodesLatin);
        if (err == KErrNone)
            {
            while (err == KErrNone) 
               {
               err = iter.Next();
               }
            err = (err == KErrNotFound) ? KErrNone : err;
            }
        iter.Close();
        __UHEAP_RESET;
        iTs->ReleaseFont(font);
        font = NULL;
        ++rep;
        }

	TESTE(err == KErrNone, err);
    if (err == KErrNone)
        {
        INFO_PRINTF2(_L("Client Heap OOM : Test passed after rep %d"), rep);
        }
    else
        {
        ERR_PRINTF3(_L("Client Heap OOM : Test failed with err=%d, after rep %d"), err, rep);
        }

    // Now test when the server-side FbServ heap fails...
    rep = 0;
    err = KErrNoMemory;
    
    while (err == KErrNoMemory)
        {
        User::LeaveIfError(iTs->GetNearestFontInPixels((CFont*&)font, GenerateDejaVuFontSpec(11)));
        iFbs->SendCommand(EFbsMessSetHeapFail, RFbsSession::EHeapFailTypeServerMemory, rep);
        RFbsGlyphDataIterator iter;
        err = iter.Open(*font, iGlyphCodesLatin, KNumGlyphCodesLatin);
        if (err == KErrNone)
            {
            while (err == KErrNone) 
               {
               err = iter.Next();
               }
            err = (err == KErrNotFound) ? KErrNone : err;
            }
        iter.Close();
        iFbs->SendCommand(EFbsMessSetHeapReset, RFbsSession::EHeapFailTypeServerMemory);
        iTs->ReleaseFont(font);
        font = NULL;
        ++rep;
        }

	TESTE(err == KErrNone, err);
    if (err == KErrNone)
        {
        INFO_PRINTF2(_L("FBServ Heap OOM : Test passed after rep %d"), rep);
        }
    else
        {
        ERR_PRINTF3(_L("FBServ Heap OOM : Test failed with err=%d, after rep %d"), err, rep);
        }
    __UHEAP_MARKEND;
    }

/**
Utility function. Prints out a description of the font's fontspec to the log.
 */
void CTFbsGlyphData::InfoPrintFontSpec(const CFont& aFont)
	{
	_LIT(KMonochromeBitmap, "Mono");
	_LIT(KAntiAliasedBitmap, "AA");
	_LIT(KStyleItalic, "Italic");
	_LIT(KStyleBold, "Bold");
	_LIT(KStyleNormal, "Normal");
	_LIT(KUnknown, "Unknown");
	TBufC<9> bitmapType;
	TBuf<12> fontStyle;
	TFontSpec fontSpec = aFont.FontSpecInTwips();
	switch(fontSpec.iFontStyle.BitmapType())
		{
		case EMonochromeGlyphBitmap:
			bitmapType = KMonochromeBitmap;
			break;
		case EAntiAliasedGlyphBitmap:
			bitmapType = KAntiAliasedBitmap;
			break;
		default:
			bitmapType = KUnknown;
		}

	if (fontSpec.iFontStyle.StrokeWeight() == EStrokeWeightBold)
		{
		fontStyle.Append(KStyleBold);
		}
	if (fontSpec.iFontStyle.Posture() == EPostureItalic)
		{
		fontStyle.Append(KStyleItalic);
		}
	if (fontStyle.Length() == 0)
		{
		fontStyle = KStyleNormal;
		}

	INFO_PRINTF5(_L("Font: name=%S size=%dtw type=%S style=%S"), &(fontSpec.iTypeface.iName), fontSpec.iHeight, &bitmapType, &fontStyle);
	}


/**
Static utility function. Performs a per-pixel comparison of two open RSgImages.
To do this requires access to the binary data of the images, only accessable
via EGL and Khronos APIs. This function will bind the RSgImages to VGImages 
and uses OpenVG to retrieve the image data in 8bpp.
@param aEGL An EGL Helper to read the SgImages into system memory.
@param aImageA The first image to compare.
@param aRectA A rectangular portion in pixels of the first image to compare.
@param aImageB The second image to compare.
@param aRectB A rectangular portion in pixels fo the second image to compare.
@param aMatch A boolean value, which on return tells the caller whether the two
	images were deemed to match.
@return KErrNone, if the comparison took place, otherwise one of the system-wide
	error codes.
*/
TInt CTFbsGlyphData::CompareSgImages(CEGLHelper* aEGL, const RSgImage& aImageA, const TRect& aRectA, const RSgImage& aImageB, const TRect& aRectB, TBool& aMatch)
	{
	return CTFbsGlyphData::CompareSgImages(aEGL, aImageA, aRectA, NULL, aImageB, aRectB, NULL, aMatch);
	}

/**
Static utility function. Performs a per-pixel comparison of two open RSgImages.
To do this requires access to the binary data of the images, only accessable
via EGL and Khronos APIs. This function will bind the RSgImages to VGImages 
and uses OpenVG to retrieve the image data in 8bpp.
This version allows pre-created memory to be used in the comparison, to avoid
allocation failure in low memory testing.
@param aEGL An EGL Helper to read the SgImages into system memory buffers.
@param aImageA The first image to compare.
@param aRectA A rectangular portion in pixels of the first image to compare.
@param aBufferA If non-NULL, specifies a memory buffer to read the data of
	aImageA into, otherwise a buffer is dynamically allocated.
@param aImageB The second image to compare.
@param aRectB A rectangular portion in pixels fo the second image to compare.
@param aBufferB If non-NULL, specifies a memory buffer to read the data of
	aImageB into, otherwise a buffer is dynamically allocated.
@param aMatch A boolean value, which on return tells the caller whether the two
	images were deemed to match.
@return KErrNone, if the comparison took place, otherwise one of the system-wide
	error codes.
*/
TInt CTFbsGlyphData::CompareSgImages(CEGLHelper* aEGL, const RSgImage& aImageA, const TRect& aRectA, TUint8* aBufferA, const RSgImage& aImageB, const TRect& aRectB, TUint8* aBufferB, TBool& aMatch)
	{
	// By default, assume they do not match.
	aMatch = EFalse;
	
#ifdef SAVEGLYPHSTOMBMDURINGCOMPARISON
	
	static TInt countToAppend = 0;
	
	CFbsBitmap* bitmap = NULL;
	if (KErrNone == CreateBitmapFromSgImage(aEGL, aImageA, aRectA, bitmap))
		{
		TBuf<KMaxFileName> buf;
		buf.AppendNum( countToAppend );
		TPtrC nameAppend( buf );
		
		SaveBmp(bitmap, &nameAppend, EFalse);
		}
	delete bitmap;	
	if (KErrNone == CreateBitmapFromSgImage(aEGL, aImageB, aRectB, bitmap))
		{
		TBuf<KMaxFileName> buf;
		buf.AppendNum( countToAppend );
		TPtrC nameAppend( buf );
	
		SaveBmp(bitmap, &nameAppend, ETrue);
		}
	delete bitmap;
	
	countToAppend++;
	
#endif // SAVEGLYPHSTOMBMDURINGCOMPARISON
	
	TSgImageInfo imageInfoA;
	TSgImageInfo imageInfoB;
	if (aImageA.GetInfo(imageInfoA) != KErrNone ||
		aImageB.GetInfo(imageInfoB) != KErrNone)
		{
		return KErrBadHandle;
		}
		
	// Check the sizes of the images match, and the rects reside on the images.
	if (aRectA.Size() != aRectB.Size() ||
		!TRect(imageInfoA.iSizeInPixels).Intersects(aRectA) ||
		!TRect(imageInfoB.iSizeInPixels).Intersects(aRectB))
		{
		return KErrNone;		
		}
	const TSize KBufferSize = aRectA.Size();
	const TInt KDataStride = KBufferSize.iWidth;

	TBool freeTempBufA = EFalse;
	TBool freeTempBufB = EFalse;
	if (!aBufferA)
		{
		aBufferA = (TUint8*) User::AllocZ(KDataStride * KBufferSize.iHeight);
		freeTempBufA = ETrue;
		}
	if (!aBufferA)
		{
		return KErrNoMemory;
		}
	TInt err = aEGL->GetSgImageData(aImageA, aRectA, aBufferA);
	if (err != KErrNone)
		{
		if (freeTempBufA)
			{
			User::Free(aBufferA);
			aBufferA = NULL;
			}
		return err;
		}
	if (!aBufferB)
		{
		aBufferB = (TUint8*) User::AllocZ(KDataStride * KBufferSize.iHeight);
		freeTempBufB = ETrue;
		}
	if (!aBufferB)
		{
		if (freeTempBufA)
			{
			User::Free(aBufferA);
			aBufferA = NULL;
			}
		return KErrNoMemory;
		}
	err = aEGL->GetSgImageData(aImageB, aRectB, aBufferB);
	if (err != KErrNone)
		{
		if (freeTempBufA)
			{
			User::Free(aBufferA);
			aBufferA = NULL;
			}
		if (freeTempBufB)
			{
			User::Free(aBufferB);
			aBufferB = NULL;
			}
		return err;
		}	

	// Perform a per-pixel comparison, scanline by scanline.
	// The loop will break as soon as a mismatch is detected.
	aMatch = ETrue;
	for (TInt scanline = 0; (scanline < KBufferSize.iHeight) && aMatch; ++scanline)
		{
		TUint8* scanlineImageA = aBufferA + (scanline * KDataStride);
		TUint8* scanlineImageB = aBufferB + (scanline * KDataStride);
		aMatch = (Mem::Compare(scanlineImageA, KBufferSize.iWidth, scanlineImageB, KBufferSize.iWidth) == 0);
		}

	if (freeTempBufA)
		{
		User::Free(aBufferA);
		aBufferA = NULL;
		}
	if (freeTempBufB)
		{
		User::Free(aBufferB);
		aBufferB = NULL;
		}
	
	return KErrNone;
	}

/**
Second thread entry function for multi-threaded tests.
*/
TInt CTFbsGlyphData::ThreadFunction(TAny* aParam)
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanupStack = CTrapCleanup::New();
	if (!cleanupStack)
		{
		return KErrNoMemory;
		}

	TGlyphDataThreadInfo* info = static_cast<TGlyphDataThreadInfo*>(aParam);
	TRAPD(result,
	switch(info->iTest)
		{
		case EGlyphDataMultiSessionTestShareGlyphs:
			CTFbsGlyphData::ThreadShareGlyphsL(info->iThreadNum, info->iParams, info->iStep);
			break;
		case EGlyphDataMultiSessionTestStressAtlas:
			CTFbsGlyphData::ThreadStressAtlasL(info->iThreadNum, info->iParams, info->iStep);
			break;
		default:
			User::Leave(KErrArgument);
		}
	);

	delete cleanupStack;
	__UHEAP_MARKEND;
	return result;
	}



/*
	-----------------------------------------
	Static utility Methods used by the tests.
	-----------------------------------------
*/



/**
Utility method that fills the RSgImage memory with RSgImages until either KErrNoMemory
or KErrNoGraphicsMemory is returned.

@param aSize The size of the image used to fill the graphics memory - a form of granularity
@param aImages Returns the array of the images used to fill the graphics memory.
@return KErrNoGraphicsMemory or KErrNoMemory if successful, otherwise one of the system
	wide error codes.
 */
/*static TInt FillGraphicsMemoryWithImages(const TSize& aSize, RArray<RSgImage>& aImages)
	{
	TInt err = KErrNone;
	while (KErrNone == err)
		{
		RSgImage sgImage;
		err = sgImage.Create(TSgImageInfo(aSize, ESgPixelFormatA_8, ESgUsageBitOpenVgImage));
		if (KErrNone == err)
			{
			err = aImages.Append(sgImage);
			}
		}
	return err;
	}*/

/**
Utility method that fills the RSgImage memory with RSgImages until either KErrNoMemory
or KErrNoGraphicsMemory is returned and then closes one RSgImage to free up some memory.

@param aSize The size of the image used to fill the graphics memory - a form of granularity
@param aImages Returns the array of the images used to fill the graphics memory.
@return KErrNone if successful, otherwise one of the system	wide error codes.
 */
/*static TInt NearlyFillGraphicsMemoryWithImages(const TSize& aSize, RArray<RSgImage>& aImages)
	{
	TInt err = FillGraphicsMemoryWithImages(aSize, aImages);
	if (err == KErrNoMemory || err == KErrNoGraphicsMemory)
		{
		if (aImages.Count() > 0)
			{
			// Remove an image to free up some memory.
			TInt lastIndex = aImages.Count() - 1;
			aImages[lastIndex].Close();
			aImages.Remove(lastIndex);
			}
		err = KErrNone;
		}
	return err;
	}*/

/**
Static utility function. Creates an 8bpp RSgImage from 1bpp or 8bpp character
data, with VGImage usage flag set.
@param aData The character image data. Either in 8bpp or 1bpp RLE format.
@param aSize The size of the character image in pixels.
@param aType The type of glyph - Monochrome or Antialiased. 
@param aSgImage A closed image which will be populated with 8bpp image data.
*/
static TInt CreateSgImageFromCharacterData(const TUint8* aData, const TSize& aSize, TGlyphBitmapType aType, RSgImage& aImage)
	{
	return CreateSgImageFromCharacterData(aData, aSize, aType, aImage, NULL, NULL);
	}

/**
Static utility function. Creates an 8bpp RSgImage from 1bpp or 8bpp character
data, with VGImage usage flag set. 
This overload allows the memory for the buffers to be pre-created to avoid
memory allocation failure during low-memory testing.
@param aData The character image data. Either in 8bpp or 1bpp RLE format.
@param aSize The size of the character image in pixels.
@param aType The type of glyph - Monochrome or Antialiased. 
@param aSgImage A closed image which will be populated with 8bpp image data.
@param aBuffer1 If non-NULL, used as a memory buffer for reading the decoded 
	image data into for monochrome images.
@param aBuffer2 If non-NULL, used as a memory buffer for the decoded image
	data for monochrome images.
*/
static TInt CreateSgImageFromCharacterData(const TUint8* aData, const TSize& aSize, TGlyphBitmapType aType, RSgImage& aImage, TUint8* aBuffer1, TUint8* aBuffer2)
	{
	TInt err = KErrNone;	
	if (aSize == TSize(0, 0))
		{
		return KErrArgument;
		}
	TUint8* dataBuf = NULL;
	TInt dataStride = 0;
	TBool freeDataBuf = EFalse;
	if (aType == EAntiAliasedGlyphBitmap)
		{
		dataBuf = const_cast<TUint8*>(aData);
		dataStride = aSize.iWidth;
		}
	else if (aType == EMonochromeGlyphBitmap)
		{
		TUint8* binaryData = NULL;
		TUint8* tempBuf = NULL;
		TInt binaryDataStride = ((aSize.iWidth + 31) / 32) << 2;
		TInt binaryDataSize = binaryDataStride * aSize.iHeight;
		if (aBuffer1 && User::AllocLen(aBuffer1) >= binaryDataSize)
			{
			binaryData = aBuffer1;
			}
		else
			{
			tempBuf = (TUint8*) User::AllocZ(binaryDataSize);
			if (!tempBuf)
				{
				return KErrNoMemory;
				}
			binaryData = tempBuf;
			}
		// Unpack the run length encoded data into 1bpp
		DecodeBinaryData(aSize, aData, binaryDataStride, reinterpret_cast<TUint32*&>(binaryData));
		dataStride = aSize.iWidth;
		TInt byteDataSize = dataStride * aSize.iHeight;
		TUint8* byteData = NULL;
		// If aByteBuf supplied, use that instead of allocating a new buffer here.
		if (aBuffer2 && User::AllocLen(aBuffer2) >= byteDataSize)
			{
			byteData = aBuffer2;
			}
		else
			{
			byteData = (TUint8*) User::AllocZ(byteDataSize);
			if (!byteData)
				{
				User::Free(tempBuf);
				return KErrNoMemory;
				}
			freeDataBuf = ETrue;
			}
		dataBuf = byteData;
		for (TInt scanline = 0; scanline < aSize.iHeight; ++scanline)
			{
			TUint8* srcByte = binaryData;
			for (TInt pixel = 0; pixel < aSize.iWidth; pixel++)
				{
				*(byteData+pixel) = ((*srcByte & (1 << (pixel % 8))) == 0) ? 0 : 0xFF;
				if (((pixel + 1) % 8) == 0) srcByte++;
				}
			byteData += dataStride;
			binaryData += binaryDataStride;
			}
		User::Free(tempBuf);
		}
	else
		{
		return KErrArgument;
		}

	// Create RSgImage from CFbsBitmap.
	TSgImageInfo sgImageInfo(aSize, ESgPixelFormatA_8, ESgUsageBitOpenVgImage);
	err = aImage.Create(sgImageInfo, dataBuf, dataStride);
	if (freeDataBuf)
		{
		User::Free(dataBuf);
		}
	return err;
	}


/**
Static utility function, Copies image data line(s) to a destination.
@param aBinaryDataPtr pointer to a destination buffer.
@param aBufferWords Stride of the image.
@param aData Pointer to a source buffer.
@param aBitShift Number of bits, binary data will be shifted. 
@param aCharWidth Width of the image.
@param aRepeatCount Number of lines to copy.
*/
static void CopyCharLine(TUint32*& aBinaryDataPtr,TInt aBufferWords,const TUint8* aData,TInt aBitShift,TInt aCharWidth, TInt16 aRepeatCount)
	{
	aBitShift&=7;
	TInt wordstocopy=(aCharWidth+31)>>5;
	if(wordstocopy>aBufferWords) wordstocopy=aBufferWords;
	TUint32* ptrlimit=aBinaryDataPtr+wordstocopy;
	TUint32* dataword=(TUint32*)(TInt(aData)&~3);
	aBitShift+=(TInt(aData)-TInt(dataword))<<3;
	
	TUint32* startBinaryDataPtr = aBinaryDataPtr;
	while(aBinaryDataPtr<ptrlimit)
		{
		*aBinaryDataPtr=*dataword++;
		*aBinaryDataPtr>>=aBitShift;
		if(aBitShift) *aBinaryDataPtr|=(*dataword<<(32-aBitShift));
		aBinaryDataPtr++;
		}
	
	TUint32* curStartBinaryDataPtr = aBinaryDataPtr;
	TInt byteToCopy = wordstocopy << 2;
	while(aRepeatCount > 1)
		{
		Mem::Copy(curStartBinaryDataPtr, startBinaryDataPtr, byteToCopy);
		curStartBinaryDataPtr += wordstocopy;
		
		aRepeatCount--;
		}
	aBinaryDataPtr = curStartBinaryDataPtr;
	}

/**
Static utility function. Decodes a monochrome glyph whose data is run length encoded, 
into a 1bpp bitmap.
@param aDataSize Image size in pixels.
@param aData Pointer to a source buffer.
@param aStride Image data stride.
@param aBinaryData Pointer to a destination buffer. This buffer must be allocated 
	by the caller.
*/
static void DecodeBinaryData(const TSize& aDataSize, const TUint8* aData, TInt aStride,
											TUint32* aBinaryData)
	{
	const TInt datalength = aDataSize.iWidth;
	const TInt dataheight = aDataSize.iHeight;
	TInt bitindex=0;
	TInt16 repeatcount=0;
	TUint32* slbuffer=aBinaryData;
	const TInt slwords=aStride;

	for(TInt charline=0;charline<dataheight;charline+=repeatcount) // for lines in the character...
		{
		repeatcount=CFbsBitGc::Load16(aData+(bitindex>>3));
		repeatcount>>=bitindex&7;
		const TInt multilineflag=repeatcount&1;
		repeatcount>>=1;
		repeatcount&=0xf;
		bitindex+=5;
		if(multilineflag)
			{
			for(TInt currentline=0;currentline<repeatcount;currentline++)
				{
				CopyCharLine(slbuffer,slwords,aData+(bitindex>>3),bitindex&7,datalength, 1);
				bitindex+=datalength;
				}
			}
		else
			{
			CopyCharLine(slbuffer,slwords,aData+(bitindex>>3),bitindex&7,datalength, repeatcount);
			bitindex+=datalength;
			}
		}
	}
//--------------
__CONSTRUCT_STEP__(FbsGlyphData)
