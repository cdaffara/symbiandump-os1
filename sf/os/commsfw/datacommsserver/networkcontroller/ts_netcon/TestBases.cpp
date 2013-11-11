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

#include "TestBases.h"
#include "CStubbedAgent.h"
#include "NetConError.h"
#include "NetConLog.h"
#include "CTelBearer.h"
#include "CNetConDlgProcessor.h"
//
//                                                                                                               //
//                                    Implementation of class CControllerTestBase                                //
//                                                                                                               //
//
CControllerTestBase::~CControllerTestBase()
	{}

/**
   Called when an unimplemented function is called in a test case -
   this is primarily intended as a debugging conveniance
 */
void CControllerTestBase::Unimplemented() const
	{
	User::Invariant();
	}

CCommsDbAccess* CControllerTestBase::NewDatabaseL()
	{
	/* Construct a tear-off CCommsDbAccess interface */
	CCommsDbAccess* database = CCommsDbAccess::NewL( this );
	return database;
	}

CNetConDlgProcessor* CControllerTestBase::NewDialogProcessorL()
	{
	/*
	  Derived classes which implement the DialogProcessor
	  functionality can over-ride this
	*/
	Unimplemented();
	return NULL;
	}

CControllerTestBase::CControllerTestBase()
	{
	}

//
// from MCommsDbAccess
TInt CControllerTestBase::GetConnectionAttempts()
	{

	Unimplemented();
	return 0;
	}

void CControllerTestBase::GetCurrentSettingsL(TConnectionSettings&, TCommDbConnectionDirection, TUint32)
	{

	Unimplemented();
	}

void CControllerTestBase::SetCurrentSettingsL(const TConnectionSettings&)
	{

	Unimplemented();
	}

void CControllerTestBase::SetOverridesL(CCommDbOverrideSettings*)
	{

	Unimplemented();
	}

void CControllerTestBase::GetPreferedIapL(TUint32& /*aIapId*/, TCommDbConnectionDirection /*aDirection*/, TUint32 /*aRank*/)
	{

	Unimplemented();
	}

void CControllerTestBase::SetPreferedIapL(TUint32& /*aIapId*/, TCommDbConnectionDirection /*aDirection*/, TUint32 /*aRank*/)
	{

	Unimplemented();
	}


TBool CControllerTestBase::DoesIapExistL(TUint32 /*aIapId*/)
	{

	return ETrue;
	}

void CControllerTestBase::GetFirstValidIapL(TUint32& aIapId)
	{

	aIapId = 1;
	}

TBool CControllerTestBase::IsTelNumLengthZeroForRasConnectionL(TConnectionSettings&)
	{

	Unimplemented();
	return EFalse;
	}

void CControllerTestBase::GetServiceSettingsL(TConnectionSettings&)
	{

	Unimplemented();
	}

RMobilePhone::TMobilePhoneNetworkMode CControllerTestBase::NetworkMode() const
	{

	Unimplemented();
	return RMobilePhone::ENetworkModeUnknown;
	}

void CControllerTestBase::SetNetworkMode(RMobilePhone::TMobilePhoneNetworkMode)
	{

	Unimplemented();
	}

void CControllerTestBase::GetDesL(const TDesC&, const TDesC&, TDes16&)
	{

	Unimplemented();
	}

void CControllerTestBase::GetIntL(const TDesC& aTable, const TDesC& aField, TUint32& aValue)
	{
	if( aTable == TPtrC(MODEM_BEARER) && aField == TPtrC(MODEM_MIN_SIGNAL_LEVEL) )
		{
		// Forward to virtual member
		GetMinSigStrengthL( aValue );
		}
	else
		{
		Unimplemented();
		}
	}

void CControllerTestBase::GetTsyNameL( TDes& /*aTsyName*/ )
	{
	Unimplemented();
	}

void CControllerTestBase::GetBearerAvailabilityTsyNameL(TDes& aTsyName)
	{
	GetTsyNameL( aTsyName );
	}

void CControllerTestBase::GetMinSigStrengthL(TUint32&)
	{
	Unimplemented();
	}

void CControllerTestBase::RequestNotificationOfServiceChangeL(MServiceChangeObserver* /*aObserver*/)
	{
	}

