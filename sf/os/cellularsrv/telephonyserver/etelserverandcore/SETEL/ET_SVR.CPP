// Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ET_SVRTraces.h"
#endif

#include <e32svr.h>
#include "ET_SSTD.H"
#include <e32cmn.h>
#include "et_record.h"
#include <u32hal.h>

#ifdef __EABI__
extern const TInt32 KPriorityClientSid;
extern const TInt32 KLocationServicesNetworkGatewaySid;
extern const TInt32 KSatEngineSid;
#endif

//
// CDestroyDummySubSession class definitions
//

CDestroyDummySubSession* CDestroyDummySubSession::NewL(CTelServer* aTelServer,CTelObject* aTelObject)
//
//	Create an async one shot
//
	{
	return new(ELeave)CDestroyDummySubSession(aTelServer,aTelObject);
	}

CDestroyDummySubSession::CDestroyDummySubSession(CTelServer* aTelServer,CTelObject* aTelObject)
//
// C'tor
//
	:CAsyncOneShot(CActive::EPriorityLow),
	iTelObject(aTelObject), iTelServer(aTelServer)
	{
	__DECLARE_NAME(_S("CDestroyDummySubSession"));
	}

CDestroyDummySubSession::~CDestroyDummySubSession()
	{
	Cancel();
	}

void CDestroyDummySubSession::RunL()
//
// Destroy the tel object and the the server
//
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDESTROYDUMMYSUBSESSION_RUNL_1, "In RunL and about to Destroy the Tel Objects");
	iOpen=EFalse;
	iTelObject->TelObjectClose();
	}

//
// CTelChannelHandler class definitions
//

CTelChannelHandler::CTelChannelHandler(CTelServer* aServer):
    CCFModuleChannelHandler(CActive::EPriorityStandard)
	/** Constructor
	@internalComponent */
	{
	iServer=aServer;
	}

CTelChannelHandler* CTelChannelHandler::NewL(RCFChannel::TMsgQueues aRxQueues, 
    RCFChannel::TMsgQueues aTxQueues, CTelServer* aServer)
	/** Create channel handler instance and construct it. Standard pattern
	@internalComponent */
	{
	CTelChannelHandler* pHandler = new (ELeave) CTelChannelHandler(aServer);
	CleanupStack::PushL(pHandler);
	pHandler->ConstructL(aRxQueues, aTxQueues);
	CleanupStack::Pop(pHandler);
	return pHandler;
	}

void CTelChannelHandler::CFMessageShutdown(const CommsFW::TCFShutdownMsg& aMessage)
	{
	iServer->CFMessageShutdown(aMessage);
	}

void CTelServer::CFMessageShutdown(const CommsFW::TCFShutdownMsg& aMessage)
	/** 
	@internalComponent */
	{
	if((aMessage.Type()==EOptional) || (aMessage.Type()==EGraceful))
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CTELSERVER_CFMESSAGESHUTDOWN_1, "CTelServer received Shutdown. Scheduling shutdown when there's no sessions.");
		iShutGracefully = ETrue;
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CTELSERVER_CFMESSAGESHUTDOWN_2, "Session Count is %d", iSessionCount);
		if(iSessionCount==0)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CTELSERVER_CFMESSAGESHUTDOWN_3, "Calling AsynOneShot::Call()");
			iSch->Call();
			}
		}
	else
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CTELSERVER_CFMESSAGESHUTDOWN_4, "CTelServer received Shutdown option EUnGraceful. Not supported, ignoring.");
		}
	}

//
// CTelSchedulerStop class definitions
//

CTelSchedulerStop* CTelSchedulerStop::NewL()
//
//	Create the TelScheduler Stop async one shot
//
	{
	return new(ELeave)CTelSchedulerStop;
	}

CTelSchedulerStop::CTelSchedulerStop()
//
// C'tor
//
	:CAsyncOneShot(CActive::EPriorityLow)
	{
	__DECLARE_NAME(_S("CTelSchedulerStop"));
	}

CTelSchedulerStop::~CTelSchedulerStop()
	{
	Cancel();
	}

void CTelSchedulerStop::RunL()
//
// Stop the Active Scheduler (after any libraries have been unloaded)
//
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CTELSCHEDULERSTOP_RUNL_1, "In RunL and about to stop CActiveScheduler");
	CActiveScheduler::Stop();
	}

//
// CTelServer class definitions
//

CTelServer* CTelServer::NewL(RCFChannel::TMsgQueues aRxQueues, RCFChannel::TMsgQueues aTxQueues)
	{
	CTelServer* pS = new (ELeave) CTelServer(EPriority);
	CleanupStack::PushL(pS);
	pS->ConstructL(aRxQueues, aTxQueues);
	CleanupStack::Pop(pS);
	return pS;
	}

CTelServer::CTelServer(TInt aPriority)
	: CPolicyServer(aPriority, iPolicy, ESharableSessions), iSessionCount(0), iShutGracefully(EFalse)
	{
	__DECLARE_NAME(_S("CTelServer"));
	}

