// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the avdtp logical channel factory
// which creates the logical (L2CAP) channels for transport and signalling channels
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/logger.h>
#include <bluetoothav.h>

#include "bt.h"
#include "avdtpLogicalChannelFactory.h"
#include "gavdpinterface.h"
#include "avdtputil.h"
#include "avdtp.h"
#include "avdtpDirectChannel.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVDTP);
#endif

CLogicalChannelFactory* CLogicalChannelFactory::NewL(CBluetoothProtocolBase& aProtocol, CProtocolBase& aSAPFactory)
	{
	LOG_STATIC_FUNC
	CLogicalChannelFactory* f = new (ELeave) CLogicalChannelFactory(aProtocol, aSAPFactory);
	CleanupStack::PushL(f);
	f->ConstructL();
	CleanupStack::Pop(f);
	return f;
	}
	

CLogicalChannelFactory::CLogicalChannelFactory(CBluetoothProtocolBase& aProtocol,
												CProtocolBase& aSAPFactory)
: iProtocol(aProtocol), iBearerSAPFactory(aSAPFactory),
  iUnclaimedLogicalChannels(_FOFF(CManagedLogicalChannel, iFactoryQLink)),
  iPendingActiveJobs(_FOFF(CLogicalChannelFactoryRequest, iFactoryQLink)),
  iPendingPassiveSignallingJobs(_FOFF(CLogicalChannelFactoryRequest, iFactoryQLink)),
  iPendingPassiveSessionJobs(_FOFF(CLogicalChannelFactoryRequest, iFactoryQLink)),
  iCloseChannelJobs(_FOFF(CLogicalChannelFactoryRequest, iFactoryQLink))
	{
	LOG_FUNC
	}

CLogicalChannelFactory::~CLogicalChannelFactory()
	{
	LOG_FUNC
	iProtocol.StopProtocolListening();
	// iterator through jobs and destroy - they'll take down unclaimed channels
	TSglQueIter<CLogicalChannelFactoryActiveRequest> iter(iPendingActiveJobs);
	
	while (iter)
		{
		delete iter++;
		}

	TSglQueIter<CCloseSessionLogicalChannels> closeIter(iCloseChannelJobs);
	while(closeIter)
		{
		delete closeIter++;
		}
		
	TDblQueIter<CManagedLogicalChannel> unclaimedChannelsIter(iUnclaimedLogicalChannels);
	
	while (unclaimedChannelsIter)
		{
		delete unclaimedChannelsIter++;
		}
	
	// We shouldn't be deleted when we have a job outstanding
	__ASSERT_DEBUG(!iAsyncTryNextJob->IsActive(), Panic(EAvdtpLogicalChannelFactoryDeletionWhileJobPending));
	delete iAsyncTryNextJob;
	delete iCurrentActiveJob;
	}
	
void CLogicalChannelFactory::ConstructL()
	{
	LOG_FUNC
	TCallBack callback(TryNextJob, this);
	iAsyncTryNextJob = new(ELeave) CAsyncCallBack(callback, CActive::EPriorityStandard);
	}


CCreateSignallingLogicalChannel* CCreateSignallingLogicalChannel::NewLC(const TBTDevAddr& aAddr,
																		XLogicalChannelFactoryClient& aClient,
																		TLogicalChannelFactoryRequestId aId,
																		CLogicalChannelFactory& aLogicalChannelFactory)
	{
	LOG_STATIC_FUNC
	CCreateSignallingLogicalChannel* r = new (ELeave) CCreateSignallingLogicalChannel(aAddr, aClient, aId);
	CleanupStack::PushL(r);
	r->ConstructL(aLogicalChannelFactory); // create the channel holders
	return r;
	}

CCreateSignallingLogicalChannel* CCreateSignallingLogicalChannel::NewL(const TBTDevAddr& aAddr,
																	   XLogicalChannelFactoryClient& aClient,
																	   TLogicalChannelFactoryRequestId aId,
																	   CLogicalChannelFactory& aLogicalChannelFactory)
	{
	LOG_STATIC_FUNC
	CCreateSignallingLogicalChannel* r =
							CCreateSignallingLogicalChannel::NewLC(aAddr,
																   aClient,
																   aId,
																   aLogicalChannelFactory);
	CleanupStack::Pop();
	return r;
	}
	
	
void CCreateSignallingLogicalChannel::ConstructL(CLogicalChannelFactory& aLogicalChannelFactory)
	{
	LOG_FUNC
	iLogicalChannels[0] = CManagedLogicalChannel::NewL(aLogicalChannelFactory,
														iRemoteDev,
														KInitialSequenceNumber,
														iId);
	}

	
CCreateSignallingLogicalChannel::CCreateSignallingLogicalChannel(const TBTDevAddr& aAddr, XLogicalChannelFactoryClient& aClient, TLogicalChannelFactoryRequestId aId)
: CLogicalChannelFactoryActiveRequest(aAddr, aClient, aId)
	{
	LOG_FUNC
	// this job has one channel
	iNumChannelsRequired = 1;
	// so SAP[0] is a signalling SAP
	}
		
CCreateSessionLogicalChannels* CCreateSessionLogicalChannels::NewLC(const TBTDevAddr& aAddr,
													XLogicalChannelFactoryClient& aClient,
													TLogicalChannelFactoryRequestId aId,
													TInt aNumRequired)
	{
	LOG_STATIC_FUNC
	CCreateSessionLogicalChannels* r =
		new (ELeave) CCreateSessionLogicalChannels(aAddr, aClient, aId, aNumRequired);
	CleanupStack::PushL(r);
	return r;
	}

CCreateSessionLogicalChannels* CCreateSessionLogicalChannels::NewL(const TBTDevAddr& aAddr,
												   XLogicalChannelFactoryClient& aClient,
												   TLogicalChannelFactoryRequestId aId,
												   TInt aNumRequired)
	{
	LOG_STATIC_FUNC
	CCreateSessionLogicalChannels* r = new (ELeave) CCreateSessionLogicalChannels(aAddr,
														aClient,
														aId,
														aNumRequired);
	return r;
	}
	
CCreateSessionLogicalChannels::CCreateSessionLogicalChannels(const TBTDevAddr& aAddr,
											 XLogicalChannelFactoryClient& aClient,
											 TLogicalChannelFactoryRequestId aId,
											 TInt aNumRequired)
: CLogicalChannelFactoryActiveRequest(aAddr, aClient, aId)
	{
	LOG_FUNC
	// this job has one, two or three channels - but all need to come up for success
	iNumChannelsRequired = aNumRequired;
	}

//************************************************************************
// Close Logical Channels
//************************************************************************

CCloseSessionLogicalChannels* CCloseSessionLogicalChannels::NewL(XLogicalChannelFactoryClient& aClient,
												   TLogicalChannelFactoryRequestId aId)
	{
	LOG_STATIC_FUNC
	CCloseSessionLogicalChannels* r =  CCloseSessionLogicalChannels::NewLC(aClient, aId);
	CleanupStack::Pop(r);
	return r;
	}

