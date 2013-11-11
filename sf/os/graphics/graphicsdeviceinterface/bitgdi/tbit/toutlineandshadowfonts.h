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
 @internalComponent - Internal Symbian test code
*/

#ifndef __T_OUTLINEANDSHADOWFONTS_H__
#define __T_OUTLINEANDSHADOWFONTS_H__

#include "TGraphicsHarness.h"
#include <hash.h>
#include <test/tefunit.h>
#include <test/thashreferenceimages.h>

#define KDefaultOutlinePenColor KRgbBlack
#define KDefaultShadowColor KRgbGray
#define KDefaultFillColor KRgbWhite

//Considering the intersection of the screen size for both hw and emulator so that same
//baselined hash can be used to test for both
#define	KScreenWidth 320
#define	KScreenHeight 240

const TInt KInvalidBitmapType = -10; //Used in negative test to set invalid glyph bitmap type
const TInt KHeight = 30; //Font height

// These are used in pixel spacing
const TInt KDifferByThree = 3; // Spacing 1, 2 and 3 are acceptable
const TInt KDifferByTwo = 2; 
const TInt KDifferByOne = 1;
const TPoint KPoint = TPoint(10,20); // Point passed as argument to DrawText

//Format for generating hash id
_LIT(KHashIDFormat, "%S_%d_%S_%S_%d");
const TInt KLengthOfHashValue = 256;
_LIT(KTextPhrase, "ShAdOW OUtLine * $ 1234");

//List of typefaces
const TInt KNumOfFontFaces = 3;
const TPtrC KFontFace[KNumOfFontFaces] = 
	{
	_L("DejaVu Sans Condensed"),
	_L("DejaVu Sans Mono"),
	_L("DejaVu Serif Condensed")
	};
// A short name (no spaces) version of KFontFace used for filenames and hash indexes.
const TPtrC KFontFaceShortName[KNumOfFontFaces] = 
	{
	_L("DVSans"),
	_L("DVMono"),
	_L("DVSerif")
	};

// List of display modes. Used in INFO_PRINTF's
const TInt KNumOfDisplayModes = 12;
const TPtrC KDisplayModeNames[KNumOfDisplayModes] = 
	{
	_L("EGray2"),
	_L("EGray4"),
	_L("EGray16"),
	_L("EGray256"),
	_L("EColor16"),
	_L("EColor256"),
	_L("EColor64K"),
	_L("EColor16M"),
	_L("EColor4K"),
	_L("EColor16MU"),
	_L("EColor16MA"),
	_L("EColor16MAP"),
	};

//List of display modes used for creating screen devices
const TDisplayMode KDisplayModes[KNumOfDisplayModes] = 
	{
	EGray2,
	EGray4,
	EGray16,
	EGray256,
	EColor16,
	EColor256,
	EColor64K,
	EColor16M,
	EColor4K,
	EColor16MU,
	EColor16MA,
	EColor16MAP,
 	};

/**	
Enumeration for the attributes to be set in fontspec and gc.
See CTOutlineAndShadowFonts::PrepareTestEnvironment()
*/
enum TFontEffectsFlags
	{
	EDropShadowOn = 0x1,
	EOutlineOn = 0x2,
	ESetMonochromeGlyphBitmap  = 0x4,
	ESetAntiAliasedGlyphBitmap = 0x8,
	ESetFourColourBlendGlyphBitmap = 0x10,
	EVerticalDraw = 0x20,
	ERotate90Degrees = 0x40,
	EIsStrikethroughOn = 0x80,
	EIsUnderlineOn = 0x100,
	ESoftEdgeOn = 0x200,
	EAlgorithmicBoldOn = 0x400
	};

/**
Structure to hold the flags of the attributes in fontspec and gc to be enabled.
*/
typedef struct
	{
	TUint32 fSpecAndGcSettings; //flags/effects in gc and fontspec to be enabled
	TRgb brushColor; //brush colour will set the colour of the fill of the font
	TRgb penColor; //pen colour which will set the colour of the outline
	TRgb shadowColor; //shadow colour which will set the colour of the shadow
	TRgb backgroundColor; //colour of the target that the text is rendered onto
	} STestFontParameters;

