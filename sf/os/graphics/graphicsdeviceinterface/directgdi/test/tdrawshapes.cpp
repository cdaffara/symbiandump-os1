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

#include "tdrawshapes.h"

#include <e32math.h>


CTDrawShapes::CTDrawShapes()
	{
	SetTestStepName(KTDirectGdiDrawShapeStep);
	}

CTDrawShapes::~CTDrawShapes()
	{
	}

/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-SHAPEDRAWING-0001

@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Test shape drawing with basic parameters.

@SYMTestStatus
	Implemented

@SYMTestPriority
	Critical

@SYMTestActions
	Test various shape drawing methods:
	void Plot(const TPoint& aPoint)
		- Draw points on the destination surface in various colours (use colours from
		COLOR_TABLE), increasing the distance between the points.
	void DrawLine(const TPoint& aStart, const TPoint& aEnd)
		- Draw lines with various colours (use colours from COLOR_TABLE).
		- Draw line from top-left to bottom-right corner. Move starting point to the right
		and ending point to the left and draw again. Repeat increasing the distance between
		the previous and next starting and ending points, until reaching a position outside
		the destination surface.
		- Draw line from top-left to bottom-right corner. Move starting point down and ending
		point up and draw again. Repeat increasing the distance between the previous and next
		starting and ending points, until reaching a position outside the destination surface.
	void DrawLineTo(const TPoint& aPoint)
		- Draw lines with various colours (use colours from COLOR_TABLE).
		- Draw line from top-left corner to the bottom of the drawing area. Now draw a line
		in opposite direction, moving the end point a little to the right. Repeat, increasing
		the distance, by which the end point is moved right, until reaching a position outside
		the destination surface.
	void DrawLineBy(const TPoint& aVector)
		- Draw lines with various colours (use colours from COLOR_TABLE).
		- Draw line from top-left corner to the right end of the drawing area. Now draw a line
		in opposite direction, moving the end point a little down. Repeat, increasing the distance,
		by which the end point is moved down, until reaching a position outside the destination surface.
	void DrawRect(const TRect& aRect)
		- Draw empty and filled rectangles.
		- Draw rectangles with various colours (pen & brush colours; use colours from COLOR_TABLE).
		- Draw rectangle with size of the whole drawing area, shrink it and repeat until rectangle
		size reaches 0.
	void DrawRoundRect(const TRect& aRect, const TSize& aEllipse)
		- Draw empty and filled rectangles.
		- Draw rectangles with various colours (pen & brush colours; use colours from COLOR_TABLE).
		- Draw rectangle with size of the whole drawing area, shrink it, increase  the round corners
		size and repeat until rectangle size reaches 0.
	void DrawArc(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd)
		- Draw arcs with various colours (use colours from COLOR_TABLE).
		- Draw an arc within the rectangle size of entire drawing area.
		- Shrink the rectangle.
		- Repeat until rectangle size reaches zero, move arc's start and end points.
	void DrawPie(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd)
		- Draw empty and filled pies.
		- Draw pies with various colours (pen & brush colours; use colours from COLOR_TABLE).
		- Draw a pie within the rectangle size of entire drawing area.
		- Shrink the rectangle.
		- Repeat until rectangle size reaches zero, move pie's start and end points.
	void DrawPolyLine(const TArray<TPoint>& aPointList)
	void DrawPolyLineNoEndPoint(const TArray<TPoint>& aPointList)
		- Draw a poly line in the target area, constructed from a large number of points.
	void DrawPolygon(const TArray<TPoint>& aPointList, CGraphicsContext::TFillRule aFillRule=CGraphicsContext::EAlternate)
		- Draw a polygon on the target area, constructed from a large number of points.
	void DrawEllipse(const TRect& aRect)
		- Draw empty and filled ellipses.
		- Draw ellipses with various colours (pen & brush colours; use colours from COLOR_TABLE).
		- Use rectangle (aRect) with size of the whole drawing area, shrink it and repeat until
		rectangle size reaches zero.

@SYMTestExpectedResults
	All graphic operations completed successfully without errors. Individual shapes should
	be drawn in the target area. Valid bitmap should be created. This bitmap should be
	the same as a reference one.
