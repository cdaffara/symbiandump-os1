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

#include "SelectionTests.h"
#include "CStubbedAgent.h"

//
//  Test case NC-2-1
//

CNetConTest0201* CNetConTest0201::NewLC()
	{
	CNetConTest0201* self = new(ELeave) CNetConTest0201();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0201::CNetConTest0201()
	{
	
	iConnectType = EConnStartExplicit;
	iConnectionAttempt = 1;
	iLastConnectionError = KErrNone;
	iAvailableBearers = KCommDbBearerCSD;
	iDialogBoxPref = ECommDbDialogPrefDoNotPrompt;
	iDialogBoxError = KErrNone;
	iExpectedError = KErrNone;
	}

void CNetConTest0201::ConstructL()
	{

	CNetConSelectionRequestTestBase::ConstructL();

	iAgent = new(ELeave) CStubbedAgent();
	}

CNetConTest0201::~CNetConTest0201()
	{ }

const TDesC& CNetConTest0201::Name() const
	{

	_LIT(KTestName, "Test Case NC-2-1");
	return KTestName();
	}

TInt CNetConTest0201::FindExistingAgentForSelection(CNifAgentBase*& aAgent, CCommsDbAccess*) const
	{

	ASSERT(iAgent);
	aAgent = iAgent;
	return KErrNone;
	}

void CNetConTest0201::RequestComplete(const CSelectionRequest* aRequest, TInt aError)
	{

	ASSERT((CNetConRequestBase*)aRequest == iRequest);

	if(aError!=iExpectedError)
		{
		CompleteTest(MNetConTest::EFailed);
		return;
		}
	
	ASSERT(iAgent);

	TNifAgentInfo info;
	iAgent->Info(info);

	if(aRequest->AgentName() == info.iName)
		{
		CompleteTest(MNetConTest::EPassed);
		}
	else
		{
		CompleteTest(MNetConTest::EFailed);
		}
	}


//
//  Test case NC-2-2
//

CNetConTest0202* CNetConTest0202::NewLC()
	{
	CNetConTest0202* self = new(ELeave) CNetConTest0202();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0202::CNetConTest0202()
	{
	
	iConnectType = EConnStartExplicit;
	iConnectionAttempt = 3;
	iLastConnectionError = KErrNone;
	iAvailableBearers = KCommDbBearerCSD;
	iDialogBoxPref = ECommDbDialogPrefDoNotPrompt;
	iDialogBoxError = KErrNone;
	iExpectedError = KErrOverflow;
	}

const TDesC& CNetConTest0202::Name() const
	{

	_LIT(KTestName, "Test Case NC-2-2");
	return KTestName();
	}


//
//  Test case NC-2-3
//

CNetConTest0203* CNetConTest0203::NewLC()
	{
	CNetConTest0203* self = new(ELeave) CNetConTest0203();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0203::CNetConTest0203()
	{
	
	iConnectType = EConnStartImplicit;
	iConnectionAttempt = 1;
	iLastConnectionError = KErrNone;
	iAvailableBearers = KCommDbBearerCSD;
	iDialogBoxPref = ECommDbDialogPrefDoNotPrompt;
	iDialogBoxError = KErrNone;
	iExpectedError = KErrNone;
	}

void CNetConTest0203::ConstructL()
	{

	CNetConSelectionRequestTestBase::ConstructL();
	
	iAgent = new(ELeave) CStubbedAgent();
	
	TNifAgentInfo info;
	iAgent->Info(info);

	iImplicitAgentName = info.iName.AllocL();
	}

CNetConTest0203::~CNetConTest0203()
	{

	if(iImplicitAgentName)
		delete iImplicitAgentName;
	}

const TDesC& CNetConTest0203::Name() const
	{

	_LIT(KTestName, "Test Case NC-2-3");
	return KTestName();
	}

TInt CNetConTest0203::FindExistingAgentForSelection(CNifAgentBase*& aAgent, CCommsDbAccess*) const
	{

	ASSERT(iAgent);
	aAgent = iAgent;
	return KErrNone;
	}

const HBufC* CNetConTest0203::ImplicitConnectionAgentName() const
	{

	return iImplicitAgentName;
	}

const TConnPref& CNetConTest0203::ImplicitConnectionPrefs() const
	{

	return iImplicitConnPref;
	}

void CNetConTest0203::RequestComplete(const CSelectionRequest* aRequest, TInt aError)
	{

	ASSERT((CNetConRequestBase*)aRequest == iRequest);

	if(aError!=iExpectedError)
		{
		CompleteTest(MNetConTest::EFailed);
		return;
		}
	
	ASSERT(iAgent);

	TNifAgentInfo info;
	iAgent->Info(info);

	if(aRequest->AgentName() == info.iName)
		{
		CompleteTest(MNetConTest::EPassed);
		}
	else
		{
		CompleteTest(MNetConTest::EFailed);
		}
	}


//
//  Test case NC-2-4
//

CNetConTest0204* CNetConTest0204::NewLC()
	{
	CNetConTest0204* self = new(ELeave) CNetConTest0204();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0204::CNetConTest0204()
	{
	
	iConnectType = EConnStartExplicit;
	iConnectionAttempt = 1;
	iLastConnectionError = KErrNone;
	iAvailableBearers = KCommDbBearerCSD;
	iDialogBoxPref = ECommDbDialogPrefDoNotPrompt;
	iDialogBoxError = KErrNone;
	iExpectedError = KErrNone;
	iNetworkId = 204;
	}

CNetConTest0204::~CNetConTest0204()
	{}

const TDesC& CNetConTest0204::Name() const
	{

	_LIT(KTestName, "Test Case NC-2-4");
	return KTestName();
	}

void CNetConTest0204::RequestComplete(const CSelectionRequest* aRequest, TInt aError)
	{

	ASSERT((CNetConRequestBase*)aRequest == iRequest);


	if(aError!=iExpectedError)
		{
		CompleteTest(MNetConTest::EFailed);
		return;
		}
	
	ASSERT(iAgent);

	TNifAgentInfo info;
	iAgent->Info(info);

	if(aRequest->AgentName() == info.iName)
		{
		CompleteTest(MNetConTest::EPassed);
		}
	else
		{
		CompleteTest(MNetConTest::EFailed);
		}
	}


//
//  Test case NC-2-5
//

CNetConTest0205* CNetConTest0205::NewLC()
	{
	CNetConTest0205* self = new(ELeave) CNetConTest0205();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0205::CNetConTest0205()
	{
	
	iConnectType = EConnStartExplicit;
	iConnectionAttempt = 1;
	iLastConnectionError = KErrNone;
	iAvailableBearers = KCommDbBearerCSD;
	iDialogBoxPref = ECommDbDialogPrefDoNotPrompt;
	iDialogBoxError = KErrNone;
	iExpectedError = KErrNone;
	iNetworkId = 205;
	}

CNetConTest0205::~CNetConTest0205()
	{ }

const TDesC& CNetConTest0205::Name() const
	{

	_LIT(KTestName, "Test Case NC-2-5");
	return KTestName();
	}

void CNetConTest0205::ConstructL()
//
// Override ConstructL() in CNetConSelectionRequestTestBase so that we can
// pass in some actual connection preferences.
//
	{
	CNetConRequestTestBase::ConstructL();

	TCommDbConnPref prefs;
	prefs.SetDirection(ECommDbConnectionDirectionIncoming);

	iRequest = CSelectionRequest::NewL(this, this, iConnectType, prefs, iConnectionAttempt, iLastConnectionError);
	}

void CNetConTest0205::RequestComplete(const CSelectionRequest* aRequest, TInt aError)
	{

	ASSERT((CNetConRequestBase*)aRequest == iRequest);

	if(aError!=iExpectedError)
		{
		CompleteTest(MNetConTest::EFailed);
		return;
		}

	ASSERT(iAgent);
	
	TNifAgentInfo info;
	iAgent->Info(info);

	if(aRequest->AgentName() == info.iName)
		{
		CompleteTest(MNetConTest::EPassed);
		}
	else
		{
		CompleteTest(MNetConTest::EFailed);
		}
	}

void CNetConTest0205::GetCurrentSettingsL(TConnectionSettings& aSettings, TCommDbConnectionDirection aDirection, TUint32 aRank)
	{

	if(aDirection!=ECommDbConnectionDirectionIncoming)
		{
		CompleteTest(MNetConTest::EFailed);
		return;
		}

	if(aRank!=(TUint32)iConnectionAttempt)
		{
		CompleteTest(MNetConTest::EFailed);
		return;
		}

	iSettings.iRank = aRank;
	iSettings.iDirection = aDirection;
	iSettings.iDialogPref = ECommDbDialogPrefDoNotPrompt;
	iSettings.iBearerSet = KCommDbBearerCSD;
	iSettings.iIAPId = 5;
	iSettings.iServiceId = 2;
	iSettings.iServiceType = TPtrC(DIAL_IN_ISP);
	iSettings.iBearerId = 2;
	iSettings.iLocationId = 2;
	iSettings.iChargeCardId = 0;

	aSettings = iSettings;
	}


//
//  Test case NC-2-6
//

CNetConTest0206* CNetConTest0206::NewLC()
	{
	CNetConTest0206* self = new(ELeave) CNetConTest0206();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0206::CNetConTest0206()
	{
	
	iConnectType = EConnStartExplicit;
	iConnectionAttempt = 1;
	iLastConnectionError = KErrNone;
	iAvailableBearers = KCommDbBearerCSD;
	iDialogBoxPref = ECommDbDialogPrefDoNotPrompt;
	iDialogBoxError = KErrNone;
	iExpectedError = KErrNone;
	iNetworkId = 206;
	}

CNetConTest0206::~CNetConTest0206()
	{ }

const TDesC& CNetConTest0206::Name() const
	{

	_LIT(KTestName, "Test Case NC-2-6");
	return KTestName();
	}

void CNetConTest0206::RequestComplete(const CSelectionRequest* aRequest, TInt aError)
	{

	ASSERT((CNetConRequestBase*)aRequest == iRequest);

	if(aError!=iExpectedError)
		{
		CompleteTest(MNetConTest::EFailed);
		return;
		}
	
	ASSERT(iAgent);

	TNifAgentInfo info;
	iAgent->Info(info);

	if(aRequest->AgentName() == info.iName)
		{
		CompleteTest(MNetConTest::EPassed);
		}
	else
		{
		CompleteTest(MNetConTest::EFailed);
		}
	}


//
//  Test case NC-2-7
//	Removed as its not easy after a user prompt to check for 
//  Bearer availablility


//
//  Test case NC-2-8
//

CNetConTest0208* CNetConTest0208::NewLC()
	{
	CNetConTest0208* self = new(ELeave) CNetConTest0208();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0208::CNetConTest0208()
	{
	
	iConnectType = EConnStartExplicit;
	iConnectionAttempt = 1;
	iLastConnectionError = KErrNone;
	iAvailableBearers = KCommDbBearerCSD;
	iDialogBoxPref = ECommDbDialogPrefPrompt;
	iDialogBoxError = KErrNone;
	iExpectedError = KErrNone;
	iNetworkId = 208;
	}

CNetConTest0208::~CNetConTest0208()
	{ }

const TDesC& CNetConTest0208::Name() const
	{

	_LIT(KTestName, "Test Case NC-2-8");
	return KTestName();
	}

void CNetConTest0208::DialogCbComplete()
	{

	ASSERT(iDialogBoxObserver);

	switch(iDialogBoxAction)
		{
		case ESelectModemLocation:
			iDialogBoxObserver->MDPOSelectModemAndLocationComplete(iDialogBoxError, iSettings);
			break;

		case ESelectConnection:
			iDialogBoxObserver->MDPOSelectComplete(iDialogBoxError, iSettings);
			break;

		default:
			User::Invariant();
		}
	
	iDialogBoxObserver = NULL;
	iDialogBoxError = KErrNone;
	}

void CNetConTest0208::RequestComplete(const CSelectionRequest* aRequest, TInt aError)
	{

	ASSERT((CNetConRequestBase*)aRequest == iRequest);

	
	if(aError!=iExpectedError)
		{
		CompleteTest(MNetConTest::EFailed);
		return;
		}
	
	ASSERT(iAgent);

	TNifAgentInfo info;
	iAgent->Info(info);

	if(aRequest->AgentName() == info.iName)
		{
		CompleteTest(MNetConTest::EPassed);
		}
	else
		{
		CompleteTest(MNetConTest::EFailed);
		};
	}


//
//  Test case NC-2-9
//

CNetConTest0209* CNetConTest0209::NewLC()
	{
	CNetConTest0209* self = new(ELeave) CNetConTest0209();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0209::CNetConTest0209()
	{
	
	iConnectType = EConnStartExplicit;
	iConnectionAttempt = 1;
	iLastConnectionError = KErrNone;
	iAvailableBearers = KCommDbBearerCSD;
	iDialogBoxPref = ECommDbDialogPrefWarn;
	iDialogBoxError = KErrNone;
	iExpectedError = KErrNone;
	iNetworkId = 209;
	}

CNetConTest0209::~CNetConTest0209()
	{ }

const TDesC& CNetConTest0209::Name() const
	{

	_LIT(KTestName, "Test Case NC-2-9");
	return KTestName();
	}

void CNetConTest0209::DialogCbComplete()
	{

	ASSERT(iDialogBoxObserver);
	ASSERT(iDialogBoxAction == EWarnNewConnection);

	TBool response;
	if(iDialogBoxError == KErrNone)
		response = ETrue;
	else
		response = EFalse;

	iDialogBoxObserver->MDPOWarnComplete(iDialogBoxError, response);
	iDialogBoxObserver = NULL;
	iDialogBoxError = KErrNone;
	}

void CNetConTest0209::RequestComplete(const CSelectionRequest* aRequest, TInt aError)
	{

	ASSERT((CNetConRequestBase*)aRequest == iRequest);
	
	if(aError!=iExpectedError)
		{
		CompleteTest(MNetConTest::EFailed);
		return;
		}

	TNifAgentInfo info;
	iAgent->Info(info);

 	if(aRequest->AgentName() == info.iName)
		{
		CompleteTest(MNetConTest::EPassed);
		}
	else
		{
		CompleteTest(MNetConTest::EFailed);
		}
	}

//
//  Test case NC-2-10
//

CNetConTest0210* CNetConTest0210::NewLC()
	{
	CNetConTest0210* self = new(ELeave) CNetConTest0210();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0210::CNetConTest0210()
	{
	
	iConnectType = EConnStartExplicit;
	iConnectionAttempt = 1;
	iLastConnectionError = KErrNone;
	iAvailableBearers = KCommDbBearerCSD;
	iDialogBoxPref = ECommDbDialogPrefWarn;
	iDialogBoxError = KErrCancel;
	iExpectedError = KErrCancel;
	iNetworkId = 210;
	}

CNetConTest0210::~CNetConTest0210()
	{ }

const TDesC& CNetConTest0210::Name() const
	{

	_LIT(KTestName, "Test Case NC-2-10");
	return KTestName();
	}

void CNetConTest0210::DialogCbComplete()
	{

	ASSERT(iDialogBoxObserver);
	ASSERT(iDialogBoxAction == EWarnNewConnection);

	TBool response;
	if(iDialogBoxError == KErrNone)
		response = ETrue;
	else
		response = EFalse;
	
	iDialogBoxObserver->MDPOWarnComplete(iDialogBoxError, response);
	iDialogBoxObserver = NULL;
	iDialogBoxError = KErrNone;
	}


//
//  Test case NC-2-11
//

CNetConTest0211* CNetConTest0211::NewLC()
	{
	CNetConTest0211* self = new(ELeave) CNetConTest0211();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0211::CNetConTest0211()
	{
	
	iConnectType = EConnStartExplicit;
	iConnectionAttempt = 1;
	iLastConnectionError = KErrNone;
	iDialogBoxPref = ECommDbDialogPrefPromptIfWrongMode;
	iDialogBoxError = KErrNone;
	iExpectedError = KErrNone;
	iNetworkId = 211;
	}

CNetConTest0211::~CNetConTest0211()
	{ }

const TDesC& CNetConTest0211::Name() const
	{

	_LIT(KTestName, "Test Case NC-2-11");
	return KTestName();
	}

void CNetConTest0211::GetCurrentSettingsL(TConnectionSettings& aSettings, TCommDbConnectionDirection aDirection, TUint32 aRank)
	{

	if(aRank!=(TUint32)iConnectionAttempt)
		{
		CompleteTest(MNetConTest::EFailed);
		return;
		}

	iSettings.iRank = aRank;
	iSettings.iDirection = aDirection;
	iSettings.iDialogPref = ECommDbDialogPrefPromptIfWrongMode;
	iSettings.iIAPId = 5;
	iSettings.iServiceId = 2;
	iSettings.iBearerId = 2;
	iSettings.iLocationId = 2;
	iSettings.iChargeCardId = 0;

	aSettings = iSettings;
	}

RMobilePhone::TMobilePhoneNetworkMode CNetConTest0211::NetworkMode() const
	{
	}

void CNetConTest0211::DialogCbComplete()
	{

	ASSERT(iDialogBoxObserver);
	ASSERT(iDialogBoxAction == ESelectConnection);

	iDialogBoxObserver->MDPOSelectComplete(iDialogBoxError, iSettings);
	iDialogBoxObserver = NULL;
	iDialogBoxError = KErrNone;
	}


//
//  Test case NC-2-12
//

CNetConTest0212* CNetConTest0212::NewLC()
	{
	CNetConTest0212* self = new(ELeave) CNetConTest0212();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0212::CNetConTest0212()
	{
	
	iConnectType = EConnStartExplicit;
	iConnectionAttempt = 1;
	iLastConnectionError = KErrNone;
	iDialogBoxPref = ECommDbDialogPrefPromptIfWrongMode;
	iDialogBoxError = KErrNone;
	iExpectedError = KErrNone;
	iNetworkId = 212;
	}

CNetConTest0212::~CNetConTest0212()
	{ }

const TDesC& CNetConTest0212::Name() const
	{

	_LIT(KTestName, "Test Case NC-2-12");
	return KTestName();
	}

void CNetConTest0212::GetCurrentSettingsL(TConnectionSettings& aSettings, TCommDbConnectionDirection aDirection, TUint32 aRank)
	{

	if(aRank!=(TUint32)iConnectionAttempt)
		{
		CompleteTest(MNetConTest::EFailed);
		return;
		}

	iSettings.iRank = aRank;
	iSettings.iDirection = aDirection;
	iSettings.iDialogPref = ECommDbDialogPrefPromptIfWrongMode;
	iSettings.iIAPId = 5;
	iSettings.iServiceId = 2;
	iSettings.iBearerId = 2;
	iSettings.iLocationId = 2;
	iSettings.iChargeCardId = 0;

	aSettings = iSettings;
	}

RMobilePhone::TMobilePhoneNetworkMode CNetConTest0212::NetworkMode() const
	{
	}

void CNetConTest0212::DialogCbComplete()
	{

	ASSERT(iDialogBoxObserver);
	ASSERT(iDialogBoxAction == ESelectConnection);

	iDialogBoxObserver->MDPOSelectComplete(iDialogBoxError, iSettings);
	iDialogBoxObserver = NULL;
	iDialogBoxError = KErrNone;
	}


//
//  Test case NC-2-13
//

CNetConTest0213* CNetConTest0213::NewLC()
	{
	CNetConTest0213* self = new(ELeave) CNetConTest0213();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0213::CNetConTest0213()
	{
	
	iConnectType = EConnStartExplicit;
	iConnectionAttempt = 1;
	iLastConnectionError = KErrNone;
	iAvailableBearers = KCommDbBearerCSD;
	iDialogBoxPref = ECommDbDialogPrefPromptIfWrongMode;
	iDialogBoxError = KErrNone;
	iExpectedError = KErrNone;
	iNetworkId = 213;
	}

CNetConTest0213::~CNetConTest0213()
	{ }

const TDesC& CNetConTest0213::Name() const
	{

	_LIT(KTestName, "Test Case NC-2-13");
	return KTestName();
	}

void CNetConTest0213::GetCurrentSettingsL(TConnectionSettings& aSettings, TCommDbConnectionDirection aDirection, TUint32 aRank)
	{

	if(aRank!=(TUint32)iConnectionAttempt)
		{
		CompleteTest(MNetConTest::EFailed);
		return;
		}

	iSettings.iRank = aRank;
	iSettings.iDirection = aDirection;
	iSettings.iDialogPref = ECommDbDialogPrefPromptIfWrongMode;
	iSettings.iIAPId = 5;
	iSettings.iServiceId = 2;
	iSettings.iBearerId = 2;
	iSettings.iLocationId = 2;
	iSettings.iChargeCardId = 0;

	aSettings = iSettings;
	}

RMobilePhone::TMobilePhoneNetworkMode CNetConTest0213::NetworkMode() const
	{
	}

void CNetConTest0213::DialogCbComplete()
	{

	ASSERT(iDialogBoxObserver);
	ASSERT(iDialogBoxAction == ESelectConnection);

	iDialogBoxObserver->MDPOSelectComplete(iDialogBoxError, iSettings);
	iDialogBoxObserver = NULL;
	iDialogBoxError = KErrNone;
	}


//
//  Test case NC-2-14
//

CNetConTest0214* CNetConTest0214::NewLC()
	{
	CNetConTest0214* self = new(ELeave) CNetConTest0214();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0214::CNetConTest0214()
	{
	
	iConnectType = EConnStartExplicit;
	iConnectionAttempt = 1;
	iLastConnectionError = KErrNone;
	iAvailableBearers = KCommDbBearerCSD;
	iDialogBoxPref = ECommDbDialogPrefDoNotPrompt;
	iDialogBoxError = KErrNone;
	iExpectedError = KErrNone;
	iNetworkId = 214;
	}

CNetConTest0214::~CNetConTest0214()
	{ }

const TDesC& CNetConTest0214::Name() const
	{

	_LIT(KTestName, "Test Case NC-2-14");
	return KTestName();
	}


//
//  Test case NC-2-15
//

CNetConTest0215* CNetConTest0215::NewLC()
	{
	CNetConTest0215* self = new(ELeave) CNetConTest0215();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0215::CNetConTest0215()
	{
	
	iConnectType = EConnStartExplicit;
	iConnectionAttempt = 1;
	iLastConnectionError = KErrNone;
	iAvailableBearers = KCommDbBearerCSD;
	iDialogBoxPref = ECommDbDialogPrefDoNotPrompt;
	iDialogBoxError = KErrNone;
	iExpectedError = KErrNone;
	}

void CNetConTest0215::ConstructL()
	{

	CNetConSelectionRequestTestBase::ConstructL();

	iAgent = new(ELeave) CStubbedAgent();
	}

CNetConTest0215::~CNetConTest0215()
	{ }

const TDesC& CNetConTest0215::Name() const
	{

	_LIT(KTestName, "Test Case NC-2-15");
	return KTestName();
	}

TInt CNetConTest0215::FindExistingAgentForSelection(CNifAgentBase*& aAgent, CCommsDbAccess*) const
	{

	ASSERT(iAgent);
	aAgent = iAgent;
	return KErrNone;
	}

void CNetConTest0215::RequestComplete(const CSelectionRequest* aRequest, TInt aError)
	{

	ASSERT((CNetConRequestBase*)aRequest == iRequest);

	if(aError!=iExpectedError)
		{
		CompleteTest(MNetConTest::EFailed);
		return;
		}
	
	TNifAgentInfo info;
	iAgent->Info(info);

	if(aRequest->AgentName() == info.iName)
		{
		CompleteTest(MNetConTest::EPassed);
		}
	else
		{
		CompleteTest(MNetConTest::EFailed);
		}
	}


//
//  Test case NC-2-16
//

CNetConTest0216* CNetConTest0216::NewLC()
	{
	CNetConTest0216* self = new(ELeave) CNetConTest0216();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0216::CNetConTest0216()
	{
	
	iConnectType = EConnStartExplicit;
	iConnectionAttempt = 1;
	iLastConnectionError = KErrNone;
	iAvailableBearers = KCommDbBearerCSD;
	iDialogBoxPref = ECommDbDialogPrefDoNotPrompt;
	iDialogBoxError = KErrNone;
	iExpectedError = KErrNone;
	iNetworkId = 216;
	}

CNetConTest0216::~CNetConTest0216()
	{}

const TDesC& CNetConTest0216::Name() const
	{

	_LIT(KTestName, "Test Case NC-2-16");
	return KTestName();
	}

void CNetConTest0216::RequestComplete(const CSelectionRequest* aRequest, TInt aError)
	{

	ASSERT((CNetConRequestBase*)aRequest == iRequest);

	if(aError!=iExpectedError)
		{
		CompleteTest(MNetConTest::EFailed);
		return;
		}
	
	TNifAgentInfo info;
	iAgent->Info(info);

	if(aRequest->AgentName() == info.iName)
		{
		CompleteTest(MNetConTest::EPassed);
		}
	else
		{
		CompleteTest(MNetConTest::EFailed);
		}
	}


//
//  Test case NC-2-17
//

CNetConTest0217* CNetConTest0217::NewLC()
	{
	CNetConTest0217* self = new(ELeave) CNetConTest0217();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0217::CNetConTest0217()
	{
	
	iConnectType = EConnStartExplicit;
	iConnectionAttempt = 1;
	iLastConnectionError = KErrNone;
	iAvailableBearers = KCommDbBearerCSD;
	iDialogBoxPref = ECommDbDialogPrefDoNotPrompt;
	iDialogBoxError = KErrNone;
	iExpectedError = KErrNone;
	iNetworkId = 217;
	}

CNetConTest0217::~CNetConTest0217()
	{}

const TDesC& CNetConTest0217::Name() const
	{

	_LIT(KTestName, "Test Case NC-2-17");
	return KTestName();
	}

void CNetConTest0217::RequestComplete(const CSelectionRequest* aRequest, TInt aError)
	{

	ASSERT((CNetConRequestBase*)aRequest == iRequest);

	if(aError!=iExpectedError)
		{
		CompleteTest(MNetConTest::EFailed);
		return;
		}
	
	TNifAgentInfo info;
	iAgent->Info(info);

	if(aRequest->AgentName() == info.iName)
		{
		CompleteTest(MNetConTest::EPassed);
		}
	else
		{
		CompleteTest(MNetConTest::EFailed);
		}
	}


//
//  Test case NC-2-18
//

CNetConTest0218* CNetConTest0218::NewLC()
	{
	CNetConTest0218* self = new(ELeave) CNetConTest0218();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0218::CNetConTest0218()
	{
	
	iConnectType = EConnStartExplicit;
	iConnectionAttempt = 1;
	iLastConnectionError = KErrNone;
	iAvailableBearers = KCommDbBearerCSD;
	iDialogBoxPref = ECommDbDialogPrefDoNotPrompt;
	iDialogBoxError = KErrNone;
	iExpectedError = KErrNotFound;
	iNetworkId = 218;
	}

CNetConTest0218::~CNetConTest0218()
	{}

const TDesC& CNetConTest0218::Name() const
	{

	_LIT(KTestName, "Test Case NC-2-18");
	return KTestName();
	}

void CNetConTest0218::GetDesL(const TDesC& aTable, const TDesC& aField, TDes16& aValue)
	{

	if(aField == TPtrC(AGENT_NAME))
		{
		_LIT(KAgentName, "MissingAgent");
		aValue = KAgentName();
		}
	else
		{
		CNetConSelectionRequestTestBase::GetDesL( aTable, aField, aValue );
		}
	}


//
//  Test case NC-2-19
//

CNetConTest0219* CNetConTest0219::NewLC()
	{
	CNetConTest0219* self = new(ELeave) CNetConTest0219();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0219::CNetConTest0219()
	{
	
	iConnectType = EConnStartExplicit;
	iConnectionAttempt = 1;
	iLastConnectionError = KErrNone;
	iAvailableBearers = (KCommDbBearerCSD | KCommDbBearerPSD);
	iDialogBoxPref = ECommDbDialogPrefPrompt;
	iDialogBoxError = KErrNone;
	iExpectedError = KErrNone;
	iNetworkId = 219;
	}

void CNetConTest0219::ConstructL()
//
// Override ConstructL() in CNetConSelectionRequestTestBase so that we can
// pass in some actual connection preferences.
//
	{
	CNetConRequestTestBase::ConstructL();

	TCommDbConnPref prefs;
	prefs.SetBearerSet(KCommDbBearerPSD);
	prefs.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
	prefs.SetDirection(ECommDbConnectionDirectionOutgoing);
	prefs.SetIapId(7);
	prefs.SetNetId(3);

	iRequest = CSelectionRequest::NewL(this, this, iConnectType, prefs, iConnectionAttempt, iLastConnectionError);
	}

CNetConTest0219::~CNetConTest0219()
	{}

const TDesC& CNetConTest0219::Name() const
	{

	_LIT(KTestName, "Test Case NC-2-19");
	return KTestName();
	}

void CNetConTest0219::SetOverridesL(CCommDbOverrideSettings* aOverrides)
	{
	
	if(!aOverrides)
		return;

	// check that the overrides match our preferences
	
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref overPrefs;

	overPrefs.iDirection = ECommDbConnectionDirectionOutgoing;
	overPrefs.iRanking = iConnectionAttempt;
	TRAPD(err, aOverrides->GetConnectionPreferenceOverride(overPrefs));
	ASSERT(err==KErrNone);

	if((overPrefs.iBearer.iBearerSet != KCommDbBearerPSD) ||
		(overPrefs.iBearer.iIapId != 7) ||
		(overPrefs.iDialogPref != ECommDbDialogPrefDoNotPrompt))
		{
		CompleteTest(MNetConTest::EFailed);
		return;
		}
	
	// update settings to reflect overrides
	iSettings.iRank = iConnectionAttempt;
	iSettings.iDirection = ECommDbConnectionDirectionOutgoing;
	iSettings.iDialogPref = ECommDbDialogPrefDoNotPrompt;
	iDialogBoxPref = ECommDbDialogPrefDoNotPrompt;
	iSettings.iBearerSet = KCommDbBearerPSD;
	iSettings.iIAPId = 7;
	}

void CNetConTest0219::DialogCbComplete()
	{

	// this should not be called since the preferences say Do Not Prompt
	CompleteTest(MNetConTest::EFailed);
	}

void CNetConTest0219::RequestComplete(const CSelectionRequest* aRequest, TInt aError)
	{

	ASSERT((CNetConRequestBase*)aRequest == iRequest);

	if(aError!=iExpectedError)
		{
		CompleteTest(MNetConTest::EFailed);
		return;
		}
	
	ASSERT(iAgent);

	TNifAgentInfo info;
	iAgent->Info(info);

	if(aRequest->AgentName() == info.iName)
		{
		CompleteTest(MNetConTest::EPassed);
		}
	else
		{
		CompleteTest(MNetConTest::EFailed);
		}
	}


//
//  Test case NC-2-20
//

CNetConTest0220* CNetConTest0220::NewLC()
	{
	CNetConTest0220* self = new(ELeave) CNetConTest0220();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0220::CNetConTest0220()
	{
	
	iConnectType = EConnStartExplicit;
	iConnectionAttempt = 1;
	iLastConnectionError = KErrNone;
	iAvailableBearers = (KCommDbBearerCSD | KCommDbBearerPSD);
	iDialogBoxPref = ECommDbDialogPrefPrompt;
	iDialogBoxError = KErrNone;
	iExpectedError = KErrNone;
	iNetworkId = 220;
	}

void CNetConTest0220::ConstructL()
//
// Override ConstructL() in CNetConSelectionRequestTestBase so that we can
// pass in some actual connection preferences.
//
	{
	CNetConRequestTestBase::ConstructL();

	TCommDbConnPref prefs;
	prefs.SetDialogPreference(ECommDbDialogPrefWarn);

	iRequest = CSelectionRequest::NewL(this, this, iConnectType, prefs, iConnectionAttempt, iLastConnectionError);
	}

CNetConTest0220::~CNetConTest0220()
	{}

const TDesC& CNetConTest0220::Name() const
	{

	_LIT(KTestName, "Test Case NC-2-20");
	return KTestName();
	}

void CNetConTest0220::SetOverridesL(CCommDbOverrideSettings* aOverrides)
	{
	
	if(!aOverrides)
		return;

	// check that the overrides match our preferences
	
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref overPrefs;

	overPrefs.iDirection = ECommDbConnectionDirectionOutgoing;
	overPrefs.iRanking = iConnectionAttempt;
	TRAPD(err, aOverrides->GetConnectionPreferenceOverride(overPrefs));
	ASSERT(err==KErrNone);

	if((overPrefs.iDialogPref != ECommDbDialogPrefWarn) || 
		(overPrefs.iBearer.iIapId != iSettings.iIAPId) ||
		(overPrefs.iBearer.iBearerSet != iSettings.iBearerSet))
		{
		CompleteTest(MNetConTest::EFailed);
		return;
		}
	
	// update settings to reflect overrides
	iDialogBoxPref = ECommDbDialogPrefWarn;
	}

void CNetConTest0220::SelectConnection(MDialogProcessorObserver&, const TConnectionPrefs&)
	{
	CompleteTest(MNetConTest::EFailed);
	}

void CNetConTest0220::SelectConnection(MDialogProcessorObserver&, const TConnectionPrefs&, TInt)
	{
	CompleteTest(MNetConTest::EFailed);
	}

void CNetConTest0220::SelectModemAndLocation(MDialogProcessorObserver&)
	{
	CompleteTest(MNetConTest::EFailed);
	}

void CNetConTest0220::DialogCbComplete()
	{

	ASSERT(iDialogBoxObserver);
	ASSERT(iDialogBoxAction == EWarnNewConnection);

	TBool response;
	if(iDialogBoxError == KErrNone)
		response = ETrue;
	else
		response = EFalse;

	iDialogBoxObserver->MDPOWarnComplete(iDialogBoxError, response);

	iDialogBoxObserver = NULL;
	iDialogBoxError = KErrNone;
	}

void CNetConTest0220::RequestComplete(const CSelectionRequest* aRequest, TInt aError)
	{

	ASSERT((CNetConRequestBase*)aRequest == iRequest);

	if(aError!=iExpectedError)
		{
		CompleteTest(MNetConTest::EFailed);
		return;
		}
	
	ASSERT(iAgent);

	TNifAgentInfo info;
	iAgent->Info(info);

	if(aRequest->AgentName() == info.iName)
		{
		CompleteTest(MNetConTest::EPassed);
		}
	else
		{
		CompleteTest(MNetConTest::EFailed);
		}
	}


//
//  Test case NC-2-21 (was NC-1-14)
//

CNetConTest0221* CNetConTest0221::NewLC()
	{
	CNetConTest0221* self = new(ELeave) CNetConTest0221();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0221::CNetConTest0221()
	{

	iConnectType = EConnStartExplicit;
	iConnectionAttempt = 1;
	iLastConnectionError = KErrNone;
	iAvailableBearers = (KCommDbBearerCSD | KCommDbBearerPSD);
	iDialogBoxPref = ECommDbDialogPrefDoNotPrompt;
	iDialogBoxError = KErrNone;
	iExpectedError = KErrNone;
	iNetworkId = 221;
	}

void CNetConTest0221::ConstructL()
	{

	CNetConRequestTestBase::ConstructL();

	TCommDbConnPref connPref1;
	connPref1.SetIapId(222);

	TCommDbConnPref connPref2;
	connPref2.SetIapId(111);

	User::LeaveIfError(iMultiPref.SetPreference(1, connPref1));
	User::LeaveIfError(iMultiPref.SetPreference(2, connPref2));

	iRequest = CSelectionRequest::NewL(this, this, iConnectType, iMultiPref, iConnectionAttempt, iLastConnectionError);
	}

CNetConTest0221::~CNetConTest0221()
	{
	}

const TDesC& CNetConTest0221::Name() const
	{

	_LIT(KTestName, "Test Case NC-2-21");
	return KTestName();
	}

void CNetConTest0221::SetOverridesL(CCommDbOverrideSettings* aOverrides)
	{

	if(!aOverrides)
		return;

	// check that the overrides match our preferences
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref overPrefs;
	overPrefs.iDirection = ECommDbConnectionDirectionOutgoing;
	overPrefs.iRanking = iConnectionAttempt;
	TRAPD(err, aOverrides->GetConnectionPreferenceOverride(overPrefs));
	ASSERT(err==KErrNone);

	if((overPrefs.iDialogPref != iSettings.iDialogPref) || 
		(overPrefs.iBearer.iIapId != 222) ||
		(overPrefs.iBearer.iBearerSet != iSettings.iBearerSet))
		{
		CompleteTest(MNetConTest::EFailed);
		return;
		}
	
	// update settings to reflect overrides
	iSettings.iIAPId = 222;
	}

void CNetConTest0221::RequestComplete(const CSelectionRequest*, TInt aError)
	{

	if(aError==KErrNone)
		CompleteTest(MNetConTest::EPassed);
	else
		CompleteTest(MNetConTest::EFailed);
	}


//
//  Test case NC-2-22 (was NC-1-15)
//

CNetConTest0222* CNetConTest0222::NewLC()
	{
	CNetConTest0222* self = new(ELeave) CNetConTest0222();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0222::CNetConTest0222()
	{

	iConnectType = EConnStartExplicit;
	iConnectionAttempt = 2;
	iLastConnectionError = KErrNone;
	iAvailableBearers = (KCommDbBearerCSD | KCommDbBearerPSD);
	iDialogBoxPref = ECommDbDialogPrefDoNotPrompt;
	iDialogBoxError = KErrNone;
	iExpectedError = KErrNone;
	iNetworkId = 222;
	}

void CNetConTest0222::ConstructL()
	{

	CNetConRequestTestBase::ConstructL();

	TCommDbConnPref connPref1;
	connPref1.SetIapId(222);

	TCommDbConnPref connPref2;
	connPref2.SetIapId(111);

	User::LeaveIfError(iMultiPref.SetPreference(1, connPref1));
	User::LeaveIfError(iMultiPref.SetPreference(2, connPref2));

	iRequest = CSelectionRequest::NewL(this, this, iConnectType, iMultiPref, iConnectionAttempt, iLastConnectionError);
	}

CNetConTest0222::~CNetConTest0222()
	{
	}

const TDesC& CNetConTest0222::Name() const
	{

	_LIT(KTestName, "Test Case NC-2-22");
	return KTestName();
	}

void CNetConTest0222::SetOverridesL(CCommDbOverrideSettings* aOverrides)
	{

	if(!aOverrides)
		return;

	// check that the overrides match our preferences
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref overPrefs;
	overPrefs.iDirection = ECommDbConnectionDirectionOutgoing;
	overPrefs.iRanking = iConnectionAttempt;
	TRAPD(err, aOverrides->GetConnectionPreferenceOverride(overPrefs));
	ASSERT(err==KErrNone);

	if((overPrefs.iDialogPref != iSettings.iDialogPref) || 
		(overPrefs.iBearer.iIapId != 111) ||
		(overPrefs.iBearer.iBearerSet != iSettings.iBearerSet))
		{
		CompleteTest(MNetConTest::EFailed);
		return;
		}
	
	// update settings to reflect overrides
	iSettings.iIAPId = 111;
	}

void CNetConTest0222::RequestComplete(const CSelectionRequest*, TInt aError)
	{

	if(aError==KErrNone)
		CompleteTest(MNetConTest::EPassed);
	else
		CompleteTest(MNetConTest::EFailed);
	}


//
//  Test case NC-2-23 (was NC-1-16)
//

CNetConTest0223* CNetConTest0223::NewLC()
	{
	CNetConTest0223* self = new(ELeave) CNetConTest0223();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0223::CNetConTest0223()
	{

	iConnectType = EConnStartExplicit;
	iConnectionAttempt = 2;
	iLastConnectionError = KErrNone;
	iAvailableBearers = (KCommDbBearerCSD | KCommDbBearerPSD);
	iDialogBoxPref = ECommDbDialogPrefDoNotPrompt;
	iDialogBoxError = KErrNone;
	iExpectedError = KErrNone;
	iNetworkId = 223;
	}

void CNetConTest0223::ConstructL()
	{

	CNetConRequestTestBase::ConstructL();

	TCommDbConnPref connPref1;
	connPref1.SetIapId(222);

	TCommDbConnPref connPref2;
	connPref2.SetIapId(111);

	User::LeaveIfError(iMultiPref.SetPreference(1, connPref1));
	User::LeaveIfError(iMultiPref.SetPreference(2, connPref2));

	iMultiPref.SetConnectionAttempts(1);

	iRequest = CSelectionRequest::NewL(this, this, iConnectType, iMultiPref, iConnectionAttempt, iLastConnectionError);
	}

CNetConTest0223::~CNetConTest0223()
	{
	}

const TDesC& CNetConTest0223::Name() const
	{

	_LIT(KTestName, "Test Case NC-2-23");
	return KTestName();
	}

void CNetConTest0223::RequestComplete(const CSelectionRequest*, TInt aError)
	{

	if(aError!=KErrNone)
		CompleteTest(MNetConTest::EPassed);
	else
		CompleteTest(MNetConTest::EFailed);
	}


//
//  Test case NC-2-24
//

CNetConTest0224* CNetConTest0224::NewLC()
	{
	CNetConTest0224* self = new(ELeave) CNetConTest0224();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0224::CNetConTest0224()
	{
	
	iConnectType = EConnStartExplicit;
	iConnectionAttempt = 1;
	iLastConnectionError = KErrNone;
	iAvailableBearers = KCommDbBearerCSD;
	iDialogBoxPref = ECommDbDialogPrefPrompt;
	iDialogBoxError = KErrCancel;
	iExpectedError = KErrCancel;
	iNetworkId = 224;
	}

CNetConTest0224::~CNetConTest0224()
	{ }

const TDesC& CNetConTest0224::Name() const
	{

	_LIT(KTestName, "Test Case NC-2-24");
	return KTestName();
	}

void CNetConTest0224::DialogCbComplete()
	{

	ASSERT(iDialogBoxObserver);
	ASSERT(iDialogBoxAction == ESelectConnection);

	iDialogBoxObserver->MDPOSelectComplete(KErrCancel, iSettings);
	iDialogBoxObserver = NULL;
	iDialogBoxError = KErrNone;
	}