// Test setting for TestOutlineandShadowFontsMetricsL() i.e. for test case 8
// All the test case conditions are defined here and are called to test font metrics
// STestFontParameters Format: outline | shadow | bitmaptype, BrushColor, PenColor, ShadowColor
const TInt KNumberOfSubTestsInTest0008 = 6;
const STestFontParameters KTestSettingsToTestFontMetrics[KNumberOfSubTestsInTest0008] =
	{{ESetMonochromeGlyphBitmap},
	{EOutlineOn | ESetMonochromeGlyphBitmap},
	{EOutlineOn | ESetAntiAliasedGlyphBitmap},
	{EDropShadowOn | ESetAntiAliasedGlyphBitmap},
	{EDropShadowOn | EOutlineOn | ESetAntiAliasedGlyphBitmap},
	{EAntiAliasedGlyphBitmap}};

// Test setting for TestDrawTextWithOutlineAndShadowL() i.e. for test case 10
// All test case conditions are defined here and are called to test outline and shadow effects
// STestFontParameters Format: outline | shadow | bitmaptype, BrushColor, PenColor, ShadowColor, BackgroundColor
const TInt KNumberOfSubTestsInTest0010 = 35;
_LIT(KTestName0010, "OutlineShadowWithDrawTextNormalAndVertical");//test case name to used to generate test case id
const STestFontParameters KTestSettingsToTestOutlineAndShadow[KNumberOfSubTestsInTest0010] =
	{
	// test with EMonochromeGlyphBitmap and alpha value 255
	{EDropShadowOn | ESetMonochromeGlyphBitmap | EVerticalDraw | ERotate90Degrees, KRgbGreen, KRgbBlack, KRgbBlue, KRgbWhite},
	{EOutlineOn | ESetMonochromeGlyphBitmap |  EVerticalDraw | ERotate90Degrees, KRgbGreen, KRgbBlack, KRgbBlue, KRgbWhite},
	{EDropShadowOn | EOutlineOn | ESetMonochromeGlyphBitmap | EVerticalDraw | ERotate90Degrees, KRgbGreen, KRgbBlack, KRgbBlue, KRgbWhite},
	{EDropShadowOn | ESetMonochromeGlyphBitmap | EVerticalDraw, KRgbGreen, KRgbBlack, KRgbBlue, KRgbWhite},
	{EOutlineOn | ESetMonochromeGlyphBitmap | EVerticalDraw, KRgbGreen, KRgbBlack, KRgbBlue, KRgbWhite},
	{EDropShadowOn | EOutlineOn | ESetMonochromeGlyphBitmap | EVerticalDraw, KRgbGreen, KRgbBlack, KRgbBlue, KRgbWhite},
	{EDropShadowOn | ESetMonochromeGlyphBitmap, KRgbGreen, KRgbBlack, KRgbBlue, KRgbWhite},
	{EOutlineOn | ESetMonochromeGlyphBitmap, KRgbGreen, KRgbBlack, KRgbBlue, KRgbWhite},
	{EDropShadowOn | EOutlineOn | ESetMonochromeGlyphBitmap, KRgbGreen, KRgbBlack, KRgbBlue, KRgbWhite},

	// test with EAntiAliasedGlyphBitmap and alpha value 255
	{EDropShadowOn | ESetAntiAliasedGlyphBitmap | EVerticalDraw | ERotate90Degrees, KRgbGreen, KRgbBlack, KRgbBlue, KRgbWhite},
	{EOutlineOn | ESetAntiAliasedGlyphBitmap | EVerticalDraw | ERotate90Degrees, KRgbGreen, KRgbBlack, KRgbBlue, KRgbWhite},
	{EDropShadowOn | EOutlineOn | ESetAntiAliasedGlyphBitmap | EVerticalDraw | ERotate90Degrees, KRgbGreen, KRgbBlack, KRgbBlue, KRgbWhite},
	{EDropShadowOn | ESetAntiAliasedGlyphBitmap | EVerticalDraw, KRgbGreen, KRgbBlack, KRgbBlue, KRgbWhite},
	{EOutlineOn | ESetAntiAliasedGlyphBitmap | EVerticalDraw, KRgbGreen, KRgbBlack, KRgbBlue, KRgbWhite},
	{EDropShadowOn | EOutlineOn | ESetAntiAliasedGlyphBitmap | EVerticalDraw, KRgbGreen, KRgbBlack, KRgbBlue, KRgbWhite},
	{EDropShadowOn | ESetAntiAliasedGlyphBitmap | EVerticalDraw | ERotate90Degrees, KRgbBlue, KRgbBlack, KRgbYellow, KRgbWhite},
	{EOutlineOn | ESetAntiAliasedGlyphBitmap | EVerticalDraw | ERotate90Degrees, KRgbBlue, KRgbBlack, KRgbYellow, KRgbWhite},
	{EDropShadowOn | EOutlineOn | ESetAntiAliasedGlyphBitmap | EVerticalDraw | ERotate90Degrees, KRgbBlue, KRgbBlack, KRgbYellow, KRgbWhite},
	{EDropShadowOn | ESetAntiAliasedGlyphBitmap | EVerticalDraw, KRgbBlue, KRgbBlack, KRgbYellow, KRgbWhite},
	{EOutlineOn | ESetAntiAliasedGlyphBitmap | EVerticalDraw, KRgbBlue, KRgbBlack, KRgbYellow, KRgbWhite},
	{EDropShadowOn | EOutlineOn | ESetAntiAliasedGlyphBitmap | EVerticalDraw, KRgbBlue, KRgbBlack, KRgbYellow, KRgbWhite},
	{EDropShadowOn | ESetAntiAliasedGlyphBitmap, KRgbGreen, KRgbBlack, KRgbBlue, KRgbWhite},
	{EOutlineOn | ESetAntiAliasedGlyphBitmap, KRgbGreen, KRgbBlack, KRgbBlue, KRgbWhite},
	{EDropShadowOn |EOutlineOn | ESetAntiAliasedGlyphBitmap, KRgbGreen, KRgbBlack, KRgbBlue, KRgbWhite},
	{EDropShadowOn | ESetAntiAliasedGlyphBitmap, KRgbBlue, KRgbGreen, KRgbYellow, KRgbWhite},
	{EOutlineOn | ESetAntiAliasedGlyphBitmap, KRgbBlue, KRgbGreen, KRgbYellow, KRgbWhite},
	{EDropShadowOn | EOutlineOn | ESetAntiAliasedGlyphBitmap, KRgbBlue, KRgbGreen, KRgbYellow, KRgbWhite},
	{ESetAntiAliasedGlyphBitmap, KRgbBlue, KRgbGreen, KRgbYellow, KRgbWhite},
	{ESetAntiAliasedGlyphBitmap | EVerticalDraw | ERotate90Degrees, KRgbBlue, KRgbGreen, KRgbYellow, KRgbWhite},
	
	// test with EAntiAliasedGlyphBitmap and alpha value < 255 (transparency)
	{EDropShadowOn | ESetAntiAliasedGlyphBitmap | EVerticalDraw | ERotate90Degrees, KRgbBlue, TRgb(128,255,100,0), KRgbMagenta, KRgbWhite},
	{EOutlineOn | ESetAntiAliasedGlyphBitmap, KRgbGreen, TRgb(128,200,100,32), KRgbYellow, KRgbWhite},
	{EDropShadowOn | EOutlineOn | ESetAntiAliasedGlyphBitmap | EVerticalDraw, KRgbYellow, TRgb(100,200,100,64), KRgbGray, KRgbWhite},
	{EDropShadowOn | ESetAntiAliasedGlyphBitmap | EVerticalDraw | ERotate90Degrees, KRgbWhite, TRgb(128,200,50,128), KRgbYellow, KRgbWhite},
	{EOutlineOn | ESetAntiAliasedGlyphBitmap, KRgbBlack, TRgb(12,20,50,255), KRgbWhite, KRgbWhite},
	{EOutlineOn | ESetAntiAliasedGlyphBitmap, KRgbGreen, TRgb(255,0,0,127), KRgbYellow, TRgb(127, 127, 127, 0)}
	};

