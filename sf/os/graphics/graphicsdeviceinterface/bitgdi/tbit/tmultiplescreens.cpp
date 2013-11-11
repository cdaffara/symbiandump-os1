// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <hal.h>
#include <graphics/gdi/gdiconsts.h>
#include "tmultiplescreens.h"

CTMultipleScreens::CTMultipleScreens(CTestStep* aStep):
	CTGraphicsBase(aStep),
	iScrDev(NULL),
	iGc(NULL)
	{
	}

CTMultipleScreens::~CTMultipleScreens()
	{
	DestroyFont();
	DeleteGraphicsContext();
	DeleteScreenDevice();
	}

void CTMultipleScreens::ConstructL()
	{
	}

void CTMultipleScreens::RunTestCaseL(TInt aCurTestCase)
	{
	((CTMultipleScreensStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	_LIT(KTest1,"SubTest %d: Create multiple screens");
	switch(aCurTestCase)
		{
	case 1:
		((CTMultipleScreensStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0072"));
		INFO_PRINTF2(KTest1,aCurTestCase);
		CreateScreenDeviceL();
		break;
	case 2:
		((CTMultipleScreensStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTMultipleScreensStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	((CTMultipleScreensStep*)iStep)->RecordTestResultL();
	}


/**
  @SYMTestCaseID GRAPHICS-BITGDI-0072
 
  @SYMDEF             

  @SYMTestCaseDesc Multiple screen test
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions creates some screens in different modes then writes some rotated text to them and test.
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/	
void CTMultipleScreens::CreateScreenDeviceL()
	{
	TDisplayMode testMode[] =  {EColor4K, EColor64K, EColor16M, EColor16MU, EColor256, EColor16MA, EColor16MAP};//tested display modes
	for(TInt ii=0;ii<TInt(sizeof(testMode)/sizeof(testMode[0]));ii++)
		{
		TInt screenCnt = 0;
		TEST(HAL::Get(0, HALData::EDisplayNumberOfScreens, screenCnt) == KErrNone);
		for(TInt screenNo=0;screenNo<screenCnt;++screenNo)
			{
			TInt err = CreateScrDevAndContext(screenNo, testMode[ii]);
			if(err == KErrNone)
				{
				DoRotateMoveTextL();
				}
			DeleteGraphicsContext();
			DeleteScreenDevice();
			}
		}
	}

void CTMultipleScreens::DoRotateMoveTextL()
	{
	__ASSERT_ALWAYS(iScrDev, User::Invariant());
	__ASSERT_ALWAYS(iGc, User::Invariant());
	
	CreateFontL();

	const CFbsBitGc::TGraphicsOrientation KOrientation[] = 
		{
		CFbsBitGc::EGraphicsOrientationNormal,
		CFbsBitGc::EGraphicsOrientationRotated90,
		CFbsBitGc::EGraphicsOrientationRotated180,
		CFbsBitGc::EGraphicsOrientationRotated270
		};

	for(TInt ii=0;ii<TInt(sizeof(KOrientation)/sizeof(KOrientation[0]));++ii)
		{
		if(!iGc->SetOrientation(KOrientation[ii]))
			{
			continue;
			}
		_LIT(KRotation,"===EOrientation%S===");
		INFO_PRINTF2(KRotation,&RotationName(KOrientation[ii]));

		TSize size = iScrDev->SizeInPixels();
		RDebug::Print(_L("Size: %d, %d\r\n"), size.iWidth, size.iHeight);
		for(TInt x=-40;x<(size.iWidth+30);x+=27)
			{
			for(TInt y=-40;y<(size.iHeight+30);y+=23)
				{
				iGc->Clear();
				iGc->SetPenStyle(CGraphicsContext::ESolidPen);
				iGc->SetPenColor(TRgb(0x00, 0x00, 0x00));
				iGc->SetPenSize(TSize(1, 1));
				
				iGc->DrawText(_L("Test text"), TPoint(x, y));
				
				iScrDev->Update();
				}
			}
		}
	iGc->SetOrientation(CFbsBitGc::EGraphicsOrientationNormal);
	DestroyFont();
	}


TInt CTMultipleScreens::CreateScrDevAndContext(TInt aScreenNo, TDisplayMode aDisplayMode)
	{
	DeleteGraphicsContext();
	DeleteScreenDevice();
	TRAPD(err, iScrDev = CFbsScreenDevice::NewL(aScreenNo, aDisplayMode));
	if ( !iScrDev )
		{
		TESTE( err == KErrNotSupported, err );
		return err;
		}
	TEST(err == KErrNone);
	TEST(iScrDev->ScreenNo() == aScreenNo);
	err = iScrDev->CreateContext((CGraphicsContext*&)iGc);
	if ( !iGc )
		{
		return err;
		}
	TEST(err == KErrNone);
	iGc->SetUserDisplayMode(aDisplayMode);
	iScrDev->ChangeScreenDevice(NULL);
	iScrDev->SetAutoUpdate(EFalse);
	return err;
	}

TInt CTMultipleScreens::CreateScrDevAndContext(TDisplayMode aDisplayMode)
	{
	return CreateScrDevAndContext(KDefaultScreenNo,aDisplayMode);
	}

void CTMultipleScreens::DeleteScreenDevice()
	{
	delete iScrDev;
	iScrDev = NULL;
	}

void CTMultipleScreens::DeleteGraphicsContext()
	{
	delete iGc;
	iGc = NULL;
	}

void CTMultipleScreens::CreateFontL()
	{
	CFbsFont* font = NULL;
	TFontSpec fs(_L("Swiss"), 12);
	User::LeaveIfError(iScrDev->GetNearestFontToDesignHeightInPixels(font, fs));
	iGc->UseFont(font);
	}

void CTMultipleScreens::DestroyFont()
	{
	if(iGc)
		{
		iGc->DiscardFont();
		}
	}


//


//--------------
__CONSTRUCT_STEP__(MultipleScreens)

void CTMultipleScreensStep::TestSetupL()
	{
	}
	
void CTMultipleScreensStep::TestClose()
	{
	}
