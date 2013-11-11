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
 @file ConnectionServer.TestSteps.h
*/

#if (!defined CONNECTIONSERVER_TESTSTEPS_H)
#define CONNECTIONSERVER_TESTSTEPS_H

#include <test/testexecutestepbase.h>
#include "Te_EsockStepBase.h"
#include <comms-infras/es_connectionserv.h>
#include <comms-infras/cs_connservparams.h>

//#include <comms-infras/connservwifiparams.h>

#include <networking/mbmsparams.h>
#include <networking/qos3gpp_subconparams.h>
#include <genericscprparams.h>

#include <comms-infras/es_availability.h>
#include <etelmm.h>

_LIT(KConnectionServName, "ConnServName");
_LIT(KConnectionServTierId, "TierId");

// Test step classes
//------------------

/**
Class implementing CreateRConnectionServStep

@internalComponent
*/
class CCreateRConnectionServStep : public CTe_EsockStepBase
	{
public:
	CCreateRConnectionServStep(CCEsockTestBase*& aEsockTest);
	TVerdict doTestStepPreambleL();
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TPtrC iConnectionServName;
	};

_LIT(KCreateRConnectionServStep, "CreateRConnectionServStep");


/**
Class implementing CConnectRConnectionServStep

@internalComponent
*/
class CConnectRConnectionServStep : public CTe_EsockStepBase
	{
public:
	CConnectRConnectionServStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();
	TInt TierNameToTierId(const TDesC& aTierName, TUint& aTierId);
	
private:
	TPtrC iConnectionServName;
	TUint iTierId;
	};

_LIT(KConnectRConnectionServStep, "ConnectRConnectionServStep");


/**
Class implementing CRConnectionServApiExtStep

@internalComponent
*/
class CRConnectionServApiExtStep : public CTe_EsockStepBase
	{
public:
	CRConnectionServApiExtStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();
	
private:
	RCommsDataMonitoringApiExt iExt;
	TPtrC iConnectionServName;
	TUint iTierId;
	};

_LIT(KRConnectionServApiExtStep, "RConnectionServApiExtStep");



/**
Class implementing CCloseRConnectionServStep

@internalComponent
*/
class CCloseRConnectionServStep : public CTe_EsockStepBase
	{
public:
	CCloseRConnectionServStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();
	
private:
	TPtrC iConnectionServName;
	};

_LIT(KCloseRConnectionServStep, "CloseRConnectionServStep");


class TExpectedAccessPointStatusMask : public ConnectionServ::TAccessPointStatusFilter
	{
public:
	TBool CompareToAPStatus(const ConnectionServ::TAccessPointStatus& aStatus) const;
		
private:
	TBool CompareStatusFlagToMaskFlag(
		const ConnectionServ::TAccessPointFlagValue apFlag,
		const ConnectionServ::TAccessPointFlagFilterValue apMaskValue) const;
	};


class TExpectedAccessPointParameters
	{
public:
	TExpectedAccessPointParameters(
		const TAccessPointInfo& aInfo,
		const TExpectedAccessPointStatusMask& aStatus)
			: iInfo(aInfo), iStatus(aStatus) {}
	const TExpectedAccessPointStatusMask& AccessPointStatus() const {return iStatus;}
	const TAccessPointInfo& AccessPointInfo() const {return iInfo;}

private:
	TAccessPointInfo iInfo;
	TExpectedAccessPointStatusMask iStatus;
	};

/**
Base class for query based RConnectionServ tests

@internalComponent
*/
class CRConnectionServQueryTestStepBase : public CTe_EsockStepBase
	{
public:
	CRConnectionServQueryTestStepBase(CCEsockTestBase*& aEsockTest) : CTe_EsockStepBase(aEsockTest) {}

protected:
	void BundleToString(const CParameterBundleBase* aBundle, TDes& aBuf);
	void ParamSetCtrToString(const CParameterSetContainer* aCtnr,TDes& aBuf);
	void GenericParamsToString(const ConnectionServ::XAccessPointGenericParameterSet* aSet, TDes& aBuf);
	//void WifiParamsToString(const ConnectionServ::XAccessPointWifiParameterSet* aSet, TDes& aBuf);
	void MBMSParamsToString(const ConnectionServ::XMBMSServiceQuerySet* aSet,
			TDes& aBuf);
	void MBMSServiceParamsToString(const ConnectionServ::XMBMSServiceParameterSet* aSet,
				TDes& aBuf); 
	TChar GetApFlagChar(ConnectionServ::TAccessPointFlagValue val);

	TInt BuildQueryL(ConnectionServ::CConnectionServParameterBundle& aQueryBundle);
	TInt BuildExpectedResultsL(
		RArray<TExpectedAccessPointParameters>& aExpectedAPParameters,
		TInt& aNumExpectedAPs);
	void FetchFilterL(const TDesC& aFilterDes, ConnectionServ::TAccessPointStatusFilter& aFetchedFilter);
	ConnectionServ::TAccessPointFlagFilterValue ReadFlagFilterValueL(TChar ch);

	TBool CompareAgainstExpected(
		TInt aNumExpected,
		RArray<TExpectedAccessPointParameters>& aAPsExpected,
		ConnectionServ::CConnectionServParameterBundle& aResultsBundle);
	};

