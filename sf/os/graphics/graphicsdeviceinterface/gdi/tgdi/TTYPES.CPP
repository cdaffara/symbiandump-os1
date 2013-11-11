// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <f32file.h>
#include <s32file.h>
#include "TTYPES.H"


RFs fs;
CDirectFileStore* writestore=NULL;
CDirectFileStore* readstore=NULL;
RStoreWriteStream writestrm;
RStoreReadStream readstrm;

TestTypeface::TestTypeface(
	const TBuf<KMaxTypefaceNameLength>& aName,
	TUint32 aFlags,
	CTTypes* aTest) :
	iName(aName),
	iFlags(aFlags),
	iTest(aTest)
	{}

/**
	TestTypeface::Test
	Tests functionality contained within TTypeface, settings various attributes
	on the typeface & retrieving them to confirm the result
	The test also streams the typeface to file, via CDirectFileStore calls & retrieves again 
	from the store to confirm result
*/
void TestTypeface::Test()
	{
	TTypeface typeface1;
	typeface1.iName = iName;
	const TBool prop = iFlags & TTypeface::EProportional;
	const TBool seri = iFlags & TTypeface::ESerif;
	const TBool symb = iFlags & TTypeface::ESymbol;

	typeface1.SetIsProportional(prop);
	iTest->TEST(typeface1.IsProportional() == prop);

	typeface1.SetIsSerif(seri);
	iTest->TEST(typeface1.IsSerif() == seri);
	iTest->TEST(typeface1.IsProportional() == prop);

	typeface1.SetIsSymbol(symb);
	iTest->TEST(typeface1.IsSymbol() == symb);
	iTest->TEST(typeface1.IsProportional() == prop);
	iTest->TEST(typeface1.IsSerif() == seri);

	TTypeface typeface2;
	typeface2.iName = iName;
	typeface2.SetIsProportional(prop);
	typeface2.SetIsSerif(seri);
	typeface2.SetIsSymbol(symb);
	iTest->TEST(typeface1 == typeface2);

	TestStream();

	for (TInt i = 0; i < 8; i++)
		{
		typeface1.SetAttributes(i);
		iTest->TEST(typeface1.Attributes() == i);
		}

	for (TInt i = 0; i < 16; i++)
		{
		typeface1.SetScriptTypeForMetrics(i);
		iTest->TEST(typeface1.ScriptTypeForMetrics() == i);
		}
	
	// test for PDEF124646. The test shouldn't cause any thread panic when passing in 
	// a big value of an invalid TLanguage index. If the fix is not present this test will crash.
	
	for (TInt ii = 1; ii < 1000000; ii *= 10)
		{
		typeface1.SetScriptTypeForMetrics(static_cast<TLanguage>(ii));	
		}
	}

/**
	TestTypeface::TestStream
	Called by the TestTypeface::Test method, this function
	sets attributes on a typeface & streams the object to file, via CDirectFileStore calls.
	It then retrieves the typeface from the store to confirm result
*/
void TestTypeface::TestStream()
	{
	TTypeface typeface1;
	typeface1.iName = iName;
	typeface1.SetIsProportional(TTypeface::EProportional & iFlags);
	typeface1.SetIsSerif(TTypeface::ESerif & iFlags);
	typeface1.SetIsSymbol(TTypeface::ESymbol & iFlags);
	iTest->InitialiseWriteStream();
	TRAPD(ret, typeface1.ExternalizeL(writestrm));
	iTest->TEST2(ret, KErrNone);
	iTest->ResetWriteStream();

	TTypeface typeface2;
	iTest->InitialiseReadStream();
	TRAP(ret, typeface2.InternalizeL(readstrm));
	iTest->TEST2(ret, KErrNone);
	iTest->ResetReadStream();
	iTest->TEST(typeface1 == typeface2);
	}

TestMargins::TestMargins(TInt aLeft,TInt aRight,TInt aTop,TInt aBottom, CTTypes* aTest):
	iLeft(aLeft),
	iRight(aRight),
	iTop(aTop),
	iBottom(aBottom),
	iTest(aTest)
	{}

/**
	TestMargins::Test
	Tests functionality contained within the TMargins class, settings various attributes
	on the margins & streaming the object to file, via CDirectFileStore calls. 
	The TMargins object is then retrieved from the store & compared within original to confirm result
*/
void TestMargins::Test()
	{
	TestStream();
	}

