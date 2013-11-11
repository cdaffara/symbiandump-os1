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
 @file Connections.TestSteps.h
*/

#if (!defined CONNECTIONS_TESTSTEPS_H)
#define CONNECTIONS_TESTSTEPS_H

#include <test/testexecutestepbase.h>
#include <metadatabase.h>
#include "Te_EsockStepBase.h"

_LIT(KTe_ConnectionName,"ConnName");
_LIT(KTe_AttachToConnectionName,"AttachToConnName");
_LIT(KTe_ConnectionStopTypeName,"ConnStopType");
_LIT(KTe_ConnStopNormal,"EStopNormal");
_LIT(KTe_ConnStopAuthoritative,"EStopAuthoritative");
_LIT(KTe_ConnectionType,"ConnType");

_LIT(KTe_ConnectionAttachTypeName,"ConnAttachType");
_LIT(KTe_ConnAttachTypeNormal,"EAttachTypeNormal");
_LIT(KTe_ConnAttachTypeMonitor,"EAttachTypeMonitor");

_LIT(KTe_CommDbBearerCSD,"KCommDbBearerCSD");
_LIT(KTe_CommDbBearerWcdma,"KCommDbBearerWcdma");
_LIT(KTe_CommDbBearerLAN,"KCommDbBearerLAN");
_LIT(KTe_ParameterType, "ParameterType");
_LIT(KTe_ParameterExpectedValue,"ParameterExpectedValue");


//mobility api
_LIT(KTe_MobilityAPIName,"MobilityExtName");

//mobility notifications

_LIT(KTe_MobilityNotificationPreferredCarrierAvailable,"PreferredCarrierAvailable");
_LIT(KTe_MobilityNotificationPreferredCarrierAvailable_OldAP,"OldAP");
_LIT(KTe_MobilityNotificationPreferredCarrierAvailable_NewAP,"NewAP");
_LIT(KTe_MobilityNotificationNewCarrierActive,"NewCarrierActive");
_LIT(KTe_MobilityNotificationNewCarrierActive_NewAP,"NewAP");

_LIT(KTe_MobilityNotificationMigrateToPreferredCarrier,"MigrateToPreferredCarrier");
_LIT(KTe_MobilityNotificationIgnorePreferredCarrier,"IgnorePreferredCarrier");
_LIT(KTe_MobilityNotificationNewCarrierAccepted,"NewCarrierAccepted");
_LIT(KTe_MobilityNotificationNewCarrierRejected,"NewCarrierRejected");

// Availability extension api

_LIT(KTe_AvailabilityExpectedValue, "AvailabilityExpectedValue");
_LIT(KTe_AvailabilityAPIExtensionName, "AvailabilityExtensionName");

// Availability notifications
_LIT(KTe_AvailabilityNotification, "RequestNotificationOfAvailabilityChange");

_LIT(KTe_NetworkIndexName,"NET");

// Events in general
//------------------
_LIT(KTimeoutInMilliSeconds, "TimeoutInMilliSeconds");

// Connection Manager
//-------------------

// Connection status and notification extension api
_LIT(KConnStatusResponse, "ConnStatusResponse");
_LIT(KConnStatusChangeNotificationResponse, "ConnStatusChangeNotificationResponse");

//helpers
_LIT(KTe_CreatePublishSubscribeVar, "createpublishsubscribevar");
_LIT(KTe_CreatePublishSubscribeVarUid, "CreatePublishSubscribeVarUid");
_LIT(KTe_CreatePublishSubscribeVarValue, "CreatePublishSubscribeVarValue");
_LIT(KTe_CreatePublishSubscribeVarKey, "CreatePublishSubscribeVarKey");

_LIT(KTe_CheckPublishSubscribeVar, "checkpublishsubscribevar");
_LIT(KTe_CheckPublishSubscribeVarUid, "CheckPublishSubscribeVarUid");
_LIT(KTe_CheckPublishSubscribeVarValue, "CheckPublishSubscribeVarValue");
_LIT(KTe_CheckPublishSubscribeVarKey, "CheckPublishSubscribeVarKey");