/**
Class implementing CAccessPointStatusQueryStep

@internalComponent
*/
class CAccessPointStatusQueryStep : public CRConnectionServQueryTestStepBase
	{
public:
	CAccessPointStatusQueryStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	~CAccessPointStatusQueryStep();

	TPtrC iConnectionServName;
	ConnectionServ::CConnectionServParameterBundle*	iQueryBundle;
	ConnectionServ::CConnectionServParameterBundle*	iResultsBundle;
	TInt iExpectedError;
	TInt iNumExpectedAPs;
	RArray<TExpectedAccessPointParameters> iExpectedAPParameters;
	};

_LIT(KAccessPointStatusQueryStep, "AccessPointStatusQueryStep");


/**
Class implementing CCancelAccessPointStatusQueryStep

@internalComponent
*/
class CCancelAccessPointStatusQueryStep : public CRConnectionServQueryTestStepBase
	{
public:
	CCancelAccessPointStatusQueryStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	~CCancelAccessPointStatusQueryStep();

	TPtrC iConnectionServName;
	ConnectionServ::CConnectionServParameterBundle*	iQueryBundle;
	ConnectionServ::CConnectionServParameterBundle*	iResultsBundle;
	TInt iNumExpectedAPs;
	RArray<TExpectedAccessPointParameters> iExpectedAPParameters;
	};

_LIT(KCancelAccessPointStatusQueryStep, "CancelAccessPointStatusQueryStep");


/**
Class implementing CRequestAccessPointNotification

@internalComponent
*/
class CRequestAccessPointNotificationStep : public CRConnectionServQueryTestStepBase
	{
public:
	CRequestAccessPointNotificationStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TPtrC iConnectionServName;
	TPtrC iNotifName;
	ConnectionServ::CConnectionServParameterBundle*	iQueryBundle;
	ConnectionServ::CConnectionServParameterBundle*	iResultsBundle;
	TInt iExpectedError;
	TInt iNumExpectedAPs;
	RArray<TExpectedAccessPointParameters> iExpectedAPParameters;
	};

_LIT(KRequestAccessPointNotificationStep, "RequestAccessPointNotificationStep");


/**
Class implementing CReceiveAccessPointNotification

@internalComponent
*/
class CReceiveAccessPointNotificationStep : public CRConnectionServQueryTestStepBase
	{
public:
	CReceiveAccessPointNotificationStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TPtrC iConnectionServName;
	TPtrC iNotifName;
	RArray<TExpectedAccessPointParameters> iExpectedAPParameters;
	TInt iNumExpectedAPs;
	TInt iInitialDelayInSeconds;
	TBool iExpectingNotification;
	};

_LIT(KReceiveAccessPointNotificationStep, "ReceiveAccessPointNotificationStep");



/**
Class implementing CCancelAccessPointNotification

@internalComponent
*/
class CCancelAccessPointNotificationStep : public CRConnectionServQueryTestStepBase
	{
public:
	CCancelAccessPointNotificationStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TPtrC iNotifName;
	TInt iInitialDelayInSeconds;
	};

_LIT(KCancelAccessPointNotificationStep, "CancelAccessPointNotificationStep");



class MObserverTimeout
	{
public:
	virtual void Timeout() = 0;
	};


