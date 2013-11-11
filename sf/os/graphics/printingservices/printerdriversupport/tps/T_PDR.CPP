// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @test
 @internalComponent Internal Symbian test code
*/

#include "T_PDR.H"
#include <graphics/fbsdefs.h>


#if defined (__WINS__)
#define PDD_NAME _L("ECDRV")
#define LDD_NAME _L("ECOMM")
#else
#define PDD_NAME _L("EUART1")
#define LDD_NAME _L("ECOMM")
#endif

// Set KVerboseOutput to "ETrue" if extra INFO_PRINTFx for debug purposes are required
const TBool KVerboseOutput = EFalse;

_LIT( KDriveName, "z:" );
_LIT( KEpsonFileName, "epson.pdr" );

//--------------------------------------------------------------------------------
void CPdrTestPrint::RunL()
	{
	CActiveScheduler::Stop();
	}
			
TRequestStatus& CPdrTestPrint::Queue()
	{
	SetActive();
	return iStatus;
	}
			
TRequestStatus& CPdrTestPrint::Status()
	{
	return iStatus;
	}
			
//-------------------------------------------------------------------------------

CTPdr::CTPdr(CTestStep* aStep) :
	CTGraphicsBase(aStep)
	{
	}

CTPdr::~CTPdr()
	{
	}


void CTPdr::ConstructL()
	{
	INFO_PRINTF1(_L( "Start the various bits and pieces of the comms server\n" ));

	TInt ret;
#if !defined(__X86GCC__)
	ret=User::LoadPhysicalDevice(PDD_NAME);
	TEST(ret==KErrNone || ret==KErrAlreadyExists);

	ret=User::LoadLogicalDevice(LDD_NAME);
	TEST(ret==KErrNone || ret==KErrAlreadyExists);
#endif

	// to avoid alloc heaven with the fbs server
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);
	User::LeaveIfError(bitmap->Create(TSize(1024, 1024), EColor16MA));
	CleanupStack::PopAndDestroy(bitmap);
	
  	// to avoid alloc heaven with the fbs server
  	CFbsBitmap bmp1;   
  	CFbsBitmap bmp2;
   	ret=bmp1.Load(_L("z:\\test\\pdrstore.mbm"),0,NULL);	// sheep
  	TEST(ret==KErrNone);
  	ret=bmp2.Load(_L("z:\\test\\pdrstore.mbm"),1,NULL);	// pslogo
  	TEST(ret==KErrNone);
  	bmp1.Reset();
  	bmp2.Reset();
 	}


