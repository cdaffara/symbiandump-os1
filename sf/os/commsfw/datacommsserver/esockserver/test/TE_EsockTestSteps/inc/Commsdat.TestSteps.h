/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file Commsdat.TestSteps.h
*/

#if (!defined COMMSDAT_TESTSTEPS_H)
#define COMMSDAT_TESTSTEPS_H

//#include <e32base.h>
#include <metadatabase.h>
#include <commsdat.h>
#include <commsdattypesv1_1.h>
//#include <commsdattypeinfov1_1.h> // TODO_CDG needed?

#include <test/testexecutestepbase.h>
#include "Te_EsockStepBase.h"

// Test step classes
//------------------

/**
Class implementing CreateRConnectionServStep

@internalComponent
*/
class CCommsdatTestStep : public CTe_EsockStepBase
	{
public:
	CCommsdatTestStep(CCEsockTestBase*& aEsockTest) : CTe_EsockStepBase(aEsockTest) {}

protected:
	};


/**
Class implementing CUpdateCommsdatRecordStep

@internalComponent
*/
class CUpdateCommsdatRecordStep : public CCommsdatTestStep
	{
public:
	CUpdateCommsdatRecordStep(CCEsockTestBase*& aEsockTest);
	TVerdict doTestStepPreambleL();
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TInt iRecordId;
	TPtrC iRecordTypeName;
	};

_LIT(KUpdateCommsdatRecordStep, "UpdateCommsdatRecordStep");


/**
Class implementing CDeleteCommsdatRecordStep

@internalComponent
*/
class CDeleteCommsdatRecordStep : public CCommsdatTestStep
	{
public:
	CDeleteCommsdatRecordStep(CCEsockTestBase*& aEsockTest);
	TVerdict doTestStepPreambleL();
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TInt iRecordId;
	TPtrC iRecordTypeName;
	};

_LIT(KDeleteCommsdatRecordStep, "DeleteCommsdatRecordStep");


/**
Class implementing CCreateCommsdatRecordStep

@internalComponent
*/
class CCreateCommsdatRecordStep : public CCommsdatTestStep
	{
public:
	CCreateCommsdatRecordStep(CCEsockTestBase*& aEsockTest);
	TVerdict doTestStepPreambleL();
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TInt iRecordId;
	TPtrC iRecordTypeName;
	};

_LIT(KCreateCommsdatRecordStep, "CreateCommsdatRecordStep");



#endif // COMMSDAT_TESTSTEPS_H

