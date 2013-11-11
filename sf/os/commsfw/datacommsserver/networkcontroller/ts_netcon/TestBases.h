// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TESTBASES_H__
#define __TESTBASES_H__

#include <e32base.h>
#include "CNetworkController.h"
#include "NetConTestBases.h"
#include <comms-infras/mdbaccess.h>
#include "MNetConDialogProcAccess.h"

//
//	Base class for tests using the stubbed CNetworkController
//
class CControllerTestBase : public CNetConTestBase, public MCommsDbAccess, public MNetConEnv
	{
public:
	virtual ~CControllerTestBase();

	// MCommsDbAccess
	virtual TInt GetConnectionAttempts();
	virtual void GetCurrentSettingsL(TConnectionSettings& aSettings, TCommDbConnectionDirection aDirection, TUint32 aRank);
	virtual void SetCurrentSettingsL(const TConnectionSettings& aSettings);
	virtual void SetOverridesL(CCommDbOverrideSettings* aOverrides);
	virtual void GetPreferedIapL(TUint32& aIapId, TCommDbConnectionDirection aDirection, TUint32 aRank);
	virtual void SetPreferedIapL(TUint32& aIapId, TCommDbConnectionDirection aDirection, TUint32 aRank);
	virtual TBool DoesIapExistL(TUint32 aIapId);
	virtual void GetFirstValidIapL(TUint32& aIapId);
	virtual TBool IsTelNumLengthZeroForRasConnectionL(TConnectionSettings& aSettings);
	virtual void GetServiceSettingsL(TConnectionSettings& aSettings);
	virtual RMobilePhone::TMobilePhoneNetworkMode NetworkMode() const;
	virtual void SetNetworkMode(RMobilePhone::TMobilePhoneNetworkMode aMode);
	virtual void GetDesL(const TDesC& aTable, const TDesC& aField, TDes16& aValue);
	virtual void GetIntL(const TDesC& aTable, const TDesC& aField, TUint32& aValue);
	virtual void GetTsyNameL(TDes& aName);
	virtual void GetBearerAvailabilityTsyNameL(TDes& aTsyName);
	virtual void RequestNotificationOfServiceChangeL(MServiceChangeObserver* aObserver);
	virtual void CancelRequestNotificationOfServiceChange(MServiceChangeObserver* aObserver);


	virtual TInt WriteInt(const TDesC& aField, TUint32 aValue);
	virtual TInt WriteBool(const TDesC& aField, TBool aValuge);
	virtual TInt WriteDes(const TDesC& aField, const TDesC8& aValue);
	virtual TInt WriteDes(const TDesC& aField, const TDesC16& aValue);

	virtual void Close();
	virtual TBool IsShowingHiddenRecords();
	virtual void SetCommPortL( const RCall::TCommPort& );
	virtual void GetServiceTypeL( TDes& );
	virtual void GetAuthParamsL( TBool&, TDes&, TDes& );
	virtual void GetAgentExtL(const TDesC& aServiceType, TDes& aAgentExt);
	virtual TInt ReadInt(const TDesC& aField, TUint32& aValue);
	virtual TInt ReadBool(const TDesC& aField, TBool& aValue);
	virtual TInt ReadDes(const TDesC& aField, TDes8& aValue);
	virtual TInt ReadDes(const TDesC& aField, TDes16& aValue);
	virtual HBufC* ReadLongDesLC(const TDesC& aField);
	virtual void GetDesL(const TDesC& aTable, const TDesC& aField, TDes8& aValue);
	virtual void GetBoolL(const TDesC& aTable, const TDesC& aField, TBool& aValue);
	virtual HBufC* GetLongDesLC(const TDesC& aTable, const TDesC& aField);
	virtual TInt GetLengthOfLongDesL(const TDesC& aTable, const TDesC& aField);
	virtual void GetGlobalL(const TDesC& aName,TUint32& aVal);

	virtual CCommsDbAccess::CCommsDbAccessModemTable* ModemTable();
	virtual TUint32 LocationId() const;
	virtual TCommDbConnectionDirection GetConnectionDirection() const;

	

	// MNetConEnv
	virtual void RequestComplete(const CSelectionRequest* aRequest, TInt aError);
	virtual void RequestComplete(const CReconnectRequest* aRequest, TInt aError);
	virtual const TConnPref& ImplicitConnectionPrefs() const;
	virtual const HBufC* ImplicitConnectionAgentName() const;
	virtual TInt FindExistingAgentForSelection(CNifAgentBase*& aAgent, CCommsDbAccess*) const;
	virtual void CheckBearerAvailability(TBool aIsReconnect);
	virtual TInt RequestSecondPhaseAvailability();
	virtual void AddAgentToNetworkL(CNifAgentBase* aAgent, TUint32 aNetworkId);
	virtual void CancelBearerAvailabilityCheck();
	virtual CCommsDbAccess* NewDatabaseL();
	virtual CNetConDlgProcessor* NewDialogProcessorL();


protected:
	CControllerTestBase();

	// Used internally in implementation of MCommsDbAccess
	virtual void GetMinSigStrengthL(TUint32& aMinAcceptedSignalStrength);

	/*
	  Used to signal the call of an unimplemented function.
	  Primarily a debugging conveniance.
	*/
	void Unimplemented() const;


	/** Data capability checking */
	virtual TInt DoCheckReadCapability( const TDesC& aField, const RMessagePtr2* aMessage );
	virtual TInt DoCheckWriteCapability( const TDesC& aField, const RMessagePtr2* aMessage );



protected:
	TConnPref iImplicitConnPref;
	};


