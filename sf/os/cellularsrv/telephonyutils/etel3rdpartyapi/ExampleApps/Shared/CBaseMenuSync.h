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


#ifndef __CBASEMENUSYNC_H__
#define __CBASEMENUSYNC_H__

#include "CISVAPISync.h"

_LIT(KNewLine, "\n");
_LIT(KError, "iStatus != KErrNone, something is wrong!\n");

/**
Base class providing functionality for and control of Etel 3rd Party Instances.
Also controls user input and handles displaying information to the user.
*/
class CBaseMenuSync : public CActive, public MExecSync
	{

// Methods
public:
	void Start();
	void CompleteOwnRequest(TInt aErr);
	void PostOwnRequest();
	virtual void ExecComplete();
	virtual void Terminate();
	~CBaseMenuSync();

protected:
	CBaseMenuSync(CConsoleBase& aConsole);
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
	CTelephony instance used by all member active objects to make calls to ETel
	ISV.
	*/
	CTelephony* iTelephony;
	/**
	Console instance.
	*/
	CConsoleBase* iConsole;

	};

/**
Returns a pointer to object pointed to by iTelephony.

@return Pointer to CTelephony object used to make ETel ISV calls
*/
inline CTelephony* CBaseMenuSync::GetTelObj() const
	{
	return iTelephony;
	}

/**
Returns a pointer to object pointed to by iConsole.

@return Pointer to CConsoleBase object used to display output
*/
inline CConsoleBase* CBaseMenuSync::GConsole() const
	{
	return iConsole;
	}


#endif // __CBASEMENUSYNC_H__

