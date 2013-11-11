// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#include "hciserver.h"
#include "corehciutil.h"
#include "hciserverrequestmgr.h"
#include "hcisession.h"
#include "hciserversecuritypolicy.h"
#include "HciCorePluginImpl.h"

#include <bluetooth/hci/hciipc.h>
#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_HCISERVER);
#endif

CHCIServer* CHCIServer::NewL(CCoreHCIPluginImpl& aParent)
	{
	LOG_STATIC_FUNC
	
	CHCIServer* self = new(ELeave) CHCIServer(aParent);
	CleanupStack::PushL(self);
	// StartL is where the kernel checks that there isn't already an instance 
	// of the same server running, so do it before ConstructL.
	self->StartL(KHCIServerName);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CHCIServer::~CHCIServer()
	{
	LOG_FUNC
	
	delete iBTPowerControlManager;
	delete iHCIDirectAccessManager;
	}

CHCIServer::CHCIServer(CCoreHCIPluginImpl& aParent)
 :	CPolicyServer(CActive::EPriorityStandard, KHCIServerPolicy),
	iParent(aParent)
	{
	LOG_FUNC
	}

void CHCIServer::ConstructL()
	{
	LOG_FUNC
	
	iBTPowerControlManager = CBTPowerControlManager::NewL(iParent);
	iHCIDirectAccessManager = CHCIDirectAccessManager::NewL(iParent);
	iBluetoothDutModeManager = CBluetoothDutModeManager::NewL(iParent);
	iA2dpOptimiserManager = CA2dpOptimiserManager::NewL(iParent);
	}

CHCIDirectAccessManager &CHCIServer::DirectAccessManager()
	{
	LOG_FUNC
	
	return *iHCIDirectAccessManager;
	}

CBTPowerControlManager& CHCIServer::PowerControlManager()
	{
	LOG_FUNC
	
	return *iBTPowerControlManager;	
	}

CBluetoothDutModeManager& CHCIServer::DutModeManager()
	{
	LOG_FUNC
	
	return *iBluetoothDutModeManager; 
	}

CA2dpOptimiserManager& CHCIServer::A2dpOptimiserManager()
	{
	LOG_FUNC
	
	return *iA2dpOptimiserManager; 
	}

CHCIServerRequestManager* CHCIServer::ConnectToServiceL(TInt aInterfaceUid)
	{
	LOG_FUNC
	TInt err = KErrInUse;
	CHCIServerRequestManager* requestManager = NULL;

	switch(aInterfaceUid)
		{
	case KHCIPowerControlManagerUid:
		if(!(iRequestManagerLocks & EBTPowerControlService))
			{
			iRequestManagerLocks |= EBTPowerControlService;
			requestManager = iBTPowerControlManager;
			err = KErrNone;
			}
		break;

	case KHCIDirectAccessManagerUid:
		if(!(iRequestManagerLocks & EHCIDirectAccessService))
			{
			iRequestManagerLocks |= EHCIDirectAccessService;
			requestManager = iHCIDirectAccessManager;
			err = KErrNone;
			}
		break;

	case KHCIBluetoothDutModeManagerUid:
		if(!(iRequestManagerLocks & EBluetoothDutModeService))
			{
			iRequestManagerLocks |= EBluetoothDutModeService;
			requestManager = iBluetoothDutModeManager;
			err = KErrNone;
			}
		break;

	case KHCIA2dpOptimserManagerUid:
		if(!(iRequestManagerLocks & EA2dpOptimiserService))
			{
			iRequestManagerLocks |= EA2dpOptimiserService;
			requestManager = iA2dpOptimiserManager;
			err = KErrNone;
			}
		break;

	default:
		err = KErrArgument;
		break;
		}

	LEAVEIFERRORL(err); // only return a request manager on success
	__ASSERT_DEBUG(requestManager, PANIC(KHciServerPanicCat, ENoRequestManagerToProvide));
	return requestManager;
	}

void CHCIServer::ReleaseService(CHCIServerRequestManager* aServiceToRelease)
	{
	if(aServiceToRelease == iBTPowerControlManager)
		{
		__ASSERT_DEBUG(iRequestManagerLocks & EBTPowerControlService, PANIC(KHciServerPanicCat, EPwrCtrlReleasedWhenNotLocked));
		iRequestManagerLocks &= ~EBTPowerControlService;
		}
	else if(aServiceToRelease == iHCIDirectAccessManager)
		{
		__ASSERT_DEBUG(iRequestManagerLocks & EHCIDirectAccessService, PANIC(KHciServerPanicCat, EHciDaReleasedWhenNotLocked));
		iRequestManagerLocks &= ~EHCIDirectAccessService;
		}
	else if(aServiceToRelease == iBluetoothDutModeManager)
		{
		__ASSERT_DEBUG(iRequestManagerLocks & EBluetoothDutModeService, PANIC(KHciServerPanicCat, EDutModeReleasedWhenNotLocked));
		iRequestManagerLocks &= ~EBluetoothDutModeService;
		}
	else if(aServiceToRelease == iA2dpOptimiserManager)
		{
		__ASSERT_DEBUG(iRequestManagerLocks & EA2dpOptimiserService, PANIC(KHciServerPanicCat, EA2dpOptimiserReleasedWhenNotLocked));
		iRequestManagerLocks &= ~EA2dpOptimiserService;
		}
	else
		{
		__ASSERT_DEBUG(EFalse, PANIC(KHciServerPanicCat, EUnknownServiceReleased));
		}
	}

CSession2* CHCIServer::NewSessionL(const TVersion& aVersion, const RMessage2& /*aMessage*/) const
	{
	LOG_FUNC
	LOG3(_L8("\taVersion = (%d,%d,%d)"), aVersion.iMajor, aVersion.iMinor, aVersion.iBuild);
		
	// Version number check...
	TVersion v(KHCISrvVersion2MajorVersionNumber,
			   KHCISrvVersion2MinorVersionNumber,
			   KHCISrvVersion2BuildNumber);
	
	if ( !User::QueryVersionSupported(v, aVersion) )
		{
		LEAVEIFERRORL(KErrNotSupported);
		}

	CHCIServer* ncThis = const_cast<CHCIServer*>(this);

	CHCISession* sess = CHCISession::NewL(*ncThis);
	LOG1(_L8("\tsess = 0x%08x"), sess);
	return sess;
	}

void CHCIServer::OpenClientReference()
	{
	iParent.OpenClientReference();
	}

void CHCIServer::CloseClientReference()
	{
	iParent.CloseClientReference();
	}
