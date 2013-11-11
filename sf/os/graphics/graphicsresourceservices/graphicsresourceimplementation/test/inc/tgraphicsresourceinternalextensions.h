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
//
/**
@file
@test
@internalTechnology
 */
#ifndef TGRAPHICSRESOURCEINTERNALEXTENSIONS_H_
#define TGRAPHICSRESOURCEINTERNALEXTENSIONS_H_

#include <e32cmn.h>

/**
Copy of the test extension from the reference Graphics Resource implementation.
This has been copied to remove the dependency of the tests upon the reference
implementation.
 */
class MSgDriver_Test
	{
public:
	enum { EInterfaceUid = 0x102872C8 };
public:
	virtual void AllocMarkStart() = 0;
	virtual void AllocMarkEnd(TInt aCount) = 0;
	virtual void SetAllocFail(RAllocator::TAllocFail aFail, TInt aRate) = 0;
	};

/**
Copy of the profiling extension from the reference Graphics Resource implementation.
This has been copied to remove the dependency of the tests upon the reference
implementation.
 */
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

#endif /* TGRAPHICSRESOURCEINTERNALEXTENSIONS_H_ */
