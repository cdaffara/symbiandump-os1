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

#include "tinterleaving.h"
#include "tdirectgdieglcontent_client.h"
#include "tdirectgdieglcontent_clientserver.h"
#include <graphics/sgimage_sw.h>
#include <graphics/directgdiimagetarget.h>
#include <graphics/directgdidriver.h>
#include <e32math.h>

/**
A utility class used by the templated function CleanupResetAndDestroyPushL() to create
a TCleanupItem item that will perform a ResetAndDestroy() operation on the class T type object.
Used for cleanup RPointerArray objects.
*/
template <class T> class CleanupResetAndDestroy
	{
public:
	static void PushL(T &aRef)
		{
		CleanupStack::PushL(TCleanupItem(&ResetAndDestroy, &aRef));
		};

private:
	static void ResetAndDestroy(TAny *aPtr)
		{
		(STATIC_CAST(T *, aPtr))->ResetAndDestroy();
		};
	};

/**
Helper function for pushing CleanupResetAndDestroy object onto CleanupStack.
*/
template <class T> void CleanupResetAndDestroyPushL(T &aRef)
	{
	CleanupResetAndDestroy<T>::PushL(aRef);
	};

CTInterleaving::CTInterleaving()
	{
	SetTestStepName(KTDirectGdiInterleavingStep);
	}

