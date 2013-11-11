// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CBASEMENUASYNC_H__
#define __CBASEMENUASYNC_H__

#include <e32base.h>
#include <etel3rdparty.h>
#include <e32cons.h>
#include "CISVAPIAsync.h"

_LIT(KNewLine, "\n");
_LIT(KError, "iStatus != KErrNone, Something's Wrong!\n");

/**
Base Class providing functionality for and control of Etel 3rd Party Instances.
Also controls user input and handles displaying information to the user.
*/
class CBaseMenuAsync : public CActive, public MExecAsync
	{

// Methods
public:
	void Start();
	void CompleteOwnRequest(TInt aErr);
	void PostOwnRequest();
	virtual void Terminate();

protected:
	CBaseMenuAsync(CConsoleBase& aConsole);

	void GetInput();
	inline CTelephony* GetTelObj() const;
	inline CConsoleBase* GConsole() const;
	virtual void DoCancel() = 0;
	void ConstructL();
// Data
protected:
	/**
	The current operating state of the active object.
	*/
	TState iState;
	/**
	CTelephony Instance used by all member active objects
	to make calls to ETel ISV.
	*/
	CTelephony* iTelephony;
	/**
	Active object to perform next operations on.
	*/
	CISVAPIBase* iLastOperation;
	/**
	Console instance.
	*/
    CConsoleBase* iConsole;
	};

/**
Returns a pointer to object pointed to by iTelephony.

@return Pointer to CTelephony object used to make ETel ISV calls
*/
inline CTelephony* CBaseMenuAsync::GetTelObj() const
	{
	return iTelephony;
	}

/**
Returns a pointer to object pointed to by iConsole.

@return Pointer to CConsoleBase object used to display output
*/
inline CConsoleBase* CBaseMenuAsync::GConsole() const
	{
	return iConsole;
	}

#endif // __CBASEMENUASYNC_H__

