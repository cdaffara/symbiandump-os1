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

#include "toutlineandshadowfonts.h"
#include <test/testexecutestepbase.h>
#include <openfont.h>
#include <test/thashreferenceimages.h>
#include <graphics/fbsdefs.h>
#include "fbsmessage.h"

//Note: This test should run with a rasterizer which supports EFourColourBlendGlyphBitmap (e.g. iType)
//otherwise the tests will fail.

//Path where reference mbm files get created, these reference bitmaps are useful when debugging.
_LIT(KPath, "c:\\bitgditest\\%S.mbm"); 

// Constructor
CTOutlineAndShadowFonts::CTOutlineAndShadowFonts(CTestStep* aStep):CTGraphicsBase(aStep),iStep(aStep) { }

void CTOutlineAndShadowFonts::ConstructL()
	{
	if (RFbsSession::Connect() != KErrNone)
		{
		FbsStartup();
		TInt conret = RFbsSession::Connect();
		TEST2(conret, KErrNone);
		}
	iFbs = RFbsSession::GetSession();
	iHashUtil = CTHashReferenceImages::NewL(iStep,iFbs,&KPath);
	}

// Destructor
CTOutlineAndShadowFonts::~CTOutlineAndShadowFonts()
	{
	FreeScreenDevice();
	RFbsSession::Disconnect();
	delete iHashUtil;
	}


/**
Creates screen device for the given display mode if supported
@param aMode display mode for which device has to be created
*/
void CTOutlineAndShadowFonts::CreateScreenDeviceL(TDisplayMode aMode)
	{
	iBitmap = new(ELeave) CFbsBitmap;
	//Considering the intersection of the screen size for both hw and emulator so that same
	//baselined hash can be used to test for both
	iBitmap->Create(TSize(KScreenWidth, KScreenHeight), aMode);
	iBitmapDevice = CFbsBitmapDevice::NewL(iBitmap);
	User::LeaveIfError(iBitmapDevice->CreateContext((CGraphicsContext*&)iGc));
	iHashUtil->SetScreenDeviceAndBitmap(iBitmapDevice,iBitmap,iGc);
	}

/**
Delete the devices created and gc.
*/
void CTOutlineAndShadowFonts::FreeScreenDevice()
	{
	delete iBitmapDevice;
	delete iBitmap;
	delete iGc;
	iBitmapDevice = NULL;
	iBitmap = NULL;
	iGc = NULL;
	}


/**
Auxilary function will be called in case of a leave to release the font
@param aCleanFont font to be released
*/
void CleanUpFont(TAny* aCleanFont)
	{
	SCleanFont* cleanFont= static_cast<SCleanFont*>(aCleanFont);
	(cleanFont->device)->ReleaseFont(cleanFont->font);
	}

TBool CTOutlineAndShadowFonts::CheckMonoTypeInstalledL()
	{
	CreateScreenDeviceL(EColor16MU);
//
	TFontSpec fontSpec;
	fontSpec.iTypeface.iName = KFontFace[0];
	fontSpec.iHeight = KHeight;
	fontSpec.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
	fontSpec.iFontStyle.SetEffects(FontEffect::EDropShadow, EDropShadowOn);
	fontSpec.iFontStyle.SetEffects(FontEffect::EOutline, EOutlineOn);
	CFont* font;
	User::LeaveIfError(iBitmapDevice->GetNearestFontToDesignHeightInPixels((CFont*&)font,fontSpec));
	TBool monoTypeInstalled=font->FontSpecInTwips().iFontStyle.BitmapType()==EFourColourBlendGlyphBitmap;
	iBitmapDevice->ReleaseFont(font);
	if (!monoTypeInstalled)
		{
		INFO_PRINTF1(_L("Monotype fonts not installed, Run iType\\binaries\\ityperast.cmd to install them"));
		TEST(EFalse);
		TestComplete();
		}
	return(monoTypeInstalled);
	}

/**
@SYMTestCaseID GRAPHICS-BITGDI-OUTLINESHA-0008

@SYMPREQ PREQ1543

@SYMTestCaseDesc Checks the fonts metrics. Tests the metrics with and without outline and shadow effects
for different typefaces and screen modes

@SYMTestPriority Critical

@SYMTestStatus Implemented

@SYMTestActions Create two fonts, one with and without shadow and outline effects on.
Get the font metrics using GetNearestFontToDesignHeightInPixels
Compare the different font metrics for both the fonts.
API Calls:
CTOutlineAndShadowFonts::DoOutlineandShadowFontsMetrics()

@SYMTestExpectedResults font metrics with and without outline and shadow effects should not vary by more than 3.
*/
void CTOutlineAndShadowFonts::TestOutlineandShadowFontsMetricsL()
	{
	INFO_PRINTF1(_L("TestOutlineandShadowFontsMetricsL()"));
	FreeScreenDevice();
	CreateScreenDeviceL(EColor16MU); //Create device for EColor16MU
	for (TInt fontFaceIndex = 0; fontFaceIndex < KNumOfFontFaces; fontFaceIndex++)
		{
		for (TInt numOfSubTests = 0; numOfSubTests < KNumberOfSubTestsInTest0008; numOfSubTests++)
			{
			DoOutlineandShadowFontsMetrics(KTestSettingsToTestFontMetrics[numOfSubTests], fontFaceIndex);
			}
		}
	}

/** 
Utility function to determine whether a specified value lies within a defined range of values.
@param aMin The lower value of the range.
@param aValue The value to be compared.
@param aMax The higher value of the range.
@return ETrue, if the specified value lies within the range; EFalse, otherwise.
*/
TBool CTOutlineAndShadowFonts::TestRange(TInt aMin, TInt aValue, TInt aMax)
	{
	if (Rng(aMin, aValue, aMax))
		{
		return ETrue;
		}
	else
		{
		INFO_PRINTF4(_L("TestRange: !(%i !<= %i !<= %i)"), aMin, aValue, aMax);
		return EFalse;
		}
	}

/** 
Utility function to determine whether a specified value is below a certain value.
@param aValue The value to be compared.
@param aMax The highest permitted value.
@return ETrue, if the specified value is below the maximum; EFalse, otherwise.
*/
TBool CTOutlineAndShadowFonts::TestLessThanOrEqual(TInt aValue, TInt aMax)
	{
	if (aValue <= aMax)
		{
		return ETrue;
		}
	else
		{
		INFO_PRINTF3(_L("TestLessThanOrEqual: !(%i !<= %i)"), aValue, aMax);
		return EFalse;
		}
	}

