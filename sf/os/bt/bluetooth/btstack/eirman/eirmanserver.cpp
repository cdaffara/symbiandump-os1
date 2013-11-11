// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "eirmanserver.h"
#include <e32base.h>
#include <bluetooth/logger.h>
#include <bluetooth/logger/components.h>
#include <bluetooth/eirmanshared.h>
#include "eirmansession.h"
#include "eirmanager.h"
#include "eirmanserversecuritypolicy.h"
#include "linkmgr.h"
#include "eirmanpanics.h"


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_EIRMANAGER);
#endif

CEirManServer* CEirManServer::NewL(MHCICommandQueue& aCommandQueue, CLinkMgrProtocol& aLinkMgrProtocol)
	{
	LOG_STATIC_FUNC
	
	CEirManServer* self = new(ELeave) CEirManServer(aCommandQueue, aLinkMgrProtocol);
	CleanupStack::PushL(self);
	// StartL is where the kernel checks that there isn't already an instance 
	// of the same server running, so do it before ConstructL.
	self->StartL(KEirManServerName);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CEirManServer::~CEirManServer()
	{
	LOG_FUNC
	delete iEirManager;
	}

CEirManServer::CEirManServer(MHCICommandQueue& aCommandQueue, CLinkMgrProtocol& aLinkMgrProtocol)
	: CPolicyServer(CActive::EPriorityStandard, KEirManServerPolicy)
	, iCommandQueue(aCommandQueue)
	, iLinkMgrProtocol(aLinkMgrProtocol)
	, iInternalSessionCount(0)
	, iExternalSessionCount(0)
	, iIsFeaturesReady(EFalse)
	, iIsEirSupported(EFalse)
	, iSessions(_FOFF(CEirManSession, iLink))
	{
	LOG_FUNC
	}

void CEirManServer::ConstructL()
	{
	LOG_FUNC
	}

CSession2* CEirManServer::NewSessionL(const TVersion& aVersion, const RMessage2& /*aMessage*/) const
	{
	LOG_FUNC
	// Server will refuse to create any more session if we have found out eir isn't supported.
	if(iIsFeaturesReady && !iIsEirSupported)
		{
		User::Leave(KErrNotSupported);
		}
	
	LOG3(_L("CEirManServer::NewSessionL aVersion = (%d,%d,%d)"), aVersion.iMajor, aVersion.iMinor, aVersion.iBuild);
	
	// Version number check...
	TVersion v(KEirManSrvMajorVersionNumber,
			   KEirManSrvMinorVersionNumber,
			   KEirManSrvBuildVersionNumber);
	
	if ( !User::QueryVersionSupported(v, aVersion) )
		{
		LEAVEIFERRORL(KErrNotSupported);
		}
	
	CEirManServer* ncThis = const_cast<CEirManServer*>(this);
	
	CEirManExternalSession* sess = CEirManExternalSession::NewL(*ncThis);
	LOG1(_L("\tsess = 0x%08x"), sess);
	return sess;
	}

CEirManInternalSession* CEirManServer::NewInternalSessionL(MEirInternalSessionNotifier& aParent)
	{
	LOG_FUNC
	// Server will refuse to create any more session if we have found out eir isn't supported.
	if(iIsFeaturesReady && !iIsEirSupported)
		{
		User::Leave(KErrNotSupported);
		}
	CEirManInternalSession* sess = CEirManInternalSession::NewL(*this, aParent);
	return sess;
	}

void CEirManServer::AddSession(CEirManSession& aSession, TBool aInternalSession)
	{
	LOG_FUNC
	
	iSessions.AddLast(aSession);
	
	if (aInternalSession)
		{
		if(iInternalSessionCount++ == 0)
			{
			// While we have clients we need to make sure that the protocol remains alive.
			iLinkMgrProtocol.LocalOpen();
			}
		}
	else
		{
		if(iExternalSessionCount++ == 0)
			{
			// While we have clients we need to make sure that the protocol remains alive.
			iLinkMgrProtocol.Open();
			}
		}
	}

void CEirManServer::DropSession(TBool aInternalSession)
	{
	LOG_FUNC
	if (aInternalSession)
		{
		if(--iInternalSessionCount == 0)
			{
			iLinkMgrProtocol.LocalClose();
			}
		}
	else
		{
		if(--iExternalSessionCount == 0)
			{
			iLinkMgrProtocol.Close();
			}		
		}
	}

void CEirManServer::NotifyFeaturesReady()
	{
	if(iIsFeaturesReady == EFalse)
		{
		__ASSERT_DEBUG(!iEirManager, EIR_SERVER_PANIC(EEirServerEirMangerAlreadyExists));
		TRAPD(err, iEirManager = CEirManager::NewL(iCommandQueue, iLinkMgrProtocol));
		iIsFeaturesReady = ETrue;
		
		TDblQueIter<CEirManSession> sessionIter(iSessions);
		
		sessionIter.SetToFirst();
		CEirManSession* sessionPtr;
		if(iLinkMgrProtocol.IsExtendedInquiryResponseSupportedLocally() && err == KErrNone)
			{
			iIsEirSupported = ETrue;
			}
		else
			{
			err = ((err != KErrNone) ? KErrNoMemory : KErrNotSupported);
			}
		
		while((sessionPtr = sessionIter++) != NULL)
			{
			sessionPtr->NotifyEirFeatureState(err);
			}
		}
	}

TEirFeatureState CEirManServer::EirFeatureState()
	{
	TEirFeatureState ret = EEirFeaturePending;
	if(iIsFeaturesReady && iIsEirSupported)
		{
		ret = EEirFeatureReady;
		}
	else if(iIsFeaturesReady && !iIsEirSupported)
		{
		ret = EEirFeatureNotSupported;
		}
	else
		{
		// ret will still be EEirFeaturePending
		}
	return ret;
	}

CPolicyServer::TCustomResult CEirManServer::CustomSecurityCheckL(const RMessage2& aMsg, TInt& /*aAction*/, TSecurityInfo& /*aMissing*/)
	{
	TEirManMessages function = static_cast<TEirManMessages>(aMsg.Function());
	TEirTag tag;
	TCustomResult result = EFail; //Fail everything by default
	_LIT_SECURITY_POLICY_S0(KSDPSecurityPolicy, KSDPServerID);
	_LIT_SECURITY_POLICY_S0(KStackSecurityPolicy, KStackID);
	_LIT_SECURITY_POLICY_C1(KVendorSpecificDataSecurityPolicy, ECapabilityWriteDeviceData);
	_LIT_SECURITY_POLICY_C1(KEirCommonSecurityPolicy, ECapabilityLocalServices);

	if(KEirCommonSecurityPolicy.CheckPolicy(aMsg))
		{
		if(function == EEirManRegisterTag)
			{
			tag = static_cast<TEirTag>(aMsg.Int0());
			switch(tag)
				{
				case EEirTagName:
				case EEirTagTxPowerLevel:
					/** These must have come from the stack **/
					if(KStackSecurityPolicy.CheckPolicy(aMsg))
						{
						result = EPass;
						}
					break;
				case EEirTagSdpUuid16:
				case EEirTagSdpUuid32:
				case EEirTagSdpUuid128:
					/** These must have come from SDP server **/
					if(KSDPSecurityPolicy.CheckPolicy(aMsg))
						{
						result = EPass;
						}
					break;
				case EEirTagManufacturerSpecific:
					/** To do this you must have write device data **/
					if(KVendorSpecificDataSecurityPolicy.CheckPolicy(aMsg))
						{
						result = EPass;
						}
					break;
				case EEirTagFlags:
					/** At present no implementation of Flags is supported. 
					So we are rejecting this until an implementation is provided. **/
				default: //unknown or reserved tag, reject
					//no need to do anything 
					break;
				}
			}
		}
	//Anything not covered by the above is invalid so do nothing and let it fail
	if(result == EFail)
		{
		LOG1(_L("CEirManServer::CustomSecurityCheckL failed. Function: %d"), function);
		}
	return result;
	}
