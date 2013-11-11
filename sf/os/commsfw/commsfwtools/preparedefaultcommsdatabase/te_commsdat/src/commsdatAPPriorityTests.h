/**
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file commsdatAPPriorityTests.h
*/
#if (!defined __COMMSDAT_APPRIORITY_TESTS_H__)
#define __COMMSDAT_APPRIORITY_TESTS_H__
#include <e32base.h>
#include <test/testexecutestepbase.h>
#include "Te_commsdatSuiteStepBase.h"
#include <metadatabase.h>
#include <commsdat.h>
#include <commsdattypesv1_1.h>

using namespace CommsDat;
using namespace Meta;

_LIT(KCommsdat500Step,"commsdat500Step");
class CCommsdat500Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat500Step();
	~CCommsdat500Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	static TBool CheckPrioritiesL(CMDBSession& aCDSsession);
private:
	CMDBSession* iCDSession;
	};	


_LIT(KCommsdat501Step,"commsdat501Step");
class CCommsdat501Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat501Step();
	~CCommsdat501Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	static TBool LaunchProcess(const TDesC& aProcName, const TDesC& aParams);
private:
	CMDBSession* iCDSession;
	};	

#endif  //__COMMSDAT_APPRIORITY_TESTS_H__