/**
	TestMargins::TestStream
	Tests functionality contained within the TMargins class, settings various attributes
	on the margins & streaming the object to file, via CDirectFileStore calls. 
	The TMargins object is then retrieved from the store & compared within original to confirm result
*/
void TestMargins::TestStream()
	{
	TMargins tm;
	tm.iLeft=iLeft;
	tm.iRight=iRight;
	tm.iTop=iTop;
	tm.iBottom=iBottom;
	iTest->InitialiseWriteStream();
	TRAPD(ret,tm.ExternalizeL(writestrm));
	iTest->TEST2(ret, KErrNone);
	iTest->ResetWriteStream();
	TMargins tm2;
	tm2.iLeft=0;
	tm2.iRight=0;
	tm2.iTop=0;
	tm2.iBottom=0;
	iTest->InitialiseReadStream();
	TRAP(ret,tm2.InternalizeL(readstrm));
	iTest->TEST2(ret, KErrNone);
	iTest->ResetReadStream();
	iTest->TEST(tm.iLeft==tm2.iLeft);
	iTest->TEST(tm.iRight==tm2.iRight);
	iTest->TEST(tm.iTop==tm2.iTop);
	iTest->TEST(tm.iBottom==tm2.iBottom);
	}
	
//---------
/**
   @SYMTestCaseID          GRAPHICS-GDI-0001

   @SYMPREQ                PREQ1301

   @SYMREQ                 REQ5381

   @SYMTestCaseDesc        Getting/setting font effects with FontEffect

   @SYMTestPriority        Critical

   @SYMTestStatus          Implemented

   @SYMTestActions         (1) Call SetEffect with attributes EAlgorithmicBold and EFalse
                           (2) Call IsEffectOn with attribute EAlgorithmicBold to check (1)
                           (3) Call SetEffect with attributes EAlgorithmicBold and ETrue
                           (4) Call IsEffectOn with attribute EAlgorithmicBold to check (3)
                           (5) Repeat (1) - (4) with all supported font effects attributes

   @SYMTestExpectedResults Getters should return the expected results set by setters
                           (2) IsEffectOn should return a value EFalse
                           (4) IsEffectOn should return a value ETrue
*/
void TestFontEffect::Test()
	{
	TUint32 fontEffect = FontEffect::ENone;
	
	FontEffect::SetEffect(FontEffect::EAlgorithmicBold, EFalse, fontEffect);
	iTest->TEST(!FontEffect::IsEffectOn(FontEffect::EAlgorithmicBold, fontEffect));
	FontEffect::SetEffect(FontEffect::EAlgorithmicBold, ETrue, fontEffect);
	iTest->TEST( FontEffect::IsEffectOn(FontEffect::EAlgorithmicBold, fontEffect));

	FontEffect::SetEffect(FontEffect::EDropShadow, EFalse, fontEffect);
	iTest->TEST(!FontEffect::IsEffectOn(FontEffect::EDropShadow, fontEffect));
	FontEffect::SetEffect(FontEffect::EDropShadow, ETrue, fontEffect);
	iTest->TEST( FontEffect::IsEffectOn(FontEffect::EDropShadow, fontEffect));

	FontEffect::SetEffect(FontEffect::EOutline, EFalse, fontEffect);
	iTest->TEST(!FontEffect::IsEffectOn(FontEffect::EOutline, fontEffect));
	FontEffect::SetEffect(FontEffect::EOutline, ETrue, fontEffect);
	iTest->TEST( FontEffect::IsEffectOn(FontEffect::EOutline, fontEffect));

	FontEffect::SetEffect(FontEffect::EEmbossed, EFalse, fontEffect);
	iTest->TEST(!FontEffect::IsEffectOn(FontEffect::EEmbossed, fontEffect));
	FontEffect::SetEffect(FontEffect::EEmbossed, ETrue, fontEffect);
	iTest->TEST( FontEffect::IsEffectOn(FontEffect::EEmbossed, fontEffect));

	FontEffect::SetEffect(FontEffect::EEngraved, EFalse, fontEffect);
	iTest->TEST(!FontEffect::IsEffectOn(FontEffect::EEngraved, fontEffect));
	FontEffect::SetEffect(FontEffect::EEngraved, ETrue, fontEffect);
	iTest->TEST( FontEffect::IsEffectOn(FontEffect::EEngraved, fontEffect));

	FontEffect::SetEffect(FontEffect::ESoftEdge, EFalse, fontEffect);
	iTest->TEST(!FontEffect::IsEffectOn(FontEffect::ESoftEdge, fontEffect));
	FontEffect::SetEffect(FontEffect::ESoftEdge, ETrue, fontEffect);
	iTest->TEST( FontEffect::IsEffectOn(FontEffect::ESoftEdge, fontEffect));
	}