CCloseSessionLogicalChannels* CCloseSessionLogicalChannels::NewLC(XLogicalChannelFactoryClient& aClient,
												   TLogicalChannelFactoryRequestId aId)
	{
	LOG_STATIC_FUNC
	CCloseSessionLogicalChannels* r = new (ELeave) CCloseSessionLogicalChannels(aClient,
														aId);
	CleanupStack::PushL(r);
	return r;
	}

CCloseSessionLogicalChannels::~CCloseSessionLogicalChannels()
	{
	LOG_STATIC_FUNC
	BTSocketTimer::Remove(iTimerEntry);
	CloseChannels(EFalse);
	}

/**
One of this job's logical channels has been closed by the remote.  If this
is the final remaining channel the job is completed.

@param aChannel The channel that has been closed.
*/
void CCloseSessionLogicalChannels::ChannelClosed(CManagedLogicalChannel* aChannel)
	{
	LOG_FUNC

	TBool channelsRemaining = EFalse;

	for(TInt i = 0; i < KAvdtpChannelArraySize; i++)
		{
		if(iLogicalChannels[i] == aChannel)
			{
			iLogicalChannels[i] = NULL;
			}
		else if(iLogicalChannels[i])
			{
			channelsRemaining = ETrue;
			}
		}

	aChannel->Shutdown();
	delete aChannel;

	if(!channelsRemaining)
		{
		BTSocketTimer::Remove(iTimerEntry);
		
		// Notify factory of request completion
		TLogicalChannelFactoryTicket ticket(NULL, iId);
		iClient.LogicalChannelFactoryRequestComplete(ticket, KErrNone);
		}
	}

/**
Begin the job.  

@param aTimeout Number of milliseconds to wait for the remote to close
				the channels before locally initiating the shutdown.
*/
void CCloseSessionLogicalChannels::StartJob(TInt aTimeout)	
	{
	LOG_FUNC

	if(aTimeout)
		{
		TCallBack callBack(WatchdogBarked, this);
		iTimerEntry.Set(callBack);

		BTSocketTimer::Queue(aTimeout, iTimerEntry);
		}
	else
		{
		CloseChannels(ETrue);
		}
	}

/**
Static function called on expiry of channel close watchdog.
*/
TInt CCloseSessionLogicalChannels::WatchdogBarked(TAny* aCloseLogicalChannels)
	{
	LOG_STATIC_FUNC
	static_cast<CCloseSessionLogicalChannels*>(aCloseLogicalChannels)->CloseChannels(ETrue);

	return KErrNone;
	}
	
/**
Close all remaining channels.  

@param aNotifyCompletion if ETrue then inform client that job is complete
						 if EFalse then silently close remaining channels
*/
void CCloseSessionLogicalChannels::CloseChannels(TBool aNotifyCompletion)
	{
	LOG_FUNC

	for(TInt i = 0; i < KAvdtpChannelArraySize; i++)
		{
		if(iLogicalChannels[i])
			{
			iLogicalChannels[i]->Shutdown();
			delete iLogicalChannels[i];
			iLogicalChannels[i] = NULL;
			}
		}

	if(aNotifyCompletion)
		{
		// Notify factory of request completion
		TLogicalChannelFactoryTicket ticket(NULL, iId);
		iClient.LogicalChannelFactoryRequestComplete(ticket, KErrNone);
		}
	}

/** 
Constructor.

@param aClient Client to be notified on completion of this job. 
@param aId The job id of this job.
*/
CCloseSessionLogicalChannels::CCloseSessionLogicalChannels(XLogicalChannelFactoryClient& aClient,
											 TLogicalChannelFactoryRequestId aId)
: CLogicalChannelFactoryRequest(aClient, aId)
	{
	LOG_FUNC
	}

CLogicalChannelFactoryRequest::CLogicalChannelFactoryRequest(XLogicalChannelFactoryClient& aClient,
															 TLogicalChannelFactoryRequestId aId)
: iClient(aClient), iId(aId)
	{
	LOG_FUNC
	}
	
CLogicalChannelFactoryPassiveRequest::CLogicalChannelFactoryPassiveRequest(XLogicalChannelFactoryClient& aClient,
    																		  TLogicalChannelFactoryRequestId aId,
    																		  CBluetoothProtocolBase& aAvdtp)
 : CLogicalChannelFactoryRequest(aClient, aId), iAvdtp(aAvdtp)
   	{
   	LOG_FUNC
   	}

CLogicalChannelFactoryPassiveRequest::~CLogicalChannelFactoryPassiveRequest()
 	{
	LOG_FUNC
 	iAvdtp.DecrementListeners();
 	} 
   	
void CLogicalChannelFactoryPassiveRequest::BaseConstructL()
 	{
 	LOG_FUNC
 	User::LeaveIfError(iAvdtp.IncrementListeners());
 	}
	
CLogicalChannelFactoryActiveRequest::CLogicalChannelFactoryActiveRequest(const TBTDevAddr& aAddr, XLogicalChannelFactoryClient& aClient, TLogicalChannelFactoryRequestId aId)
: CLogicalChannelFactoryRequest(aClient, aId), iRemoteDev(aAddr)
	{
	LOG_FUNC
	}

CExpectSignallingLogicalChannel* CExpectSignallingLogicalChannel::NewL(XLogicalChannelFactoryClient& aClient,
 																	   TLogicalChannelFactoryRequestId aId,
 																	   CBluetoothProtocolBase& aAvdtp)											 
   	{
   	LOG_STATIC_FUNC
 	CExpectSignallingLogicalChannel* req = new (ELeave)CExpectSignallingLogicalChannel(aClient, aId, aAvdtp);
 	CleanupStack::PushL(req);
 	req->ConstructL();
 	CleanupStack::Pop(req);
 	return req;
 	}
 	
void CExpectSignallingLogicalChannel::ConstructL()
 	{
 	LOG_FUNC
	BaseConstructL();
   	}
   
CExpectSignallingLogicalChannel::CExpectSignallingLogicalChannel(XLogicalChannelFactoryClient& aClient,
 																 TLogicalChannelFactoryRequestId aId,
 																 CBluetoothProtocolBase& aAvdtp)
: CLogicalChannelFactoryPassiveRequest(aClient, aId, aAvdtp)
   	{
   	LOG_FUNC
   	iNumChannelsRequired = 1;
   	}
   
CExpectSessionLogicalChannels* CExpectSessionLogicalChannels::NewL(XLogicalChannelFactoryClient& aClient,
																   TLogicalChannelFactoryRequestId aId,
										   						   TInt aNumRequired,
										   						   CBluetoothProtocolBase& aAvdtp)
	{
	LOG_STATIC_FUNC
	CExpectSessionLogicalChannels* req = new (ELeave)CExpectSessionLogicalChannels(aClient, aId, aNumRequired, aAvdtp);
	CleanupStack::PushL(req);
	req->ConstructL();
	CleanupStack::Pop(req);
	return req;
	}
 
