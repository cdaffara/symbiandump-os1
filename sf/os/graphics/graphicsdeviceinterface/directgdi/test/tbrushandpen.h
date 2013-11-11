// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TBRUSHANDPEN_H__
#define __TBRUSHANDPEN_H__

#include "tdirectgdi_test_step_base.h"
#include <bitdev.h>

/**
Class for testing the brush and pen usage in DirectGdi vs BitGdi. 
*/
class CTBrushAndPen : public CTDirectGdiStepBase
	{
public:
	CTBrushAndPen();
	~CTBrushAndPen();
	
private:
	// Main test methods	
	void TestBrushColorAndStyleL(const DirectGdi::TDrawMode aDrawMode);
	void TestBrushPatternOriginL();
	void TestPenColorSizeL(const DirectGdi::TDrawMode aDrawMode);
	void TestPenStyleL();
	void TestBrushAndPenInvalidParametersL();	
	
	//helper methods
	void SetHashColors(TInt aGeneralIndex, TInt aAlphaStep, TInt aPenBrushIndex);
	
	// From CStepStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();	
	
	// from CTDirectGdiStepBase
	void RunTestsL();
	};		
	
_LIT(KTDirectGdiBrushAndPenStep,"TDirectGDIBrushAndPen");

#endif