void TestTFontStyle::Test()
	{
	TFontStyle fontStyle00;
	TFontStyle fontStyle01(EPostureUpright, EStrokeWeightNormal, EPrintPosNormal);
	TFontStyle fontStyle02(EPostureItalic,  EStrokeWeightNormal, EPrintPosNormal);
	TFontStyle fontStyle03(EPostureUpright, EStrokeWeightBold,   EPrintPosNormal);
	TFontStyle fontStyle04(EPostureItalic,  EStrokeWeightBold,   EPrintPosNormal);
	TFontStyle fontStyle05(EPostureUpright, EStrokeWeightNormal, EPrintPosSuperscript);
	TFontStyle fontStyle06(EPostureItalic,  EStrokeWeightNormal, EPrintPosSuperscript);
	TFontStyle fontStyle07(EPostureUpright, EStrokeWeightBold,   EPrintPosSuperscript);
	TFontStyle fontStyle08(EPostureItalic,  EStrokeWeightBold,   EPrintPosSuperscript);
	TFontStyle fontStyle09(EPostureUpright, EStrokeWeightNormal, EPrintPosSubscript);
	TFontStyle fontStyle10(EPostureItalic,  EStrokeWeightNormal, EPrintPosSubscript);
	TFontStyle fontStyle11(EPostureUpright, EStrokeWeightBold,   EPrintPosSubscript);
	TFontStyle fontStyle12(EPostureItalic,  EStrokeWeightBold,   EPrintPosSubscript);

	Test(fontStyle00);
	Test(fontStyle01);
	Test(fontStyle02);
	Test(fontStyle03);
	Test(fontStyle04);
	Test(fontStyle05);
	Test(fontStyle06);
	Test(fontStyle07);
	Test(fontStyle08);
	Test(fontStyle09);
	Test(fontStyle10);
	Test(fontStyle11);
	Test(fontStyle12);
	}

/**
	TestFontStyle::Test
	Tests functionality contained within the TFontStyle class.
	a TFontStyle object is streamed to & from a file store (via CDirectFileStore) & the results are compared. 
	Attributes such as Posture, Stroke, Position, BitmapType etc are set & the result verified
*/
void TestTFontStyle::Test(TFontStyle& aFontStyle)
	{
	TestStream(aFontStyle);
	TestStyle(aFontStyle);
	TestBitmapType(aFontStyle);
	TestFontEffect(aFontStyle);
	}

/**
	TestFontStyle::TestStream
	Streams a TFontStyle object is to & from a file store (via CDirectFileStore) & the results are compared. 
*/
void TestTFontStyle::TestStream(const TFontStyle& aFontStyle)
	{
	iTest -> InitialiseWriteStream();
	TRAPD(ret, aFontStyle.ExternalizeL(writestrm));
	iTest -> TEST2(ret, KErrNone);
	iTest -> ResetWriteStream();
	TFontStyle fontStyle;
	iTest -> InitialiseReadStream();
	TRAP(ret, fontStyle.InternalizeL(readstrm));
	iTest -> TEST2(ret, KErrNone);
	iTest -> ResetReadStream();
	iTest -> TEST(aFontStyle == fontStyle);
	}

/**
	TestFontStyle::TestStyle
	Tests key attributes of a TFontStyle object by settings Posture, Stroke, Print position & verifiying the result
*/
void TestTFontStyle::TestStyle(TFontStyle& aFontStyle)
	{
	iTest -> TEST(TestPosture(aFontStyle));
	iTest -> TEST(TestStrokeWeight(aFontStyle));
	iTest -> TEST(TestPrintPosition(aFontStyle));
	}

/**
	TestFontStyle::TestPosture
	Tests Posture attribute of a TFontStyle object by setting a particular value that opposes the original posture & verifiying the new setting
*/
TBool TestTFontStyle::TestPosture(TFontStyle& aFontStyle)
	{
	if (EPostureUpright == aFontStyle.Posture())
		{
		aFontStyle.SetPosture(EPostureItalic);
		return VerifyStyle(aFontStyle, EPostureItalic, aFontStyle.StrokeWeight(), aFontStyle.PrintPosition());
		}
	else if (EPostureItalic == aFontStyle.Posture())
		{
		aFontStyle.SetPosture(EPostureUpright);
		return VerifyStyle(aFontStyle, EPostureUpright, aFontStyle.StrokeWeight(), aFontStyle.PrintPosition());
		}
	return EFalse;
	}

/**
	TestFontStyle::TestStrokeWeight
	Tests Stroke attribute of a TFontStyle object by setting a particular value that opposes the original stroke & verifiying the new setting
*/
TBool TestTFontStyle::TestStrokeWeight(TFontStyle& aFontStyle)
	{
	if (EStrokeWeightNormal == aFontStyle.StrokeWeight())
		{
		aFontStyle.SetStrokeWeight(EStrokeWeightBold);
		return VerifyStyle(aFontStyle, aFontStyle.Posture(), EStrokeWeightBold, aFontStyle.PrintPosition());
		}
	else if (EStrokeWeightBold == aFontStyle.StrokeWeight())
		{
		aFontStyle.SetStrokeWeight(EStrokeWeightNormal);
		return VerifyStyle(aFontStyle, aFontStyle.Posture(), EStrokeWeightNormal, aFontStyle.PrintPosition());
		}
	return EFalse;
	}