void CExpectSessionLogicalChannels::ConstructL()
 	{
 	LOG_FUNC
 	BaseConstructL();
   	}
   
CExpectSessionLogicalChannels::CExpectSessionLogicalChannels(XLogicalChannelFactoryClient& aClient,
															 TLogicalChannelFactoryRequestId aId,
															 TInt aNumRequired,
															 CBluetoothProtocolBase& aAvdtp)
: CLogicalChannelFactoryPassiveRequest(aClient, aId, aAvdtp)
	{
	LOG_FUNC
	iNumChannelsRequired = aNumRequired;
	}

CLogicalChannelFactoryActiveRequest::~CLogicalChannelFactoryActiveRequest()
	{
	LOG_FUNC
	// clear up any unclaimed stuff - array will be empty if saps claimed
	for (TInt i=0; i<iLogicalChannels.Count(); i++)
		{
		delete iLogicalChannels[i];
		}
	}
		
void CLogicalChannelFactory::DeleteRequest(CLogicalChannelFactoryRequest *aRequest)
	{
	LOG_FUNC
	
	iIdManager.FreeId(aRequest->iId);	// free the ID associated with this request

	delete aRequest;
	}

void CLogicalChannelFactory::Cancel(TLogicalChannelFactoryTicket& aJobSpec)
/*
The caller has decided they don't want to have the SAPs
we need to find the job and update it as Cancelled
we MUST then disconnect these channels as the remote entity will
misunderstand what sessions could be running over them
i.e. we cannot keep these SAPs just in case someone else wants them!
*/
	{
	LOG_FUNC
	// only cancel if the job is outstanding
	if (aJobSpec.State()==TLogicalChannelFactoryTicket::ERequestOutstanding)
		{
		TSglQueIter<CLogicalChannelFactoryRequest> iter(iPendingActiveJobs);
		CLogicalChannelFactoryRequest* request = NULL;
		TBool found = EFalse;
		
		while (iter)
			{
			// try active jobs first
			request = iter++;
			if (request->iId==aJobSpec.iId)
				{
				iPendingActiveJobs.Remove(*static_cast<CLogicalChannelFactoryActiveRequest*>(request));
				DeleteRequest(request);
				found = ETrue;
				}
			}
			
		if (!found)
			{
			// go through pending *passive signalling* jobs now
			iter = iPendingPassiveSignallingJobs;
			
			while (iter)
				{
				request = iter++;
				if (request->iId==aJobSpec.iId)
					{
					iPendingPassiveSignallingJobs.Remove(*static_cast<CLogicalChannelFactoryPassiveRequest*>(request));
					DeleteRequest(request);
					found = ETrue;
					}
				}
			}
			
		if (!found)
			{
			// go through pending *passive session* jobs now
			iter = iPendingPassiveSessionJobs;
			
			while (iter)
				{
				request = iter++;
				if (request->iId==aJobSpec.iId)
					{
					iPendingPassiveSessionJobs.Remove(*static_cast<CLogicalChannelFactoryPassiveRequest*>(request));
					DeleteRequest(request);
					found = ETrue;
					}
				}
			}
				
		if (!found)
			{
			// perhaps it's the current one
			if (iCurrentActiveJob && iCurrentActiveJob->iId == aJobSpec.iId)
				{
				iCurrentJobCancelled = ETrue;
				}
			}
		
		if (found)
			{
			aJobSpec.SetState(TLogicalChannelFactoryTicket::ERequestIdle);
			}
		}
	}

void CLogicalChannelFactory::FreeId(TAny* aFactory)
	{
	LOG_STATIC_FUNC
	CLogicalChannelFactory *fact = static_cast<CLogicalChannelFactory*>(aFactory);
	fact->iIdManager.FreeId(fact->iId);
	}
	
TLogicalChannelFactoryTicket CLogicalChannelFactory::CreateSignallingLogicalChannelL(const TBTDevAddr& aAddr, XLogicalChannelFactoryClient& aClient)
	{
	LOG_FUNC
	// check that we don't have an inbound one from remote
	// if we have then we just use that unclaimed one
	// if we don't then go and create an outbound one
	TLogicalChannelFactoryRequestId id;
	TLogicalChannelFactoryTicket::TLogicalChannelFactoryRequestState state;
	
	CManagedLogicalChannel* ch = FindUnclaimedLogicalChannel(aAddr, ESignalling, id);
	if (ch)
		{
		state = TLogicalChannelFactoryTicket::ERequestComplete;
		// they'll come back and get SAP off their request
		}
	else
		{
		// need to create a new one
		User::LeaveIfError(iIdManager.GetId(id));
		iId = id; // just in case we need to clean it up this needs to be stored
		CleanupStack::PushL(TCleanupItem(FreeId, (TAny*)this));
		CCreateSignallingLogicalChannel* req =
				CCreateSignallingLogicalChannel::NewL(aAddr,
													  aClient,
													  id,
													  *this);
		CleanupStack::Pop();
		// got a complete job to do - put into job list
		iPendingActiveJobs.AddLast(*req);

		state = TLogicalChannelFactoryTicket::ERequestOutstanding;
		TryNextActiveJob();
		}

	TLogicalChannelFactoryTicket ticket(this, id);
	ticket.SetState(state);
	return ticket;
	}

TLogicalChannelFactoryTicket CLogicalChannelFactory::CreateSessionLogicalChannelsL(
								const TBTDevAddr& aAddr,
								XLogicalChannelFactoryClient& aClient,
								TInt aNumRequired)
	{
	LOG_FUNC
//#pragma message("Check not already got or getting signalling channel for remote")
	
	TLogicalChannelFactoryRequestId id;
	User::LeaveIfError(iIdManager.GetId(id));
	
	iId = id; // just in case we need to clean it up this needs to be stored
	CleanupStack::PushL(TCleanupItem(FreeId, (TAny*)this));
	CCreateSessionLogicalChannels* req = CCreateSessionLogicalChannels::NewLC(aAddr, aClient, id, aNumRequired);
		
	CManagedLogicalChannel* ch;
	// we connect up the SAPs backwards to save a member variable (a counter)
	// so the sequence number needs to go backwards
	// then the claimer (who specifies sequence number) is really getting the
	// right sequence number (e.g. the 3rd channel connected really was the 3rd)
	for (TInt chIndex=aNumRequired-1; chIndex>=0; chIndex--)
		{
		// sequene number is 1 indexed, so chIndex+1...
		ch = CManagedLogicalChannel::NewL(*this, aAddr, chIndex+1, id);
		// transfer ownership to request
		req->iLogicalChannels[chIndex] = ch;														  
		}	
											  
	CleanupStack::Pop(req);
	CleanupStack::Pop();
	// got a complete job to do - put into job list
	iPendingActiveJobs.AddLast(*req);			
	TryNextActiveJob();
	
	TLogicalChannelFactoryTicket ticket(this, id);
	ticket.SetState(TLogicalChannelFactoryTicket::ERequestOutstanding);
	return ticket;
	}

