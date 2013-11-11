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
// Graphics Resource - internal extension interface for profiling
//

#ifndef SGDRIVER_PROFILING_H
#define SGDRIVER_PROFILING_H

#include <e32def.h>

class MSgDriver_Profiling
	{
public:
	enum { EInterfaceUid = 0x102872C9 };
public:
	virtual TInt LocalResourceCount() const = 0;
	virtual TInt GlobalResourceCount() const = 0;
	virtual TInt LocalGraphicsMemoryUsed() const = 0;
	virtual TInt GlobalGraphicsMemoryUsed() const = 0;
	};

#endif // SGDRIVER_PROFILING_H
