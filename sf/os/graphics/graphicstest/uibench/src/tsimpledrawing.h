// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 
#ifndef __TSIMPLEDRAWING_H__
#define __TSIMPLEDRAWING_H__

#include "te_graphicsperformanceSuiteStepBase.h"
#include <biditext.h>

class CTSimpleDrawing : public CTe_graphicsperformanceSuiteStepBase
	{
public:
	CTSimpleDrawing();
	~CTSimpleDrawing();
	
private:
	enum TMbmTbmp
		{
		EMbmTbmpTbmp,
		EMbmTbmpTblank,
		EMbmTbmpTgdigen,
		EMbmTbmpTcolor
		};
	
	// From CStepStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	
	void ClearingL();
	void LineDrawingL();
	void CopyRectL();
	void TextDrawingL();
	void TextDrawingAntiAliasedL();
	void TextDrawingWithOverheadL();
	void LargeTextDrawingL();
	void TextDrawingExtendedLtoRL();
	void TextDrawingExtendedRtoLL();
	void LargeTextDrawingExtendedLtoRL();
	void LargeTextDrawingExtendedRtoLL();
	void BidiTextDrawingL();
	void LargeBidiTextDrawingL();
	void MeasureTextL();
	void BitmapDrawingL();
	void ShadowingL();
	void ColorMappingL();
	void AndClearingL();
	void AndCopyRectL();
	void MaskBlittingL();
	void VerticalLinesL();
	void HorizontalLinesL();
	void ThickLinesL();
	void DrawRectL();
	void BltGcL();
	void BltBmpL();

	
private:	
	TUint iStartTime;
	TUint iStopTime;	
	CBitmapDevice* iDevice;	
	CFont* iFont;
	CFont* iLargeFont;
	CFbsBitmap* iBitmap;
	TBidiText* iBidiText;
	TRect iScreenRect;	
	CFbsTypefaceStore* iTs; 
	};
	
_LIT(KTSimpleDrawingPerfName,"tsimpledrawing");

#endif
