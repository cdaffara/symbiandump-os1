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
// PhysLinksManHelper.cpp
// 
//

#include <bluetooth/logger.h>
#include "PhysicalLinkHelper.h"
#include "physicallinksmanager.h"
#include "AclDataQController.h"
#include "ProxySAP.h"
#include "linkmgr.h"


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_LINKMGR);
#endif

CPhysicalLinkHelper::CPhysicalLinkHelper(CPhysicalLinksManager& aLinkMgr, CPhysicalLink& aLink)
	: iLinkMgr(aLinkMgr)
	, iLink(aLink)
	{
	LOG_FUNC
	}
	
void CPhysicalLinkHelper::BaseConstructL()
	{
	LOG_FUNC
	
	// create Proxy telling it the possible PHY
	iBTProxySAP = CBTProxySAP::NewL(iLinkMgr, &iLink);
	iBTProxySAP->SetNotify(this);
	
	TCallBack ecb(EventReceivedCallBack, this);
	iEventReceivedCallBack = new (ELeave)CAsyncCallBack(ecb, EActiveHighPriority);
	}
	
CPhysicalLinkHelper::~CPhysicalLinkHelper()
	{
	LOG_FUNC
	
	RemoveTimer();
	delete iBTProxySAP;
	
	if (iEventReceivedCallBack)
		{
		iEventReceivedCallBack->Cancel();
		delete iEventReceivedCallBack;
		}
	}

void CPhysicalLinkHelper::DisableLPM()
   	{
	LOG_FUNC
	TPckgBuf<TInt> optionBuf;	
   	iBTProxySAP->SAPSetOption(KSolBtLMProxy, EBBRequestPreventAllLowPowerModes, optionBuf);
   	}

TBool CPhysicalLinkHelper::IsEPRSupported() const
	{
	LOG_FUNC
	// For Lisbon (Bluetooth 2.1), if EPR is supported both locally and remotely,
	// then the controllers will disable/enable encryption automatically for us,
	// so skip some states.
	return iLink.IsEncryptionPauseResumeSupported();
	}

void CPhysicalLinkHelper::NotifyBasebandEvent(TNotifyEvent aEvent)
	{
	LOG_FUNC
	TBTBasebandEvent event(aEvent.NotifyEvent());
	iBTProxySAP->Ioctl(KSolBtLMProxy, KLMBasebandEventOneShotNotificationIoctl, &event);
	}

void CPhysicalLinkHelper::CancelNotify()
	{
	LOG_FUNC
	iBTProxySAP->CancelIoctl(KSolBtLMProxy, KLMBasebandEventOneShotNotificationIoctl);
	}
	
void CPhysicalLinkHelper::QueueTimer(TTimeIntervalMicroSeconds32 aTimerVal)
	{
	LOG_FUNC

	TCallBack cb(TimerExpiredCallBack, this);
	iTimerEntry.Set(cb);
	BTSocketTimer::Queue(aTimerVal, iTimerEntry);
	}


void CPhysicalLinkHelper::RemoveTimer()
	{
	LOG_FUNC

	BTSocketTimer::Remove(iTimerEntry);
	}
   	   	
// From MSocketNotify
void CPhysicalLinkHelper::NewData(TUint /*aCount*/)
	{
	LOG_FUNC
	
	}
	
void CPhysicalLinkHelper::CanSend()
	{
	LOG_FUNC
	
	}
	
void CPhysicalLinkHelper::ConnectComplete()
	{
	LOG_FUNC
	
	}
	
void CPhysicalLinkHelper::ConnectComplete(const TDesC8& /*aConnectData*/)
	{
	LOG_FUNC
	
	}
	
void CPhysicalLinkHelper::ConnectComplete(CServProviderBase& /*aSSP*/)
	{
	LOG_FUNC
	
	}
	
void CPhysicalLinkHelper::ConnectComplete(CServProviderBase& /*aSSP*/,const TDesC8& /*aConnectData*/)
	{
	LOG_FUNC
	
	}
	
void CPhysicalLinkHelper::CanClose(TDelete /*aDelete*/)
	{
	LOG_FUNC
	
	}

void CPhysicalLinkHelper::CanClose(const TDesC8& /*aDisconnectData*/,TDelete /*aDelete*/)
	{
	LOG_FUNC
	
	}
	
void CPhysicalLinkHelper::Error(TInt /*aError*/,TUint /*aOperationMask*/)
	{
	LOG_FUNC
	
	}
	
void CPhysicalLinkHelper::Disconnect(void)
	{
	LOG_FUNC
	Error(KErrDisconnected);
	}

void CPhysicalLinkHelper::Disconnect(TDesC8& /*aDisconnectData*/)
	{
	LOG_FUNC
	Error(KErrDisconnected);
	}

void CPhysicalLinkHelper::IoctlComplete(TDesC8 *aBuf)
	{
	LOG_FUNC
	const TBTBasebandEventNotification* event = reinterpret_cast<const TBTBasebandEventNotification*>(aBuf->Ptr());
	iBasebandEvent = *event;
	iEventReceivedCallBack->CallBack();
	}

/*static*/ TInt CPhysicalLinkHelper::EventReceivedCallBack(TAny* aThis)
	{
	LOG_STATIC_FUNC
	CPhysicalLinkHelper* helper = static_cast<CPhysicalLinkHelper*>(aThis);
	helper->DoEventReceivedCallBack();
	return EFalse;
	}

void CPhysicalLinkHelper::DoEventReceivedCallBack()
	{
	LOG_FUNC
	EventReceived(iBasebandEvent);
	}

/*static*/ TInt CPhysicalLinkHelper::TimerExpiredCallBack(TAny* aThis)
	{
	LOG_STATIC_FUNC
	CPhysicalLinkHelper* helper = static_cast<CPhysicalLinkHelper*>(aThis);
	helper->DoTimerExpiredCallBack();
	return EFalse;
	}

void CPhysicalLinkHelper::DoTimerExpiredCallBack()
	{
	LOG_FUNC
	TimerExpired();
	}

	