/**
Auxilary function called from TestOutlineandShadowFontsMetricsL
@param aSettings holds the effects settings (outline, shadow etc...)
@param aFontIndex holds the font index
*/
void CTOutlineAndShadowFonts::DoOutlineandShadowFontsMetrics(const STestFontParameters &aSettings, TInt aFontIndex)
	{
	TFontSpec fontSpecWithOutEffects;
	//Create fontspec without shadow and outline effects on
	PrepareTestEnvironment(fontSpecWithOutEffects, aSettings , aFontIndex, EFalse, EFalse);
	CFbsFont* font1 = NULL;
	TEST(iBitmapDevice->GetNearestFontToDesignHeightInPixels((CFont*&)font1,fontSpecWithOutEffects) == KErrNone);
	TEST(font1 != NULL);

	TFontSpec fontSpecWithEffects;
	//Create fontspec with shadow and outline effects on if asked for
	PrepareTestEnvironment(fontSpecWithEffects, aSettings, aFontIndex, ETrue, EFalse);
	CFbsFont* font2 = NULL;
	TEST(iBitmapDevice->GetNearestFontToDesignHeightInPixels((CFont*&)font2,fontSpecWithEffects) == KErrNone);
	TEST(font2 != NULL);

	//get the name of the fonts
	TOpenFontFaceAttrib font1Attrib;
	TBool haveFont1Attrib;
	haveFont1Attrib = font1->GetFaceAttrib(font1Attrib);
	INFO_PRINTF1(_L("Font names"));
	if (haveFont1Attrib)
		{
		INFO_PRINTF1(font1Attrib.FullName());
		}
	TOpenFontFaceAttrib font2Attrib;
	TBool haveFont2Attrib;
	haveFont2Attrib = font2->GetFaceAttrib(font2Attrib);
	if (haveFont2Attrib)
		{
		INFO_PRINTF1(font2Attrib.FullName());
		}

	//Check different font metrics with and without outline and shadow effects
	TUid uid1 = font1->TypeUid();
	TUid uid2 = font2->TypeUid();
	ASSERT_EQUALS(KCFbsFontUid, uid1);
	ASSERT_EQUALS(KCFbsFontUid, uid2);
	TInt valueFontWithoutEffects = font1->AscentInPixels();
	TInt valueFontWithEffects = font2->AscentInPixels();
	TEST(valueFontWithoutEffects == valueFontWithEffects);

	valueFontWithoutEffects = font1->BaselineOffsetInPixels();
	valueFontWithEffects = font2->BaselineOffsetInPixels();
	TEST(valueFontWithoutEffects == valueFontWithEffects);
	for (TInt c=0;c<256;c++)
		{
		valueFontWithoutEffects = font1->CharWidthInPixels(c);
		valueFontWithEffects = font2->CharWidthInPixels(c);
		TEST(valueFontWithoutEffects == valueFontWithEffects);
		}
	//CharactersJoin
	valueFontWithoutEffects = font1->DescentInPixels();
	valueFontWithEffects = font2->DescentInPixels();
	TEST(valueFontWithoutEffects == valueFontWithEffects);

	//Extended function
	valueFontWithoutEffects = font1->FontCapitalAscent();
	valueFontWithEffects = font2->FontCapitalAscent();
	TEST(valueFontWithoutEffects == valueFontWithEffects);

	valueFontWithoutEffects = font1->FontLineGap();
	valueFontWithEffects = font2->FontLineGap();
	TEST(valueFontWithoutEffects == valueFontWithEffects);

	valueFontWithoutEffects = font1->FontMaxAscent();
	valueFontWithEffects = font2->FontMaxAscent();
	TEST(valueFontWithoutEffects == valueFontWithEffects);

	valueFontWithoutEffects = font1->FontMaxDescent();
	valueFontWithEffects = font2->FontMaxDescent();
	TEST(valueFontWithoutEffects == valueFontWithEffects);

	valueFontWithoutEffects = font1->FontMaxHeight();
	valueFontWithEffects = font2->FontMaxHeight();
	TEST(valueFontWithoutEffects == valueFontWithEffects);

	//FontSpecInTwips -  will be different
	TFontSpec spec = font1->FontSpecInTwips();
	TFontSpec spec2 = font2->FontSpecInTwips();
	TEST(spec.iHeight == spec2.iHeight);
	valueFontWithoutEffects = font1->FontStandardDescent();
	valueFontWithEffects = font2->FontStandardDescent();
	TEST(valueFontWithoutEffects+KDifferByOne >= valueFontWithEffects);

	valueFontWithoutEffects = font1->HeightInPixels();
	valueFontWithEffects = font2->HeightInPixels();
	TEST(valueFontWithoutEffects == valueFontWithEffects);

	valueFontWithoutEffects = font1->MaxCharWidthInPixels();
	valueFontWithEffects = font2->MaxCharWidthInPixels();
	TEST(valueFontWithoutEffects == valueFontWithEffects);

	valueFontWithoutEffects = font1->MaxNormalCharWidthInPixels();
	valueFontWithEffects = font2->MaxNormalCharWidthInPixels();
	TEST(valueFontWithoutEffects == valueFontWithEffects);

	_LIT(KMeasureText,"ABCdef456");
	CFont::TMeasureTextOutput out1;
	CFont::TMeasureTextOutput out2;
	valueFontWithoutEffects = font1->MeasureText(KMeasureText,NULL,&out1);
	valueFontWithEffects = font2->MeasureText(KMeasureText,NULL,&out2);
	TEST(valueFontWithoutEffects == valueFontWithEffects);
	TEST(out1.iChars == out2.iChars);
	TEST(out1.iGlyphs == out2.iGlyphs);
	TEST(out1.iGroups == out2.iGroups);
	TEST(out1.iSpaces == out2.iSpaces);
	TEST(TestLessThanOrEqual(out1.iBounds.iTl.iX-KDifferByOne, out2.iBounds.iTl.iX));
	TEST(TestLessThanOrEqual(out1.iBounds.iTl.iY-KDifferByOne, out2.iBounds.iTl.iY));
	TEST(TestLessThanOrEqual(out1.iBounds.iBr.iX-KDifferByOne, out2.iBounds.iBr.iX));
	TEST(TestLessThanOrEqual(out1.iBounds.iBr.iY-KDifferByOne, out2.iBounds.iBr.iY));
	TEST(TestLessThanOrEqual(out1.iMaxGlyphSize.iHeight-KDifferByOne, out2.iMaxGlyphSize.iHeight));
	TEST(TestLessThanOrEqual(out1.iMaxGlyphSize.iWidth-KDifferByOne, out2.iMaxGlyphSize.iWidth));
	valueFontWithoutEffects = font1->TextCount(KMeasureText, 30);
	valueFontWithEffects = font2->TextCount(KMeasureText, 30);
	TEST(valueFontWithoutEffects == valueFontWithEffects);
	TInt val3;
	TInt val4;
	valueFontWithoutEffects = font1->TextCount(KMeasureText, 30, val3);
	valueFontWithEffects = font2->TextCount(KMeasureText, 30, val4);
	TEST(val3 == val4);
	TEST(valueFontWithoutEffects == valueFontWithEffects);
	valueFontWithoutEffects = font1->TextWidthInPixels(KMeasureText);
	valueFontWithEffects = font2->TextWidthInPixels(KMeasureText);
	TEST(TestRange(valueFontWithoutEffects, valueFontWithEffects, valueFontWithoutEffects+KDifferByThree));
	uid1 = font1->TypeUid();
	uid2 = font2->TypeUid();
	TEST(uid1 == uid2);
	for (TInt c=0;c<256;c++)
		{
		const TUint8 *p1 = NULL;
		const TUint8 *p2 = NULL;
		TCharacterMetrics m1 = font1->CharacterMetrics(c, p1);
		TCharacterMetrics m2 = font2->CharacterMetrics(c, p2);
		TEST(TestLessThanOrEqual(m1.iAscentInPixels-KDifferByOne, m2.iAscentInPixels));
		TEST(TestLessThanOrEqual(m1.iHeightInPixels-KDifferByOne, m2.iHeightInPixels));
		TEST(TestLessThanOrEqual(m1.iLeftAdjustInPixels-KDifferByTwo, m2.iLeftAdjustInPixels));
		TEST(m1.iMoveInPixels == m2.iMoveInPixels);
		TEST(TestLessThanOrEqual(m1.iRightAdjustInPixels-KDifferByTwo, m2.iRightAdjustInPixels));
		}
	TOpenFontMetrics ofm1;
	TOpenFontMetrics ofm2;
	TBool r1 = font1->GetFontMetrics(ofm1);
	TBool r2 = font2->GetFontMetrics(ofm2);
	TEST(ofm1.Size() == ofm2.Size());
	TEST(ofm1.Ascent() == ofm2.Ascent());
	TEST(ofm1.Descent() == ofm2.Descent());
	TEST(ofm1.MaxHeight() == ofm2.MaxHeight());
	TEST(ofm1.MaxDepth() == ofm2.MaxDepth());
	TEST(ofm1.MaxWidth() == ofm2.MaxWidth());

	valueFontWithoutEffects = font1->RawTextWidthInPixels(KMeasureText);
	valueFontWithEffects = font2->RawTextWidthInPixels(KMeasureText);
	TEST(valueFontWithoutEffects == valueFontWithEffects);

	valueFontWithoutEffects = font1->TextWidthInPixels(KMeasureText);
	valueFontWithEffects = font2->TextWidthInPixels(KMeasureText);
	TEST(TestLessThanOrEqual(valueFontWithoutEffects, valueFontWithEffects));

	SCharWidth scw1;
	SCharWidth scw2;
	font1->TextWidthInPixels(KMeasureText,scw1);
	font2->TextWidthInPixels(KMeasureText,scw2);
	TEST(TestLessThanOrEqual(scw1.iLeftAdjust-KDifferByOne, scw2.iLeftAdjust));
	TEST(scw1.iMove == scw2.iMove);
	TEST(TestLessThanOrEqual(scw1.iRightAdjust-KDifferByTwo, scw2.iRightAdjust));
	TEST(TestLessThanOrEqual(scw1.iWidth-KDifferByOne, scw2.iWidth));
	iBitmapDevice->ReleaseFont(font2);
	iBitmapDevice->ReleaseFont(font1);
	}

