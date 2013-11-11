/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*    Implementation for the CDosService class
*
*/


#include <e32svr.h>
#include "dosservice.h"
#include "dosserver.h"

//
// ---------------------------------------------------------
// CDosService::Write
// ---------------------------------------------------------
//  
void CDosService::Write(const RMessage2& aMessage, const TInt& aIndex, const TDesC8& aDes) const
//
// Write and kill the client if it leaves.
//
{
	TInt ret(0);

	TRAP(ret, aMessage.WriteL(aIndex, aDes));
	if (ret!=KErrNone)
	{
		PanicClient(aMessage,EPanicBadDescriptor);
	}
}


