// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "lbsdevloggermacros.h"
#include <lbs/lbsloccommon.h>
#include "suplproxyserver.h"
#include "suplproxyprivacysession.h"
#include "suplproxypossession.h"

/**
LBSSUPLPROXY_POLICY_ALLOW_COMMDD

If this is defined, clients need only the CommDD capability to 
use this server.

If undefined, then clients must have all of the following capabilities:
  Location
  ProtServ
  ReadDeviceData
  WriteDeviceData
*/
#define LBSSUPLPROXY_POLICY_ALLOW_COMMDD


/**
Define the TPolicy for this server.
*/
const TUint KRangeCount = 3;
const TInt KRanges[KRangeCount] = 
	{
	0,
	1, // 1-22
	23 
	};
const TUint8 KElementsIndex[KRangeCount] = 
	{
	CPolicyServer::ENotSupported,
	CPolicyServer::ECustomCheck, // 1-21
	CPolicyServer::ENotSupported 
	};
const CPolicyServer::TPolicy KPolicy = 
	{
	CPolicyServer::EAlwaysPass,
	KRangeCount,
	KRanges,
	KElementsIndex,
	NULL
	};

// NB the name matches that of the LBS Privacy Server to allow common API class
_LIT(KSuplProxyServerName,"LBSPrivacyServer");

