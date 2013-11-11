// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SCREENINFO_H__
#define __SCREENINFO_H__

#include <e32def.h>         //TAny declaration

//Forward declarations
class TSize;

//TScreenInfo structure contains an information about video memory address and 
//screen size of a particular screen. It is used by TScreenDeviceFactory::CreateInstanceL
//method.
struct TScreenInfo
	{
	TScreenInfo(TAny* aScreenAddress, const TSize& aScreenSize) :
		iAddress(aScreenAddress),
		iSize(aScreenSize)
		{
		}
	TAny* iAddress;// The linear address of the screen
	TSize iSize;   // The size of the screen
	};

#endif//__SCREENINFO_H__