// CConnServNotificationWatcher
//-----------------------------
class CConnServNotificationWatcher :
	public CBase,
	public ConnectionServ::MAccessPointNotificationObserver
	{
public:
	// Thread entry point
	//-------------------
	static TInt EntryPoint(TAny* aArgs);
	void RunNotificationL();
	
	CConnServNotificationWatcher(ConnectionServ::RConnectionServ* aConnServ)
		: iConnServ(*aConnServ) {}

	static CConnServNotificationWatcher* NewLC(
		ConnectionServ::RConnectionServ* aConnServ,
		ConnectionServ::CConnectionServParameterBundle* aQueryBundle);
	
	void ConstructL(ConnectionServ::CConnectionServParameterBundle* aQueryBundle);

	// MAccessPointNotificationObserver
	//---------------------------------
	virtual void AccessPointNotification(ConnectionServ::CConnectionServParameterBundle* aResult);
	virtual void AccessPointNotificationError(TInt aErrCode);

	void Wait() {iMutex.Wait();}
	void Signal() {iMutex.Signal();}
	
	void AwaitStartupCompletionL() const;
	void WaitOnQueueL() const;
	
	void PerformNotificationRequestL();
	ConnectionServ::CConnectionServParameterBundle* GetResultsBundle();

	
	void CancelNotification();
	TInt ErrCode() const { return iErrCode; }
	void DoCancel();
	~CConnServNotificationWatcher();
	
private:
	TInt iErrCode;
	ConnectionServ::RConnectionServ iConnServ;
	ConnectionServ::CConnectionServParameterBundle* iQueryBundle;
	RPointerArray<ConnectionServ::CConnectionServParameterBundle> iNotificationBundles;
	TInt iTimeout;
	RMutex iMutex;
	RProperty iQueueProp;
	TBool iHaveReceivedNotifSetupAck;
	
public:
	RThread iNotifThread;
	RThread iCommandThread;
	TRequestStatus* iCancelStatus;
	};


// CNotifCancelControl
//--------------------
class CNotifCancelControl : public CActive
	{
public:
	CNotifCancelControl(CConnServNotificationWatcher& aNotifMgr)
		: iNotifManager(aNotifMgr), CActive(EPriorityStandard) {}
	
	static CNotifCancelControl* NewL(CConnServNotificationWatcher& aNotifMgr)
		{
		CNotifCancelControl* mine = new(ELeave) CNotifCancelControl(aNotifMgr);
		CActiveScheduler::Add(mine);
		return mine;
		}

	void Wait()
		{
		iStatus = KRequestPending;
		SetActive();
		iNotifManager.iCancelStatus = &iStatus;
		}

private:
	void DoCancel() {}
	void RunL()
		{
		// Stop the active scheduler which will cancel all outstanding requests
		CActiveScheduler::Stop();
		}
		
private:
	CConnServNotificationWatcher& iNotifManager;
	};


/**
Class implementing CSetAccessPointAvailabilityStep

@internalComponent
@description Enables the setting of availability status on access points for simulation of availability
*/
class CSetAccessPointAvailabilityStep : public CTe_EsockStepBase
	{
public:
	CSetAccessPointAvailabilityStep(CCEsockTestBase*& aEsockTest);
	TVerdict doTestStepPreambleL();
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TInt iAccessPointId;
	TInt iAvailabilityScore;
	};

_LIT(KSetAccessPointAvailabilityStep, "SetAccessPointAvailabilityStep");

/**
Class implementing CSetPppAccessPointAvailabilityStep

@internalComponent
@description Enables the setting of Ppp availability status on access points for simulation of availability
*/
class CSetPppAccessPointAvailabilityStep : public CTe_EsockStepBase
	{
public:
	CSetPppAccessPointAvailabilityStep(CCEsockTestBase*& aEsockTest);
	TVerdict doTestStepPreambleL();
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	RMobilePhone::TMobilePhoneRegistrationStatus iRegStatus;
	};

_LIT(KSetPppAccessPointAvailabilityStep, "SetPppAccessPointAvailabilityStep");

/**
Class implementing CSetPacketDataAccessPointAvailabilityStep

@internalComponent
@description Enables the setting of PDP availability status on access points for simulation of availability
*/
class CSetPacketDataAccessPointAvailabilityStep : public CTe_EsockStepBase
    {
public:
    CSetPacketDataAccessPointAvailabilityStep(CCEsockTestBase*& aEsockTest);
    TVerdict doTestStepPreambleL();
    TVerdict doSingleTestStep();
    TInt ConfigureFromIni();

private:
    TBool iSetToAvailable;
    };

_LIT(KSetPacketDataAccessPointAvailabilityStep, "SetPacketDataAccessPointAvailabilityStep");