/**
Static public constructor.

@param aPrivacyProtocol Interface to send supl proxy requests to.

@return A new instance of this class.
*/
CSuplProxyServer* CSuplProxyServer::NewL(MSuplProxyPrivacyProtocol& aPrivacyProtocol, MSuplProxyPosProtocol& aPosProtocol, 
										TInt aRefLocSourceId)
	{
	LBSLOG(ELogP1, "CSuplProxyServer::NewL() Begin\n");
	CSuplProxyServer* self = new(ELeave) CSuplProxyServer(aPrivacyProtocol, aPosProtocol, aRefLocSourceId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();

	LBSLOG(ELogP1, "CSuplProxyServer::NewL() End\n");
	return self;
	}


/**
Create a new session.

@param aVersion Version information.
@param aMessage Incoming message.

@return A new CSuplProxyPrivacySession object.
*/
CSession2* CSuplProxyServer::NewSessionL(const TVersion& aVersion, const RMessage2& /*aMessage*/) const
	{
	LBSLOG(ELogP1, "CSuplProxyServer::NewSessionL() Begin\n");

	TLbsProxyApiType apiType = ELbsProxyApiTypeUnknown;
	
	// Check the client version is supported.
	// We also use this to distinguish between local and network sessions.
	if (aVersion.iMajor == KLbsLocalPrivacyAPIVersionMajor &&
		aVersion.iMinor == KLbsLocalPrivacyAPIVersionMinor &&
		aVersion.iBuild == KLbsLocalPrivacyAPIVersionBuild)
		{
		apiType = ELbsProxyApiTypeLocal;
		}
	else if (aVersion.iMajor == KLbsNetworkPrivacyAPIVersionMajor &&
			  aVersion.iMinor == KLbsNetworkPrivacyAPIVersionMinor &&
			  aVersion.iBuild == KLbsNetworkPrivacyAPIVersionBuild)
		{
		apiType = ELbsProxyApiTypeNetwork;
		}
	else if(aVersion.iMajor == KLbsSuplProxyPosAPIVersionMajor &&
            aVersion.iMinor == KLbsSuplProxyPosAPIVersionMinor &&
            aVersion.iBuild == KLbsSuplProxyPosAPIVersionBuild)
	    {
	    apiType = ELbsProxyApiTypePos;
	    }
	else
		{
		// Unknown client version
		User::Leave(KErrNotSupported);
		}
 
	// Check we aren't about to exceed the maximum number of sessions for this request source
	if (MaxSessionsReached(apiType))
		{
		User::Leave(KErrInUse);
		}
	
	CSession2* session = NULL;
	
	if(apiType == ELbsProxyApiTypePos)
	    {
	    //session = new (ELeave) CSuplProxyPosSession(iPosProtocol);
	    session = CSuplProxyPosSession::NewL(iPosProtocol, iRefLocSourceId);
	    }
	else
	    {
        // Need to cast away the const.
        session = new (ELeave) CSuplProxyPrivacySession(iPrivacyProtocol, apiType);
	    }
	
	LBSLOG(ELogP1, "CSuplProxyServer::NewSessionL() End\n");
	return session;
	}


/**
Set the maximum number of network sessions allowed.
*/
void CSuplProxyServer::SetMaxNetworkSessions(TInt aMaxNetworkSessions)
	{
	iMaxNetworkSessions = aMaxNetworkSessions;
	}

/**
Destructor.
*/
CSuplProxyServer::~CSuplProxyServer()
	{
	LBSLOG(ELogP1, "CSuplProxyServer::~CSuplProxyServer() Begin and End\n");
	}
		

/**
Increment the session count.

This is to allow the server to keep track of the number of active sessions.
*/
void CSuplProxyServer::IncSessionCount(TLbsProxyApiType aApiType)
	{
	LBSLOG(ELogP1, "CSuplProxyServer::IncSessionCount() Begin\n");

	if (aApiType == ELbsProxyApiTypeLocal)
		{
		++iLocalSessionCount;
		}
	else if (aApiType == ELbsProxyApiTypeNetwork)
		{
		++iNetworkSessionCount;
		}
	else   // ELbsProxyApiTypePos
	    {
	    __ASSERT_DEBUG(aApiType == ELbsProxyApiTypePos, User::Invariant());
	    ++iPosSessionCount;
	    }

	LBSLOG(ELogP1, "CSuplProxyServer::IncSessionCount() End\n");
	}

	
/**
Decrement the session count.

This is to allow the server to keep track of the number of active sessions.
*/
void CSuplProxyServer::DecSessionCount(TLbsProxyApiType aApiType)
	{
	LBSLOG(ELogP1, "CSuplProxyServer::DecSessionCount() Begin\n");
	
	if (aApiType == ELbsProxyApiTypeLocal)
		{
		--iLocalSessionCount;
		}
	else if (aApiType == ELbsProxyApiTypeNetwork)
		{
		--iNetworkSessionCount;
		}
	else   // ELbsProxyApiTypePos
	    {
	    __ASSERT_DEBUG(aApiType == ELbsProxyApiTypePos, User::Invariant());
	    --iPosSessionCount;
	    }
	
	LBSLOG(ELogP1, "CSuplProxyServer::DecSessionCount() End\n");
	}

/**
Determine if the maximum number of sessions has been reached for this request source

*/
TBool CSuplProxyServer::MaxSessionsReached(TLbsProxyApiType aApiType) const
	{
	LBSLOG(ELogP1, "CSuplProxyServer::MaxSessionsReached() Begin\n");

	if (aApiType == ELbsProxyApiTypeNetwork)
		{
		return (iMaxNetworkSessions >= 0 && (iNetworkSessionCount >= iMaxNetworkSessions));
		}
	else if(aApiType == ELbsProxyApiTypePos)
	    {
	    return (iPosSessionCount > 0); // only allow one. 
	    }

	LBSLOG(ELogP1, "CSuplProxyServer::MaxSessionsReached() End\n");

	// Unlimited number of local sessions allowed
	return EFalse;
	}

/**
Security policies used for custom security check below:
*/
static _LIT_SECURITY_POLICY_C1(KLocationPolicy, ECapabilityLocation);
static _LIT_SECURITY_POLICY_C1(KReadDeviceDataPolicy, ECapabilityReadDeviceData);
static _LIT_SECURITY_POLICY_C1(KCommDDPolicy, ECapabilityCommDD);
static _LIT_SECURITY_POLICY_C1(KNetworkServicesPolicy, ECapabilityNetworkServices);


/**
Perform a custom security check.

@param aMsg The message to check.
@param aAction A reference to the action to take if the security check fails.
@param aMissing A reference to the list of security attributes missing from the checked process.

@return Enum specifying the result of the security check.
*/
CPolicyServer::TCustomResult CSuplProxyServer::CustomSecurityCheckL(const RMessage2 &aMsg, TInt& /*aAction*/, TSecurityInfo& /*aMissing*/)
	{
	LBSLOG(ELogP1, "CSuplProxyServer::CustomSecurityCheckL() Begin\n");

	switch(aMsg.Function())
		{				
		case ELbsPrivacyServerConnect: 
		case ELbsPrivacyServerDisconnect:
        case ELbsSuplProxyServerPosOpen:
        case ELbsSuplProxyServerPosClose:
        case ELbsSuplProxyServerPosResetRequestId:
        case ELbsSuplProxyServerGetPosition:
        case ELbsSuplProxyServerCancelGetPosition:	
        case ELbsSuplProxyServerPosNotifySessionComplete:   
        case ELbsSuplProxyServerPosPayload: 
        case ELbsSuplProxyServerGetSuplInfo:
        case ELbsSuplProxyServerCancelGetSuplInfo:
        case ELbsSuplProxyServerSetPosMethod:
        case ELbsSuplProxyServerSetSlpAddress:
			{
			if (KLocationPolicy().CheckPolicy(aMsg) &&
				KReadDeviceDataPolicy.CheckPolicy(aMsg))
				{
				LBSLOG(ELogP1, "CSuplProxyServer::CustomSecurityCheckL() End (PASS)\n");
				return EPass;
				}

			#ifdef LBSSUPLPROXY_POLICY_ALLOW_COMMDD  
				if (KCommDDPolicy.CheckPolicy(aMsg))
					{
					LBSLOG(ELogP1, "CSuplProxyServer::CustomSecurityCheckL() End (PASS with CommDD)\n");
					return EPass;
					}
			#endif
			break;
			}
		case ELbsPrivacyServerNotifyLocation:
		case ELbsPrivacyServerVerifyLocation:
		case ELbsPrivacyServerNotifyVerificationTimeout:
		case ELbsPrivacyServerCancelVerifyLocationRequest:
			{
			if (KLocationPolicy().CheckPolicy(aMsg) &&
				KReadDeviceDataPolicy.CheckPolicy(aMsg) &&
				KNetworkServicesPolicy.CheckPolicy(aMsg))
				{
				LBSLOG(ELogP1, "CSuplProxyServer::CustomSecurityCheckL() End (PASS)\n");
				return EPass;
				}

			#ifdef LBSSUPLPROXY_POLICY_ALLOW_COMMDD
				if (KCommDDPolicy.CheckPolicy(aMsg))
					{
					LBSLOG(ELogP1, "CSuplProxyServer::CustomSecurityCheckL() End (PASS with CommDD)\n");
					return EPass;
					}
			#endif
			break;
			}
		case ELbsPrivacyServerNewPrivacyRequestLocal:
		case ELbsPrivacyServerRepeatPrivacyRequestLocal:
		case ELbsPrivacyServerCompleteRequest:
			{
			if (KLocationPolicy().CheckPolicy(aMsg) &&
				KReadDeviceDataPolicy.CheckPolicy(aMsg))
				{
				LBSLOG(ELogP1, "CSuplProxyServer::CustomSecurityCheckL() End (PASS)\n");
				return EPass;
				}
			break;
			}
		case ELbsPrivacyServerNewPrivacyRequestNetwork:
		case ELbsPrivacyServerRepeatPrivacyRequestNetwork: 
			{
			if (KLocationPolicy().CheckPolicy(aMsg) &&
				KReadDeviceDataPolicy.CheckPolicy(aMsg) &&
				KNetworkServicesPolicy.CheckPolicy(aMsg))
				{
				LBSLOG(ELogP1, "CSuplProxyServer::CustomSecurityCheckL() End (PASS)\n");
				return EPass;
				}
			break;
			}		    
		default:
			{
			return EFail;
			}
		}

	LBSLOG(ELogP1, "CSuplProxyServer::CustomSecurityCheckL() End (FAIL)\n");

	return EFail;
	}


/**
Constructor.

@param aPrivacyProtocol Interface to send supl proxy requests to.
*/
CSuplProxyServer::CSuplProxyServer(MSuplProxyPrivacyProtocol& aPrivacyProtocol, MSuplProxyPosProtocol& aPosProtocol, 
							TInt aRefLocSourceId) :
	CPolicyServer(EPriorityStandard, KPolicy),
	iPrivacyProtocol(aPrivacyProtocol), iPosProtocol(aPosProtocol), iRefLocSourceId(aRefLocSourceId)
	{
	LBSLOG(ELogP1, "CSuplProxyServer::CSuplProxyServer()\n");
	}


/**
Second phase constructor.
*/
void CSuplProxyServer::ConstructL()
	{
	LBSLOG(ELogP1, "CSuplProxyServer::ConstructL() Begin\n");

	StartL(KSuplProxyServerName);

	LBSLOG(ELogP1, "CSuplProxyServer::ConstructL() End\n");
	}