//
//	Base class for the CSelectionRequest and CReconnectRequest tests
//
class CNetConRequestTestBase : public CControllerTestBase, public MNetConDialogProcAccess, public MNetworkControllerObserver
	{
public:

	enum TDialogBoxAction
			{
			ESelectModemLocation,
			ESelectConnection,
			EWarnNewConnection
			};

	virtual ~CNetConRequestTestBase();

	virtual CNetConDlgProcessor* NewDialogProcessorL();

	// MNetConDialogProcAccess
	virtual void SelectConnection(MDialogProcessorObserver& aObserver, const TConnectionPrefs& aPrefs);
	virtual void SelectConnection(MDialogProcessorObserver& aObserver, const TConnectionPrefs& aPrefs, TInt aLastError);
	virtual void SelectModemAndLocation(MDialogProcessorObserver& aObserver);
	virtual void WarnNewConnection(MDialogProcessorObserver& aObserver, const TConnectionPrefs& aPrefs, const TDesC* aNewIapName, const TIspConnectionNames* aNewConnectionNames, TInt aLastError);
	virtual void Reconnect(MDialogProcessorObserver& aObserver);
	virtual void CancelEverything();

	// MNetworkControllerObserver
	virtual void SelectComplete(const TDesC& aName);
	virtual void SelectComplete(TInt aError);
	virtual void ReconnectComplete(TInt aError);

	// override of function from CNetConTestBase
	TInt RequestSecondPhaseAvailability();

protected:
	CNetConRequestTestBase();
	void ConstructL();

	static TInt BearerCb(TAny* aThisPtr);
	void BearerCbComplete();
	static TInt DialogCb(TAny* aThisPtr);
	virtual void DialogCbComplete() = 0;

	void StartTestL();
	void GetDefaultTestSettings(TConnectionSettings& aSettings);

protected:
	CNifAgentBase* iAgent;
	CNetConRequestBase* iRequest;
	CAsyncCallBack* iBearerCb;
	CAsyncCallBack* iDialogCb;
	MDialogProcessorObserver* iDialogBoxObserver;
	TDialogBoxAction iDialogBoxAction;
	TInt iExpectedError;
	TUint32 iAvailableBearers;
	TBool iSecondPhaseAvailability;
	TCommDbDialogPref iDialogBoxPref;
	TInt iDialogBoxError;
	};


