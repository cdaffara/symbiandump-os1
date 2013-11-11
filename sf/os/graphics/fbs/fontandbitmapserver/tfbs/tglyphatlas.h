// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TGLYPHATLAS_H
#define TGLYPHATLAS_H

#include "test/TGraphicsHarness.h"

/**
Test class for the glyph data extensions RFbsGlyphDataIterator and 
RFbsGlyphMetricsArray. Positive and negative tests
*/
class CTGlyphAtlas : public CTGraphicsBase
	{
public:
	CTGlyphAtlas(CTestStep* aStep);
	~CTGlyphAtlas();
protected:
//from 	CTGraphicsBase
	virtual void RunTestCaseL(TInt aCurTestCase);
	void ConstructL();
	
private:
	// Test Cases
	void TestFullCache();
	void TestFontReleased();
	
private:
	RFbsSession* iFbs;
	CFbsTypefaceStore* iTs;
	TUint* iGlyphCodesLatin;
	CFbsFont* iFont;
	CFbsFont* iFont2;
	};


class CTGlyphAtlasStep : public CTGraphicsStep
	{
public:
	CTGlyphAtlasStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTGlyphAtlasStep,"TGlyphAtlas");

#endif /* TGLYPHATLAS_H */