/**
	TestFontStyle::TestPrintPosition
	Tests Print Position attribute of a TFontStyle object by setting a particular value that opposes the original position & verifiying the new setting
*/
TBool TestTFontStyle::TestPrintPosition(TFontStyle& aFontStyle)
	{
	if (EPrintPosNormal == aFontStyle.PrintPosition())
		{
		aFontStyle.SetPrintPosition(EPrintPosSuperscript);
		return VerifyStyle(aFontStyle, aFontStyle.Posture(), aFontStyle.StrokeWeight(), EPrintPosSuperscript);
		}
	else if (EPrintPosSuperscript == aFontStyle.PrintPosition())
		{
		aFontStyle.SetPrintPosition(EPrintPosSubscript);
		return VerifyStyle(aFontStyle, aFontStyle.Posture(), aFontStyle.StrokeWeight(), EPrintPosSubscript);
		}
	else if (EPrintPosSubscript == aFontStyle.PrintPosition())
		{
		aFontStyle.SetPrintPosition(EPrintPosNormal);
		return VerifyStyle(aFontStyle, aFontStyle.Posture(), aFontStyle.StrokeWeight(), EPrintPosNormal);
		}
	return EFalse;
	}

/**
	TestFontStyle::VerifyStyle
	Verifies the various attributes in a TFontStyle object
*/
TBool TestTFontStyle::VerifyStyle(
	const TFontStyle&	aFontStyle,
	TFontPosture		aFontPosture,
	TFontStrokeWeight	aFontStrokeWeight,
	TFontPrintPosition	aFontPrintPosition)
	{
	return
		aFontPosture       == aFontStyle.Posture() &&
		aFontStrokeWeight  == aFontStyle.StrokeWeight() &&
		aFontPrintPosition == aFontStyle.PrintPosition();
	}

/**
	TestFontStyle::TestBitmapType
	Sets the BitmapType attribute for a TFontStyle object to different values & verifies the result is recorded
*/
void TestTFontStyle::TestBitmapType(TFontStyle& aFontStyle)
	{
	aFontStyle.SetBitmapType(EDefaultGlyphBitmap);
	iTest -> TEST(EDefaultGlyphBitmap == aFontStyle.BitmapType());
	aFontStyle.SetBitmapType(EMonochromeGlyphBitmap);
	iTest -> TEST(EMonochromeGlyphBitmap == aFontStyle.BitmapType());
	aFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
	iTest -> TEST(EAntiAliasedGlyphBitmap == aFontStyle.BitmapType());
	}

/**
   @SYMTestCaseID          GRAPHICS-GDI-0002

   @SYMPREQ                PREQ1301

   @SYMREQ                 REQ5381

   @SYMTestCaseDesc        Getting/setting font effects on TFontStyle

   @SYMTestPriority        Critical

   @SYMTestStatus          Implemented

   @SYMTestActions         (1) Call SetEffects with attributes EAlgorithmicBold and EFalse
                           (2) Call IsEffectOn with attribute EAlgorithmicBold to check (1)
                           (3) Call SetEffects with attributes EAlgorithmicBold and ETrue
                           (4) Call IsEffectOn with attribute EAlgorithmicBold to check (3)
                           (5) Call SetEffects/Effects to verify the attribute set in (3)
                           (6) Repeat (1) - (5) with all supported font effects attributes

   @SYMTestExpectedResults Getters should return the expected results set by setters
                           (2) IsEffectOn should return a value EFalse
                           (4) IsEffectOn should return a value ETrue
                           (5) Effects should return the value set by SetEffects
*/
void TestTFontStyle::TestFontEffect(TFontStyle& aFontStyle)
	{
	aFontStyle.SetEffects(FontEffect::EAlgorithmicBold, EFalse);
	iTest -> TEST(!aFontStyle.IsEffectOn(FontEffect::EAlgorithmicBold));
	aFontStyle.SetEffects(FontEffect::EAlgorithmicBold, ETrue);
	iTest -> TEST( aFontStyle.IsEffectOn(FontEffect::EAlgorithmicBold));
	VerifyFontEffect(aFontStyle);

	aFontStyle.SetEffects(FontEffect::EDropShadow, EFalse);
	iTest -> TEST(!aFontStyle.IsEffectOn(FontEffect::EDropShadow));
	aFontStyle.SetEffects(FontEffect::EDropShadow, ETrue);
	iTest -> TEST( aFontStyle.IsEffectOn(FontEffect::EDropShadow));
	VerifyFontEffect(aFontStyle);

	aFontStyle.SetEffects(FontEffect::EOutline, EFalse);
	iTest -> TEST(!aFontStyle.IsEffectOn(FontEffect::EOutline));
	aFontStyle.SetEffects(FontEffect::EOutline, ETrue);
	iTest -> TEST( aFontStyle.IsEffectOn(FontEffect::EOutline));
	VerifyFontEffect(aFontStyle);

	aFontStyle.SetEffects(FontEffect::EEmbossed, EFalse);
	iTest -> TEST(!aFontStyle.IsEffectOn(FontEffect::EEmbossed));
	aFontStyle.SetEffects(FontEffect::EEmbossed, ETrue);
	iTest -> TEST( aFontStyle.IsEffectOn(FontEffect::EEmbossed));
	VerifyFontEffect(aFontStyle);

	aFontStyle.SetEffects(FontEffect::EEngraved, EFalse);
	iTest -> TEST(!aFontStyle.IsEffectOn(FontEffect::EEngraved));
	aFontStyle.SetEffects(FontEffect::EEngraved, ETrue);
	iTest -> TEST( aFontStyle.IsEffectOn(FontEffect::EEngraved));
	VerifyFontEffect(aFontStyle);

	aFontStyle.SetEffects(FontEffect::ESoftEdge, EFalse);
	iTest -> TEST(!aFontStyle.IsEffectOn(FontEffect::ESoftEdge));
	aFontStyle.SetEffects(FontEffect::ESoftEdge, ETrue);
	iTest -> TEST( aFontStyle.IsEffectOn(FontEffect::ESoftEdge));
	VerifyFontEffect(aFontStyle);
	}