//
//	Base class for the CReconnectRequest tests
//
class CNetConReconnectRequestTestBase : public CNetConRequestTestBase
	{
public:

	virtual ~CNetConReconnectRequestTestBase();

	// override of functions from CNetConTestBase
	virtual void RequestComplete(const CReconnectRequest* aRequest, TInt aError);
	virtual void CheckBearerAvailability(TBool aIsReconnect);
	virtual void SetOverridesL(CCommDbOverrideSettings* /*aOverrides*/) {;};

	// override of functions from CNetConRequestTestBase
	virtual void Reconnect(MDialogProcessorObserver& aObserver);

protected:
	CNetConReconnectRequestTestBase();
	void ConstructL();

	virtual void DialogCbComplete();
	};


//
//	Base class for the CSelectionRequest tests
//
class CNetConSelectionRequestTestBase : public CNetConRequestTestBase
	{
public:

	virtual ~CNetConSelectionRequestTestBase();

	// override of functions from CNetConTestBase
	virtual void RequestComplete(const CSelectionRequest* aRequest, TInt aError);
	virtual TInt GetConnectionAttempts();
	virtual void GetCurrentSettingsL(TConnectionSettings& aSettings, TCommDbConnectionDirection aDirection, TUint32 aRank);
	virtual void SetCurrentSettingsL(const TConnectionSettings& aSettings);
	virtual void SetOverridesL(CCommDbOverrideSettings* aOverrides);
	virtual void GetPreferedIapL(TUint32& aIapId, TCommDbConnectionDirection aDirection, TUint32 aRank);
	virtual void SetPreferedIapL(TUint32& aIapId, TCommDbConnectionDirection aDirection, TUint32 aRank);
	virtual TBool DoesIapExistL(TUint32 aIapId);
	virtual TBool IsTelNumLengthZeroForRasConnectionL(TConnectionSettings& aSettings);
	virtual void GetServiceSettingsL(TConnectionSettings& aSettings);
	virtual TInt FindExistingAgentForSelection(CNifAgentBase*& aAgent, CCommsDbAccess*) const;
	virtual void GetDesL(const TDesC& aTable, const TDesC& aField, TDes16& aValue);
	virtual void GetIntL(const TDesC& aTable, const TDesC& aField, TUint32& aValue);
	virtual void CheckBearerAvailability(TBool aIsReconnect);
	void AddAgentToNetworkL(CNifAgentBase* aAgent, TUint32 aNetworkId);

	// override of functions from CNetConRequestTestBase
	virtual void SelectConnection(MDialogProcessorObserver& aObserver, const TConnectionPrefs& aPrefs);
	virtual void SelectConnection(MDialogProcessorObserver& aObserver, const TConnectionPrefs& aPrefs, TInt aLastError);
	virtual void SelectModemAndLocation(MDialogProcessorObserver& aObserver);
	virtual void WarnNewConnection(MDialogProcessorObserver& aObserver, const TConnectionPrefs& aPrefs, const TDesC* aNewIapName, const TIspConnectionNames* aNewConnectionNames, TInt aLastError);

protected:
	CNetConSelectionRequestTestBase();
	void ConstructL();
	void DialogCbComplete();

protected:
	TConnStartType iConnectType;
	TInt iConnectionAttempt;
	TInt iLastConnectionError;
	TConnectionSettings iSettings;
	TUint32 iNetworkId;
	};


//
//	Base class for the CTelBearer tests
//

class CTelBearer;
class CCommsDbAccess;
class CNetConBearerTestBase : public CControllerTestBase, public MBearerObserver
	{
public:
	virtual ~CNetConBearerTestBase();

	// MBearerObserver
	virtual void BearerStateChange(CBearerBase* aBearer);
	virtual CCommsDbAccess * DbAccess();
protected:
	CNetConBearerTestBase();
	void ConstructL();

protected:
	CTelBearer *iTelBearer;
private:
	CCommsDbAccess *iDatabase;
	};


#endif // __TESTBASES_H__