/**
@SYMTestCaseID GRAPHICS-BITGDI-OUTLINESHA-0009

@SYMPREQ PREQ1543

@SYMTestCaseDesc Test the setter and getters for pen colour, brush colour and shadow colour.

@SYMTestPriority Critical

@SYMTestStatus Implemented

@SYMTestActions Set colors to pen, brush and shadow call SetPenColor(), SetBrushColor(), SetShadowColor() respectively
Get colors of pen, brush and shadow using PenColor(), BrushColor() and GetShadowColor() respectively

@SYMTestExpectedResults pen, brush and shadow colour should return default colours if not set,
else should return the colour set using SetPenColor(), SetBrushColor() and SetShadowColor() respectively
*/
void CTOutlineAndShadowFonts::TestOutlinePenAndShadowColorL()
	{
	INFO_PRINTF1(_L("TestOutlinePenAndShadowColorL()"));
	for(TInt displayMode = ENone; displayMode < KNumOfDisplayModes; displayMode++)
		{
		FreeScreenDevice();
		CreateScreenDeviceL(KDisplayModes[displayMode]);
		DoOutlinePenAndShadowColor();
		}
	}

/**
Auxilary function called from TestOutlinePenAndShadowColorL
API Calls:
CFbsBitGc::PenColor()
CFbsBitGc::SetPenColor()
CFbsBitGc::BrushColor()
CFbsBitGc::SetBrushColor()
CFbsBitGc::GetShadowColor()
CFbsBitGc::SetShadowColor()
*/
void CTOutlineAndShadowFonts::DoOutlinePenAndShadowColor()
	{
	//iGc->Reset();
	TRgb color;
	color = iGc->PenColor();

	//Pen colour is used for drawing outline
	TEST(color == KDefaultOutlinePenColor);
	color = iGc->BrushColor();

	//Brush colour is used as fill colour
	TEST(color == KDefaultFillColor);
	iGc->GetShadowColor(color);
	TEST(color == KDefaultShadowColor);
	iGc->SetShadowColor(KRgbGreen);
	iGc->GetShadowColor(color);
	TEST(color == KRgbGreen);
	iGc->SetPenColor(KRgbYellow);
	color = iGc->PenColor();
	TEST(color == KRgbYellow);
	iGc->SetBrushColor(KRgbRed);
	color = iGc->BrushColor();
	TEST(color == KRgbRed);
	}

