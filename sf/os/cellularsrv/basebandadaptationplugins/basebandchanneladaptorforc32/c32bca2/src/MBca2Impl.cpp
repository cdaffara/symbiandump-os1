// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// C32 Baseband Channel Adaptor(BCA) Factory APIs implementation.
// This file contains all the API implementations of C32BcaFactory for Symbian OS.
// 
//

/**
 @file 
 @internalComponent
*/
#include "MBca2Impl.h"
#include <networking/bca.h>
#include <comms-infras/commsbuf.h>
#include "C32BcaFactory.h"

using namespace BasebandChannelAdaptation;
using namespace BasebandChannelAdaptation::C32Bca;
using namespace BasebandChannelAdaptation2;
using namespace BasebandChannelAdaptation2::C32BcaShim;

MBca2* MBca2FactoryImpl::NewBcaL()
	{
	MBca2Impl* instance = new(ELeave) MBca2Impl;
	CleanupStack::PushL(instance);
	instance->ConstructL();
	CleanupStack::Pop(instance);
	return instance;
	}
	
void MBca2FactoryImpl::Release()
	{
	delete this;
	}

MBca2Impl::MBca2Impl() : iControl(NULL),
	iBcaParams(NULL),
	iLowerDataSender(NULL),
	iReceiver(NULL),
	iMBca(NULL),
	iC32BcaFactory(NULL),
	iRecvBlocked(EFalse)
	{
	
	}

MBca2Impl::~MBca2Impl()
	{	
	iReceiver->Cancel();
	delete iReceiver;
	delete iLowerDataSender;
	iMBca->Release();
	iC32BcaFactory->Release();
	}

void MBca2Impl::ConstructL()
	{
	iC32BcaFactory = new(ELeave) CC32BcaFactory;
	iMBca = iC32BcaFactory->NewBcaL();
	iLowerDataSender = MLowerDataSenderImpl::NewL(iMBca);
	iReceiver = new (ELeave) CReceiver(iMBca);
	}

TInt MBca2Impl::Open(MUpperControl& aControl, 
						MUpperDataReceiver& aData, 
						const TBcaParams& aBcaParams)
	{
	iControl = &aControl;
	iLowerDataSender->SetControl(&aControl);
	iReceiver->SetParams(&aData,&aBcaParams);
	iBcaParams = &aBcaParams;
	return KErrNone;
	}
		
void MBca2Impl::Start()
	{
	TRequestStatus status;
	iMBca->Open(status,iBcaParams->iChannelId);
	User::WaitForRequest(status);
	TInt ret = status.Int();

	if (ret != KErrNone && ret != KErrAlreadyExists )
		{
		TRequestStatus shutdownStatus;
		iMBca->Shutdown(shutdownStatus);
		User::WaitForRequest(shutdownStatus);
		}
	else
		{
		iControl->StartSending();
		iReceiver->StartListening();
		}
	return;
	}
		
MLowerDataSender& MBca2Impl::GetSender()
	{
	return *iLowerDataSender;
	}
		
void MBca2Impl::Close()
	{
	iMBca->Close();
	}
		
void MBca2Impl::Release()
	{	
	delete this;
	}
		
void MBca2Impl::SetFlowControl(TBlockOption aOption)
	{
	if(aOption == EBlockFlow)
		{
		iRecvBlocked = ETrue;
		iReceiver->Cancel();
		}
	else if(aOption == EUnblockFlow)
		{
		if( iRecvBlocked)
			{
			iReceiver->StartListening();
			}
		else
			{
			iRecvBlocked = EFalse;
			}
		}
	}
		
void MBca2Impl::Control(TRequestStatus& aStatus,
								TUint aOptLevel,
								TUint aOptName,
								TDes8& aOpt)
	{
	iMBca->Ioctl(aStatus,aOptLevel,aOptName,aOpt);
	}
		
void MBca2Impl::CancelControl()
	{
	iMBca->CancelIoctl();
	}