void CTelServer::ConstructL(RCFChannel::TMsgQueues aRxQueues, RCFChannel::TMsgQueues aTxQueues)
	{
	iPhoneManager=CPhoneManager::NewL();
	iSch=CTelSchedulerStop::NewL();
	iPriorityClientSession=NULL;
	iChannelHandler = CTelChannelHandler::NewL(aRxQueues, aTxQueues, this);
#if defined (__EABI__) 
	iPriorityClientSid = KPriorityClientSid;
	iLocationServicesNetworkGatewaySid = KLocationServicesNetworkGatewaySid;
	iSatEngineSid = KSatEngineSid;
#else // WINS
	// for the emulator the SecureId of Master phone application is patched via the epoc.ini file 
	UserSvr::HalFunction(EHalGroupEmulator,EEmulatorHalIntProperty,(TAny*)"etel_KPriorityClientSid",&iPriorityClientSid);
	// for the emulator the SecureId of Location Services Network Gateway process is patched via the epoc.ini file
	UserSvr::HalFunction(EHalGroupEmulator,EEmulatorHalIntProperty,(TAny*)"etel_KLocationServicesNetworkGatewaySid",&iLocationServicesNetworkGatewaySid);
	// for the emulator the SecureId of the SAT engine is patched via the epoc.ini file
	UserSvr::HalFunction(EHalGroupEmulator,EEmulatorHalIntProperty,(TAny*)"etel_KSatEngineSid",&iSatEngineSid);
#endif
    RECORD_HEADER
	StartL(ETEL_SERVER_NAME);
	}

CTelServer::~CTelServer()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CTELSERVER_DTOR_1, "CTelServer::~CTelServer()");
	delete iChannelHandler;
	delete iPhoneManager;
	delete iSch;
	iEmergencyClientSessions.Reset();
	}

CSession2* CTelServer::NewSessionL(const TVersion &aVersion,const RMessage2& aMessage) const
//
// Create a new client for this server.
//
	{
	TVersion v(KEtelMajorVersionNumber,KEtelMinorVersionNumber,KEtelBuildVersionNumber);
	if (User::QueryVersionSupported(v,aVersion)==FALSE)
		User::Leave(KErrNotSupported);
	CTelSession* sess = new(ELeave) CTelSession(iPhoneManager);	
	RECORD_SESSION_NEW(sess);
	RECORD_COMPLETE(sess,0,KErrNone);
	
#ifdef OST_TRACE_COMPILER_IN_USE
	/* Get name of the client. */
	RProcess process;
	RThread thread;
	User::LeaveIfError(aMessage.Client(thread));
	TInt err=thread.Process(process);
	thread.Close();
	User::LeaveIfError(err);
	TName cliName = process.Name();
	process.Close();
	    
	TInt index = cliName.LocateReverse('[');
	if (index >= 1)
	{
	   cliName.SetLength(index);
    }
	TBuf8<KMaxName> processName;
	processName.Copy(cliName);
#endif
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CTELSERVER_NEWSESSIONL_1, "CTelServer::NewSessionL - 0x%08X, Client Name: %s", (TUint)sess, processName);
	return sess;
	}

void CTelServer::Inc()
//
// Increase session count
//
	{
	if((iSessionCount==0)&&(iSch->IsActive()))
		{
		iSch->Cancel();
		}
	iSessionCount++;
	}

TInt CTelServer::Count() const
//
// Session count
//
	{
	return iSessionCount;
	}

void CTelServer::Dec()
//
// Decrement a session stop scheduler if no session
//
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CTELSERVER_DEC_1, "Entered CTelServer::Dec");
	__ASSERT_ALWAYS((iSessionCount>0),Fault(EEtelFaultNegativeSessionCount));
	iSessionCount--;
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CTELSERVER_DEC_2, "Session Count is %d", iSessionCount);
	// If iShutGracefully==ETrue the Rootserver has informed us that it wants us to shutdown
	// next time there is no sessions. If it is EFalse no such notification has arrived and 
	// ETel continues running even though there are no sessions.
	if((iSessionCount==0) && (iShutGracefully))
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CTELSERVER_DEC_3, "Calling AsynOneShot::Call()");
		iSch->Call();
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CTELSERVER_DEC_4, "Exited from CTelServer::Dec");
	}

TBool CTelServer::IsPriorityClient(const CTelSession* aSession) const
//
// Check if aSession is the priority client session
//
	{
	return (aSession==iPriorityClientSession);
	}

TInt CTelServer::SetPriorityClient(CTelSession* aSession)
//
// Set aSession as the priority client session if there is not one already.
// Return KErrAlreadyExists if there is one already, KErrNoMemory if the
// heap cannot be allocated.
//
	{
	if (iPriorityClientSession==NULL)
		{
		iPriorityClientSession=aSession;
		return KErrNone;
		}
	return KErrAlreadyExists;
	}

TInt CTelServer::RemovePriorityClient(CTelSession* aSession)
//
// Remove aSession as the priority client if it is the priority client
// Returns KErrAccessDenied if aSession is not the priority client
//
	{
	if (IsPriorityClient(aSession))
		{
		iPriorityClientSession=NULL;
		return KErrNone;
		}
	return KErrAccessDenied;
	}