void CControllerTestBase::CancelRequestNotificationOfServiceChange(MServiceChangeObserver* /*aObserver*/)
	{
	}

//
// from MNetConEnv
void CControllerTestBase::RequestComplete(const CSelectionRequest*, TInt)
	{

	Unimplemented();
	}

void CControllerTestBase::RequestComplete(const CReconnectRequest*, TInt)
	{

	Unimplemented();
	}

const HBufC* CControllerTestBase::ImplicitConnectionAgentName() const
	{

	Unimplemented();
	return NULL;
	}

const TConnPref& CControllerTestBase::ImplicitConnectionPrefs() const
	{
	Unimplemented();
	return iImplicitConnPref;
	}

TInt CControllerTestBase::FindExistingAgentForSelection(CNifAgentBase*&, CCommsDbAccess*) const
	{

	Unimplemented();
	return 0;
	}

void CControllerTestBase::CheckBearerAvailability(TBool)
	{

	Unimplemented();
	}

TInt CControllerTestBase::RequestSecondPhaseAvailability()
	{

	Unimplemented();
	return 0;
	}

void CControllerTestBase::AddAgentToNetworkL(CNifAgentBase*, TUint32)
	{

	Unimplemented();
	}

void CControllerTestBase::CancelBearerAvailabilityCheck()
	{
	}

void CControllerTestBase::Close()
	{
	}

TBool CControllerTestBase::IsShowingHiddenRecords()
	{
	Unimplemented();
	return EFalse;
	}

void CControllerTestBase::SetCommPortL( const RCall::TCommPort& )
	{
	Unimplemented();
	}

void CControllerTestBase::GetServiceTypeL( TDes& )
	{
	Unimplemented();
	}

TInt CControllerTestBase::ReadInt(const TDesC& /*aField*/, TUint32& /*aValue*/)
	{
	Unimplemented();
	return 0;
	}
TInt CControllerTestBase::ReadBool(const TDesC& /*aField*/, TBool& /*aValue*/)
	{
	Unimplemented();
	return 0;
	}
TInt CControllerTestBase::ReadDes(const TDesC& /*aField*/, TDes8& /*aValue*/)
	{
	Unimplemented();
	return 0;
	}
TInt CControllerTestBase::ReadDes(const TDesC& /*aField*/, TDes16& /*aValue*/)
	{
	Unimplemented();
	return 0;
	}
HBufC* CControllerTestBase::ReadLongDesLC(const TDesC& /*aField*/)
	{
	Unimplemented();
	return 0;
	}

void CControllerTestBase::GetDesL(const TDesC& /*aTable*/, const TDesC& /*aField*/, TDes8& /*aValue*/)
	{
	Unimplemented();
	}

void CControllerTestBase::GetBoolL(const TDesC& /*aTable*/, const TDesC& /*aField*/, TBool& /*aValue*/)
	{
	Unimplemented();
	}

HBufC* CControllerTestBase::GetLongDesLC(const TDesC& /*aTable*/, const TDesC& /*aField*/)
	{
	Unimplemented();
	return 0;
	}
TInt CControllerTestBase::GetLengthOfLongDesL(const TDesC& /*aTable*/, const TDesC& /*aField*/)
	{
	Unimplemented();
	return 0;
	}

void CControllerTestBase::GetGlobalL(const TDesC& /*aName*/,TUint32& /*aVal*/ )
	{
	Unimplemented();
	}

CCommsDbAccess::CCommsDbAccessModemTable* CControllerTestBase::ModemTable()
	{
	Unimplemented();
	return 0;
	}

TUint32 CControllerTestBase::LocationId() const
	{
	Unimplemented();
	return 0;
	}

TCommDbConnectionDirection CControllerTestBase::GetConnectionDirection() const
	{
	Unimplemented();
	return ECommDbConnectionDirectionUnknown;
	}

void  CControllerTestBase::GetAgentExtL(const TDesC& /*aServiceType*/, TDes& /*aAgentExt*/ )
	{
	Unimplemented();
	}

void CControllerTestBase::GetAuthParamsL( TBool&, TDes&, TDes& )
	{
	Unimplemented();
	}