MLowerDataSenderImpl* MLowerDataSenderImpl::NewL(BasebandChannelAdaptation::MBca* aMBca)
	{
	MLowerDataSenderImpl* instance = new (ELeave) MLowerDataSenderImpl;
	CleanupStack::PushL(instance);
	instance->ConstructL(aMBca);
	CleanupStack::Pop(instance);
	return instance;	 
	}

MLowerDataSenderImpl::MLowerDataSenderImpl() : iSender(NULL), iControl(NULL), iSendBlocked(EFalse)
	{
	
	}

void MLowerDataSenderImpl::ConstructL(BasebandChannelAdaptation::MBca* aMBca)
	{
	iSender = new (ELeave) CSender(aMBca);
	}

MLowerDataSenderImpl::~MLowerDataSenderImpl()
	{
	iSender->Cancel();
	delete iSender;
	}

void MLowerDataSenderImpl::SetControl(MUpperControl* aControl)
	{
	iControl = aControl;
	}

MLowerDataSender::TSendResult MLowerDataSenderImpl::Send(RCommsBufChain& aCommsBufChain)
	{	
	TRequestStatus status;
	TInt retSend = iSender->Send(status, aCommsBufChain);
	if(retSend == -1)
		{
		iSendBlocked = ETrue;
		return ESendBlocked;
		}	
	User::WaitForRequest(status);
	TInt ret = status.Int();
	if(ret == KErrNone)
		{
		if(iSendBlocked)
			{
			iControl->StartSending();
			iSendBlocked = EFalse;
			}
		return ESendAccepted;
		}
	else
		{
		iControl->Error(ret);
		return ESendBlocked;
		}
	}

CReceiver::CReceiver(BasebandChannelAdaptation::MBca* aMBca) : CActive(EPriorityNormal), iMBca(aMBca)
	{
	CActiveScheduler::Add(this);
	}

CReceiver::~CReceiver()
	{
	
	}

void CReceiver::SetParams(MUpperDataReceiver* aDataReceiver, const MBca2::TBcaParams* aBcaParams)
	{
	iDataReceiver = aDataReceiver;
	iBcaParams = aBcaParams;
	}

void CReceiver::RunL()
	{	
	TCommsBufAllocator allocator = const_cast<RCommsBufPond&>(iBcaParams->iCommsBufPond).Allocator();
	iBufChain.Alloc(iData.Length(),allocator);
	iBufChain.Write(iData);
	iDataReceiver->Process(iBufChain);
	StartListening();
	}

void CReceiver::DoCancel()
	{
	iMBca->CancelRead();
	}

void CReceiver::StartListening()
	{
	// call read on MBca
	iData.SetMax();
	iMBca->Read(iStatus,iData);
	SetActive();
	}

CSender::CSender(BasebandChannelAdaptation::MBca* aMBca) : CActive(EPriorityNormal), iMBca(aMBca)
	{
	CActiveScheduler::Add(this);
	}
CSender::~CSender()
	{
	
	}

void CSender::RunL()
	{
	User::RequestComplete(iSendRequest, iStatus.Int());
	}

void CSender::DoCancel()
	{
	iMBca->CancelWrite();
	}

TInt CSender::Send(TRequestStatus& aStatus,RCommsBufChain& aPdu)
	{
	if(iStatus == KRequestPending)
		{
			return -1;
		}
	else
		{
		aStatus = KRequestPending;
		iSendRequest = &aStatus;
		TBuf8<KMaxIPPacketAndHeader> buffer;
		if(aPdu.Length() > KMaxIPPacketAndHeader)
			{
			// discard packet and return			
			aPdu.Free();
			User::RequestComplete(iSendRequest, KErrNone);
			return 0;
			}
		buffer.SetLength(aPdu.Length());
		aPdu.Read(buffer);
		aPdu.Free();
		SendBuffer(buffer);
		return 0;
		}
		
	}
void CSender::SendBuffer(const TDesC8& aBuffer)
	{
	iMBca->Write(iStatus,aBuffer);
	SetActive();		
	}

