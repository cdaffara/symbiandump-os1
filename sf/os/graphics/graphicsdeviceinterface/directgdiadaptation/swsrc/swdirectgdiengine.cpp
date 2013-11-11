// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "swdirectgdiengine.h"
#include <bitdraworigin.h>
#include <bitdrawinterfaceid.h>

/**
Sets a new pen array

@param aPenArray The new array.
*/ 
void CSwDirectGdiEngine::SetPenArray(TInt* aPenArray)
	{
	delete[] iPenArray;
	iPenArray = aPenArray;
	}

/**
Destroys the current pen array.
*/ 
void CSwDirectGdiEngine::ResetPenArray()
	{
	delete[] iPenArray;
	iPenArray = NULL;
	}

/**
Truncates the given rectangle.

@param aRect The rectangle to truncate.
 */ 
void CSwDirectGdiEngine::TruncateRect(TRect& aRect)
	{
	TInt width = iDrawDevice->SizeInPixels().iWidth << 4;
	TInt height = iDrawDevice->SizeInPixels().iHeight << 4;

	aRect.iTl.iX = Min(aRect.iTl.iX,width);
	aRect.iTl.iY = Min(aRect.iTl.iY,height);
	aRect.iBr.iX = Min(aRect.iBr.iX,width);
	aRect.iBr.iY = Min(aRect.iBr.iY,height);

	width = (-width);
	height = (-height);

	aRect.iTl.iX = Max(aRect.iTl.iX,width);
	aRect.iTl.iY = Max(aRect.iTl.iY,height);
	aRect.iBr.iX = Max(aRect.iBr.iX,width);
	aRect.iBr.iY = Max(aRect.iBr.iY,height);
	}

/**
@see MDirectGdiEngine::GetInterface()
*/
TInt CSwDirectGdiEngine::GetInterface(TUid aInterfaceId, TAny*& aInterface)
	{
	aInterface = NULL;
	TInt err = KErrNotSupported;
	
	// Extension switch
	switch (aInterfaceId.iUid)
		{
		case KDrawDeviceOriginInterfaceID:
			{
			aInterface= (MDrawDeviceOrigin*)this;
			err=KErrNone;
			}
		break;
		default:;
		}
		
	return err;
	}

