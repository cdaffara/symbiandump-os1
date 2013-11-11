// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32hal.h>
#include "TGDI.H"
#include <bitdev.h>
#include <hal.h>
#include "TBMP.H"
#include "tbase.h"
#include <graphics/fbsdefs.h>


CTGdi::CTGdi(CTestStep* aStep):
	CTGraphicsBase(aStep),
	iTestGdi()
	{
	}

void CTGdi::RunTestCaseL(TInt aCurTestCase)
	{
	((CTGdiStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
		((CTGdiStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0030"));
		TestScreenL(EGray2);
		break;
	case 2:
		((CTGdiStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0030"));
		TestScreenL(EGray4);
		break;
	case 3:
		((CTGdiStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0030"));
		TestScreenL(EGray16);
		break;
	case 4:
		((CTGdiStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0030"));
		TestScreenL(EGray256);
		break;
	case 5:
		((CTGdiStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0030"));
		TestScreenL(EColor16);
		break;
	case 6:
		((CTGdiStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0030"));
		TestScreenL(EColor256);
		break;
	case 7:
		((CTGdiStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0030"));
		TestScreenL(EColor4K);
		break;
	case 8:
		((CTGdiStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0030"));
		TestScreenL(EColor64K);
		break;
	case 9:
		((CTGdiStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0030"));
		TestScreenL(EColor16M);
		break;
	case 10:
		((CTGdiStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0030"));
		TestScreenL(EColor16MU);
		break;
	case 11:
		((CTGdiStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
//		Test still failing, see DEF094106	
//		TestScreenL(EColor16MA); 
		break;
	case 12:
		((CTGdiStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0031"));
		TestBitmapL(EGray2);
		break;
	case 13:
		((CTGdiStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0031"));
		TestBitmapL(EGray4);
		break;
	case 14:
		((CTGdiStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0031"));
		TestBitmapL(EGray16);
		break;
	case 15:
		((CTGdiStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0031"));
		TestBitmapL(EGray256);
		break;
	case 16:
		((CTGdiStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0031"));
		TestBitmapL(EColor16);
		break;
	case 17:
		((CTGdiStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0031"));
		TestBitmapL(EColor256);
		break;
	case 18:
		((CTGdiStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0031"));
		TestBitmapL(EColor4K);
		break;
	case 19:
		((CTGdiStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0031"));
		TestBitmapL(EColor64K);
		break;
	case 20:
		((CTGdiStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0031"));
		TestBitmapL(EColor16M);
		break;
	case 21:
		((CTGdiStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0031"));
		TestBitmapL(EColor16MU);
		break;
	case 22:  
		((CTGdiStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
//		Test still failing, see DEF094106
//		TestBitmapL(EColor16MA);
		break;
	case 23:		
		((CTGdiStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTGdiStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	((CTGdiStep*)iStep)->RecordTestResultL();
	//
	}


/**
  @SYMTestCaseID GRAPHICS-BITGDI-0030
 
  @SYMDEF             

  @SYMTestCaseDesc Tests screen creation in various colour modes
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Creates a screen in a certain colour mode, draws a bitmap on it then destroys it 
 
  @SYMTestExpectedResults Test should pass
*/
void CTGdi::TestScreenL(TDisplayMode aDispMode)
	{
	CFbsScreenDevice* device = NULL;
	TRAPD(err,device = CFbsScreenDevice::NewL(_L("scdv"),aDispMode));
	if (err == KErrNotSupported)
		return;
	else
		User::LeaveIfError(err);
	CleanupStack::PushL(device);
	device->ChangeScreenDevice(NULL);
	device->SetAutoUpdate(ETrue);

	DoTestAndDeleteL(device,ETrue);
	CleanupStack::PopAndDestroy(1,device);
	}


/**
  @SYMTestCaseID GRAPHICS-BITGDI-0031
 
  @SYMDEF             

  @SYMTestCaseDesc Tests bitmap creation in various colour modes
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Creates a bitmap in a certain colour mode, draws a bitmap on it then destroys it 
 
  @SYMTestExpectedResults Test should pass
*/
void CTGdi::TestBitmapL(TDisplayMode aDispMode)
	{
	CFbsBitmap* bitmap = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);
	//Creating a Reference Bitmap of screensize to compare the results
	CFbsScreenDevice* qdevice = NULL;
	TRAPD(error, qdevice = CFbsScreenDevice::NewL(_L("scdv"),aDispMode));

	if (error == KErrNotSupported)
		{
		CleanupStack::PopAndDestroy(bitmap);
		return;
		}
	else
		{
		User::LeaveIfError(error);
		}

	CleanupStack::PushL(qdevice);
	qdevice->SetAutoUpdate(ETrue);
	TSize screensize=qdevice->SizeInPixels();
	TInt ret = bitmap->Create(screensize,aDispMode);
	if (ret == KErrNotSupported)
		{
		CleanupStack::PopAndDestroy(2,bitmap);
		return;
		}
	else
		{
		User::LeaveIfError(ret);
		}

	CFbsBitmapDevice* device = CFbsBitmapDevice::NewL(bitmap);
	CleanupStack::PushL(device);
	DoTestAndDeleteL(device, EFalse);
	CleanupStack::PopAndDestroy(3,bitmap);
	}

void CTGdi::DoTestAndDeleteL(CFbsDevice* aDevice,TBool aScreen)
	{
	CFbsBitGc* gc = NULL;
	User::LeaveIfError(aDevice->CreateContext(gc));
	CleanupStack::PushL(gc);

	CFbsFont* font = NULL;
	User::LeaveIfError(aDevice->GetNearestFontToDesignHeightInTwips(font,TFontSpec()));
	gc->UseFont(font);
	aDevice->ReleaseFont(font);

	CFbsBitmap* bmp = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp);
	TInt ret = bmp->Load(_L("z:\\system\\data\\tbmp.mbm"),EMbmTbmpTblank);
	User::LeaveIfError(ret);
	gc->UseBrushPattern(bmp->Handle());

	if (aScreen)
		INFO_PRINTF2(_L("Gdi screen device testing, mode %d\n"),aDevice->DisplayMode());
	else
		INFO_PRINTF2(_L("Gdi bitmap device testing, mode %d\n"),aDevice->DisplayMode());

	TBuf<KFailureSectionNameBufferLength> section;
	iTestGdi.Construct(aDevice,gc,section);

	TBool pass = iTestGdi.Test();

	if (!pass)
		{
		INFO_PRINTF2(_L("Gdi device test failed section: %S"),&section);
		//Getch();
		}
	TEST(pass);

	CleanupStack::PopAndDestroy(2,gc);
	}

//--------------
__CONSTRUCT_STEP__(Gdi)

void CTGdiStep::TestSetupL()
	{
	FbsStartup();
	User::LeaveIfError(RFbsSession::Connect());
	}

void CTGdiStep::TestClose()
	{
	RFbsSession::Disconnect();
	}
