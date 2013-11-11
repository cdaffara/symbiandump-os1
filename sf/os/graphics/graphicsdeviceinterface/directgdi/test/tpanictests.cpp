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

#include "tpanictests.h"
#include <graphics/directgdicontext.h>
#include <graphics/directgdiimagetarget.h>

_LIT(KPanicCategory, "DGDI");

CTPanicTests::CTPanicTests()
	{
	SetTestStepName(KTDirectGdiPanicTestsStep);
	}

CTPanicTests::~CTPanicTests()
	{
	}

/**
@SYMTestCaseID		
	GRAPHICS-DIRECTGDI-PANIC-0001

@SYMTestPriority
	Medium

@SYMPREQ
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Tests if DirectGDI methods panic when called on a inactivated context.

@SYMTestActions		
	Construct CDirectGdiDriver.
	Construct CDirectGdiContext.
	Construct RSgImage.
	Construct MDirectGdiImageTarget. 
	Create MDirectGdiImageTarget, using RSgImage.

	Do not Activate CDirectGdiContext.
	Call any API on the context.

	Destroy RSgImage.
	Destroy MDirectGdiImageTarget.
	Destroy CDirectGdiContext.
	Close CDirectGdiDriver.

@SYMTestExpectedResults
	It should panic with panic code DGDI 7, EDirectGdiPanicContextNotActivated
	
@SYMTestStatus
	Implemented
*/
void CTPanicTests::TestContextNotActivatedL()
	{
	if (iUseDirectGdi)
		{
		TInt result = CDirectGdiDriver::Open();
		TESTNOERRORL(result);
		
		CDirectGdiDriver* dgdiDriver = CDirectGdiDriver::Static();		
		TEST(dgdiDriver != NULL);
		CleanupClosePushL(*dgdiDriver);
		
		CDirectGdiContext* gc = CDirectGdiContext::NewL(*dgdiDriver);		
	
		RSgImage* rsgImage1 = new RSgImage();
		TESTL(rsgImage1 != NULL);
		
		// Set the bitmap up...
		TSgImageInfo imageInfo;
		imageInfo.iSizeInPixels = TSize (320, 240);
		imageInfo.iPixelFormat = iTestParams.iTargetPixelFormat;

		imageInfo.iUsage = ESgUsageDirectGdiTarget;
		result = rsgImage1->Create(imageInfo, NULL,0);
		TESTNOERRORL(result);
		
		RDirectGdiImageTarget* dgdiImageTarget1 = new RDirectGdiImageTarget(*dgdiDriver);
		TESTL(dgdiImageTarget1 != NULL);
		result = dgdiImageTarget1->Create(*rsgImage1);
		TESTNOERRORL(result);
		
		TRect rect(10, 15, 100, 100);
		gc->DrawEllipse(rect);
		
		dgdiImageTarget1->Close();	
		delete(dgdiImageTarget1);
		rsgImage1->Close();
		delete(rsgImage1);
		delete(gc);
		CleanupStack::PopAndDestroy(1);
		}
	else //BitGDI
		User::Panic(KPanicCategory, 7);
	}

/**
@SYMTestCaseID	
	GRAPHICS-DIRECTGDI-PANIC-0002

@SYMTestPriority
	Medium

@SYMPREQ
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Testing if a panic is raised during text drawing functions usage without valid font set.

@SYMTestActions
	Context is created and activated.

	Set font in context.
	Call any DrawText() API on the context.

	Reset font in context.
	Call any DrawText() API on the context.

@SYMTestExpectedResults
	It should panic with panic code DGDI 11, EDirectGdiPanicNoFontSelected.

@SYMTestStatus
	Implemented
*/

void CTPanicTests::TestFontNotSetL()
	{
	if(iUseDirectGdi)
		{
		_LIT(KText, "test");

		ResetGc();

		CFont* font = GetFont();
		TESTL(font != NULL);

		iGc->SetPenColor(TRgb(0, 0, 0));

		iGc->SetFont(font);
		iGc->DrawText(KText, NULL, TPoint(10, 30));
	
		iGc->ResetFont();
		iGc->DrawText(KText, NULL, TPoint(10, 50));
	
		ReleaseFont(font);
		}
	else //BitGDI
		User::Panic( KPanicCategory, 11);
	}

