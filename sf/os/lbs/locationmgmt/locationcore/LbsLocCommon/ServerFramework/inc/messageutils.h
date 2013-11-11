// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Definition of utilty class used to write ans read RMessage2 and send panic message 
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef __LBSMESSAGEUTILS_H__
#define __LBSMESSAGEUTILS_H__

#include <e32base.h>

/** 
@internalTechnology
@released

Message object handling class, include message writing, reading and send panic message to client.
 */
class MessageUtils
	{
public:
	static TInt Write(const RMessage2& aMessage, const TInt aIndex, const TDesC16& aBuf);
	static TInt Write(const RMessage2& aMessage, const TInt aIndex, const TDesC8& aBuf);
	static TInt Read(const RMessage2& aMessage, const TInt aIndex, TDes16& aBuf);
	static TInt Read(const RMessage2& aMessage, const TInt aIdnex, TDes8& aBuf);
	static void PanicClient(const RMessage2&, const TDesC& aCat, const TInt aReason);
	};
	
#endif // __LBSMESSAGEUTILS_H__