/**
Request some logical channels be closed.  This takes ownership of the lower protocol
SAPs from the transport channels.  Unlike other logical channel factory jobs this does
not require a client to callback on completion, as it is anticipated that clients do
not have actions that are required on channel closure.

This does not support multiplexed channels.

@param aChannels CTransportChannels that are the current owners of the lower SAPs.  All transport 
				 channels should be passed so they can be appropriately informed that they no longer 
				 have a valid lower SAP.  Note that although ownership of the lower SAP is passed
				 ownership of the CTransportChannel itself is not.
@param aTimeout  If this is non-zero then this job will wait for the remote to initiate
				 a close of the channels, with a guard timer of aTimeout milliseconds, after
				 which it will initiate the shutdown locally.
*/
void CLogicalChannelFactory::CloseSessionLogicalChannelsL(
								TArray<CDirectChannel*>& aChannels,
								TInt aTimeout)
	{
	LOG_FUNC

	TLogicalChannelFactoryRequestId id;
	User::LeaveIfError(iIdManager.GetId(id));

	iId = id; // just in case we need to clean it up this needs to be stored
	CleanupStack::PushL(TCleanupItem(FreeId, (TAny*)this));

	// We want to take all or none of the SAP's, so we will wait until after the failable
	// allocations before transferring SAP ownership
	CCloseSessionLogicalChannels* req = CCloseSessionLogicalChannels::NewLC(*this, id);

	CManagedLogicalChannel* ch;

	for (TInt i = 0; i < KAvdtpChannelArraySize; i++)
		{
		// default the channel to NULL
		// so it doesn't appear in the job we are creating
		req->iLogicalChannels[i] = NULL;
		if (aChannels[i])
			{
			ch = CManagedLogicalChannel::NewL(*this, id);
			// transfer ownership to request
			req->iLogicalChannels[i] = ch;	
			}													  
		}

	// failing stuff all done now - no leaving after this point

	for (TInt i = 0; i < KAvdtpChannelArraySize; i++)
		{
		if (aChannels[i])
			{
			CServProviderBase* sap = aChannels[i]->ObtainSAP();
			/*
			NOTE it IS possible that the channel is NOT totally setup
			if the remote is behaving badly.
			So in case check the channel has a SAP BEFORE 
			calling shutdown. */
			if (sap)
				{
				req->iLogicalChannels[i]->ProvideSAP(sap);
				}
			}													  
		}

	req->StartJob(aTimeout);
									  
	CleanupStack::Pop(req);
	CleanupStack::Pop();

	iCloseChannelJobs.AddLast(*req);	
	}

void CLogicalChannelFactory::DoObtainChannelL()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iCurrentActiveJob && iCurrentActiveJob->iNumChannelsRequired, Panic(EAvdtpLogicalChannelFactoryJobHasNoLogicalChannels));
	
	static const TInt KAVDTPPSM = 0x19;	
	
	TInt iter = 0;
	TInt currentLogicalChannel;
	
	//Get the actual channel counts, (iter-1)
	while(++iter <= iCurrentActiveJob->iLogicalChannels.Count() && iCurrentActiveJob->iLogicalChannels[iter-1] != NULL)
		{		
		}
	//currentLogicalChannel's value will increase for each call, 
	//as iCurrentActiveJob->iNumChannelsRequired decreases each time
	currentLogicalChannel = iter - iCurrentActiveJob->iNumChannelsRequired - 1;
	
	//Make sure that channel's index in the array matches its sequence number('iSequenceNumber')
	//Correct match is: index = 0 ----> iSequenceNumber = 1 (Media Channel)
	//					index = 1 ----> iSequenceNumber = 2 (Reporting/Recovery Channel)
	//					index = 2 ----> iSequenceNumber = 3 (Reporting/Recovery Channel)
	//Please refer to function: CLogicalChannelFactory::CreateSessionLogicalChannelsL()
	__ASSERT_DEBUG(iCurrentActiveJob->iLogicalChannels[currentLogicalChannel]->iSequenceNumber == currentLogicalChannel + 1, 
						Panic(EAvdtpLogicalChannelArrayMismatchesIndexAndSequence));
	
	// working forwards through array of SAPs requiring connections
	CServProviderBase& sap = 
		*iCurrentActiveJob->iLogicalChannels[currentLogicalChannel]->iLogicalChannelSAP;

	// for the 'active' SAP we become the socket for now
	sap.SetNotify(this);
	
	TBTServiceSecurity sec;
	sec.SetAuthentication(EMitmDesired);
	sec.SetAuthorisation(EFalse);
	sec.SetEncryption(ETrue);
	sec.SetDenied(EFalse);

	TL2CAPSockAddr addr;
	addr.SetBTAddr(iCurrentActiveJob->iRemoteDev);
	addr.SetPort(KAVDTPPSM);
	addr.SetSecurity(sec);
	
	TInt err = sap.SetRemName(addr);
	__ASSERT_DEBUG(err==KErrNone, Panic(EAvdtpUnexpectedErrorFromL2CAP));
	
	sap.ActiveOpen();
	
	// The call to ActiveOpen could affect iCurrentActiveJob if 
	// L2CAP synchrononously errors -- in which case iCurrentActiveJob could now
	// have become NULL. Make sure it is still valid.
	if (iCurrentActiveJob)
		{		
		// decrement how many more channels required for this job
		--iCurrentActiveJob->iNumChannelsRequired;
		iCurrentJobCancelled = EFalse;
		}
	// a next job may be attempted from a synchronous error from l2cap
	}

/**
This examines the job queue and tries to pass off the new channel to an outstanding
job.

@param aRemote The device from which the connection orginates
@param aSAP An L2CAP SAP.  The job initiator takes this from the factory on
			successful job completion.
@return KErrNone if the connection was successfully taken by the job.
		KErrNotReady if there are no jobs on this queue.
		System wide error if the new PendingLogicalChannel could not be created.
*/	
TInt CLogicalChannelFactory::TryToTakeConnection(const TBTDevAddr& aRemote,
	CServProviderBase* aSAP, TSglQue<CLogicalChannelFactoryPassiveRequest>& aJobQueue)
	{
	LOG_FUNC
	
	if(aJobQueue.IsEmpty())
		{
		// No jobs on this queue mate!
		return KErrNotReady;
		}

	CLogicalChannelFactoryPassiveRequest* job = aJobQueue.First();

	// need to place this newSAP into a logical channel - then put that into job
	TInt seqNumber = KInitialSequenceNumber;
	TDblQueIter<CManagedLogicalChannel> iter(iUnclaimedLogicalChannels);
	
	while (iter)
		{
		CManagedLogicalChannel* r = iter++;
		if (r->iRemoteAddress == aRemote)
			{
			seqNumber++;
			}
		}

	// job doesnt take ownership yet
	CManagedLogicalChannel* pch = NULL;
	TRAPD(err, pch = CManagedLogicalChannel::NewL(*this, aRemote, seqNumber, job->iId, aSAP));
	if (err==KErrNone)
		{
		iUnclaimedLogicalChannels.AddFirst(*pch);
		if (--job->iNumChannelsRequired == 0)
			{
			// all channels obtained
			aJobQueue.Remove(*job);
			NotifyComplete(KErrNone, *job);
			delete job;
			}
		}
		
	return err;
	}	