/**
@SYMTestCaseID		
	GRAPHICS-DIRECTGDI-PANIC-0003

@SYMTestPriority
	Medium

@SYMPREQ
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Tests the negative conditions that can only be reached by creating RSgImage with an invalid pixel type, to
	improve the code coverage.
	
@SYMTestActions		
	Construct CDirectGdiDriver.
	Construct CDirectGdiContext.

	Construct RSgImage with unsupported pixel type and do not check for the return error code.
	Construct MDirectGdiImageTarget.
	Create MDirectGdiImageTarget, using RSgImage, ignoring the return error code.

	Activate CDirectGdiContext using MDirectGdiImageTarget.
   
	Destroy RSgImage.
	Destroy MDirectGdiImageTarget.

	Destroy CDirectGdiContext.
	Close CDirectGdiDriver.

@SYMTestExpectedResults
	It should panic with panic code DGDI 21, EDirectGdiImageTargetInfoError
	
@SYMTestStatus
	Implemented
*/
void CTPanicTests::TestInvalidTargetL()
	{	
	if(iUseDirectGdi)
		{
		TInt err = CDirectGdiDriver::Open();
		TESTNOERROR(err);
		
		CDirectGdiDriver* dgdiDriver = CDirectGdiDriver::Static();	
		TESTL(dgdiDriver != NULL);
		CleanupClosePushL(*dgdiDriver);
		
		CDirectGdiContext* gc = CDirectGdiContext::NewL(*dgdiDriver);
		RSgImage rsgImage;
			
		// Set the bitmap up...
		TSgImageInfo imageInfo;
		imageInfo.iSizeInPixels = TSize (320, 240);
		imageInfo.iPixelFormat = EUidPixelFormatA_8;
		imageInfo.iUsage = ESgUsageDirectGdiTarget;
		rsgImage.Create(imageInfo, NULL,0);
		
		RDirectGdiImageTarget dgdiImageTarget(*dgdiDriver);
		dgdiImageTarget.Create(rsgImage);	
		gc->Activate (dgdiImageTarget);
			
		rsgImage.Close();
		dgdiImageTarget.Close();	
		delete gc;
		CleanupStack::PopAndDestroy(1);
		}
	else //BitGDI
		User::Panic( KPanicCategory, 21);
	}

/**
@SYMTestCaseID		
	GRAPHICS-DIRECTGDI-PANIC-0004

@SYMTestPriority
	Medium

@SYMPREQ
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Test that the adapter panics when an attempt is made to activate a target with an invalid handle.

@SYMTestActions		
	Create an RSgImage.
	Make it a target for the driver.
	Change the target's handle so it has an erroneous handle.
	Attempt to activate the target.
	(This test only works in _DEBUG mode as the handle check only happens in _DEBUG mode)

@SYMTestExpectedResults
	The test should panic when the target is activated DGDIAdapter 32, EDirectGdiPanicResourceHandleNotFound.
  
@SYMTestStatus
	Implemented
*/
void CTPanicTests::TestImageInvalidTargetHandleL()
	{
#ifdef _DEBUG
	TInt err = CDirectGdiDriver::Open();
	TESTNOERROR(err);
	
	CDirectGdiDriver* dgdiDriver = CDirectGdiDriver::Static();	
	TESTL(dgdiDriver != NULL);
	CleanupClosePushL(*dgdiDriver);
	
	CDirectGdiContext* gc = CDirectGdiContext::NewL(*dgdiDriver);
	RSgImage rsgImage;
		
	// Set the bitmap up...
	TSgImageInfo imageInfo;
	imageInfo.iSizeInPixels = TSize (320, 240);
	imageInfo.iPixelFormat = iTestParams.iTargetPixelFormat;
	imageInfo.iUsage = ESgUsageDirectGdiTarget;
	rsgImage.Create(imageInfo, NULL,0);
	
	RDirectGdiImageTarget dgdiImageTarget(*dgdiDriver);
	dgdiImageTarget.Create(rsgImage);	
	
	// Set the target's handle to a non-null invalid handle.
	dgdiImageTarget.iHandle = 0x12345678;
	
	// Activate should panic (DGDIAdapter EDirectGdiTargetHandleNotFound 32)
	gc->Activate (dgdiImageTarget);
		
	rsgImage.Close();
	dgdiImageTarget.Close();	
	delete gc;
	CleanupStack::PopAndDestroy(1);
#else
	User::Panic(KPanicCategory, 32);
#endif
	}

