// Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the base class for baseband SAPs
// 
//

#include <bluetooth/logger.h>
#include <bluetooth/hci/hciconsts.h>
#include "basebandsap.h"
#include "physicallinksmanager.h"
#include "physicallinks.h"
#include "linkutil.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_LINKMGR);
#endif

#pragma warning (disable: 4100) //unreference formal param { These are both OK
#pragma warning (disable: 4355) //this in base init list   { for our usage


CBTBasebandSAP::CBTBasebandSAP(CPhysicalLinksManager& aLinksMan, CPhysicalLink* aPhysicalLink)
: CBluetoothSAP(aLinksMan.SecMan(), aLinksMan.CodMan()),
  iLinksMan(aLinksMan),
  iPhysicalLink(aPhysicalLink),
  iAcceptWatchdog(*this),
  iQueLink(this)
	{
	LOG_FUNC
	// Initialise handle to a non-zero number
	iHandle = KInvalidConnectionHandle;
	}

void CBTBasebandSAP::ConstructL()
	{
	LOG_FUNC
	CBluetoothSAP::ConstructL();
	}

// from CServProviderBase - the saps will not do all of these
void CBTBasebandSAP::Start()
	{
	LOG_FUNC
	Panic(EBTBaseSAPUnimplementedEvent);
	}

void CBTBasebandSAP::LocalName(TSockAddr& aAddr) const
	{
	LOG_FUNC
	// base class just returns address as a BTSockAddr
	TBTSockAddr btAddr;
	btAddr.SetBTAddr(iLinksMan.LinkManagerProtocol().LocalBTAddress());
	aAddr = btAddr;
	}

TInt CBTBasebandSAP::SetLocalName(TSockAddr& /*aAddr*/)
	{
	LOG_FUNC
	return KErrNotSupported;
	}

void CBTBasebandSAP::RemName(TSockAddr& aAddr) const
	{
	LOG_FUNC
	// base class just returns address as a BTSockAddr
	TBTSockAddr btAddr;
	btAddr.SetBTAddr(iRemoteDev);
	aAddr = btAddr;
	}

TInt CBTBasebandSAP::SetRemName(TSockAddr& aAddr)
	{
	LOG_FUNC
	// base class just sets BTAddr
	TBTSockAddr btAddr = TBTSockAddr::Cast(aAddr);
	iRemoteDev = btAddr.BTAddr();
	return KErrNone;
	}

TInt CBTBasebandSAP::GetOption(TUint /*aLevel*/,TUint /*aName*/,TDes8& /*aOption*/) const
	{
	LOG_FUNC
	Panic(EBTBaseSAPUnimplementedEvent);
	return KErrNotSupported;
	}

void CBTBasebandSAP::Ioctl(TUint /*aLevel*/,TUint /*aName*/, TDes8* /*aOption*/)
	{
	LOG_FUNC
	Panic(EBTBaseSAPUnimplementedEvent);
	}

void CBTBasebandSAP::CancelIoctl(TUint /*aLevel*/,TUint /*aName*/)
	{
	LOG_FUNC
	Panic(EBTBaseSAPUnimplementedEvent);
	}

TInt CBTBasebandSAP::SAPSetOption(TUint /*aLevel*/,TUint /*aName*/,const TDesC8& /*aOption*/)
	{
	LOG_FUNC
	Panic(EBTBaseSAPUnimplementedEvent);
	return KErrNotSupported;
	}

void CBTBasebandSAP::ActiveOpen()
	{
	LOG_FUNC
	Panic(EBTBaseSAPUnimplementedEvent);
	}

void CBTBasebandSAP::ActiveOpen(const TDesC8& /*aConnectionData*/)
	{
	LOG_FUNC
	Panic(EBTBaseSAPUnimplementedEvent);
	}

TInt CBTBasebandSAP::PassiveOpen(TUint /*aQueSize*/)
	{
	LOG_FUNC
	Panic(EBTBaseSAPUnimplementedEvent);
	return KErrNotSupported;
	}

TInt CBTBasebandSAP::PassiveOpen(TUint /*aQueSize*/,const TDesC8& /*aConnectionData*/)
	{
	LOG_FUNC
	Panic(EBTBaseSAPUnimplementedEvent);
	return KErrNotSupported;
	}

void CBTBasebandSAP::Shutdown(TCloseType /*aOption*/)
	{
	LOG_FUNC
	Panic(EBTBaseSAPUnimplementedEvent);
	}

void CBTBasebandSAP::Shutdown(TCloseType /*aOption*/,const TDesC8& /*aDisconnectionData*/)
	{
	LOG_FUNC
	Panic(EBTBaseSAPUnimplementedEvent);
	}

void CBTBasebandSAP::AutoBind()
	{
	LOG_FUNC
	Panic(EBTBaseSAPUnimplementedEvent);
	}

TUint CBTBasebandSAP::Write(const TDesC8& /*aDesc*/,TUint /*aOptions*/, TSockAddr* /*aAddr*/)
	{
	LOG_FUNC
	Panic(EBTBaseSAPUnimplementedEvent);
	return 0;
	}

void CBTBasebandSAP::GetData(TDes8& /*aDesc*/,TUint /*aOptions*/,TSockAddr* /*aAddr*/)
	{
	LOG_FUNC
	Panic(EBTBaseSAPUnimplementedEvent);
	}

CBTBasebandModel& CBTBasebandSAP::Baseband() const
	{
	LOG_FUNC
	return iLinksMan.Baseband();
	}

TInt CBTBasebandSAP::BindLink(TLinkType aLinkType, CPhysicalLink& aPhysicalLink)
	{
	LOG_FUNC
	__ASSERT_DEBUG(iPhysicalLink==NULL || iPhysicalLink==&aPhysicalLink, Panic(EBTBasebandSAPTwoPhysicalLinks));
	TInt err = aPhysicalLink.AttachLogicalLink(aLinkType, *this);
	if (err == KErrNone)
		{
		iPhysicalLink = &aPhysicalLink;
		iPhysicalLink->SubscribeLinkObserver(*this); // logical links might want to know about park etc
		}
	return err;
	}

void CBTBasebandSAP::UnbindLink(TLinkType aLinkType)
	{
	LOG_FUNC
	if (iPhysicalLink)
		{
		iPhysicalLink->UnsubscribeLinkObserver(*this);
		iPhysicalLink->DetachLogicalLink(aLinkType, *this);
		iPhysicalLink = NULL;
		}
	}

void CBTBasebandSAP::RemoveChild(CBTBasebandSAP* __DEBUG_ONLY(aChild))
	{
	LOG_FUNC
	__ASSERT_DEBUG(aChild == iChild, Panic(EBTBasebandSAPBadChild));
	iChild = NULL;
	}
	
void CBTBasebandSAP::DeleteChild(CBTBasebandSAP* __DEBUG_ONLY(aChild))
	{
	LOG_FUNC
	__ASSERT_DEBUG(aChild == iChild, Panic(EBTBasebandSAPBadChild));
	delete iChild, iChild = NULL;
	}
	
void CBTBasebandSAP::LinkStateIdle()
	{
	LOG_FUNC
	if (iPhysicalLink)
		{
		iPhysicalLink->PhysicalLinkUserIdle();
		}
	}

TBool CBTBasebandSAP::IsIdle() const
	{
	LOG_FUNC
	return EFalse;
	}

void CBTBasebandSAP::ParentClosing()
	{
	LOG_FUNC
	//Ignore
	}