TInt CControllerTestBase::WriteDes(const TDesC& /*aField*/, const TDesC16& /*aValue*/)
/**
Writes from the NIF are currently not supported...
*/
	{
	return KErrNotSupported;
	}

TInt CControllerTestBase::WriteDes(const TDesC& /*aField*/, const TDesC8& /*aValue*/)
/**
Writes from the NIF are currently not supported...
*/
	{
	return KErrNotSupported;
	}


TInt CControllerTestBase::WriteBool(const TDesC& /*aField*/, TBool /*aValue*/)
/**
Writes from the NIF are currently not supported...
*/
	{
	return KErrNotSupported;
	}

TInt CControllerTestBase::WriteInt(const TDesC& /*aField*/, TUint32 /*aValue*/)
/**
Writes from the NIF are currently not supported...
*/
	{
	return KErrNotSupported;
	}

TInt CControllerTestBase::DoCheckReadCapability( const TDesC& /*aField*/, const RMessagePtr2* /*aMessage*/ )
	{
	return KErrNone;
	}

TInt CControllerTestBase::DoCheckWriteCapability( const TDesC& /*aField*/, const RMessagePtr2* /*aMessage*/ )
	{
	return KErrNone;
	}

//
//
//


//
//                                                                                                               //
//                                    Implementation of class CNetConRequestTestBase                             //
//                                                                                                               //
//
//
//
//

CNetConRequestTestBase::CNetConRequestTestBase()
	: iExpectedError(KErrNone), iAvailableBearers(ECommDbBearerUnknown), iSecondPhaseAvailability(ETrue),
	  iDialogBoxPref(ECommDbDialogPrefUnknown), iDialogBoxError(KErrNone)
	{}

void CNetConRequestTestBase::ConstructL()
	{

	TCallBack bearerCallBack(BearerCb, this);
	iBearerCb = new(ELeave) CAsyncCallBack(bearerCallBack, CActive::EPriorityStandard);

	TCallBack dialogCallBack(DialogCb, this);
	iDialogCb = new(ELeave) CAsyncCallBack(dialogCallBack, CActive::EPriorityStandard);
	}

CNetConRequestTestBase::~CNetConRequestTestBase()
	{

	delete iRequest;

	delete iAgent;

	delete iDialogCb;

	delete iBearerCb;
	}

CNetConDlgProcessor* CNetConRequestTestBase::NewDialogProcessorL()
	{
	// Construct a tear-off interface
	CNetConDlgProcessor *processor = CNetConDlgProcessor::NewL( this, EFalse );
	return processor;
	}

TInt CNetConRequestTestBase::RequestSecondPhaseAvailability()
	{

	if (iSecondPhaseAvailability)
		{
		return KErrNone;
		}
	else
		{
		return KErrNetConInadequateSignalStrengh;
		}
	}

TInt CNetConRequestTestBase::BearerCb(TAny* aThisPtr)
	{
	CNetConRequestTestBase* self = static_cast<CNetConRequestTestBase*>(aThisPtr);
	self->BearerCbComplete();
	return KErrNone;
	}

void CNetConRequestTestBase::BearerCbComplete()
	{

	iRequest->SetAvailableBearers(iAvailableBearers);
	iAvailableBearers = ECommDbBearerUnknown;
	}

TInt CNetConRequestTestBase::DialogCb(TAny* aThisPtr)
	{
	CNetConRequestTestBase* self = static_cast<CNetConRequestTestBase*>(aThisPtr);
	self->DialogCbComplete();
	return KErrNone;
	}

void CNetConRequestTestBase::StartTestL()
	{

	iRequest->StartRequest();
	}

void CNetConRequestTestBase::GetDefaultTestSettings(TConnectionSettings& aSettings)
	{

	aSettings.iRank = 1;
	aSettings.iDirection = ECommDbConnectionDirectionOutgoing;
	aSettings.iDialogPref = iDialogBoxPref;
	aSettings.iBearerSet = KCommDbBearerCSD;
	aSettings.iIAPId = 1;
	aSettings.iServiceId = 1;
	aSettings.iServiceType = TPtrC(DIAL_OUT_ISP);
	aSettings.iBearerId = 2;
	aSettings.iLocationId = 2;
	aSettings.iChargeCardId = 0;
	}

