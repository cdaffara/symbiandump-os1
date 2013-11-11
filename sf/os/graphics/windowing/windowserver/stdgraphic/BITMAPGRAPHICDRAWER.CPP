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

#include "stdgraphicdrawer.h"
#include "wsgraphicdrawercontext.h"
#include <s32mem.h>
#include <fbs.h>

CWsGraphicDrawerBitmap* CWsGraphicDrawerBitmap::CreateL()
	{
	return new(ELeave) CWsGraphicDrawerBitmap;
	}
	
void CWsGraphicDrawerBitmap::ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TGraphicDrawerId& aId,MWsClient& aOwner,const TDesC8& aData)
	{
	TInt bitmapHandle = 0;
	TInt maskHandle = 0;
	RDesReadStream in(aData);
	in.PushL();
	bitmapHandle = in.ReadInt32L();
	maskHandle = in.ReadInt32L();
	in.Pop();
	BaseConstructL(aEnv,aId,aOwner);
	if(bitmapHandle)
		{
		iBitmap = new(ELeave) CFbsBitmap;
		User::LeaveIfError(iBitmap->Duplicate(bitmapHandle));
		if(maskHandle)
			{
			iMask = new(ELeave) CFbsBitmap;
			User::LeaveIfError(iMask->Duplicate(maskHandle));
			}
		}
	if (!(aEnv.Screen(0)->ResolveObjectInterface(KMWsCompositionContext) || aEnv.Screen(0)->ResolveObjectInterface(KMWsScene)))
		{
		iContext = CWsGraphicDrawerNonNgaContext::NewL();
		}
	else
		{
		iContext = CWsGraphicDrawerNgaContext::NewL();
		}
	}
	
CWsGraphicDrawerBitmap::CWsGraphicDrawerBitmap()
	{
	}
	
CWsGraphicDrawerBitmap::~CWsGraphicDrawerBitmap()
	{
	if (iContext)
		{
		iContext->Destroy();
		iContext = NULL;
		}
	delete iBitmap;
	delete iMask;
	}

void CWsGraphicDrawerBitmap::DoDraw(MWsGc& aGc,const TRect& aRect,const TDesC8& /*aData*/) const
	{
	if (iBitmap)
		{
		if (KErrNone != iContext->Push(aGc))
			{
			return;
			}
		TRect bmpRect(aRect.iTl,iBitmap->SizeInPixels());
		bmpRect.Intersection(aRect);
		iContext->DrawBitmap(aGc, aRect.iTl, iBitmap, bmpRect.Size(), iMask);
		iContext->Pop(aGc);
		}
	}
	
void CWsGraphicDrawerBitmap::HandleMessage(const TDesC8& /*aData*/)
	{
	}
	
