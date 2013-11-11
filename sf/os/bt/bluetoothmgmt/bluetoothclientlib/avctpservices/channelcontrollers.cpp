// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// basechcontroller.cpp
//



/**
 @file
 @internalTechnology
*/
#include <bluetooth/logger.h>
#include "avctpPriorities.h"
#include "avctpcommon.h"
#include "channelcontrollers.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVCTP_SERVICES);
#endif

using namespace SymbianAvctp;

CBaseChController::CBaseChController(MControlChannelNotify& aNotify, RSocketServ& aSockServer, TControlIoctls aIoctl, SymbianAvctp::TPid aPid, TInt aChannel) :
	CActive(KDefaultPriority),
	iNotify(aNotify),
	iSockServer(aSockServer),
	iIoctl(aIoctl),
	iPid(aPid),
	iChannel(aChannel)
	{
	LOG_FUNC
	}

CBaseChController::~CBaseChController()
	{
	LOG_FUNC
	Cancel();
	iTransport.Close();
	}

void CBaseChController::BaseConstructL()
	{
	LOG_FUNC

	CActiveScheduler::Add(this);
	TInt err = iTransport.Open(iSockServer, KBTAddrFamily, KSockRaw, KAVCTP);
	User::LeaveIfError(err);
	}
	
void CBaseChController::DoCancel()
	{
	LOG_FUNC
	iTransport.CancelAll();
	}

void CBaseChController::RunL()
	{
	LOG_FUNC
	TInt err = iStatus.Int();
	if (err == KErrNone && iIoctlBuf().iError != KErrNone)
		{
		err = iIoctlBuf().iError;
		}
	
	NotifyIoctlCompleted(err);
	}

TInt CBaseChController::RunError(TInt aError)
	{
	LOG_FUNC
	NotifyIoctlError(aError);
	return KErrNone;	
	}

void CBaseChController::Ioctl()
	{
	LOG_FUNC
	iTransport.CancelIoctl();
	iIoctlBuf().iIoctl = iIoctl;
	iIoctlBuf().iAddr.SetChannel(iChannel);
	iTransport.Ioctl(iIoctl, iStatus, &iIoctlBuf, KSolBtAVCTP);
	SetActive();
	}

void CBaseChController::AttachRequest(const TBTDevAddr& aBTDevice)
	{
	LOG_FUNC
	
	Cancel();	// cancel the outstanding active object

	TAvctpSockAddr addr;
	addr.SetBTAddr(aBTDevice);
	addr.SetPid(iPid);
	addr.SetChannel(iIoctl ==  EPrimaryChannelAttachToTransport || iIoctl == EAwaitProvidedTransport ? 0 : 1);
	
	iIoctlBuf().iAddr = addr;
	Ioctl();
	}

void CBaseChController::Listen()
	{
	LOG_FUNC
	Listen(TBTDevAddr(0));
	}

void CBaseChController::Listen(const TBTDevAddr& aBTDevice)
	{
	LOG_FUNC
	Cancel();
	
	TAvctpSockAddr addr;
	addr.SetBTAddr(aBTDevice);
	addr.SetPid(iPid);
	
	iIoctlBuf().iAddr = addr;
	Ioctl();
	}

void CBaseChController::DetachRequest(const TBTDevAddr& aBTDevice)
	{
	LOG_FUNC
	Cancel();	// cancel all outstanding ioctl
	
	TAvctpSockAddr addr;
	addr.SetBTAddr(aBTDevice);
	addr.SetPid(iPid);
	
	iIoctlBuf().iAddr = addr;
	Ioctl();
	}

void CBaseChController::RefuseAttach(const TBTDevAddr& aBTDevice)
	{
	LOG_FUNC
	Cancel();	// cancel all outstanding ioctl
	
	TAvctpSockAddr addr;
	addr.SetBTAddr(aBTDevice);
	addr.SetPid(iPid);
	
	iIoctlBuf().iAddr = addr;
	Ioctl();
	}

void CBaseChController::AgreeAttachment(const TBTDevAddr& aBTDevice)
	{
	LOG_FUNC
	Cancel();	// cancel all outstanding ioctl
	
	TAvctpSockAddr addr;
	addr.SetBTAddr(aBTDevice);
	addr.SetPid(iPid);
	
	iIoctlBuf().iAddr = addr;
	Ioctl();
	}


// CPrimaryChannelController
CPrimaryChannelController* CPrimaryChannelController::NewL(MControlChannelNotify& aNotify, RSocketServ& aSockServer, SymbianAvctp::TPid aPid)
	{
	LOG_STATIC_FUNC

	CPrimaryChannelController* self = CPrimaryChannelController::NewLC(aNotify, aSockServer, aPid);
	CleanupStack::Pop(self);
	return self;		
	}