/**
@SYMTestCaseID		
	GRAPHICS-DIRECTGDI-PANIC-0005

@SYMTestPriority
	Medium

@SYMPREQ
	PREQ39

@SYMTestCaseDesc
	Tests the negative conditions that the same RDirectGdiImageTarget object
	is created twice.
	
@SYMTestActions		
	Construct CDirectGdiDriver.
	Construct RDirectGdiImageTarget twice.
  
@SYMTestExpectedResults
	It should panic with panic code DGDIAdapter 17, EDirectGdiImageTargetAlreadyExists
	
@SYMTestStatus
	Implemented
*/
void CTPanicTests::TestImageTargetActivatedTwiceL()
	{
	if(iUseDirectGdi)
		{
		TInt err = CDirectGdiDriver::Open();
		TESTNOERRORL(err);			
		CDirectGdiDriver* dgdiDriver = CDirectGdiDriver::Static();
		TESTL(dgdiDriver != NULL);	
		CleanupClosePushL(*dgdiDriver);	
		
		TSgImageInfo info2;
		info2.iSizeInPixels = TSize(8, 8);
		info2.iUsage = ESgUsageDirectGdiTarget;
		info2.iPixelFormat = EUidPixelFormatRGB_565;
		info2.iCpuAccess = ESgCpuAccessNone;
		info2.iShareable = ETrue;
		RSgImage image2;
		User::LeaveIfError(image2.Create(info2, NULL, 0));
		CleanupClosePushL(image2);
		
		RDirectGdiImageTarget dgdiImageTarget(*dgdiDriver);
		TESTNOERRORL(dgdiImageTarget.Create(image2));
		CleanupClosePushL(dgdiImageTarget);
		dgdiImageTarget.Create(image2); //should panic here
		CleanupClosePushL(dgdiImageTarget);
		
		CleanupStack::PopAndDestroy(4, dgdiDriver);	
		}
	else //BitGDI
		{
		User::Panic( KPanicCategory, 17);
		}
	}

/**
@SYMTestCaseID		
	GRAPHICS-DIRECTGDI-PANIC-0006

@SYMTestPriority
	Medium

@SYMPREQ
	PREQ39

@SYMTestCaseDesc
	Tests the negative conditions that the same RDirectGdiDrawableSource object
	is created twice.
	
@SYMTestActions		
	Construct CDirectGdiDriver.
	Construct RDirectGdiDrawableSource twice.
  
@SYMTestExpectedResults
	It should panic with panic code DGDI 18, EDirectGdiImageSourceAlreadyExists
	
@SYMTestStatus
	Implemented
*/
void CTPanicTests::TestImageSourceActivatedTwiceL()
	{
	if(iUseDirectGdi)
		{
		TInt err = CDirectGdiDriver::Open();
		TESTNOERRORL(err);			
		CDirectGdiDriver* dgdiDriver = CDirectGdiDriver::Static();
		TESTL(dgdiDriver != NULL);	
		CleanupClosePushL(*dgdiDriver);	
		
		TSgImageInfo info1;
		info1.iSizeInPixels = TSize(8, 8);
		info1.iUsage = ESgUsageDirectGdiSource;
		info1.iPixelFormat = EUidPixelFormatRGB_565;
		info1.iCpuAccess = ESgCpuAccessReadWrite;
		info1.iShareable = ETrue;	
		RSgImage image1;
		User::LeaveIfError(image1.Create(info1, NULL, 0));
		CleanupClosePushL(image1);
		
		RDirectGdiDrawableSource dgdiImageSource(*dgdiDriver);
		TESTNOERRORL(dgdiImageSource.Create(image1));
		CleanupClosePushL(dgdiImageSource);
		dgdiImageSource.Create(image1); //should panic here
		CleanupClosePushL(dgdiImageSource);
		
		CleanupStack::PopAndDestroy(4, dgdiDriver);
		}
	else //BitGDI
		{
		User::Panic( KPanicCategory, 18);
		}
	}

/**
@SYMTestCaseID		
	GRAPHICS-DIRECTGDI-PANIC-0007

@SYMTestPriority
	Medium

@SYMPREQ
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Tests the negative conditions that the same RDirectGdiDrawableSource object
	is created twice.
	
@SYMTestActions		
	Construct CDirectGdiDriver.
	Construct RDirectGdiDrawableSource twice.
  
@SYMTestExpectedResults
	It should panic with panic code DGDIAdapter 19, EDirectGdiDrawableSourceAlreadyExists
	
@SYMTestStatus
	Implemented
*/
void CTPanicTests::TestDrawableSourceActivatedTwiceL()
	{
	if(iUseDirectGdi)
		{
		TInt err = CDirectGdiDriver::Open();
		TESTNOERRORL(err);			
		CDirectGdiDriver* dgdiDriver = CDirectGdiDriver::Static();
		TESTL(dgdiDriver != NULL);	
		CleanupClosePushL(*dgdiDriver);	
		
		TSgImageInfo info1;
		info1.iSizeInPixels = TSize(8, 8);
		info1.iUsage = ESgUsageDirectGdiSource;
		info1.iPixelFormat = EUidPixelFormatARGB_8888_PRE;
		info1.iCpuAccess = ESgCpuAccessReadWrite;
		info1.iShareable = ETrue;	
		RSgImage image1;
		TESTNOERRORL(image1.Create(info1, NULL, 0));
		CleanupClosePushL(image1);
		
		RDirectGdiDrawableSource dgdiDrawableSource(*dgdiDriver);
		TESTNOERRORL(dgdiDrawableSource.Create(image1));
		CleanupClosePushL(dgdiDrawableSource);
		dgdiDrawableSource.Create(image1); //should panic here
		CleanupClosePushL(dgdiDrawableSource);
		
		CleanupStack::PopAndDestroy(4, dgdiDriver);
		}
	else //BitGDI
		{
		User::Panic( KPanicCategory, 19);
		}
	}