*/
void CTDrawShapes::TestBasicDrawShapeL()
	{
	_LIT(KTestName, "ShapeDrawing-Basic"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}

	ResetGc();

	TSize size(iGdiTarget->SizeInPixels().iWidth/4,
			   iGdiTarget->SizeInPixels().iHeight/4);

	for(TInt i=0; i<17; i++)
		{
		TPoint pos(size.iWidth*(i%4), size.iHeight*(i/4));

		iGc->Reset();

		switch(i)
			{
			case 0: // void Plot(const TPoint& aPoint)
				{
				TInt step = 0;
				for(TInt y=0; y<size.iHeight; y+=step)
					{
					for(TInt x=0; x<size.iWidth; x+=step)
						{
						iGc->SetPenColor(KColor16Table[step%16]);
						iGc->SetPenStyle(DirectGdi::ENullPen);
						iGc->Plot(TPoint(x, y)+pos);
						TESTNOERRORL(iGc->GetError());
						iGc->SetPenStyle(DirectGdi::ESolidPen);
						for(TInt penSize = 0; penSize <= 2; penSize++)
							{
							iGc->SetPenSize(TSize(penSize,penSize));
							iGc->Plot(TPoint(x, y)+pos);
							TESTNOERRORL(iGc->GetError());
							}						
						step++;
						}
					}
				break;
				}

			case 1: // void DrawLine(const TPoint& aStart, const TPoint& aEnd)
				{
				TInt step = 0;
				for(TInt x1=0,x2=size.iWidth-1; x1<size.iWidth; x1+=step,x2-=step)
					{
					iGc->SetPenColor(KColor16Table[step%16]);
					iGc->DrawLine(TPoint(x1, 0)+pos, TPoint(x2, size.iHeight-1)+pos);
					TESTNOERRORL(iGc->GetError());
					step++;
					}
				
				step = 0;
				for(TInt y1=0,y2=size.iHeight-1; y1<size.iHeight; y1+=step,y2-=step)
					{
					iGc->SetPenColor(KColor16Table[step%16]);
					iGc->DrawLine(TPoint(0, y1)+pos, TPoint(size.iWidth-1, y2)+pos);
					TESTNOERRORL(iGc->GetError());
					step++;
					}

				break;
				}

			case 2: // void DrawLineTo(const TPoint& aPoint), void DrawLineBy(const TPoint& aVector)
				{
				iGc->MoveTo(pos);
				for(TInt x1=1,a=1,s=1; x1<size.iWidth; a++,x1+=a,s=-s)
					{
					iGc->SetPenColor(KColor16Table[a%16]);
					iGc->DrawLineTo(TPoint(x1, (s>0)*(size.iHeight-1))+pos);
					TESTNOERRORL(iGc->GetError());
					}
				
				iGc->MoveTo(pos);
				for(TInt y1=1,a=1,s=1; y1<size.iHeight; a++,y1+=a,s=-s)
					{
					iGc->SetPenColor(KColor16Table[a%16]);
					iGc->DrawLineBy(TPoint(s*(size.iWidth-1), a));
					TESTNOERRORL(iGc->GetError());
					}

				break;
				}

			case 4: // void DrawRect(const TRect& aRect), filled
				{
				iGc->SetBrushStyle(DirectGdi::ESolidBrush);
				}
			case 3: // void DrawRect(const TRect& aRect)
				{
				TRect rect(pos, size);
				for(TInt a=0; (rect.Size().iWidth>0) && (rect.Size().iHeight>0); a++,rect.Shrink(3, 3))
					{
					iGc->SetPenColor(KColor16Table[a%16]);
					iGc->SetBrushColor(KColor16Table[(a+2)%16]);
					iGc->DrawRect(rect);
					TESTNOERRORL(iGc->GetError());
					}
				break;
				}
			case 6: // void DrawRoundRect(const TRect& aRect, const TSize& aEllipse), filled
				{
				iGc->SetBrushStyle(DirectGdi::ESolidBrush);
				}
			case 5: // void DrawRoundRect(const TRect& aRect, const TSize& aEllipse)
				{
				TRect rect(pos, size);
				TSize csize(1, 1);
				for(TInt a=0; (rect.Size().iWidth>0) && (rect.Size().iHeight>0); a++,rect.Shrink(5, 5),csize+=TSize(2, 2))
					{
					iGc->SetPenColor(KColor16Table[a%16]);
					iGc->SetBrushColor(KColor16Table[(a+2)%16]);
					for(TInt penSize = 0; penSize <= 1; penSize++)
						{
						iGc->SetPenSize(TSize(penSize,penSize));
						iGc->SetPenStyle(DirectGdi::ENullPen);
						iGc->DrawRoundRect(rect, csize);
						TESTNOERRORL(iGc->GetError());
						iGc->SetPenStyle(DirectGdi::ESolidPen);
						iGc->DrawRoundRect(rect, csize);
						TESTNOERRORL(iGc->GetError());
						iGc->SetPenStyle(DirectGdi::EDottedPen);
						iGc->DrawRoundRect(rect, csize);
						TESTNOERRORL(iGc->GetError());
						}
					}
				break;
				}

			case 7: // void DrawArc(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd)
				{
				TRect rect(pos, size);
				TPoint pos2(pos);
				pos2 += TPoint(size.iWidth/2, size.iHeight/2); // center
				TReal start = KPiBy2;
				TReal end = 0.0;
				for(TInt a=0; (rect.Size().iWidth>0) && (rect.Size().iHeight>0); a++,rect.Shrink(3, 3),start+=0.3,end+=0.2)
					{
					TReal ss;
					TReal cc;
					Math::Sin(ss, start);
					Math::Cos(cc, start);
					TInt32 startX;
					TInt32 startY;
					Math::Int(startX, cc*100);
					Math::Int(startY, ss*100);
					Math::Sin(ss, end);
					Math::Cos(cc, end);
					TInt32 endX;
					TInt32 endY;
					Math::Int(endX, cc*100);
					Math::Int(endY, ss*100);
					
					iGc->SetPenColor(KColor16Table[a%16]);
					iGc->DrawArc(rect, TPoint(startX, startY)+pos2, TPoint(endX, endY)+pos2);
					TESTNOERRORL(iGc->GetError());
					}

				break;
				}

			case 9: // void DrawPie(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd), filled
					{
					iGc->SetBrushStyle(DirectGdi::ESolidBrush);
					}
			case 8: // void DrawPie(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd)
				{
				TRect rect(pos, size);
				TPoint pos2(pos);
				pos2 += TPoint(size.iWidth/2, size.iHeight/2); // center
				TReal start = 0.0;
				TReal end = 0.1;
				for(TInt a=0; (rect.Size().iWidth>0) && (rect.Size().iHeight>0); a++,rect.Shrink(4, 4),start+=end,end+=0.04)
					{
					TReal ss;
					TReal cc;
					Math::Sin(ss, start+end);
					Math::Cos(cc, start+end);
					TInt32 startX;
					TInt32 startY;
					Math::Int(startX, cc*100);
					Math::Int(startY, ss*100);
					Math::Sin(ss, start);
					Math::Cos(cc, start);
					TInt32 endX;
					TInt32 endY;
					Math::Int(endX, cc*100);
					Math::Int(endY, ss*100);
					
					iGc->SetPenColor(KColor16Table[a%16]);
					iGc->SetBrushColor(KColor16Table[(a+2)%16]);
					iGc->DrawPie(rect, TPoint(startX, startY)+pos2, TPoint(endX, endY)+pos2);
					TESTNOERRORL(iGc->GetError());
					}

				break;
				}

			case 10: // void DrawPolyLine(const TArray<TPoint>* aPointList)
			case 11: // void DrawPolyLineNoEndPoint(const TArray<TPoint>* aPointList)
				{
				TInt numPoints = 60;
				CArrayFixFlat<TPoint> *array = new (ELeave)CArrayFixFlat<TPoint>(numPoints);
				CleanupStack::PushL(array);
				TPoint pos2(pos);
				pos2 += TPoint(size.iWidth/2, size.iHeight/2); // center
				TReal angle = 0.0;
				for(TInt r=numPoints; r>0; r--,angle+=0.47)
					{
					TReal ss;
					TReal cc;
					Math::Sin(ss, angle);
					Math::Cos(cc, angle);
					TInt32 x;
					TInt32 y;
					Math::Int(x, cc*r);
					Math::Int(y, ss*r);

					array->AppendL(TPoint(x, y)+pos2);
					}

				if(i == 10)
					{
					for(TInt penSize = 0; penSize <= 1; penSize++)
						{
						iGc->SetPenSize(TSize(penSize,penSize));
						iGc->DrawPolyLine(*array);
						}
					}
				else //if(i == 11)
					{
					iGc->DrawPolyLineNoEndPoint(*array);
					}
				TESTNOERRORL(iGc->GetError());
				CleanupStack::PopAndDestroy(array);

				break;
				}

			case 13:// TInt DrawPolygon(const TArray<TPoint>* aPointList, CGraphicsContext::TFillRule aFillRule=CGraphicsContext::EAlternate), filled
					{
					iGc->SetBrushStyle(DirectGdi::ESolidBrush);
					iGc->SetBrushColor(KColor16Table[1]);
					}
			case 12:// TInt DrawPolygon(const TArray<TPoint>* aPointList, CGraphicsContext::TFillRule aFillRule=CGraphicsContext::EAlternate)
				{
				TInt numPoints = 20;
				CArrayFixFlat<TPoint> *array = new (ELeave)CArrayFixFlat<TPoint>(numPoints);
				CleanupStack::PushL(array);
				TPoint pos2(pos);
				pos2 += TPoint(size.iWidth/2, size.iHeight/2); // center
				TReal angle = 0.0;
				for(TInt r=60; r>0; r-=3,angle+=0.47)
					{
					TReal ss;
					TReal cc;
					Math::Sin(ss, angle);
					Math::Cos(cc, angle);
					TInt32 x;
					TInt32 y;
					Math::Int(x, cc*r);
					Math::Int(y, ss*r);

					array->AppendL(TPoint(x, y)+pos2);
					}

				iGc->DrawPolygon(*array, DirectGdi::EAlternate);
				TESTNOERRORL(iGc->GetError());

				CleanupStack::PopAndDestroy(array);
				
				break;
				}

			case 15: // void DrawEllipse(const TRect& aRect), filled
				{
				iGc->SetBrushStyle(DirectGdi::ESolidBrush);
				}
			case 14: // void DrawEllipse(const TRect& aRect)
				{
				TRect rect(pos, size);
				for(TInt a=0; (rect.Size().iWidth>0) && (rect.Size().iHeight>0); a++,rect.Shrink(3, 5))
					{
					iGc->SetPenColor(KColor16Table[a%16]);
					iGc->SetBrushColor(KColor16Table[(a+2)%16]);
					iGc->DrawEllipse(rect);
					TESTNOERRORL(iGc->GetError());
					}

				break;
				}
			}

		TESTNOERRORL(iGc->GetError());
		}

	TESTNOERROR(WriteTargetOutput(iTestParams, KTestName()));
	}


