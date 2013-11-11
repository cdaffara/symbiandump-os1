// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Error handler for CATIO
// 
//

/**
 @file
 @internalAll 
*/

#ifndef __ATERROR_H__
#define __ATERROR_H__

#include "ATBASE.H"

class CNotifications;
class CATWaitForCall;
class CATErrorHandler : public CBase
/**
@internalTechnology
*/
	{
public:
	static CATErrorHandler* NewL(CPhoneGlobals* aPhoneGlobals,CATWaitForCall* aWaitForCall);
	~CATErrorHandler();
	void CleanUp(TInt aError);
protected:
	CATErrorHandler(CPhoneGlobals* aPhoneGlobals,CATWaitForCall* aWaitForCall);
private:
	CATWaitForCall* iWaitForCall;
	CPhoneGlobals* iPhoneGlobals;
	};

#endif
