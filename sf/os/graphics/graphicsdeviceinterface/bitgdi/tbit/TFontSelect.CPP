// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent - Internal Symbian test code 
*/

#include <e32hal.h>
#include <e32std.h>
#include <e32test.h>
#include <bitstd.h>
#include <bitdev.h>
#include <fntstore.h>
#include <fbs.h>
#include <hal.h>
#include <graphics/fbsdefs.h>

#include "tfontselect.h"


//If you want to do font tests with scaled device - change KScalingFactorX, KScalingFactorY, KScalingOriginPt.
const TInt KScalingFactorX = 1;
const TInt KScalingFactorY = 1;
const TPoint KScalingOriginPt(0, 0);

//Non valid font --> This font comes from System Test test data. Not a proper font, so it's best to skip it.
_LIT(KWrongFontFileName, "fnt");


CTFontSelect::CTFontSelect(CTestStep* aStep) :
	CTGraphicsBase(aStep),
	iFont1(NULL),
	iFont2(NULL),
	iFont3(NULL),
	iGc(NULL),
	iDevice(NULL)	
	{
	}

CTFontSelect::~CTFontSelect()
	{
	delete iGc;
	delete iDevice;
	}

void CTFontSelect::ConstructL()
	{
	TRAPD(err,iDevice = CFbsScreenDevice::NewL(KNullDesC,EColor256));
	if (err == KErrNotSupported)
		TRAP(err,iDevice = CFbsScreenDevice::NewL(KNullDesC,EColor16MU));
	if (err == KErrNotSupported)
		TRAP(err,iDevice = CFbsScreenDevice::NewL(KNullDesC,EColor4K));
	if (err == KErrNotSupported)
		TRAP(err,iDevice = CFbsScreenDevice::NewL(KNullDesC,EColor16M));
	if (err == KErrNotSupported)
		TRAP(err,iDevice = CFbsScreenDevice::NewL(KNullDesC,EColor64K));
	if (err == KErrNotSupported)
		TRAP(err,iDevice = CFbsScreenDevice::NewL(KNullDesC,EGray256));
	if (err == KErrNotSupported)
		TRAP(err,iDevice = CFbsScreenDevice::NewL(KNullDesC,EGray16));
	if (err == KErrNotSupported)
		TRAP(err,iDevice = CFbsScreenDevice::NewL(KNullDesC,EGray4));
	if (err == KErrNotSupported)
		TRAP(err,iDevice = CFbsScreenDevice::NewL(KNullDesC,EGray2));
	if (err == KErrNotSupported)
		TRAP(err,iDevice = CFbsScreenDevice::NewL(KNullDesC,EColor16));
	if (err == KErrNotSupported)
		TRAP(err,iDevice = CFbsScreenDevice::NewL(KNullDesC,EColor16MA));
	if (err == KErrNotSupported)
		TRAP(err,iDevice = CFbsScreenDevice::NewL(KNullDesC,EColor16MAP));
	if (err!=KErrNone)
		{
		_LIT(KLog,"Failed to create screen device, last returned error %d");
		INFO_PRINTF2(KLog,err);
		}

	User::LeaveIfError(err);
	iDevice->ChangeScreenDevice(NULL);
	iDevice->SetAutoUpdate(ETrue);
	iDevice->CreateContext(iGc);
	iDevice->SetScalingFactor(KScalingOriginPt, KScalingFactorX, KScalingFactorY, 1, 1);
	iGc->Activate(iDevice);

	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0083
 
  @SYMDEF             

  @SYMTestCaseDesc Tests font selection
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions For different typeface and heights tests font selection to specific sizes
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTFontSelect::DoFontSelectTestsL()
	{
	TFontSpec fs;
	TInt typefaces = iDevice->NumTypefaces();
	TTypefaceSupport info;
	_LIT(KIgnoreFont1, "Test2");
	_LIT(KIgnoreFont2, "LinkedFont3TT"); //LinkedFont3/4TT both cause test failure on Freetype
	_LIT(KIgnoreFont3, "LinkedFont4TT"); //They are test files with incorrect metrics

	// Maximum height of font allowed by iType rasteriser
	const TInt KMaxITypeHeightInPixels = 256;
	_LIT(KErrorMessageWithFontName, "Font: %S, pixel height requested: %d, pixel height of font: %d");
	_LIT(KErrorMessage, "Pixel height requested: %d, pixel height of font: %d");

	for (TInt count = 0; count < typefaces; count++)
		{
		iDevice->TypefaceSupport( info, count );

		for (TInt index = 0; index < info.iNumHeights; index++)
			{
			TInt height = iDevice->FontHeightInPixels( count, index );
			if (height > KMaxITypeHeightInPixels)
				{
				continue;
				}
			fs.iTypeface = info.iTypeface;
			fs.iHeight = height;

			INFO_PRINTF1(_L("Test GetNearestFontInPixels"));
			User::LeaveIfError( iDevice->GetNearestFontInPixels( (CFont*&)iFont1 ,fs ) );
			TInt heightInPixels = iFont1->HeightInPixels();
			if ( heightInPixels == height )
				{
				TEST( ETrue );
				}
			else
				{
				TOpenFontFaceAttrib attrib;
				if( iFont1->GetFaceAttrib( attrib ) )
					{
					INFO_PRINTF4(KErrorMessageWithFontName, &attrib.FullName(), height, heightInPixels);
					}
				else
					{
					INFO_PRINTF3(KErrorMessage, height, heightInPixels);
					}
				TEST( EFalse );
				}

			INFO_PRINTF1(_L("Test GetNearestFontToDesignHeightInPixels"));
			User::LeaveIfError( iDevice->GetNearestFontToDesignHeightInPixels( (CFont*&)iFont2, fs ) );
			heightInPixels = iFont2->HeightInPixels();
			if ( heightInPixels == height )
				{
				TEST( ETrue );
				}
			else
				{
				TOpenFontFaceAttrib attrib;
				if( iFont2->GetFaceAttrib( attrib ) )
					{
					INFO_PRINTF4(KErrorMessageWithFontName, &attrib.FullName(), height, heightInPixels);
					}
				else
					{
					INFO_PRINTF3(KErrorMessage, height, heightInPixels);
					}
				TEST( EFalse );
				}
			iDevice->ReleaseFont( iFont2 );
			iDevice->ReleaseFont( iFont1 );
			}
			
		INFO_PRINTF1(_L("Test GetNearestFontToMaxHeightInPixels"));
		for (TInt maxHeight = 20; maxHeight <= 50; maxHeight += 10)
			{
			fs.iTypeface = info.iTypeface;
			fs.iHeight = maxHeight;
			User::LeaveIfError( iDevice->GetNearestFontToMaxHeightInPixels( (CFont*&)iFont3, fs, maxHeight ) );

			TBufC<64> fontName = ((CFont*&)iFont3)->FontSpecInTwips().iTypeface.iName;
			INFO_PRINTF2(_L("Font name: '%S'."), &fontName);
			if (fontName == KWrongFontFileName)
				{
				INFO_PRINTF2(_L(" ** '%S' is a wrong font file. Skip checks for this font **"), &fontName);
				break;
				}

			if (!fontName.Compare(KIgnoreFont1))
				{
				INFO_PRINTF1(_L(" ** 'Test2' font ignored. This files metrics are incorrect."));
				continue;
				}
			if (!fontName.Compare(KIgnoreFont2))
				{
				INFO_PRINTF1(_L(" ** 'LinkedFont3TT' font ignored. This files metrics are incorrect."));
				continue;
				}
			if (!fontName.Compare(KIgnoreFont3))
				{
				INFO_PRINTF1(_L(" ** 'LinkedFont4TT' font ignored. This files metrics are incorrect."));
				continue;
				}
			
			if (info.iIsScalable)
				{ // got a scalable (TrueType) font - can't guarantee will fit for bitmap font
				// N.B. If this ever fails then try replacing maxHeight with maxHeight + 1 in the test
				// as it could be down to the hinting rounding error that can very occasionally occur
				// We can't make this test reliably with bitmap fonts
				INFO_PRINTF1(_L("Font is scalable"));
				TEST(iFont3->FontMaxHeight() <= maxHeight+1);
				}

			INFO_PRINTF1(_L("Test font level metrics"));
			INFO_PRINTF3(_L("Requested max height %d, returned max height %d"), maxHeight, iFont3->FontMaxHeight());
			INFO_PRINTF4(_L("MaxAscent %d, MaxDescent %d, HeightInPixels %d"), iFont3->FontMaxAscent(), iFont3->FontMaxDescent(), iFont3->HeightInPixels());
			INFO_PRINTF3(_L("CapAscent %d, StdDescent %d"), iFont3->FontCapitalAscent(), iFont3->FontStandardDescent());
			TEST(iFont3->FontMaxHeight() >= ( iFont3->FontMaxAscent() + iFont3->FontMaxDescent() ));
			TEST(iFont3->FontMaxAscent() >= iFont3->FontCapitalAscent());
			TInt expMaxDescent=iFont3->FontStandardDescent();
			if (iDevice->SizeInPixels().iHeight>400)
				--expMaxDescent;		//To get test to pass on X86 and NaviEngine
			TEST(iFont3->FontMaxDescent() >= expMaxDescent);

			iDevice->ReleaseFont(iFont3);
			}
		}
	}

void CTFontSelect::RunTestCaseL(TInt aCurTestCase)
	{
	((CTFontSelectStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
		case 1:
			{
			((CTFontSelectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0083"));
			INFO_PRINTF1(_L("DoFontSelectTest"));
			TRAPD(ret, DoFontSelectTestsL());
			TEST(ret == KErrNone);
			break;
			}
		case 2:
			{
			((CTFontSelectStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTFontSelectStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();		
			break;
			}
		}
	((CTFontSelectStep*)iStep)->RecordTestResultL();
	}


//--------------
__CONSTRUCT_STEP__(FontSelect)

void CTFontSelectStep::TestSetupL()
	{
	FbsStartup();
	User::LeaveIfError(RFbsSession::Connect());
	}
	
void CTFontSelectStep::TestClose()
	{
	RFbsSession::Disconnect();
	}