// Test setting for TestDrawTextWithStrikeThroughL() i.e. for test case 11
// All test case conditions are defined here and are called to test outline and shadow with strikethrough and underline effects.
// STestFontParameters Format: outline | shadow | bitmaptype | strikethroughon | underlineon, BrushColor, PenColor, ShadowColor
const TInt KNumberOfSubTestsInTest0011 = 4;
_LIT(KTestName0011, "OutlineShadowWithStrikeThroughBoldAndUnderline");//test case name to used to generate test case id
const STestFontParameters KTestSettingsToTestStrikeAndUnderline[KNumberOfSubTestsInTest0011] =
	{
	{EDropShadowOn | EOutlineOn | EIsStrikethroughOn | ESetAntiAliasedGlyphBitmap},
	{EDropShadowOn | EOutlineOn | ESetAntiAliasedGlyphBitmap},
	{EAlgorithmicBoldOn | ESoftEdgeOn | EIsUnderlineOn | ESetAntiAliasedGlyphBitmap},
	{ESetAntiAliasedGlyphBitmap}
	};

//Structure to store bitmap device and font, can be used to clean in case of a leave
//See CleanUpFont()
typedef struct
	{
	CBitmapDevice* device;
	CFont* font;
	} SCleanFont;

class CTOutlineAndShadowFontsStep : public CTGraphicsStep
	{
public:
	CTOutlineAndShadowFontsStep();
protected:
	virtual CTGraphicsBase* CreateTestL();
	};

