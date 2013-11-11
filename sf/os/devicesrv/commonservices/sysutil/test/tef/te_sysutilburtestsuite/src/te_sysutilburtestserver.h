/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* file te_sysutilburtestserver.h
* 
*
*/



#ifndef __TE_SYSUTIL_BUR_TEST_SERVER_H__
#define __TE_SYSUTIL_BUR_TEST_SERVER_H__

#include <test/testexecuteserverbase.h>
#include <babackup.h>

/*
This is used by the observer to determine which set of APIs it should be calling.
*/
enum TTestStep
	{
	ENoStep,
	EDeviceTypeInfoAPIsStep,
	EVersionAPIsStep
	};

/*
Concrete version of MBackupOperationObserver
*/
class CBackupObserver : public MBackupOperationObserver
	{
public:
	void HandleBackupOperationEventL(const TBackupOperationAttributes &aBackupOperationAttributes);
	};

class CSysUtilBurTestServer : public CTestServer
	{
public:
	virtual ~CSysUtilBurTestServer();
	static CSysUtilBurTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	void ConstructL(const TDesC& aName);
	TTestStep GetTestStep();
	TInt GetTestStepError();
	void SetTestStep(TTestStep aTestStep);
	void SetTestStepError(TInt aTestStepError);
private:
	CBaBackupSessionWrapper* iBackupSessionWrapper;
	CBackupObserver* iBackupObserver;
	
	// Used to store which set of APIs should be called during a backup/restore.
	TTestStep iTestStep;
	
	// Used to store the error code returned after calling a set of APIs during a backup/restore.
	TInt iTestStepError;
	};

#endif // __TE_SYSUTIL_BUR_TEST_SERVER_H__
