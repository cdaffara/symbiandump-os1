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

#ifndef __TGENERALDRAWING_H__
#define __TGENERALDRAWING_H__

#include "tdirectgdi_test_step_base.h"
#include <bitdev.h>

/**
Class for testing the general drawing functionality in DirectGdi vs BitGdi. Drawing to default context,
reseted context and using various draw modes are tested.
*/
class CTGeneralDrawing : public CTDirectGdiStepBase
	{
public:
	CTGeneralDrawing();
	~CTGeneralDrawing();
	
private:
	// Main test methods
 	void TestDrawToDefaultContextL();
	void TestDrawToResetContextL();	
	void TestSetDrawMode();
	void TestSetDrawModeInvalidParametersL();
	void TestMoveBy();
	void TestInternalDrawingPositionDrawRect();
	void TestDrawShapesIrregularPenSizeL(const TSize& aPenSize);
	void TestDrawShapesChangingOriginL();
	void DrawToClosedActiveTargetL();
 	
	// From CStepStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();	
	
	// from CTDirectGdiStepBase
	void RunTestsL();
	
	};		
	
_LIT(KTDirectGdiGeneralDrawingStep,"TDirectGDIGeneralDrawing");

#endif