void CTPdr::testPrint(CPrinterDevice* aDevice)
	{
	INFO_PRINTF1(_L("calling testPrint()..."));
  
	CFbsBitmap bmp1;
	CFbsBitmap bmp2;

	TInt ret=bmp1.Load(_L("z:\\test\\pdrstore.mbm"),0,EFalse);	// sheep
	TEST(ret==KErrNone);
	ret=bmp2.Load(_L("z:\\test\\pdrstore.mbm"),1,EFalse);	// pslogo
	TEST(ret==KErrNone);

	bmp1.SetSizeInTwips(TSize(3000,1500));
	bmp2.SetSizeInTwips(TSize(2000,900));

	TBandAttributes attributes;
	CPrinterControl::TMoreOnPage moreonpage=CPrinterControl::EMoreOnPage;

	TInt h=aDevice->HorizontalPixelsToTwips(1000);
	TInt v=aDevice->VerticalPixelsToTwips(1000);
	TInt k=4800;

	TInt i=0;
	do
		{
		if (KVerboseOutput)
			{
			INFO_PRINTF2(_L("Get band %d"),i+1);
			}
		moreonpage=aDevice->iControl->QueueGetBand(iStopper->Queue(),attributes);
		CActiveScheduler::Start();

		CGraphicsContext* gc=NULL;
		ret=aDevice->CreateContext((CGraphicsContext *&) gc);
		TEST(ret==KErrNone);

		gc->SetPenColor(KRgbBlack);							 
		gc->SetPenStyle(CGraphicsContext::ESolidPen);
		gc->SetPenSize(TSize(4*k/h,4*k/v));
		gc->DrawEllipse(TRect(391,352,1152,601));
		gc->DrawEllipse(TRect(391,336,1152,585));
		gc->Reset();
		gc->DrawEllipse(TRect(727,693,1488,942));
		gc->DrawEllipse(TRect(727,677,1488,926));
		CFont *font;
		TFontSpec spec (_L("CG Times"),300);
		spec.iTypeface.SetIsProportional(ETrue);
		ret=aDevice->GetNearestFontToDesignHeightInTwips((CFont *&) font,spec);
		TEST(ret==KErrNone);

		if (KVerboseOutput)
			{
			INFO_PRINTF2(_L("TextWidthInPixels(HELLO WORLD) %d"),font->TextWidthInPixels(_L("HELLO WORLD")));
			}
			
		gc->UseFont(font);
		gc->SetUnderlineStyle(EUnderlineOn);
		gc->DrawText(_L("HELLO WORLD"),TRect(300*k/h,200*k/v,1800*k/h,300*k/v),100*k/v,CGraphicsContext::ELeft,0);
		gc->SetUnderlineStyle(EUnderlineOff);
		gc->DrawText(_L("HELLO WORLD"),TRect(300*k/h,200*k/v,1800*k/h,300*k/v),100*k/v,CGraphicsContext::ERight,0);
		gc->DrawText(_L("HELLO WORLD"),TRect(300*k/h,200*k/v,1800*k/h,300*k/v),100*k/v,CGraphicsContext::ECenter,0);

		TBuf<20> text;
		text.Format(_L("Test symbols %c %c %c %c"),130,131,132,133);

		gc->SetPenColor(KRgbBlack);							 
		gc->DrawText(text,TRect(1000*k/h,2500*k/v,1800*k/h,2700*k/v),50*k/v,CGraphicsContext::ELeft,0);
		gc->SetWordJustification(1200*k/h,4);
		gc->DrawText(_L("Bramm an gath"),TRect(1000*k/h,2500*k/v,1800*k/h,2700*k/v),100*k/v,CGraphicsContext::ELeft,0);
		gc->DrawText(_L("Bramm an gath"),TRect(1000*k/h,2500*k/v,1800*k/h,2700*k/v),150*k/v,CGraphicsContext::ELeft,0);
		aDevice->ReleaseFont(font);

		ret=aDevice->GetNearestFontToDesignHeightInTwips((CFont *&) font,spec);
		TEST(ret==KErrNone);
		gc->UseFont(font);
		gc->DrawText(_L("BRAMM AN GATH"),TRect(1000*k/h,2800*k/v,1800*k/h,3000*k/v),200*k/v,CGraphicsContext::ELeft,0);
		spec.iFontStyle.SetPrintPosition(EPrintPosSuperscript);
		aDevice->ReleaseFont(font);
		ret=aDevice->GetNearestFontToDesignHeightInTwips((CFont *&) font,spec);
		TEST(ret==KErrNone);
		gc->UseFont(font);
		gc->DrawText(_L("BRAMM AN GATH"),TRect(1000*k/h,2800*k/v,1800*k/h,3000*k/v),200*k/v,CGraphicsContext::ERight,0);
		aDevice->ReleaseFont(font);
		spec.iFontStyle.SetPrintPosition(EPrintPosSubscript);
		ret=aDevice->GetNearestFontToDesignHeightInTwips((CFont *&) font,spec);
		TEST(ret==KErrNone);
		gc->UseFont(font);
		gc->SetBrushColor(KRgbWhite);				   
		gc->SetBrushStyle(CGraphicsContext::ENullBrush);
		gc->DrawText(_L("BRAMM AN GATH"),TRect(1000*k/h,2800*k/v,1800*k/h,3000*k/v),200*k/v,CGraphicsContext::ECenter,0);
		aDevice->ReleaseFont(font);

		// Draw bitmap
		gc->DrawBitmap(TPoint(800*k/h,900*k/v),&bmp1);
		gc->DrawBitmap(TPoint(1700*k/h,3100*k/v),&bmp2);
		gc->DrawBitmap(TPoint(1000*k/h,1400*k/v),&bmp2);
		gc->DrawBitmap(TPoint(100*k/h,1700*k/v),&bmp2);

		// Draw line
		gc->SetPenSize(TSize(30*k/h,30*k/v));
		gc->DrawLine(TPoint(800*k/h,100*k/v),TPoint(2000*k/h,100*k/v));
		gc->DrawLine(TPoint(1500*k/h,400*k/v),TPoint(1800*k/h,1500*k/v));

		// Draw rectangle
		gc->SetPenSize(TSize(4*k/h,4*k/v));				 
		gc->SetBrushStyle(CGraphicsContext::ENullBrush);
		gc->SetBrushColor(KRgbWhite);
		gc->DrawRect(TRect(300*k/h,200*k/v,1800*k/h,300*k/v));

		// Draw circle
		gc->SetPenSize(TSize(10*k/h,10*k/v));
		gc->DrawEllipse(TRect(300*k/h,2600*k/v,700*k/h,3000*k/v));

		// Plot point
		gc->SetPenSize(TSize(20*k/h,20*k/v));
		gc->Plot(TPoint(1800*k/h,1600*k/v));

		delete gc;
		
		if (KVerboseOutput)
			{
			INFO_PRINTF2(_L("End of band %d"),i+1);
			}
		i++;
		}

	while (moreonpage==CPrinterControl::EMoreOnPage);
	INFO_PRINTF1(_L("QueueEndPrint"));
	aDevice->iControl->QueueEndPrint(iStopper->Queue());
	CActiveScheduler::Start();
	INFO_PRINTF2(_L("Request status %d"),iStopper->Status().Int());

	bmp2.Reset();
	bmp1.Reset();
	
	}


