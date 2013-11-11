// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bluetooth/logger.h>
#include "secman.h"
#include "hostresolver.h"
#ifdef BT_LINKMGR_V2
#include "physicallinks.h"
#include "physicallinksmanager.h"
#else
#include "PhysicalLinks.h"
#include "PhysicalLinksManager.h"
#endif

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_SECMAN);
#endif

#ifdef _DEBUG
PANICCATEGORY("secman");
#endif

#pragma warning (disable: 4355) //'this' in base init list - is OK for our usage

static const TInt KBTSecManAccessRequesterArrayGranularity = 4;
static const TInt KBTSecManNotifierRequesterArrayGranularity = 4;


//------------------------------------------------------------------------//
//class CBTSecMan
//------------------------------------------------------------------------//
void Panic(TBTSecPanic aPanic)
	{
	LOG_FUNC
	User::Panic(KBTSecPanic, aPanic);
	}

CBTSecMan* CBTSecMan::NewL()
	{
	LOG_FUNC
	CBTSecMan* self = CBTSecMan::NewLC();
	CleanupStack::Pop();	//self
	return self;
	}

CBTSecMan* CBTSecMan::NewLC()
	{
	LOG_FUNC
	CBTSecMan* self = new(ELeave) CBTSecMan();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CBTSecMan::CBTSecMan()
	: iAccessRequesters(KBTSecManAccessRequesterArrayGranularity),
	  iNotifierRequesters(KBTSecManNotifierRequesterArrayGranularity)
	{
	LOG_FUNC
	}

void CBTSecMan::ConstructL()
	{
	LOG_FUNC
	}

void CBTSecMan::SetPhysicalLinksMgr(const CPhysicalLinksManager& aConnectionsMgr)
	{
	LOG_FUNC
	iPhysicalLinksManager = &const_cast<CPhysicalLinksManager&>(aConnectionsMgr);
	}

CPhysicalLinksManager& CBTSecMan::ConnectionsManager() const
	{
	LOG_FUNC
	return *iPhysicalLinksManager;
	}


CBTSecMan::~CBTSecMan()
	{
	LOG_FUNC

	iAccessRequesters.ResetAndDestroy();
	iAccessRequesters.Close();

	iNotifierRequesters.ResetAndDestroy();
	iNotifierRequesters.Close();
	}

void CBTSecMan::AccessRequestL(const TBTServiceSecurity& aSecurity,
							   const TBTServiceSecurityPerDevice* const aOverride,
							   const TBTDevAddr& aBDAddr,
							   MAccessRequestResponseHandler& aRequester)
/**
Handle an access request...
Create a new CBTAccessRequester object to handle the request.
**/
	{
	LOG_FUNC
	// find the baseband this SAP is running on
	CPhysicalLink& con = *iPhysicalLinksManager->FindPhysicalLink(aBDAddr);
	CBTAccessRequester* p = CBTAccessRequester::NewLC(con, aSecurity,
													  aOverride,
													  aRequester,
													  *this);
	User::LeaveIfError(iAccessRequesters.Append(p));
	CleanupStack::Pop(); //clean up of p now handled by iAccessRequesters
	// Try to start- it may not happen (depends on if device retreived from registry
	p->DoRequest();
	
	}

void CBTSecMan::CancelRequest(MAccessRequestResponseHandler& aRequester)
	{
	LOG_FUNC
	// search through access requesters to find correct one
	LOG1(_L("sec\tCBTSecMan::CancelRequest from SAP 0x%08x"), &aRequester)
	TInt count = iAccessRequesters.Count();

	for (TInt i=(count-1); i>=0; i--)
		{
		CBTAccessRequester* requester = iAccessRequesters[i];
		if(&requester->ServiceRequester() == &aRequester)
			{
			// should assert that aRequester is the same as in AccessRequester?
			LOG(_L("sec\tRemoving AccessRequester..."))
			iAccessRequesters.Remove(i);
			delete requester;
			break;
			}
		}
	}
	
void CBTSecMan::GetPassKeyLengthAndOriginator(const TBTDevAddr& aAddr, TUint& aPasskeyMinLength,
				   	   					      TBool& aLocallyInitiatedAuthentication)

/**
If authorisation request was initiated locally it will return true and will
return the passkey minimal length requred by user
**/
	{
	LOG_FUNC

	TUint tmpPasskeyLength = 0;
	TBool locallyInitiated = EFalse;
	TInt count = iAccessRequesters.Count();

	if (count == 0)
		{
		aLocallyInitiatedAuthentication = EFalse;
		return;
		}

	// find all pending AccessRequesters for given BTAddr and find maximum of PasskeyMinLength
	
	for (TInt i=0; i<count;i++)
		{
		CBTAccessRequester* requester = iAccessRequesters[i];
		if (requester->IsAuthenticationReqPending(aAddr, tmpPasskeyLength))
			{
			locallyInitiated = ETrue;
			if (aPasskeyMinLength < tmpPasskeyLength)
				{
				aPasskeyMinLength = tmpPasskeyLength;
				}
			}	
		}
	
	aLocallyInitiatedAuthentication = locallyInitiated;		
	}
	
void CBTSecMan::AuthenticationInProgress()
/**
When authorisation request was sent to HW, HCI will notify SecMan
**/
	{
	LOG_FUNC
	
	// find first pending AccessRequesters and set AuthenticationInProgress flag
	for (TInt i=0; i<iAccessRequesters.Count(); i++)
		{
		CBTAccessRequester* requester = iAccessRequesters[i];
		
		if (requester->AuthenticationRequired() && !requester->AuthenticationInProgress())
			{
			requester->SetAuthenticationInProgress();
			break;
			}		
		}
	}

void CBTSecMan::AccessRequestComplete(CBTAccessRequester* aAccessRequester, TInt aResult)
/**
The access request has been fully completed.
Delete the CBTAccessRequester that was handling the request.
**/
	{
	LOG_FUNC

	TInt count = iAccessRequesters.Count();

	ASSERT_DEBUG(count);

	// find the originating service *now*
	MAccessRequestResponseHandler& service = const_cast<MAccessRequestResponseHandler&>
											(aAccessRequester->ServiceRequester());

	// clean up the AccessRequester object
	TInt i;
	for (i=(count-1); i>=0; i--)
		{
		//find aRequester in iAccessRequesters and delete it
		if(iAccessRequesters[i] == aAccessRequester)
			{
			iAccessRequesters.Remove(i);
			delete aAccessRequester;
			aAccessRequester = NULL;
			break;
			}
		}
	//compress the array if needs be
	if ((count!=0) && (i!=count))
		{
		iAccessRequesters.GranularCompress();
		}

	// now tell the service
	service.AccessRequestComplete(aResult);
	}


void CBTSecMan::AddNotifierRequestToQueL(CSecNotifierRequester& aRequest)
/**
Add notifier request to front of queue.  If there are no other requests already in the queue,
initiate this request.
**/
	{
	LOG_FUNC
	TInt count = iNotifierRequesters.Count();
	User::LeaveIfError(iNotifierRequesters.Insert(&aRequest,0));	//add to front of queue since requests are taken from the back
	if (count == 0)	//ok since count was calculated before we inserted the new element
		{
		iActiveNotifierRequester = &aRequest;
		aRequest.DoRequest();
		}
	else
		{
		LOG(_L("Secman: Request NOT initiated, should start later..."));
		}
	}

void CBTSecMan::RemoveNotifierRequestFromQue(CSecNotifierRequester& aRequest)
/**
Remove the request from the queue.  If aRequest is the currently active request then we can activate
the next one in the queue.  Otherwise, aRequest is being deleted prematurely and we must simply
remove it from the array.
**/
	{
	LOG_FUNC
	LOG(_L("sec\tCBTSecMan - removing notifier request from queue"));
	TInt count = iNotifierRequesters.Count();
	TInt found = 0;
	for (TInt i=(count-1); i>=0; i--)
		{
		if (iNotifierRequesters[i] == &aRequest)
			{
			found++;
			iNotifierRequesters.Remove(i);
			}
		}
	__ASSERT_DEBUG(found, User::Panic(KBTSecPanic, EBTSecBadNotifierArray));

	if (&aRequest == iActiveNotifierRequester)
		{
		//start the next request if there is one...
		count = iNotifierRequesters.Count();
		if (count > 0)
			{
			LOG(_L("sec\tCBTSecMan - auto-starting next notifier request from queue"));
			iActiveNotifierRequester = iNotifierRequesters[count-1];
			iActiveNotifierRequester->DoRequest();
			}
		}
	}


CSecNotifierRequester::CSecNotifierRequester(CBTSecMan& aSecMan)
: CActive(EPriorityStandard),
  iInquiryMgr(aSecMan.ConnectionsManager().LinkManagerProtocol().InquiryMgr()),
  iSecMgr(aSecMan)
	{
	LOG_FUNC
	}

void CSecNotifierRequester::ConstructL(const TBTDevAddr& aAddr)
	{
	LOG_FUNC
	User::LeaveIfError(iNotifier.Connect());

	// find the name at this stage for this device - may not be there yet
	iDeviceName = iInquiryMgr.DeviceNameFromCache(aAddr);

	if (!iDeviceName || iDeviceName->Length() ==0)
		{
		// cache didn't have name - so we'll ask for it as a HR action
		TRAP_IGNORE(iHR = iInquiryMgr.NewHostResolverL());
		iHRNameRecord = new TNameRecord;

		// ignore error - only an optimisation - don't want to leave if there's
		// a problem doing this optimisation
		if (iHR && iHRNameRecord)
			{
			iHR->SetNotify(this);

			TInquirySockAddr i;
			i.SetAction(KHostResName);
			i.SetBTAddr(aAddr);
			iHRNameRecord->iAddr = i;
			iHR->GetByAddress(*iHRNameRecord);
			}
		}
		
	iDevAddr = aAddr;
	
	iSecMgr.AddNotifierRequestToQueL(*this);
	iIsAddedToNotifierQue = ETrue;
	}

CSecNotifierRequester::~CSecNotifierRequester()
	{
	LOG_FUNC
	Cancel();
	
	//remove ourself from the notifier que if we're still on it.
	if (iIsAddedToNotifierQue)
		{
		iSecMgr.RemoveNotifierRequestFromQue(*this);
		iIsAddedToNotifierQue = EFalse;
		}

	delete iHR;
	delete iHRNameRecord;
	delete iUpdateNotifierAO; 
	
	iNotifier.Close();
	}

void CSecNotifierRequester::QueryComplete(TInt aErr)
	{
	LOG_FUNC
	if ( (aErr==KErrNone) && (iHRNameRecord!=NULL) )
		{
		// now have device name - update notifiers
		// we do have a copy of the name - but it is now wide :-|
		// and also we have iDeviceName that is still NULL, so best bet is
		// to just set our pointer and use the cache one (which we *know* is there!)
		TBTDevAddr a = TBTSockAddr::Cast(iHRNameRecord->iAddr).BTAddr();
		iDeviceName = iInquiryMgr.DeviceNameFromCache(a);
		DoUpdateNotifier();
		}

	delete iHRNameRecord;
	iHRNameRecord = NULL;
	}

//------------------------------------------------------------------------//
//class CBTPinRequester
//------------------------------------------------------------------------//


CBTPinRequester* CBTPinRequester::NewL(CPhysicalLink& aParent,
									   MPINCodeResponseHandler& aRequester,
									   CBTSecMan& aSecMan,
									   TUint aPasskeyMinLength,
									   TBool aInternallyInitiated)
	{
	LOG_FUNC
	CBTPinRequester* s = CBTPinRequester::NewLC(aParent, aRequester, aSecMan,
	                                            aPasskeyMinLength, aInternallyInitiated);
	CleanupStack::Pop();
	return s;
	}

CBTPinRequester* CBTPinRequester::NewLC(CPhysicalLink& aParent,
										MPINCodeResponseHandler& aRequester,
										CBTSecMan& aSecMan,
										TUint aPasskeyMinLength,
										TBool aInternallyInitiated)
	{
	LOG_FUNC
	CBTPinRequester* s = new(ELeave) CBTPinRequester(aParent, aRequester, aSecMan,
	                                                 aPasskeyMinLength, aInternallyInitiated);
	CleanupStack::PushL(s);
	s->ConstructL(aParent.BDAddr());
	return s;
	}

CBTPinRequester::CBTPinRequester(CPhysicalLink& aParent,
								 MPINCodeResponseHandler& aRequester,
								 CBTSecMan& aSecMan,
								 TUint aPasskeyMinLength,
								 TBool aInternallyInitiated) :
	CSecNotifierRequester(aSecMan),
	iParent(aParent),
	iRequester(aRequester),
	iSecMan(aSecMan),
	iPasskeyMinLength(aPasskeyMinLength),
	iInternallyInitiated(aInternallyInitiated)
	{
	LOG_FUNC
	// a lot of inlines
	CActiveScheduler::Add(this);
	}

CBTPinRequester::~CBTPinRequester()
	{
	LOG_FUNC
	Cancel();
	}


void CBTPinRequester::DoUpdateNotifier()
	{
	LOG_FUNC
 	if(IsActive())
 		{	
 		if(!iUpdateNotifierAO)
 			{
 			//Create a new CSecNotifierUpdateAO object
 			TRAP_IGNORE(iUpdateNotifierAO = CSecNotifierUpdateAO::NewL(iNotifier, KBTManPinNotifierUid));
 			}
 		
 		if( (iUpdateNotifierAO) && (!iUpdateNotifierAO->IsActive()) )
 			{
 			TBTNotifierUpdateParamsPckg pckg;
 			if(iDeviceName)
 				{
 				TRAPD(err, pckg().iName = BTDeviceNameConverter::ToUnicodeL(*iDeviceName));
 				pckg().iResult = err; 	// Error code can be KErrNone 
 				if (err!=KErrNone)
	 				{
	 				pckg().iName = KNullDesC;
	 				}
 				}
 			else
 				{
 				pckg().iName = KNullDesC;
 				pckg().iResult = KErrNotFound;
 				}
 	
 			iUpdateNotifierAO->DoUpdate(pckg);
 			}
   		}
	}

void CBTPinRequester::DoRequest()
/**
It's our turn...start the RNotifier plugin that deals with authorisation.
**/
	{
	LOG_FUNC
	ASSERT_DEBUG(iDevAddr == iParent.BDAddr());
	
	iPasskeyParamsPckg().iBDAddr = iDevAddr;
	if (iDeviceName)
		{
		TRAPD(err, iPasskeyParamsPckg().iName =	BTDeviceNameConverter::ToUnicodeL(*iDeviceName));
		if (err!=KErrNone)
			{
			iPasskeyParamsPckg().iName = KNullDesC;
			}
		}
	else
		{
		iPasskeyParamsPckg().iName = KNullDesC;	
		}			
	iPasskeyParamsPckg().iPasskeyMinLength = iPasskeyMinLength;	
	iPasskeyParamsPckg().iLocallyInitiated = iInternallyInitiated;	

	iNotifier.StartNotifierAndGetResponse(iStatus, KBTManPinNotifierUid, iPasskeyParamsPckg, iPassKey);
	SetActive();
	}

void CBTPinRequester::FriendlyNameRetrieved(const TDesC& /*aName*/, TInt /*aResult*/)
	{
	LOG_FUNC
	// do nothing for now 
	}


void CBTPinRequester::DoCancel()
	{
	LOG_FUNC
	iNotifier.CancelNotifier(KBTManPinNotifierUid);
	if (iUpdateNotifierAO)
		{
	 	iUpdateNotifierAO->Cancel();
		}
	}

void CBTPinRequester::RunL()
	{
	LOG_FUNC
	//got a PIN or error, so finish off: unload the plugin
	iNotifier.CancelNotifier(KBTManPinNotifierUid);

	//remove ourself from the notifier que, allowing the next notifier to be activated
	iSecMan.RemoveNotifierRequestFromQue(*this);
	iIsAddedToNotifierQue = EFalse;

	ASSERT_DEBUG(iPasskeyParamsPckg().iBDAddr == iParent.BDAddr());
	if (iStatus.Int())
		{
		// it failed - be unpairable
		iRequester.PINCodeRequestNegativeReply((iParent.BDAddr()));
		}
	else
		{
		// got a PIN
		iRequester.PINCodeRequestReply(iParent.BDAddr(),iPassKey);		
		iParent.SetPassKey(iPassKey);
		iParent.PinRequestSent();
		iParent.DeleteLinkKeyL(); //only delete link key when user has entered PIN
		}
	// in either case tell parent as we're done.
	iParent.PinRequestComplete();
	}

#ifdef __FLOG_ACTIVE
TInt CBTPinRequester::RunError(TInt aError) 
#else
TInt CBTPinRequester::RunError(TInt /*aError*/)
#endif
	{
	LOG_FUNC
	ASSERT_DEBUG(iPasskeyParamsPckg().iBDAddr == iParent.BDAddr());
	LOG1(_L("sec\tCBTPinRequester::RunError(%d)"), aError);
	iRequester.PINCodeRequestNegativeReply(iParent.BDAddr());
	iParent.PinRequestComplete();
	return KErrNone;
	}

//------------------------------------------------------------------------//
//class CBTAuthorisor
//------------------------------------------------------------------------//

CBTAuthorisor* CBTAuthorisor::NewL(CBTAccessRequester& aParent, TUid aServiceUID)
	{
	LOG_FUNC
	CBTAuthorisor* s = CBTAuthorisor::NewLC(aParent, aServiceUID);
	CleanupStack::Pop();
	return s;
	}

CBTAuthorisor* CBTAuthorisor::NewLC(CBTAccessRequester& aParent, TUid aServiceUID)
	{
	LOG_FUNC
	CBTAuthorisor* s = new(ELeave) CBTAuthorisor(aParent, aServiceUID);
	CleanupStack::PushL(s);
	s->ConstructL(aParent.DeviceAddress());
	return s;
	}

CBTAuthorisor::CBTAuthorisor(CBTAccessRequester& aAccessRequester, TUid aServiceUID) :
	CSecNotifierRequester(aAccessRequester.SecMan()),iAccessRequester(aAccessRequester)
	{
	LOG_FUNC
	iAuthorisationParamsPckg().iUid = aServiceUID;
	CActiveScheduler::Add(this);
	}

CBTAuthorisor::~CBTAuthorisor()
	{
	LOG_FUNC
	Cancel();
	}


void CBTAuthorisor::DoUpdateNotifier()
	{
	LOG_FUNC 
 	if(IsActive())
 		{	
 		if(!iUpdateNotifierAO)
   			{
 			//Create a new CSecNotifierUpdateAO object
 			TRAP_IGNORE(iUpdateNotifierAO = CSecNotifierUpdateAO::NewL(iNotifier, KBTManAuthNotifierUid));
   			}
	
 		if( (iUpdateNotifierAO) && (!iUpdateNotifierAO->IsActive()) )
   			{
 			TBTNotifierUpdateParamsPckg pckg;
 			if(iDeviceName)
 				{
 				TRAPD(err, pckg().iName = BTDeviceNameConverter::ToUnicodeL(*iDeviceName));
 				pckg().iResult = err; 	// Error code can be KErrNone 
 				if (err!=KErrNone)
	 				{
	 				pckg().iName = KNullDesC;
	 				}
 				}
 			else
 				{
 				pckg().iName = KNullDesC;
 				pckg().iResult = KErrNotFound;
 				}
 				
 			iUpdateNotifierAO->DoUpdate(pckg);
 			}
 		}
	}

void CBTAuthorisor::DoRequest()
/**
Start the RNotifier plugin that deals with authorisation.
**/
	{
	LOG_FUNC
	TInt err(KErrNone);
	
	if (iDeviceName)
		{
		TRAP(err, iAuthorisationParamsPckg().iName = BTDeviceNameConverter::ToUnicodeL(*iDeviceName));
		if (err!=KErrNone)
			{
			iAuthorisationParamsPckg().iName = KNullDesC;
			}
		}
	else
		{
		iAuthorisationParamsPckg().iName = KNullDesC;
		}			
	iAuthorisationParamsPckg().iBDAddr  = iDevAddr;

	iNotifier.StartNotifierAndGetResponse(iStatus, KBTManAuthNotifierUid, iAuthorisationParamsPckg, iResultPckg);
	SetActive();
	}


void CBTAuthorisor::DoCancel()
	{
	LOG_FUNC

	iNotifier.CancelNotifier(KBTManAuthNotifierUid);
		
	}

void CBTAuthorisor::RunL()
	{
	LOG_FUNC
	//unload the plugin
	iNotifier.CancelNotifier(KBTManAuthNotifierUid);
	//remove ourself from the notifier que, allowing the next notifier to be activated
	iAccessRequester.SecMan().RemoveNotifierRequestFromQue(*this);
	iIsAddedToNotifierQue = EFalse;
	//check for errors + notify owner of completion
	LOG1(_L("sec\tCBTAuthorisor::RunL(): iStatus = %d"), iStatus.Int());
	if (iStatus.Int()!=KErrNone)
		{
		//error
		iAccessRequester.CompleteRequest(iStatus.Int());
		}
	else
		{
		iAccessRequester.AuthorisationComplete(iResultPckg());
		}
	}

TInt CBTAuthorisor::RunError(TInt aError)
	{
	LOG_FUNC
	//will never get called as our RunL doesn't leave.
	LOG1(_L("sec\tCBTAuthorisor::RunError(%d)"), aError);
	return aError;
	}


/**
	class CBTAccessRequester


	The access requester handles the state machine involved in allowing accesses:
	setting authentication, entering PINs, asking for authorisation and encrypting

	This class deals only with security procedures we initiate (which could be for
	inbound or outbound connections)

*/

CBTAccessRequester* CBTAccessRequester::NewLC(CPhysicalLink& aConnection,
											  const TBTServiceSecurity& aSecurityRequired,
  			  							      const TBTServiceSecurityPerDevice* const aOverride,
											  MAccessRequestResponseHandler& aRequester,
											  CBTSecMan& aParent)
	{
	LOG_FUNC
	CBTAccessRequester* s = new(ELeave) CBTAccessRequester(aConnection, aSecurityRequired,
															aOverride, aRequester, aParent);
	CleanupStack::PushL(s);
	s->ConstructL();
	return s;
	}


CBTAccessRequester::CBTAccessRequester(CPhysicalLink& aConnection,
									   const TBTServiceSecurity& aServiceSecurity,
 	  							       const TBTServiceSecurityPerDevice* const aOverride,
									   MAccessRequestResponseHandler& aRequester,
									   CBTSecMan& aParent) :
	iRequester(aRequester),
	iSecMan(aParent),
	iBaseband(aConnection),
	iServiceRequirements(aServiceSecurity),
	iOverride(aOverride),
	iIsSubscribedToConnection(EFalse),
	iDeviceRetrievedFromRegistry(EFalse),
	iQueLink(this),
	iAuthenticationInProgress(EFalse)
	{
	LOG_FUNC
	// try to get name for UI dialogs
	SetDeviceName();
	}

void CBTAccessRequester::ConstructL()
	{
	LOG_FUNC
	LOG2(_L("sec\tCBTAccessRequester 0x%08x constructed; sizeof %d"), this, sizeof(*this));
	}

void CBTAccessRequester::SetDeviceName()
	{
	LOG_FUNC
	// not *that* bad - mostly inlines

	iDeviceName = iSecMan.ConnectionsManager().
					LinkManagerProtocol().InquiryMgr().
					  DeviceNameFromCache(iBaseband.BDAddr());
	}


const MAccessRequestResponseHandler& CBTAccessRequester::ServiceRequester() const
	{
	LOG_FUNC
	return iRequester;
	}

const TBTDevAddr& CBTAccessRequester::DeviceAddress() const
	{
	LOG_FUNC
	return (iBaseband.BDAddr());
	}

void CBTAccessRequester::DoRequest()
/**
Subscribe to the baseband link notifier to find out about the link state.
This will kick off the state machine.
**/
	{
	LOG_FUNC
	LOG1(_L("sec\tAccessRequester 0x%08x DoRequest"),this);
	iBaseband.SubscribeLinkObserver(*this);
	iIsSubscribedToConnection = ETrue;

	// if the link is already up (e.g. we're SecMode 2 for L2CAP/RFCOMM) then proceed
	if (iBaseband.IsConnected())
		{
		TBTBasebandEventNotification event(ENotifyPhysicalLinkUp);
		PhysicalLinkChange(event); // kick statemachine
		}
	// else wait until it is retrieved
	}

void CBTAccessRequester::CompleteRequest(TInt aResult)
	{
	LOG_FUNC
	LOG2(_L("sec\tAccessRequester 0x%08x CompleteRequest, result %d"),this,aResult);
	iSecMan.AccessRequestComplete(this, aResult);
	}

CBTAccessRequester::~CBTAccessRequester()
	{
	LOG_FUNC
	LOG1(_L("sec\tAccessRequester 0x%08x Destroying"),this);
	if (iIsSubscribedToConnection)
		{
		iBaseband.UnsubscribeLinkObserver(*this);
		}
	delete iAuthorisor;
	}


void CBTAccessRequester::NewStateL()
/**
This access request has entered a new state.  Work out what we should do next.
**/
	{
	LOG_FUNC
	
 	// We should ensure that at each iteration through the state machine the requirements
 	// are up-to-date.  This is needed as the physical link may have changed
 	// some internal state e.g. as a result of the Registry having been modified.
 	iRequirements = OverallRequirements(iServiceRequirements, iBaseband.RemoteDevice());
 	
#ifdef _DEBUG
	_LIT(KRequestNone,					"Not Request.     ");
	_LIT(KRequestPending,				"Request Pending. ");
	_LIT(KRequestFailed,				"Request Failed.  ");
	_LIT(KRequestComplete,				"Request Complete.");

	TBuf<20> iStateName[4] = 
		{
		KRequestNone(),
		KRequestPending(),
		KRequestFailed(),
		KRequestComplete(),
		};

	TBTDevAddr addr = iBaseband.BDAddr();
	LOG3(_L("sec\tCBTAccessRequester::NewStateL(Authorised[%S] Authenticated[%S] Encrypted[%S])"),
		&iStateName[iState.AuthorisationState()],
		&iStateName[iState.AuthenticationState()], 
		&iStateName[iState.EncryptionState()]);
	LOG6(_L("\t\taddr[0x%02x%02x%02x%02x%02x%02x]"),
		addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);
	LOG4(_L("\t\tRequirements: a[%d] p[%d] e[%d] d[%d]"), iRequirements.AuthenticationRequired(),
		iRequirements.AuthorisationRequired(), iRequirements.EncryptionRequired(), iRequirements.Denied());
#endif

	if (iRequirements.AuthenticationRequired() && iRequirements.PasskeyMinLength() &&
	       iBaseband.Authenticated() )
		{
		LOG(_L("sec\tCheck min passkey length")) // for remotly initiated connection
		TBTPinCode passKey = iBaseband.PassKey();
		if ( passKey().iLength < iRequirements.PasskeyMinLength())
			{
			// the remote device is authenticated, but longer passkey is required
			LOG(_L("sec\tCBTAccessRequester::Complete(ACCESS DENIED) Passkey min length requirement is longer, then current used for authentication"));
			CompleteRequest(EBTSecManAccessDenied);
			return;			
			}		
		}

	//1. Check for any failures or for "Denied" in iRequirements
	//If anything that was required has failed, complete the request with EBTManAccessDenied and delete this.
	if ((iState.AuthenticationState() == TBTAccessRequestState::ERequestFailed && iRequirements.AuthenticationRequired()) 
		|| (iState.EncryptionState() == TBTAccessRequestState::ERequestFailed && iRequirements.EncryptionRequired())
		|| (iState.AuthorisationState() == TBTAccessRequestState::ERequestFailed && iRequirements.AuthorisationRequired())
		|| iRequirements.Denied())
		{
		LOG(_L("sec\tCBTAccessRequester::Complete(ACCESS DENIED)"));
		CompleteRequest(EBTSecManAccessDenied);
		}
	//2. Check for authentication
	else if (iRequirements.AuthenticationRequired() && 
			 !iBaseband.Authenticated() && !iBaseband.Encrypted())
		{
		// might have been encrypted by remote side - so no need to reauthenticate
		// this makes us more interoperable with controllers that don't honour erratum E2244
		LOG(_L("sec\tAuthentication required..."))
		//if we don't have a pending authentication request, start authentication.
		if (iState.AuthenticationState() != TBTAccessRequestState::ERequestPending)
			{
			LOG(_L("sec\tStarting Authentication..."));
			iState.SetAuthenticationState(TBTAccessRequestState::ERequestPending);
			User::LeaveIfError(iBaseband.Authenticate());
			}
		}	
	//3. Check for encryption
	else if (iRequirements.EncryptionRequired() && !iBaseband.Encrypted())
		{
		LOG(_L("sec\tEncryption required..."))

		if (!iBaseband.IsEncryptionDisabledForRoleSwitch())
			{
			//if we don't have a pending encryption request, start encryption.
			if (iState.EncryptionState() != TBTAccessRequestState::ERequestPending)
				{
				LOG(_L("sec\tStarting Encryption..."))
				iState.SetEncryptionState(TBTAccessRequestState::ERequestPending);
				User::LeaveIfError(iBaseband.ChangeEncryption(EPointToPointEncryption));
				}			
			}
		}
	//4. Check for authorisation
	else if (iRequirements.AuthorisationRequired() && 
	        (iState.AuthorisationState() == TBTAccessRequestState::ERequestNone || iState.AuthorisationState() == TBTAccessRequestState::ERequestPending))
		{
		//if we don't have a pending authorisation request, start authorisation.
		LOG(_L("sec\tAuthorisation required..."))
		if (iState.AuthorisationState() != TBTAccessRequestState::ERequestPending)
			{
			LOG(_L("sec\tStarting Authorisation..."))
			__ASSERT_ALWAYS(!iAuthorisor, User::Panic(KBTSecPanic,EBTSecAuthorisationRequestAlreadyExists));
			iAuthorisor = CBTAuthorisor::NewL(*this, iServiceRequirements.Uid());
			iState.SetAuthorisationState(TBTAccessRequestState::ERequestPending);
			}
		}
	//5. Before EBTManAccessGranted, doublecheck all the checks were done
	else
		{
		TBool authenticationComplete;
		TBool encryptionComplete;
		TBool authorisationComplete;
		
		authenticationComplete = !iRequirements.AuthenticationRequired() || 
		                         (iRequirements.AuthenticationRequired() && 
		                           (iBaseband.Authenticated() || iBaseband.Encrypted()));
		                         
		                         
		                         
		encryptionComplete = !iRequirements.EncryptionRequired() ||
							 (iRequirements.EncryptionRequired() && iBaseband.Encrypted());
							 
							 
		authorisationComplete = !iRequirements.AuthorisationRequired() ||
								(iRequirements.AuthorisationRequired()  && iState.AuthorisationState()==TBTAccessRequestState::ERequestComplete);
		
		                
		if (authenticationComplete && encryptionComplete && authorisationComplete)
			{
			LOG(_L("sec\tEBTManAccessGranted"))
			CompleteRequest(EBTSecManAccessGranted);	
			}
		else
			{
			LOG(_L("sec\tCBTAccessRequester::Complete(ACCESS DENIED) during double check"));
			CompleteRequest(EBTSecManAccessDenied);			
			}
		}
	}

TBTAccessRequirements CBTAccessRequester::OverallRequirements(const TBTServiceSecurity& aServiceSecurity,
														 const TBTNamelessDevice& aDevice)
/**
Take the access requirements of the service and compare them with the permissions given 
to the device by the user.  This results in a list of tasks to be carried out by the 
access requester before it may let the connection proceed.
**/
	{
	LOG_FUNC
	//Create the initial stab at the access requirements from the requirements of this service...
	TBTAccessRequirements req;
	req.SetAuthentication(aServiceSecurity.AuthenticationRequired());
	req.SetAuthorisation(aServiceSecurity.AuthorisationRequired());
	req.SetEncryption(aServiceSecurity.EncryptionRequired());
	req.SetDenied(aServiceSecurity.Denied());
	req.SetPasskeyMinLength(aServiceSecurity.PasskeyMinLength());

	//Look at the global security setting of the device...
	if (aDevice.IsValidGlobalSecurity())
		{
		LOG(_L("sec\tDevice has global security settings"));
		TBTDeviceSecurity devSec = aDevice.GlobalSecurity();
		if (devSec.Banned())
			req.SetDenied(ETrue);
		if (devSec.Encrypt())
			req.SetEncryption(ETrue);
		if (devSec.NoAuthenticate())
			req.SetAuthentication(EFalse);
		if (devSec.NoAuthorise())
			req.SetAuthorisation(EFalse);
		if (devSec.PasskeyMinLength() && devSec.PasskeyMinLength() > req.PasskeyMinLength())
			req.SetPasskeyMinLength(devSec.PasskeyMinLength());
		}

	//Check to see if the device has anything specific about this service...

	if (iOverride)
		{
		// get the device security from the override (the override points to the correct device)
		LOG(_L("sec\tSAP has overriden global security settings for device"));
		const TBTDeviceSecurity& servSec = iOverride->DeviceSecurity();
		if (servSec.Banned())
			req.SetDenied(ETrue);
		if (servSec.Encrypt())
			req.SetEncryption(ETrue);
		if (servSec.NoAuthenticate())
			req.SetAuthentication(EFalse);
		if (servSec.NoAuthorise())
			req.SetAuthorisation(EFalse);
		if (servSec.PasskeyMinLength() && servSec.PasskeyMinLength() > req.PasskeyMinLength() )
		    req.SetPasskeyMinLength(servSec.PasskeyMinLength());
		}

	// if only encryption is required, then must need authentication
	if (req.EncryptionRequired())
		{
		req.SetAuthentication(ETrue);
		}

	return req;
	}

void CBTAccessRequester::FriendlyNameRetrieved(const TDesC& /*aName*/, TInt /*aResult*/)

	{
	LOG_FUNC
	// do nothing for now
	}

void CBTAccessRequester::DeviceRetrieved(const TBTNamelessDevice& aDevice, TInt aError)
/**
	A device has been retrieved from the registry
	If it's for us we need to continue the access request based on the overrides it has

	This object chooses not to keep a copy of the device, but notes its availability
**/
	{
	LOG_FUNC
	if (aDevice.Address() != iBaseband.BDAddr())
		{
		return; // wasn't for us
		}
	
	TInt err = aError;

	iDeviceRetrievedFromRegistry = ETrue;

	if (err == KErrNone || err == KErrNotFound)
		{
		// proceed with the security check...
		iRequirements = OverallRequirements(iServiceRequirements, aDevice);
		}

	if (err!=KErrNone)
		{
		CompleteRequest(err);
		}
	}


void CBTAccessRequester::AuthorisationComplete(TBool aResult)
/**
We have a new authorisation state.
**/
	{
	LOG_FUNC
	LOG1(_L("sec\tCBTAccessRequester::AuthorisationComplete(%d)"), aResult);
	if (aResult)
		{
		iState.SetAuthorisationState(TBTAccessRequestState::ERequestComplete);
		}
	else
		{
		iState.SetAuthorisationState(TBTAccessRequestState::ERequestFailed);
		}
	TRAPD(err,NewStateL());
	if (err)
		{
		CompleteRequest(err);
		}
	}


void CBTAccessRequester::PhysicalLinkChange(const TBTBasebandEventNotification & aEvent, CPhysicalLink& /*aPhysicalLink*/)
	{
	LOG_FUNC
	// only forward events that secman is interested in
	// linkup, linkdown, encryption, authentication, error
	// Care needed: other events may harm operation of secman 
	// and open security hole, such as ENotifySniffMode, ENotifyParkMode
 	// and ENotifyHoldMode
 	TBTPhysicalLinkStateNotifier secmanEvents = static_cast<TBTPhysicalLinkStateNotifier>
 	           (ENotifyPhysicalLinkUp |
 	            ENotifyPhysicalLinkDown |
	            ENotifyPhysicalLinkError |
	            ENotifyAuthenticationComplete |
	            ENotifyEncryptionChangeOn |
 	            ENotifyEncryptionChangeOff);
		            
	if (aEvent.EventType() & secmanEvents)
		{  
		PhysicalLinkChange(aEvent); 		}
 	// else drop
	}

void CBTAccessRequester::PhysicalLinkChange(const TBTBasebandEventNotification & aEvent)
	{
	LOG_FUNC
	if(aEvent.EventType() & (ENotifyPhysicalLinkDown | ENotifyPhysicalLinkError))
		{
		CompleteRequest(aEvent.ErrorCode());
		return;
		}

	if (iState.AuthenticationState() == TBTAccessRequestState::ERequestPending && 
		aEvent.EventType() == ENotifyAuthenticationComplete)
		{
		LOG(_L("sec\tAuthentication complete..."))

		if(aEvent.ErrorCode() != KErrNone)
			{
			iState.SetAuthenticationState(TBTAccessRequestState::ERequestFailed);
			//if Auth fail then ensure any previously stored Linkkey is removed
			//Don't know which AccessRequester initiated this so can't complete
			//the request with the error code, only thing to do is trap this error
			//Deleting a LinkKey is not Critical merely cosmetic
			LOG(_L("CPhysicalLink: Deleting link key due to failed Authentication"))
			TRAP_IGNORE(iBaseband.DeleteLinkKeyL());
			}
		else
			{
			iState.SetAuthenticationState(TBTAccessRequestState::ERequestComplete);
			}
		}

	if (iState.EncryptionState() == TBTAccessRequestState::ERequestPending && 
		(aEvent.EventType() == ENotifyEncryptionChangeOn || aEvent.EventType() == ENotifyEncryptionChangeOff))
		{
		LOG(_L("sec\tEncryption Change complete..."))
		
		// We can try again as there was a race with some other PHY modification
		// Curiously, the firmware always return ELMPErrorTransactionCollision (0x23) for both
		// kinds of transaction collisions (0x23,0x2a), we guard against both situations here
		// anyway just to be safe.
		if(aEvent.ErrorCode() == ELMPErrorTransactionCollision || aEvent.ErrorCode() == EDifferentTransactionCollision)
			{	
			// This will force the state machine logic to try sending the command again
			iState.SetEncryptionState(TBTAccessRequestState::ERequestNone);
			}
		else if(aEvent.ErrorCode() != KErrNone)
			{
			
			iState.SetEncryptionState(TBTAccessRequestState::ERequestFailed);
			}
		else
			{
			iState.SetEncryptionState(TBTAccessRequestState::ERequestComplete);
			}
		}

	TRAPD(err,NewStateL());
	if (err)
		{
		CompleteRequest(err);
		}
	}


CBTSecMan& CBTAccessRequester::SecMan()
	{
	LOG_FUNC
	return iSecMan;
	}

TBool CBTAccessRequester::IsAuthenticationReqPending(const TBTDevAddr& aAddr, TUint& aPasskeyMinLength)
	{
	LOG_FUNC
		if (iState.AuthenticationState() == TBTAccessRequestState::ERequestPending &&
		    iAuthenticationInProgress && aAddr == iBaseband.BDAddr())
			{
			aPasskeyMinLength = iServiceRequirements.PasskeyMinLength();
			return ETrue;
			}
		else 
			{
			aPasskeyMinLength = 0;
			return EFalse;
			}
	}

TBool CBTAccessRequester::AuthenticationRequired() const
	{
	LOG_FUNC
	return iRequirements.AuthenticationRequired();
	}

TBool CBTAccessRequester::AuthenticationInProgress() const
	{
	LOG_FUNC
	return iAuthenticationInProgress;
	}

void  CBTAccessRequester::SetAuthenticationInProgress()
	{
	LOG_FUNC
	iAuthenticationInProgress = ETrue;
	}
	
	 
 //------------------------------------------------------------------------//
 //class CSecNotifierUpdateAO
 //------------------------------------------------------------------------//
 
CSecNotifierUpdateAO* CSecNotifierUpdateAO::NewL(RNotifier& aNotifier, TUid aNotifierUid)
 	{
	LOG_FUNC
 	CSecNotifierUpdateAO* s = CSecNotifierUpdateAO::NewLC(aNotifier, aNotifierUid);
 	CleanupStack::Pop();
 	return s;
 	}
 
CSecNotifierUpdateAO* CSecNotifierUpdateAO::NewLC(RNotifier& aNotifier, TUid aNotifierUid)
 	{
	LOG_FUNC
 	LOG(_L("sec\tCSecNotifierUpdateAO::NewLC()"));
 	CSecNotifierUpdateAO* s = new(ELeave) CSecNotifierUpdateAO();
 	CleanupStack::PushL(s);
 	s->ConstructL(aNotifier, aNotifierUid);
 	return s;
 	}
 
CSecNotifierUpdateAO::CSecNotifierUpdateAO()
: CActive(EPriorityStandard)
 	{
	LOG_FUNC
 	CActiveScheduler::Add(this);
 	}
 
CSecNotifierUpdateAO::~CSecNotifierUpdateAO()
 	{
	LOG_FUNC
 	Cancel();
 	}
 
void CSecNotifierUpdateAO::ConstructL(RNotifier& aNotifier, TUid aNotifierUid)
 	{
	LOG_FUNC
 	iNotifier = aNotifier;
 	iNotifierUid = aNotifierUid;	
 	}
 	
void CSecNotifierUpdateAO::DoUpdate(const TBTNotifierUpdateParamsPckg& aPckg)
 	{
	LOG_FUNC
 	//Retain a copy so that it does not go out of memory scope
 	iPckg = aPckg;
 	
 	//we're not expecting an answer...
	iNotifier.UpdateNotifierAndGetResponse(iStatus, iNotifierUid, iPckg, iAnswer);
 	SetActive();	
 	}
 	
void CSecNotifierUpdateAO::RunL()
 	{
	LOG_FUNC
 	//We can't do anything if an error is returned - just make sure we haven't done anything stupid...	
 	__ASSERT_DEBUG((iStatus==KErrNone)||(iStatus==KErrNoMemory)||(iStatus==KErrNotReady), User::Panic(KBTSecPanic, EBTSecBadNotifierUpdate));
 	}
 	
void CSecNotifierUpdateAO::DoCancel()
 	{
	LOG_FUNC
 	LOG(_L("sec\tCSecNotifierUpdateAO::DoCancel()"));
	iNotifier.CancelNotifier(iNotifierUid); // no other API on Notifier to just cancel the update; but typically we'll want to cancel the whole notifier at this point(?)
	}
 	
TInt CSecNotifierUpdateAO::RunError(TInt aError)
 	{
	LOG_FUNC
 	LOG1(_L("sec\tCSecNotifierUpdateAO::RunError(%d)"), aError);
 	
 	#ifndef __FLOGGING__
 		aError += 0; //Remove Compiler warning
 	#endif
 	 
 	return KErrNone;	
 	}
