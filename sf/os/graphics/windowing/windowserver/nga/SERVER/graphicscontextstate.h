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
// TInternalGcStatus and associated classes definitions
// Helper class which reads/writes to the given buffer graphics context attributes 
// 
//

#ifndef __GRAPHICSCONTEXTSTATE_H__
#define __GRAPHICSCONTEXTSTATE_H__

#include <gdi.h>
#include <s32strm.h>
#include <graphics/wsgraphicscontext.h>

class CWsClientWindow;

class TInternalGcStatus
	{
public:	
	void ResetInternalStatus(CWsClientWindow* aWin);
	static void InternalizeGcAttributesL(MWsGraphicsContext* aGc, RReadStream& aReadStream);
	void ExternalizeGcAttributesL(RWriteStream& aWriteStream);
public:	
	CGraphicsContext::TDrawMode iDrawMode;
	CGraphicsContext::TBrushStyle iBrushStyle;
	CGraphicsContext::TPenStyle iPenStyle;
	TRgb iPenColor;
	TRgb iBrushColor;
	TSize iPenSize;
	TInt iBrushPatternHandle;
	TInt iWordExcessWidth;
	TInt iWordNumChars;
	TInt iCharExcessWidth;
	TInt iCharNumChars;
	TRgb iShadowColor;
	TPoint iOrigin;
	TPoint iBrushOrigin;
	TFontUnderline iUnderline;
	TFontStrikethrough iStrikethrough;
	TInt iFontHandle;
	};

#endif