//
// Test read file handling.
//
void CTPdr::testPdrStore()
	{
	__UHEAP_MARK;

	INFO_PRINTF1(_L("Add to the active scheduler"));
	iStopper=new(ELeave) CPdrTestPrint();
	CActiveScheduler::Add(iStopper);

	// Open file
	INFO_PRINTF1(_L("OpenPdrL(pdr file)"));
	CPrinterDriver* driver=NULL;
	TRAPD(ret,driver=CPrinterDriver::NewL());
	TEST(ret==KErrNone);
	
	TFileName filename( KDriveName );
	filename.Append( KDefaultPrinterDriverPath );
	filename.Append( KEpsonFileName );

	TRAP(ret,driver->OpenPdrL( filename ));
	TEST(ret==KErrNone);
	INFO_PRINTF2(_L("NumModels %d"),driver->NumModels());

	// Get model info
	TPrinterModelEntry entry;
	TInt i = 0;
	iNumModels = driver->NumModels();
	for (i=0; i<iNumModels; i++)
		{
		entry=driver->Model(i);
		INFO_PRINTF2(_L("  *********************** ModelName %S"),&entry.iModelName);
		}

	// Create device
	INFO_PRINTF1(_L("Create printer device"));
	CPrinterDevice* device=NULL;
	entry=driver->Model(iModelIndex);
	TRAP(ret,device=driver->CreatePrinterDeviceL(entry.iUid));
	TEST(ret==KErrNone);
	CPrinterDriverUI* driverui=NULL;
 	TRAP(ret,driverui=driver->CreatePrinterDriverUIL());
	INFO_PRINTF2(_L("DriverUI %d"),driverui);
	delete driverui;

	INFO_PRINTF2(_L("NumTypefaces %d"),device->NumTypefaces());
	INFO_PRINTF2(_L("**Model name %S**"),&entry.iModelName);
	INFO_PRINTF2(_L("KPixelWidthInTwips %d"),device->HorizontalPixelsToTwips(1000));
	INFO_PRINTF2(_L("KPixelHeightInTwips %d"),device->VerticalPixelsToTwips(1000));
	TPageSpec pagespec(TPageSpec::EPortrait,KA4PaperSizeInTwips);
	device->SelectPageSpecInTwips(pagespec);

	// Get typeface info
	INFO_PRINTF1(_L("Get typeface info"));
	TTypefaceSupport support;
	device->TypefaceSupport(support,0);
	TBuf<KMaxTypefaceNameLength> name;
	name.Copy(support.iTypeface.iName);
	INFO_PRINTF2(_L("Typeface Name %S"),&name);
	INFO_PRINTF2(_L("Typeface serif %d"),support.iTypeface.IsSerif());
	INFO_PRINTF2(_L("Typeface proportional %d"),support.iTypeface.IsProportional());
	INFO_PRINTF2(_L("Typeface symbol %d"),support.iTypeface.IsSymbol());
	INFO_PRINTF2(_L("NumHeights %d"),support.iNumHeights);
	INFO_PRINTF2(_L("MinHeightInTwips %d"),support.iMinHeightInTwips);
	INFO_PRINTF2(_L("MaxHeightInTwips %d"),support.iMaxHeightInTwips);
	INFO_PRINTF2(_L("IsScalable %d"),support.iIsScalable);
	
	// Get nearest font
 	INFO_PRINTF1(_L("Get nearest font"));
	CFont *font;
	TFontSpec spec;
	support.iTypeface.iName=_L("DejaVu Sans Condensed");
	support.iTypeface.SetIsProportional(ETrue);
	support.iTypeface.SetIsSerif(ETrue);
	spec.iTypeface=support.iTypeface;
	spec.iHeight=680;
	spec.iFontStyle.SetPosture(EPostureUpright);
	spec.iFontStyle.SetStrokeWeight(EStrokeWeightNormal);
	spec.iFontStyle.SetPrintPosition(EPrintPosNormal);
	ret=device->GetNearestFontToDesignHeightInTwips((CFont *&) font,spec);
	TEST(ret==KErrNone);
	INFO_PRINTF2(_L("HeightInPixels %d"),font->HeightInPixels());
	INFO_PRINTF2(_L("AscentInPixels %d"),font->AscentInPixels());
	INFO_PRINTF3(_L("Font Posture %d StrokeWeight %d"),font->FontSpecInTwips().iFontStyle.Posture(),font->FontSpecInTwips().iFontStyle.StrokeWeight());
	INFO_PRINTF3(_L("Font AlgPosture %d AlgStrokeWeight %d"),((CBitmapFont*)font)->iAlgStyle.IsItalic(),((CBitmapFont*)font)->iAlgStyle.IsBold());
	spec=font->FontSpecInTwips();
	device->ReleaseFont(font);
	ret=device->GetNearestFontToDesignHeightInTwips((CFont *&) font,spec);
	TEST(ret==KErrNone);
	INFO_PRINTF2(_L("HeightInPixels %d"),font->HeightInPixels());
	INFO_PRINTF2(_L("AscentInPixels %d"),font->AscentInPixels());
	INFO_PRINTF3(_L("Font Posture %d StrokeWeight %d"),font->FontSpecInTwips().iFontStyle.Posture(),font->FontSpecInTwips().iFontStyle.StrokeWeight());
	INFO_PRINTF3(_L("Font AlgPosture %d AlgStrokeWeight %d"),((CBitmapFont*)font)->iAlgStyle.IsItalic(),((CBitmapFont*)font)->iAlgStyle.IsBold());
	name.Copy(font->FontSpecInTwips().iTypeface.iName);
	INFO_PRINTF2(_L("Typeface Name %S"),&name);
	INFO_PRINTF2(_L("Typeface Proportional %d"),font->FontSpecInTwips().iTypeface.IsProportional());
	INFO_PRINTF2(_L("Typeface Serif %d"),font->FontSpecInTwips().iTypeface.IsSerif());
	INFO_PRINTF2(_L("CharWidthInPixels(32) %d"),font->CharWidthInPixels(TChar(32)));
	INFO_PRINTF2(_L("MaxCharWidthInPixels %d"),font->MaxCharWidthInPixels());
	INFO_PRINTF2(_L("MaxNormalCharWidthInPixels %d"),font->MaxNormalCharWidthInPixels());
	INFO_PRINTF2(_L("HeightInTwips %d"),font->FontSpecInTwips().iHeight);
	INFO_PRINTF2(_L("HeightInPixels %d"),font->HeightInPixels());
	INFO_PRINTF2(_L("AscentInPixels %d"),font->AscentInPixels());
	INFO_PRINTF2(_L("TextWidthInPixels(HELLO WORLD) %d"),font->TextWidthInPixels(_L("HELLO WORLD")));
	device->ReleaseFont(font);

	spec.iHeight=1000;
	spec.iFontStyle.SetPrintPosition(EPrintPosNormal);

	spec.iFontStyle.SetPrintPosition(EPrintPosNormal);
	spec.iFontStyle.SetPosture(EPostureUpright);
	spec.iFontStyle.SetStrokeWeight(EStrokeWeightNormal);
	for (i=0; i<device->NumTypefaces(); i++)
		{
		device->TypefaceSupport(support,i);
		name.Copy(support.iTypeface.iName);
		INFO_PRINTF2(_L("Typeface Name %S"),&name);
		for (TInt j=0; j<support.iNumHeights; j++)
			{
			spec.iTypeface=support.iTypeface;
			spec.iHeight=device->FontHeightInTwips(i,j);
			ret=device->GetNearestFontToDesignHeightInTwips((CFont *&) font,spec);
			TEST(ret==KErrNone);
			if (KVerboseOutput)
				{
				INFO_PRINTF2(_L("Spec HeightInTwips %d"),spec.iHeight);
				INFO_PRINTF2(_L("Font HeightInTwipss %d"),font->FontSpecInTwips().iHeight);
				INFO_PRINTF3(_L("Font Posture %d StrokeWeight %d"),font->FontSpecInTwips().iFontStyle.Posture(),font->FontSpecInTwips().iFontStyle.StrokeWeight());
				}
			device->ReleaseFont(font);
			}
		}

	RFile file;
	RFs fs;
	ret=fs.Connect();
	TEST(ret==KErrNone);
	_LIT(KPath,"C:\\system\\data\\");
	fs.MkDirAll(KPath);
	ret=file.Replace(fs,_L("C:\\system\\data\\WIDTHS.TMP"),EFileStream|EFileWrite);
	if (ret!=KErrNone)
		{
		_LIT(KLog,"Doing Replace on file C:\\system\\data\\WIDTHS.TMP gave error %d");
		INFO_PRINTF2(KLog,ret);
		}
	TEST(ret==KErrNone);

	TBuf8<120> buf;

	for (i=0; i<device->NumTypefaces(); i++)
		{
		device->TypefaceSupport(support,i);
		spec.iTypeface=support.iTypeface;
		for (TInt k=0; k<support.iNumHeights; k++)
			{
			spec.iHeight=device->FontHeightInTwips(i,k);
			if ((spec.iHeight==1000))
				{
				for (TInt j=0; j<4; j++)
					{
					name.Copy(support.iTypeface.iName);
					buf.Format(_L8("FontInfo %S"),&support.iTypeface.iName);
					ret=file.Write(buf);
					TEST(ret==KErrNone);
					if (j==0)
						{
						buf.Format(_L8("Std%d"),spec.iHeight/20);
						ret=file.Write(buf);
						TEST(ret==KErrNone);
						spec.iFontStyle.SetPosture(EPostureUpright);
						spec.iFontStyle.SetStrokeWeight(EStrokeWeightNormal);
						}
					if (j==1)
						{
						buf.Format(_L8("Bold%d"),spec.iHeight/20);
						ret=file.Write(buf);
						TEST(ret==KErrNone);
						spec.iFontStyle.SetPosture(EPostureUpright);
						spec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
						}
					if (j==2)
						{
						buf.Format(_L8("Italic%d"),spec.iHeight/20);
						ret=file.Write(buf);
						TEST(ret==KErrNone);
						spec.iFontStyle.SetPosture(EPostureItalic);
						spec.iFontStyle.SetStrokeWeight(EStrokeWeightNormal);
						}
					if (j==3)
						{
						buf.Format(_L8("BoldItalic%d"),spec.iHeight/20);
						ret=file.Write(buf);
						TEST(ret==KErrNone);
						spec.iFontStyle.SetPosture(EPostureItalic);
						spec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
						}
					ret=device->GetNearestFontToDesignHeightInTwips((CFont *&) font,spec);
					TEST(ret==KErrNone);
					INFO_PRINTF4(_L("Name %S Height %d CharWidthInPixels(W) %d"),&name,font->FontSpecInTwips().iHeight,font->CharWidthInPixels(TChar('W')));
					buf.Format(_L8("MaxNormalCharWidth %dAscent %d"),font->MaxNormalCharWidthInPixels(),font->AscentInPixels());
					ret=file.Write(buf);
					TEST(ret==KErrNone);
					ret=file.Write(_L8("CodeSection 0:255"));
					TEST(ret==KErrNone);
					for (TInt l=0; l<256; l++)
						{
						if (font->CharWidthInPixels(k))
							{
							buf.Format(_L8("%d:%d  "),l,font->CharWidthInPixels(l));
							ret=file.Write(buf);
							TEST(ret==KErrNone);
							}
						if (((l/8)*8)==l)
							{
							ret=file.Write(_L8("\n"));
							TEST(ret==KErrNone);
							}
						}
					ret=file.Write(_L8("\nEndCodeSection\nEndFontInfo\n"));
					TEST(ret==KErrNone);
					device->ReleaseFont(font);
					}
				}
			}
		}
	file.Flush();
	file.Close();
	fs.Close();

	// Create control
	INFO_PRINTF1(_L("Create control"));
  
	CPrinterPort* printerport=NULL;
#if defined (__WINS__)
	if (device->Model().iRequiresPrinterPort)
		{
		TRAP(ret,printerport=CFilePrinterPort::NewL(_L("c:\\Printfile")));
		}
#else 
	if (device->Model().iRequiresPrinterPort)
		{
		TRAP(ret,printerport=CFilePrinterPort::NewL(_L("c:\\temp.prn")));
		INFO_PRINTF2(_L("ret %d"),ret);
		INFO_PRINTF2(_L("device displaymode %d"),device->DisplayMode());
		}
#endif

	TEST(ret==KErrNone);
	TRAP(ret,device->CreateControlL(printerport));
	INFO_PRINTF2(_L("CreateControlL() returns %d"), ret);
	TEST(ret==KErrNone);
	
	// Create context
	TRAP(ret,testPrint(device));
	TEST(ret==KErrNone);
	INFO_PRINTF2(_L("...testPrint() returns %d"), ret);

	INFO_PRINTF1(_L("Delete printer driver"));	

	delete driver;
	delete iStopper;

	__UHEAP_MARKEND;
	}
	
	

