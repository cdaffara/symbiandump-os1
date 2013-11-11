// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "colorblender.h"
#include <fbs.h>
#include <bitdev.h>
#include <bitstd.h>

CColorBlender* CColorBlender::NewLC(TDisplayMode aDisplayMode)
	{
	CColorBlender* self = new(ELeave) CColorBlender;
	CleanupStack::PushL(self);
	self->iBitmap = new(ELeave) CFbsBitmap;
	User::LeaveIfError(self->iBitmap->Create(TSize(1, 1), aDisplayMode));
	self->iDevice = CFbsBitmapDevice::NewL(self->iBitmap);
	self->iGc = CFbsBitGc::NewL();
	self->iGc->Activate(self->iDevice);
	return self;
	}

CColorBlender::~CColorBlender()
	{
	delete iBitmap;
	delete iDevice;
	delete iGc;
	}

void CColorBlender::SetInitialColor(TRgb aColor)
	{
	iGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
	iGc->SetBrushColor(aColor);
	iGc->Clear();
	iGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
	}

void CColorBlender::Blend(TRgb aColor)
	{
	iGc->SetBrushColor(aColor);
	iGc->Clear();
	}

TRgb CColorBlender::Color() const
	{
	TRgb color;
	iBitmap->GetPixel(color, TPoint(0, 0));
	return color;
	}

