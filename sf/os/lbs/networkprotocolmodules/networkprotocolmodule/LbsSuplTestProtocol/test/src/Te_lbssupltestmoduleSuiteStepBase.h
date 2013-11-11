/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file Te_lbssupltestmoduleSuiteStepBase.h
*/

#if (!defined __TE_LBSSUPLTESTMODULE_STEP_BASE__)
#define __TE_LBSSUPLTESTMODULE_STEP_BASE__
#include <test/testexecutestepbase.h>
#include "Te_lbssupltestmoduleSuiteServer.h"
#include "Te_lbssupltestmoduleSuiteDefs.h"
/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/
class CTe_lbssupltestmoduleSuiteStepBase : public CTestStep
	{
public:
	virtual ~CTe_lbssupltestmoduleSuiteStepBase();
	CTe_lbssupltestmoduleSuiteStepBase(CTe_lbssupltestmoduleSuite* aServer);
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();

//Please add your class members which will be common to all individual test steps:
protected:
	HBufC8*		iReadData;
	HBufC8*		iWriteData;
	//Pointer to test server
	CTe_lbssupltestmoduleSuite* iServer;
	};

class MTestStepObserver
	{
public:
	virtual void notifyResult(const TInt& aResult) = 0;
	};
	
class CAssistanceDataRetriever : public CActive, public MLbsAssistanceDataSourceObserver
{
public:

	static CAssistanceDataRetriever* NewL(MTestStepObserver& aTestObserver, const TInt& aMaxNumberRequests);

	~CAssistanceDataRetriever();
	
	// Asynchronous request for assistance data
	void GetTheAssistanceData(const TLbsAssistanceDataGroup & aAssistanceDataFilter);
	
	// From CActive
	void RunL();
	void DoCancel();
	
	// From MLbsAssistanceDataSourceObserver
	void DataSourceNotification(const TInt& aError);
	
	void SetPosition(const TCoordinate& aPosition);
	
	static TInt StopWaitingForTimer(TAny*);
	
private:

	void ConstructL();
	CAssistanceDataRetriever(MTestStepObserver& aTestObserver, const TInt& aMaxNumberRequests);
    void DoStopWaitingForTimer();
	
private:
	CAssistanceDataSourceBase* iSuplSession;
	CAssistanceDataSourceModuleBase* iSuplModule;	
	RLbsAssistanceDataBuilderSet iSet;
	MTestStepObserver& iTestObserver;
	TInt iMaxNumberRequests;
	TInt iDataSetsReceived;
	TLbsAssistanceDataGroup iAssistanceDataFilter;
	CPeriodic* iTimer;
	TCoordinate iPosition;
};

#endif
