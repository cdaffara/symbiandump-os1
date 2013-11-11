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

#include "etel.h"
#include "etelmm.h"
#include "etelpckt.h"
#include "EtelBehaviour.h"

//
//  Implementation of RTelServer
//

TInt RTelServer::Connect()
	{
	
	ASSERT(!iTestBehaviour);

	return KErrNone;
	}

void RTelServer::Close()
	{

	if(iTestBehaviour)
		{
		delete iTestBehaviour;
		iTestBehaviour = NULL;
		}
	}

TInt RTelServer::EnumeratePhones(TInt& aNoOfPhones) const
	{

	ASSERT(iTestBehaviour);

	return iTestBehaviour->RTelServerEnumeratePhones(aNoOfPhones);
	}

TInt RTelServer::GetPhoneInfo(const TInt aIndex,TPhoneInfo& aInfo) const
	{

	ASSERT(iTestBehaviour);

	return iTestBehaviour->RTelServerGetPhoneInfo(aIndex, aInfo);
	}

TInt RTelServer::GetTsyName(const TInt aIndexOfPhone, TDes& aTsyName) const
	{

	ASSERT(iTestBehaviour);

	return iTestBehaviour->RTelServerGetTsyName(aIndexOfPhone, aTsyName);
	}

CEtelBehaviourBase* RTelServer::CreateTestBehaviourL(const TDesC& aFileName)
	{

	CEtelBehaviourBase* behaviour = NULL;

	if(aFileName == _L("Test Case NC-4-9"))
		{
		behaviour = CTest0409Behaviour::NewL();
		}
	else if(aFileName == _L("Test Case NC-4-11"))
		{
		behaviour = CTest0411Behaviour::NewL();
		}
	else if(aFileName == _L("Test Case NC-4-13"))
		{
		behaviour = CTest0413Behaviour::NewL();
		}
	else if(aFileName == _L("Test Case NC-4-15"))
		{
		behaviour = CTest0415Behaviour::NewL();
		}
	else if(aFileName == _L("Test Case NC-4-17"))
		{
		behaviour = CTest0417Behaviour::NewL();
		}
	else if(aFileName == _L("Test Case NC-4-20"))
		{
		behaviour = CTest0420Behaviour::NewL();
		}
	else
		{
		behaviour = CEtelBehaviour::NewL();
		}

	return behaviour;
	}

TInt RTelServer::LoadPhoneModule(const TDesC& aFileName)
	{
	
	ASSERT(!iTestBehaviour);

	TRAPD(err, iTestBehaviour = CreateTestBehaviourL(aFileName));
	if(err == KErrNoMemory)
		return err;

	ASSERT(iTestBehaviour);

	return iTestBehaviour->RTelServerLoadPhoneModule(aFileName);
	}

TInt RTelServer::UnloadPhoneModule(const TDesC& aFileName) const
	{

	ASSERT(iTestBehaviour);

	return iTestBehaviour->RTelServerUnloadPhoneModule(aFileName);
	}

CEtelBehaviourBase* RTelServer::TestBehaviour() const
	{

	ASSERT(iTestBehaviour);

	return iTestBehaviour;
	}

//
//  Implementation of RPhone
//

TInt RPhone::Open(RTelServer& aSession,const TDesC& aName)
	{

	iTestBehaviour = aSession.TestBehaviour();

	ASSERT(iTestBehaviour);

	return iTestBehaviour->RPhoneOpen(aSession, aName);
	}

void RPhone::Close()
	{
	
	ASSERT(iTestBehaviour);

	iTestBehaviour = NULL;
	}

void RPhone::CancelAsyncRequest(TInt aReqToCancel) const
	{

	ASSERT(iTestBehaviour);

	iTestBehaviour->RPhoneCancelAsyncRequest(aReqToCancel);
	}

TInt RPhone::GetCaps(TCaps& aCaps) const
	{

	ASSERT(iTestBehaviour);

	return iTestBehaviour->RPhoneGetCaps(aCaps);
	}

CEtelBehaviourBase* RPhone::TestBehaviour() const
	{

	ASSERT(iTestBehaviour);

	return iTestBehaviour;
	}

//
//  Implementation of RMobilePhone
//

TInt RMobilePhone::GetMultimodeCaps(TUint32& aCaps) const
	{

	ASSERT(iTestBehaviour);

	return iTestBehaviour->RMobilePhoneGetMultimodeCaps(aCaps);
	}

TInt RMobilePhone::GetCurrentMode(TMobilePhoneNetworkMode& aNetworkMode) const
	{

	ASSERT(iTestBehaviour);
	
	return iTestBehaviour->RMobilePhoneGetCurrentMode(aNetworkMode);
	}

void RMobilePhone::GetSignalStrength(TRequestStatus& aReqStatus, TInt32& aSignalStrength, TInt8& aBar) const
	{

	ASSERT(iTestBehaviour);

	iTestBehaviour->RMobilePhoneGetSignalStrength(aReqStatus, aSignalStrength, aBar);
	}

void RMobilePhone::NotifySignalStrengthChange(TRequestStatus& aReqStatus, TInt32& aSignalStrength, TInt8& aBar) const
	{
	ASSERT(iTestBehaviour);

	iTestBehaviour->RMobilePhoneNotifySignalStrengthChange(aReqStatus, aSignalStrength, aBar);
	}

//
//  Implementation of RPacketService
//

TInt RPacketService::Open(RPhone& aPhone)
	{

	iTestBehaviour = aPhone.TestBehaviour();

	ASSERT(iTestBehaviour);

	return iTestBehaviour->RPacketServiceOpen(aPhone);
	}

void RPacketService::Close()
	{

	iTestBehaviour = NULL;
	}

void RPacketService::CancelAsyncRequest(TInt aReqToCancel) const
	{

	ASSERT(iTestBehaviour);

	iTestBehaviour->RPacketServiceCancelAsyncRequest(aReqToCancel);
	}

TInt RPacketService::GetStatus(TStatus& aPacketStatus) const
	{

	ASSERT(iTestBehaviour);

	return iTestBehaviour->RPacketServiceGetStatus(aPacketStatus);
	}

void RPacketService::GetMSClass(TRequestStatus& aStatus, TMSClass& aCurrentClass, TMSClass& aMaxClass) const
	{

	ASSERT(iTestBehaviour);

	iTestBehaviour->RPacketServiceGetMSClass(aStatus, aCurrentClass, aMaxClass);
	}

void RPacketService::GetNtwkRegStatus(TRequestStatus& aStatus, TRegistrationStatus& aRegistrationStatus) const
	{
	
	ASSERT(iTestBehaviour);

	iTestBehaviour->RPacketServiceGetNtwkRegStatus(aStatus, aRegistrationStatus);
	}

TInt RPacketService::GetAttachMode(TAttachMode& aMode) const
	{

	ASSERT(iTestBehaviour);

	return iTestBehaviour->RPacketServiceGetAttachMode(aMode);
	}

