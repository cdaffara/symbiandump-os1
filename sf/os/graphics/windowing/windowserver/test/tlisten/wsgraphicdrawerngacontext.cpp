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

#define SYMBIAN_GRAPHICS_GCE

#include "wsgraphicdrawercontext.h"
#include <graphics/wsgraphicscontext.h>

MWsGraphicDrawerContext* CWsGraphicDrawerNgaContext::NewL()
	{
	return new(ELeave) CWsGraphicDrawerNgaContext();
	}

void CWsGraphicDrawerNgaContext::Destroy()
	{
	delete this;
	}

void CWsGraphicDrawerNgaContext::DrawEllipse(MWsGc& aGc, const TRect& aRect, const TRgb& aColor) const
	{
	MWsGraphicsContext* context = aGc.ObjectInterface<MWsGraphicsContext>();
	if (context)
		{
		context->Push();
		context->SetBrushStyle(MWsGraphicsContext::ESolidBrush);
		context->SetBrushColor(aColor);
		context->DrawEllipse(aRect);
		context->Pop();
		}
	}