/**
Class implementing CSetWifiAccessPointAvailabilityStep

@internalComponent
@description Enables the setting of Ppp availability status on access points for simulation of availability
*/
class CSetWifiAccessPointAvailabilityStep : public CTe_EsockStepBase
	{
public:
	CSetWifiAccessPointAvailabilityStep(CCEsockTestBase*& aEsockTest);
	TVerdict doTestStepPreambleL();
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TInt	iAPAvailable;
	TPtrC	iSsid;
	TPtrC	iBssid;
	TPtrC	iSupportedRates;
	TPtrC	iNetworkType;
	TPtrC	iAuthenticated;
	TPtrC	iEncrypt;
	TPtrC	iAPEncryptionType;
	TPtrC	iAssociated;
	TPtrC	iChannel;
	TPtrC	iRssi;
	};

_LIT(KSetWifiAccessPointAvailabilityStep, "SetWifiAccessPointAvailabilityStep");

// Changes for MBMS Broadcast related test cases starts here 
enum TTypeOfNotification
		{
		EMbmsBearerAvailNtfn =1,
		EMbmsServiceAvailNtfn,
		EMbmsRemoveServiceNtfn,
		EMbmsActiveListNtfn,
		EMbmsMonitorListNtfn
		};

_LIT(KRequestMBMSNtfnStep, "RequestMBMSNtfnStep");
_LIT(KReceiveMBMSNtfnStep, "ReceiveMBMSNtfnStep");
_LIT(KCancelMBMSNtfnStep, "CancelMBMSNtfnStep");

/**
Class implementing CRequestMBMSNtfnStep
@internalComponent
*/

class CRequestMBMSNtfnStep : public CRConnectionServQueryTestStepBase
	{
public:
	CRequestMBMSNtfnStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	 enum TAction
			   {
			    EAddService =1,
			    ERemoveService =2,
			    ERemoveAllServices=3
			    };
	 
	~CRequestMBMSNtfnStep();
	TInt BuildBearerAvailQueryL(ConnectionServ::CConnectionServParameterBundle& aQueryBundle);
	TInt BuildServiceAvailQueryL(ConnectionServ::CConnectionServParameterBundle& aQueryBundle);
	TInt BuildRemoveServiceQueryL(ConnectionServ::CConnectionServParameterBundle& aQueryBundle,TAction aAction);
	TInt BuildActiveListQueryL(ConnectionServ::CConnectionServParameterBundle& aQueryBundle);
	TInt BuildMonitorListQueryL(ConnectionServ::CConnectionServParameterBundle& aQueryBundle);
			
	TPtrC iConnectionServName;
	TPtrC iNotifName;   
	ConnectionServ::CConnectionServParameterBundle*	iQueryBundle;
	ConnectionServ::CConnectionServParameterBundle*	iQueryBundleAdd;
		
	TTypeOfNotification iNotificationType;

	TBool iIsMultiClient; 
	TUint iNumTmgis; 
	TUint iMCC;
	TUint iMNC;
	TInt iServiceId;
	TInt iAccessBearer;
	TInt iServiceMode;
	
	CArrayFixFlat <TTmgi> *iTmgiList;
		
	TBool iAllTmgiValid;
	TBool iAllTmgiInvalid;
	TBool iRemoveAll;
	};

/**
Class implementing CReceiveMBMSNtfnStep
@internalComponent
*/

class CReceiveMBMSNtfnStep : public CRConnectionServQueryTestStepBase
	{
public:
	CReceiveMBMSNtfnStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	~CReceiveMBMSNtfnStep();
	
	TPtrC iConnectionServName;
	TPtrC iNotifName;   
	TInt iInitialDelayInSeconds;
	TInt iExpectedValue;
	TInt iExpectedValue1;
	TTypeOfNotification iNotificationType;
	
	TUint iNumTmgis; 
	TUint iMCC;
	TUint iMNC;
	TUint iServiceId;
	TBool iAllTmgiValid;
	TBool iAllTmgiInvalid;
	TBool iRemoveAll;
	
	CArrayFixFlat <TTmgi> *iTmgiList;
	};

/**
Class implementing CancelMBMSNtfnStep
@internalComponent
*/

class CCancelMBMSNtfnStep : public CRConnectionServQueryTestStepBase
	{
public:
	CCancelMBMSNtfnStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	~CCancelMBMSNtfnStep();
		
	TPtrC iConnectionServName;
	TPtrC iNotifName;   
	};

	
#endif // CONNECTIONSERVER_TESTSTEPS_H