CPolicyServer::TCustomResult CTelServer::CustomSecurityCheckL(const RMessage2& aMsg, TInt& /*aAction*/, TSecurityInfo& /*aMissing*/)
/**
Performs a custom security policy check on an IPC. Requests from the TSY 
the capabilities required to service the specified IPC.

In the case of Etel RTelServer::SetPriorityClient and 
RTelServer::SetPriorityClientV2 APIs which has IPC opcodes 
EEtelServerSetPriorityClient, EEtelServerSetPriorityClientV2 respectively 
a combination of NetworkControl and SID of the phone application is checked.

In the case of ETelMM RMobileLocationServices::Open call which has IPC opcode 
EEtelOpenFromSubSessionLcs, the SecureId of Location Services Network 
Gateway is checked.

For ETelMM RMobileCall::DialCallControl, which has IPC 
EMobileCallDialCallControl, a check for NetworkServices and NetworkControl 
capabilities is made and the SID of the SAT engine is checked.

@param aMsg RMessage2 containing client capabilites and request IPC.
@param aAction A reference to the action to take if the security check fails.
@param aMissing  reference to the list of security attributes missing from the checked process.
@return CPolicyServer::EPass or CPolicyServer::EFail
*/

	{
	//Set the defualt required capability to always fail the check
	TSecurityPolicy requiredCapabilities(TSecurityPolicy::EAlwaysFail);
	TCustomResult retCode = EFail;
	
	CTelSession* session=static_cast<CTelSession*>(aMsg.Session());
	if(session)
		{
		switch(aMsg.Function())
			{
		// RTelServer::SetPriorityClient is policed with NetworkControl capability and SecureId of master phone application
		case EEtelServerSetPriorityClient:
		case EEtelServerSetPriorityClientV2:
			{
			TSecurityPolicy securityPolicy(ECapabilityNetworkControl);
			if(securityPolicy.CheckPolicy(aMsg) && aMsg.SecureId() == iPriorityClientSid)
				{
				retCode = EPass;	
				}
			break;
			}
		// RMobileLocationServices::Open call is policed with SecureId of Location Services Network Gateway process
		case EEtelOpenFromSubSessionLcs:
			{
			if(aMsg.SecureId() == iLocationServicesNetworkGatewaySid)
				{
				retCode = EPass;
				}
			break;
			}
		// RMobileCall::DialCallControl and cancellation of this
		// request is policed by NetworkControl and 
		// NetworkServices capability and the SecureId of the SAT engine.
		case EMobileCallDialCallControl:
		case EMobileCallDialCallControlCancel:		
			{
			TSecurityPolicy securityPolicy(ECapabilityNetworkControl, ECapabilityNetworkServices);
			if (securityPolicy.CheckPolicy(aMsg) && aMsg.SecureId() == iSatEngineSid)
				{
				retCode = EPass;
				}
			break;
			}
		default:
			{
			CTelObject* theObj=session->CObjectFromHandle(aMsg.Int3());
			if(theObj)
				{
				//Get the policy for the incoming custom IPC request from the TSY
				requiredCapabilities = theObj->GetRequiredPlatSecCaps(aMsg.Function()); 
				}
			//Check the capability of the client against the required capability 
			//retrieved from the TSY
			if(requiredCapabilities.CheckPolicy(aMsg))
				{
				retCode = EPass;
				}
			break;	
			}
			}
		}
	return retCode;
	}
	
void CTelServer::SetEmergencyClientSession(CTelSession* aEmergencyClientSession)
	{
	iEmergencyClientSessions.Append(aEmergencyClientSession);
	}

TInt CTelServer::EmergencyClientSessionsCount()
	{
	return iEmergencyClientSessions.Count();
	}

TInt CTelServer::RemoveEmergencyClient(CTelSession* aSession)
	{
	for(int i =0;i<iEmergencyClientSessions.Count();i++)
		{
		if(iEmergencyClientSessions[i] == aSession)
			{
			iEmergencyClientSessions.Remove(i);
			return KErrNone;
			}
		}
	return KErrAccessDenied;
	}

TBool CTelServer::IsEmergencyClient(const CTelSession* aSession) const
	{
	for(int i =0;i<iEmergencyClientSessions.Count();i++)
		{
		if(iEmergencyClientSessions[i] == aSession)
			{
			return ETrue;
			}
		}
	return EFalse;
	}


//
// CTelScheduler class definitions
//

CTelScheduler* CTelScheduler::New()
//
// Create and install the active scheduler.
//
	{
	CTelScheduler* pA=new CTelScheduler;
	__ASSERT_ALWAYS(pA!=NULL,Fault(EEtelFaultMainSchedulerError));
	CTelScheduler::Install(pA);
	return pA;
	}

void CTelScheduler::Error(TInt) const
//
// Called if any Run() method leaves.
//
	{
	Fault(EEtelFaultMainSchedulerError);
	}