CPrimaryChannelController* CPrimaryChannelController::NewLC(MControlChannelNotify& aNotify, RSocketServ& aSockServer, SymbianAvctp::TPid aPid)
	{
	LOG_STATIC_FUNC

	CPrimaryChannelController* self = new(ELeave) CPrimaryChannelController(aNotify, aSockServer, aPid);
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;	
	}

CPrimaryChannelController::CPrimaryChannelController(MControlChannelNotify& aNotify, RSocketServ& aSockServer, SymbianAvctp::TPid aPid) :
	CBaseChController(aNotify, aSockServer, EAwaitProvidedTransport, aPid, KAvctpPrimaryChannel)
	{
	LOG_FUNC
	}

void CPrimaryChannelController::AttachRequest(const TBTDevAddr& aBTDevice)
	{
	LOG_FUNC
	iIoctl = EPrimaryChannelAttachToTransport;
	CBaseChController::AttachRequest(aBTDevice);
	}

void CPrimaryChannelController::DetachRequest(const TBTDevAddr& aBTDevice)
	{
	LOG_FUNC
	iIoctl = EPrimaryChannelDetachFromTransport;
	CBaseChController::DetachRequest(aBTDevice);
	}

void CPrimaryChannelController::RefuseAttach(const TBTDevAddr& aBTDevice)
	{
	LOG_FUNC
	iIoctl = EPrimaryChannelRefuseAttach;
	CBaseChController::RefuseAttach(aBTDevice);
	}

CPrimaryChannelController::~CPrimaryChannelController()
	{
	LOG_FUNC
	}

void CPrimaryChannelController::NotifyIoctlCompleted(TInt aError)
	{
	LOG_FUNC
	
	// get some parameters
	TBTDevAddr remdev = iIoctlBuf().iAddr.BTAddr();
	
	switch(iIoctlBuf().iIoctl)
		{
		case EAttachConfirm:
			{
			TInt mtu = KDefaultMtu;
			if (aError == KErrNone)
				{
				TPckgBuf<TOptionMessage> optBuf;
				optBuf().iAddr = remdev;
				TInt mtuErr = iTransport.GetOpt(KAvctpBaseOutboundMTU, KSolBtAVCTP, optBuf);
				mtu = mtuErr ? KDefaultMtu : optBuf().iMtu;
				}
			iNotify.PrimaryChannelAttachConfirm(remdev, mtu, aError);
			break;
			}
		case EAttachIndicate:
			{
			__ASSERT_DEBUG(aError == KErrNone, Panic(EAvctpUnexpectedErrorCode));
			TInt mtu = KDefaultMtu;
			TPckgBuf<TOptionMessage> optBuf;
			optBuf().iAddr = remdev;
			TInt mtuErr = iTransport.GetOpt(KAvctpBaseOutboundMTU, KSolBtAVCTP, optBuf);
			mtu = mtuErr ? KDefaultMtu : optBuf().iMtu;
			iNotify.PrimaryChannelAttachIndicate(remdev, mtu);
			break;
			}
		case EDetachConfirm:
			{
			iNotify.PrimaryChannelDetachConfirm(remdev, aError);
			break;
			}
		case EDetachIndicate:
			{
			iNotify.PrimaryChannelDetachIndicate(remdev);
			break;
			}
		case EError:
			iNotify.PrimaryChannelIoctlError(remdev, aError);
			break;
		case EPrimaryChannelAgreeAttachment:
			iNotify.PrimaryChannelAgreementError(remdev, aError);
			break;
		}
	}

void CPrimaryChannelController::NotifyIoctlError(TInt aError)
	{
	LOG_FUNC
	iNotify.PrimaryChannelIoctlError(iIoctlBuf().iAddr, aError);
	}

void CPrimaryChannelController::Listen()
	{
	LOG_FUNC
	iIoctl =EAwaitProvidedTransport;	// so when the ioctl is submitted again it is the it is the right one
	CBaseChController::Listen();
	}

void CPrimaryChannelController::AgreeAttachment(const TBTDevAddr& aBTDevice)
	{
	LOG_FUNC
	iIoctl = EPrimaryChannelAgreeAttachment;
	CBaseChController::AgreeAttachment(aBTDevice);
	}

// CSecondaryChannelController
CSecondaryChannelController* CSecondaryChannelController::NewL(MControlChannelNotify& aNotify, RSocketServ& aSockServer, SymbianAvctp::TPid aPid)
	{
	LOG_STATIC_FUNC

	CSecondaryChannelController* self = CSecondaryChannelController::NewLC(aNotify, aSockServer, aPid);
	CleanupStack::Pop(self);
	return self;		
	}