/**
@SYMTestCaseID GRAPHICS-BITGDI-OUTLINESHA-0010

@SYMPREQ PREQ1543

@SYMTestCaseDesc Tests the text drawn using DrawTextVertical and DrawText with shadow and outline effects on
with different display modes and orientations.

@SYMTestPriority Critical

@SYMTestStatus Implemented

@SYMTestActions Call DrawTextVertical and DrawText with different effect on, display modes and orientations.
API Calls:
CFbsBitGc::DrawTextVertical()
CFbsBitGc::DrawText()
CTOutlineAndShadowFonts::CompareHashValuesL()

@SYMTestExpectedResults Hash generated for the drawn text should be same with the baselined hash value.
*/
void CTOutlineAndShadowFonts::TestDrawTextWithOutlineAndShadowL()
	{
	INFO_PRINTF1(_L("TestDrawTextWithOutlineAndShadowL()"));
	for(TInt displayMode = 0; displayMode < KNumOfDisplayModes; displayMode++)
		{
		FreeScreenDevice();
		CreateScreenDeviceL(KDisplayModes[displayMode]);
		INFO_PRINTF2(_L("Display mode: %S"), &KDisplayModeNames[displayMode]);
		for (TInt orient = CFbsBitGc::EGraphicsOrientationNormal; orient <= CFbsBitGc::EGraphicsOrientationRotated270; orient++)
			{
			iGc->SetOrientation((CFbsBitGc::TGraphicsOrientation)orient);
			for (TInt fontFaceIndex = 0; fontFaceIndex < KNumOfFontFaces; fontFaceIndex++)
				{
				for (TInt numOfSubTests = 0; numOfSubTests < KNumberOfSubTestsInTest0010; numOfSubTests++)
					{
					//Test with only one display mode i.e. EColor16MU for EMonochromeGlyphBitmap to reduce test execution time
					if ((KTestSettingsToTestOutlineAndShadow[numOfSubTests].fSpecAndGcSettings & ESetMonochromeGlyphBitmap) && (displayMode != EColor16MU))
						{
						continue;
						}

					//Test transparency for display mode EColor64K and above as it is not supported in other display modes
					if (KTestSettingsToTestOutlineAndShadow[numOfSubTests].penColor.Alpha() < 255 && displayMode < 6)
						{
						continue;
						}
					HBufC* hashIdString = iHashUtil->GenerateHashIdStringLC(KTestName0010, numOfSubTests, KFontFaceShortName, fontFaceIndex, displayMode, orient);
					DoDrawTextWithOutlineAndShadowL(KTestSettingsToTestOutlineAndShadow[numOfSubTests], *hashIdString, fontFaceIndex);
					CleanupStack::PopAndDestroy(hashIdString);
					}
				}
			}
		}
	iGc->SetOrientation(CFbsBitGc::EGraphicsOrientationNormal);
	}

/**
Auxilary function called TestDrawTextWithOutlineAndShadowL.
@param aSettings holds the effects settings (Outline, Shadow, Bitmap type etc...)
@param aHashIdString holds the HashID string
@param aFontIndex holds the font index
*/
void CTOutlineAndShadowFonts::DoDrawTextWithOutlineAndShadowL(const STestFontParameters &aSettings, const TDesC& aHashIdString, const TInt aFontIndex)
	{
	TFontSpec fontSpec;
	PrepareTestEnvironment(fontSpec, aSettings, aFontIndex, ETrue, ETrue);
	CFont* font;
	TEST(iBitmapDevice->GetNearestFontToDesignHeightInPixels((CFont*&)font, fontSpec) == KErrNone);
	SCleanFont cleanupFont = {iBitmapDevice, font};
	CleanupStack::PushL(TCleanupItem(CleanUpFont, &cleanupFont));
	iGc->UseFont(font);
	if(EVerticalDraw & aSettings.fSpecAndGcSettings)
		{
		iGc->DrawTextVertical(KTextPhrase, TPoint(20,60), ERotate90Degrees & aSettings.fSpecAndGcSettings);
		}
	else
		{
		iGc->DrawText(KTextPhrase, KPoint);
		}
	iHashUtil->CompareHashValuesL(aHashIdString);
	CleanupStack::Pop();//font
	iBitmapDevice->ReleaseFont(font);
	iGc->DiscardFont();
	}

/**
@SYMTestCaseID GRAPHICS-BITGDI-OUTLINESHA-0011

@SYMPREQ PREQ1543

@SYMTestCaseDesc Tests outline and shadow effects with strikethrough, softedge and underLine
for different orientations.

@SYMTestPriority Critical

@SYMTestStatus Implemented

@SYMTestActions Draw text with different effects on
Repeat the same with all supported display modes and orientations. Generate hash key for the drawn text.
API Calls:
CFbsBitGc::DrawText()
CTHashReferenceImages::CompareHashValuesL()

@SYMTestExpectedResults Hash generated for the drawn text should be same with the baselined hash value.
*/
void CTOutlineAndShadowFonts::TestDrawTextWithStrikeThroughL()
	{
	INFO_PRINTF1(_L("TestDrawTextWithStrikeThroughL()"));
	for(TInt displayMode = 0; displayMode < KNumOfDisplayModes; displayMode++)
		{
		INFO_PRINTF2(_L("Display mode: %S"), &KDisplayModeNames[displayMode]);
		FreeScreenDevice();
		CreateScreenDeviceL(KDisplayModes[displayMode]);

		for (TInt numOfSubTests = 0; numOfSubTests < KNumberOfSubTestsInTest0011; numOfSubTests++)
			{
			HBufC* hashIdString = iHashUtil->GenerateHashIdStringLC(KTestName0011, numOfSubTests, &(KFontFace[0]), 0, displayMode, 0);
			DoSetStrikeThroughAndDrawTestL(KTestSettingsToTestStrikeAndUnderline[numOfSubTests], *hashIdString);
			CleanupStack::PopAndDestroy(hashIdString);
			}
		}
	}

