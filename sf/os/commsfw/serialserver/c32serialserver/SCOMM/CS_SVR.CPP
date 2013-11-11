// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/** @file
 *
 * Implements CC32Server, CCommScheduler and some thread functions
 */

#include "CS_STD.H"
#include "cs_common.h"
#include <f32file.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <c32comm_internal.h>
#endif

#include "C32LOG.H"
#include "cs_thread.h"
#include "cs_roles.h"
#include "cs_glob.h"

//
// implementation of CC32Server
//

class CC32WorkerThread;
class CC32ThreadManager;

CC32Server::CC32Server(CC32WorkerThread* aOwnerThread, TInt aPriority) 
/**
 * Constructor.
 *
 * @param aPriority
 */
	: CPolicyServer(aPriority, iPolicy), 
	iOwnerThread(aOwnerThread)
	{
	C32LOG1(KC32Dealer, _L8("CC32Server::CC32Server()"));
	}

void CC32Server::ConstructL(CC32ThreadManager* aThreadManager)
/**
 * start the server
 */
	{
	iThreadManager = aThreadManager;
	C32LOG1(KC32Dealer, _L8("Starting Server"));
	StartL(KCommServerName());

#ifdef 	_DEBUG
	TInt processHandles;
	RThread thread;
	thread.HandleCount(processHandles, iThreadHandles);
#endif
	}

CC32Server::~CC32Server()
//
// Destructor.
//
	{
#ifdef _DEBUG
	RThread thread;
	TInt processHandles;
	TInt threadHandles;
	thread.HandleCount(processHandles, threadHandles);
	
	if (threadHandles != iThreadHandles)
		{
		C32LOG2(KC32Detail,_L8("CC32Server::~CC32Server() %d handles were leaked"), threadHandles - iThreadHandles);
		__ASSERT_DEBUG(EFalse, Fault(EDbgHandleLeak, _L("CC32Server::~CC32Server() Leaked a handle")));
		}
#endif
	}

CSession2* CC32Server::NewSessionL(const TVersion& aVersion,const RMessage2& /*aMessage*/) const
/**
 * Create a new client for this server.
 *
 * @param aVersion Client version number
 */
	{
	C32LOG1(KC32Dealer, _L8("CC32Server::NewSessionL()"));
	TVersion v(KEC32MajorVersionNumber, KEC32MinorVersionNumber, KEC32BuildVersionNumber);
	if (!User::QueryVersionSupported(v, aVersion))
		User::Leave(KErrNotSupported);
	
	CCommSession* session = CCommSession::NewL(iThreadManager);
	return session;
	}

CPolicyServer::TCustomResult CC32Server::CustomSecurityCheckL(const RMessage2& aMsg, TInt& /*aAction*/, TSecurityInfo& /*aMissing*/)
/**
Perform custom security policy check on an IPC.  Request from the CSY the capabilities required to process this call.

It is possible that in attempting to read the port name from the client that this might fail which will panic the client. This completes the message 
so to avoid a double-completion (which will panic us) we return EASync wich means the policy server will not do anything further with the message.

@param aMsg RMessage2 containing client capabilites and request IPC.
@param aAction A reference to the action to take if the security check fails.
@param aMissing  reference to the list of security attributes missing from the checked process.
@return EPass, EFail or EAsync
*/
	{
	C32LOG1(KC32Dealer, _L8("CC32Server::CustomSecurityCheckL()"));
	TSecurityPolicy csySecurityPolicy(TSecurityPolicy::EAlwaysFail);
	
	CCommSession* session=static_cast<CCommSession*>(aMsg.Session());
	CPolicyServer::TCustomResult retCode = EFail;
 	if(session)
 		{
 		TFullName name;	
		TUint port;
		TInt len;
		// Extract port name and number
		if(session->ExtractPortNameAndNumber(aMsg, name, port, len) == KErrNone)
			{
			CSerial *serial=NULL;
			TRAPD(res, serial=iThreadManager->GetSerialL(name.Left(len)));
			if (res==KErrNone)
				{
				//get policy from CSY
				csySecurityPolicy = serial->PortPlatSecCapability(port);			
				}	
			}
		}
	//check policy.  First check message handle is valid otherwise we panic ourselves.
	if(aMsg.Handle() != 0 && csySecurityPolicy.CheckPolicy(aMsg, __PLATSEC_DIAGNOSTIC_STRING("Checked in CC32Server::CustomSecurityCheckL")))
		{
		//The message has not been nulled and the policy has passed.
		retCode = EPass;
		}   
	else if(aMsg.Handle() ==0)
		{
		//The message has been nulled so stop policy server attempting further completion by returning EASync
		retCode = EAsync;
		} 	
 	return retCode;
	}


//
// implementation of CCommScheduler
//

CCommScheduler* CCommScheduler::New()
/**
 * Create and install the active scheduler.
 * Return the CCommScheduler*
 */
	{
	CCommScheduler* pA = new CCommScheduler;
	__ASSERT_ALWAYS(pA!=NULL, Fault(EMainSchedulerError));
	CCommScheduler::Install(pA);
	return pA;
	}


void CCommScheduler::Error(TInt aError) const
/**
 * Called if any RunL() method leaves.
 */
	{
	C32LOG2(KC32Dealer, _L8("CCommScheduler::Error() - a RunL or ServiceL has Left! panicking. Leavecode was: %d"),aError);
	(void)aError;
	Fault(EMainSchedulerError);
	}


// Definition of iPolicy dictating the capability checking for C32 server
const TUint CC32Server::iRangeCount = 7;

const TInt CC32Server::iRanges[iRangeCount] = 
	{
	0,		//range is 0-5 inclusive
	6,		//range is 6
	7,		//range is 7
	8,		//range is 8-49 inclusive
	50,		//range is 50
	51,		//range is 51
	52,		//range is 52-KMaxTInt inclusive
	};

const TUint8 CC32Server::iElementsIndex[iRangeCount] = 
	{
	0,
	1,
	CPolicyServer::ECustomCheck,
	2,
	CPolicyServer::ECustomCheck,
	3,
	CPolicyServer::ENotSupported,
	};


const CPolicyServer::TPolicyElement CC32Server::iElements[] = 
	{
	{ _INIT_SECURITY_POLICY_C1( ECapability_None), CPolicyServer::EFailClient},	// policy 0:  range 0 - 5
	{ _INIT_SECURITY_POLICY_C1( ECapabilityCommDD), CPolicyServer::EFailClient},	// policy 1:  range 6 - 6
	{ _INIT_SECURITY_POLICY_C1( ECapability_None), CPolicyServer::EFailClient},	// policy 2:  range 8 - 8
	{ _INIT_SECURITY_POLICY_C1( ECapability_None), CPolicyServer::EFailClient},	// policy 2:  range 51
	};


const CPolicyServer::TPolicy CC32Server::iPolicy = 
	{
	CPolicyServer::EAlwaysPass ,
	iRangeCount,
	iRanges,
	iElementsIndex,
	iElements
	};

// EOF - CS_SVR.CPP
