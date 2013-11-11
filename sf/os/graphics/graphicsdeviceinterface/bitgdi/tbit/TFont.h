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


#if !defined(__TFONT_H__)
#define __TFONT_H__

#include <test/testexecutestepbase.h>
#include <test/fontinjector.h>
#include "TGraphicsHarness.h"


//--------------------------------------------------------------

class CFbsFontEx : public CFbsFont
	{
public:
	static CBitmapFont* FontAddress(CFbsFont* aFont);
	};

//--------------------------------------------------------------

#ifdef __WINS__
template<class C> class XTCallCounter : public MShellcode
	{
public:
	XTCallCounter(CTGraphicsBase& aTestBase);
private:
	// From MShellcode
	void ExecuteShellcode(TInt aFromFunction);
private:
	CTGraphicsBase& iTestBase;
public:
	XVtableInjector<C> iInjector;
	TInt iVFCallsOutsideFBServ;
	};
#endif

//--------------------------------------------------------------

class CTFont : public CTGraphicsBase
	{
public:
	CTFont(CTestStep* aStep);
	~CTFont();
	void ConstructL();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
public:
	void DoAllFontsL();
	void DoTextFormatsL();
	void DoBitAlignmentL();
	void DoLargeFontsL();
	void DoAntiAliasedTextL();
	void DoVerticalText();
	void DoVerticalBoxL();
	void DoUlStL();
	void DoSuperSubScriptL();
	void DoTextFormatsInContextL();
private:
	void ResetGc();
	void DrawTextInContext();
	void DrawTextInContextBox();
	void DrawTextInContextVertical();
	void Box();
	void Underline();
	void Clipping();
	void Justification();
	void BoxInContext();
	void JustificationInContext();
	void DisplayAsciiCharSet(const TDesC& aTypefaceName);
	void DisplayBoldItalic(const TDesC& aTypefaceName);
	void DisplayLargeChars();
	TInt DisplayCharAndBounds(TInt aChar,TPoint aPos,TBool aVertical,TBool aUp);
	void FontExternalizeL();
	TInt TestFontL(const TFontSpec &aFsp);
	TInt GetNearestFontToDesignHeightInPixels(CFbsFont*& aFont, const TFontSpec& aFontSpec);
	TInt GetNearestFontToDesignHeightInTwips(CFbsFont*& aFont, const TFontSpec& aFontSpec);
private:
	CFbsFont* iFont;
	CFbsBitGc* iGc;
	CFbsScreenDevice* iDev;
#ifdef __WINS__
	XTCallCounter<CFont> iFontCallCounter;
	XTCallCounter<COpenFont> iOpenFontCallCounter;
#endif
	};

//--------------------------------------------------------------

class CTFontStep : public CTGraphicsStep
	{
public:
	CTFontStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	virtual void TestSetupL();
	virtual void TestClose();
	};

_LIT(KTFontStep,"TFont");

#endif	// __TFONT_H__