CTInterleaving::~CTInterleaving()
	{
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-INTERLEAVING-0001
	
@SYMPREQ 			
	PREQ39

@SYMREQ
	REQ9188 
	REQ9189 
	REQ9196 
	REQ9213 
	REQ9227 
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237
	
@SYMTestCaseDesc	
	Test synchronized drawing with egl content
	
@SYMTestPriority	
	High
	
@SYMTestStatus 		
	Implemented
	
@SYMTestActions 	Establish connection to EglContentServer.
					Render frame synchronized with EGL content in following way:
					 - Draw multicolor checkboard to gdi target
					 - Get EGL content sgimage id from EglContentServer in synchronous mode.
					 - Using obtained id create DirectGdi image source and draw it on target with  DrawResource() method.
					 - Draw semi-transparent vertical bars to gdi target.
					Repeat above steps a few times. 

@SYMTestExpectedResults 
	Valid set of bitmaps should be created.
    These bitmaps should be the same as a reference bitmaps.
*/
void CTInterleaving::TestSyncL()
	{
	_LIT(KTestName, "Interleaving-Sync"); //test case name
	// start EGL content server
	REglContentSession eglContentSession;
	TESTNOERRORL(eglContentSession.Connect());
	CleanupClosePushL(eglContentSession);

	// render few frames synchronized with egl content
	for(TInt frame=0; frame<10; frame++)
		{
		ResetGc();

		DrawDirectGdiCheckboardToTarget();
		TESTNOERRORL(iGc->GetError());

		// get egl content sgimage id
		TSgDrawableId imageId;
		TESTNOERRORL(eglContentSession.GetSyncImage(imageId));

		DrawImageToTargetL(imageId);
		TESTNOERRORL(iGc->GetError());

		DrawDirectGdiBarsToTarget();
		TESTNOERRORL(iGc->GetError());

		// save target
		TBuf<30> testName;
		testName.Append(KTestName);
		testName.AppendNum(frame);
		TESTNOERRORL(CTDirectGdiStepBase::WriteTargetOutput(iTestParams, testName));
		}

	CleanupStack::PopAndDestroy(&eglContentSession);
	}

/**
@SYMTestCaseID  	
	GRAPHICS-DIRECTGDI-INTERLEAVING-0002
	
@SYMPREQ 			
	PREQ39

@SYMREQ
	REQ9188 
	REQ9189 
	REQ9196 
	REQ9213 
	REQ9227 
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237
	
@SYMTestCaseDesc	
	Test asynchronous drawing of egl content
	
@SYMTestPriority	
	High
	
@SYMTestStatus 		
	Implemented
	
@SYMTestActions 	Establish connection to EglContentServer.
					Render frame in following way:
					 - Draw multicolor checkboard to gdi target
					 - Get EGL content sgimage id and frame number from EglContentServer in asynchronous mode. 
					 - Using obtained id create DirectGdi image source and draw it on target with  DrawResource() method.
					 - Draw semi-transparent vertical bars to gdi target.
					 - Find currently created target bitmap in stored bitmaps array. 
					   Use frame number obtained from EglContentServer as an array index. 
					   If it not exists in this array, store it. Otherwise  compare it with stored one pixel by pixel. 
					   Test fails when this comparison fails.
					EglContentServer produce ten different frames, 
					so for proper testing repeat above steps at least fifty times.

@SYMTestExpectedResults Valid set of bitmaps should be created.
                        These bitmaps should be the same as a reference bitmaps.
*/
void CTInterleaving::TestAsyncL(TBool aDebug)
	{
	_LIT(KTestName, "Interleaving-Async"); //test case name

	// start EGL content server
	REglContentSession eglContentSession;
	TESTNOERRORL(eglContentSession.Connect());
	CleanupClosePushL(eglContentSession);

	// prepare array to store frames
	RPointerArray<CFbsBitmap> storedBitmaps(KEglContentAsyncMaxFrames);
	CleanupResetAndDestroyPushL(storedBitmaps);
	for(TInt i=0; i<KEglContentAsyncMaxFrames; i++)
		{
		storedBitmaps.AppendL(NULL);
		}

	// prepare line buffers for bitmap comparision
	TInt bufSize = iGdiTarget->SizeInPixels().iWidth*4; // buffer is 32-bit BGRA
	HBufC8* lineBuf1 = HBufC8::NewLC(bufSize);
	HBufC8* lineBuf2 = HBufC8::NewLC(bufSize);

	// With the EGL context continuously being generated, we cannot guarantee that
	// all the frames will be retrieved.
	// However, in theory, the probability of picking each frame can be
	// approximated to a uniform discrete distribution.
	// So we can work out how many times we need to pick a frame to get all
	// the unique frames 99% of the time.
	// This works out to be approximately 65 times for 10 unique frames.
	const TInt KNumTriesToGetAllFrames = 65;

	if(!aDebug)
		{
		WARN_PRINTF1(_L("Note: Due to the nature of the test, it may be possible that not all the frames are captured"));
		}
	TInt64 seed = (TInt64)User::FastCounter();

	// render few frames asynchronous from egl content
	for(TInt frame=0; frame<KNumTriesToGetAllFrames; frame++)
		{
		ResetGc();

		DrawDirectGdiCheckboardToTarget();
		TESTNOERRORL(iGc->GetError());

		// get egl content sgimage id
		TSgDrawableId imageId;
		TInt fnum;
		if(!aDebug)
			{
			TESTNOERRORL(eglContentSession.GetAsyncImage(imageId, fnum));
			// Due to the nature of the asynchronous test, where the frames are
			// continuously being rendered and we can only dip in and pick the last
			// rendered frame, it cannot be guaranteed that all the unique frames will
			// be retrieved.  This could be due to this loop and the EGL content
			// generation running in harmony.  To try to overcome this, introduce
			// a random delay between 0 and 3 frames (0 seconds to 100 milliseconds - 
			// EGL content generated at 30fps).
			TInt delay = Math::Rand(seed)%100000;
			User::After(delay);
			}
		else
			{
			TESTNOERRORL(eglContentSession.GetAsyncImageDebug(imageId, fnum));
			}

		DrawImageToTargetL(imageId);
		TESTNOERRORL(iGc->GetError());

		DrawDirectGdiBarsToTarget();
		TESTNOERRORL(iGc->GetError());

		iGdiTarget->Finish();
		CFbsBitmap* targetBitmap = iGdiTarget->GetTargetFbsBitmapL();

		if(storedBitmaps[fnum] != NULL)
			{
			// compare with previously stored bitmap
			TUint32 cmpMask; // BGRA mask
			if((iTestParams.iTargetPixelFormat == EUidPixelFormatXRGB_8888) ||
				(iTestParams.iTargetPixelFormat == EUidPixelFormatXRGB_4444))
				{
				cmpMask = 0xffffff00; // exclude unused pixel part from comparision
				}
			else
				{
				cmpMask = 0xffffffff;
				}

			TInt width = targetBitmap->SizeInPixels().iWidth;
			TInt height = targetBitmap->SizeInPixels().iHeight;

			TPtr8 linePtr1(lineBuf1->Des());
			TPtr8 linePtr2(lineBuf2->Des());

			TBool pass = ETrue;

			// compare pixel by pixel
			for(TInt line=0; line<height; line++)
				{
				targetBitmap->GetScanLine(linePtr1, TPoint(0, line), width, EColor16MA);
				storedBitmaps[fnum]->GetScanLine(linePtr2, TPoint(0, line), width, EColor16MA);

				const TUint32* pPtr1 = (const TUint32*)linePtr1.Ptr();
				const TUint32* pPtr2 = (const TUint32*)linePtr2.Ptr();
				for(TInt x=0; x<width; x++)
					{
					if((pPtr1[x] & cmpMask) != (pPtr2[x] & cmpMask))
						{
						pass = EFalse;
						break; // break inner loop
						}
					}

				if(!pass)
					break; // break outer loop if test failed in inner loop
				}

			if(!pass)
				{
				INFO_PRINTF2(_L("Frame %d not equal to previous one"), fnum);
				TEST(pass);
				}
			}
		else
			{
			// copy and store target bitmap
			storedBitmaps[fnum] = new(ELeave) CFbsBitmap();
			TESTL(storedBitmaps[fnum]->Create(targetBitmap->SizeInPixels(), targetBitmap->DisplayMode()) == KErrNone);
		    Mem::Copy((TUint8*)storedBitmaps[fnum]->DataAddress(),
		    		(TUint8*)targetBitmap->DataAddress(),
		    		storedBitmaps[fnum]->DataStride()*storedBitmaps[fnum]->SizeInPixels().iHeight);

			// save target
			TBuf<40> testName;
			testName.Append(KTestName);
			if(aDebug)
				{
				_LIT(KDebug, "Debug");
				testName.Append(KDebug);
				}
			testName.AppendNum(fnum);
			TESTNOERRORL(CTDirectGdiStepBase::WriteTargetOutput(iTestParams, testName));
			}
		}

	CleanupStack::PopAndDestroy(4, &eglContentSession);
	}

/**
Draw multicolor checkboard to gdi target.
*/
void CTInterleaving::DrawDirectGdiCheckboardToTarget()
	{
	const TInt width = iGdiTarget->SizeInPixels().iWidth;
	const TInt height = iGdiTarget->SizeInPixels().iHeight;
	const TInt rwidth = 13;
	const TInt rheight = 13;

	iGc->SetPenStyle(DirectGdi::ENullPen);
	iGc->SetBrushStyle(DirectGdi::ESolidBrush);

	TInt color = 0;
	for(TInt y=0; y<height; y+=rheight)
		{
		for(TInt x=0; x<width; x+=rwidth)
			{
			iGc->SetBrushColor(KColor16Table[color%16]);
			color++;
			iGc->DrawRect(TRect(TPoint(x, y), TSize(rwidth, rheight)));
			}
		}
	}

/**
Draw semi-transparent vertical bars to gdi target.
*/
void CTInterleaving::DrawDirectGdiBarsToTarget()
	{
	TPoint pos(40, 40);
	TSize size(iGdiTarget->SizeInPixels());
	size -= TSize(80, 80);
	TSize barSize(size.iWidth/10, size.iHeight);

	iGc->SetPenStyle(DirectGdi::ENullPen);
	iGc->SetBrushStyle(DirectGdi::ESolidBrush);

	for(TInt i=0; i<size.iWidth; i+=barSize.iWidth)
		{
		TRgb color(255, 0, 0, 25*i/barSize.iWidth);
		iGc->SetBrushColor(color);
		iGc->DrawRect(TRect(pos+TPoint(i, 0), barSize));
		}
	}

/**
Draw four rotated EGL images to gdi target.
*/
void CTInterleaving::DrawImageToTargetL(TSgDrawableId aImageId)
	{
	// prepare sgimage from id
	RSgImage image;	
	TESTNOERRORL(image.Open(aImageId));
	CleanupClosePushL(image);

	TESTNOERRORL(CDirectGdiDriver::Open());

	CDirectGdiDriver* dgdiDriver = CDirectGdiDriver::Static();
	TESTL(dgdiDriver != NULL);	
	CleanupClosePushL(*dgdiDriver);	

	// prepare DirectGdi image source
	RDirectGdiDrawableSource source(*dgdiDriver);
	TESTNOERRORL(source.Create(image));
	CleanupClosePushL(source);

	// draw sgimage resorce on gdi target
	TDrawableSourceAndEquivRotatedBmps imageSource;
	imageSource.iDrawableSrc = &source;
	TPoint pos(20, 20);
	TSize size(iGdiTarget->SizeInPixels());
	size -= TSize(40, 40);
	size.iWidth /= 2;
	size.iHeight /= 2;
	TSize size2(size);
	size2 -= TSize(10, 10);

	iGc->DrawResource(TRect(pos, size2), imageSource);
	TESTNOERRORL(iGc->GetError());
	iGc->DrawResource(TRect(pos+TPoint(size.iWidth, 0), size2), imageSource, DirectGdi::EGraphicsRotation90);
	TESTNOERRORL(iGc->GetError());
	iGc->DrawResource(TRect(pos+TPoint(0, size.iHeight), size2), imageSource, DirectGdi::EGraphicsRotation180);
	TESTNOERRORL(iGc->GetError());
	iGc->DrawResource(TRect(pos+size.AsPoint(), size2), imageSource, DirectGdi::EGraphicsRotation270);
	TESTNOERRORL(iGc->GetError());

	CleanupStack::PopAndDestroy(3, &image);
	}

/**
Override of base class virtual
@leave Gets system wide error code
@return - TVerdict code
*/
TVerdict CTInterleaving::doTestStepPreambleL()
	{
	CTDirectGdiStepBase::doTestStepPreambleL();	
	return TestStepResult();
	}
	
/** 
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.
@leave Gets system wide error code
@return TVerdict code
*/	
TVerdict CTInterleaving::doTestStepL()
	{
	// Test for each target pixel format
	for(TInt targetPixelFormatIndex = iTargetPixelFormatArray.Count() - 1; targetPixelFormatIndex >= 0 ; targetPixelFormatIndex--)
		{
		iTestParams.iTargetPixelFormat = iTargetPixelFormatArray[targetPixelFormatIndex];
			
		SetTargetL(iTestParams.iTargetPixelFormat, EOneContextOneTarget, TSize(400, 400));
		RunTestsL();
		}
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

/**
Override of base class virtual
@leave Gets system wide error code
@return - TVerdict code
*/
TVerdict CTInterleaving::doTestStepPostambleL()
	{
	CTDirectGdiStepBase::doTestStepPostambleL();
	return TestStepResult();
	}

/**
Override of base class pure virtual
Lists the tests to be run
*/
void CTInterleaving::RunTestsL()
	{
	if(iUseDirectGdi)
		{
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-INTERLEAVING-0001"));
		TestSyncL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-INTERLEAVING-0002"));
		TestAsyncL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-INTERLEAVING-0002"));
		TestAsyncL(ETrue);
		RecordTestResultL();
		}
	}
