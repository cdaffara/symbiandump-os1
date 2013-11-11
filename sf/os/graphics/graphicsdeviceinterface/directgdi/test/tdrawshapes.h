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

#ifndef __TDRAWSHAPES_H__
#define __TDRAWSHAPES_H__

#include "tdirectgdi_test_step_base.h"
#include <bitdev.h>

enum TDrawShapeAPI
	{
	EDrawLine,
	EDrawLineTo,
	EDrawLineBy,
	EDrawRect,
	EDrawRoundRect,
	EDrawEllipse,
	EDrawPie,
	EDrawArc,
	EDrawPolyLine,
	EDrawPolyLineNoEndPoint,
	EDrawPolygon
	};

/**
Class for testing the drawing of shapes in DirectGdi vs BitGdi. Shapes tested are
rect, round rect, line, plot, arc, pie and ellipse. Clearing and various pen and 
brush styles are also tested in this class.
 */
class CTDrawShapes : public CTDirectGdiStepBase
	{
public:
	CTDrawShapes();
	~CTDrawShapes();
	
private:
	// Main test methods
	void TestBasicDrawShapeL();
	void TestDrawLineL(TDrawShapeAPI aApi);
	void TestDrawShapePositionAndSizeL(TDrawShapeAPI aApi, DirectGdi::TPenStyle, DirectGdi::TBrushStyle aBrushStyle);
	void TestDrawShapeInvalidParametersL(TDrawShapeAPI aApi);
	void TestDrawRoundRectL(DirectGdi::TBrushStyle aBrushStyle=DirectGdi::ENullBrush);
	void TestPlot();
	void TestDrawArcL();
	void TestDrawPieL(DirectGdi::TBrushStyle aBrushStyle=DirectGdi::ENullBrush);
	void TestDrawPolyLineL(TDrawShapeAPI aApi);
	void TestDrawPolygonL(TInt aWidth, TInt aHeight,
			DirectGdi::TBrushStyle aBrushStyle=DirectGdi::ENullBrush,
			DirectGdi::TFillRule aFillRule=DirectGdi::EAlternate);
	void TestDrawLargeArc();
	void TestSetAttributesInvalidParametersL();
	void TestDrawShapeTransparentOutlineL();

	// From CStepStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();	
	
	// from CTDirectGdiStepBase
	void RunTestsL();
	
	};		
	
_LIT(KTDirectGdiDrawShapeStep,"TDirectGDIDrawShape");

#endif
