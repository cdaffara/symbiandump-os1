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

/**
 @file
 @test
 @internalComponent Internal Symbian test code
*/

#ifndef __TTYPES_H__
#define __TTYPES_H__

#include <gdi.h>
#include "TGraphicsHarness.h"

class CTTypes : public CTGraphicsBase
	{
public:
	CTTypes(CTestStep* aStep);
	~CTTypes();
	void InitialiseWriteStream();
	void ResetWriteStream();
	void InitialiseReadStream();
	void ResetReadStream();

protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
	virtual void ConstructL();
	};

class MTestMap : public MGraphicsDeviceMap
	{
	virtual TInt HorizontalTwipsToPixels(TInt aTwips) const {return(aTwips);}
	virtual TInt VerticalTwipsToPixels(TInt aTwips) const {return(aTwips);}
	virtual TInt HorizontalPixelsToTwips(TInt aPixels) const {return(aPixels);}
	virtual TInt VerticalPixelsToTwips(TInt aPixels) const {return(aPixels);}
	TInt GetNearestFontInTwips(CFont*&, const TFontSpec&) { return KErrNotSupported; }
	virtual void ReleaseFont(CFont*) {}
	};

class CTestPicture : public CPicture
	{
public:
	virtual void Draw(CGraphicsContext&,const TPoint&,const TRect&,MGraphicsDeviceMap*) const {}
	virtual void ExternalizeL(RWriteStream& /*aStream*/) const {}
	virtual void GetOriginalSizeInTwips(TSize& aSize) const {aSize.iWidth=100;aSize.iHeight=100;}
	virtual TBool NativePixelSize(TSize& aSize) { aSize = iSize; return ETrue; }
	TSize iSize;
	};

class CTestPicture2 : public CPicture
	{
public:
	virtual void Draw(CGraphicsContext&,const TPoint&,const TRect&,MGraphicsDeviceMap*) const {}
	virtual void ExternalizeL(RWriteStream& /*aStream*/) const {}
	virtual void GetOriginalSizeInTwips(TSize& aSize) const {aSize.iWidth=100;aSize.iHeight=100;}
	};

class TestPicture
	{
public:
	TestPicture(CTTypes* aTest);
	~TestPicture();
	void Test();
private:
	CTestPicture* iPicture;
	CTTypes* iTest;
	};

class CTestPalette : public CPalette
	{
public:
	enum {eNumColors=256};
public:
	inline static CTestPalette* NewL() {return STATIC_CAST(CTestPalette*,CPalette::NewL(eNumColors));}
	TBool TestGetPtr();
	};

class TestPalette
	{
public:
	TestPalette(CTTypes* aTest);
	~TestPalette();
	void Test();
	void TestGeneral();
	void TestDefaults();
	void TestGetPtr();
private:
	CPalette* iPalette;
	CTestPalette* iPalette2;
	CTTypes* iTest;
	};

class TTestFontAccess
	{
public:
	CFont* iFont;
	TInt iAccessCount;
	};

class CTestTypefaceStore : public CTypefaceStore
	{
public:
	CTestTypefaceStore(CTTypes* aTest);
	void ConstructTestL();
	TInt Count();
	TInt AccessCount(TInt aIndex);
	void AddFont(CFont*);
	void OpenFont(CFont*);
	// CTypefaceStore implementation
	TInt GetNearestFontInTwips(CFont*&, const TFontSpec&) { return KErrNone; }
	TInt GetNearestFontToDesignHeightInTwips(CFont*&, const TFontSpec&) { return KErrNone; }
	TInt GetNearestFontToMaxHeightInTwips(CFont*&, const TFontSpec&, TInt) { return KErrNone; }
	virtual TInt NumTypefaces() const {return(0);}
	virtual void TypefaceSupport(TTypefaceSupport&,TInt) const {}
	virtual TInt FontHeightInTwips(TInt,TInt) const {return(0);}
	virtual TBool IsSameFont(CFont* aFirstFont,CFont* aSecondFont) {return(aFirstFont==aSecondFont);}
private:
	CTTypes* iTest;	
	};

class TestTFStore
	{
public:
	TestTFStore(CTTypes* aTest);
	void Test();
private:
	CTestTypefaceStore iTTFStore;
	CTTypes* iTest;
	};