void CTPdr::bandTest(TBandingDirection aBandingDirection, TPtrC aName)
	{
	CBandedDevice* banded = NULL;
	TInt expectedWidth=0;
	TInt expectedheight=0;
	TInt ret = KErrNone;
	TRect rect(0,0,1000,1000);
	TSize size;
	size.iWidth = size.iHeight = 2000;
	TRect outRect;

	if( aBandingDirection==EBandingLeftToRight || aBandingDirection==EBandingRightToLeft )
		{
		expectedWidth=32;
		expectedheight=1000;
		}
	else if( aBandingDirection==EBandingBottomToTop || aBandingDirection==EBandingTopToBottom )
		{
		expectedWidth=1000;
		expectedheight=32;
		}

	TRAP(ret, banded=CBandedDevice::NewL(rect,size,EColor256,aBandingDirection,0));
	TEST(ret==KErrNone);

	banded->NextBand();
	banded->NextBand();
	outRect = banded->BandRect();

	TEST(outRect.Width() == expectedWidth && outRect.Height() == expectedheight);
	
	INFO_PRINTF2(_L("*** BandRect call (%s) ***"), &aName[0]);
	INFO_PRINTF3(_L("Expected results are Width=%d, Height=%d."), expectedWidth, expectedheight);
	INFO_PRINTF3(_L("Actual results are Width=%d, Height=%d."), outRect.Width(), outRect.Height());
	INFO_PRINTF5(_L("Tlx: %d, Tly: %d, Brx: %d, Bry: %d"), outRect.iTl.iX, outRect.iTl.iY, outRect.iBr.iX, outRect.iBr.iY);

	delete banded;
	}