/**
Auxilary function called from TestDrawTextWithStrikeThroughL API
@SYMTestCaseDesc Tests drawn text with different effects like
strikethrough, softedge, underLine etc for different orientations.
@param aSettings holds font settings
@param aHashIdString holds HashID
*/
void CTOutlineAndShadowFonts::DoSetStrikeThroughAndDrawTestL(const STestFontParameters &aSettings, const TDesC& aHashIdString)
	{
	TFontSpec fontSpec;
	//Passing 0 as font index as testing with one typeface is sufficient in this case
	PrepareTestEnvironment(fontSpec, aSettings, 0, ETrue, EFalse);

	CFont* font;
	TEST(iBitmapDevice->GetNearestFontToDesignHeightInPixels((CFont*&)font, fontSpec) == KErrNone);
	SCleanFont cleanupFont = {iBitmapDevice, font};
	CleanupStack::PushL(TCleanupItem(CleanUpFont, &cleanupFont));
	iGc->UseFont(font);
	iGc->DrawText(KTextPhrase,KPoint);
	iHashUtil->CompareHashValuesL(aHashIdString);
	CleanupStack::Pop();//font
	iBitmapDevice->ReleaseFont(font);
	iGc->DiscardFont();
	}

/**
@SYMTestCaseID GRAPHICS-BITGDI-OUTLINESHA-0012

@SYMTestCaseDesc Negative tests.

@SYMTestPriority Critical

@SYMTestStatus Implemented

@SYMTestActions Create font spec with different -ve scenarios.
Like pass bitmap type other than valid on, -ve pen size etc.

API Calls:
CFbsBitGc::GetNearestFontToDesignHeightInPixels()
TFontStyle::SetEffects()

@SYMTestExpectedResults Tests should not panic.
*/
void CTOutlineAndShadowFonts::NegativeRastTestL()
	{
	INFO_PRINTF1(_L("NegativeRastTestL()"));
	FreeScreenDevice();
	CreateScreenDeviceL(KDisplayModes[9]); //Create device for EColor16MU

	TFontSpec fsp;
	STestFontParameters settings;

	INFO_PRINTF1(_L("Drawing empty text"));
	settings.fSpecAndGcSettings = EDropShadowOn | EOutlineOn | ESetAntiAliasedGlyphBitmap;
	PrepareTestEnvironment(fsp, settings, 0);
	iGc->SetPenSize(TSize(10,10));
	NegativeRastTestDrawText(fsp);

	INFO_PRINTF1(_L("Drawing empty text with unknown typeface"));
	fsp.iTypeface.iName=_L("ERROR FONT");  //Typeface which does not exist
	iGc->SetPenSize(TSize(3,3));
	NegativeRastTestDrawText(fsp);

	INFO_PRINTF1(_L("Drawing empty text with invalid glyph bitmap type"));
	fsp.iFontStyle.SetBitmapType((TGlyphBitmapType)KInvalidBitmapType); //Invalid glyph bitmap type
	CFont* font;
	TEST(iBitmapDevice->GetNearestFontToDesignHeightInPixels((CFont*&)font, fsp) == KErrNone);
	iGc->UseFont(font);
	iGc->DrawText(KTextPhrase,KPoint);
	TEST(font->FontSpecInTwips().iFontStyle.BitmapType() == EMonochromeGlyphBitmap);
	iBitmapDevice->ReleaseFont(font);
	iGc->DiscardFont();
	
	INFO_PRINTF1(_L("Drawing text with brush style ENullBrush"));
	iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	settings.fSpecAndGcSettings = EDropShadowOn | EOutlineOn | ESetAntiAliasedGlyphBitmap;
	PrepareTestEnvironment(fsp, settings, 0);
	NegativeRastTestDrawText(fsp);
	
	INFO_PRINTF1(_L("Drawing text with brush style ESolidBrush"));
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	NegativeRastTestDrawText(fsp);
	
	INFO_PRINTF1(_L("Drawing text with large pen size"));
	iGc->SetPenSize(TSize(1000,1000));
	NegativeRastTestDrawText(fsp);

	INFO_PRINTF1(_L("Drawing text with -ve pen size"));
	settings.fSpecAndGcSettings = EDropShadowOn | ESetAntiAliasedGlyphBitmap;
	PrepareTestEnvironment(fsp, settings, 0);
	iGc->SetPenSize(TSize(-1,-1));
	NegativeRastTestDrawText(fsp);
	}

/**
Auxilary function called from NegativeRastTestL
@param aFontSpec holds fonts specifications
*/
void CTOutlineAndShadowFonts::NegativeRastTestDrawText(const TFontSpec &aFontSpec)
	{
	CFont* font;
	TEST(iBitmapDevice->GetNearestFontToDesignHeightInPixels((CFont*&)font, aFontSpec) == KErrNone);
	iGc->UseFont(font);
	iGc->DrawText(KTextPhrase, KPoint);
	iGc->DrawTextVertical(KTextPhrase, KPoint, EFalse);
	iGc->DiscardFont();
	iBitmapDevice->ReleaseFont(font);
	}

/**
@SYMTestCaseID GRAPHICS-BITGDI-OUTLINESHA-0013

@SYMPREQ PREQ1543

@SYMTestCaseDesc Test glyph bitmap types with outline and shadow effects on and off

@SYMTestPriority Critical

@SYMTestStatus Implemented

@SYMTestActions Create fonts with and without specifying the bitmap type
API Calls:
TFontStyle::BitmapType()

@SYMTestExpectedResults Glyph bitmap type should be EFourColourBlendGlyphBitmap when either of outline or shadow effect on and EAntiAliasedGlyphBitmap set in fontspec
*/
void CTOutlineAndShadowFonts::TestCheckBitmapTypesL()
	{
	INFO_PRINTF1(_L("TestCheckBitmapTypesL()"));
	FreeScreenDevice();
	CreateScreenDeviceL(KDisplayModes[9]); //Create device for EColor16MU

	CFont* font;
	TFontSpec fontSpec;
	STestFontParameters settings;

	PrepareTestEnvironment(fontSpec, settings, 0);
	TEST(iBitmapDevice->GetNearestFontToDesignHeightInPixels((CFont*&)font,fontSpec) == KErrNone);
	//Check for the bitmap type to be EMonochromeGlyphBitmap when no bitmap type is set.
	TEST(font->FontSpecInTwips().iFontStyle.BitmapType() == EMonochromeGlyphBitmap);
	iBitmapDevice->ReleaseFont(font);

	settings.fSpecAndGcSettings = EDropShadowOn | EOutlineOn | ESetAntiAliasedGlyphBitmap;
	PrepareTestEnvironment(fontSpec, settings, 0);
	TEST(iBitmapDevice->GetNearestFontToDesignHeightInPixels((CFont*&)font,fontSpec) == KErrNone);
	//Check for the bitmap type to be EFourColourBlendGlyphBitmap when either or both the
	//EDropShadow or EOutline effects are on.
	TEST(font->FontSpecInTwips().iFontStyle.BitmapType() == EFourColourBlendGlyphBitmap);
	iBitmapDevice->ReleaseFont(font);

	settings.fSpecAndGcSettings = EDropShadowOn | ESetAntiAliasedGlyphBitmap;
	PrepareTestEnvironment(fontSpec, settings, 0);
	TEST(iBitmapDevice->GetNearestFontToDesignHeightInPixels((CFont*&)font,fontSpec) == KErrNone);
	//Check for the bitmap type to be EFourColourBlendGlyphBitmap when either or both the
	//EDropShadow or EOutline effects are on.
	TEST(font->FontSpecInTwips().iFontStyle.BitmapType() == EFourColourBlendGlyphBitmap);
	iBitmapDevice->ReleaseFont(font);

	settings.fSpecAndGcSettings = EOutlineOn | ESetAntiAliasedGlyphBitmap;
	PrepareTestEnvironment(fontSpec, settings, 0);
	TEST(iBitmapDevice->GetNearestFontToDesignHeightInPixels((CFont*&)font,fontSpec) == KErrNone);
	//Check for the bitmap type to be EFourColourBlendGlyphBitmap when either or both the
	//EDropShadow or EOutline effects are on.
	TEST(font->FontSpecInTwips().iFontStyle.BitmapType() == EFourColourBlendGlyphBitmap);
	iBitmapDevice->ReleaseFont(font);
	}