EXPORT_C TInt EtelServerThread(TAny* aArg)
//
// The ETel Server Thread.
//
	{
	// RThread::SetProtected is superceded by the security model
	
	__UHEAP_MARK;
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDERS, ETELSERVERTHREAD_1, "----------New Log----------\015\012");
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDERS, ETELSERVERTHREAD_2, "Entered ETel Server thread");
	OstTraceDef0(OST_TRACE_CATEGORY_PRODUCTION, TRACE_BORDERS, ETELSERVERTHREAD_3, "Entered ETel Server thread");
	
	CTrapCleanup* pT;
	if ((pT=CTrapCleanup::New())==NULL)
		{
		Fault(EEtelFaultCreateTrapCleanup);
		}
	
	//
	// Start the scheduler and then the server
	//
	CTelScheduler* pScheduler = CTelScheduler::New();
	if(!pScheduler)
		{
		Fault(EEtelFaultMainSchedulerError);
		}
	
	TCFModuleInfo* pParameters = (TCFModuleInfo*) aArg;
	CTelServer* pServer=NULL;
	TRAPD(err, pServer=CTelServer::NewL(pParameters->iRxQueues, pParameters->iTxQueues));
	
	if(err!=KErrNone)
		{
		Fault(EEtelFaultSvrStartServer);
		}

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDERS, ETELSERVERTHREAD_4, "ETel:\tMeeting rendezvous\n");
	RThread::Rendezvous(KErrNone);
	CTelScheduler::Start();
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDERS, ETELSERVERTHREAD_5, "ETel:\tScheduler has been stopped\n");

	delete pT;
	delete pServer;
	delete pScheduler;

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDERS, ETELSERVERTHREAD_6, "ETel:\tAbout to exit ETel thread function\n");
	OstTraceDef0(OST_TRACE_CATEGORY_PRODUCTION, TRACE_BORDERS, ETELSERVERTHREAD_7, "ETel:\tAbout to exit ETel thread function\n");
	__UHEAP_MARKEND;

	return(KErrNone);
	}

//Definition of ETel's policy table -
//Dicates what capability checking takes place for any given IPC request