void CTPdr::testPdrBandRect()
	{
	INFO_PRINTF1(_L("Running BandRect tests"));

	__UHEAP_MARK;
	bandTest(EBandingLeftToRight, _L("EBandingLeftToRight"));
	bandTest(EBandingBottomToTop, _L("EBandingBottomToTop"));
	bandTest(EBandingRightToLeft, _L("EBandingRightToLeft"));
	bandTest(EBandingTopToBottom, _L("EBandingTopToBottom"));
	__UHEAP_MARKEND;
	
	INFO_PRINTF1(_L("BandRect tests completed"));
	}


void CTPdr::RunTestCaseL(TInt aCurTestCase)
    {
	((CTPdrStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
		case 1:
			{
			INFO_PRINTF1(_L("TEST CASE1 - testPdrStore"));
/**
@SYMTestCaseID			GRAPHICS-PDR-0001
*/
			((CTPdrStep*)iStep)->SetTestStepID(_L("GRAPHICS-PDR-0001"));
			do 
				{
				testPdrStore();
				iModelIndex++;
				}
			while (iModelIndex < iNumModels);
			INFO_PRINTF1(_L("TEST CASE1 - ended!\n"));
			break;				
			}
		case 2:
			{
			INFO_PRINTF1(_L("TEST CASE2 - testPdrBandRect"));
/**
@SYMTestCaseID			GRAPHICS-PDR-0002
*/
			((CTPdrStep*)iStep)->SetTestStepID(_L("GRAPHICS-PDR-0002"));
			testPdrBandRect();
			INFO_PRINTF1(_L("TEST CASE2 - ended!\n"));
			break;	
			}
		case 3:
			{
			((CTPdrStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTPdrStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
			break;	
			}
		}
		((CTPdrStep*)iStep)->RecordTestResultL();
    }



//--------------
__CONSTRUCT_STEP__(Pdr)

void CTPdrStep::TestSetupL()
	{
	FbsStartup();
	User::LeaveIfError(RFbsSession::Connect());
	}
	
void CTPdrStep::TestClose()
	{
	RFbsSession::Disconnect();
	}


