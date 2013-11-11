/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <e32def.h>

class RMessage2;
class TDes8;
class TDesC8;
class TDes16;
class TDesC16;

namespace Utility
	{
	// Panic the client
	void PanicClient(TInt aPanic, const RMessage2& aMessage);
	TInt Write(const RMessage2& aMessage, TInt aArg, const TDesC8& aDes, TInt aOffset=0);
	TInt Read(const RMessage2& aMessage, TInt aArg, TDes8& aDes, TInt aOffset=0);
	TInt Write(const RMessage2& aMessage, TInt aArg, const TDesC16& aDes, TInt aOffset=0);
	TInt Read(const RMessage2& aMessage, TInt aArg, TDes16& aDes, TInt aOffset=0);
	}

#endif // __UTILITY_H__
