// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SSMDEFERREDDELETER_H__
#define __SSMDEFERREDDELETER_H__

#include <e32base.h>

class CSsmCommandListImpl;

/**
This active object is used to delete the command list after the execution of 
all the deferred commands present in the command list.

@internalComponent
@released
*/
NONSHARABLE_CLASS(CSsmDeferredDeleter) : public CActive
	{
public:
	static CSsmDeferredDeleter* NewL();
	~CSsmDeferredDeleter();

	void DeferredDelete(CSsmCommandListImpl* aCommandList);

protected:
	// from CActive
	void RunL();
	void DoCancel();

	CSsmDeferredDeleter();

#ifdef _DEBUG
	void ConstructL();
	static TInt TimeoutCallback(TAny* aSelf);
	// Print warnings in every minute if Deferred deleter is waiting for pending commands
	void PrintWarning();
#endif

private:
	CSsmCommandListImpl* iCommandList;

#ifdef _DEBUG
	CPeriodic* iTimer;
#endif
	};

#endif // __SSMDEFERREDDELETER_H__
