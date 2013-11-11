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
 * @file TE_RConnectionStep.h
 *
 * This defines the TE_RConnectionStep class which is the base class for all 
 * the RConnection multihoming test step classes
 *
 */
 
#if (!defined __TE_CONNPREFSTEPS_H__)
#define __TE_CONNPREFSTEPS_H__

#include <e32std.h>
#include <cdbstore.h>
#include "es_sock.h"
#include "in_sock.h"
#include <es_enum.h>
#include <commdbconnpref.h>

#include "TE_ConnPrefServer.h"
#include "TE_ConnPrefStep.h"

//
class CTE_ConstructAndVersion : public CTE_ConnPrefStep
{
public:
	CTE_ConstructAndVersion();
	virtual ~CTE_ConstructAndVersion();
	virtual enum TVerdict doTestStepL();
};

//
class CTE_SetAndGetConnPrefs : public CTE_ConnPrefStep
{
public:
	CTE_SetAndGetConnPrefs();
	virtual ~CTE_SetAndGetConnPrefs();
	virtual enum TVerdict doTestStepL();
};

//
class CTE_ComparePrefs : public CTE_ConnPrefStep
{
public:
	CTE_ComparePrefs();
	virtual ~CTE_ComparePrefs();
	virtual enum TVerdict doTestStepL();
};

//
class CTE_AssignAndCopyConstruct : public CTE_ConnPrefStep
{
public:
	CTE_AssignAndCopyConstruct();
	virtual ~CTE_AssignAndCopyConstruct();
	virtual enum TVerdict doTestStepL();
};

// 
class CTE_ExtensionIdAndCast : public CTE_ConnPrefStep
{
public:
	CTE_ExtensionIdAndCast();
	virtual ~CTE_ExtensionIdAndCast();
	virtual enum TVerdict doTestStepL();
};

//  
class CTE_MultiConnPref : public CTE_ConnPrefStep
{
public:
	CTE_MultiConnPref();
	virtual ~CTE_MultiConnPref();
	virtual enum TVerdict doTestStepL();
};


///////////////////////////////////////
//	Test step CTE_MultiConnPrefVersion
///////////////////////////////////////

NONSHARABLE_CLASS(CTE_MultiConnPrefVersion) : public CTE_ConnPrefStep
{
public:
	CTE_MultiConnPrefVersion();
	virtual ~CTE_MultiConnPrefVersion();
	virtual enum TVerdict doTestStepL();
	CCommsDbTableView*	iTheView;
	CCommsDatabase*		iTheDb;

};

////////////////////////////////////////////
//	Test step CTE_MultiConnectionAttempts
////////////////////////////////////////////

NONSHARABLE_CLASS(CTE_MultiConnectionAttempts): public CTE_ConnPrefStep
{
public:
	CTE_MultiConnectionAttempts();
	virtual ~CTE_MultiConnectionAttempts();
	virtual enum TVerdict doTestStepL();
};


#endif /* __TS_CONNPREFSTEPS_H__ */