CSecondaryChannelController* CSecondaryChannelController::NewLC(MControlChannelNotify& aNotify, RSocketServ& aSockServer, SymbianAvctp::TPid aPid)
	{
	LOG_STATIC_FUNC

	CSecondaryChannelController* self = new(ELeave) CSecondaryChannelController(aNotify, aSockServer, aPid);
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;	
	}

CSecondaryChannelController::CSecondaryChannelController(MControlChannelNotify& aNotify, RSocketServ& aSockServer, SymbianAvctp::TPid aPid) :
	CBaseChController(aNotify, aSockServer, EAwaitExtendedTransport, aPid, KAvctpSecondaryChannel)
	{
	LOG_FUNC
	}

void CSecondaryChannelController::AttachRequest(const TBTDevAddr& aBTDevice)
	{
	LOG_FUNC
	iIoctl = ESecondaryChannelAttachToTransport;
	CBaseChController::AttachRequest(aBTDevice);
	}

void CSecondaryChannelController::AttachPassively(const TBTDevAddr& aBTDevice)
	{
	LOG_FUNC
	iIoctl = EAwaitExtendedTransport;
	CBaseChController::Listen(aBTDevice);
	}

void CSecondaryChannelController::DetachRequest(const TBTDevAddr& aBTDevice)
	{
	LOG_FUNC
	iIoctl = ESecondaryChannelDetachFromTransport;
	CBaseChController::DetachRequest(aBTDevice);
	}

void CSecondaryChannelController::RefuseAttach(const TBTDevAddr& aBTDevice)
	{
	LOG_FUNC
	iIoctl = ESecondaryChannelRefuseAttach;
	CBaseChController::RefuseAttach(aBTDevice);
	}

CSecondaryChannelController::~CSecondaryChannelController()
	{
	LOG_FUNC
	}

void CSecondaryChannelController::NotifyIoctlCompleted(TInt aError)
	{
	LOG_FUNC
	// get some parameters
	
	TBTDevAddr remdev = iIoctlBuf().iAddr.BTAddr();
	
	switch(iIoctlBuf().iIoctl)
		{
		case EAttachConfirm:
			{
			TInt mtu = KDefaultMtu;
			if (aError == KErrNone)
				{
				TPckgBuf<TOptionMessage> optBuf;
				optBuf().iAddr = remdev;
				TInt mtuErr = iTransport.GetOpt(KAvctpExtendOutboundMTU, KSolBtAVCTP, optBuf);
				mtu = mtuErr ? KDefaultMtu : optBuf().iMtu;
				}
			iNotify.SecondaryChannelAttachConfirm(remdev, mtu, aError);
			break;
			}
		case EAttachIndicate:
			{
			__ASSERT_DEBUG(aError == KErrNone, Panic(EAvctpUnexpectedErrorCode));
			TInt mtu = KDefaultMtu;
			TPckgBuf<TOptionMessage> optBuf;
			optBuf().iAddr = remdev;
			TInt mtuErr = iTransport.GetOpt(KAvctpExtendOutboundMTU, KSolBtAVCTP, optBuf);
			mtu = mtuErr ? KDefaultMtu : optBuf().iMtu;
			iNotify.SecondaryChannelAttachIndicate(remdev, mtu);
			break;
			}
		case EDetachConfirm:
			{
			iNotify.SecondaryChannelDetachConfirm(remdev, aError);
			break;
			}
		case EDetachIndicate:
			{
			iNotify.SecondaryChannelDetachIndicate(remdev);
			break;
			}
		case EError:
			iNotify.SecondaryChannelIoctlError(remdev, aError);
			break;
		case ESecondaryChannelAgreeAttachment:
			iNotify.SecondaryChannelAgreementError(remdev, aError);
			break;
		default:
			break;
		}
	}

void CSecondaryChannelController::NotifyIoctlError(TInt aError)
	{
	LOG_FUNC
	iNotify.SecondaryChannelIoctlError(iIoctlBuf().iAddr, aError);
	}

void CSecondaryChannelController::Listen()
	{
	LOG_FUNC
	iIoctl =EAwaitExtendedTransport;	// so when the ioctl is submitted again it is the it is the right one
	CBaseChController::Listen();
	}

void CSecondaryChannelController::AgreeAttachment(const TBTDevAddr& aBTDevice)
	{
	LOG_FUNC
	iIoctl = ESecondaryChannelAgreeAttachment;
	CBaseChController::AgreeAttachment(aBTDevice);
	}