/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-SHAPEDRAWING-0002

@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Test line drawing with basic parameters.

@SYMTestStatus
	Implemented

@SYMTestPriority
	Critical

@SYMTestActions
	Set default pen.
		- Draw lines in all directions.
		- Draw lines starting or/and ending outside the target surface.		
	Call functions with valid parameters.
	void DrawLine(const TPoint& aStart, const TPoint& aEnd)
		The test is implemented the following way: lines will be drawn from a little
		distance from the centre (to avoid lines overlapping) to a point outside of the surface
		(testing surface boundary clipping). The next line will be drawn in opposite direction
		(from outside to the centre) at a slightly incremented angle. 
	void DrawLineBy(const TPoint& aVector)
		Test for DrawLineBy() is implemented in a similar way, with checking the continuity
		of line drawing (drawing the next line from the end of the previous one).
	void DrawLineTo(const TPoint& aPoint)
		Test for DrawLineTo() is implemented in a similar way, with checking the continuity
		of line drawing (drawing the next line from the end of the previous one).
		
@SYMTestExpectedResults
	Valid bitmap should be created. This bitmap shall be compared to a reference bitmap.
*/
void CTDrawShapes::TestDrawLineL(TDrawShapeAPI aApi)
	{
	_LIT(KDrawLine, "ShapeDrawing-DrawLine");
	_LIT(KDrawLineTo, "ShapeDrawing-DrawLineTo");
	_LIT(KDrawLineBy, "ShapeDrawing-DrawLineBy");

	TBuf<30> testName;
	if(aApi == EDrawLine)
		testName = KDrawLine();
	else if(aApi == EDrawLineTo)
		testName = KDrawLineTo();
	else if(aApi == EDrawLineBy)
		testName = KDrawLineBy();
	else
		ASSERT(EFalse); // test not supported for shapes other than those above

	if(!iRunningOomTests)
		{
		INFO_PRINTF1(testName);
		}
	
	ResetGc();

	TPoint center(iGdiTarget->SizeInPixels().iWidth/2,
			iGdiTarget->SizeInPixels().iHeight/2);

	TInt steps = 180;
	TReal radius1 = 400.0;
	TReal radius2 = 70.0;

	TPoint pp;

	if(aApi != EDrawLine)
		{
		const TInt radius = 200;
		radius1 = radius;
		radius2 = 90.0;
		pp = center;
		pp.iX += radius;
		iGc->MoveTo(pp);
		}

	for(TInt i=0; i<steps; i++)
		{
		TReal angle = KPi*2.0*i/steps;

		TReal angleSin, angleCos;
		Math::Sin(angleSin, angle);
		Math::Cos(angleCos, angle);

		TPoint p1(static_cast<TInt>(angleCos*radius1), static_cast<TInt>(angleSin*radius1));
		p1 += center;
		TPoint p2(static_cast<TInt>(angleCos*radius2), static_cast<TInt>(angleSin*radius2));
		p2 += center;

		if(i&1)
			{
			TPoint pp = p1;
			p1 = p2;
			p2 = pp;
			}

		if(aApi == EDrawLine)
			{
			for(TInt penSize = 0; penSize <= 1; penSize ++)
				{
				iGc->SetPenSize(TSize(penSize,penSize));
				iGc->SetPenStyle(DirectGdi::ESolidPen);
				iGc->DrawLine(p1, p2);
				}				
			}
		else if(aApi == EDrawLineTo)
			{
			iGc->DrawLineTo(p1);
			iGc->DrawLineTo(p2);
			}
		else // if(aApi == EDrawLineBy)
			{
			iGc->DrawLineBy(p1-pp);
			iGc->DrawLineBy(p2-p1);
			pp = p2;
			}
		}

	TESTNOERRORL(iGc->GetError());
	TESTNOERROR(WriteTargetOutput(iTestParams, testName));
	}


/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-SHAPEDRAWING-0003

@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Test correctness of size and position reproduction of rectangle, ellipse, arc, and pie.

@SYMTestStatus
	Implemented

@SYMTestPriority
	Critical

@SYMTestActions
	Set default pen and brush.
		- Draw shapes at various positions and sizes.
		- Test position and size outside the target surface.
		- Test boundary positions and sizes.
	Call functions with valid parameters:
	void DrawRect(const TRect& aRect)
		The size of bounding rectangle (aRect) will be iterated over (both height and width),
		each time calling the appropriate drawing method.
	For DrawArc() and DrawPie also the starting and ending points will be changing.

@SYMTestExpectedResults
	Valid bitmap should be created. This bitmap shall be compared to a reference bitmap.
*/
void CTDrawShapes::TestDrawShapePositionAndSizeL(TDrawShapeAPI aApi,
		DirectGdi::TPenStyle aPenStyle,
		DirectGdi::TBrushStyle aBrushStyle)
	{
	_LIT(KDrawRectPositionAndSize, "ShapeDrawing-DrawRect-PositionAndSize");
	_LIT(KDrawRoundRectPositionAndSize, "ShapeDrawing-DrawRoundRect-PositionAndSize");
	_LIT(KDrawEllipsePositionAndSize, "ShapeDrawing-DrawEllipse-PositionAndSize");
	_LIT(KDrawPiePositionAndSize, "ShapeDrawing-DrawPie-PositionAndSize");
	_LIT(KDrawArcPositionAndSize, "ShapeDrawing-DrawArc-PositionAndSize");

	TBuf<70> testName;
	if(aApi == EDrawRect)
		testName = KDrawRectPositionAndSize();
	else if(aApi == EDrawRoundRect)
		testName = KDrawRoundRectPositionAndSize();
	else if(aApi == EDrawEllipse)
		testName = KDrawEllipsePositionAndSize();
	else if(aApi == EDrawPie)
		testName = KDrawPiePositionAndSize();
	else if(aApi == EDrawArc)
		testName = KDrawArcPositionAndSize();
	else
		ASSERT(EFalse); // test not supported for shapes other than those above

	if(aPenStyle == DirectGdi::ENullPen)
		{
		testName.Append(KSeparator);
		testName.Append(KPenNameNull);
		}
	
	if(aBrushStyle != DirectGdi::ENullBrush)
		{
		testName.Append(KSeparator);
		testName.Append(KBrushStyleTableNames[aBrushStyle]);
		}

	if(!iRunningOomTests)
		{
		INFO_PRINTF1(testName);
		}
	
	ResetGc();

	TInt width = iGdiTarget->SizeInPixels().iWidth;
	TInt height = iGdiTarget->SizeInPixels().iHeight;
	TInt rectWidth = 0;
	TInt rectHeight = 0;
	TInt step = 0;

	iGc->SetPenStyle(aPenStyle);
	iGc->SetBrushStyle(aBrushStyle);

	for(TInt y=-45, offsetY=0; y<height+30; y+=rectHeight+1, offsetY++, rectHeight++)
		{
		rectWidth = 0;
		for(TInt x=-45, offsetX=0; x<width+30; x+=rectWidth+1, offsetX++, rectWidth++)
			{
			TRect rect(TPoint(x+offsetY, y+offsetX), TSize(rectWidth, rectHeight));

			iGc->SetBrushColor(KColor16Table[step%16]);
			step++;

			if(aApi == EDrawRect)
				{
				iGc->DrawRect(rect);
				}
			else if(aApi == EDrawRoundRect)
				{
				iGc->DrawRoundRect(rect, TSize(offsetX/3, offsetY/3));
				}
			else if(aApi == EDrawEllipse)
				{
				iGc->DrawEllipse(rect);
				}
			else
				{
				TReal angle1 = offsetX*0.2;
				TReal angle2 = angle1-(offsetY+1)*0.18;

				TReal angleSin, angleCos;
				Math::Sin(angleSin, angle1);
				Math::Cos(angleCos, angle1);
				TPoint p1(static_cast<TInt>(angleCos*100), static_cast<TInt>(angleSin*100));

				Math::Sin(angleSin, angle2);
				Math::Cos(angleCos, angle2);
				TPoint p2(static_cast<TInt>(angleCos*100), static_cast<TInt>(angleSin*100));

				p1 += rect.Center();
				p2 += rect.Center();

				if(aApi == EDrawPie)
					{
					iGc->DrawPie(rect, p1, p2);
					}
				else //if(aApi == EDrawArc)
					{
					iGc->DrawArc(rect, p1, p2);
					}
				}
			TESTNOERRORL(iGc->GetError());
			}
		}
	TESTNOERROR(WriteTargetOutput(iTestParams, testName));
	}