TInt CLogicalChannelFactory::BearerConnectComplete(const TBTDevAddr& aRemote,
													CServProviderBase* aSAP)
	{
	LOG_FUNC
	// inbound connection!!
	// then bung on queue and notify 
	
	if (iPendingPassiveSignallingJobs.IsEmpty() && iPendingPassiveSessionJobs.IsEmpty())
		{
		return KErrNotReady; // connection will be auto-disposed by listener
		}
	
	if (iCurrentActiveJob)
		{
		// we're doing outbound stuff - need to make sure it's not to same remote
		// as it would then be being naughty...
		if (iCurrentActiveJob->iRemoteDev == aRemote)
			{
			//tsk tsk
			return KErrAlreadyExists; // connection will be auto-disposed
			}
		}

	TInt res;
	// We need to work out what type of session this is.  Because the first, and only
	// the first, channel to each remote must be a signalling channel we can use this 
	// information to decide whether to use this new channel to complete a signalling
	// job or a session job.
	if((static_cast<CAvdtpProtocol&>(iProtocol)).FindSignallingChannel(aRemote))
		{
		// Already got a signalling channel, complete a session job if we have one
		LOG(_L("Looking for a session job to complete"));
		res = TryToTakeConnection(aRemote, aSAP, iPendingPassiveSessionJobs);
		}
	else
		{
		// No signalling channel yet, complete a signalling job if we have one
		LOG(_L("Looking for a signalling job to complete"));
		res = TryToTakeConnection(aRemote, aSAP, iPendingPassiveSignallingJobs);
		}

	return res;
	}


// from MSocketNotify
void CLogicalChannelFactory::NewData(TUint /*aCount*/)
	{
	LOG_FUNC
	// we should have transferred ownership before newdata is called
	__DEBUGGER();
	}

void CLogicalChannelFactory::CanSend()
	{
	LOG_FUNC
	// we don't send data
	__DEBUGGER();
	}

void CLogicalChannelFactory::ConnectComplete()
	{
	LOG_FUNC
	// that's what we want!
	TBool done = CheckActiveJobComplete(*iCurrentActiveJob);
	
	if (done)
		{
//		LOG1(_L("ConnectComplete for request: id=%d"), iCurrentActiveJob->iId)
		CompleteActiveJob(KErrNone);
		}
	else
		{
		// more channels required for job
//		LOG1(_L("ConnectComplete for request: id=%d (but more channels needed)"), iCurrentActiveJob->iId)
		TRAPD(err, DoObtainChannelL());
		if (err)
			{
			CompleteActiveJob(KErrNoMemory);
			}
		}
	}

void CLogicalChannelFactory::ConnectComplete(const TDesC8& /*aConnectData*/)
	{
	LOG_FUNC
	// no connect data supported
	ConnectComplete();
	}
	
void CLogicalChannelFactory::ConnectComplete(CServProviderBase& /*aSAP*/)
	{
	LOG_FUNC
	__DEBUGGER(); // should have come via protocol's listener
	}
	
void CLogicalChannelFactory::ConnectComplete(CServProviderBase& /*aSAP*/,const TDesC8& /*aConnectData*/)
	{
	LOG_FUNC
	// not used	
	__DEBUGGER();
	}

void CLogicalChannelFactory::CanClose(TDelete /*aDelete*/)
	{
	LOG_FUNC
	// not used
	__DEBUGGER();
	}

void CLogicalChannelFactory::CanClose(const TDesC8& /*aDisconnectData*/,TDelete /*aDelete*/)
	{
	LOG_FUNC
	// not used
	__DEBUGGER();
	}

void CLogicalChannelFactory::Error(TInt aError,TUint /*aOperationMask*/)
	{
	LOG_FUNC
	// whoops - need to find the job on which the sap is
	// eventually we may process jobs out of order see!

	// at present we error the whole job if any channel fails
	CompleteActiveJob(aError);
	}

void CLogicalChannelFactory::Disconnect()
	{
	LOG_FUNC
	// not used
	__DEBUGGER();
	}

void CLogicalChannelFactory::Disconnect(TDesC8& /*aDisconnectData*/)
	{
	LOG_FUNC
	//not used
	__DEBUGGER();
	}

void CLogicalChannelFactory::IoctlComplete(TDesC8* /*aBuf*/)
	{
	LOG_FUNC
	// not used	
	__DEBUGGER();
	}

void CLogicalChannelFactory::NoBearer(const TDesC8& /*aConnectionInfo*/)
	{
	LOG_FUNC
	// not used	
	__DEBUGGER();
	}

void CLogicalChannelFactory::Bearer(const TDesC8& /*aConnectionInfo*/)
	{
	LOG_FUNC
	// not used
	__DEBUGGER();
	}

/*static*/ TInt CLogicalChannelFactory::TryNextJob(TAny* aAny)
	{
	LOG_STATIC_FUNC
	CLogicalChannelFactory* self = static_cast<CLogicalChannelFactory*>(aAny);
	self->TryNextActiveJob();
	return KErrNone;
	}


void CLogicalChannelFactory::TryNextActiveJob()
	{
	LOG_FUNC
	// since each channel *might* take a different amount of time to come up we
	// must serialise these so that we *and* the remote agree on the transport session
	// that will flow over the l2cap logical channel.

	// so we don't call DoObtainChannel until the previous job is completed, or there are none waiting
	if (!iCurrentActiveJob)
		{
		if (!iPendingActiveJobs.IsEmpty())
			{
			iCurrentActiveJob = iPendingActiveJobs.First();
			// iCurrent Job takes ownership from queue...
			iPendingActiveJobs.Remove(*iCurrentActiveJob);
			LOG1(_L("Obtaining channel for current active request: id=%d"), iCurrentActiveJob->iId);
			TRAPD(error, DoObtainChannelL());
			if (error != KErrNone)
				{
				CompleteActiveJob(error);
				iCurrentActiveJob = NULL;
				TryNextActiveJob();
				}			
			}
		}
	}
	