/**
 Class defines all the API's which are used to test outline and shadow effects
 */
class CTOutlineAndShadowFonts : public CTGraphicsBase
	{
public:
	CTOutlineAndShadowFonts(CTestStep* aStep);
	virtual ~CTOutlineAndShadowFonts();
private:
	void DoOutlinePenAndShadowColor();
	void DoOutlineandShadowFontsL();
	void DoOOM(TInt aFontIndex, TBool aVerticalText);
	void DoDrawTextWithOutlineAndShadowL(const STestFontParameters &aSettings, const TDesC& aHashIdString, const TInt aFontIndex);
	void DoOutlineandShadowFontsMetrics(const STestFontParameters &aSettings, TInt aFontIndex);
	void GetFontAndDrawOOM(TFontSpec aFontSpec, TBool aVerticalText);
	void DoSetStrikeThroughAndDrawTestL(const STestFontParameters &aSettings, const TDesC& aHashIdString);
	void CreateScreenDeviceL(TDisplayMode aMode);
	void FreeScreenDevice();
	void SetScreenMode(TDisplayMode aScreen);
	void TestOutlineandShadowFontsMetricsL();
	void TestOutlinePenAndShadowColorL();
	void TestDrawTextWithOutlineAndShadowL();
	void TestDrawTextWithStrikeThroughL();
	void TestCheckBitmapTypesL();
	void TestOOML();
	void TestAntiAliasedOrMonochromeGlyphBitmapL();
	void NegativeRastTestL();
	void NegativeRastTestDrawText(const TFontSpec &aFontSpec);
	void PrepareTestEnvironment(TFontSpec &aFontSpec, const STestFontParameters &aSettings, const TInt aFontIndex, TBool aSetEffects = ETrue, TBool aSetColors = EFalse);
	TBool CheckMonoTypeInstalledL();
	TBool TestRange(TInt aMin, TInt aValue, TInt aMax);
	TBool TestLessThanOrEqual(TInt aValue, TInt aMax);
protected:
	virtual void RunTestCaseL(TInt aCurTestCase);
	virtual void ConstructL();
private:
	CFbsBitGc* iGc;
	CFbsDevice* iBitmapDevice;
	RFbsSession* iFbs;
	CFbsBitmap* iBitmap;
	CTestStep* iStep;
	CTHashReferenceImages *iHashUtil;
	};

_LIT(KTOutlineAndShadowFontsStep,"TOutlineAndShadowFonts");
#endif
