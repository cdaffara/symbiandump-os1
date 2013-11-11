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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#if (!defined __APPFWK_SYSSTART_TEST_STFP_H__)
#define __APPFWK_SYSSTART_TEST_STFP_H__

#include <domainmember.h>
#include "appfwk_sysstart_test_consts.h"

// Entry point
GLDEF_C TInt E32Main(void);

/**
Class encapsulates the functionality required to fail the state
transitions during startup. This can then be used to test the
State Transition Failure Policy as defined in the startup
resource files.

The class registers with the Domain Manager, listens for state
transitions, and either fails or passes the transition depending
on the presence of configuration files, which have been created
as part of the setup for the specific test case.
*/
class CStateTransFailurePolicy : public CDmDomain
	{
public:
	static CStateTransFailurePolicy* NewL(TDmHierarchyId aHierarchyId, TDmDomainId aDomainId);
	static CStateTransFailurePolicy* NewLC(TDmHierarchyId aHierarchyId, TDmDomainId aDomainId);
	void Start();
		
private:
	CStateTransFailurePolicy(TDmHierarchyId aHierarchyId, TDmDomainId aDomainId);
	void RunL();
	void PerformActionL();
	void WriteResultFileL();
	void ReadResultFileL();
	TBool IsTestableState();
	void AckStateL();

private:
	TInt         iCSCount;     // number of times test component called in Critical Static state
	TInt         iCDCount;     // number of times test component called in Critical Dynamic state
	TInt         iNCCount;     // number of times test component called in Non Critical state
	TInt         iCSFailCount; // number of times transition to Critical Static failed
	TInt         iCDFailCount; // number of times transition to Critical Dynamic failed
	TInt         iNCFailCount; // number of times transition to Non Critical failed
	RFs            iFs;
	TDmDomainState iState;
	};

#endif
