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


#ifndef __COPERATORNAME_H__
#define __COPERATORNAME_H__

#include <e32base.h>
#include <e32cons.h>
#include <e32def.h>
#include <etel3rdparty.h>

#include "CISVAPIAsync.h"

#include "CMainMenu.h"

/**
Retrieves the operator the phone is currently working on.
*/
class COperatorName : public CISVAPIAsync
	{

// Methods
public:
	static COperatorName* NewL(MExecAsync* aController);
	~COperatorName();

	void DoStartRequestL();

private:
	COperatorName(MExecAsync* aController);
	void ConstructL();

	void RunL();
	void DoCancel();

// Data
public:
	/**
	Stores retrieved network operator name.
	*/
	CTelephony::TOperatorNameV1 iOperatorNameV1;

private:
	/**
	Packages iOperatorNameV1.
	*/
	CTelephony::TOperatorNameV1Pckg iOperatorNameV1Pckg;

	};

#endif // __COPERATORNAME_H__