void TestTFontStyle::VerifyFontEffect(TFontStyle& aFontStyle)
	{
	const TUint32 effects = aFontStyle.Effects();
	aFontStyle.SetEffects(effects);
	iTest -> TEST(effects == aFontStyle.Effects());
	}

//-----------
TestPageSpec::TestPageSpec(TPageSpec::TPageOrientation aOrientation,const TSize& aSize, CTTypes* aTest):
	iPortraitPageSize(aSize),
	iOrientation(aOrientation),
	iTest(aTest)
	{}

/**
	TestPageSpec::Test
	Tests functionality contained within the TPageSpec class.
	a TPageSpec object is streamed to & from a file store (via CDirectFileStore) & the results are compared. 
	Attributes such as Orientaton, Page Size are also tested
*/
void TestPageSpec::Test()
	{
	TPageSpec tps(iOrientation,iPortraitPageSize);
	iTest->TEST(tps.iOrientation==iOrientation);
	iTest->TEST(tps.iPortraitPageSize==iPortraitPageSize);
	if(iOrientation==TPageSpec::EPortrait)
		iTest->TEST(tps.OrientedPageSize()==iPortraitPageSize);
	else
		iTest->TEST(tps.OrientedPageSize()==TSize(iPortraitPageSize.iHeight,iPortraitPageSize.iWidth));
	TestStream();
	}

void TestPageSpec::TestStream()
	{
	TPageSpec tps(iOrientation,iPortraitPageSize);
	iTest->InitialiseWriteStream();
	TRAPD(ret,tps.ExternalizeL(writestrm));
	iTest->TEST2(ret, KErrNone);
	iTest->ResetWriteStream();
	TPageSpec tps2;
	iTest->InitialiseReadStream();
	TRAP(ret,tps2.InternalizeL(readstrm));
	iTest->TEST2(ret, KErrNone);
	iTest->ResetReadStream();
	TPageSpec::TPageOrientation tpo=tps.iOrientation;
	TPageSpec::TPageOrientation tpo2=tps2.iOrientation;
	iTest->TEST(tpo==tpo2);
	TSize tpss=tps.iPortraitPageSize;
	TSize tpss2=tps2.iPortraitPageSize;
	iTest->TEST(tpss==tpss2);
	}

/**
	TestFontSSpec::Test
	Tests functionality contained within the TFontSpec class.
	a TFontSpec object is streamed to & from a file store (via CDirectFileStore) & the results are compared. 
	Attributes such as typeface, font height, style are set & the result verified
*/
TestFontSpec::TestFontSpec(const TTypeface& aTypeface,TInt aHeight,TFontStyle aStyle, CTTypes* aTest):
	iTypeface(aTypeface),
	iHeight(aHeight),
	iStyle(aStyle),
	iTest(aTest)
	{}

void TestFontSpec::Test()
	{
	TFontSpec fontspec;
	fontspec.iTypeface=iTypeface;
	fontspec.iHeight=iHeight;
	fontspec.iFontStyle=iStyle;
	TFontSpec fontspec2(iTypeface.iName,iHeight);
	fontspec2.iTypeface=iTypeface;
	fontspec2.iFontStyle=iStyle;
	iTest->TEST(fontspec==fontspec2);
	TestStream();
	}

void TestFontSpec::TestStream()
	{
	TFontSpec fs(iTypeface.iName,iHeight);
	fs.iTypeface=iTypeface;
	fs.iFontStyle=iStyle;
	iTest->InitialiseWriteStream();
	TRAPD(ret,fs.ExternalizeL(writestrm));
	iTest->TEST2(ret, KErrNone);
	iTest->ResetWriteStream();
	TFontSpec fs2;
	iTest->InitialiseReadStream();
	TRAP(ret,fs2.InternalizeL(readstrm));
	iTest->TEST2(ret, KErrNone);
	iTest->ResetReadStream();
	iTest->TEST(fs==fs2);
	}