CManagedLogicalChannel* CLogicalChannelFactory::FindUnclaimedLogicalChannel(
														const TBTDevAddr& aAddr,
														TInt aSequenceNumber,
														TLogicalChannelFactoryRequestId& aId)
	{
	LOG_FUNC
	TDblQueIter<CManagedLogicalChannel> iter(iUnclaimedLogicalChannels);
	
	while (iter)
		{
		CManagedLogicalChannel* ch = iter++;
		if (ch->iRemoteAddress == aAddr && ch->iSequenceNumber == aSequenceNumber)
			{
			aId = ch->iId;
			return ch;
			}
		}
	return NULL;
	}

	
TBool CLogicalChannelFactory::CheckActiveJobComplete(CLogicalChannelFactoryActiveRequest& aJob)
	{
	LOG_FUNC
	//if we get to zero - or error occurs - job is done
	if (iCurrentJobCancelled)
		{
		// we left the cancelled job running, to sequence the channels
		// but proceed no more with other channels, destroy via completion of job
		CompleteActiveJob(KErrCancel);
		return ETrue;
		}
	return (!aJob.iNumChannelsRequired);
	}
	

void CLogicalChannelFactory::CompleteActiveJob(TInt aError)
	{
	LOG_FUNC
	if ((!iCurrentJobCancelled) && (iCurrentActiveJob))
		{
		if (aError == KErrNone)
			{
			for (TInt index = 0; index<iCurrentActiveJob->iLogicalChannels.Count(); index++)
				{
				CManagedLogicalChannel* ch = iCurrentActiveJob->iLogicalChannels[index];
				if (ch)
					{
					// put on unclaimed queue now - caller may come back synchronously
					iUnclaimedLogicalChannels.AddFirst(*ch);
					
					// transferred ownership, so take them off job array
					iCurrentActiveJob->iLogicalChannels[index] = NULL;
					}
				}
			}
		else
			{
			// leave to be destroyed below
			}
		NotifyComplete(aError, *iCurrentActiveJob);
		}
	else
		{
		// cleanup 
		if (iCurrentActiveJob)
			{
			iIdManager.FreeId(iCurrentActiveJob->iId);		
			}
		// rest of cleanup will be done on destruction of job
		}
	
	// the job is done, and the logical channels transferred onto pending queue, so delete job
	delete iCurrentActiveJob;
	iCurrentActiveJob = NULL;
	iAsyncTryNextJob->CallBack();
	}

void CLogicalChannelFactory::NotifyComplete(TInt aError, CLogicalChannelFactoryRequest& aRequest)
	{
	LOG_FUNC

	// if client cancelled this request, ID was already freed and NotifyComplete() should not be called
	__ASSERT_DEBUG(aError!=KErrCancel, Panic(EAvdtpNotifyCompleteCancelledRequest));

	TLogicalChannelFactoryTicket ticket(this, aRequest.iId);
	ticket.SetState((aError==KErrNone) ? TLogicalChannelFactoryTicket::ERequestComplete : TLogicalChannelFactoryTicket::ERequestErrored);

	aRequest.iClient.LogicalChannelFactoryRequestComplete(ticket, aError);
	
	// LogicalChannelFactoryRequestComplete() does not free the ID of errorred jobs
	if (aError != KErrNone)
		{
		iIdManager.FreeId(aRequest.iId);
		}
	
	}

TLogicalChannelRecord CLogicalChannelFactory::ClaimLogicalChannel(TInt aSequenceNumber, TLogicalChannelFactoryRequestId aId, TBool& aFinished)
	{
	LOG_FUNC
	__ASSERT_DEBUG(aSequenceNumber>=KInitialSequenceNumber, Panic(EAvdtpLogicalChannelFactoryBadClaimLogicalChannel));
	CManagedLogicalChannel* ch = NULL;
	CManagedLogicalChannel* chFound = NULL;
	TDblQueIter<CManagedLogicalChannel> iter(iUnclaimedLogicalChannels);
	TInt channelsWithId=0;

	aFinished = EFalse; // set to true below if last channel of job retrieved
	
	while (iter)
		{		
		ch = iter++;
		if (ch->iId == aId)
			{
			// channel is part of the request - count it
			++channelsWithId;
			
			if (ch->iSequenceNumber == aSequenceNumber)
				{
				// this is the channel explicitly sought
				ch->iFactoryQLink.Deque(); // this one link is claimed - may be more on job
				chFound = ch;
				}
			// don't break loop - we want to continue to see if this is the last channel for the job
			}
		}
		
	__ASSERT_DEBUG(chFound, Panic(EAvdtpLogicalChannelFactoryBadClaimLogicalChannel));

	TLogicalChannelRecord rec;
	rec.iLogicalChannelSAP = chFound->ObtainSAP();
	rec.iDataCount = chFound->iDataCount;
	rec.iEndOfData = chFound->iEndOfData;	
	
	// logicalchannel is no longer pending, so remove
	delete chFound;
	
	// but don't release the id until all the logical channels on the job claimed
	if (channelsWithId==1)	
		{
		// that was the last channel on the job, so the job id can be freed
		iIdManager.FreeId(aId);
		aFinished = ETrue;
		}
	
	return rec;
	}
	
TLogicalChannelFactoryTicket CLogicalChannelFactory::ExpectSignallingLogicalChannelL(XLogicalChannelFactoryClient& aClient)
	{
	LOG_FUNC
	TLogicalChannelFactoryRequestId id;
	User::LeaveIfError(iIdManager.GetId(id));

	iId = id; // just in case we need to clean it up this needs to be stored
	CleanupStack::PushL(TCleanupItem(FreeId, (TAny*)this));
 	CExpectSignallingLogicalChannel* req = CExpectSignallingLogicalChannel::NewL(aClient, id, iProtocol);
	CleanupStack::Pop();
	
	// got a complete job to do - put into job list
	iPendingPassiveSignallingJobs.AddLast(*req);

	TLogicalChannelFactoryTicket ticket(this, id);
	ticket.SetState(TLogicalChannelFactoryTicket::ERequestOutstanding);
	return ticket;
	}
	
	
TLogicalChannelFactoryTicket CLogicalChannelFactory::
								ExpectSessionLogicalChannelsL(XLogicalChannelFactoryClient& aClient,
																TInt aNumRequired)
	{
	LOG_FUNC

	TLogicalChannelFactoryRequestId id;
	User::LeaveIfError(iIdManager.GetId(id)); //Qualified
		
	iId = id; // just in case we need to clean it up this needs to be stored
	CleanupStack::PushL(TCleanupItem(FreeId, (TAny*)this));
	CExpectSessionLogicalChannels* req = CExpectSessionLogicalChannels::NewL(aClient,id, aNumRequired, iProtocol); //Qualified
	CleanupStack::Pop();
		
	// got a complete job to do - put into job list
	iPendingPassiveSessionJobs.AddLast(*req);

	TLogicalChannelFactoryTicket ticket(this, id);
	ticket.SetState(TLogicalChannelFactoryTicket::ERequestOutstanding);
	return ticket;
	}
	