/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-SHAPEDRAWING-0004

@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Test functions behaviour after calling with invalid parameters.

@SYMTestStatus
	Implemented

@SYMTestPriority
	Critical

@SYMTestActions
	Test shape drawing methods invalid parameters handling.
	Set default pen and brush.
	Set valid parameters (see ShapeDrawing-Basic).
	Replace valid parameters with invalid ones:
		negative size
			aRect.iTl = ( -30, -30 ) ; aRect.iBr = ( -10, -10 )
			aRect.iTl = ( 30, 30 ) ; aRect.iBr = ( 10, 10 )
			aEllipse.iWidth = -30
			aEllipse.iHeight = -30
		zero size
			aRect.iTl = ( -30, -30 ) ; aRect.iBr = ( -30, -30 )
			aRect.iTl = ( 30, 30 ) ; aRect.iBr = ( 30, 30 )
			Trect(10,50,10,80)
			TRect(80,90,140,90)
			aEllipse.iWidth = 0
			aEllipse.iHeight = 0

@SYMTestExpectedResults
	Function shall detect invalid parameters and return. Nothing will be drawn on the target surface.
*/
void CTDrawShapes::TestDrawShapeInvalidParametersL(TDrawShapeAPI aApi)
	{
	_LIT(KDrawRectInvalidParameters, "ShapeDrawing-DrawRect-InvalidParameters");
	_LIT(KDrawRoundRectInvalidParameters, "ShapeDrawing-DrawRoundRect-InvalidParameters");
	_LIT(KDrawEllipseInvalidParameters, "ShapeDrawing-DrawEllipse-InvalidParameters");
	_LIT(KDrawPieInvalidParameters, "ShapeDrawing-DrawPie-InvalidParameters");
	_LIT(KDrawArcInvalidParameters, "ShapeDrawing-DrawArc-InvalidParameters");
	_LIT(KDrawPolygonInvalidParameters, "ShapeDrawing-DrawPolygon-InvalidParameters");
	
	TBuf<70> testName;
	if(aApi == EDrawRect)
		testName = KDrawRectInvalidParameters();
	else if(aApi == EDrawRoundRect)
		testName = KDrawRoundRectInvalidParameters();
	else if(aApi == EDrawEllipse)
		testName = KDrawEllipseInvalidParameters();
	else if(aApi == EDrawPie)
		testName = KDrawPieInvalidParameters();
	else if(aApi == EDrawArc)
		testName = KDrawArcInvalidParameters();
	else if(aApi == EDrawPolygon)
		testName = KDrawPolygonInvalidParameters();
	else
		ASSERT(EFalse); // test not supported for shapes other than those above

	if(!iRunningOomTests)
		{
		INFO_PRINTF1(testName);
		}
	
	ResetGc();

	RArray<TRect> rects;
	CleanupClosePushL(rects);

	RArray<TSize> sizes;
	CleanupClosePushL(sizes);

	// negative rect size
	rects.AppendL(TRect(-30, -30, -10, -10));
	rects.AppendL(TRect(30, 30, 10, 10));
	rects.AppendL(TRect(100, 30, 120, 10));
	rects.AppendL(TRect(30, 100, 10, 120));
	
	// negative ellipse size
	sizes.AppendL(TSize(-30, 30));
	sizes.AppendL(TSize(30, -30));
	sizes.AppendL(TSize(-30, -30));

	// zero rect size
	rects.AppendL(TRect(-30, -30, -30, -30));
	rects.AppendL(TRect(30, 30, 30, 30));
	rects.AppendL(TRect(10, 50, 10, 80));
	rects.AppendL(TRect(80, 90, 140, 90));

	// zero ellipse size
	sizes.AppendL(TSize(0, 30));
	sizes.AppendL(TSize(30, 0));
	sizes.AppendL(TSize(0, 0));

	switch(aApi)
		{
		case EDrawRect:
			{
			for(TInt i=0; i<rects.Count(); i++)
				{
				for(TInt penSize = 0; penSize <= 1; penSize++)
					{
					iGc->SetPenSize(TSize(penSize,penSize));
					iGc->DrawRect(rects[i]);
					TESTNOERRORL(iGc->GetError());
					}
				}
			break;
			}

		case EDrawRoundRect:
			{
			for(TInt i=0; i<rects.Count(); i++)
				{
				for(TInt j=0; j<sizes.Count(); j++)
					{
					iGc->DrawRoundRect(rects[i], sizes[j]);
					TESTNOERRORL(iGc->GetError());
					}
				}
			break;
			}

		case EDrawEllipse:
			{
			for(TInt i=0; i<rects.Count(); i++)
				{		
				iGc->DrawEllipse(rects[i]);
				TESTNOERRORL(iGc->GetError());
				}
			break;
			}

		case EDrawPie:
			{
			for(TInt i=0; i<rects.Count(); i++)
				{
				TPoint pos(rects[i].Center());		
				iGc->SetPenStyle(DirectGdi::ENullPen);
				iGc->DrawPie(rects[i], TPoint(100, 0)+pos, TPoint(0, -100)+pos);
				iGc->SetPenStyle(DirectGdi::ESolidPen);
				for(TInt penSize = 0; penSize <= 1; penSize ++)
					{
					iGc->SetPenSize(TSize(penSize,penSize));
					iGc->DrawPie(rects[i], TPoint(100, 0)+pos, TPoint(0, -100)+pos);
					TESTNOERRORL(iGc->GetError());
					}				
				}
			break;
			}

		case EDrawArc:
			{
			for(TInt i=0; i<rects.Count(); i++)
				{
				TPoint pos(rects[i].Center());
				iGc->SetPenStyle(DirectGdi::ENullPen);
				iGc->DrawArc(rects[i], TPoint(100, 0)+pos, TPoint(0, -100)+pos);
				iGc->DrawArc(rects[i], TPoint(0, -100)+pos, TPoint(100, 0)+pos);
				iGc->SetPenStyle(DirectGdi::ESolidPen);
				for(TInt penSize = 0; penSize <= 1; penSize ++)
					{
					iGc->SetPenSize(TSize(penSize,penSize));
					iGc->DrawArc(rects[i], TPoint(100, 0)+pos, TPoint(0, -100)+pos);
					}
				TESTNOERRORL(iGc->GetError());
				}
			break;
			}
			
		case EDrawPolygon:
			{
			// Try to draw a polygon with 0 points, and a polygon with an invalid fill argument
			CArrayFixFlat<TPoint> *tmpArray = new (ELeave)CArrayFixFlat<TPoint>(1);			
			iGc->DrawPolygon(*tmpArray, DirectGdi::EAlternate);
			TESTNOERROR(iGc->GetError());
			iGc->DrawPolygon(*tmpArray, static_cast<DirectGdi::TFillRule>(-1));
			if (iUseDirectGdi)
				{
				TEST(iGc->GetError() == KErrArgument);
				}
			delete tmpArray;
			break;
			}
		}

	iGc->Reset();
	CleanupStack::PopAndDestroy(&sizes);
	CleanupStack::PopAndDestroy(&rects);

	// test if target is still clear
	TBool pass = TestTargetL(KRgbWhite);
	if(!pass)
		{
		// write target only if test failed
		TEST(KErrNone == WriteTargetOutput(iTestParams, testName));
		}
	TEST(pass);
	}