const TUint CTelServer::iRangeCount = 332;
const TInt CTelServer::iRanges[iRangeCount] = 
	{
	0,		//range is 0-61 inclusive
	62,		//range is 62-70 inclusive
	71,		//range is 71-72 inclusive
	73,		//range is 73
	74,		//range is 74
	75,		//range is 75-76 inclusive
	77,		//range is 77
	78,		//range is 78-84 inclusive
	85,		//range is 85
	86,		//range is 86
	87,		//range is 87
	88,		//range is 88
	89,		//range is 89-91 inclusive
	92,		//range is 92-505 inclusive
	506,		//range is 506
	507,		//range is 507-507 inclusive
	508,		//range is 508
	509,		//range is 509-509 inclusive
	510,		//range is 510
	511,		//range is 511-519 inclusive
	520,		//range is 520
	521,		//range is 521-521 inclusive
	522,		//range is 522
	523,		//range is 523-523 inclusive
	524,		//range is 524
	525,		//range is 525-532 inclusive
	533,		//range is 533
	534,		//range is 534-539 inclusive
	540,		//range is 540
	541,		//range is 541-541 inclusive
	542,		//range is 542
	543,		//range is 543-543 inclusive
	544,		//range is 544
	545,		//range is 545-550 inclusive
	551,		//range is 551
	552,		//range is 552-562 inclusive
	563,		//range is 563
	564,		//range is 564-564 inclusive
	565,		//range is 565
	566,		//range is 566-566 inclusive
	567,		//range is 567
	568,		//range is 568-570 inclusive
	571,		//range is 571
	572,		//range is 572-574 inclusive
	575,		//range is 575
	576,		//range is 576-577 inclusive
	578,		//range is 578
	579,		//range is 579-579 inclusive
	580,		//range is 580
	581,		//range is 581-581 inclusive
	582,		//range is 582
	583,		//range is 583-584 inclusive
	585,		//range is 585
	586,		//range is 586
	587,		//range is 587-590 inclusive
	591,		//range is 591
	592,		//range is 592-19999 inclusive
	20000,		//range is 20000-20118 inclusive
	20119,		//range is 20119-20500 inclusive
	20501,		//range is 20501-20502 inclusive
	20503,		//range is 20503-20507 inclusive
	20508,		//range is 20508-20516 inclusive
	20517,		//range is 20517-20519 inclusive
	20520,		//range is 20520-20522 inclusive
	20523,		//range is 20523-20523 inclusive
	20524,		//range is 20524-20525 inclusive
	20526,		//range is 20526-20529 inclusive
	20530,		//range is 20530-20532 inclusive
	20533,		//range is 20533-20533 inclusive
	20534,		//range is 20534-20535 inclusive
	20536,		//range is 20536-20537 inclusive
	20538,		//range is 20538
	20539,		//range is 20539-20539 inclusive
	20540,		//range is 20540-20541 inclusive
	20542,		//range is 20542-20543 inclusive
	20544,		//range is 20544-20546 inclusive
	20547,		//range is 20547-20547 inclusive
	20548,		//range is 20548-20550 inclusive
	20551,		//range is 20551-20553 inclusive
	20554,		//range is 20554-20556 inclusive
	20557,		//range is 20557-20557 inclusive
	20558,		//range is 20558
	20559,		//range is 20559-20559 inclusive
	20560,		//range is 20560-20601 inclusive
	20602,		//range is 20602-20603 inclusive
	20604,		//range is 20604
	20605,		//range is 20605-20606 inclusive
	20607,		//range is 20607-20609 inclusive
	20610,		//range is 20610-20610 inclusive
	20611,		//range is 20611-20618 inclusive
	20619,		//range is 20619-20999 inclusive
	21000,		//range is 21000-21030 inclusive
	21031,		//range is 21031-21499 inclusive
	21500,		//range is 21500-21508 inclusive
	21509,		//range is 21509-21509 inclusive
	21510,		//range is 21510-21511 inclusive
	21512,		//range is 21512-21513 inclusive
	21514,		//range is 21514-21521 inclusive
	21522,		//range is 21522-21522 inclusive
	21523,		//range is 21523-21524 inclusive
	21525,		//range is 21525-21525 inclusive
	21526,		//range is 21526
	21527,		//range is 21527-21528 inclusive
	21529,		//range is 21529-21530 inclusive
	21531,		//range is 21531-21999 inclusive
	22000,		//range is 22000-22003 inclusive
	22004,		//range is 22004
	22005,		//range is 22005-22008 inclusive
	22009,		//range is 22009
	22010,		//range is 22010-22011 inclusive
	22012,		//range is 22012
	22013,		//range is 22013-22015 inclusive
	22016,		//range is 22016
	22017,		//range is 22017-22057 inclusive
	22058,		//range is 22058-22500 inclusive
	22501,		//range is 22501-22503 inclusive
	22504,		//range is 22504
	22505,		//range is 22505-22507 inclusive
	22508,		//range is 22508-22508 inclusive
	22509,		//range is 22509
	22510,		//range is 22510-22510 inclusive
	22511,		//range is 22511
	22512,		//range is 22512
	22513,		//range is 22513-22514 inclusive
	22515,		//range is 22515-22515 inclusive
	22516,		//range is 22516
	22517,		//range is 22517-22524 inclusive
	22525,		//range is 22525-22525 inclusive
	22526,		//range is 22526
	22527,		//range is 22527-22527 inclusive
	22528,		//range is 22528
	22529,		//range is 22529-22529 inclusive
	22530,		//range is 22530
	22531,		//range is 22531-22531 inclusive
	22532,		//range is 22532-22543 inclusive
	22544,		//range is 22544-22546 inclusive
	22547,		//range is 22547-22552 inclusive
	22553,		//range is 22553-22553 inclusive
	22554,		//range is 22554-22556 inclusive
	22557,		//range is 22557-22999 inclusive
	23000,		//range is 23000-23028 inclusive
	23029,		//range is 23029-23030 inclusive
	23031,		//range is 23031-23035 inclusive
	23036,		//range is 23036-23499 inclusive
	23500,		//range is 23500-23502 inclusive
	23503,		//range is 23503-23503 inclusive
	23504,		//range is 23504-23528 inclusive
	23529,		//range is 23529-23530 inclusive
	23531,		//range is 23531-23535 inclusive
	23536,		//range is 23536-23999 inclusive
	24000,		//range is 24000-24010 inclusive
	24011,		//range is 24011
	24012,		//range is 24012-24014 inclusive
	24015,		//range is 24015
	24016,		//range is 24016-24019 inclusive
	24020,		//range is 24020-24500 inclusive
	24501,		//range is 24501
	24502,		//range is 24502-24502 inclusive
	24503,		//range is 24503-24508 inclusive
	24509,		//range is 24509-24509 inclusive
	24510,		//range is 24510
	24511,		//range is 24511
	24512,		//range is 24512-24514 inclusive
	24515,		//range is 24515
	24516,		//range is 24516-24519 inclusive
	24520,		//range is 24520-24999 inclusive
	25000,		//range is 25000-25007 inclusive
	25008,		//range is 25008-25499 inclusive
	25500,		//range is 25500-25507 inclusive
	25508,		//range is 25508-25999 inclusive
	26000,		//range is 26000-26003 inclusive
	26004,		//range is 26004-26499 inclusive
	26500,		//range is 26500-26503 inclusive
	26504,		//range is 26504-26999 inclusive
	27000,		//range is 27000-27006 inclusive
	27007,		//range is 27007-27499 inclusive
	27500,		//range is 27500-27506 inclusive
	27507,		//range is 27507-27999 inclusive
	28000,		//range is 28000-28002 inclusive
	28003,		//range is 28003-28499 inclusive
	28500,		//range is 28500-28502 inclusive
	28503,		//range is 28503-28999 inclusive
	29000,		//range is 29000-29004 inclusive
	29005,		//range is 29005-29499 inclusive
	29500,		//range is 29500
	29501,		//range is 29501-29501 inclusive
	29502,		//range is 29502-29504 inclusive
	29505,		//range is 29505-29999 inclusive
	30000,		//range is 30000-30002 inclusive
	30003,		//range is 30003-30499 inclusive
	30500,		//range is 30500-30502 inclusive
	30503,		//range is 30503-30999 inclusive
	31000,		//range is 31000-31015 inclusive
	31016,		//range is 31016-31499 inclusive
	31500,		//range is 31500
	31501,		//range is 31501-31501 inclusive
	31502,		//range is 31502
	31503,		//range is 31503-31503 inclusive
	31504,		//range is 31504
	31505,		//range is 31505-31505 inclusive
	31506,		//range is 31506
	31507,		//range is 31507-31507 inclusive
	31508,		//range is 31508
	31509,		//range is 31509-31509 inclusive
	31510,		//range is 31510-31511 inclusive
	31512,		//range is 31512-31513 inclusive
	31514,		//range is 31514
	31515,		//range is 31515-31999 inclusive
	32000,		//range is 32000-32001 inclusive
	32002,		//range is 32002-32499 inclusive
	32500,		//range is 32500-32501 inclusive
	32502,		//range is 32502-32999 inclusive
	33000,		//range is 33000-33010 inclusive
	33011,		//range is 33011-33499 inclusive
	33500,		//range is 33500-33510 inclusive
	33511,		//range is 33511-33999 inclusive
	34000,		//range is 34000-34003 inclusive
	34004,		//range is 34004-34499 inclusive
	34500,		//range is 34500-34503 inclusive
	34504,		//range is 34504-34999 inclusive
	35000,		//range is 35000-35009 inclusive
	35010,		//range is 35010-35499 inclusive
	35500,		//range is 35500-35507 inclusive
	35508,		//range is 35508-35508 inclusive
	35509,		//range is 35509
	35510,		//range is 35510-39999 inclusive
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	40000,		//range is 40000-40029 inclusive
	40030,		//range is 40030-40499 inclusive
#else 
	40000,		//range is 40000-40027 inclusive
	40028,		//range is 40028-40499 inclusive
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	40500,		//range is 40500
	40501,		//range is 40501-40503 inclusive
	40504,		//range is 40504-40510 inclusive
	40511,		//range is 40511-40511 inclusive
	40512,		//range is 40512-40514 inclusive
	40515,		//range is 40515-40516 inclusive
	40517,		//range is 40517-40524 inclusive
	40525,		//range is 40525-40525 inclusive
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	40526,		//range is 40526-40529 inclusive
	40530,		//range is 40530-40999 inclusive
#else 
	40526,		//range is 40526-40527 inclusive
	40528,		//range is 40528-40999 inclusive
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	41000,		//range is 41000-41011 inclusive
	41012,		//range is 41012-41499 inclusive
	41500,		//range is 41500-41511 inclusive
	41512,		//range is 41512-41999 inclusive
	42000,		//range is 42000-42016 inclusive
	42017,		//range is 42017-42499 inclusive
	42500,		//range is 42500-42511 inclusive
	42512,		//range is 42512-42512 inclusive
	42513,		//range is 42513-42515 inclusive
	42516,		//range is 42516-42999 inclusive
	43000,		//range is 43000-43005 inclusive
	43006,		//range is 43006-43499 inclusive
	43500,		//range is 43500-43505 inclusive
	43506,		//range is 43506-43999 inclusive
	44000,		//range is 44000-44001 inclusive
	44002,		//range is 44002-44499 inclusive
	44500,		//range is 44500-44501 inclusive
	44502,		//range is 44502-44999 inclusive
	45000,		//range is 45000-45001 inclusive
	45002,		//range is 45002-45499 inclusive
	45500,		//range is 45500-45501 inclusive
	45502,		//range is 45502-45999 inclusive
	46000,		//range is 46000-46006 inclusive
	46007,		//range is 46007-46499 inclusive
	46500,		//range is 46500-46506 inclusive
	46507,		//range is 46507-59999 inclusive
	60000,		//range is 60000-60027 inclusive
	60028,		//range is 60028-60499 inclusive
	60500,		//range is 60500-60527 inclusive
	60528,		//range is 60528-60999 inclusive
	61000,		//range is 61000-61021 inclusive
	61022,		//range is 61022-61499 inclusive
	61500,		//range is 61500-61521 inclusive
	61522,		//range is 61522-61999 inclusive
	62000,		//range is 62000
	62001,		//range is 62001-62499 inclusive
	62500,		//range is 62500
	62501,		//range is 62501-62999 inclusive
	63000,		//range is 63000-63001 inclusive
	63002,		//range is 63002-63499 inclusive
	63500,		//range is 63500
	63501,		//range is 63501-63999 inclusive
	64000,		//range is 64000-64004 inclusive
	64005,		//range is 64005-64499 inclusive
	64500,		//range is 64500-64504 inclusive
	64505,		//range is 64505-64999 inclusive
	65000,		//range is 65000-65004 inclusive
	65005,		//range is 65005-65500 inclusive
	65501,		//range is 65501-65504 inclusive
	65505,		//range is 65505-65999 inclusive
	66000,		//range is 66000
	66001,		//range is 66001-66499 inclusive
	66500,		//range is 66500
	66501,		//range is 66501-79999 inclusive
	80000,		//range is 80000-80018 inclusive
	80019,		//range is 80019-80499 inclusive
	80500,		//range is 80500-80518 inclusive
	80519,		//range is 80519-80999 inclusive
	81000,		//range is 81000-81001 inclusive
	81002,		//range is 81002-81499 inclusive
	81500,		//range is 81500-81501 inclusive
	81502,		//range is 81502-81999 inclusive
	82000,		//range is 82000-82002 inclusive
	82003,		//range is 82003-82499 inclusive
	82500,		//range is 82500
	82501,		//range is 82501-82501 inclusive
	82502,		//range is 82502
	82503,		//range is 82503-82999 inclusive
	83000,		//range is 83000-83007 inclusive
	83008,		//range is 83008-83499 inclusive
	83500,		//range is 83500-83507 inclusive
	83508,		//range is 83508-83999 inclusive
	84000,		//range is 84000
	84001,		//range is 84001-84499 inclusive
	84500,		//range is 84500
	84501,		//range is 84501-84999 inclusive
	85000,		//range is 85000-85001 inclusive
	85002,		//range is 85002-85499 inclusive
	85500,		//range is 85500-85501 inclusive
	85502,		//range is 85502-85999 inclusive
	86000,		//range is 86000
	86001,		//range is 86001-86499 inclusive
	86500,		//range is 86500
	86501,		//range is 86501-86999 inclusive
	87000,		//range is 87000
	87001,		//range is 87001-87499 inclusive
	87500,		//range is 87500
	87501,		//range is 87501-87999 inclusive
	88000,		//range is 88000-88001 inclusive
	88002,		//range is 88002-88499 inclusive
	88500,		//range is 88500-88501 inclusive
	88502,		//range is 88502-999999 inclusive
	1000000,		//range is 1000000-1000999 inclusive
	1001000,		//range is 1001000-KMaxTInt inclusive
	};



