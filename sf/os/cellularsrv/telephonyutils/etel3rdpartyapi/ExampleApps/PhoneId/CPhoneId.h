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
// CPhoneApp.h
// 
//

#ifndef __CPHONEID_H__
#define __CPHONEID_H__

#include <e32base.h>
#include <e32cons.h>
#include <e32def.h>
#include <etel3rdparty.h>


#include "CISVAPISync.h"

#include "CMainMenu.h"

/**
Retrieves information regarding the current phone and prints this to the 
console.
*/
class CPhoneId : public CISVAPISync
	{

// Methods
public:
	static CPhoneId* NewL(MExecSync* aController);
	~CPhoneId();

	void DoStartRequestL();

private:
	CPhoneId(MExecSync* aController);
	void ConstructL();

	void RunL();
	void DoCancel();
	TBool CheckAppRegistered();

  	// Data
public:
	/**
	Stores the mobile phone identity information which is checked by the
	running application.
	*/
	CTelephony::TPhoneIdV1 iPhoneIdV1;

private:
	/**
	Packages iPhoneIdV1.
	*/
	CTelephony::TPhoneIdV1Pckg iPhoneIdV1Pckg;

	};

#endif // __CPHONEID_H__
