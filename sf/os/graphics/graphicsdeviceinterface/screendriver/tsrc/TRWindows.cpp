// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Note that this tests emulator-specific functionality. The test does not apply to GCE mode.
// 
//

#include "TRWindows.h"
#include <hal.h>
#include <bitdraw.h>
#ifdef __WINS__	
	#include "_WININC.H"
	#include <winsdef.h>
#endif

CTRWindows::CTRWindows(CTestStep* aStep) :
	CTGraphicsBase(aStep)
	{
	INFO_PRINTF1(_L("Test for RWindows"));
	}

GLDEF_C void Panic(TScreenDriverPanic aPanicCode)
	{
	_LIT(KSCDVPanicCategory,"SCDV");
	User::Panic(KSCDVPanicCategory,aPanicCode);
	}

//This function is used to test the changes made in the RWindows class
//which will allow creating a screen size with the ScanLinebyte not multiple of
//12 bytes. E.g a 250 * 250 pixels would have scanlinebyte=250*3(RGB)=750 bytes
//(not a multiple of 12 bytes) which will cause the panic in the previous RWindows
//settings. For more details please refer to INC042143 

#ifdef __WINS__	
void CTRWindows::TestCreatingWindowsL(TSize aSize)
	{
	TInt	screenNo=0;   
	TInt	maxScreenNo=0;
	TInt err=HAL::Get(screenNo, HALData::EDisplayNumberOfScreens,maxScreenNo);					
	User::LeaveIfError(err);								
	TEST(maxScreenNo!=0);
	TInt pixelDepth=0;
	err=HAL::Get(0, HALData::EDisplayMode,pixelDepth);	

#if defined(SYMBIAN_GRAPHICS_GCE)
	if (!(pixelDepth&KEmulIsBitMask))
		{
		INFO_PRINTF1(_L("Native Wins mode not detected (Mode is not a bit field) Test skipped."));
		}
	else
#endif //SYMBIAN_GRAPHICS_GCE
		{
		for (screenNo=0;screenNo<maxScreenNo;screenNo++)		
			{							
			TBuf<128> buf;
			buf.Format(_L("INC042143 fix. Screen size: %03d x %03d, screen %i"), aSize.iWidth, aSize.iHeight,screenNo);
			INFO_PRINTF1(buf);
			TInt address=NULL;
			TInt err=HAL::Get(screenNo, HALData::EDisplayMemoryAddress,address);
			User::LeaveIfError(err);								
			RWindows* theWindow=RWindows::GetWindow(screenNo, REINTERPRET_CAST(TAny*,address),aSize);
			TEST(theWindow!=NULL);
			RWindows::ReleaseWindow(screenNo);			
			}
		}
	}

#endif

void CTRWindows::RunTestCaseL(TInt aCurTestCase)
	{
	((CTRWindowsStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
#ifdef __WINS__	
	case 1:
/**
	@SYMTestCaseID GRAPHICS-SCREENDRIVER-0019
*/
		((CTRWindowsStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0019"));
		TestCreatingWindowsL(TSize(250,250));
		break;
	case 2:
/**
	@SYMTestCaseID GRAPHICS-SCREENDRIVER-0020
*/
		((CTRWindowsStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0020"));
		TestCreatingWindowsL(TSize(121,121));
		break;
	case 3:
/**
	@SYMTestCaseID GRAPHICS-SCREENDRIVER-0021
*/
		((CTRWindowsStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0021"));
		TestCreatingWindowsL(TSize(301,301));
		break;
#endif		
	case 4:
		((CTRWindowsStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTRWindowsStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
#ifdef __WINS__	
	((CTRWindowsStep*)iStep)->RecordTestResultL();
#endif
	}

//--------------
__CONSTRUCT_STEP__(RWindows)

