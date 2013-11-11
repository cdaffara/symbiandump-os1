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
#include "privacyserver.h"
#include "privacysession.h"
#include "privacyshutdown.h"


/**
LBSPRIVACY_POLICY_ALLOW_COMMDD

If this is defined, clients need only the CommDD capability to 
use this server.

If undefined, then clients must have all of the following capabilities:
  Location
  ProtServ
  ReadDeviceData
  WriteDeviceData
*/
#define LBSPRIVACY_POLICY_ALLOW_COMMDD


/**
Define the TPolicy for this server.
*/
const TUint KRangeCount = 3;
const TInt KRanges[KRangeCount] = 
	{
	0,
	1, // 1-11
	12
	};
const TUint8 KElementsIndex[KRangeCount] = 
	{
	CPolicyServer::ENotSupported,
	CPolicyServer::ECustomCheck, // 1-11
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

_LIT(KPrivacyServerName,"LBSPrivacyServer");

/**
Static public constructor.

@param aProtocol Interface to send privacy requests to.

@return A new instance of this class.
*/
CPrivacyServer* CPrivacyServer::NewL(MPrivacyProtocol& aProtocol)
	{
	LBSLOG(ELogP1, "CPrivacyServer::NewL() Begin\n");
	CPrivacyServer* self = new(ELeave) CPrivacyServer(aProtocol);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();

	LBSLOG(ELogP1, "CPrivacyServer::NewL() End\n");
	return self;
	}


/**
Create a new session.

@param aVersion Version information.
@param aMessage Incoming message.

@return A new CPrivacySession object.
*/
CSession2* CPrivacyServer::NewSessionL(const TVersion& aVersion, const RMessage2& /*aMessage*/) const
	{
	LBSLOG(ELogP1, "CPrivacyServer::NewSessionL() Begin\n");

	// Check that the shutdown timer hasn't already requested
	// the LBS system to shutdown
	if (iShutdown->State() == CPrivacyShutdown::EShutdownStateShutdownRequested)
		{
		User::Leave(KErrServerTerminated);
		}

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
	
	// Need to castaway the const.
	CSession2* session = new (ELeave) CPrivacySession(iProtocol,apiType);

	LBSLOG(ELogP1, "CPrivacyServer::NewSessionL() End\n");
	return session;
	}


/**
Set the shutdown delay.
*/
void CPrivacyServer::SetShutdownDelay(TInt32 aShutdownDelay)
	{
	iShutdownDelay = aShutdownDelay;
	}

/**
Set the maximum number of network sessions allowed.
*/
void CPrivacyServer::SetMaxNetworkSessions(TInt aMaxNetworkSessions)
	{
	iMaxNetworkSessions = aMaxNetworkSessions;
	}

/**
Destructor.
*/
CPrivacyServer::~CPrivacyServer()
	{
	LBSLOG(ELogP1, "CPrivacyServer::~CPrivacyServer() Begin\n");
	delete iShutdown;
	LBSLOG(ELogP1, "CPrivacyServer::~CPrivacyServer() End\n");
	}
		

/**
Increment the session count.

This is to allow the server to keep track of the number of active sessions.
*/
void CPrivacyServer::IncSessionCount(TLbsProxyApiType aApiType)
	{
	LBSLOG(ELogP1, "CPrivacyServer::IncSessionCount() Begin\n");

	if (aApiType == ELbsProxyApiTypeLocal)
		{
		++iLocalSessionCount;
		}
	else if (aApiType == ELbsProxyApiTypeNetwork)
		{
		++iNetworkSessionCount;
		}

	if (iShutdown && ((iLocalSessionCount + iNetworkSessionCount) == 1))
		{
		// Cancel shutdown sequence as we have an active session
		LBSLOG(ELogP1, "CPrivacyServer::IncSessionCount() Cancelling shutdown timer\n");
		iShutdown->Cancel();
		}

	LBSLOG(ELogP1, "CPrivacyServer::IncSessionCount() End\n");
	}

	
/**
Decrement the session count.

This is to allow the server to keep track of the number of active sessions.
*/
void CPrivacyServer::DecSessionCount(TLbsProxyApiType aApiType)
	{
	LBSLOG(ELogP1, "CPrivacyServer::DecSessionCount() Begin\n");
	
	if (aApiType == ELbsProxyApiTypeLocal)
		{
		--iLocalSessionCount;
		}
	else if (aApiType == ELbsProxyApiTypeNetwork)
		{
		--iNetworkSessionCount;
		}

	if (iShutdown && ((iLocalSessionCount + iNetworkSessionCount) <= 0))
		{
		// Last session has disconnected, start shutdown timer
		LBSLOG(ELogP1, "CPrivacyServer::DecSessionCount() Session count zero - starting shutdown timer\n");
		iShutdown->Start(iShutdownDelay);
		}
	
	LBSLOG(ELogP1, "CPrivacyServer::DecSessionCount() End\n");
	}

/**
Determine if the maximum number of sessions has been reached for this request source

*/
TBool CPrivacyServer::MaxSessionsReached(TLbsProxyApiType aApiType) const
	{
	if (aApiType == ELbsProxyApiTypeNetwork)
		{
		return (iMaxNetworkSessions >= 0 && (iNetworkSessionCount >= iMaxNetworkSessions));
		}

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
CPolicyServer::TCustomResult CPrivacyServer::CustomSecurityCheckL(const RMessage2 &aMsg, TInt& /*aAction*/, TSecurityInfo& /*aMissing*/)
	{
	LBSLOG(ELogP1, "CPrivacyServer::CustomSecurityCheckL() Begin\n");
	switch(aMsg.Function())
		{
		case ELbsPrivacyServerConnect: 
		case ELbsPrivacyServerDisconnect:
			{
			if (KLocationPolicy().CheckPolicy(aMsg) &&
				KReadDeviceDataPolicy.CheckPolicy(aMsg))
				{
				LBSLOG(ELogP1, "CPrivacyServer::CustomSecurityCheckL() End (PASS)\n");
				return EPass;
				}

			#ifdef LBSPRIVACY_POLICY_ALLOW_COMMDD
				if (KCommDDPolicy.CheckPolicy(aMsg))
					{
					LBSLOG(ELogP1, "CPrivacyServer::CustomSecurityCheckL() End (PASS with CommDD)\n");
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
				LBSLOG(ELogP1, "CPrivacyServer::CustomSecurityCheckL() End (PASS)\n");
				return EPass;
				}

			#ifdef LBSPRIVACY_POLICY_ALLOW_COMMDD
				if (KCommDDPolicy.CheckPolicy(aMsg))
					{
					LBSLOG(ELogP1, "CPrivacyServer::CustomSecurityCheckL() End (PASS with CommDD)\n");
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
				LBSLOG(ELogP1, "CPrivacyServer::CustomSecurityCheckL() End (PASS)\n");
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
				LBSLOG(ELogP1, "CPrivacyServer::CustomSecurityCheckL() End (PASS)\n");
				return EPass;
				}
			break;
			}
		default:
			{
			return EFail;
			}
		}

	LBSLOG(ELogP1, "CPrivacyServer::CustomSecurityCheckL() End (FAIL)\n");
	return EFail;
	}


/**
Constructor.

@param aProtocol Interface to send privacy requests to.
*/
CPrivacyServer::CPrivacyServer(MPrivacyProtocol& aProtocol) :
	CPolicyServer(EPriorityStandard, KPolicy),
	iProtocol(aProtocol)
	{
	LBSLOG(ELogP1, "CPrivacyServer::CPrivacyServer()\n");
	}


/**
Second phase constructor.
*/
void CPrivacyServer::ConstructL()
	{
	LBSLOG(ELogP1, "CPrivacyServer::ConstructL() Begin\n");
	iShutdown = CPrivacyShutdown::NewL();
	StartL(KPrivacyServerName);
	LBSLOG(ELogP1, "CPrivacyServer::ConstructL() End\n");
	}
