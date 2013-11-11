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
// server side exmaple of how to use the abstract server framework
// 
//

#include <e32debug.h>
#include "lbsdevloggermacros.h"

#include "lbsnrhprivacycontrollerserver.h"
#include "lbsnrhx3pserver.h"
#include "opensessionparams.h"
#include "lbsnrhserverdata.h"
#include "lbsnrhmessageenums.h"

const TInt KLbsLogQueueSize = 30;

//
// Security policy data structures
//

const TUint myRangeCount = 7;

const TInt myRanges[myRangeCount] = 
    {
    0, //range is 0-1 inclusive
    ESecureSubSessionBaseClose, //range is 2 to (ENrhPrivacyControllerSubSessionFirstMessageId - 1)
    ENrhPrivacyControllerRegister, // Register a MLbsPrivacyObserver for callbacks
    ENrhPrivacyControllerResponse, // All other CLbsPrivacyController functions
    ENrhPrivacyControllerSubSessionLastMessageId,  // Range between last privacy handler message
    											   // and first X3P message.
    EX3pSubSessionFirstMessageId,	// All X3P messages
    EX3pSubSessionLastMessageId	// Remaining messages.
    };
const TUint8 myElementsIndex[myRangeCount] = 
    {
    CPolicyServer::ECustomCheck,
    CPolicyServer::ENotSupported,
    0,
    1,
    CPolicyServer::ENotSupported,
    2,
    CPolicyServer::ENotSupported
    };
    
const CPolicyServer::TPolicyElement myElements[] = 
    {
    {_INIT_SECURITY_POLICY_C2(ECapabilityLocation, ECapabilityReadDeviceData), CPolicyServer::EFailClient},
    {_INIT_SECURITY_POLICY_C2(ECapabilityLocation, ECapabilityWriteDeviceData), CPolicyServer::EFailClient},
    {_INIT_SECURITY_POLICY_C2(ECapabilityLocation, ECapabilityNetworkServices), CPolicyServer::EFailClient}
    };
    
const CPolicyServer::TPolicy myPolicy =
    {
    CPolicyServer::EAlwaysPass, //specifies all connect attempts should pass
    myRangeCount,                   
    myRanges,
    myElementsIndex,
    myElements,
    };

//
// Server Framework Function Implementations
//

MCreateServerImpl* CSecureASBase::GetImplLC()
	{
	CNrhServerImpl* impl = new(ELeave) CNrhServerImpl();
	CleanupStack::PushL(impl);
	return impl;
	}

void CNrhServerImpl::CreateServerLC(TServerStartParams& aParams)
	{
	CNrhServer* s = new(ELeave) CNrhServer(CActive::EPriorityStandard, myPolicy);
	CleanupStack::PushL(s);
	s->ConstructL(aParams.GetServerName());
	// leave the server object on the CS
	}
	
//
// CNrhServer
//

RLbsLogger CNrhServer::iLogger;

CNrhServer::CNrhServer(TInt aPriority, const CPolicyServer::TPolicy& aSecurityPolicy) 
:	CSecureServerBase(aPriority, aSecurityPolicy),
	iVersion(KNrhServerMajorVersionNumber,
			 KNrhServerMinorVersionNumber,
			 KNrhServerBuildVersionNumber)
	{		
	}
	
CNrhServer::~CNrhServer()
	{
	iLogger.Close();
	
	iX3pObserver = NULL;
	iPrivacyObserver = NULL;

	delete iNetworkRequestHandler;
	delete iCloseDownRequestDetector;
	}
	
void CNrhServer::ConstructL(const TDesC& aServerName)
	{			
	StartL(aServerName);
	BaseConstructL(EFalse); // MUST BE CALLED. No shutdown timer. Makes the NRH a perm. server	
	
    // Create the monitor which detects a closedown signal from
    // the LBS Root Process.
    iCloseDownRequestDetector = CLbsCloseDownRequestDetector::NewL(
    								this, 
									KLbsNetRequestHandlerUid);

	iNetworkRequestHandler = CLbsNetworkRequestHandler::NewL(this);	
	
	iLogger.Open(KLbsLogQueueSize);
	}
	
CSession2* CNrhServer::DoNewSessionL(const TVersion& /*aVersion*/, const RMessage2& /*aMessage*/) const
	{
	return new (ELeave) CNrhServerSession(); // version number already checked at this point
	}
	
	
TVersion CNrhServer::GetServerVersion() const
	{
	return iVersion;
	}

/**
Security policies used for custom security check below:
*/
static _LIT_SECURITY_POLICY_C1(KLocationPolicy, ECapabilityLocation);
static _LIT_SECURITY_POLICY_C1(KReadDeviceDataPolicy, ECapabilityReadDeviceData);
static _LIT_SECURITY_POLICY_C1(KNetworkServicesPolicy, ECapabilityNetworkServices);


