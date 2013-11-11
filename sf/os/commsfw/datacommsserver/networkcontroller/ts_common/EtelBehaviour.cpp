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

#include "EtelBehaviour.h"
#include "etelmm.h"
#include "etelpckt.h"
#include "pcktcs.h"

CEtelBehaviourBase::~CEtelBehaviourBase()
	{

	if(iCurrentRPhoneRequestStatus)
		CompleteCurrentRPhoneAsyncRequest(KErrCancel);
	}

CEtelBehaviourBase::CEtelBehaviourBase()
	{
	}

void CEtelBehaviourBase::CompleteCurrentRPhoneAsyncRequest(TInt aError)
	{

	if(iCurrentRPhoneRequestStatus)
		{
		User::RequestComplete(iCurrentRPhoneRequestStatus, aError);
		iCurrentRPhoneRequestStatus = NULL;
		iCurrentRPhoneAsyncRequest = 0;
		}
	}

void CEtelBehaviourBase::CompleteCurrentRPacketServiceAsyncRequest(TInt aError)
	{

	if(iCurrentRPacketServiceRequestStatus)
		{
		User::RequestComplete(iCurrentRPacketServiceRequestStatus, aError);
		iCurrentRPacketServiceRequestStatus = NULL;
		iCurrentRPacketServiceAsyncRequest = 0;
		}
	}

COneShotTimer::COneShotTimer(MTimerObserver* aObserver)
: CTimer(CActive::EPriorityStandard), iObserver(aObserver)
	{

	CActiveScheduler::Add(this);
	}

void COneShotTimer::RunL()
	{

	iObserver->TimerComplete(iStatus.Int());
	}