/**
@SYMTestCaseID GRAPHICS-BITGDI-OUTLINESHA-0014

@SYMPREQ PREQ1543

@SYMTestCaseDesc OOM tests, checks OOM condition for GetNearestFontToDesignHeightInPixels.

@SYMTestPriority Critical

@SYMTestStatus Implemented

@SYMTestActions
API Calls:
CFbsBitGc::GetNearestFontToDesignHeightInPixels()
TFontStyle::SetEffects()

@SYMTestExpectedResults Checks the OOM condition and finds if it takes care of OOM cases for GetNearestFontToDesignHeightInPixels
*/
void CTOutlineAndShadowFonts::TestOOML()
	{
	INFO_PRINTF1(_L("TestOOML()"));
	FreeScreenDevice();
	CreateScreenDeviceL(KDisplayModes[9]); //Create device for EColor16MU

	//OOM for normal text
	for(TInt fontFaceIndex = 0; fontFaceIndex < KNumOfFontFaces; fontFaceIndex++)
		{
		DoOOM(fontFaceIndex, EFalse);
		}
	}

/**
Auxilary function called from TestOOML API
@SYMTestCaseDesc checks OOM condition for DrawText() and DrawTextVertical().
@param aFontSpec holds font specifications
@param aDrawVerticalText holds bool, if true calls DrawTextVertical else calls DrawText api's
*/
void CTOutlineAndShadowFonts::DoOOM(TInt aFontIndex, TBool aDrawVerticalText)
	{
	__UHEAP_RESET;
	TFontSpec fSpec;
	STestFontParameters settings;
	settings.fSpecAndGcSettings = EDropShadowOn | EOutlineOn | ESetAntiAliasedGlyphBitmap;
	PrepareTestEnvironment(fSpec, settings, aFontIndex);

	//GetNearestFontToDesignHeightInPixels
	iFbs->SendCommand(EFbsMessDefaultAllocFail,0);
	CFont* font = NULL;
	TEST(iBitmapDevice->GetNearestFontToDesignHeightInPixels(font,fSpec) == KErrNone);
	TInt ret;
	iBitmapDevice->ReleaseFont(font);
	for (TInt count = 1; ; count++)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic,count);
		__UHEAP_MARK;
		font=NULL;
		ret = iBitmapDevice->GetNearestFontToDesignHeightInPixels(font,fSpec);
		if (ret == KErrNoMemory)
			{
			ASSERT(!font);
			__UHEAP_MARKEND;
			}
		else if (ret == KErrNone)
			{
			ASSERT(font);
			iBitmapDevice->ReleaseFont(font);
			__UHEAP_MARKEND;
			break;
			}
		else
			{
			__UHEAP_MARKEND;
			TEST(ret ==  KErrNone);
			}
		iBitmapDevice->ReleaseFont(font);
		}

	//DrawText and DrawTextVertical with EDropShadow effect on and EOutline effect off
	settings.fSpecAndGcSettings = EDropShadowOn | ESetAntiAliasedGlyphBitmap;
	settings.brushColor = KRgbBlack;
	settings.penColor = KRgbGreen;
	settings.shadowColor =  KRgbRed;
	PrepareTestEnvironment(fSpec, settings, aFontIndex, ETrue, ETrue);
	GetFontAndDrawOOM(fSpec, aDrawVerticalText);

	//DrawText and DrawTextVertical with EDropShadow effect off and EOutline effect on
	settings.fSpecAndGcSettings = EOutlineOn | ESetAntiAliasedGlyphBitmap;
	PrepareTestEnvironment(fSpec, settings, aFontIndex, ETrue, ETrue);
	GetFontAndDrawOOM(fSpec, aDrawVerticalText);

	//DrawText and DrawTextVertical with EDropShadow effect on and EOutline effect on
	settings.fSpecAndGcSettings = EDropShadowOn | EOutlineOn | ESetAntiAliasedGlyphBitmap;
	PrepareTestEnvironment(fSpec, settings, aFontIndex, ETrue, ETrue);
	GetFontAndDrawOOM(fSpec, aDrawVerticalText);

	__UHEAP_RESET;
	}