//
// from MNetConDialogProcAccess
void CNetConRequestTestBase::SelectConnection(MDialogProcessorObserver&, const TConnectionPrefs&)
	{

	Unimplemented();
	}

void CNetConRequestTestBase::SelectConnection(MDialogProcessorObserver&, const TConnectionPrefs&, TInt)
	{

	Unimplemented();
	}

void CNetConRequestTestBase::SelectModemAndLocation(MDialogProcessorObserver&)
	{

	Unimplemented();
	}

void CNetConRequestTestBase::WarnNewConnection(MDialogProcessorObserver&, const TConnectionPrefs&, const TDesC*, const TIspConnectionNames*, TInt)
	{

	Unimplemented();
	}

void CNetConRequestTestBase::Reconnect(MDialogProcessorObserver&)
	{

	Unimplemented();
	}

void CNetConRequestTestBase::CancelEverything()
	{

	Unimplemented();
	}

//
// from MNetworkControllerObserver
void CNetConRequestTestBase::SelectComplete(const TDesC&)
	{

	Unimplemented();
	}

void CNetConRequestTestBase::SelectComplete(TInt)
	{

	Unimplemented();
	}

void CNetConRequestTestBase::ReconnectComplete(TInt)
	{

	Unimplemented();
	}
//
//
//


//
//                                                                                                               //
//                                    Implementation of class CNetConReconnectRequestTestBase                    //
//                                                                                                               //
//
//
//
//
CNetConReconnectRequestTestBase::CNetConReconnectRequestTestBase()
	{}

void CNetConReconnectRequestTestBase::ConstructL()
	{

	CNetConRequestTestBase::ConstructL();

	iAgent = new(ELeave) CStubbedAgent();

	TConnectionSettings settings;
	GetDefaultTestSettings(settings);
	iAgent->SetConnectionSettingsL(settings);

	iRequest = CReconnectRequest::NewL(this, this, iAgent, NULL);
	}

CNetConReconnectRequestTestBase::~CNetConReconnectRequestTestBase()
	{ }

void CNetConReconnectRequestTestBase::CheckBearerAvailability(TBool aIsReconnect)
	{

	ASSERT(aIsReconnect);
	(void)aIsReconnect;

	iBearerCb->CallBack();
	}

void CNetConReconnectRequestTestBase::Reconnect(MDialogProcessorObserver& aObserver)
	{

	ASSERT(&aObserver == iRequest);

	iDialogBoxObserver = &aObserver;
	iDialogCb->CallBack();
	}

void CNetConReconnectRequestTestBase::DialogCbComplete()
	{

	iDialogBoxObserver->MDPOReconnectComplete(iDialogBoxError);
	iDialogBoxObserver = NULL;
	iDialogBoxError = KErrNone;
	}

void CNetConReconnectRequestTestBase::RequestComplete(const CReconnectRequest* aRequest, TInt aError)
	{

	ASSERT(static_cast<const CNetConRequestBase*>(aRequest) == iRequest);
	(void)aRequest;

	if(aError == iExpectedError)
		CompleteTest(MNetConTest::EPassed);
	else
		CompleteTest(MNetConTest::EFailed);
	}
//
//
//


//
//                                                                                                               //
//                                    Implementation of class CNetConSelectionRequestTestBase                    //
//                                                                                                               //
//
//
//
//
CNetConSelectionRequestTestBase::CNetConSelectionRequestTestBase()
	:iConnectType(EConnStartExplicit),
	 iConnectionAttempt(1),
	 iLastConnectionError(KErrNone),
	 iNetworkId(0)
	{}

void CNetConSelectionRequestTestBase::ConstructL()
	{

	CNetConRequestTestBase::ConstructL();

	TCommDbConnPref prefs;
	iRequest = CSelectionRequest::NewL(this, this, iConnectType, prefs, iConnectionAttempt, iLastConnectionError);
	}

CNetConSelectionRequestTestBase::~CNetConSelectionRequestTestBase()
	{ }

