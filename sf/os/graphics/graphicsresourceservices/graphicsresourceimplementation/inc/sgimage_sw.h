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
// Graphics Resource - internal extension interface for software implementations
//

#ifndef SGIMAGE_SW_H
#define SGIMAGE_SW_H

#include <e32def.h>

class MSgImage_Sw
	{
public:
	enum { EInterfaceUid = 0x102858F0 };
public:
	virtual TAny* DataAddress() const = 0;
	virtual TInt DataStride() const = 0;
	};

#endif // SGIMAGE_SW_H