/**
Auxilary function called from DoOOM API
@SYMTestCaseDesc checks OOM condition for DrawText() and DrawTextVertical().
@param aFontSpec holds font specifications
@param aDrawVerticalText holds bool value, if true calls DrawTextVertical else calls DrawText api's
*/
void CTOutlineAndShadowFonts::GetFontAndDrawOOM(TFontSpec aFontSpec, TBool aDrawVerticalText)
	{
	__UHEAP_RESET;
	_LIT(KDRAWME,"Outline and Shadow");
	CFont* font = NULL;
	iFbs->SendCommand(EFbsMessDefaultAllocFail,0);
	TInt ret= iBitmapDevice->GetNearestFontToDesignHeightInPixels((CFont*&)font, aFontSpec);
	TEST(ret ==  KErrNone);
	for (TInt count = 1; ; count++)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic,count);
		__UHEAP_MARK;
		iGc->UseFont(font);
		if (aDrawVerticalText)
			{
			iGc->DrawTextVertical(KDRAWME, TPoint(3, 4), EFalse);
			}
		else
			{
			iGc->DrawText(KDRAWME, TPoint(3, 4));
			}
		if (ret == KErrNoMemory)
			{
			iGc->DrawTextVertical(KDRAWME, TPoint(3, 4), EFalse);
			__UHEAP_MARKEND;
			}
		else if (ret == KErrNone)
			{
			ASSERT(font);
			iBitmapDevice->ReleaseFont(font);
			iGc->DiscardFont();
			__UHEAP_MARKEND;
			break;
			}
		else
			{
			__UHEAP_MARKEND;
			TEST(ret ==  KErrNone);
			}
		}
	}

/**
@SYMTestCaseID GRAPHICS-BITGDI-OUTLINESHA-0015

@SYMPREQ PREQ2555

@SYMTestCaseDesc Check bitmaptype EAntiAliasedOrMonochromeGlyphBitmap returns the correct actual
				bitmap type. This test uses the "Series 60 Sans SemiBold" font. 
				Sizes 0-7:monochrome; 8-16:anti-aliased; 16 and above:monochrome.  

@SYMTestPriority Normal

@SYMTestStatus Implemented

@SYMTestActions
Set the font size of the test font to 6, 12, 20 respectively and the bitmap type to EAntiAliasedOrMonochromeGlyphBitmap. 
Then call CFbsDevice::GetNearestFontToDesignHeightInPixels() to return the font and draw a text string on the left side of
the bitmap. Draw the same string using the reference font on the right side of the bitmap. Use CFbsDevice::RectCompare()
to compare the two halves of the bitmap.

@SYMTestExpectedResults The two halves should be the same.
*/
void CTOutlineAndShadowFonts::TestAntiAliasedOrMonochromeGlyphBitmapL()
	{
	iGc->Reset();
	FreeScreenDevice();
	CreateScreenDeviceL(EColor64K);
	_LIT(KGaspTypefaceName, "Series 60 Sans SemiBold");
	TFontSpec testFontSpec;
	testFontSpec.iTypeface.iName = KGaspTypefaceName;	
	testFontSpec.iFontStyle.SetBitmapType(EAntiAliasedOrMonochromeGlyphBitmap);
	TFontSpec refFontSpec;
	refFontSpec.iTypeface.iName = KGaspTypefaceName;	
	
	TInt fontsizes[3] = {6, 12, 20};
	CFont* testFont;
	CFont* refFont;
	TPoint leftMidPoint = TPoint(KScreenWidth/4, KScreenHeight/2);
	TPoint rightMidPoint = TPoint(KScreenWidth*3/4, KScreenHeight/2);
	_LIT(KTestString, "Abcde");
	
	//anti-aliased
	testFontSpec.iHeight = fontsizes[0];
	refFontSpec.iHeight = fontsizes[0];
	refFontSpec.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
	User::LeaveIfError(iBitmapDevice->GetNearestFontToDesignHeightInPixels((CFont*&)testFont,testFontSpec));
	iGc->UseFont(testFont);
	iGc->DrawText(KTestString, leftMidPoint);
	User::LeaveIfError(iBitmapDevice->GetNearestFontToDesignHeightInPixels((CFont*&)refFont,refFontSpec));
	iGc->UseFont(refFont);
	iGc->DrawText(KTestString, rightMidPoint);
	TBool testPassed = iBitmapDevice->RectCompare(TRect(0,0,KScreenWidth>>1,KScreenHeight), *iBitmapDevice, TRect(KScreenWidth>>1,0,KScreenWidth,KScreenHeight));
	TEST(testPassed);
	if (!testPassed)
		{
		TFontSpec fontSpec = testFont->FontSpecInTwips();
		INFO_PRINTF3(_L("Test font: %S, height in twips = %d"), &fontSpec.iTypeface.Name(), fontSpec.iHeight);
		fontSpec = refFont->FontSpecInTwips();
		INFO_PRINTF3(_L("Ref font: %S, height in twips = %d"), &fontSpec.iTypeface.Name(), fontSpec.iHeight);
		}
	iBitmapDevice->ReleaseFont(testFont);
	iBitmapDevice->ReleaseFont(refFont);
	
	//monochrome
	iGc->Clear();
	testFontSpec.iHeight = fontsizes[1];
	refFontSpec.iHeight = fontsizes[1];
	refFontSpec.iFontStyle.SetBitmapType(EMonochromeGlyphBitmap);
	User::LeaveIfError(iBitmapDevice->GetNearestFontToDesignHeightInPixels((CFont*&)testFont,testFontSpec));
	iGc->UseFont(testFont);
	iGc->DrawText(KTestString, leftMidPoint);
	User::LeaveIfError(iBitmapDevice->GetNearestFontToDesignHeightInPixels((CFont*&)refFont,refFontSpec));
	iGc->UseFont(refFont);
	iGc->DrawText(KTestString, rightMidPoint);
	testPassed = iBitmapDevice->RectCompare(TRect(0,0,KScreenWidth>>1,KScreenHeight), *iBitmapDevice, TRect(KScreenWidth>>1,0,KScreenWidth,KScreenHeight));
	TEST(testPassed);
	if (!testPassed)
		{
		TFontSpec fontSpec = testFont->FontSpecInTwips();
		INFO_PRINTF3(_L("Test font: %S, height in twips = %d"), &fontSpec.iTypeface.Name(), fontSpec.iHeight);
		fontSpec = refFont->FontSpecInTwips();
		INFO_PRINTF3(_L("Ref font: %S, height in twips = %d"), &fontSpec.iTypeface.Name(), fontSpec.iHeight);
		}
	iBitmapDevice->ReleaseFont(testFont);
	iBitmapDevice->ReleaseFont(refFont);
	
	//anti-aliased
	iGc->Clear();
	testFontSpec.iHeight = fontsizes[2];
	refFontSpec.iHeight = fontsizes[2];
	refFontSpec.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
	User::LeaveIfError(iBitmapDevice->GetNearestFontToDesignHeightInPixels((CFont*&)testFont,testFontSpec));
	iGc->UseFont(testFont);
	iGc->DrawText(KTestString, leftMidPoint);
	User::LeaveIfError(iBitmapDevice->GetNearestFontToDesignHeightInPixels((CFont*&)refFont,refFontSpec));
	iGc->UseFont(refFont);
	iGc->DrawText(KTestString, rightMidPoint);
	testPassed = iBitmapDevice->RectCompare(TRect(0,0,KScreenWidth>>1,KScreenHeight), *iBitmapDevice, TRect(KScreenWidth>>1,0,KScreenWidth,KScreenHeight));
	TEST(testPassed);
	if (!testPassed)
		{
		TFontSpec fontSpec = testFont->FontSpecInTwips();
		INFO_PRINTF3(_L("Test font: %S, height in twips = %d"), &fontSpec.iTypeface.Name(), fontSpec.iHeight);
		fontSpec = refFont->FontSpecInTwips();
		INFO_PRINTF3(_L("Ref font: %S, height in twips = %d"), &fontSpec.iTypeface.Name(), fontSpec.iHeight);
		}
	iBitmapDevice->ReleaseFont(testFont);
	iBitmapDevice->ReleaseFont(refFont);
	}
