//
// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/** 
 * @file TE_EncrptStep.cpp.h
 *
 * This defines the CTE_EncrptStep class which is the base class for all 
 * the EncrptStep test step classes
 *
 */
 
#if (!defined __TE_ENCRPTSTEP_H__)
#define __TE_ENCRPTSTEP_H__

#include <e32std.h>
#include <e32svr.h>
#include <cdbstore.h>

#include "TE_EncrptServer.h"

// Connection Configuration Lits
_LIT(KNameDefault, "default");

// Test step names
_LIT(KCreateEntries,  "CreateEntries"); 
_LIT(KCheckPasswords ,  "CheckPasswords");
_LIT(KDeleteEntries  ,  "DeleteEntries");

//Error strings
_LIT(KEGeneric, "Test left!");

class TGlobalDbIds;			
class CTS_EncrptServer;

/* CTS_ConnPrefStep class declaration
 * Provides the base class for all ConnPref test steps
 *
 */
class CTE_EncrptStep : public CTestStep
	{
public:
	CTE_EncrptStep();
	virtual ~CTE_EncrptStep();
	enum TVerdict Test(TInt aResult);

	void ExpLogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,
		TRefByValue<const TDesC> aFmt,...);

	TGlobalDbIds* Get();
	void Set(TGlobalDbIds* aGlobals);

	//TGlobalDbIds* iGlobals;
	
	CTE_EncrptServer* iOwnerSuite;	///< Pointer to suite which owns this test step
	
	};


class TGlobalDbIds
{ 
public:
	TUint32 id1;
	TUint32 id2;
};

#endif /* __TE_ENCRPTSTEP_H__ */