/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-SHAPEDRAWING-0005

@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Test a few round rectangles, some of which have corners larger than the width
	or height or the rectangles.

@SYMTestStatus
	Implemented

@SYMTestPriority
	Critical

@SYMTestActions
	The tests is implemented the following way: the rectangles' corners' width and height
	will be iterated over, and for each	iteration a set of rounded rectangles will be drawn.
	The test will be repeated with a solid brush.

@SYMTestExpectedResults
	Valid bitmap should be created. This bitmap shall be compared to a reference bitmap.
*/
void CTDrawShapes::TestDrawRoundRectL(DirectGdi::TBrushStyle aBrushStyle)
	{
	_LIT(KDrawRoundRect, "ShapeDrawing-DrawRoundRect");

	TBuf<50> testName;
	testName = KDrawRoundRect();
	if(aBrushStyle != DirectGdi::ENullBrush)
		{
		testName.Append(KSeparator);
		testName.Append(KBrushStyleTableNames[aBrushStyle]);
		}
	
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(testName);
		}
	
	ResetGc();

	TInt width = iGdiTarget->SizeInPixels().iWidth;
	TInt height = iGdiTarget->SizeInPixels().iHeight;
	const TInt xRectNum = 16;
	const TInt yRectNum = 16;
	TInt step = 0;

	iGc->SetBrushStyle(aBrushStyle);

	for(TInt i=0; i<yRectNum; i++)
		{
		for(TInt j=0; j<xRectNum; j++)
			{
			TRect rect(TPoint(j*width/xRectNum, i*height/yRectNum), TSize(width/xRectNum, height/yRectNum));

			// draw many concentric rectangles, each smaller than previous
			while((rect.Size().iWidth > 2) && (rect.Size().iHeight > 2))
				{
				iGc->SetBrushColor(KColor16Table[step%16]);
				step++;

				iGc->DrawRoundRect(rect, TSize(j, i));
				TESTNOERRORL(iGc->GetError());
				rect.Shrink(3, 3);
				}
			}
		}
	TESTNOERROR(WriteTargetOutput(iTestParams, testName));
	}


/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-SHAPEDRAWING-0006

@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Test drawing arc functionality.

@SYMTestStatus
	Implemented

@SYMTestPriority
	Critical

@SYMTestActions
	Set default pen and brush.
	Draw various chunks of arc.
	Call functions with valid parameters.
	The tests is implemented the following way: the arc's starting angle will be
	iterated over, and for each iteration a set of arcs with different end angles will be drawn.

@SYMTestExpectedResults
	Valid bitmap should be created. This bitmap shall be compared to a reference bitmap.
*/
void CTDrawShapes::TestDrawArcL()
	{
	_LIT(KTestName, "ShapeDrawing-DrawArc"); 

	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	ResetGc();

	TInt width = iGdiTarget->SizeInPixels().iWidth;
	TInt height = iGdiTarget->SizeInPixels().iHeight;
	const TInt xRectNum = 8;
	const TInt yRectNum = 8;
	const TReal angleStep = 2.0*KPi/11.0;

	for(TInt i=0; i<yRectNum; i++)
		{
		for(TInt j=0; j<xRectNum; j++)
			{
			TRect rect(TPoint(j*width/xRectNum, i*height/yRectNum), TSize(width/xRectNum, height/yRectNum));

			TReal angle1 =(i*xRectNum+j)*2.0*KPi/(xRectNum*yRectNum);

			TReal angleSin, angleCos;
			Math::Sin(angleSin, angle1);
			Math::Cos(angleCos, angle1);
			TPoint p1(static_cast<TInt>(angleCos*100), static_cast<TInt>(angleSin*100));
			p1 += rect.Center();

			TReal angle2 = angle1-angleStep;

			// draw many concentric arcs, each smaller than previous and with different angles
			while((rect.Size().iWidth > 2) && (rect.Size().iHeight > 2))
				{
				Math::Sin(angleSin, angle2);
				Math::Cos(angleCos, angle2);
				TPoint p2(static_cast<TInt>(angleCos*100), static_cast<TInt>(angleSin*100));
				p2 += rect.Center();

				iGc->DrawArc(rect, p1, p2);
				TESTNOERRORL(iGc->GetError());

				angle2 -= angleStep;
				rect.Shrink(3, 3);
				}
			}
		}
	TESTNOERROR(WriteTargetOutput(iTestParams, KTestName()));
	}


/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-SHAPEDRAWING-0007

@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Test drawing pie functionality.

@SYMTestStatus
	Implemented

@SYMTestPriority
	Critical

@SYMTestActions
	Set default pen and brush.
	Draw various chunks of pie.
	Call functions with valid parameters (see Graphics-DirectGDI-ShapeDrawing-001).
	The tests is implemented the following way: the pie's starting angle will be
	iterated over, and for each iteration a complete circle composed of different pies will be drawn.

@SYMTestExpectedResults
	A set of 64 groups of pies should be draw, each one rotated slightly more than the last. 
	The size of the pies drawn should look the same for all rotations.
*/
void CTDrawShapes::TestDrawPieL(DirectGdi::TBrushStyle aBrushStyle)
	{
	_LIT(KDrawPie, "ShapeDrawing-DrawPie");

	TBuf<50> testName;
	testName = KDrawPie();
	if(aBrushStyle != DirectGdi::ENullBrush)
		{
		testName.Append(KSeparator);
		testName.Append(KBrushStyleTableNames[aBrushStyle]);
		}
	
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(testName);
		}
	
	ResetGc();

	TInt width = iGdiTarget->SizeInPixels().iWidth;
	TInt height = iGdiTarget->SizeInPixels().iHeight;
	const TInt xRectNum = 8;
	const TInt yRectNum = 8;
	const TReal angleStep = 2.0*KPi/21.0;
	TInt step = 0;

	iGc->SetBrushStyle(aBrushStyle);

	for(TInt i=0; i<yRectNum; i++)
		{
		for(TInt j=0; j<xRectNum; j++)
			{
			TRect rect(TPoint(j*width/xRectNum, i*height/yRectNum), TSize(width/xRectNum, height/yRectNum));

			TReal angle1 =(i*xRectNum+j)*2.0*KPi/(xRectNum*yRectNum);

			//draw six parts of pie
			for(TInt k=1; k<=6; k++)
				{
				TReal angleSin, angleCos;
				Math::Sin(angleSin, angle1);
				Math::Cos(angleCos, angle1);
				TPoint p1(static_cast<TInt>(angleCos*100), static_cast<TInt>(angleSin*100));
				p1 += rect.Center();

				TReal angle2 = angle1-k*angleStep;
				Math::Sin(angleSin, angle2);
				Math::Cos(angleCos, angle2);
				TPoint p2(static_cast<TInt>(angleCos*100), static_cast<TInt>(angleSin*100));
				p2 += rect.Center();

				iGc->SetBrushColor(KColor16Table[step%16]);
				step++;

				iGc->DrawPie(rect, p1, p2);
				TESTNOERRORL(iGc->GetError());

				angle1 -= k*angleStep;
				rect.Shrink(2, 2);
				}
			}
		}
	TESTNOERROR(WriteTargetOutput(iTestParams, testName));
	}