TestRgb::TestRgb(TUint8 r1,TUint8 g1,TUint8 b1, CTTypes* aTest):
	iR(r1),
	iG(g1),
	iB(b1),
	iTest(aTest)
	{}

/**
	TestRgb::Test
	Tests functionality contained within the TRgb class.
	Attributes such as base colours are set/retrieved & the result verified
*/
void TestRgb::Test()
	{
	TestSet();
	TestGet();
	TestComp();
	TestStream();
	}

void TestRgb::TestSet()
	{
	TRgb c1;
	TRgb c2(128,128,128);
	TRgb c3(500,500,500);
	TRgb c4(iR,iG,iB);
	TRgb c5(500,600,700);
	iTest->TEST(c2.Red()==128);
	iTest->TEST(c2.Green()==128);
	iTest->TEST(c2.Blue()==128);
	iTest->TEST(c3.Red()<256);
	iTest->TEST(c3.Green()<256);
	iTest->TEST(c3.Blue()<256);
	iTest->TEST(c4.Red()==iR && c4.Green()==iG && c4.Blue()==iB);
	iTest->TEST(c5.Red()<256);
	iTest->TEST(c5.Green()<256);
	iTest->TEST(c5.Blue()<256);
	c1=c4;
	iTest->TEST(c1.Red()==iR && c1.Green()==iG && c1.Blue()==iB);
	c1=TRgb(64,128,192);
	iTest->TEST(c1.Red()==64);
	iTest->TEST(c1.Green()==128);
	iTest->TEST(c1.Blue()==192);
	c1=c5;
	iTest->TEST(c1.Red()<256);
	iTest->TEST(c1.Green()<256);
	iTest->TEST(c1.Blue()<256);
	c1=TRgb::Gray4(3);
	iTest->TEST(c1.Red()==255 && c1.Green()==255 && c1.Blue()==255);
	c1=TRgb::Gray16(15);
	iTest->TEST(c1.Red()==255 && c1.Green()==255 && c1.Blue()==255);
	c1=TRgb::Gray256(255);
	iTest->TEST(c1.Red()==255 && c1.Green()==255 && c1.Blue()==255);
	c1=TRgb::Gray4(4);
	iTest->TEST(c1.Red()<256 && c1.Green()<256 && c1.Blue()<256);
	c1=TRgb::Gray16(16);
	iTest->TEST(c1.Red()<256 && c1.Green()<256 && c1.Blue()<256);
	c1=TRgb::Gray256(256);
	iTest->TEST(c1.Red()<256 && c1.Green()<256 && c1.Blue()<256);
	iTest->TEST(c4.Red()==iR && c4.Green()==iG && c4.Blue()==iB);
	}

/**
	TRgb::TestGet
	Tests mapping of the TRgb value to various gray-scale displaymodes
*/
void TestRgb::TestGet()
	{
	TRgb c1(iR,iG,iB);
	iTest->TEST(c1.Gray4()==(2*iR+5*iG+iB)/512);
	iTest->TEST(c1.Gray16()==(2*iR+5*iG+iB)/128);
	iTest->TEST(c1.Gray256()==(2*iR+5*iG+iB)/8);
	iTest->TEST(c1.Internal()==*((TUint32*)&c1));
	TRgb c2;
	c2=TRgb(*((TUint32*)&c1), 0xff);
	iTest->TEST(c2==TRgb(iR,iG,iB));
	}

/**
	TestRgb::TestComp
	Test operator overrides for TRgb class
*/
void TestRgb::TestComp()
	{
	TRgb c1(iR,iG,iB);
	TRgb c2(iR,iG,iB);
	TRgb c3(255-iR,iG,iB);
	iTest->TEST(c1==c2);
	iTest->TEST(c1!=c3);
	iTest->TEST(c2!=c3);
	iTest->TEST(c1==c2);
	c1&=c3;
	iTest->TEST(c1==TRgb(0,iG,iB));
	c2|=c3;
	iTest->TEST(c2==TRgb(255,iG,iB));
	c1^=c2;
	iTest->TEST(c1==TRgb(255,0,0));
	}

/**
	TestRgb::TestStream
	Streams a TRgb object to & from a file store (via CDirectFileStore calls) & copmares results
*/
void TestRgb::TestStream()
	{
	TRgb rgb(iR,iG,iB);
	iTest->InitialiseWriteStream();
	TRAPD(ret,rgb.ExternalizeL(writestrm));
	iTest->TEST2(ret, KErrNone);
	iTest->ResetWriteStream();
	TRgb rgb2;
	iTest->InitialiseReadStream();
	TRAP(ret,rgb2.InternalizeL(readstrm));
	iTest->TEST2(ret, KErrNone);
	iTest->ResetReadStream();
	iTest->TEST(rgb==rgb2);
	}

