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


#ifndef __CCURRENTNETWORKNAME_H__
#define __CCURRENTNETWORKNAME_H__

#include <e32base.h>
#include <e32cons.h>
#include <e32def.h>
#include <etel3rdparty.h>

#include "CISVAPIAsync.h"

#include "CMainMenu.h"

/**
Retrieves the network the SIM is registered to.
*/
class CCurrentNetworkName : public CISVAPIAsync
	{

// Methods
public:
	static CCurrentNetworkName* NewL(MExecAsync* aController);
	~CCurrentNetworkName();

	void DoStartRequestL();

private:
	CCurrentNetworkName(MExecAsync* aController);
	void ConstructL();

	void RunL();
	void DoCancel();

// Data
public:
	/**
	Stores the retrieved current network name information.
	*/
	CTelephony::TNetworkNameV1 iCurrentNetworkNameV1;

private:
	/**
	Packages iCurrentNetworkNameV1.
	*/
	CTelephony::TNetworkNameV1Pckg iCurrentNetworkNameV1Pckg;

	};

#endif // __CCURRENTNETWORKNAME_H__