//progress notification events
_LIT(KTe_ConnEventsName,"ConnEventsName");
_LIT(KTe_SelectedProgress,"SelectedProgress");
_LIT(KTe_LinkLayerOpen,"KLinkLayerOpen");
_LIT(KTe_LinkLayerClosed,"KLinkLayerClosed");

// Classes
//--------

/**
Class implementing CreateRConnectionStep

@internalComponent
*/
class CCreateRConnectionStep : public CTe_EsockStepBase
	{
public:
	CCreateRConnectionStep(CCEsockTestBase*& aEsockTest);
	TVerdict doTestStepPreambleL();
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TRConnectionParams iParams; //current params (.ini)
	};

_LIT(KCreateRConnectionStep,"CreateRConnectionStep");


/**
Class implementing StopRConnectionStep

@internalComponent
*/
class CStopRConnectionStep : public CTe_EsockStepBase
	{
public:
	CStopRConnectionStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TRConnectionParams iParams; //current params (.ini)
	};

_LIT(KStopRConnectionStep,"StopRConnectionStep");


/**
Class implementing CloseRConnectionStep

@internalComponent
*/
class CCloseRConnectionStep : public CTe_EsockStepBase
	{
public:
	CCloseRConnectionStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TRConnectionParams iParams; //current params (.ini)
	};

_LIT(KCloseRConnectionStep,"CloseRConnectionStep");


/**
Class implementing StartRConnectionStep

@internalComponent
*/
class CStartRConnectionStep : public CTe_EsockStepBase
	{
public:
	CStartRConnectionStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

protected:
    TRConnectionParams iParams; //current params (.ini)
	};

_LIT(KStartRConnectionStep,"StartRConnectionStep");

/**
Class implementing StartStopCrazyLoopRConnectionStep
The test does a loop, where every iteration:
(1) Starts the connection
(2) Snoozes for an <interval>
(3) Stops the connection injecting a cancel at that stage of the connection start
(4) Increases the <interval> a bit so that next time round the cancel is injected at a different stage.

Note that since the test is normally executed by a thread of lower priority than ESock_IP, it relies on
ESock_IP yielding (otherwise it won't test different phases. 
For instance DummyMCPR will artificially yield on TCFServiceProvider::TJoinRequest.
NetMCPR will not normally yield as is. DummyFlowFactory will yield on serving a TFindOrCreateRequest.

@internalComponent
*/
class CStartStopCrazyLoopRConnectionStep : public CStartRConnectionStep
    {
public:
    enum {
    /*
     * The test executes a loop of starting and then stopping a connection at discrete, growing intervals.
     * It is crucial that enough iterations are executed, as otherwise the test isn't doing its job.
     * The test will fail if this is detected. As iteration gaps are time-based the test first calibrates 
     * itself by running CStartStopCrazyLoopRConnectionStep::CalibrateStart to determine how 
     * much time does the start take in order to chop this span further into ELoops intevals. 
     * Calibration isn't uterrly precise and it can be that a connection startup executed within
     * the loop completes quicker than during calibration. It has been in fact observed 
     * in the overnight builds (about 7.5%). A number of things can be done to accomdate for that.
     * The approach chosen is to: 
     * (1) Plan for EPlannedLoops (chop down the CStartStopCrazyLoopRConnectionStep::CalibrateStart 
     *     into EPlannedLoops
     * (2) Insist on excuting at least EMandatoryLoops (the test will fail if less iterations are executed)
     * (3) Keep on iterating beyond EPlannedLoops until the snooze slides of the connection 
     *     startup time (the calbrated connection startup time equally might have been unrepresentativelly short)
     */     
    EPlannedLoops = 15,
    EMandatoryLoops = 14,
    };
    
    CStartStopCrazyLoopRConnectionStep(CCEsockTestBase*& aEsockTest);
    virtual ~CStartStopCrazyLoopRConnectionStep();
    
    TVerdict doSingleTestStep();
    TInt CalibrateStart();
    virtual TInt doLoopStep(TInt aTimerVal);
    
protected:
    RTimer iTimer;
    TRequestStatus* ipConnectionStartStatus;
    };