/**
For some jobs that clients are not interested in the results of we act as the client, dealing
with completion of the job.
*/
void CLogicalChannelFactory::LogicalChannelFactoryRequestComplete(TLogicalChannelFactoryTicket aTicket, TInt aResult)
	{
	LOG_FUNC
#ifdef _DEBUG
	TBool found = EFalse;
#endif

	CCloseSessionLogicalChannels* clc = NULL;
	TSglQueIter<CCloseSessionLogicalChannels> iter(iCloseChannelJobs);
	while(iter)
		{
		clc = iter++;
		if(clc->iId == aTicket.iId)
			{
#ifdef _DEBUG 
			found = ETrue;
#endif 
			iCloseChannelJobs.Remove(*clc);
			delete clc;
			// Only free the ID of successful jobs - NotifyComplete() frees the ID of unsuccessful jobs
			if (aResult == KErrNone)
			{
				iIdManager.FreeId(aTicket.iId);				
			}
			break;
			}
		}
		
	__ASSERT_DEBUG(found, Panic(EAvdtpJobCompleteForUnknownCloseChannelJob));
	}

CServProviderBase* CManagedLogicalChannel::ObtainSAP()
	{
	LOG_FUNC
	CServProviderBase* sap = iLogicalChannelSAP;
	iLogicalChannelSAP = NULL;
	return sap;
	}

void CManagedLogicalChannel::ProvideSAP(CServProviderBase* aSAP)
	{
	__ASSERT_DEBUG(aSAP, Panic(EAvdtpPassingNullSapOwnershipToChannel));
	__ASSERT_DEBUG(!iLogicalChannelSAP, Panic(EAvdtpPassingSapOwnershipToChannelThatAlreadyHasASap));
	iLogicalChannelSAP = aSAP;
	iLogicalChannelSAP->SetNotify(this);
	}

void CLogicalChannelFactory::LogicalChannelLost(CManagedLogicalChannel* aChannel)
	{
	LOG_FUNC

	// This could be a channel we've been asked to disconnect, or a channel
	// we've brought up with has been lost before being claimed.

	TSglQueIter<CCloseSessionLogicalChannels> closeChannelIter(iCloseChannelJobs);
	while(closeChannelIter)
		{
		CCloseSessionLogicalChannels* clc = closeChannelIter++;
		if(clc->iId == aChannel->iId)
			{
			clc->ChannelClosed(aChannel);
			return;
			}
		}

	// We weren't waiting for this channel to go down, check the unclaimed channels
	// find, remove and delete
	TDblQueIter<CManagedLogicalChannel> unclaimedIter(iUnclaimedLogicalChannels);
	
	while (unclaimedIter)
		{
		CManagedLogicalChannel* ch = unclaimedIter++;
		if (ch == aChannel)
			{
			ch->iFactoryQLink.Deque();
			delete ch;
			break; // cos we're adding at head
			}
		}
	}


CManagedLogicalChannel* CManagedLogicalChannel::NewL(CLogicalChannelFactory& aFactory,
								 		const TBTDevAddr& aAddr,
								 		TInt aSequenceNumber,
								 		TLogicalChannelFactoryRequestId aId,
								 		CServProviderBase* aPrecreatedSAP/*=NULL*/)
	{
	LOG_STATIC_FUNC
	CManagedLogicalChannel* self = new (ELeave) CManagedLogicalChannel(aFactory, aAddr, aSequenceNumber, aId);
	CleanupStack::PushL(self);
	self->ConstructL(aPrecreatedSAP);
	CleanupStack::Pop(self);
	return self;
	}

CManagedLogicalChannel* CManagedLogicalChannel::NewL(CLogicalChannelFactory& aFactory,
								 		TLogicalChannelFactoryRequestId aId)
	{
	LOG_STATIC_FUNC
	// We don't call the ConstructL here as this overload of NewL is used when the SAP
	// will be provided later.
	CManagedLogicalChannel* self = new (ELeave) CManagedLogicalChannel(aFactory, aId);
	return self;
	}

void CManagedLogicalChannel::ConstructL(CServProviderBase* aPrecreatedSAP)
	{
	LOG_FUNC
	iLogicalChannelSAP = aPrecreatedSAP ? aPrecreatedSAP :
										  iFactory.SAPFactory().NewSAPL(KSockSeqPacket);
	
	// There are two things we may need to change here, depending on what
	// sort of channel we're creating:
	// - On outgoing request for a signalling channel we need to 
	//   request a reliable channel
	// - On any request for a transport channel we need to bump up the channel priority
	
	TPckgBuf<TL2CapConfig> configBuf;
	iLogicalChannelSAP->GetOption(KSolBtL2CAP, KL2CAPUpdateChannelConfig, configBuf);
	if(!aPrecreatedSAP)
		{
		// FIXME consider value of rtx timer, should consider Tgavdp
		// Note: The 'rtx timer' actually sets max retransmit count instead [Piotr].
		configBuf().ConfigureReliableChannel(500);
		iLogicalChannelSAP->SetOption(KSolBtL2CAP, KL2CAPUpdateChannelConfig, configBuf);
		}

	// tell sap who its socket is
	iLogicalChannelSAP->SetNotify(this);
	iLogicalChannelSAP->Start();
	}

CManagedLogicalChannel::CManagedLogicalChannel(CLogicalChannelFactory& aFactory,
										   	   const TBTDevAddr& aAddr,
						   					   TInt aSequenceNumber ,
						   					   TLogicalChannelFactoryRequestId aId)
: iFactory(aFactory), iRemoteAddress(aAddr), iSequenceNumber(aSequenceNumber), iId(aId)
	{
	LOG_FUNC
	__ASSERT_DEBUG(iSequenceNumber>=KInitialSequenceNumber, Panic(EAvdtpLogicalChannelFactoryBadSequenceNumber));
	}

CManagedLogicalChannel::CManagedLogicalChannel(CLogicalChannelFactory& aFactory,
										   	   TLogicalChannelFactoryRequestId aId)
: iFactory(aFactory), iId(aId)
	{
	LOG_FUNC
	}
	
CManagedLogicalChannel::~CManagedLogicalChannel()
	{
	LOG_FUNC
	// need to clear up SAP (we're unclaimed at this point)
	delete iLogicalChannelSAP;
	}
	
void CManagedLogicalChannel::Shutdown()
	{
	/*
	NOTE it IS possible that the channel is NOT totally setup
	if the remote is behaving badly.
	So in case check the channel has a SAP BEFORE 
	calling shutdown. */
	if (iLogicalChannelSAP)
		{
		iLogicalChannelSAP->Shutdown(CServProviderBase::EImmediate);
		}
	}

