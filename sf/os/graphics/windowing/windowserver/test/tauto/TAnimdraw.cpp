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

#include "TAnimDraw.h"

GLREF_C void DoDraw(TInt aDraw,CBitmapContext& aGc,TRgb aColor,TRect aRect,CFbsFont *aFont,TInt aTextVal)
	{
	switch (aDraw)
		{
		case 36:
			{
			aGc.Clear();
			aGc.SetPenSize(TSize(15,15));
			aGc.SetPenColor(aColor);
			aGc.DrawLine(TPoint(aRect.iTl),TPoint(aRect.iBr));
			break;
			}
		case 37:
			{
			TBuf<3> buf;
			buf.Format(_L("%d"), aTextVal);
			aGc.Clear();
			aGc.SetDrawMode(CGraphicsContext::EDrawModePEN);
			aGc.SetPenStyle(CGraphicsContext::ESolidPen);
			aGc.SetPenSize(TSize(1,1));
			aGc.SetPenColor(aColor);
			aGc.UseFont(aFont);
			TInt drawHeight=aFont->AscentInPixels();
			aRect.iTl.iY=aRect.iBr.iY/3;
			aGc.DrawText(buf,aRect,drawHeight,CGraphicsContext::ECenter);
			break;
			}
	default:;
		}
	}