/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-SHAPEDRAWING-0008

@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Test drawing polyline functionality.

@SYMTestStatus
	Implemented

@SYMTestPriority
	Critical

@SYMTestActions
	Test DrawPolyLine() and DrawPolyLineNoEndPoint() methods with various valid parameters.
	Draw polylines with:
		Different number of points (including zero points).
		Different position of points.
		Boundary point position.
		Point position outside the target surface.
	The test will be implemented by repeatedly drawing the same shape with varying number of points.

@SYMTestExpectedResults
	Valid bitmap should be created. This bitmap shall be compared to a reference bitmap.
*/
void CTDrawShapes::TestDrawPolyLineL(TDrawShapeAPI aApi)
	{
	_LIT(KDrawPolyLine, "ShapeDrawing-DrawPolyLine");
	_LIT(KDrawPolyLineNoEndPoint, "ShapeDrawing-DrawPolyLineNoEndPoint");

	TBuf<50> testName;
	if(aApi == EDrawPolyLine)
		testName = KDrawPolyLine();
	else //if(aApi == EDrawPolyLineNoEndPoint)
		testName = KDrawPolyLineNoEndPoint();

	if(!iRunningOomTests)
		{
		INFO_PRINTF1(testName);
		}
	
	ResetGc();
	
	TInt width = iGdiTarget->SizeInPixels().iWidth;
	TInt height = iGdiTarget->SizeInPixels().iHeight;
	TSize size(width/10, height/10);
	TSize size2(size.iWidth/2, size.iHeight/2);

	const TInt pCount = 28;
	CArrayFixFlat<TPoint> *tmpArray = new (ELeave)CArrayFixFlat<TPoint>(pCount);
	CleanupStack::PushL(tmpArray);

	TReal angle = 0.0;
	for(TInt r=pCount-1; r>0; r--,angle+=0.6)
		{
		TReal ss;
		TReal cc;
		Math::Sin(ss, angle);
		Math::Cos(cc, angle);
		TInt32 x;
		TInt32 y;
		Math::Int(x, cc*r);
		Math::Int(y, ss*r);

		tmpArray->AppendL(TPoint(x, y)+size2.AsPoint());
		//duplicate point to test consecutive points with the same position
		if(r==15)
			{
			tmpArray->AppendL(TPoint(x, y)+size2.AsPoint());
			}
		}

	CArrayFixFlat<TPoint> *array = new (ELeave)CArrayFixFlat<TPoint>(pCount);
	CleanupStack::PushL(array);

	for(TInt y=-size2.iHeight,k=0; y<height+size2.iHeight; y+=size.iHeight, k++)
		{
		for(TInt x=-size2.iWidth,l=0; x<width+size2.iWidth; x+=size.iWidth, l++)
			{
			TPoint pos(x+k, y+l);

			TInt pNum = Abs(l-5) + Abs(k-5)*5 +1;
			if(pNum > pCount)
				pNum = pCount;

			for(TInt i=0; i<pNum; i++)
				{
				array->AppendL(tmpArray->At(i)+pos);
				}
			
			if(aApi == EDrawPolyLine)
				{
				iGc->SetPenStyle(DirectGdi::EDottedPen);
				iGc->DrawPolyLine(*array);
				iGc->SetPenStyle(DirectGdi::ESolidPen);
				iGc->DrawPolyLine(*array);
				}
			else //if(aApi == EDrawPolyLineNoEndPoint)
				iGc->DrawPolyLineNoEndPoint(*array);
			TESTNOERRORL(iGc->GetError());
			array->Delete(0, pNum);
			}
		}
	CleanupStack::PopAndDestroy(2, tmpArray);
	TESTNOERROR(WriteTargetOutput(iTestParams, testName));
	}


/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-SHAPEDRAWING-0009

@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Test drawing  polygon functionality.

@SYMTestStatus
	Implemented

@SYMTestPriority
	Critical

@SYMTestActions
	Test DrawPolygon() methods with various valid parameters.
	Draw polygons with:
		Different number of points (including zero points and two points).
		Different position of points.
		Boundary point positions.
		Point positions outside the target surface.
		Test self-crossing polygons
		Test fill rules.
	The test will be implemented by repeatedly drawing the same shape with varying number of points.

@SYMTestExpectedResults
	Valid bitmap should be created. This bitmap shall be compared to a reference bitmap.
*/
void CTDrawShapes::TestDrawPolygonL(TInt aWidth, TInt aHeight, DirectGdi::TBrushStyle aBrushStyle, DirectGdi::TFillRule aFillRule)
	{
	_LIT(KDrawPolygon, "ShapeDrawing-DrawPolygon");

	TBuf<50> testName;
	testName = KDrawPolygon();
	if(aBrushStyle != DirectGdi::ENullBrush)
		{
		testName.Append(KSeparator);
		testName.Append(KBrushStyleTableNames[aBrushStyle]);
		testName.Append(KSeparator);
		testName.Append(KFillRuleNames[aFillRule]);
		}
	
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(testName);
		}
	
	ResetGc();

	TSize size(aWidth/10, aHeight/10);
	TSize size2(size.iWidth/2, size.iHeight/2);

	const TInt pCount = 28;
	CArrayFixFlat<TPoint> *tmpArray = new (ELeave)CArrayFixFlat<TPoint>(pCount);
	CleanupStack::PushL(tmpArray);

	TReal angle = 0.0;
	for(TInt r=pCount-1; r>0; r--,angle+=0.6)
		{
		TReal ss;
		TReal cc;
		Math::Sin(ss, angle);
		Math::Cos(cc, angle);
		TInt32 x;
		TInt32 y;
		Math::Int(x, cc*r);
		Math::Int(y, ss*r);

		tmpArray->AppendL(TPoint(x, y)+size2.AsPoint());
		//duplicate point to test consecutive points with the same position
		if(r==15)
			{
			tmpArray->AppendL(TPoint(x, y)+size2.AsPoint());
			}
		}

	CArrayFixFlat<TPoint> *array = new (ELeave)CArrayFixFlat<TPoint>(pCount);
	CleanupStack::PushL(array);

	iGc->SetBrushStyle(aBrushStyle);

	TInt step = 0;
	TBool ZeroPenSizeSet = EFalse;

	for(TInt y=-size2.iHeight,k=0; y<aHeight+size2.iHeight; y+=size.iHeight, k++)
		{
		for(TInt x=-size2.iWidth,l=0; x<aWidth+size2.iWidth; x+=size.iWidth, l++)
			{
			TPoint pos(x+k, y+l);

			TInt pNum = Abs(l-5) + Abs(k-5)*5 +1;
			if(pNum > pCount)
				pNum = pCount;

			for(TInt i=0; i<pNum; i++)
				{
				array->AppendL(tmpArray->At(i)+pos);
				}

			iGc->SetBrushColor(KColor16Table[step%16]);
			step++;
			
			if(ZeroPenSizeSet)
				{
				iGc->SetPenSize(TSize(1,1));
				}
			else
				{
				iGc->SetPenSize(TSize(0,0));
				ZeroPenSizeSet = ETrue;
				}				

			iGc->DrawPolygon(*array, aFillRule);
			TESTNOERRORL(iGc->GetError());
			array->Delete(0, pNum);
			}
		}
	CleanupStack::PopAndDestroy(2, tmpArray);

	TESTNOERROR(WriteTargetOutput(iTestParams, testName));
	}