TInt CNetConSelectionRequestTestBase::GetConnectionAttempts()
	{

	return 2;
	}

void CNetConSelectionRequestTestBase::GetCurrentSettingsL(TConnectionSettings& aSettings, TCommDbConnectionDirection aDirection, TUint32 aRank)
	{

	if(aRank!=(TUint32)iConnectionAttempt)
		{
		CompleteTest(MNetConTest::EFailed);
		return;
		}

	GetDefaultTestSettings(iSettings);
	iSettings.iDirection = aDirection;
	iSettings.iRank = aRank;
	aSettings = iSettings;
	}

void CNetConSelectionRequestTestBase::SetCurrentSettingsL(const TConnectionSettings& aSettings)
	{

	TBool settingsOk(ETrue);

	if (aSettings.iRank != iSettings.iRank)
		settingsOk = EFalse;

	if (aSettings.iDirection != iSettings.iDirection)
		settingsOk = EFalse;

	if (aSettings.iDialogPref != iSettings.iDialogPref)
		settingsOk = EFalse;

	if (aSettings.iBearerSet != iSettings.iBearerSet)
		settingsOk = EFalse;

	if (aSettings.iIAPId != iSettings.iIAPId)
		settingsOk = EFalse;

	if (aSettings.iServiceId != iSettings.iServiceId)
		settingsOk = EFalse;

	if (aSettings.iServiceType != iSettings.iServiceType)
		settingsOk = EFalse;

	if (aSettings.iBearerId != iSettings.iBearerId)
		settingsOk = EFalse;

	if (aSettings.iLocationId != iSettings.iLocationId)
		settingsOk = EFalse;

	if (aSettings.iChargeCardId != iSettings.iChargeCardId)
		settingsOk = EFalse;

	if(!settingsOk)
		CompleteTest(MNetConTest::EFailed);
	}

void CNetConSelectionRequestTestBase::SetOverridesL(CCommDbOverrideSettings* aOverrides)
	{

	if(aOverrides)
		{
		User::Invariant();
		}
	}

void CNetConSelectionRequestTestBase::GetPreferedIapL(TUint32& aIapId, TCommDbConnectionDirection /*aDirection*/, TUint32 /*aRank*/)
	{

	// default to IAP 1
	aIapId = 1;
	}

void CNetConSelectionRequestTestBase::SetPreferedIapL(TUint32& /*aIapId*/, TCommDbConnectionDirection /*aDirection*/, TUint32 /*aRank*/)
	{
	// This should not get called as DoesIapExistL returns true
	Unimplemented();
	}


TBool CNetConSelectionRequestTestBase::DoesIapExistL(TUint32 /*aIapId*/)
	{

	return ETrue;
	}

TBool CNetConSelectionRequestTestBase::IsTelNumLengthZeroForRasConnectionL(TConnectionSettings&)
	{

	return EFalse;
	}

void CNetConSelectionRequestTestBase::GetServiceSettingsL(TConnectionSettings& aSettings)
	{

	aSettings.iServiceId = iSettings.iServiceId;
	aSettings.iServiceType = iSettings.iServiceType;
	}

void CNetConSelectionRequestTestBase::GetDesL(const TDesC& aTable, const TDesC& aField, TDes16& aValue)
	{

	if (aTable == TPtrC(IAP) && aField == TPtrC(COMMDB_NAME))
		{
		_LIT(KIapName, "An Internet Access Point");
		aValue = KIapName();
		}
	else if(aField == TPtrC(AGENT_NAME))
		{
		_LIT(KAgentName, "StubbedAgent");
		aValue = KAgentName();
		}
	else
		{
		CNetConRequestTestBase::GetDesL( aTable, aField, aValue );
		}
	}

void CNetConSelectionRequestTestBase::GetIntL(const TDesC& aTable, const TDesC& aField, TUint32& aValue)
	{

	if(aTable == TPtrC(IAP) && aField == TPtrC(IAP_NETWORK))
		{
		aValue = iNetworkId;
		}
	else
		{
		CNetConRequestTestBase::GetIntL( aTable, aField, aValue );
		}
	}