/**
Sets the fontspec and gc attributes according to the flags set in STestFontParameters for the test.
Before setting gc attributes it is cleared and reset.
@param aFontSpec holds all effective font settings
@param aSettings holds the flags for setting the fontspec and gc
@param aFontIndex the fonts index of KFontFace to be set as typeface
@param aSetEffects ETrue will set the effects on in fontspec if their flags are enabled. By default it is ETrue.
@param aSetColors ETrue will set the colours in gc from STestFontParameters. By default it is EFalse.
*/
void CTOutlineAndShadowFonts::PrepareTestEnvironment(TFontSpec &aFontSpec, const STestFontParameters &aSettings, const TInt aFontIndex, TBool aSetEffects, TBool aSetColors)
	{
	//Set the typeface from the font face array
	aFontSpec.iTypeface.iName = KFontFace[aFontIndex];
	aFontSpec.iHeight = KHeight;

	//Set the effects on if any of flags for the effects are set in aSettings and aSetEffects is ETrue
	if(aSetEffects)
		{
		aFontSpec.iFontStyle.SetEffects(FontEffect::EDropShadow, EDropShadowOn & aSettings.fSpecAndGcSettings);
		aFontSpec.iFontStyle.SetEffects(FontEffect::EOutline, EOutlineOn & aSettings.fSpecAndGcSettings);
		aFontSpec.iFontStyle.SetEffects(FontEffect::ESoftEdge, ESoftEdgeOn & aSettings.fSpecAndGcSettings);
		aFontSpec.iFontStyle.SetEffects(FontEffect::EAlgorithmicBold, EAlgorithmicBoldOn & aSettings.fSpecAndGcSettings);
		}

	iGc->Reset();
	iGc->Clear();
	iGc->SetStrikethroughStyle((EIsStrikethroughOn & aSettings.fSpecAndGcSettings) ? EStrikethroughOn : EStrikethroughOff);
	iGc->SetUnderlineStyle((EIsUnderlineOn & aSettings.fSpecAndGcSettings) ? EUnderlineOn : EUnderlineOff);

	//Set the glyph bitmap type
	if(ESetMonochromeGlyphBitmap & aSettings.fSpecAndGcSettings)
		{
		aFontSpec.iFontStyle.SetBitmapType(EMonochromeGlyphBitmap);
		}
	else if(ESetAntiAliasedGlyphBitmap & aSettings.fSpecAndGcSettings)
		{
		aFontSpec.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
		}

	//Set the colours in gc if asked for
	if(aSetColors)
		{
		iGc->SetBrushColor(aSettings.brushColor);
		iGc->SetPenColor(aSettings.penColor);
		iGc->SetShadowColor(aSettings.shadowColor);

		if (aSettings.backgroundColor != KRgbWhite)
			{
			iGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
			CFbsBitmap* bgBitmap = new CFbsBitmap;
			bgBitmap->Create(TSize(1, 1), EColor16MA);
			bgBitmap->BeginDataAccess();
			Mem::Copy(bgBitmap->DataAddress(), &(aSettings.backgroundColor), 4);
			bgBitmap->EndDataAccess(EFalse);
			iGc->DrawBitmap(TRect(TPoint(0, 0), iBitmap->SizeInPixels()), bgBitmap);
			delete bgBitmap;
			iGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
			}
		}
	}

void CTOutlineAndShadowFonts::RunTestCaseL(TInt aCurTestCase)
	{
		((CTOutlineAndShadowFontsStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch (aCurTestCase)
		{
		case 1:
			{
/**
@SYMTestCaseID GRAPHICS-BITGDI-OUTLINESHA-0007
*/
			((CTOutlineAndShadowFontsStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-OUTLINESHA-0007"));
			CheckMonoTypeInstalledL();
			break;
			}
		case 2:
			{
			((CTOutlineAndShadowFontsStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-OUTLINESHA-0008"));
			TestOutlineandShadowFontsMetricsL();
			break;
			}
		case 3:
			{
			((CTOutlineAndShadowFontsStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-OUTLINESHA-0009"));
			TestOutlinePenAndShadowColorL();
			break;
			}
		case 4:
			{
			((CTOutlineAndShadowFontsStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-OUTLINESHA-0010"));
			TestDrawTextWithOutlineAndShadowL();
			break;
			}
		case 5:
			{
			((CTOutlineAndShadowFontsStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-OUTLINESHA-0011"));
			TestDrawTextWithStrikeThroughL();
			break;
			}
		case 6:
			{
			((CTOutlineAndShadowFontsStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-OUTLINESHA-0012"));
			NegativeRastTestL();
			break;
			}
		case 7:
			{
			((CTOutlineAndShadowFontsStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-OUTLINESHA-0013"));
			TestCheckBitmapTypesL();
			break;
			}
		case 8:
			{
			((CTOutlineAndShadowFontsStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-OUTLINESHA-0015"));
			TestAntiAliasedOrMonochromeGlyphBitmapL();
			break;
			}
		case 9:
			{
			((CTOutlineAndShadowFontsStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-OUTLINESHA-0014"));
			TestOOML();
			break;
			}
		default:
			{
			((CTOutlineAndShadowFontsStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTOutlineAndShadowFontsStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
			break;
			}
		}
		((CTOutlineAndShadowFontsStep*)iStep)->RecordTestResultL();
	}

//--------------
__CONSTRUCT_STEP__(OutlineAndShadowFonts)