void CManagedLogicalChannel::NewData(TUint aCount)
	{
	LOG_FUNC
	// this class doesn't "know" the protocol. it just leaves the data in l2cap
	// for the eventual user of this sap to extract
	if (aCount==KNewDataEndofData)
		{
		iEndOfData = ETrue;
		}
	else
		{
		iDataCount+=aCount;
		}
	}
	
void CManagedLogicalChannel::CanSend()
	{
	LOG_FUNC
	// we never send
	__DEBUGGER();
	}
	
void CManagedLogicalChannel::ConnectComplete()
	{
	LOG_FUNC
	// we never connect
	__DEBUGGER();
	}
	
void CManagedLogicalChannel::ConnectComplete(const TDesC8& /*aConnectData*/)
	{
	LOG_FUNC
	// we never connect
	__DEBUGGER();
	}

void CManagedLogicalChannel::ConnectComplete(CServProviderBase& /*aSSP*/)
	{
	LOG_FUNC
	// we never connect
	__DEBUGGER();
	}
	
void CManagedLogicalChannel::ConnectComplete(CServProviderBase& /*aSSP*/,const TDesC8& /*aConnectData*/)
	{
	LOG_FUNC
	// we never connect
	__DEBUGGER();
	}

void CManagedLogicalChannel::CanClose(TDelete /*aDelete*/)
	{
	LOG_FUNC
	// we only shutdown immediately
	__DEBUGGER();
	}
	
void CManagedLogicalChannel::CanClose(const TDesC8& /*aDisconnectData*/,TDelete /*aDelete*/)
	{
	LOG_FUNC
	// we only shutdown immediately
	__DEBUGGER();
	}
	
void CManagedLogicalChannel::Error(TInt /*aError*/,TUint /*aOperationMask*/)
	{
	LOG_FUNC
	iFactory.LogicalChannelLost(this); // will delete us
	}
	
void CManagedLogicalChannel::Disconnect()
	{
	LOG_FUNC
	iFactory.LogicalChannelLost(this); // will delete us
	}

void CManagedLogicalChannel::Disconnect(TDesC8& /*aDisconnectData*/)
	{
	LOG_FUNC
	// bearer doesnt support disconnect data
	__DEBUGGER();
	Disconnect();
	}
	
void CManagedLogicalChannel::IoctlComplete(TDesC8* /*aBuf*/)
	{
	LOG_FUNC
	// we don't issue ioctls
	__DEBUGGER();
	}
	
void CManagedLogicalChannel::NoBearer(const TDesC8& /*aConnectionInfo*/)
	{
	LOG_FUNC
	// a redundant artefact of IP nonsense
	__DEBUGGER();
	}
	
void CManagedLogicalChannel::Bearer(const TDesC8& /*aConnectionInfo*/)
	{
	LOG_FUNC
	// a redundant artefact of IP nonsense
	__DEBUGGER();
	}


TLogicalChannelFactoryTicket::TLogicalChannelFactoryTicket(CLogicalChannelFactory* aFactory, TLogicalChannelFactoryRequestId aId)
	: iFactory(aFactory)
	, iId(aId)
	, iState(TLogicalChannelFactoryTicket::ERequestIdle)
	{
	LOG_FUNC
	}

TLogicalChannelFactoryTicket::TLogicalChannelFactoryTicket()
	: iState(TLogicalChannelFactoryTicket::ERequestIdle)
	{
	LOG_FUNC
	}

TLogicalChannelRecord TLogicalChannelFactoryTicket::GetLogicalChannel(TInt aSequenceNumber/*=1*/)
	{
	LOG_FUNC
	__ASSERT_DEBUG(iState != ERequestErrored, Panic(EAvdtpInvalidResponseInUse));
	TBool finished = EFalse;
	TLogicalChannelRecord rec = iFactory->ClaimLogicalChannel(aSequenceNumber, iId, finished);
	if (finished)
		{
		// to stop people forgetting
		iState = ERequestComplete;
		}
	return rec;
	}

void TLogicalChannelFactoryTicket::SetState(TLogicalChannelFactoryRequestState aNewState)
	{
	LOG_FUNC
	LOG2(_L("State Transition %d -> %d"), iState, aNewState);
	iState = aNewState;
	}

	
TLogicalChannelFactoryTicketInspector::TLogicalChannelFactoryTicketInspector(
							TLogicalChannelFactoryTicket& aTicket,
							TBool aRequireReporting,
							TBool aRequireRecovery, TBool aMuxed)
	: iTicket(aTicket)
	, iSignallingSequenceNumber(KInitialSequenceNumber)
	, iMediaSequenceNumber(KInitialSequenceNumber)
	, iCached(EFalse)
	{
	LOG_FUNC
#ifdef _DEBUG
	// to assert against later
	iRequireReporting = aRequireReporting;
	iRequireRecovery = aRequireRecovery;
#endif
	if (aMuxed)
		{
		// the reporting and media go in one channel, the first created
		// this class knows a bit about the strategy of mux allocation
		iReportingSequenceNumber = 1; // with media
		iRecoverySequenceNumber = 2;
		}
	else
		{
		iReportingSequenceNumber = 2;
		iRecoverySequenceNumber = aRequireReporting & aRequireRecovery ? 3 : 2;
		}
	}
							
TLogicalChannelRecord TLogicalChannelFactoryTicketInspector::GetLogicalChannel(TAvdtpTransportSessionType aType)
	{
	LOG_FUNC
	TLogicalChannelRecord rec;
	
	if (!iCached)
		{
		const TInt* index;
		switch (aType)
			{
			case ESignalling:
				index = &iSignallingSequenceNumber;
				break;
			case EMedia:
				index = &iMediaSequenceNumber;
				break;
			case EReporting:
				__ASSERT_DEBUG(iRequireReporting, Panic(EAvdtpLogicalChannelFactoryBadClaimLogicalChannel));
				index = &iReportingSequenceNumber;
				break;
			case ERecovery:
				__ASSERT_DEBUG(iRequireRecovery, Panic(EAvdtpLogicalChannelFactoryBadClaimLogicalChannel));
				index = &iRecoverySequenceNumber;
				break;
			default:
				index = NULL;
			}
		__ASSERT_DEBUG(index, Panic(EAvdtpLogicalChannelFactoryBadClaimLogicalChannel));
		rec = iTicket.GetLogicalChannel(*index);
		
		if (iReportingSequenceNumber==iMediaSequenceNumber)
			{
			// we're in muxed case...
			// this helper helps further by caching the record
			// so that the client can claim again for reporting, even if same record
			// that the factory will have (rightly) purged
			// therefore cache this
			iCachedRecord = rec;
			iCached = ETrue;
			}
		}
	else
		{
		// cached - use same record, and allow arbitrary order of
		// collection of cache-able session types
		if (aType == EMedia || aType == EReporting)
			{
			rec = iCachedRecord;
			iCached = EFalse;
			}
		}
	return rec;
	}
