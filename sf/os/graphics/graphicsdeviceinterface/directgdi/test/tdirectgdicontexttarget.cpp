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

#include "tdirectgdicontexttarget.h"
#include <graphics/directgdicontext.h>

CTDirectGdiContextTarget::CTDirectGdiContextTarget()
	{
	SetTestStepName(KTDirectGdiContextTargetStep);
	}

CTDirectGdiContextTarget::~CTDirectGdiContextTarget()
	{
	}

/**
@SYMTestCaseID		
	GRAPHICS-DIRECTGDI-CONTEXTTARGET-0001

@SYMTestPriority
	Critical

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
	Use case - Single context and multiple targets.

@SYMTestActions		
	Construct CDirectGdiDriver.
	Construct CDirectGdiContext.

	Construct RSgImage1.
	Construct MDirectGdiImageTarget1. 
 	Create MDirectGdiImageTarget1, using RSgImage1.

	Activate CDirectGdiContext using MDirectGdiImageTarget1.
	Do some drawing operations using a different pen/brush setting
  
	Construct RSgImage2.
	Construct MDirectGdiImageTarget2. 
	Create MDirectGdiImageTarget2, using RSgImage2.

	Activate CDirectGdiContext using MDirectGdiImageTarget2.
	Do some drawing operations using a different pen/brush setting
  
	Destroy RSgImage.
	Destroy MDirectGdiImageTarget.

	Destroy CDirectGdiContext.
	Close CDirectGdiDriver.	

@SYMTestExpectedResults
	It should be able to switch the targets between the same context.
	
@SYMTestStatus 		
	Implemented
*/
void CTDirectGdiContextTarget::TestSwitchingTargetsL()
	{
	TInt result = KErrNone;
	_LIT(KTestName, "ContextTarget-SwitchingTargets"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	if (iTargetPixelFormatArray.Count() <= 1)
		{
		// The MBX driver only supports one pixelformat. This test has no purpose
		// when both targets are the same, so in this case we don't run it.
		INFO_PRINTF1(_L("Not enough formats supported. Skipping test..."));
		return;
		}
	
	TInt err = CDirectGdiDriver::Open();
	TESTNOERRORL(err);
			
	CDirectGdiDriver* dgdiDriver = CDirectGdiDriver::Static();
	TESTL(dgdiDriver != NULL);	
	CleanupClosePushL(*dgdiDriver);	
	
	CDirectGdiContext* gc = CDirectGdiContext::NewL(*dgdiDriver);	
	CleanupStack::PushL(gc);
	
	RSgImage rsgImage1;	
	RSgImage rsgImage2;	
	
	// Set the bitmap up...
	TSgImageInfo imageInfo;
	imageInfo.iSizeInPixels = TSize (320, 240);
	imageInfo.iPixelFormat = iTargetPixelFormatArray[0];
	imageInfo.iUsage = ESgUsageDirectGdiTarget;
	result = rsgImage1.Create(imageInfo, NULL,0);
	TESTNOERRORL(result);
	CleanupClosePushL(rsgImage1);
	
	imageInfo.iSizeInPixels = TSize (300, 300);
	imageInfo.iPixelFormat =  iTargetPixelFormatArray[1];
	result = rsgImage2.Create(imageInfo, NULL,0);
	TESTNOERRORL(result);
	CleanupClosePushL(rsgImage2);	

	RDirectGdiImageTarget dgdiImageTarget1(*dgdiDriver);	
	result = dgdiImageTarget1.Create(rsgImage1);
	TESTNOERRORL(result);
	CleanupClosePushL(dgdiImageTarget1);

	result = gc->Activate (dgdiImageTarget1);
	TESTNOERRORL(result);
	gc->SetPenColor(TRgb(100,100,100));
	TEST(gc->PenColor() == TRgb(100,100,100));
	gc->DrawEllipse(TRect(0,0,30,30));
	TESTNOERROR(dgdiDriver->GetError());

	RDirectGdiImageTarget dgdiImageTarget2(*dgdiDriver);	
	result = dgdiImageTarget2.Create(rsgImage1);
	TESTNOERRORL(result);
	CleanupClosePushL(dgdiImageTarget2);

	result = gc->Activate (dgdiImageTarget2);
	TESTNOERRORL(result);
	gc->SetPenColor(TRgb(0,0,0));
	TEST(gc->PenColor() == TRgb(0,0,0));	
	gc->DrawEllipse(TRect(0,0,30,30));
	TESTNOERROR(dgdiDriver->GetError());		

	CleanupStack::PopAndDestroy(6, dgdiDriver); //dgdiImageTarget1&2, rsgImage1&2, gc, dgdiDriver
	
	}

/**
@SYMTestCaseID		
	GRAPHICS-DIRECTGDI-CONTEXTTARGET-0002

@SYMTestPriority
	Critical

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
	Test the integrity of RSgImage metadata after being written to.

@SYMTestActions		
	Construct CDirectGdiDriver.
	Construct CDirectGdiContext.
	Construct RSgImage.
	Construct MDirectGdiImageTarget. 
	Create MDirectGdiImageTarget1, using RSgImage.
	Check the current RSgImage info matches that at creation time.
	Activate CDirectGdiContext using MDirectGdiImageTarget.
	Do some drawing operations on the target.
	Check the current RSgImage info matches that at creation time.  
	Destroy RSgImage.
	Destroy MDirectGdiImageTarget.
	Destroy CDirectGdiContext.
	Close CDirectGdiDriver.

@SYMTestExpectedResults
	The RSgImage info should be the same at all times.
  
@SYMTestStatus 		
	Implemented
*/
void CTDirectGdiContextTarget::TestRSgImageMetadataIntegrityL()
	{
	_LIT(KTestName, "ContextTarget-RSgImageMetadataIntegrity"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	TInt err = CDirectGdiDriver::Open();
	TESTNOERRORL(err);			
	CDirectGdiDriver* dgdiDriver = CDirectGdiDriver::Static();
	TESTL(dgdiDriver != NULL);	
	CleanupClosePushL(*dgdiDriver);	
	CDirectGdiContext* gc = CDirectGdiContext::NewL(*dgdiDriver);
	CleanupStack::PushL(gc);
	
	// Create RSgImage, initialise some info.
	RSgImage rsgImage;	
	TSgImageInfo imageInfo;
	imageInfo.iSizeInPixels = TSize (320, 240);
	imageInfo.iPixelFormat = iTestParams.iTargetPixelFormat;
	imageInfo.iUsage = ESgUsageDirectGdiTarget;
	TInt result = rsgImage.Create(imageInfo, NULL,0);
	TESTNOERRORL(result);
	CleanupClosePushL(rsgImage);
	
	// Test that after creation, the info is still the same.
	TSgImageInfo retrievedInfo;
	TESTNOERRORL(rsgImage.GetInfo(retrievedInfo));
	TBool match = CompareInfos(retrievedInfo, imageInfo);
	TEST(match);
	
	// Perform writing to the target.
	RDirectGdiImageTarget dgdiImageTarget(*dgdiDriver);	
	TESTNOERRORL(dgdiImageTarget.Create(rsgImage));
	CleanupClosePushL(dgdiImageTarget);
	TESTNOERRORL(gc->Activate (dgdiImageTarget) );
	gc->SetBrushColor(TRgb(255,0,0,255));
	gc->Clear();
	dgdiDriver->Finish();
	
	// Test that after being written to, the info is still the same. If this fails, 
	// an indication that image data is being written over the metadata.
	TESTNOERRORL(rsgImage.GetInfo(retrievedInfo));
	match = CompareInfos(retrievedInfo, imageInfo);
	TEST(match);
	
	CleanupStack::PopAndDestroy(4); //dgdiImageTarget, rsgImage, gc, dgdiDriver	
	}

/**
@SYMTestCaseID		
	GRAPHICS-DIRECTGDI-CONTEXTTARGET-0003

@SYMTestPriority
	Critical

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
	Test that the RdirectGdiImageTarget::Create() and RDirectGdiDrawableSource::Create() methods 
	validate their arguments.
	- Test that creation of a target from an RSgImage that was created as a source fails.
	- Test that creation of a source from an RSgImage that was created as a target fails.

@SYMTestActions		
	Construct CDirectGdiDriver.
	Construct CDirectGdiContext.
	Create a RSgImage 1 as a source.
	Create a RSgImage 2 as a target.
	Attempt to create RDirectGdiImageTarget from RSgImage 1 (a source).
	KErrNotSupported should be returned.
	Attempt to create RDirectGdiDrawableSource from RSgImage 2 (a target).
	KErrNotSupported should be returned.

@SYMTestExpectedResults
	Functions return the correct error code.
  
@SYMTestStatus 		
	Implemented
*/
void CTDirectGdiContextTarget::TestCreateCloseSourceTargetInvalidParametersL()
	{
	_LIT(KTestName, "ContextTarget-CreateCloseSourceTargetInvalidParameters"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	TInt err = CDirectGdiDriver::Open();
	TESTNOERRORL(err);			
	CDirectGdiDriver* dgdiDriver = CDirectGdiDriver::Static();
	TESTL(dgdiDriver != NULL);	
	CleanupClosePushL(*dgdiDriver);	
	
	CDirectGdiContext* gc = CDirectGdiContext::NewL(*dgdiDriver);	
	CleanupStack::PushL(gc);
	
	//Create an image with usage set to ESgUsageDirectGdiSource	
	TSgImageInfo info1;
	info1.iSizeInPixels = TSize(8, 8);
	info1.iUsage = ESgUsageDirectGdiSource;
	info1.iPixelFormat = EUidPixelFormatARGB_8888;
	info1.iCpuAccess = ESgCpuAccessReadWrite;
	info1.iShareable = ETrue;	
	RSgImage image1;
	TESTNOERRORL(image1.Create(info1, NULL, 0));
	CleanupClosePushL(image1);	
	
	//Create an image with usage set to ESgUsageDirectGdiTarget
	TSgImageInfo info2;
	info2.iSizeInPixels = TSize(8, 8);
	info2.iUsage = ESgUsageDirectGdiTarget;
	info2.iPixelFormat = EUidPixelFormatRGB_565;
	info2.iCpuAccess = ESgCpuAccessNone;
	info2.iShareable = ETrue;
	RSgImage image2;
	TESTNOERRORL(image2.Create(info2, NULL, 0));
	CleanupClosePushL(image2);
	
	//create a RDirectGdiImageTarget from an image with 
	//usage set to ESgUsageDirectGdiSource
	RDirectGdiImageTarget dgdiImageTarget(*dgdiDriver);	
	TESTL(KErrNotSupported == dgdiImageTarget.Create(image1));	
	
	//create a RDirectGdiDrawableSource from an image with
	//usage set to ESgUsageDirectGdiTarget	
	RDirectGdiDrawableSource dgdiImageSource(*dgdiDriver);
	TESTL(KErrNotSupported == dgdiImageSource.Create(image2));
	
	//create a RDirectGdiImageTarget from an image with 
	//usage set to ESgUsageDirectGdiTarget	
	//should succeed
	TESTNOERRORL(dgdiImageTarget.Create(image2));
	CleanupClosePushL(dgdiImageTarget);
	
	// Need to activate a target before we can create a source due to OpenVG limitation.
	// An OpenVG context must exist before a source can be created, but is not available
	// until a target is activated.
	gc->Activate(dgdiImageTarget);	

	//create a RDirectGdiDrawableSource from an image with
	//usage set to ESgUsageDirectGdiSource	
	//should succeed
	TESTNOERRORL(dgdiImageSource.Create(image1));
	CleanupClosePushL(dgdiImageSource);
	
	CleanupStack::PopAndDestroy(6, dgdiDriver);
	}

/** 
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.
@leave Gets system wide error code
@return TVerdict code
*/	
TVerdict CTDirectGdiContextTarget::doTestStepL()
	{
	if(iUseDirectGdi)
		{
		// Test for each pixel format
		for(TInt targetPixelFormatIndex = iTargetPixelFormatArray.Count() - 1; targetPixelFormatIndex >= 0 ; targetPixelFormatIndex--)
			{	
			iTestParams.iTargetPixelFormat = iTargetPixelFormatArray[targetPixelFormatIndex];
			RunTestsL();
			RunOomTestsL();			
			}
		}
	else
		{
		INFO_PRINTF1(_L("BitGDI has no RSgImage to test!"));
		}
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

/**
Override of base class virtual
@leave Gets system wide error code
@return - TVerdict code
*/
TVerdict CTDirectGdiContextTarget::doTestStepPreambleL()
	{			
	CTDirectGdiStepBase::doTestStepPreambleL();	
	return TestStepResult();
	}

/**
Override of base class pure virtual
Lists the tests to be run
*/
void CTDirectGdiContextTarget::RunTestsL()
	{
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-CONTEXTTARGET-0001"));
	TestSwitchingTargetsL();	
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-CONTEXTTARGET-0002"));
	TestRSgImageMetadataIntegrityL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-CONTEXTTARGET-0003"));
	TestCreateCloseSourceTargetInvalidParametersL();
	RecordTestResultL();
	}

/**
Helper function to test the equivalence of two TSgImageInfo structures.

@see     CTSgTestStepBase::CompareInfos
@param   info1 A TSgImageInfo structure to compare.
@param   info2 A TSgImageInfo structure to compare.

@return  ETrue if the two are identical, EFalse otherwise.
*/
TBool CTDirectGdiContextTarget::CompareInfos(const TSgImageInfo& info1, const TSgImageInfo& info2)
	{
	return info1.iCpuAccess == info2.iCpuAccess 
			&& info1.iPixelFormat == info2.iPixelFormat
			&& info1.iScreenId == info2.iScreenId
			&& info1.iShareable == info2.iShareable
			&& info1.iSizeInPixels == info2.iSizeInPixels
			&& info1.iUsage | info2.iUsage
			&& info1.iUserAttributeCount == info2.iUserAttributeCount;
	}
