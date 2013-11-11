// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include "Utility.h"
#include <lbsipc.h>
#include <lbserrors.h>

void Utility::PanicClient(TInt aPanic, const RMessage2& aMessage)
	{
	_LIT(KTxtServer, "Pos-Server");
	aMessage.Panic(KTxtServer, aPanic);
	}

TInt Utility::Write(const RMessage2& aMessage, TInt aArg, const TDesC8& aDes, TInt aOffset)
//
// Write and kill the client if it leaves.
//
	{
	TRAPD(ret, aMessage.WriteL(aArg, aDes, aOffset))
	if (ret != KErrNone)
		PanicClient(EPositionBadDescriptor, aMessage);
	return ret;
	}

TInt Utility::Read(const RMessage2& aMessage, TInt aArg, TDes8& aDes, TInt aOffset)
//
// Write and kill the client if it leaves.
//
	{
	TRAPD(ret, aMessage.ReadL(aArg, aDes, aOffset))
	if (ret != KErrNone)
		PanicClient(EPositionBadDescriptor, aMessage);
	return ret;
	}

TInt Utility::Write(const RMessage2& aMessage, TInt aArg, const TDesC16& aDes, TInt aOffset)
//
// Write and kill the client if it leaves.
//
	{
	TRAPD(ret, aMessage.WriteL(aArg, aDes, aOffset))
	if (ret != KErrNone)
		PanicClient(EPositionBadDescriptor, aMessage);
	return ret;
	}

TInt Utility::Read(const RMessage2& aMessage, TInt aArg, TDes16& aDes, TInt aOffset)
//
// Write and kill the client if it leaves.
//
	{
	TRAPD(ret, aMessage.ReadL(aArg, aDes, aOffset))
	if (ret != KErrNone)
		PanicClient(EPositionBadDescriptor, aMessage);
	return ret;
	}
