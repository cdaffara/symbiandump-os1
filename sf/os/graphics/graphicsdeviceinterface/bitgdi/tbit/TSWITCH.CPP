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

#include <bitdev.h>
#include <hal.h>
#include "TBMP.H"
#include "tswitch.h"
#include <graphics/fbsdefs.h>


CTSwitch::CTSwitch(CTestStep* aStep):
	CTGraphicsBase(aStep), 
	iBitmap(NULL),
	iDevice(NULL),
	iGc(NULL),
	iAltDevice(NULL),
	iAltGc(NULL),
	iSrceBuffer(NULL),
	iDestBuffer(NULL)
	{
	INFO_PRINTF1(_L(""));
	}

CTSwitch::~CTSwitch()
	{
	delete iBitmap;
	delete iDevice;
	delete iGc;
	delete iAltDevice;
	delete iAltGc;
	delete[] iSrceBuffer;
	delete[] iDestBuffer;
	}

void CTSwitch::RunTestCaseL(TInt aCurTestCase)
	{
	((CTSwitchStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
		((CTSwitchStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0065"));
		TestL();
		break;
	case 2:
		((CTSwitchStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTSwitchStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	((CTSwitchStep*)iStep)->RecordTestResultL();
	}

	
/**
  @SYMTestCaseID GRAPHICS-BITGDI-0065
 
  @SYMDEF             

  @SYMTestCaseDesc Tests display mode switching
   
  @SYMTestPriority normal

  @SYMTestStatus Implemented

  @SYMTestActions Creates a bitmap then switches between displaymodes and checks bitmap is not corrupted
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTSwitch::TestL()
	{
	INFO_PRINTF1(_L("Testing display mode switching"));
	const TInt KNumDispModes = 12;

	TDisplayMode displayModeArray[KNumDispModes] =
		{
		EGray2, EGray4, EGray16, EGray256,
		EColor16, EColor256, EColor4K, EColor64K, EColor16M,
		EColor16MU, EColor16MA, EColor16MAP
		};

	const TPtrC displayModeNameArray[KNumDispModes] =
		{
		_L("EGray2"), _L("EGray4"), _L("EGray16"), _L("EGray256"),
		_L("EColor16"), _L("EColor256"), _L("EColor4K"), _L("EColor64K"), _L("EColor16M"),
		_L("EColor16MU"), _L("EColor16MA"), _L("EColor16MAP")
		};

	iBitmap = new(ELeave) CFbsBitmap;
	TInt ret = iBitmap->Load(_L("z:\\system\\data\\tbmp.mbm"),EMbmTbmpTcolor,EFalse);
	User::LeaveIfError(ret);

	iSrceBuffer = new(ELeave) TRgb[iBitmap->SizeInPixels().iWidth];
	iDestBuffer = new(ELeave) TRgb[iBitmap->SizeInPixels().iWidth];

	for (TInt dispModeIndex = 0; dispModeIndex < KNumDispModes; dispModeIndex++)
		{
		TDisplayMode dispMode = displayModeArray[dispModeIndex];
		if (!SetModeL(dispMode))
			continue;

		INFO_PRINTF1(displayModeNameArray[dispModeIndex]);

		for (TInt altDispModeIndex = 0; altDispModeIndex < KNumDispModes; altDispModeIndex++)
			{
			if (dispModeIndex == altDispModeIndex)
				continue;

			FillScreen();

			TDisplayMode altDispMode = displayModeArray[altDispModeIndex];
			if (SwitchModeL(altDispMode))
				{
				INFO_PRINTF1(displayModeNameArray[altDispModeIndex]);
				INFO_PRINTF1(_L(" "));
				CheckSwitch(dispMode,altDispMode);
				}
			else
				WARN_PRINTF1(_L("Not supported"));
			}

		INFO_PRINTF1(_L("\r\n"));
		}
	}

TBool CTSwitch::SetModeL(TDisplayMode aDispMode)
	{
	delete iDevice;
	iDevice = NULL;
	delete iGc;
	iGc = NULL;

	TRAPD(err,iDevice = CFbsScreenDevice::NewL(_L("scdv"),aDispMode));
	if (err == KErrNotSupported)
		return EFalse;
	User::LeaveIfError(err);

	User::LeaveIfError(iDevice->CreateContext((CGraphicsContext*&)iGc));
	iDevice->ChangeScreenDevice(NULL);
	iDevice->SetAutoUpdate(ETrue);

	return ETrue;
	}

void CTSwitch::FillScreen()
	{
	TSize size = iDevice->SizeInPixels();
	TSize bmpSize = iBitmap->SizeInPixels();

	for (TInt height = 0; height < size.iHeight; height += bmpSize.iHeight)
		for (TInt width = 0; width < size.iWidth; width += bmpSize.iWidth)
			iGc->BitBlt(TPoint(width,height),iBitmap);
	}

TBool CTSwitch::SwitchModeL(TDisplayMode aDispMode)
	{
	delete iAltDevice;
	iAltDevice = NULL;
	delete iAltGc;
	iAltGc = NULL;

	TRAPD(err,iAltDevice = CFbsScreenDevice::NewL(_L("scdv"),aDispMode));
	if (err == KErrNotSupported)
		return EFalse;
	User::LeaveIfError(err);

	User::LeaveIfError(iAltDevice->CreateContext((CGraphicsContext*&)iAltGc));
	iAltDevice->SetAutoUpdate(ETrue);

	iAltDevice->ChangeScreenDevice(iDevice);

	return ETrue;
	}

void CTSwitch::CheckSwitch(TDisplayMode aOldDispMode,TDisplayMode aNewDispMode)
	{
	TSize size = iDevice->SizeInPixels();
	TSize bmpSize = iBitmap->SizeInPixels();
	TPtr8 srce((TUint8*)iSrceBuffer,bmpSize.iWidth * sizeof(TRgb),bmpSize.iWidth * sizeof(TRgb));
	TPtr8 dest((TUint8*)iDestBuffer,bmpSize.iWidth * sizeof(TRgb),bmpSize.iWidth * sizeof(TRgb));

	for (TInt yy = 0; yy < size.iHeight; yy++)
		{

		for (TInt xx = 0; xx < size.iWidth; xx += bmpSize.iWidth)
			{
			// Width for scanline as wide as bitmap or remaining width of screen, whichever smallest
			TUint lineWidth = Min(bmpSize.iWidth,size.iWidth - xx);

			// yy%bmpSize.iHeight used if screen height greater than bitmap
			// as it is tessellated across the screen
			iBitmap->GetScanLine(srce,TPoint(00,yy%bmpSize.iHeight),lineWidth,ERgb);
			iAltDevice->GetScanLine(dest,TPoint(xx,yy),lineWidth,ERgb);
			CheckBuffers(aOldDispMode,aNewDispMode,lineWidth,(yy * 2 + xx) & 3); // Last parameter is aDitherOffset
			}
		}
	}

void CTSwitch::CheckBuffers(TDisplayMode aOldDispMode, TDisplayMode aNewDispMode, TUint aLineWidth,TInt aDitherOffset)
	{
	TRgb* srcePtr = iSrceBuffer;
	TRgb* destPtr = iDestBuffer;

	for (TUint count = 0; count < aLineWidth; count++)
		{
		TRgb srce = *srcePtr++;
		TRgb dest = *destPtr++;
		CheckRgbs(aOldDispMode,srce,aNewDispMode,dest,aDitherOffset ^ (count & 1));
		}
	}

void CTSwitch::CheckRgbs(TDisplayMode aOldDispMode,TRgb aSrceRgb,TDisplayMode aNewDispMode,TRgb aDestRgb,TInt aDitherOffset)
	{
	ConvertRgb(aSrceRgb,aOldDispMode,aDitherOffset);
	ConvertRgb(aSrceRgb,aNewDispMode,1); // 1 suppresses dithering in EGray2 mode
	TEST(aSrceRgb == aDestRgb);
	}

void CTSwitch::ConvertRgb(TRgb& aRgb,TDisplayMode aDispMode,TInt aDitherOffset)
	{
	switch (aDispMode)
		{
	case EGray2:
		{
		TInt gray4 = aRgb.Gray4();
		if (gray4 == 0)
			aRgb = KRgbBlack;
		else if (gray4 == 1)
			{
			if (aDitherOffset == 1 || aDitherOffset == 2)
				aRgb = KRgbBlack;
			else
				aRgb = KRgbWhite;
			}
		else if (gray4 == 2)
			{
			if (aDitherOffset == 3)
				aRgb = KRgbBlack;
			else
				aRgb = KRgbWhite;
			}
		else if (gray4 == 3)
			aRgb = KRgbWhite;
		}
		break;
	case EGray4:
		aRgb = TRgb::Gray4(aRgb.Gray4());
		break;
	case EGray16:
		aRgb = TRgb::Gray16(aRgb.Gray16());
		break;
	case EGray256:
		aRgb = TRgb::Gray256(aRgb.Gray256());
		break;
	case EColor16:
		aRgb = TRgb::Color16(aRgb.Color16());
		break;
	case EColor256:
		aRgb = TRgb::Color256(aRgb.Color256());
		break;
	case EColor4K:
		aRgb = TRgb::Color4K(aRgb.Color4K());
		break;
	case EColor64K:
		aRgb = TRgb::Color64K(aRgb.Color64K());
		break;
	default:
		break;
		}
	}

//--------------
__CONSTRUCT_STEP__(Switch)

void CTSwitchStep::TestSetupL()
	{
	FbsStartup();
	User::LeaveIfError(RFbsSession::Connect());
	}
	
void CTSwitchStep::TestClose()
	{
	RFbsSession::Disconnect();
	}