/**
@SYMTestCaseID		
	GRAPHICS-DIRECTGDI-PANIC-0008
	
@SYMTestPriority
	Medium

@SYMPREQ
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Make sure a panic occurs when calling SetBrushStyle() with EPatternedBrush when no brush pattern has been set.
	
@SYMTestActions		
 	Set brush style to EPatternedBrush.
  
@SYMTestExpectedResults
	It should panic with panic code DGDI 9, EDirectGdiBrushPatternNotSet.
	
@SYMTestStatus
	Implemented
*/
void CTPanicTests::TestBrushPatternNotSetL()
	{
	if(iUseDirectGdi)
		{
		ResetGc();
		iGc->SetBrushStyle(DirectGdi::EPatternedBrush);		
		}
	else //BitGDI
		User::Panic( KPanicCategory, 9);
	}

/**
Override of base class virtual
@leave Gets system wide error code
@return - TVerdict code
*/
TVerdict CTPanicTests::doTestStepPreambleL()
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
TVerdict CTPanicTests::doTestStepL()
	{	
	// Test for each pixel format
	for(TInt targetPixelFormatIndex = iTargetPixelFormatArray.Count() - 1; targetPixelFormatIndex >= 0 ; targetPixelFormatIndex--)
		{
		iTestParams.iTargetPixelFormat = iTargetPixelFormatArray[targetPixelFormatIndex];
		SetTargetL(iTestParams.iTargetPixelFormat);
		RunTestsL();
		RunOomTestsL();
		}

	return TestStepResult();
	}

/**
Override of base class pure virtual
Lists the tests to be run
*/
void CTPanicTests::RunTestsL()
	{
	INFO_PRINTF1(_L("DirectGdi Panic Tests" ));
	TInt aCurTestCase;
	//Read the case number from the ini file
	TBool res = GetIntFromConfig(ConfigSection(), KCaseNumber, aCurTestCase);		
	if(!res)
		{
		return ;
		}			
	SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{			
		case 1:
			INFO_PRINTF1(_L("TestContextNotActivatedL\n"));
			SetTestStepID(_L("GRAPHICS-DIRECTGDI-PANIC-0001"));
			TestContextNotActivatedL();
			break;
		case 2:
			INFO_PRINTF1(_L("TestFontNotSetL\r\n"));
			SetTestStepID(_L("GRAPHICS-DIRECTGDI-PANIC-0002"));
			TestFontNotSetL();
			break;
		case 3:
			INFO_PRINTF1(_L("TestInvalidTargetL\r\n"));
			SetTestStepID(_L("GRAPHICS-DIRECTGDI-PANIC-0003"));
			TestInvalidTargetL();
			break;
		case 4:
			INFO_PRINTF1(_L("TestImageInvalidTargetHandleL\r\n"));
			SetTestStepID(_L("GRAPHICS-DIRECTGDI-PANIC-0004"));
			TestImageInvalidTargetHandleL();
			break;
		case 5:
			INFO_PRINTF1(_L("TestImageTargetActivatedTwiceL\r\n"));
			SetTestStepID(_L("GRAPHICS-DIRECTGDI-PANIC-0005"));
			TestImageTargetActivatedTwiceL();
			break;	
		case 6:
			INFO_PRINTF1(_L("TestImageSourceActivatedTwiceL\r\n"));
			SetTestStepID(_L("GRAPHICS-DIRECTGDI-PANIC-0006"));
			TestImageSourceActivatedTwiceL();
			break;
		case 7:
			INFO_PRINTF1(_L("TestDrawableSourceActivatedTwiceL\r\n"));
			SetTestStepID(_L("GRAPHICS-DIRECTGDI-PANIC-0007"));
			TestDrawableSourceActivatedTwiceL();
			break;
		case 8:
			INFO_PRINTF1(_L("TestBrushPatternNotSetL\r\n"));
			SetTestStepID(_L("GRAPHICS-DIRECTGDI-PANIC-0008"));
			TestBrushPatternNotSetL();
			break;
		}
	RecordTestResultL();
	}