/**
Perform a custom security check.

A custom security check is only needed for the sub-session connect
message. The reason is that the two type of sub-sessions the NRH supports
(X3P and privacy controller) have different PlatSec capabilities to
create them.

@param aMsg The message to check.
@param aAction A reference to the action to take if the security check fails.
@param aMissing A reference to the list of security attributes missing from the checked process.

@return Enum specifying the result of the security check.
*/
CPolicyServer::TCustomResult CNrhServer::CustomSecurityCheckL(const RMessage2 &aMsg, TInt& /*aAction*/, TSecurityInfo& /*aMissing*/)
	{
	LBSLOG(ELogP1, "CNrhServer::CustomSecurityCheckL() Begin\n");

	TNrhSubSessionType subSessType;
	TPckg<TNrhSubSessionType> subSessTypeBuf(subSessType);
	MessageUtils::Read(aMsg, 0, subSessTypeBuf); // data is always in slot zero
		
	// Capabilities to check depend on the type of sub-session
	CPolicyServer::TCustomResult result(EFail);
	switch (subSessType)
		{
		case ENrhSubSessionPrivacyController:
			{
			if (KLocationPolicy().CheckPolicy(aMsg) 
				&& KReadDeviceDataPolicy.CheckPolicy(aMsg))
				{
				LBSLOG(ELogP1, "CNrhServer::CustomSecurityCheckL() End (PASS)\n");
				result = EPass;
				}
			break;
			}
		case ENrhSubSessionX3P:
			{
			if (KLocationPolicy().CheckPolicy(aMsg) 
				&& KNetworkServicesPolicy().CheckPolicy(aMsg))
				{
				LBSLOG(ELogP1, "CNrhServer::CustomSecurityCheckL() End (PASS)\n");
				result = EPass;
				}
			break;
			}
		case ENrhSubSessionUnknown:
		default:
			{
			LBSLOG(ELogP1, "CNrhServer::CustomSecurityCheckL() End (FAIL)\n");
			result = EFail;
			break;
			}
		}

	return result;
	}

/* Called when LbsRoot has requested that the NRH should 
   closedown.
*/
void CNrhServer::OnProcessCloseDown()
    {
    CActiveScheduler::Stop();
    }
    
MX3pHandlerNotifier* CNrhServer::X3pObserver() const
	{
	return(iX3pObserver);
	}

void CNrhServer::SetPrivacyServerObserver(MPrivacyHandlerObserver* aObserver)
	{
	iPrivacyObserver = aObserver;
	}

MPrivacyHandlerObserver* CNrhServer::PrivacyObserver() const
	{
	return(iPrivacyObserver);
	}

CLbsAdmin* CNrhServer::Admin() const
	{
	return iNetworkRequestHandler->Admin();
	}

RLbsNetworkRegistrationStatus& CNrhServer::NetworkRegistrationStatus()
	{
	return iNetworkRequestHandler->NetworkRegistrationStatus();
	}

void CNrhServer::SetX3pServerObserver(MX3pHandlerNotifier* aObserver)
	{
	iX3pObserver = aObserver;
	}

CNrhServerSession::~CNrhServerSession()
	{
	}
	
// called by the CServer2 code to complete construction of the session 
void CNrhServerSession::CreateL()
	{
	CSecureSessionSubSessionBase::CreateL(); // MUST do this
	}
	
void CNrhServerSession::ServiceMessageL(const RMessage2& /*aMessage*/)
	{
	// Got a message from the client
	}
	
void CNrhServerSession::ServiceMessageError(const RMessage2& /*aMessage*/, const TInt /*aError*/)
	{
	// service leaves from the above in here
	}
	
	
//------------------------------------------------------------------	

MSubSessionImpl* CSubSessionBase::GetImplL(const RMessage2& aMessage)
	{	
	// Read in the subsession type
	TNrhSubSessionType subSessType;
	TPckg<TNrhSubSessionType> subSessTypeBuf(subSessType);
	MessageUtils::Read(aMessage, 0, subSessTypeBuf); // data is always in slot zero
		
	// Create the subsession, depending on the type requested
	MSubSessionImpl* subSession(NULL);
	switch (subSessType)
		{
		case ENrhSubSessionPrivacyController:
			{
			subSession = CNrhPrivacyServerSubsession::NewL();
			break;
			}
		case ENrhSubSessionX3P:
			{
			subSession = CNrhX3pServerSubsession::NewL(CNrhServer::iLogger);
			break;
			}
		case ENrhSubSessionUnknown:
		default:
			{
			_LIT(KPanicNrhServer, "Unknown SubSession Type");
			MessageUtils::PanicClient(aMessage, KPanicNrhServer, 0);
			break;
			}
		}
		
	return subSession;
	}