COneShotTimer* COneShotTimer::NewL(MTimerObserver* aObserver)
	{

	COneShotTimer* self = new(ELeave) COneShotTimer(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

void COneShotTimer::ConstructL()
	{

	CTimer::ConstructL();
	}

CEtelBehaviour* CEtelBehaviour::NewL()
	{
	CEtelBehaviour* self = new(ELeave) CEtelBehaviour();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

CEtelBehaviour::CEtelBehaviour()
	{ }

void CEtelBehaviour::ConstructL()
	{

	iTimer = COneShotTimer::NewL(this);
	}

CEtelBehaviour::~CEtelBehaviour()
	{

	if(iTimer)
		delete iTimer;
	}

TInt CEtelBehaviour::RTelServerLoadPhoneModule(const TDesC&)
	{

	return KErrNone;
	}

TInt CEtelBehaviour::RTelServerEnumeratePhones(TInt& aNoOfPhones)
	{

	aNoOfPhones = 5;
	return KErrNone;
	}

TInt CEtelBehaviour::RTelServerGetTsyName(const TInt, TDes& aTsyName)
	{

	_LIT(KTsyName, "mm");
	aTsyName = KTsyName();
	return KErrNone;
	}

TInt CEtelBehaviour::RTelServerGetPhoneInfo(const TInt,RTelServer::TPhoneInfo& aInfo)
	{

	_LIT(KPhoneName, "mm");
	aInfo.iNetworkType = RTelServer::ENetworkTypeMobileDigital;
	aInfo.iName = KPhoneName();
	aInfo.iNumberOfLines = 1;
	aInfo.iExtensions = 1;
	return KErrNone;
	}

TInt CEtelBehaviour::RTelServerUnloadPhoneModule(const TDesC&)
	{

	return KErrNone;
	}

TInt CEtelBehaviour::RPhoneOpen(RTelServer&,const TDesC&)
	{

	return KErrNone;
	}

void CEtelBehaviour::RPhoneCancelAsyncRequest(TInt aReqToCancel)
	{
	
	ASSERT(iCurrentRPhoneAsyncRequest == aReqToCancel);
	(void)aReqToCancel;

	CompleteCurrentRPhoneAsyncRequest(KErrCancel);
	}

TInt CEtelBehaviour::RPhoneGetCaps(RPhone::TCaps& aCaps)
	{

	aCaps.iFlags = RPhone::KCapsData;
	return KErrNone;
	}

TInt CEtelBehaviour::RMobilePhoneGetCurrentMode(RMobilePhone::TMobilePhoneNetworkMode& aNetworkMode)
	{

	aNetworkMode = RMobilePhone::ENetworkModeGsm;
	return KErrNone;
	}

void CEtelBehaviour::RMobilePhoneGetSignalStrength(TRequestStatus& aReqStatus, TInt32& aSignalStrength, TInt8& aBar)
	{

	ASSERT(!iCurrentRPhoneRequestStatus);
	
	aReqStatus = KRequestPending;	
	aSignalStrength = 1000;
	aBar = 10;

	iCurrentRPhoneAsyncRequest = EMobilePhoneGetSignalStrength;
	iCurrentRPhoneRequestStatus = &aReqStatus;

	CompleteCurrentRPhoneAsyncRequest(KErrNone);
	}

void CEtelBehaviour::RMobilePhoneNotifySignalStrengthChange(TRequestStatus& aReqStatus, TInt32& aSignalStrength, TInt8& aBar)
	{

	if(iCurrentRPhoneRequestStatus)
		return;

	ASSERT(!iCurrentRPhoneRequestStatus);

	aReqStatus = KRequestPending;	
	aSignalStrength = 1500;
	aBar = 15;

	iCurrentRPhoneAsyncRequest = EMobilePhoneNotifySignalStrengthChange;
	iCurrentRPhoneRequestStatus = &aReqStatus;

	// Complete request after 1 second
	iTimer->After(1000000);
	}

void CEtelBehaviour::TimerComplete(TInt)
	{

	CompleteCurrentRPhoneAsyncRequest(KErrNone);
	}

TInt CEtelBehaviour::RMobilePhoneGetMultimodeCaps(TUint32& aCaps)
	{

	aCaps = RMobilePhone::KCapsGprsSupported;
	return KErrNone;
	}

TInt CEtelBehaviour::RPacketServiceOpen(RPhone&)
	{

	return KErrNone;
	}

void CEtelBehaviour::RPacketServiceCancelAsyncRequest(TInt aReqToCancel)
	{
	
	ASSERT(iCurrentRPacketServiceAsyncRequest == aReqToCancel);
	(void)aReqToCancel;

	CompleteCurrentRPacketServiceAsyncRequest(KErrCancel);
	}

TInt CEtelBehaviour::RPacketServiceGetStatus(RPacketService::TStatus& aPacketStatus)
	{

	aPacketStatus = RPacketService::EStatusAttached;
	return KErrNone;
	}

void CEtelBehaviour::RPacketServiceGetMSClass(TRequestStatus& aStatus, RPacketService::TMSClass& aCurrentClass, RPacketService::TMSClass& aMaxClass)
	{

	ASSERT(!iCurrentRPacketServiceRequestStatus);

	aStatus = KRequestPending;
	aCurrentClass = RPacketService::EMSClassDualMode;
	aMaxClass = RPacketService::EMSClassDualMode;

	iCurrentRPacketServiceAsyncRequest = EPacketGetMSClass;
	iCurrentRPacketServiceRequestStatus = &aStatus;

	CompleteCurrentRPacketServiceAsyncRequest(KErrNone);
	}

void CEtelBehaviour::RPacketServiceGetNtwkRegStatus(TRequestStatus& aStatus, RPacketService::TRegistrationStatus& aRegistrationStatus)
	{

	ASSERT(!iCurrentRPacketServiceRequestStatus);

	aStatus = KRequestPending;
	aRegistrationStatus = RPacketService::ERegisteredOnHomeNetwork;

	iCurrentRPacketServiceAsyncRequest = EPacketGetNtwkRegStatus;
	iCurrentRPacketServiceRequestStatus = &aStatus;

	CompleteCurrentRPacketServiceAsyncRequest(KErrNone);
	}

TInt CEtelBehaviour::RPacketServiceGetAttachMode(RPacketService::TAttachMode& aMode)
	{

	aMode = RPacketService::EAttachWhenPossible;
	return KErrNone;
	}


//
//  ETEL behaviour for test case NC-4-9
//
//  RTelServer LoadPhoneModule() will return KErrNotFound
//

CTest0409Behaviour* CTest0409Behaviour::NewL()
	{

	CTest0409Behaviour* self = new(ELeave) CTest0409Behaviour();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

CTest0409Behaviour::CTest0409Behaviour()
	{ }

void CTest0409Behaviour::ConstructL()
	{

	CEtelBehaviour::ConstructL();
	}

CTest0409Behaviour::~CTest0409Behaviour()
	{ }

TInt CTest0409Behaviour::RTelServerLoadPhoneModule(const TDesC&)
	{

	return KErrNotFound;
	}


//
//  ETEL behaviour for test case NC-4-11
//
//  RPhone Open() will return KErrGeneral
//

CTest0411Behaviour* CTest0411Behaviour::NewL()
	{

	CTest0411Behaviour* self = new(ELeave) CTest0411Behaviour();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

CTest0411Behaviour::CTest0411Behaviour()
	{ }

void CTest0411Behaviour::ConstructL()
	{

	CEtelBehaviour::ConstructL();
	}

CTest0411Behaviour::~CTest0411Behaviour()
	{ }

TInt CTest0411Behaviour::RPhoneOpen(RTelServer&,const TDesC&)
	{

	return KErrGeneral;
	}

TInt CTest0411Behaviour::RTelServerGetTsyName(const TInt, TDes& aTsyName)
	{

	_LIT(KTsyName, "Test Case NC-4-11");
	aTsyName = KTsyName();
	return KErrNone;
	}


//
//  ETEL behaviour for test case NC-4-13
//
//  RPhone GetCaps() will return KErrGeneral
//

CTest0413Behaviour* CTest0413Behaviour::NewL()
	{

	CTest0413Behaviour* self = new(ELeave) CTest0413Behaviour();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

CTest0413Behaviour::CTest0413Behaviour()
	{ }

void CTest0413Behaviour::ConstructL()
	{

	CEtelBehaviour::ConstructL();
	}

CTest0413Behaviour::~CTest0413Behaviour()
	{ }

TInt CTest0413Behaviour::RTelServerGetTsyName(const TInt, TDes& aTsyName)
	{

	_LIT(KTsyName, "Test Case NC-4-13");
	aTsyName = KTsyName();
	return KErrNone;
	}

TInt CTest0413Behaviour::RPhoneGetCaps(RPhone::TCaps&)
	{

	return KErrGeneral;
	}


//
//  ETEL behaviour for test case NC-4-15
//
//  RPacketService Open() will return KErrNotFound
//

CTest0415Behaviour* CTest0415Behaviour::NewL()
	{

	CTest0415Behaviour* self = new(ELeave) CTest0415Behaviour();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

CTest0415Behaviour::CTest0415Behaviour()
	{ }

void CTest0415Behaviour::ConstructL()
	{

	CEtelBehaviour::ConstructL();
	}

CTest0415Behaviour::~CTest0415Behaviour()
	{ }

TInt CTest0415Behaviour::RTelServerGetTsyName(const TInt, TDes& aTsyName)
	{

	_LIT(KTsyName, "Test Case NC-4-15");
	aTsyName = KTsyName();
	return KErrNone;
	}

TInt CTest0415Behaviour::RPacketServiceOpen(RPhone&)
	{

	return KErrNotFound;
	}


//
//  ETEL behaviour for test case NC-4-17
//
//  RPacketService GetStatus() will return KErrGeneral
//

CTest0417Behaviour* CTest0417Behaviour::NewL()
	{

	CTest0417Behaviour* self = new(ELeave) CTest0417Behaviour();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

CTest0417Behaviour::CTest0417Behaviour()
	{ }

void CTest0417Behaviour::ConstructL()
	{

	CEtelBehaviour::ConstructL();
	}

CTest0417Behaviour::~CTest0417Behaviour()
	{ }

TInt CTest0417Behaviour::RTelServerGetTsyName(const TInt, TDes& aTsyName)
	{

	_LIT(KTsyName, "Test Case NC-4-17");
	aTsyName = KTsyName();
	return KErrNone;
	}

TInt CTest0417Behaviour::RPacketServiceGetStatus(RPacketService::TStatus&)
	{

	return KErrGeneral;
	}

//
//  ETEL behaviour for test case NC-4-20
//
//  RPacketService GetAttachMode() will RPacketService::EAttachWhenNeeded
//

CTest0420Behaviour* CTest0420Behaviour::NewL()
	{

	CTest0420Behaviour* self = new(ELeave) CTest0420Behaviour();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

CTest0420Behaviour::CTest0420Behaviour()
	{ }

void CTest0420Behaviour::ConstructL()
	{

	CEtelBehaviour::ConstructL();
	}

CTest0420Behaviour::~CTest0420Behaviour()
	{ }

TInt CTest0420Behaviour::RTelServerGetTsyName(const TInt, TDes& aTsyName)
	{

	_LIT(KTsyName, "Test Case NC-4-20");
	aTsyName = KTsyName();
	return KErrNone;
	}

TInt CTest0420Behaviour::RPacketServiceGetAttachMode(RPacketService::TAttachMode& aMode)
	{

	aMode = RPacketService::EAttachWhenNeeded;
	return KErrNone;
	}

