// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TGLYPHSEL_H_
#define __TGLYPHSEL_H_


#include "TGraphicsContext.h"
#include "TGraphicsHarness.h"


class TStopWatch
	{
public:
	inline void Start();
	inline TUint Stop();
private:
	TTime iTime;
	};

void TStopWatch::Start()
	{
	iTime.UniversalTime();
	}

TUint TStopWatch::Stop()
	{
	TTime t;
	t.UniversalTime();
	TInt64 i = ((t.MicroSecondsFrom(iTime).Int64()) + 500)/1000;
	return I64LOW(i);
	}

//	Forward declaration
struct TTestInput_PositionParam6;
class CTGlyphSelection : public CTGraphicsBase
/**
 This is a unit test class. Its responsibilites are to define and
 execute test cases that exercise CFont::GetCharacterPosition() 
 functionality.
@internalComponent
@test
*/
	{
public:

	// House keeping methods for test setup and shutdown.
	CTGlyphSelection(CTestStep* aStep);
	~CTGlyphSelection();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
	virtual void ConstructL();
	
	void CreateTestGraphicsContextL();
	void TestPerformance();

	// Test case methods testing specific functionality.
	void TestCtrlCharsIgnored();
	void TestApiBoundaries();
	void TestLatinChars();
	void TestGlyphs(CFont* aFont, CFont::TPositionParam& aParam,
		const TPtrC16& aText, const TPtrC16& aExpectedGlyphs);
	
	//TestIndicChars() is main function to perform testing for all Indic languages and calls all the indic language functions.
	 
	void TestIndicCharsL();
	void TestHindiChars(CFbsFont* aFont);
	void TestKannadaChars(CFbsFont* aFont);
	void TestMarathiChars(CFbsFont* aFont);
	void TestGujaratiChars(CFbsFont* aFont);
	void TestBengaliChars(CFbsFont* aFont);
	void TestTamilChars(CFbsFont* aFont);
	void TestTeluguChars(CFbsFont* aFont);
	void TestIndicPunctuation(CFbsFont* aFont);
	void TestIndicPunctuationGlyphs(CFbsFont* aFont, CFont::TPositionParam& aParam,
		const TPtrC16& aText, const TPtrC16& aExpectedGlyphs);
	
	//Gurumukhi and Malayalam support
	void ExecuteIndicGurmukhiMalayalamTests();
	void DoEachGurmukhiMalayalamCase( const TTestInput_PositionParam6* aArray, const CFbsFont& aFont );
	void TestGurmukhiDigit( const CFbsFont& aFont );
	void TestGurmukhiAndLatin( const CFbsFont& aFont );
	void TestGurmukhiModifierAfterConsonantViramaZWJ( const CFbsFont& aFont );
	void TestGurmukhiWithConsonantInitial( const CFbsFont& aFont );
	void TestGurmukhiWithVowelInitial( const CFbsFont& aFont );
	void TestMalayalamDigit( const CFbsFont& aFont );
	void TestMalayalamAndLatin( const CFbsFont& aFont );
	void TestMalayalamModifierAfterConsonantViramaZWJ( const CFbsFont& aFont );
	void TestMalayalamWithConsonantInitial( const CFbsFont& aFont );
	void TestMalayalamWithVowelInitial( const CFbsFont& aFont );
	
	void TestVietnameseChars();
	void TestSoftHyphen();
	void TestSurrogates();
	void TestNonBmpCharsL();
	void TestNonBmpCharsInGB18030(CFbsFont* aFont);
	void TestCombiningLatinChars();
	void TestTextDirection();
	void TestAllUnicodeChars();
	void TestLigaturesWithDiacritics();
	void TestIsolatedCombiningLatinChars();
	void TestContextInShapeInfo();

	// Test cases methods testing Thai functionality.
	void ExecuteThaiTests();
	void TestThai_SimpleChars();
	void TestThai_ValidGlyphClusters();
	void TestThai_InvalidGlyphClusters();
	void TestThai_INC118424();
	
	void TestMeasureText();

	void TestCodepoint(TUint aCodepoint, CFont* aFont);
	
	void GenerateGurmukhiMalayalamData( const TDesC& filename, const TTestInput_PositionParam6* aArray, const CFbsFont& aFont );

public:
	// Context data needed for the test cases in this class.
	CTestFont* iTestFont;
	CTestGraphicsDevice* iTestGraphicsDevice;
	CTestGraphicsContext* iTestGraphicsContext;
private:
    RFs iFs;
	};

class CTGlyphSelectionStep : public CTGraphicsStep
	{
public:
	CTGlyphSelectionStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	void TestSetupL();
	};

_LIT(KTGlyphSelectionStep,"TGlyphSelection");


#endif //__TGLYPHSEL_H_

