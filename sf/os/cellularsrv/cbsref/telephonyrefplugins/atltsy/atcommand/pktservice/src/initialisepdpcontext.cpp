// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CInitialisePdpContext:
// 

#include "initialisepdpcontext.h"
#include "mslogger.h"
#include "tsyconfg.h"
#include "globalphonemanager.h"

const TInt KLtsyOnePause = 100000;

// ---------------------------------------------------------------------------
// CInitialisePdpContext::NewL()
// other items were commented in a header
// ---------------------------------------------------------------------------
CInitialisePdpContext* CInitialisePdpContext::NewL(CGlobalPhonemanager& aGloblePhone, 
	                                               CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CInitialisePdpContext* self = new(ELeave) CInitialisePdpContext(aGloblePhone,aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop();
	return self ;
	}

// ---------------------------------------------------------------------------
// CInitialisePdpContext::CInitialisePdpContext()
// other items were commented in a header
// ---------------------------------------------------------------------------
CInitialisePdpContext::CInitialisePdpContext(CGlobalPhonemanager& aGloblePhone, 
	                                         CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:iPhoneGlobals(aGloblePhone),
		 iCtsyDispatcherCallback(aCtsyDispatcherCallback)
	{
	}

// ---------------------------------------------------------------------------
// CInitialisePdpContext::ConstructL()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CInitialisePdpContext::ConstructL()
	{
	iCallbackTimer = CCallbackTimer::NewL(*this);
	}

// ---------------------------------------------------------------------------
// CInitialisePdpContext::~CInitialisePdpContext()
// other items were commented in a header
// ---------------------------------------------------------------------------
CInitialisePdpContext::~CInitialisePdpContext()
	{
	if(iCallbackTimer)
		{
		iCallbackTimer->Cancel();
		delete iCallbackTimer;
		iCallbackTimer = NULL;
		}
	}

// ---------------------------------------------------------------------------
// CInitialisePdpContext::StartRequest()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CInitialisePdpContext::StartRequest()
	{
	iCallbackTimer->After(KLtsyOnePause);
	}

// ---------------------------------------------------------------------------
// CInitialisePdpContext::SetContext()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CInitialisePdpContext::SetContext(const TInt aContextId)
	{
	iContextId = aContextId;
	}

// ---------------------------------------------------------------------------
// CInitialisePdpContext::LoanCommPort()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CInitialisePdpContext::LoanCommPort()
	{
	TInt count = iPhoneGlobals.iContextList.Count();
	TInt i = 0;
	
	//Get the context from context list
	for(i = 0; i < count;i++)
		{
		if(iPhoneGlobals.iContextList[i]->iContextID == iContextId)
			{
			break;
			}
		}
	
	TFileName csy;
	TName port;
	if(iPhoneGlobals.iPhoneStatus.iDataPortLoaned)
		{
		iCtsyDispatcherCallback.CallbackPacketServicesInitialisePdpContextComp(KErrEtelPortAlreadyLoaned,
				                iPhoneGlobals.iContextList[i]->iInfo.iName,
								port);
		return ;
		}
	TInt ret = 0;
	TRAP(ret,iPhoneGlobals.iConfiguration->ConfigModemStringL(TPtrC(KCDTypeNameCsyName),csy));
	if (ret!=KErrNone)
		{
		iCtsyDispatcherCallback.CallbackPacketServicesInitialisePdpContextComp(ret,
				                iPhoneGlobals.iContextList[i]->iInfo.iName,
				                port);
		return ;
		}
	TBuf8<KMaxName> tmp;
	tmp.Copy(csy);
    LOGTEXTREL2(_L8("csy:\t%S"),&tmp);
	
	TRAP(ret,iPhoneGlobals.iConfiguration->ConfigModemStringL(TPtrC(KCDTypeNamePortName),port));
	if (ret!=KErrNone)
		{
		iCtsyDispatcherCallback.CallbackPacketServicesInitialisePdpContextComp(ret,
				                iPhoneGlobals.iContextList[i]->iInfo.iName,
				                port);
		return;
		}
    tmp.Copy(port);
    LOGTEXTREL2(_L8("port:\t%S"),&tmp);

	iCtsyDispatcherCallback.CallbackPacketServicesInitialisePdpContextComp(KErrNone,
			                iPhoneGlobals.iContextList[i]->iInfo.iName,
			                port);

	TContextMisc contextMisc;
	contextMisc.iStatus = RPacketContext::EStatusInactive;
	iPhoneGlobals.iContextList[i]->iInfo.iStatus = RPacketContext::EStatusInactive;
	iCtsyDispatcherCallback.CallbackPacketServicesNotifyPdpContextStatusChangeInd(ret,
			                iPhoneGlobals.iContextList[i]->iInfo.iName,
							contextMisc);
	iCtsyDispatcherCallback.CallbackPacketServicesNotifyPdpContextAddedInd(ret, 
			                iPhoneGlobals.iContextList[i]->iInfo.iName);
	}

void CInitialisePdpContext::TimerRun(TInt aError)
    {
	if(aError == KErrNone)
	    {
	    LoanCommPort();
		}
	}

// End of file
