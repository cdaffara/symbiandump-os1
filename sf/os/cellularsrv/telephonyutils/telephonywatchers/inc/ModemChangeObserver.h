// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __MODEMCHANGEOBSERVER_H__
#define __MODEMCHANGEOBSERVER_H__

// System includes
#include <e32base.h>
#include <e32property.h>


//
// ------> MModemChangeObserver (header)
//
class MModemChangeObserver
/**
@internalComponent
*/
	{
//
public: // FROM MModemChangeObserver
//
	virtual void HandleModemChangedL() = 0;
	};



//
// ------> CModemChangeObserver (header)
//
class CModemChangeObserver : public CActive
/**
@internalComponent
*/
{
//
public: // CONSTRUCT / DESTRUCT
//
	IMPORT_C CModemChangeObserver(MModemChangeObserver& aObserver);
	IMPORT_C ~CModemChangeObserver();
	IMPORT_C void ConstructL();

//
public: // ISSUE REQUEST
//
	void IssueRequestL();

//
private: // FROM CActive
//
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

//
private: // MEMBER DATA
//
	RProperty iModemChangeProperty;
	MModemChangeObserver& iObserver;
	};


#endif