TestZoomFactor::TestZoomFactor(CTTypes* aTest, TInt aZoomFactor):
	iZoomFactor(aZoomFactor),
	iTest(aTest)
	{}

/**
	TestZoomFactor::Test
	Tests attributes associated with TZoomFactor for mapping twips to pixels/device-specifc units
*/
void TestZoomFactor::Test()
	{
	TZoomFactor zf((MGraphicsDeviceMap*)NULL);
	zf.SetZoomFactor(iZoomFactor);
	iTest->TEST(zf.ZoomFactor()==iZoomFactor);
	zf.SetZoomFactor(iZoomFactor*10);
	iTest->TEST(zf.ZoomFactor()==iZoomFactor*10);
	zf.SetZoomFactor(iZoomFactor/10);
	iTest->TEST(zf.ZoomFactor()==iZoomFactor/10);
	zf.SetGraphicsDeviceMap(NULL);
	}

/**
	TestDisplayModeUtils::Test
	Test attributes associated with TDisplayModeUtils
*/
void TestDisplayModeUtils::Test()
	{
	const TInt KNumTestModes = 12;
	TDisplayMode mode[] = {	ENone, EGray2, EGray4, EGray16, EGray256,
							EColor16, EColor256, EColor4K, EColor64K, EColor16M, ERgb, EColor16MU, EColor16MA, EColor16MAP};
	TInt numColors[] = { 0, 2, 4, 16, 256, 16, 256, 4096, 65536, 16777216, 0 ,16777216, 16777216, 16777216};
	TBool isColor[] = {	EFalse, EFalse, EFalse, EFalse, EFalse,
						ETrue, ETrue, ETrue, ETrue, ETrue, ETrue, ETrue, ETrue, ETrue};

	for (TInt count = 0; count < KNumTestModes; count++)
		{
		iTest->TEST(TDisplayModeUtils::NumDisplayModeColors(mode[count]) == numColors[count]);
		iTest->TEST((TDisplayModeUtils::IsDisplayModeColor(mode[count]) && isColor[count]) || 
				(!TDisplayModeUtils::IsDisplayModeColor(mode[count]) && !isColor[count]));
		}
	}

CTTypes::CTTypes(CTestStep* aStep):
	CTGraphicsBase(aStep)
	{
	INFO_PRINTF1(_L("Testing T classes in GDI.H:"));
	}

CTTypes::~CTTypes()
	{
	fs.Delete(_L("c:\\ttypes.str"));
	}
	
void CTTypes::ConstructL()
	{
	User::LeaveIfError(fs.Connect());
	}
	
