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
 @file SocketServer.TestSteps.h
*/
#if (!defined SOCKETSERVER_TESTSTEPS_H)
#define SOCKETSERVER_TESTSTEPS_H

#include <test/testexecutestepbase.h>
#include "Te_EsockStepBase.h"

// General constants
//------------------
_LIT(KTe_SocketServName,"SocketServName");


/**
Class implementing CreateRSocketServStep

@internalComponent
*/
class CCreateRSocketServStep : public CTe_EsockStepBase
	{
public:
	CCreateRSocketServStep(CCEsockTestBase*& aEsockTest);
	TVerdict doTestStepPreambleL();
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TPtrC iSockServName;
	};

_LIT(KCreateRSocketServStep,"CreateRSocketServStep");


/**
Class implementing ConnectRSocketServStep

@internalComponent
*/
class CConnectRSocketServStep : public CTe_EsockStepBase
	{
public:
	CConnectRSocketServStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TPtrC iSockServName;
	};

_LIT(KConnectRSocketServStep,"ConnectRSocketServStep");


/**
Class implementing CloseRSocketServStep

@internalComponent
*/
class CCloseRSocketServStep : public CTe_EsockStepBase
	{
public:
	CCloseRSocketServStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TPtrC iSockServName;
	};

_LIT(KCloseRSocketServStep,"CloseRSocketServStep");


/**
Class implementing CBackupRestoreStep

@internalComponent
*/
class CBackupRestoreStep : public CTe_EsockStepBase
	{
public:
	CBackupRestoreStep(CCEsockTestBase*& aEsockTest);
	TVerdict doTestStepPreambleL();
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TPtrC iOriginalFileName;
    TPtrC iBackupFileName;
    TPtrC iNewFileName;
    TBool iIsBackup;
    };

_LIT(KBackupRestoreStep,"BackupRestoreStep");

/**
Class implementing CleanAllStep

@internalComponent
*/
class CCleanAllStep : public CTe_EsockStepBase
	{
public:
	CCleanAllStep(CCEsockTestBase*& aEsockTest);
	TVerdict doTestStepL();

	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();
	};

_LIT(KCleanAllStep,"CleanAllStep");


/**
Class implementing SimTSYTriggerEventStep

@internalComponent
*/
class CSimTSYTriggerEventStep : public CTe_EsockStepBase
	{
public:
	CSimTSYTriggerEventStep(CCEsockTestBase*& aEsockTest);
	TVerdict doTestStepPreambleL();
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    
    TSimTsyEventParams iParams;
	};

_LIT(KSimTSYTriggerEventStep,"SimTSYTriggerEventStep");


/**
Class implementing StartSimTSYStep

@internalComponent
*/
class CStartSimTSYStep : public CTe_EsockStepBase
	{
public:
	CStartSimTSYStep(CCEsockTestBase*& aEsockTest);
	TVerdict doTestStepPreambleL();
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	// Test section number
	void SetTestSectionNumber(TInt aTestSectionNumber);
	TInt TestSectionNumber() const;
	TInt iTestSectionNumber;
	};

_LIT(KStartSimTSYStep,"StartSimTSYStep");
_LIT(KSimTsySectionNumber,"TestSection");


/**
Class implementing StopSimTSYStep

@internalComponent
*/
class CStopSimTSYStep : public CTe_EsockStepBase
	{
public:
	CStopSimTSYStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    //TPtrC iSimTsyName;
	};

_LIT(KStopSimTSYStep,"StopSimTSYStep");


/**
Class implementing CSocketServerNumProtocolsStep

@internalComponent
*/
class CSocketServerNumProtocolsStep : public CTe_EsockStepBase
	{
public:
	CSocketServerNumProtocolsStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TSocketServerNumProtocolsParams iParams;
	};

_LIT(KSocketServerNumProtocolsStep,"SocketServerNumProtocolsStep");


/**
Class implementing CSocketServerFindProtocolStep

@internalComponent
*/
class CSocketServerFindProtocolStep : public CTe_EsockStepBase
	{
public:
	CSocketServerFindProtocolStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TSocketServFindProtocolParams iParams;
	};

_LIT(KSocketServerFindProtocolStep,"SocketServerFindProtocolStep");



/**
Class implementing CSocketServerGetProtocolInfoStep

@internalComponent
*/
class CSocketServerGetProtocolInfoStep : public CTe_EsockStepBase
	{
public:
	CSocketServerGetProtocolInfoStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TSocketServGetProtocolInfoParams iParams;
	};

_LIT(KSocketServerGetProtocolInfoStep,"SocketServerGetProtocolInfoStep");


/**
Class implementing CSocketServerStartProtocolStep

@internalComponent
*/
class CSocketServerStartProtocolStep : public CTe_EsockStepBase
	{
public:
	CSocketServerStartProtocolStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TSocketServStartStopProtocolParams iParams;
	};

_LIT(KSocketServerStartProtocolStep,"SocketServerStartProtocolStep");


/**
Class implementing CSocketServerStopProtocolStep

@internalComponent
*/
class CSocketServerStopProtocolStep : public CTe_EsockStepBase
	{
public:
	CSocketServerStopProtocolStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TSocketServStartStopProtocolParams iParams;
	};

_LIT(KSocketServerStopProtocolStep,"SocketServerStopProtocolStep");


/**
Class implementing CCompareIntegerValuesStep

@internalComponent
*/
class CCompareIntegerValuesStep : public CTe_EsockStepBase
	{
public:
	CCompareIntegerValuesStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TCompareIntegersParams iParams;
	};

_LIT(KCompareIntegerValuesStep,"CompareIntegerValuesStep");


#endif // SOCKETSERVER_TESTSTEPS_H