_LIT(KStartStopCrazyLoopRConnectionStep,"StartStopCrazyLoopRConnectionStep");

/**
Class implementing StartCloseCrazyLoopRConnectionStep
The test does a loop, where every iteration:
(1) Starts the connection
(2) Snoozes for an <interval>
(3) Closes the connection injecting a cancel at that stage of the connection start
(4) Increases the <interval> a bit so that next time round the cancel is injected at a different stage.

Note that since the test is normally executed by a thread of lower priority than ESock_IP, it relies on
ESock_IP yielding (otherwise it won't test different phases. 
For instance DummyMCPR will artificially yield on TCFServiceProvider::TJoinRequest.
NetMCPR will not normally yield as is. DummyFlowFactory will yield on serving a TFindOrCreateRequest.

@internalComponent
*/
class CStartCloseCrazyLoopRConnectionStep : public CStartStopCrazyLoopRConnectionStep
    {
public:
    CStartCloseCrazyLoopRConnectionStep(CCEsockTestBase*& aEsockTest);
    virtual TInt doLoopStep(TInt aTimerVal);
    };

_LIT(KStartCloseCrazyLoopRConnectionStep,"StartCloseCrazyLoopRConnectionStep");

/**
Class implementing openrconnectionStep

@internalComponent
*/
class COpenRConnectionStep : public CTe_EsockStepBase
	{
public:
	COpenRConnectionStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TRConnectionParams iParams; //current params (.ini)
	};

_LIT(KOpenRConnectionStep,"openrconnectionStep");


/**
Class implementing AcceptIncomingConnectionStep

@internalComponent
*/
class CAcceptIncomingConnectionStep : public CTe_EsockStepBase
	{
public:
	CAcceptIncomingConnectionStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TRSubConnectionParams iParams;  //current params (.ini)
	};

_LIT(KAcceptIncomingConnectionStep,"AcceptIncomingConnectionStep");


/**
Class implementing RejectIncomingConnectionStep

@internalComponent
*/
class CRejectIncomingConnectionStep : public CTe_EsockStepBase
	{
public:
	CRejectIncomingConnectionStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TRSubConnectionParams iParams;  //current params (.ini)
	};

_LIT(KRejectIncomingConnectionStep,"RejectIncomingConnectionStep");


/**
Class implementing WaitForIncomingConnectionStep

@internalComponent
*/
class CWaitForIncomingConnectionStep : public CTe_EsockStepBase
	{
public:
	CWaitForIncomingConnectionStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TPtrC timerSet;
    TPtrC subConnStatus;
    RTimer iTimer;
    TRSubConnectionParams iParams;  //current params (.ini)

	};

_LIT(KWaitForIncomingConnectionStep,"WaitForIncomingConnectionStep");


/**
Class implementing AsyncAcceptIncomingConnectionStep.h

@internalComponent
*/
class CAsyncAcceptIncomingConnectionStep : public CTe_EsockStepBase
	{
public:
	CAsyncAcceptIncomingConnectionStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TPtrC timerSet;
    TPtrC subConnStatus;
    RTimer iTimer;
    TRSubConnectionParams iParams;  //current params (.ini)
	};

_LIT(KAsyncAcceptIncomingConnectionStep,"AsyncAcceptIncomingConnectionStep");


// Enumerating connections
//------------------------
class TEnumerateConnectionsParams
	{
public:
	TPtrC iConnectionName;
	TUint iExpectedConnCount;
	};