void CTTypes::RunTestCaseL(TInt aCurTestCase)
	{
	switch(aCurTestCase)
		{
	case 1:
		{
		__UHEAP_MARK;
		TestRgb tr1(0,0,0, this);
		TestRgb tr2(100,100,100, this);
		TestRgb tr3(10,20,30, this);
		TestRgb tr4(110,160,210, this);
		TestRgb tr5(255,255,255, this);
		INFO_PRINTF1(_L("TRgb"));
		tr1.Test();
		tr2.Test();
		tr3.Test();
		tr4.Test();
		tr5.Test();
		((CTTypesStep*)iStep)->CloseTMSGraphicsStep();
		__UHEAP_MARKEND;
		}
		break;
	case 2:
		{
		INFO_PRINTF1(_L("TTypeface"));
		TestTypeface ttf1(_L(""), 0, this);
		TestTypeface ttf2(_L("Font name"), 1, this);
		TestTypeface ttf3(_L("Font name"), 2, this);
		TestTypeface ttf4(_L("Font name"), 3, this);
		TestTypeface ttf5(_L("Font name"), 4, this);
		TestTypeface ttf6(_L("Font name"), 5, this);
		TestTypeface ttf7(_L("Font name"), 6, this);
		TestTypeface ttf8(_L("Another font name"), 7, this);
		ttf1.Test();
		ttf2.Test();
		ttf3.Test();
		ttf4.Test();
		ttf5.Test();
		ttf6.Test();
		ttf7.Test();
		ttf8.Test();
		}
		break;
	case 3:
		{
		TestMargins tm1(0,0,0,0, this);
		TestMargins tm2(10,20,30,40, this);
		TestMargins tm3(-10,-20,-30,-40, this);
		INFO_PRINTF1(_L("TMargins"));
		tm1.Test();
		tm2.Test();
		tm3.Test();
		}
		break;
	case 4:
		{
		TestPageSpec tps1(TPageSpec::EPortrait,TSize(0,0), this);
		TestPageSpec tps2(TPageSpec::ELandscape,TSize(0,0), this);
		TestPageSpec tps3(TPageSpec::EPortrait,TSize(10,-5), this);
		TestPageSpec tps4(TPageSpec::ELandscape,TSize(15,-20), this);
		TestPageSpec tps5(TPageSpec::EPortrait,TSize(1000,1500), this);
		TestPageSpec tps6(TPageSpec::ELandscape,TSize(2000,500), this);
		INFO_PRINTF1(_L("TPageSpec"));
		tps1.Test();
		tps2.Test();
		tps3.Test();
		tps4.Test();
		tps5.Test();
		tps6.Test();
		}
		break;
	case 5:
		{
		INFO_PRINTF1(_L("FontEffect"));
        	((CTTypesStep*)iStep)->SetTestStepID(_L("GRAPHICS-GDI-0002"));
		TestFontEffect te(this); 
		((CTTypesStep*)iStep)->RecordTestResultL();
		te.Test();
		}
		break;
	case 6:
		{
		INFO_PRINTF1(_L("TFontSyle"));
		TestTFontStyle ts(this);
		ts.Test();
		}
		break;
	case 7:
		{
		TTypeface typeface;
		typeface.iName=_L("Font name");
		TFontStyle fontstyle;
		TestFontSpec tfspec(typeface,200,fontstyle, this);
		INFO_PRINTF1(_L("TFontSpec"));
		tfspec.Test();
		}
		break;
	case 8:
		{
		TestLine tl1(TPoint(10,10),TPoint(90,90), this);
		TestLine tl2(TPoint(100,150),TPoint(50,-50), this);
		TestLine tl3(TPoint(-50,50),TPoint(60,-40), this);
		TestLine tl4(TPoint(-100,0),TPoint(0,200), this);
		TestLine tl5(TPoint(150,-50),TPoint(50,75), this);
		TestLine tl6(TPoint(0,-100),TPoint(-50,-150), this);
		TestLine tl7(TPoint(-1000,-1000),TPoint(1000,1000), this);
		TestLine tl8(TPoint(1000,-1000),TPoint(-1000,1000), this);
		TestLine tl9(TPoint(500,-1000),TPoint(-500,1000), this);
		TestLine tl10(TPoint(-500,-1000),TPoint(500,1000), this);
		TestLine tl11(TPoint(1000,-500),TPoint(-1000,500), this);
		TestLine tl12(TPoint(1000,500),TPoint(-1000,-500), this);
		INFO_PRINTF1(_L("TLinearDDA"));
		tl1.Test();
		tl2.Test();
		tl3.Test();
		tl4.Test();
		tl5.Test();
		tl6.Test();
		tl7.Test();
		tl8.Test();
		tl9.Test();
		tl10.Test();
		tl11.Test();
		tl12.Test();
		}
		break;
	case 9:
		{
		INFO_PRINTF1(_L("CTypefaceStore"));
		TestTFStore ttfs(this);
		ttfs.Test();
		}
		break;
	case 10:
		{
		INFO_PRINTF1(_L("CFontCache"));
		TestFontCache tfc(this);
		tfc.Test();
		}
		break;
	case 11:
		{
		INFO_PRINTF1(_L("CScaleCropPicture"));
		TestPicture tp(this);
		tp.Test();
		}
		break;
	case 12:
		{
		INFO_PRINTF1(_L("CPalette"));
		TestPalette tpal(this);
		tpal.Test();
		}
		break;
	case 13:
		{
		INFO_PRINTF1(_L("TDisplayModeUtils"));
		TestDisplayModeUtils tdmu(this);
		tdmu.Test();
		}
		break;
	case 14:
        	((CTTypesStep*)iStep)->SetOverallTestStepID(_L("GRAPHICS-GDI-0001"));
		((CTTypesStep*)iStep)->RecordTestResultL();
		((CTTypesStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();		
		break;
		}
	}
	
void CTTypes::InitialiseWriteStream()
	{
	TRAPD(ret,writestore=CDirectFileStore::ReplaceL(fs,_L("c:\\ttypes.str"),EFileStream|EFileWrite));
	TEST2(ret, KErrNone);
	TRAP(ret,writestore->SetTypeL(KDirectFileStoreLayoutUid));
	TEST2(ret, KErrNone);
	TStreamId headerid(0);      //To stop a warning
	TRAP(ret,headerid=writestrm.CreateL(*writestore));
	TEST2(ret, KErrNone);
	TRAP(ret,writestore->SetRootL(headerid));
	TEST2(ret, KErrNone);
	}

void CTTypes::ResetWriteStream()
	{
	writestrm.Close();
	delete writestore;
	}

void CTTypes::InitialiseReadStream()
	{
	TRAPD(ret,readstore=CDirectFileStore::OpenL(fs,_L("c:\\ttypes.str"),EFileStream|EFileRead)); //* --Geert
	TEST2(ret, KErrNone);
	TStreamId headerid=readstore->Root();
	TRAP(ret,readstrm.OpenL(*readstore,headerid));
	TEST2(ret, KErrNone);
	}

void CTTypes::ResetReadStream()
	{
	readstrm.Close();
	delete readstore;
	}
	
//--------------
__CONSTRUCT_STEP__(Types)