/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-SHAPEDRAWING-0010

@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Test Plot() functionality.

@SYMTestStatus
	Implemented

@SYMTestPriority
	Critical

@SYMTestActions
	Draw many plots using pen size 1.
	Draw various other plots of differing pen sizes and colours.

@SYMTestExpectedResults
	VValid bitmap should be created. This bitmap should be the same as the reference bitmap.
*/
void CTDrawShapes::TestPlot()
	{
	_LIT(KTestName, "ShapeDrawing-Plot");
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}

	ResetGc();

	const TInt width = iGdiTarget->SizeInPixels().iWidth;
	const TInt height = iGdiTarget->SizeInPixels().iHeight;
	
	// Plot many 1-pixel points.
	TPoint plotPoint(10,10);
	while (plotPoint.iY < height)
		{
		iGc->Plot(plotPoint);
		plotPoint.iX += 10;
		if (plotPoint.iX > width)
			{
			plotPoint.iX -= width;
			plotPoint.iY += 10;
			}
		}
	
	// Plot points of various sizes/colours.
	TInt count = 0;
	TInt size;
	plotPoint = TPoint(10,10);
	while (plotPoint.iY < height)
		{
		iGc->SetPenColor(KColor16Table[count%16]);
		size = (count*2) % 15;
		iGc->SetPenSize(TSize(size, size));
		iGc->Plot(plotPoint);
		plotPoint.iX += 20;
		plotPoint.iY += 10;
		if (plotPoint.iX > width)
			{
			plotPoint.iX -= width;
			}
		++count;
		}
	TESTNOERROR(iGc->GetError());
	
	TESTNOERROR(WriteTargetOutput(iTestParams, KTestName()));
	
	iGc->Reset();
	}

/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-SHAPEDRAWING-0011

@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Test drawing arc functionality.

@SYMTestStatus
	Implemented

@SYMTestPriority
	Critical

@SYMTestActions
	Set default pen and brush.
	Draw 4 arcs of an ellipse on the same ellipse, one in each quadrant.
	Draw lines from the centre of that ellipse to the points defining the beginning and end points of the arcs.

	Draw 3 arcs with an angle extent of >90 degrees which span more than one quadrant.
	Draw lines from the centre of that ellipse to the points defining the beginning and end points of the arcs.

@SYMTestExpectedResults
	Valid bitmap should be created. This bitmap shall be compared to a reference bitmap.
*/
void CTDrawShapes::TestDrawLargeArc()
	{
	_LIT(KTestName, "ShapeDrawing-DrawArc-LargeArcs"); 

	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	ResetGc();

	// Draw arcs with short extents using same base ellipse.
	iGc->DrawArc(TRect(100,63,150,193), TPoint(110,4), TPoint(71,43));
	iGc->DrawLine(TPoint(125,128), TPoint(71,43));
	iGc->DrawLine(TPoint(125,128), TPoint(110,4));
	
	iGc->DrawArc(TRect(100,63,150,193), TPoint(71,213), TPoint(110,252));
	iGc->DrawLine(TPoint(125,128), TPoint(71,213));
	iGc->DrawLine(TPoint(125,128), TPoint(110,252));
	
	iGc->DrawArc(TRect(100,63,150,193), TPoint(140,252), TPoint(179,213));
	iGc->DrawLine(TPoint(125,128), TPoint(140,252));
	iGc->DrawLine(TPoint(125,128), TPoint(179,213));
	
	iGc->DrawArc(TRect(100,63,150,193), TPoint(179,43), TPoint(140,4));
	iGc->DrawLine(TPoint(125,128), TPoint(179,43));
	iGc->DrawLine(TPoint(125,128), TPoint(140,4));
	
	
	// Draw arc with large extent starting and finishing in same quadrant.
	iGc->DrawArc(TRect(300,63,350,193), TPoint(271,43), TPoint(310,4));
	iGc->DrawLine(TPoint(325,128), TPoint(271,43));
	iGc->DrawLine(TPoint(325,128), TPoint(310,4));
	
	// Draw arc with large extent starting and finishing in neighbouring quadrants.
	iGc->DrawArc(TRect(100,319,150,449), TPoint(71,299), TPoint(179,299));
	iGc->DrawLine(TPoint(125,381), TPoint(71,299));
	iGc->DrawLine(TPoint(125,381), TPoint(179,299));
		
	// Draw arc with large extent starting and finishing in opposite quadrants.
	iGc->DrawArc(TRect(300,319,350,449), TPoint(271,299), TPoint(340,508));
	iGc->DrawLine(TPoint(325,381), TPoint(271,299));
	iGc->DrawLine(TPoint(325,381), TPoint(340,508));
	
	TESTNOERROR(WriteTargetOutput(iTestParams, KTestName()));
	}

/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-SHAPEDRAWING-0012

@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Tests polygon drawing behaviour when called with invalid parameters.

@SYMTestStatus
	Implemented

@SYMTestPriority
	Critical

@SYMTestActions
	Draw several polygons where some of the points are repeated.

@SYMTestExpectedResults
	No errors or panics should be reported.
*/
void CTDrawShapes::TestSetAttributesInvalidParametersL()
	{
	_LIT(KTestName, "ShapeDrawing-SetAttributes-InvalidParameters");
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	ResetGc();
	
	CArrayFixFlat<TPoint> *array = new (ELeave)CArrayFixFlat<TPoint>(20);
	CleanupStack::PushL(array);
	
	// points with the same positions
	array->Reset();
	array->AppendL(TPoint(10, 10));
	array->AppendL(TPoint(20, 53));
	array->AppendL(TPoint(20, 53));
	array->AppendL(TPoint(20, 53));
	array->AppendL(TPoint(42, 27));
	array->AppendL(TPoint(42, 27));

	iGc->Reset();
	iGc->DrawPolygon(*array, DirectGdi::EAlternate);
	TESTNOERRORL(iGc->GetError());
	iGc->SetOrigin(TPoint(100, 100));
	iGc->SetBrushStyle(DirectGdi::ESolidBrush);
	iGc->SetBrushColor(KColor16Table[5]);
	iGc->DrawPolygon(*array, DirectGdi::EAlternate);
	TESTNOERRORL(iGc->GetError());
	iGc->SetOrigin(TPoint(200, 200));
	iGc->DrawPolygon(*array, DirectGdi::EWinding);
	TESTNOERRORL(iGc->GetError());
	
	CleanupStack::PopAndDestroy(array);

	TESTNOERROR(WriteTargetOutput(iTestParams, KTestName()));
	}

/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-SHAPEDRAWING-0013

@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Draw some shapes with semi-transparent pen outlines.

@SYMTestStatus
	Implemented

@SYMTestPriority
	Critical

@SYMTestActions
	Set the pen to a 10% opacity red colour.
	Set the brush fill colour to solid green.
	Call DrawRect() and DrawPoylgon() with varying pen-sizes.
	(Ellipses and arcs have been deliberately left out because the algorithm used for BitGdi
	involves multiple plots at the same point resulting in multiple blends and incorrect colour,
	meaning the test would always fail.)

@SYMTestExpectedResults
	The outline colour should match for all shapes, and be a consistant colour
	for the entire outline of the shape.
	The brush fill should reach the centre of the pen outlines, and be visible through the
	semi-transparent outline.
