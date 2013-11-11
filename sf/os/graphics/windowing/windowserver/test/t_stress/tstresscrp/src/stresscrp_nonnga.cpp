// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "stresscrp.h"
#undef SYMBIAN_GRAPHICS_GCE

#include <bitstd.h>
#include <graphics/wsgraphicdrawerinterface.h>

void StressCrpNonNgaContext::Draw(MWsGc& aGc, const TRect& aRect)
	{
	CFbsBitGc& bitgdi=aGc.BitGc();
	bitgdi.SetDrawMode(CFbsBitGc::EDrawModePEN);
	bitgdi.SetPenColor(TRgb(255,0,0));
	bitgdi.SetPenStyle(CFbsBitGc::ESolidPen);
	bitgdi.SetBrushStyle(CFbsBitGc::ESolidBrush);
	bitgdi.SetBrushColor(TRgb(255,0,0));
	bitgdi.DrawRect(aRect);
	bitgdi.SetBrushColor(TRgb(0,0,255));
	bitgdi.DrawEllipse(aRect);		
	}