class TestFontCache
	{
public:
	TestFontCache(CTTypes* aTest);
	void Test();
	void TestL();
public:
	CFontCache* iCache;
	CTTypes* iTest;
	};

class TestLine
	{
public:
	TestLine(const TPoint& aStart,const TPoint& aFinish, CTTypes* aTest);
	void Test();
	void TestSingleStep();
	void TestSingleScanLine();
	void TestNextStep();
	void TestJumpToRect();
	void TestJumpToXCoord();
	void TestJumpToYCoord();
private:
	TLinearDDA iLine;
	TPoint iStart;
	TPoint iFinish;
	CTTypes* iTest;
	};

class TestTypeface
	{
public:
	TestTypeface(const TBuf<KMaxTypefaceNameLength>& aName,TUint32 aFlags, CTTypes* aTest);
	void Test();
	void TestStream();
private:
    TBuf<KMaxTypefaceNameLength> iName;
	TUint32 iFlags;
	CTTypes* iTest;
	};

class TestMargins
	{
public:
	TestMargins(TInt aLeft,TInt aRight,TInt aTop,TInt aBottom, CTTypes* aTest);
	void Test();
	void TestStream();
private:
	TInt iLeft;
	TInt iRight;
	TInt iTop;
	TInt iBottom;
	CTTypes* iTest;
	};

class TestPageSpec
	{
public:
	TestPageSpec(TPageSpec::TPageOrientation anOrientation,const TSize& aSize, CTTypes* aTest);
	void Test();
	void TestStream();
private:
	TSize iPortraitPageSize;
	TPageSpec::TPageOrientation iOrientation;
	CTTypes* iTest;
	};

class TestFontEffect
	{
public:
	TestFontEffect(CTTypes* aTest) : 
		iTest(aTest)
		{
		
		}
	void Test();
	CTTypes* iTest;
	};

class TestTFontStyle
	{
public:
	TestTFontStyle(CTTypes* aTest) : 
		iTest(aTest)
		{
		
		}
	void Test();
private:
	void Test(TFontStyle& aFontStyle);
	void TestStream(const TFontStyle& aFontStyle);
	void TestStyle(TFontStyle& aFontStyle);
	TBool TestPosture(TFontStyle& aFontStyle);
	TBool TestStrokeWeight(TFontStyle& aFontStyle);
	TBool TestPrintPosition(TFontStyle& aFontStyle);
	TBool VerifyStyle(const TFontStyle& aFontStyle, TFontPosture aFontPosture, TFontStrokeWeight aFontStrokeWeight, TFontPrintPosition aFontPrintPosition);
	void TestBitmapType(TFontStyle& aFontStyle);
	void TestFontEffect(TFontStyle& aFontStyle);
	void VerifyFontEffect(TFontStyle& aFontStyle);
protected:
	CTTypes* iTest;
	};

class TestFontSpec
	{
public:
	TestFontSpec(const TTypeface& aTypeface,TInt aHeight,TFontStyle aStyle, CTTypes* aTest);
	void Test();
	void TestStream();
private:
	TTypeface iTypeface;
	TInt iHeight;
	TFontStyle iStyle;
	CTTypes* iTest;
	};

class TestRgb
	{
public:
	TestRgb(TUint8 r1,TUint8 g1,TUint8 b1, CTTypes* aTest);
	void Test();
private:
	void TestSet();
	void TestGet();
	void TestComp();
	void TestStream();
private:
	TInt iR;
	TInt iG;
	TInt iB;
	CTTypes* iTest;
	};

class TestZoomFactor
	{
public:
	TestZoomFactor(CTTypes* aTest, TInt aZoomFactor=TZoomFactor::EZoomOneToOne);
	void Test();
private:
	TInt iZoomFactor;
	CTTypes* iTest;
	};

class TestDisplayModeUtils
	{
public:
	TestDisplayModeUtils(CTTypes* aTest) :
		iTest(aTest)
		{
		}
	void Test();
private:
	CTTypes* iTest;	
	};




class CTTypesStep : public CTGraphicsStep
	{
public:
	CTTypesStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTTypesStep,"TTypes");


#endif
