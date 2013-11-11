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


#ifndef __CCALLINFO_H__
#define __CCALLINFO_H__

#include <e32base.h>
#include <e32cons.h>
#include <e32def.h>
#include <etel3rdparty.h>

#include "CISVAPIAsync.h"

#include "CMainMenu.h"

/**
Retrieves information regarding the current call including the number to be
traversed with. Can be notified on change.
*/
class CCallInfo : public CISVAPIAsync
	{

// Methods
public:
	static CCallInfo* NewL(MExecAsync* aController);
	~CCallInfo();

	void DoStartRequestL();

private:
	CCallInfo(MExecAsync* aController);
	void ConstructL();

	void RunL();
	void DoCancel();

// Data
public:
	/**
	Stores general information about the call.
	*/
	CTelephony::TCallInfoV1 iCallInfoV1;
	/**
	Stores information about the remote party of iCallInfoV1.
	*/
	CTelephony::TRemotePartyInfoV1 iRemoteInfoV1;

private:
	/**
	Package descriptor for iCallInfoV1.
	*/
	CTelephony::TCallInfoV1Pckg iCallInfoV1Pckg;
	/**
	Package descriptor for iRemoteInfoV1.
	*/
	CTelephony::TRemotePartyInfoV1Pckg	iRemoteInfoV1Pckg;
	};

#endif // __CCALLINFO_H__
