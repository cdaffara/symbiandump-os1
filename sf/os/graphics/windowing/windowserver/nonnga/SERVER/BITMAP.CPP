// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Window server side bitmap class
// All this class does is actually give the window server ownership of the bitmap
// 
//

#include "ScrDev.H"

DWsBitmap::DWsBitmap(CWsClient *aOwner) : CWsObject(aOwner, WS_HANDLE_BITMAP)
	{
	}

DWsBitmap::~DWsBitmap()
	{
	delete iFbsBitmap;
	}

void DWsBitmap::ConstructL(const TWsClCmdCreateBitmap &aParams)
	{
	NewObjL();
	iFbsBitmap=new(ELeave) CFbsBitmap();
	TInt ret = iFbsBitmap->Duplicate(aParams.handle);
	if(ret == KErrNoMemory)
		{
		User::Leave(ret);
		}
	if (ret !=KErrNone)
		OwnerPanic(EWservPanicBitmap);
	IncRefCount();
	}

void DWsBitmap::CommandL(TInt aOpcode, const TAny *)
	{
	switch(aOpcode)
		{
		case EWsBitmapOpFree:
			iFlags |= EWsBitmapClientObjectFreed;
			DecRefCount();
			break;
		default:
			OwnerPanic(EWservPanicOpcode);
			break;
		}
	}

/*
DWsBitmap is a reference couting object so we cannot simply close the object as somebody else may have a reference to it
*/
void DWsBitmap::CloseObject()
	{
	RemoveFromIndex();
	if (!(iFlags & EWsBitmapClientObjectFreed))
		{
		DecRefCount();
		}
	}

void DWsBitmap::IncRefCount()
	{
	iRefCount++;
	}

void DWsBitmap::DecRefCount()
	{
	if (--iRefCount <= 0)
		delete this;
	}
