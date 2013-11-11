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

#ifndef __APASTARTER_H__
#define __APASTARTER_H__

#include <e32base.h>

/**
This is an abstract interface to be loaded by systart before Ecom is up and running.

Startsafe expect apstart.dll to have a factory-function at its first ordinal position, 
which returns an implementation of this interface.

@internalTechnology
@released
*/
class CApaStarter : public CBase
	{
public:
	virtual void WaitForApparcToInitialiseL()=0;
	virtual void StartAppL(const TDesC& aFileName, const TDesC& aArgs, TBool aViewLess, TBool aStartInBackground, TThreadId& aThreadId)=0;
	virtual void StartAppL(const TDesC& aFileName, const TDesC& aArgs, TBool aViewLess, TBool aStartInBackground, TThreadId& aThreadId, TRequestStatus& aRequestStatusForRendezvous)=0;
	};

#endif   	
	


