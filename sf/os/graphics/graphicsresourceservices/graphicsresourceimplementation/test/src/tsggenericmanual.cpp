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
// Tests for manual execution.

/**
 @file
 @test
 @internalComponent - Graphics Resource API Conformance Test Suite
*/

#include "tsggenericmanual.h"

CTSgGenericManual::CTSgGenericManual(TBool aConformanceTests) :
	CTSgTestStepBase(aConformanceTests)
	{
	INFO_PRINTF1(_L("Graphics resource component test - Generic Manual Tests.\r\n"));
	}

CTSgGenericManual::~CTSgGenericManual()
	{
	iSecondProcess.Close();
	iMsgQ.Close();
	}

/**
This is intented to be used for TestStressResourceLeakL (GRAPHICS-RESOURCE-0050) test. 
It creates images until the memory full situation. The images are kept in the passed RArray of RSgImage. 
The returned error code is expected to be either KErrNoMemory or KErrNoGraphicsMemory.
Optionally, it opens and closes a duplicate handle to each image in the same process and in another process.
*/
TInt CTSgGenericManual::CreateImages(const TSgImageInfo& aInfo, RArray<RSgImage>& aTestImages, TBool aDuplicate)
    {
    TInt err = KErrNone;
    while(err == KErrNone)
        {
        RSgImage image;
        err = image.Create(aInfo);
        if(err == KErrNone)
            {
            err = aTestImages.Append(image);
            if (err != KErrNone)
            	{
				image.Close();
				return err;
            	}
            if (aDuplicate)
            	{
				RSgImage image2;
				err = image2.Open(image.Id());
				if (err != KErrNone)
					{
					return err;
					}
				// send the image ID to the second process and wait until the
				// second process has opened and closed a handle to the image
				TRequestStatus status;
				iSecondProcess.Rendezvous(status);
				iMsgQ.SendBlocking(image.Id());
				User::WaitForRequest(status);
				image2.Close();
				err = status.Int();
            	}
            }
        }
    return err;
    }

void CTSgGenericManual::DestroyImages(RArray<RSgImage>& aTestImages)
    {
    TInt count = aTestImages.Count();
    for(TInt i=0; i<count; ++i)
        {
        aTestImages[i].Close();
        }   
    aTestImages.Reset();    
    }

/** 
@leave Gets system wide error code
@return TVerdict code
*/
TVerdict CTSgGenericManual::doTestStepL()
	{	
    SetTestStepID(_L("GRAPHICS-RESOURCE-0050"));
    INFO_PRINTF1(_L("RSgImage generic resource leak stress test.\r\n"));
    TestStressResourceLeakL();
	RecordTestResultL();
	
	return TestStepResult();
	}

/**
@SYMTestCaseID          GRAPHICS-RESOURCE-0050
@SYMTestCaseDesc        RSgImage exhaustive and resource leak test
@SYMPREQ                PREQ2637
@SYMFssID               RSgImage::Create(const TSgImageInfo&, const TAny*, TInt)
                        RSgImage::Open(TSgDrawableId)
                        RSgImage::Close()
@SYMTestPriority        Medium
@SYMTestType            CT
@SYMTestPurpose         To ensure no resource leaks while creating and destroying RSgImage multiple times
@SYMTestActions         Create images until it returns no memory error. Close the created images and          
                        create as many images as possible until memory is full. Test the number of images
                        created and also for each iteration the number of images created to be the same.
@SYMTestExpectedResults There should be no panics or leaves.
 */
void CTSgGenericManual::TestStressResourceLeakL()
    {
    TestOpenDriverL();
    _LIT(KSection, "TestStressResourceLeak");
    TInt numIterations;
    if (!GetIntFromConfig(KSection, _L("NumIterations"), numIterations))
    	{
		numIterations = 2;
    	}
    TInt tolerance;
    if (!GetIntFromConfig(KSection, _L("Tolerance"), tolerance))
    	{
		tolerance = -1;
    	}
    TInt width;
    if (!GetIntFromConfig(KSection, _L("ImageWidth"), width))
    	{
		width = 1;
    	}
    TInt height;
    if (!GetIntFromConfig(KSection, _L("ImageHeight"), height))
    	{
		height = 1;
    	}
    TSgImageInfo info;
    info.iPixelFormat = EUidPixelFormatRGB_565;
    info.iSizeInPixels = TSize(width, height);
    info.iUsage = ESgUsageBitOpenVgImage;
    TBool duplicate;
    if (!GetBoolFromConfig(KSection, _L("DuplicateHandle"), duplicate))
    	{
		duplicate = EFalse;
    	}
    if (duplicate)
    	{
		User::LeaveIfError(iMsgQ.CreateGlobal(KSgTestMultiprocessMsgQ, 1));
		_LIT(KProcessName, "tgraphicsresourcemanualsecondprocess.exe");
		User::LeaveIfError(iSecondProcess.Create(KProcessName, KNullDesC));
		iSecondProcess.Resume();
    	}

    RArray<RSgImage> testImages;    
    
    TInt count(0);
    TInt err = KErrNone;
    for (TInt i = 0; i < numIterations && err == KErrNone; ++i)
        {
        err = CreateImages(info, testImages, duplicate);
        TInt thisCount = testImages.Count();
        DestroyImages(testImages);

        if (err == KErrNoMemory || err == KErrNoGraphicsMemory)
        	{
        	err = KErrNone;
        	}
        else if (err != KErrNone)
        	{
        	WARN_PRINTF2(_L("Create images error [%d]"), err);
        	SetTestStepResult(ETestSuiteError);
        	}
      
        if (i == 0)
        	{
			count = thisCount;
        	}
        else
            {
			if (count != thisCount)
				{
				INFO_PRINTF4(_L("Mismatch @ iteration %d: initial %d, now %d"), i,  count, thisCount);
				}
			if (tolerance >= 0)
				{
				TEST(Abs(count - thisCount) <= tolerance);
				}
            }
        }
    
    INFO_PRINTF2(_L("Last iteration: %d images created\r\n"), count);
    if (duplicate)
    	{
		// send a null ID to tell the second process to kill itself
		// and wait until the second process terminates
		TRequestStatus status;
		iSecondProcess.Logon(status);
		iMsgQ.SendBlocking(KSgNullDrawableId);
		User::WaitForRequest(status);
		iMsgQ.Close();
		iSecondProcess.Close();
    	}
    TestCloseDriver();
    }

