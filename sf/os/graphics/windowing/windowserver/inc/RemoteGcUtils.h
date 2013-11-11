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

#ifndef __REMOTEGCUTILS_H__
#define __REMOTEGCUTILS_H__

/**
All possible drawcommands that can be recorded and played by the RemoteGc.
@publishedPartner
@released*/
enum TDrawCode
	{
	ECommandClear,
	ECommandClearRect,
	ECommandCopyRect,
	ECommandBitBlt1,
	ECommandBitBlt2,	
	ECommandBitBltMasked,
	ECommandSetFaded,
	ECommandSetFadingParameters,
	ECommandAlphaBlendBitmaps,
	ECommandSetOrigin,
	ECommandSetDrawMode,
	ECommandSetClippingRect,
	ECommandCancelClippingRect,
	ECommandReset,
	ECommandUseFont,
	ECommandDiscardFont,
	ECommandSetUnderlineStyle,
	ECommandSetStrikethroughStyle,
	ECommandSetWordJustification,
	ECommandSetCharJustification,	
	ECommandSetPenColor,
	ECommandSetPenStyle,
	ECommandSetPenSize,
	ECommandSetBrushColor,
	ECommandSetBrushStyle,
	ECommandSetBrushOrigin,
	ECommandUseBrushPattern,
	ECommandDiscardBrushPattern,
	ECommandMoveTo,
	ECommandMoveBy,
	ECommandPlot,
	ECommandDrawArc,
	ECommandDrawLine,
	ECommandDrawLineTo,
	ECommandDrawLineBy,
	ECommandDrawPolyLine,
	ECommandDrawPie,
	ECommandDrawEllipse,
	ECommandDrawRect,
	ECommandDrawRoundRect,
	ECommandDrawPolygon,
	ECommandDrawBitmap1,
	ECommandDrawBitmap2,
	ECommandDrawBitmap3,
	ECommandDrawBitmapMasked,
	ECommandDrawText1,
	ECommandDrawText2,
	ECommandDrawText3,
	ECommandMapColors,
	ECommandSetClippingRegion,
	ECommandCancelClippingRegion,
	ECommandDrawTextVertical1,
	ECommandDrawTextVertical2,
	ECommandDrawWsGraphic1,
	ECommandDrawWsGraphic2,
	ECommandSetShadowColor,
	// New DrawText API's that take in context
	ECommandDrawText4,
	ECommandDrawText5,
	ECommandDrawTextVertical3,
	ECommandDrawTextVertical4,
	
	ECommandGetUnderlineMetrics,
	
	ECommandDrawResourceToPos,
	ECommandDrawResourceToRect,
	ECommandDrawResourceFromRectToRect,
	ECommandDrawResourceWithData,
	};

#endif // __REMOTEGCUTILS_H__