class CEnumerateConnectionsStep : public CTe_EsockStepBase
	{
public:
	CEnumerateConnectionsStep(CCEsockTestBase*& aEsockTest);
	TInt CEnumerateConnectionsStep::ConfigureFromIni();
	TVerdict CEnumerateConnectionsStep::doSingleTestStep();

private:
    TEnumerateConnectionsParams iParams;
	};

_LIT(KEnumerateConnectionsStep,"EnumerateConnectionsStep");

// Attach to connection
//---------------------

class CattachtorconnectionStep : public CTe_EsockStepBase
	{
public:
	CattachtorconnectionStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TRConnectionParams iParams; //current params (.ini)
	};


_LIT(KattachtorconnectionStep,"attachtorconnectionStep");

// Await connection start complete
//---------------------

class CAwaitRConnectionStartComplete : public CTe_EsockStepBase
	{
public:
	CAwaitRConnectionStartComplete(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TRConnectionParams iParams; //current params (.ini)
	};


_LIT(KAwaitRConnectionStartComplete,"AwaitRConnectionStartComplete");

//Set a publish subscribe var

/**
Class implementing CreateRConnectionStep

@internalComponent
*/
class CCreatePublishSubscribeVar : public CTe_EsockStepBase
	{
public:
	CCreatePublishSubscribeVar(CCEsockTestBase*& aEsockTest);
	TVerdict doTestStepPreambleL();
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TInt iUid;
	TInt iValue;
	TInt iKey;
	};

_LIT(KCreatePublishSubscribeVar,"CreatePublishSubscribeVar");

//Check a publish subscribe var
/**
Class implementing CreateRConnectionStep

@internalComponent
*/
class CCheckPublishSubscribeVar : public CTe_EsockStepBase
	{
public:
	CCheckPublishSubscribeVar(CCEsockTestBase*& aEsockTest);
	TVerdict doTestStepPreambleL();
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TInt iUid;
	TInt iValue;
	TInt iKey;
	};

_LIT(KCheckPublishSubscribeVar,"CheckPublishSubscribeVar");


/**
Class implementing RegisterProgressNotificationStep

@internalComponent
*/
class CRegisterProgressNotificationStep : public CTe_EsockStepBase
	{
public:
	CRegisterProgressNotificationStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TEventContainerParams iParams; //current params (.ini)
	};

_LIT(KRegisterProgressNotificationStep,"RegisterProgressNotificationStep");


class CCheckProgressNotificationStep : public CTe_EsockStepBase
	{
public:
	CCheckProgressNotificationStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();
private:
	TEventContainerParams iParams; //current params (.ini)
	};

_LIT(KCheckProgressNotificationStep,"CheckProgressNotificationStep");

class CCheckNegativeProgressNotificationStep : public CTe_EsockStepBase
	{
public:
	CCheckNegativeProgressNotificationStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();
private:
	TEventContainerParams iParams; //current params (.ini)
	};

_LIT(KCheckNegativeProgressNotificationStep,"CheckNegativeProgressNotificationStep");


class CGetParameters_IntStep : public CTe_EsockStepBase
    {
public:
    CGetParameters_IntStep(CCEsockTestBase*& aEsockTest);
    TVerdict doSingleTestStep();
    TInt ConfigureFromIni();
private:
    TPtrC iConnectionName;
    TPtrC iFieldName;    
    CommsDat::TMDBElementId iRecordTypeId;
    TInt  iExpectedValue;
    };

_LIT(KGetParameters_IntStep,"GetParameters_IntStep");



class CWaitStep : public CTe_EsockStepBase
    {
public:
    CWaitStep(CCEsockTestBase*& aEsockTest);
    TVerdict doSingleTestStep();
    TInt ConfigureFromIni();
private:
    TInt iTimeOutMs;
    };

_LIT(KWaitStep,"WaitStep");


#endif // CONNECTIONS_TESTSTEPS_H