*/
void CTDrawShapes::TestDrawShapeTransparentOutlineL()
	{
	_LIT(KTestName, "ShapeDrawing-TransparentOutline");
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	ResetGc();
	
	const TInt width = iGdiTarget->SizeInPixels().iWidth;
	const TInt height = iGdiTarget->SizeInPixels().iHeight;
	
	iGc->SetPenColor(TRgb(255,0,0,25));
	iGc->SetBrushColor(TRgb(0,255,0,255));
	iGc->SetBrushStyle(DirectGdi::ESolidBrush);
	
	const TInt KCount = 3;
	TInt left = 10;
	for (TInt count = 0; count < KCount; ++count)
		{
		// The sw version allocates memory when you set a pen size and this memory is not freed
		// until after the __UHEAP_MARKEND macro when running OOM tests, this causes a memory 
		// leak when this test leaves. The following TCleanupItem has been added to reset the pen
		// size to 1,1 on account of a leave as settting the pen size to 1,1 deletes the allocated
		// pen memory.
		CleanupStack::PushL(TCleanupItem(ResetPenSize, iGc));

		switch(count)
			{
			case 0:
				iGc->SetPenSize(TSize(1,1));
				break;
			case 1:
				iGc->SetPenSize(TSize(15,7));
				break;
			case 2:
				iGc->SetPenSize(TSize(width/(KCount*2),width/(KCount*2)));
				break;
			}
		TInt right = left + (width/(KCount+2));
		iGc->SetPenStyle(DirectGdi::ESolidPen);
		iGc->DrawRect(TRect(left, height/10, right, height/5));
		CArrayFixFlat<TPoint> *array = new (ELeave)CArrayFixFlat<TPoint>(20);
		CleanupStack::PushL(array);
		array->AppendL(TPoint(left, height*3/5));
		array->AppendL(TPoint(left, height*9/10));
		array->AppendL(TPoint(right, height*9/10));
		iGc->DrawPolygon(*array, DirectGdi::EWinding);
		CleanupStack::PopAndDestroy(2);
		left = right + 50;
		}

	TESTNOERRORL(iGc->GetError());
	TESTNOERROR(WriteTargetOutput(iTestParams, KTestName()));
	}

/**
Override of base class virtual
@leave Gets system wide error code
@return - TVerdict code
*/
TVerdict CTDrawShapes::doTestStepPreambleL()
	{			
	CTDirectGdiStepBase::doTestStepPreambleL();	
	return TestStepResult();
	}
	
/** 
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.
@leave Gets system wide error code
@return TVerdict code
*/	
TVerdict CTDrawShapes::doTestStepL()
	{		
	// Test for each pixel format
	for(TInt targetPixelFormatIndex = iTargetPixelFormatArray.Count() - 1; targetPixelFormatIndex >= 0 ; targetPixelFormatIndex--)
		{
		iTestParams.iTargetPixelFormat = iTargetPixelFormatArray[targetPixelFormatIndex];
		SetTargetL(iTestParams.iTargetPixelFormat, EOneContextOneTarget, TSize(512, 512));
		RunTestsL();
		// only run OOM tests for one target pixel format to prevent duplication of tests
		if (targetPixelFormatIndex == 0)
			{
			RunOomTestsL();  //from base class
			}
		}
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

/**
Override of base class pure virtual
Lists the tests to be run
*/
void CTDrawShapes::RunTestsL()
	{
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0001"));
	TestBasicDrawShapeL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0010"));
	TestPlot();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0013"));
	TestDrawShapeTransparentOutlineL();
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0008"));
	TestDrawPolyLineL(EDrawPolyLine);
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0008"));
	TestDrawPolyLineL(EDrawPolyLineNoEndPoint);
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0009"));
	TestDrawPolygonL(iGdiTarget->SizeInPixels().iWidth, iGdiTarget->SizeInPixels().iHeight);
	RecordTestResultL();
	if(!iRunningOomTests)
		{
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0009"));
		TestDrawPolygonL(iGdiTarget->SizeInPixels().iWidth, iGdiTarget->SizeInPixels().iHeight, DirectGdi::ESolidBrush, DirectGdi::EAlternate);
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0009"));
		TestDrawPolygonL(iGdiTarget->SizeInPixels().iWidth, iGdiTarget->SizeInPixels().iHeight, DirectGdi::ESolidBrush, DirectGdi::EWinding);
		RecordTestResultL();
		}

	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0002"));
	TestDrawLineL(EDrawLine);
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0002"));
	TestDrawLineL(EDrawLineTo);
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0002"));
	TestDrawLineL(EDrawLineBy);
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0005"));
	TestDrawRoundRectL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0005"));
	TestDrawRoundRectL(DirectGdi::ESolidBrush);
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0006"));
	TestDrawArcL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0011"));
	TestDrawLargeArc();
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0007"));
	TestDrawPieL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0007"));
	TestDrawPieL(DirectGdi::ESolidBrush);
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0003"));
	TestDrawShapePositionAndSizeL(EDrawRect, DirectGdi::ESolidPen, DirectGdi::ENullBrush);
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0003"));
	TestDrawShapePositionAndSizeL(EDrawRoundRect, DirectGdi::ESolidPen, DirectGdi::ENullBrush);
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0003"));
	TestDrawShapePositionAndSizeL(EDrawEllipse, DirectGdi::ESolidPen, DirectGdi::ENullBrush);
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0003"));
	TestDrawShapePositionAndSizeL(EDrawPie, DirectGdi::ESolidPen, DirectGdi::ENullBrush);
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0003"));
	TestDrawShapePositionAndSizeL(EDrawArc, DirectGdi::ESolidPen, DirectGdi::ENullBrush);
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0003"));
	TestDrawShapePositionAndSizeL(EDrawRect, DirectGdi::ESolidPen, DirectGdi::ESolidBrush);
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0003"));
	TestDrawShapePositionAndSizeL(EDrawRoundRect, DirectGdi::ESolidPen, DirectGdi::ESolidBrush);
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0003"));
	TestDrawShapePositionAndSizeL(EDrawEllipse, DirectGdi::ESolidPen, DirectGdi::ESolidBrush);
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0003"));
	TestDrawShapePositionAndSizeL(EDrawPie, DirectGdi::ESolidPen, DirectGdi::ESolidBrush);
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0003"));
	TestDrawShapePositionAndSizeL(EDrawRect, DirectGdi::ENullPen, DirectGdi::ESolidBrush);
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0003"));
	TestDrawShapePositionAndSizeL(EDrawRoundRect, DirectGdi::ENullPen, DirectGdi::ESolidBrush);
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0003"));
	TestDrawShapePositionAndSizeL(EDrawEllipse, DirectGdi::ENullPen, DirectGdi::ESolidBrush);
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0003"));
	TestDrawShapePositionAndSizeL(EDrawPie, DirectGdi::ENullPen, DirectGdi::ESolidBrush);
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0004"));
	TestDrawShapeInvalidParametersL(EDrawRect);
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0004"));
	TestDrawShapeInvalidParametersL(EDrawRoundRect);
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0004"));
	TestDrawShapeInvalidParametersL(EDrawEllipse);
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0004"));
	TestDrawShapeInvalidParametersL(EDrawPie);
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0004"));
	TestDrawShapeInvalidParametersL(EDrawArc);
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0004"));
	TestDrawShapeInvalidParametersL(EDrawPolygon);
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-DIRECTGDI-SHAPEDRAWING-0012"));
	TestSetAttributesInvalidParametersL();
	RecordTestResultL();
	}