const TUint8 CTelServer::iElementsIndex[iRangeCount] = 
	{
	11,
	5,
	11,
	14,
	0,
	11,
	CPolicyServer::ECustomCheck,
	1,
	8,
	10,
	11,
	14,
	CPolicyServer::ECustomCheck,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	1,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	5,
	CPolicyServer::ENotSupported,
	5,
	CPolicyServer::ENotSupported,
	5,
	CPolicyServer::ENotSupported,
	5,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	1,
	CPolicyServer::ENotSupported,
	1,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	8,
	10,
	CPolicyServer::ENotSupported,
	CPolicyServer::ECustomCheck,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	5,
	CPolicyServer::ENotSupported,
	5,
	CPolicyServer::ENotSupported,
	5,
	CPolicyServer::ENotSupported,
	5,
	CPolicyServer::ENotSupported,
	5,
	CPolicyServer::ENotSupported,
	5,
	CPolicyServer::ENotSupported,
	5,
	CPolicyServer::ENotSupported,
	12,
	11,
	12,
	11,
	12,
	11,
	12,
	11,
	12,
	CPolicyServer::ENotSupported,
	12,
	11,
	12,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	12,
	11,
	12,
	CPolicyServer::ENotSupported,
	11,
	12,
	CPolicyServer::ENotSupported,
	12,
	CPolicyServer::ENotSupported,
	12,
	CPolicyServer::ENotSupported,
	12,
	CPolicyServer::ENotSupported,
	12,
	CPolicyServer::ENotSupported,
	12,
	CPolicyServer::ENotSupported,
	12,
	CPolicyServer::ENotSupported,
	14,
	4,
	14,
	CPolicyServer::ENotSupported,
	14,
	CPolicyServer::ENotSupported,
	14,
	4,
	14,
	CPolicyServer::ENotSupported,
	13,
	11,
	13,
	11,
	13,
	CPolicyServer::ENotSupported,
	13,
	CPolicyServer::ENotSupported,
	13,
	CPolicyServer::ENotSupported,
	13,
	11,
	13,
	11,
	13,
	CPolicyServer::ENotSupported,
	15,
	CPolicyServer::ENotSupported,
	15,
	CPolicyServer::ENotSupported,
	12,
	CPolicyServer::ENotSupported,
	12,
	CPolicyServer::ENotSupported,
	1,
	CPolicyServer::ENotSupported,
	1,
	CPolicyServer::ENotSupported,
	2,
	CPolicyServer::ENotSupported,
	2,
	CPolicyServer::ENotSupported,
	3,
	CPolicyServer::ENotSupported,
	3,
	CPolicyServer::ENotSupported,
	3,
	CPolicyServer::ENotSupported,
	4,
	CPolicyServer::ENotSupported,
	4,
	CPolicyServer::ENotSupported,
	6,
	CPolicyServer::ENotSupported,
	6,
	CPolicyServer::ENotSupported,
	6,
	CPolicyServer::ENotSupported,
	6,
	CPolicyServer::ENotSupported,
	6,
	CPolicyServer::ENotSupported,
	6,
	CPolicyServer::ENotSupported,
	6,
	CPolicyServer::ENotSupported,
	6,
	CPolicyServer::ENotSupported,
	8,
	CPolicyServer::ENotSupported,
	8,
	CPolicyServer::ENotSupported,
	9,
	CPolicyServer::ENotSupported,
	9,
	CPolicyServer::ENotSupported,
	10,
	CPolicyServer::ENotSupported,
	10,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	5,
	CPolicyServer::ENotSupported,
	5,
	CPolicyServer::ENotSupported,
	12,
	CPolicyServer::ENotSupported,
	12,
	CPolicyServer::ENotSupported,
	12,
	CPolicyServer::ENotSupported,
	14,
	CPolicyServer::ENotSupported,
	14,
	CPolicyServer::ENotSupported,
	13,
	CPolicyServer::ENotSupported,
	13,
	CPolicyServer::ENotSupported,
	6,
	CPolicyServer::ENotSupported,
	6,
	CPolicyServer::ENotSupported,
	9,
	CPolicyServer::ENotSupported,
	9,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	12,
	CPolicyServer::ENotSupported,
	12,
	CPolicyServer::ENotSupported,
	14,
	CPolicyServer::ENotSupported,
	14,
	CPolicyServer::ENotSupported,
	0,
	CPolicyServer::ENotSupported,
	0,
	CPolicyServer::ENotSupported,
	3,
	CPolicyServer::ENotSupported,
	3,
	CPolicyServer::ENotSupported,
	4,
	CPolicyServer::ENotSupported,
	4,
	CPolicyServer::ENotSupported,
	9,
	CPolicyServer::ENotSupported,
	9,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	11,
	CPolicyServer::ENotSupported,
	5,
	CPolicyServer::ENotSupported,
	5,
	CPolicyServer::ENotSupported,
	12,
	CPolicyServer::ENotSupported,
	12,
	CPolicyServer::ENotSupported,
	12,
	CPolicyServer::ENotSupported,
	14,
	CPolicyServer::ENotSupported,
	14,
	CPolicyServer::ENotSupported,
	13,
	CPolicyServer::ENotSupported,
	13,
	CPolicyServer::ENotSupported,
	1,
	CPolicyServer::ENotSupported,
	1,
	CPolicyServer::ENotSupported,
	7,
	CPolicyServer::ENotSupported,
	7,
	CPolicyServer::ENotSupported,
	8,
	CPolicyServer::ENotSupported,
	8,
	CPolicyServer::ENotSupported,
	9,
	CPolicyServer::ENotSupported,
	9,
	CPolicyServer::ENotSupported,
	CPolicyServer::ECustomCheck,
	CPolicyServer::ENotSupported,
	};


