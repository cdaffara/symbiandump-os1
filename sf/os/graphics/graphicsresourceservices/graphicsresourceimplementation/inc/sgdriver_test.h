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
// Graphics Resource - internal extension interface for testing
//

#ifndef SGDRIVER_TEST_H
#define SGDRIVER_TEST_H

#include <e32def.h>

class MSgDriver_Test
	{
public:
	enum { EInterfaceUid = 0x102872C8 };
public:
	virtual void AllocMarkStart() = 0;
	virtual void AllocMarkEnd(TInt aCount) = 0;
	virtual void SetAllocFail(RAllocator::TAllocFail aType, TInt aRate) = 0;
	};

#endif // SGDRIVER_TEST_H
