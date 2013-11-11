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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "bufferdrawer.h"
#include <s32mem.h>
#include <s32strm.h>
#include <fbs.h>
#include <bitdev.h>
#include <bitstd.h>
#include <graphics/wsgraphicscontext.h>
#include <graphics/wsuibuffer.h>


// CWsGraphicDrawer	
CWsBufferGraphicDrawer* CWsBufferGraphicDrawer::NewL()
	{
	return new(ELeave) CWsBufferGraphicDrawer;	
	}
	
CWsBufferGraphicDrawer::~CWsBufferGraphicDrawer()
	{
	}

void CWsBufferGraphicDrawer::ConstructL(MWsGraphicDrawerEnvironment& aEnv, const TGraphicDrawerId& aId, MWsClient& aOwner, const TDesC8& /*aData*/)
	{
	BaseConstructL(aEnv, aId, aOwner);
 	// default white line number
	iWhiteLinePos = 0;
 	}

void CWsBufferGraphicDrawer::HandleMessage(const TDesC8& aData)
	{
	TInt linePos = aData[0];
	DoUpdateWhiteLinePos(linePos);
	}

void CWsBufferGraphicDrawer::DoUpdateWhiteLinePos(TInt aWhiteLinePos)
	{
	iWhiteLinePos = aWhiteLinePos;
	// Invalidate the redrawing
	Invalidate();
	}

void CWsBufferGraphicDrawer::DoDraw(MWsGc& aGc, const TRect& aRect, const TDesC8& /*aData*/) const
	{
	MWsGraphicsContext* context = static_cast<MWsGraphicsContext*>(aGc.ResolveObjectInterface(KMWsGraphicsContext));
	
	//Draw a filled rect with the chosen color
	context->Push();
	context->SetBrushStyle(MWsGraphicsContext::ESolidBrush);
	context->SetBrushColor(KRgbBlue);
	context->DrawRect(aRect);
	context->Pop();
	
	//Obtain access to the screen/OSB buffer
	MWsUiBuffer* buffer = static_cast<MWsUiBuffer*>(aGc.ResolveObjectInterface(KMWsUiBufferInterfaceId));
	TAny* data;
	TInt stride;
	
	TInt err = buffer->MapReadWrite(data, stride);
	
	if (err == KErrNone)
		{
		//Fill the chosen line with white
		TUint8* scanLine = static_cast<TUint8*>(data);
		scanLine += stride * iWhiteLinePos;
		Mem::Fill(scanLine, stride, 0xFF);
	
		buffer->Unmap();
		}
	}