const CPolicyServer::TPolicyElement CTelServer::iElements[] = 
	{
	{ _INIT_SECURITY_POLICY_C1( ECapabilityNetworkControl), CPolicyServer::EFailClient},
	{ _INIT_SECURITY_POLICY_C2( ECapabilityNetworkControl, ECapabilityNetworkServices), CPolicyServer::EFailClient},
	{ _INIT_SECURITY_POLICY_C3( ECapabilityNetworkControl, ECapabilityNetworkServices, ECapabilityWriteDeviceData), CPolicyServer::EFailClient},
	{ _INIT_SECURITY_POLICY_C2( ECapabilityNetworkControl, ECapabilityReadDeviceData), CPolicyServer::EFailClient},
	{ _INIT_SECURITY_POLICY_C2( ECapabilityNetworkControl, ECapabilityWriteDeviceData), CPolicyServer::EFailClient},
	{ _INIT_SECURITY_POLICY_C1( ECapabilityNetworkServices), CPolicyServer::EFailClient},
	{ _INIT_SECURITY_POLICY_C2( ECapabilityNetworkServices, ECapabilityReadDeviceData), CPolicyServer::EFailClient},
	{ _INIT_SECURITY_POLICY_C3( ECapabilityNetworkServices, ECapabilityReadDeviceData, ECapabilityWriteDeviceData), CPolicyServer::EFailClient},
	{ _INIT_SECURITY_POLICY_C2( ECapabilityNetworkServices, ECapabilityReadUserData), CPolicyServer::EFailClient},
	{ _INIT_SECURITY_POLICY_C2( ECapabilityNetworkServices, ECapabilityWriteDeviceData), CPolicyServer::EFailClient},
	{ _INIT_SECURITY_POLICY_C2( ECapabilityNetworkServices, ECapabilityWriteUserData), CPolicyServer::EFailClient},
	{ _INIT_SECURITY_POLICY_C1( ECapability_None), CPolicyServer::EFailClient},
	{ _INIT_SECURITY_POLICY_C1( ECapabilityReadDeviceData), CPolicyServer::EFailClient},
	{ _INIT_SECURITY_POLICY_C1( ECapabilityReadUserData), CPolicyServer::EFailClient},
	{ _INIT_SECURITY_POLICY_C1( ECapabilityWriteDeviceData), CPolicyServer::EFailClient},
	{ _INIT_SECURITY_POLICY_C1( ECapabilityWriteUserData), CPolicyServer::EFailClient},
	};


const CPolicyServer::TPolicy CTelServer::iPolicy = 
	{
	CPolicyServer::EAlwaysPass,
	iRangeCount,
	iRanges,
	iElementsIndex,
	iElements
	};
