// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 
#ifndef __TSIMPLEDRAWING_DIRECTGDI_H__ 
#define __TSIMPLEDRAWING_DIRECTGDI_H__   

#include "tdirectgditestbase.h" 
#include <bitdev.h>
#include <graphics/directgdicontext.h>

#include <test/tefunit.h> 
#include <bitdraw.h>

class CFbsBitmap;

class CTSimpleDrawingDirectGdi : public CTDirectGdiTestBase
	{
public:
	CTSimpleDrawingDirectGdi();
	~CTSimpleDrawingDirectGdi();
	
private:
	// From CStepStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

	void ClearingL();
	void LineDrawingL();
	void CopyRectL();
	void VerticalLinesL();
	void HorizontalLinesL();
	void ThickLinesL();
	void DrawRectL();
	void TextDrawingL();
	void TextDrawingAntiAliasedL();
	void TextDrawingWithOverheadL();
	void LargeTextDrawingL();
	
	// Helpers
	void DoLineDrawingL(TPtrC aTestName, const TSize& aPenSize);
	
private:		
	CFbsBitmap* iBitmap;	
	TRect iScreenRect;
	TDisplayMode iDisplayMode;
	};
	
_LIT(KTSimpleDrawingPerfDirectGdi,"tsimpledrawing_directgdi");

#endif // __TSIMPLEDRAWING_DIRECTGDI_H__
