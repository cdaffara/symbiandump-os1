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

#include "drawresource.h"
#include "wstop.h"
#include <graphics/sgimage.h>


CWsDrawableSource::CWsDrawableSource(CWsClient *aOwner)
	: CWsObject(aOwner, WS_HANDLE_DRAWABLE_SOURCE)
	{
	}

CWsDrawableSource::~CWsDrawableSource()
	{
	if (iDrawableSource && iDrawResource)
		{
		iDrawResource->CloseDrawableSource(iDrawableSource);
		}
	}

void CWsDrawableSource::ConstructL(const TWsClCmdCreateDrawableSource &aParams)
	{
	NewObjL();

	CScreen* screen = NULL;
	if (Rng(0, aParams.iScreenNumber, CWsTop::NumberOfScreens() - 1))
		{
		screen = CWsTop::Screen(aParams.iScreenNumber);
		}
	else
		{
		User::Leave(KErrArgument);
		}
	MWsScreen* pOI=screen;
	CWsDrawableSource::iDrawResource = pOI->ObjectInterface<MWsDrawableSourceProvider>();
	if (!iDrawResource)
		{
		User::Leave(KErrNotSupported);
		}
	User::LeaveIfError(iDrawResource->CreateDrawableSource(aParams.iDrawableId, iDrawableSource));
	IncRefCount();
	}

void CWsDrawableSource::CommandL(TInt aOpcode, const TAny *)
	{
	switch(aOpcode)
		{
		case EWsDrawableSourceOpFree:
			iFlags |= EWsClientHandleClosed;
			DecRefCount();
			break;
		default:
			OwnerPanic(EWservPanicOpcode);
			break;
		}
	}

/*
CWsDrawableSource is a reference counted object so we cannot simply close the object as somebody else may have a reference to it
*/
void CWsDrawableSource::CloseObject()
	{
	RemoveFromIndex();
	if (!(iFlags & EWsClientHandleClosed))
		{
		DecRefCount();
		}
	}

void CWsDrawableSource::IncRefCount()
	{
	iRefCount++;
	}

void CWsDrawableSource::DecRefCount()
	{
	if (--iRefCount <= 0)
		{
		delete this;
		}
	}

void CWsDrawableSource::DrawResource(MWsGraphicsContext *aGc, const TPoint& aPos, CWindowGc::TGraphicsRotation aRotation) const
	{
	ASSERT(iDrawResource);
	MWsDrawableSourceProvider* drawResource = aGc->ObjectInterface<MWsDrawableSourceProvider>();
	if (drawResource == iDrawResource)
		{
		drawResource->DrawResource(iDrawableSource, aPos, aRotation);
		}
	}

void CWsDrawableSource::DrawResource(MWsGraphicsContext *aGc, const TRect& aRect, CWindowGc::TGraphicsRotation aRotation) const
	{
	ASSERT(iDrawResource);
	MWsDrawableSourceProvider* drawResource = aGc->ObjectInterface<MWsDrawableSourceProvider>();
	if (drawResource == iDrawResource)
		{
		drawResource->DrawResource(iDrawableSource, aRect, aRotation);
		}
	}

void CWsDrawableSource::DrawResource(MWsGraphicsContext *aGc, const TRect& aRectDest, const TRect& aRectSrc, CWindowGc::TGraphicsRotation aRotation) const
	{
	ASSERT(iDrawResource);
	MWsDrawableSourceProvider* drawResource = aGc->ObjectInterface<MWsDrawableSourceProvider>();
	if (drawResource == iDrawResource)
		{
		drawResource->DrawResource(iDrawableSource, aRectDest, aRectSrc, aRotation);
		}
	}

void CWsDrawableSource::DrawResource(MWsGraphicsContext *aGc, const TRect& aRect, const TDesC8& aDes) const
	{
	ASSERT(iDrawResource);
	MWsDrawableSourceProvider* drawResource = aGc->ObjectInterface<MWsDrawableSourceProvider>();
	if (drawResource == iDrawResource)
		{
		drawResource->DrawResource(iDrawableSource, aRect, aDes);
		}
	}