TInt CNetConSelectionRequestTestBase::FindExistingAgentForSelection(CNifAgentBase*&, CCommsDbAccess*) const
	{

	return KErrNotFound;
	}

void CNetConSelectionRequestTestBase::CheckBearerAvailability(TBool aIsReconnect)
	{

	ASSERT(!aIsReconnect);
	(void)aIsReconnect;

	iBearerCb->CallBack();
	}

void CNetConSelectionRequestTestBase::AddAgentToNetworkL(CNifAgentBase* aAgent, TUint32 aNetworkId)
	{

	if(aNetworkId != iNetworkId || !aAgent)
		{
		CompleteTest(MNetConTest::EFailed);
		return;
		}

	ASSERT(!iAgent);
	iAgent = aAgent;

	CleanupStack::Pop();  // CStubbedAgent allocated in Nif::CreateAgentL()
	}

void CNetConSelectionRequestTestBase::DialogCbComplete()
	{

	Unimplemented();
	}

void CNetConSelectionRequestTestBase::SelectConnection(MDialogProcessorObserver& aObserver, const TConnectionPrefs& aPrefs)
	{

	SelectConnection(aObserver, aPrefs, KErrNone);
	}

void CNetConSelectionRequestTestBase::SelectConnection(MDialogProcessorObserver& aObserver, const TConnectionPrefs&, TInt aLastConnectionError)
	{

	ASSERT(&aObserver == iRequest);
	ASSERT(!iDialogBoxObserver);

	if(aLastConnectionError != iLastConnectionError)
		{
		CompleteTest(MNetConTest::EFailed);
		return;
		}

	iDialogBoxAction = ESelectConnection;
	iDialogBoxObserver = &aObserver;
	iDialogCb->CallBack();
	}

void CNetConSelectionRequestTestBase::SelectModemAndLocation(MDialogProcessorObserver& aObserver)
	{

	ASSERT(&aObserver == iRequest);
	ASSERT(!iDialogBoxObserver);

	iDialogBoxAction = ESelectModemLocation;
	iDialogBoxObserver = &aObserver;
	iDialogCb->CallBack();
	}

void CNetConSelectionRequestTestBase::WarnNewConnection(MDialogProcessorObserver& aObserver, const TConnectionPrefs&, const TDesC*, const TIspConnectionNames*, TInt aLastConnectionError)
	{

	ASSERT(&aObserver == iRequest);
	ASSERT(!iDialogBoxObserver);

	if(aLastConnectionError != iLastConnectionError)
		{
		CompleteTest(MNetConTest::EFailed);
		return;
		}

	iDialogBoxAction = EWarnNewConnection;
	iDialogBoxObserver = &aObserver;
	iDialogCb->CallBack();
	}

void CNetConSelectionRequestTestBase::RequestComplete(const CSelectionRequest* aRequest, TInt aError)
	{

	ASSERT(static_cast<const CNetConRequestBase*>(aRequest) == iRequest);
	(void)aRequest;

	if(aError == iExpectedError)
		CompleteTest(MNetConTest::EPassed);
	else
		CompleteTest(MNetConTest::EFailed);
	}
//
//
//


//
//                                                                                                               //
//                                    Implementation of class CNetConBearerTestBase                              //
//                                                                                                               //
//
//
//
//
CNetConBearerTestBase::~CNetConBearerTestBase()
	{
	delete iTelBearer;
	iTelBearer = 0;
	delete iDatabase;
	iDatabase = 0;
	}

CNetConBearerTestBase::CNetConBearerTestBase()
	:iTelBearer(0),
	 iDatabase(0)
	{
	}

void CNetConBearerTestBase::ConstructL()
	{
	ASSERT( !iDatabase );
	ASSERT( !iTelBearer );
	iTelBearer = CTelBearer::NewL( this );
	iDatabase = CCommsDbAccess::NewL(this);
	}

void CNetConBearerTestBase::BearerStateChange(CBearerBase*)
	{
	Unimplemented();
	}

CCommsDbAccess *CNetConBearerTestBase::DbAccess()
	{
	ASSERT( iDatabase ); // in case someone forgot to call ConstructL from a derived class...
	return iDatabase;
	}
//
//
//

