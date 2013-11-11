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
#include "tparam.h"
#include <graphics/fbsdefs.h>


CTParam::CTParam(CTestStep* aStep):
	CTGraphicsBase(aStep), 
	iDevice(NULL),
	iContext(NULL)
	{
	INFO_PRINTF1(_L("Large parameter testing"));
	}

void CTParam::RunTestCaseL(TInt aCurTestCase)
	{
	((CTParamStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
		{
		((CTParamStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0084"));
		INFO_PRINTF1(_L("Test display mode EGray256"));
		DoTestL(EGray256);
		break;
		}
	case 2:
		{
		((CTParamStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0084"));
		INFO_PRINTF1(_L("Test display mode EColor256"));
		DoTestL(EColor256);
		break;
		}
	case 3:
		{
		((CTParamStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0084"));
		INFO_PRINTF1(_L("Test display mode EColor4K"));
		DoTestL(EColor4K);
		break;
		}
	case 4:
		{
		((CTParamStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0084"));
		INFO_PRINTF1(_L("Test display mode EColor64K"));
		DoTestL(EColor64K);
		break;
		}
	case 5:
		{
		((CTParamStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0084"));
		INFO_PRINTF1(_L("Test display mode EColor16M"));
		DoTestL(EColor16M);
		break;
		}
	case 6:
		{
		((CTParamStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0084"));
		INFO_PRINTF1(_L("Test display mode EColor16MU"));
		DoTestL(EColor16MU);
		break;
		}
	case 7:
		{
		((CTParamStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0084"));
		INFO_PRINTF1(_L("Test display mode EColor16MA"));
		DoTestL(EColor16MA);
		break;
		}
	case 8:
		{
		((CTParamStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0084"));
		INFO_PRINTF1(_L("Test display mode EColor16MAP"));
		DoTestL(EColor16MAP);
		break;
		}
	case 9:
		{
		((CTParamStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTParamStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
		}
	((CTParamStep*)iStep)->RecordTestResultL();
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0084
 
  @SYMDEF             

  @SYMTestCaseDesc tests the display modes
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions tests various graphic drawing and text drawing functions to all current screen modes
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTParam::DoTestL(TDisplayMode aDispMode)
    {
	TRAPD(err,iDevice = CFbsScreenDevice::NewL(_L("scdv"),aDispMode));
	if (err == KErrNotSupported)
		return;
	iDevice->ChangeScreenDevice(NULL);
	iDevice->SetAutoUpdate(ETrue);
	User::LeaveIfError(iDevice->CreateContext(iContext));

	iDevice->SetScalingFactor(TPoint(15, 7), 3, 2, 1, 1);
	iContext->Activate(iDevice);

	CFbsFont* font = NULL;
	User::LeaveIfError(iDevice->GetNearestFontToDesignHeightInTwips(font,TFontSpec()));
	iContext->UseFont(font);
	iDevice->ReleaseFont(font);

	TInt ret = iBitmap.Load(_L("z:\\system\\data\\tbmp.mbm"),EMbmTbmpTbmp,EFalse); 
	User::LeaveIfError(ret);

	DoAnotherTestL();

	iBitmap.Reset();
	delete iContext;
	delete iDevice;
	}

void CTParam::DoAnotherTestL()
	{
	INFO_PRINTF1(_L("Parameters : check large parameters do not take too long"));
	TRect vlarge(-1000000000,-1000000000,1000000000,1000000000);
	TRect screen;
	iDevice->GetDrawRect(screen);
	 
	INFO_PRINTF1(_L("Origin"));
	iContext->SetOrigin(vlarge.iTl);
	iContext->DrawRect(screen);
	iContext->SetOrigin();
	iContext->Clear(screen);
	iContext->SetOrigin(vlarge.iBr);
	iContext->DrawRect(screen);
	iContext->SetOrigin();
	iContext->Clear(screen);
	 
	iContext->SetBrushStyle(CGraphicsContext::EDiamondCrossHatchBrush);
	iContext->SetBrushColor(KRgbBlack);
	INFO_PRINTF1(_L("BrushOrigin"));
	iContext->SetBrushOrigin(vlarge.iTl);
	iContext->DrawRect(screen);
	iContext->SetBrushOrigin(TPoint(0,0));
	iContext->SetBrushColor(KRgbWhite);
	iContext->Clear(screen);
	iContext->SetBrushOrigin(vlarge.iBr);
	iContext->SetBrushColor(KRgbBlack);
	iContext->DrawRect(screen);
	iContext->SetBrushOrigin(TPoint(0,0));
	iContext->SetBrushColor(KRgbWhite);
	iContext->Clear(screen);
	 
	iContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
	INFO_PRINTF1(_L("Plot"));
	iContext->Plot(vlarge.iBr);
	iContext->Clear(screen);
	 
	INFO_PRINTF1(_L("DrawLine - Solid pen"));
	TInt count=1;
	for(;count<10000;count*=10)
		{
		iContext->SetPenSize(TSize(count,count));
		INFO_PRINTF1(_L("Very large line 1"));
		iContext->DrawLine(vlarge.iBr,vlarge.iTl);
		INFO_PRINTF1(_L("Very large line 2"));
		iContext->DrawLine(TPoint(-1000000000,screen.iTl.iY),TPoint(1000000000,1000000000));
		INFO_PRINTF1(_L("Very large line 3"));
		iContext->DrawLine(TPoint(screen.iTl.iX,-1000000000),TPoint(1000000000,1000000000));
		INFO_PRINTF1(_L("Very large line 4"));
		iContext->DrawLine(TPoint(-1000000000,-1000000000),screen.iTl);
		INFO_PRINTF1(_L("Very large line 5"));
		iContext->DrawLine(screen.iBr,TPoint(1000000000,1000000000));
		iContext->Clear(screen);
		}
	 
	iContext->SetPenStyle(CGraphicsContext::EDottedPen);
	INFO_PRINTF1(_L("Line - Dotted pen"));
	for(count=1;count<10000;count*=10)
		{
		iContext->SetPenSize(TSize(count,count));
		INFO_PRINTF1(_L("Very large line 1"));
		iContext->DrawLine(vlarge.iBr,vlarge.iTl);
		INFO_PRINTF1(_L("Very large line 2"));
		iContext->DrawLine(TPoint(-1000000000,screen.iTl.iY),TPoint(1000000000,1000000000));
		INFO_PRINTF1(_L("Very large line 3"));
		iContext->DrawLine(TPoint(screen.iTl.iX,-1000000000),TPoint(1000000000,1000000000));
		INFO_PRINTF1(_L("Very large line 4"));
		iContext->DrawLine(TPoint(-1000000000,-1000000000),screen.iTl);
		INFO_PRINTF1(_L("Very large line 5"));
		iContext->DrawLine(screen.iBr,TPoint(1000000000,1000000000));
		iContext->Clear(screen);
		}
	 
	iContext->SetPenStyle(CGraphicsContext::ESolidPen);
	iContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iContext->SetBrushColor(KRgbBlack);
	iContext->SetPenSize(TSize(1,1));
	INFO_PRINTF1(_L("Arc"));
	iContext->DrawArc(vlarge,vlarge.iBr,vlarge.iBr);
	iContext->Clear(screen);
	 
	INFO_PRINTF1(_L("Ellipse"));
	iContext->DrawEllipse(vlarge);
	iContext->Clear(screen);
 
	INFO_PRINTF1(_L("Pie"));
	iContext->DrawPie(vlarge,vlarge.iBr,vlarge.iBr);
	iContext->Clear(screen);
	 
	INFO_PRINTF1(_L("RoundRect"));
	iContext->DrawRoundRect(vlarge,TSize(100000000,100000000));
	iContext->Clear(screen);
	 
	INFO_PRINTF1(_L("Rect"));
	iContext->DrawRect(vlarge);
	iContext->Clear(screen);
	 
	INFO_PRINTF1(_L("CopyRect"));
	iContext->CopyRect(TPoint(100000000,100000000),vlarge);
	iContext->Clear(screen);
	 
	INFO_PRINTF1(_L("BitBlt"));
	iContext->BitBlt(TPoint(0,0),*iContext,vlarge);
	iContext->Clear(screen);
	iContext->BitBlt(TPoint(0,0),&iBitmap,vlarge);
	iContext->Clear(screen);
	 
	INFO_PRINTF1(_L("BitBltMasked"));
	iContext->BitBltMasked(TPoint(0,0),&iBitmap,vlarge,&iBitmap,EFalse);
	iContext->Clear(screen);
	 
	iContext->SetBrushColor(KRgbBlack);
	iContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
	INFO_PRINTF1(_L("Polygon"));
	CArrayFixFlat<TPoint>* poly=new CArrayFixFlat<TPoint>(4);
	poly->AppendL(vlarge.iTl);
	TPoint temp(vlarge.iBr.iX,vlarge.iTl.iY);
	poly->AppendL(temp);
	poly->AppendL(vlarge.iBr);
	temp.SetXY(vlarge.iTl.iX,vlarge.iBr.iY);
	poly->AppendL(temp);
	iContext->DrawPolygon(poly);
	iContext->Clear(screen);
	delete poly;
	 
	INFO_PRINTF1(_L("DrawBitmap"));
	iContext->DrawBitmap(vlarge,&iBitmap);
	iContext->Clear(screen);
	 
	INFO_PRINTF1(_L("Text"));
	TBuf<16> text(_L("Test text."));
	iContext->DrawText(text,vlarge.iTl);
	iContext->DrawText(_L("Test text."),vlarge.iBr);
	iContext->SetCharJustification(100000000,1);
	iContext->SetWordJustification(100000000,1);
	iContext->DrawText(_L("Test text."),TPoint(0,0));
	iContext->Clear(screen);
	}

//--------------
__CONSTRUCT_STEP__(Param)

void CTParamStep::TestSetupL()
	{
	FbsStartup();
	User::LeaveIfError(RFbsSession::Connect());
	}
	
void CTParamStep::TestClose()
	{
	RFbsSession::Disconnect();
	}
