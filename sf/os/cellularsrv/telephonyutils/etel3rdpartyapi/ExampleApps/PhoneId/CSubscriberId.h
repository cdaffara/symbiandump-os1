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


#ifndef __CSUBSCRIBERID_H__
#define __CSUBSCRIBERID_H__

#include <e32base.h>
#include <e32cons.h>
#include <e32def.h>
#include <etel3rdparty.h>


#include "CISVAPISync.h"

#include "CMainMenu.h"

/**
Retrieves and communicates information pertaining to the sim card currently
present in the phone.
*/
class CSubscriberId : public CISVAPISync
	{

// Methods
public:
	static CSubscriberId* NewL(MExecSync* aController);
	~CSubscriberId();

	void DoStartRequestL();

private:
	CSubscriberId(MExecSync* aController);
	void ConstructL();

	void RunL();
	void DoCancel();

// Data
public:
	/**
	Stores retrieved subscriber (IMSI) ID.
	*/
	CTelephony::TSubscriberIdV1 iSubscriberIdV1;

private:
	/**
	Packages iSubscriberIdV1.
	*/
	CTelephony::TSubscriberIdV1Pckg iSubscriberIdV1Pckg;

	};

#endif // __CSUBSCRIBERID_H__
