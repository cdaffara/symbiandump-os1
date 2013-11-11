// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of the helper active objects that talk to AVDTP
// 
//

/**
 @publishedPartner
*/

#include "gavdpInternal.h"
#include "gavdpinterface.h"

/**
The default helper has EPriorityStandard.  Some of the derived helpers
will set their priority to something different when running.  This enables
enforcement of orderings, as because both a helper and an indication may
have an outstanding completion we need to ensure that these are scheduled
in the order that will cause the notifications to the client in the correct
order.

An example of this:
AVDTP recieves an Open, which it indicates to GAVDP.  
On receipt of the Open GAVDP will issue socket connects.
AVDTP completes the socket connects.
AVDTP receives an incoming Start and issues a Start indication to GAVDP by Ioctl completion.
GAVDP now has two outstanding completed requests: 
	- the Start indication (leading to GAVDP_StartIndication).
	- the UPlaneConnect completion (leading to GAVDP_BearerReady).
Unless we ensure that the connect is processed before the start, the client
may receive the start indication before it has sockets available.
*/
CGavdpHelper::CGavdpHelper(CGavdp& aGavdp, MGavdpUser& aUser)
: CActive(EPriorityStandard),
  iGavdp(&aGavdp),
  iUser(aUser)
	{
	CActiveScheduler::Add(this);
	}

TInt CGavdpHelper::RunError(TInt aError)
	{
	RDebug::Print(_L("GAVDP::RunError %d"), aError);
	
	if (iGavdp)
		{	
		Gavdp().ServiceComplete(aError);
		}
	
	User().GAVDP_Error(aError, KNullDesC8); // helpers with extended error can't use this path
	//delete this whether or not we know about our CGavdp owner:
	//if we do - it will no longer know about us (see CGavdp::ServiceComplete)
	//if we don't - it will have detached from us earlier and expect us to
	//clean ourselves up
	delete this;
	
	return KErrNone;
	}	

void CGavdpHelper::CheckFailedL()
	{
	// if failed, leave and tell Gavdp service is complete
	User::LeaveIfError(iStatus.Int());
	}
	
/**
Called by helpers to inform GAVDP they no-longer need its services, but still have
calls to make to the User
The helper will be detached from GAVDP, and upon completion of their duties they should
delete themselves
This enables GAVDP to do more for the client while the helper carries on
**/
void CGavdpHelper::DetachFromGavdp(TInt aError)
	{
	Gavdp().ServiceComplete(aError);
	iGavdp = NULL;
	}
	
inline MGavdpUser& CGavdpHelper::User()
	{
	return iUser;
	}
	
CGavdpDiscover* CGavdpDiscover::NewL(CGavdp& aGavdp, MGavdpUser& aUser)
	{
	return new (ELeave) CGavdpDiscover(aGavdp, aUser);
	}
	
CGavdpDiscover::CGavdpDiscover(CGavdp& aGavdp, MGavdpUser& aUser)
								   
: CGavdpHelper(aGavdp, aUser)
	{
	}

CGavdpDiscover::~CGavdpDiscover()
	{
	Cancel();
	}
	
void CGavdpDiscover::DoCancel()
	{
	AvdtpRequester().CancelAll();
	}
	
void CGavdpDiscover::RunL()
	{
	CheckFailedL();
	// found n SEPs - create a buffer into which to place the details
	// short circuit this if zero SEPs found
	User::LeaveIfError(iConfirm().iResult);
	DetachFromGavdp(KErrNone);

	TFixedArray<TAvdtpSEPInfo, KMaxAvdtpNumSEPs>& sepInfos = iConfirm().iDiscoveredSEPs();
	for (TInt i=0;i<=iConfirm().iNumSEPs-1;i++)
		{
		const TAvdtpSEPInfo info = sepInfos[i];
		User().GAVDP_SEPDiscovered(info);
		}

	User().GAVDP_SEPDiscoveryComplete();
	
	delete this;
	}
	
	
void CGavdpDiscover::Begin()
	{
	AvdtpRequester().Ioctl(EDiscoverSEPs, iStatus, &iConfirm, KSolBtAVDTPSignalling);
	SetActive();	
	}


CGavdpGetCaps* CGavdpGetCaps::NewL(CGavdp& aGavdp,
					  			  MGavdpUser& aUser,
					  			  TSEID aSEID,
					  			  TAvdtpServiceCategories aInterestingCategories)
	{
	CGavdpGetCaps* g = new (ELeave) CGavdpGetCaps(aGavdp, aUser, aSEID, aInterestingCategories);
	CleanupStack::PushL(g);
	g->ConstructL();
	CleanupStack::Pop(g);
	return g;
	}
	
void CGavdpGetCaps::ConstructL()
	{
	TPckg<TSEID> seidPckg(iSEID);
	iSEPCapBuf = seidPckg.AllocL();
	iSEPCapPtr.Set(iSEPCapBuf->Des());
	}
	

CGavdpGetCaps::CGavdpGetCaps(CGavdp& aGavdp, MGavdpUser& aUser,
							 TSEID aSEID, TAvdtpServiceCategories aInterestingCategories)
: CGavdpHelper(aGavdp, aUser), iSEPCapPtr(0,0), iSEID(aSEID), iInteresting(aInterestingCategories)
	{
	}
	
CGavdpGetCaps::~CGavdpGetCaps()
	{
	Cancel();
	delete iSEPCapBuf;
	}

void CGavdpGetCaps::DoCancel()
	{
	AvdtpRequester().CancelAll();
	}

void CGavdpGetCaps::Begin()
	{
	AvdtpRequester().Ioctl(EGetCapabilities, iStatus ,&iSEPCapPtr, KSolBtAVDTPSignalling);
	SetActive();
	}

void CGavdpGetCaps::RunL()
	{
	CheckFailedL();
	
	// iSEPCapPtr now has mask of capabilities at this point
	TAvdtpServiceCatBitMask sepCapMask = *reinterpret_cast<const TAvdtpServiceCatBitMask*>(iSEPCapPtr.Ptr());
	// don't trouble user with ones they'd ignore anyway
	sepCapMask &= iInteresting();
		
	if (!sepCapMask)
		{
		// the remote sep has no capabilities the user is interested in
		// tell them NotSupported
		User::Leave(KErrNotSupported);
		}

	for (TInt index=0; index<ENumberOfServiceCategories; index++)
		{
		// go through each bit
		if (sepCapMask & (1<<index)) // test for bit set
			{		
			// it's set - create a buffer big enough to receive the value
			const TAvdtpServiceCategory servcat = static_cast<TAvdtpServiceCategory>(index+1);
			
			TInt bufsize =0;
			
			switch (servcat)
				{
				case EServiceCategoryMediaTransport:
					{
					bufsize = sizeof(TAvdtpMediaTransportCapabilities);
					}
					break;
				case EServiceCategoryReporting:
					{
					bufsize = sizeof(TAvdtpReportingCapabilities);
					}
					break;
				case EServiceCategoryRecovery:
					{
					bufsize = sizeof(TAvdtpRecoveryCapabilities);
					}
					break;
				case EServiceCategoryContentProtection:
					{
					bufsize = sizeof(TAvdtpContentProtectionCapabilities);
					}
					break;
				case EServiceCategoryHeaderCompression:
					{
					bufsize = sizeof(TAvdtpHeaderCompressionCapabilities);	
					}
					break;
				case EServiceCategoryMultiplexing:
					{
					bufsize = sizeof(TAvdtpMultiplexingCapability);
					}
					break;
				case EServiceCategoryMediaCodec:
					{
					// to handle an arbitrary codec (different LOSCs)
					// we need to assume largest LOSC.  It's not around
					// for long, so should be ok to make it big.
					bufsize = KUnknownLOSC;
					}
					break;
				}
			
			if (iSEPCapBuf->Length() < bufsize)
				{
				iSEPCapBuf = iSEPCapBuf->ReAllocL(Max(bufsize, sizeof(TInternalGetSEPCapability)));
				}
			
			TPckgBuf<TInternalGetSEPCapability> capbuf;
			capbuf().iSEID = iSEID;
			capbuf().iServiceCategory = servcat;
			
			// send into socket, output will be capability
			iSEPCapPtr.Set(iSEPCapBuf->Des());
			iSEPCapPtr.Copy(capbuf);
			
			User::LeaveIfError(AvdtpRequester().GetOpt(EGetAVDTPCapabilityResponse, KSolBtAVDTPSignalling, iSEPCapPtr));

			TAvdtpServiceCapability* cap = NULL;
			
			TRAPD(err, cap = TAvdtpServiceCapability::AllocFromPDUL(servcat, *iSEPCapBuf));
			
			if (err == KErrNone)
				{
				User().GAVDP_SEPCapability(cap); //ownership transferred!!
				}
			else if (err == KErrNoMemory)
				{
				// Special case OOM conditions to allow the client code to see them.
				// Otherwise the 'assume capability unsupported' block below hides the
				// error.
				User::Leave(err);
				}
			else
				{
				// probably a capability not supported by us, so no point telling user
				// we don't want to leave though - try to process next capability
				}
			}
		}

	DetachFromGavdp(KErrNone);		
	User().GAVDP_SEPCapabilityComplete();
	// in future - should prob go back to scheduler and do one at a time		
	// we've got them all
	delete this;
	}

CGavdpAbort* CGavdpAbort::NewL(CGavdp& aGavdp, MGavdpUser& aUser, TSEID aSEID)
	{
	return new (ELeave) CGavdpAbort(aGavdp, aUser, aSEID);
	}
	
CGavdpAbort::CGavdpAbort(CGavdp& aGavdp, MGavdpUser& aUser, TSEID aSEID)
: CGavdpHelper(aGavdp, aUser)
	{
	iSEID() = aSEID;
	}

CGavdpAbort::~CGavdpAbort()
	{
	Cancel();
	}

void CGavdpAbort::DoCancel()
	{
	AvdtpRequester().CancelAll();
	}

void CGavdpAbort::Begin()
	{
	SetPriority(EPriorityHigh);
	AvdtpRequester().Ioctl(EAbortStream, iStatus ,&iSEID, KSolBtAVDTPSignalling);
	SetActive();
	}

void CGavdpAbort::RunL()
	{
	CheckFailedL();
	DetachFromGavdp(KErrNone);	

	User().GAVDP_AbortStreamConfirm();
	
	delete this;
	}



CGavdpStart* CGavdpStart::NewL(CGavdp& aGavdp,
					  		   MGavdpUser& aUser,
					  		   TSEID aSeid)
	{
	return new (ELeave) CGavdpStart(aGavdp, aUser, aSeid);
	}
	
CGavdpStart::CGavdpStart(CGavdp& aGavdp, MGavdpUser& aUser, TSEID aSeid)
: CGavdpHelper(aGavdp, aUser), iSeidPckg(aSeid)
	{
	}

CGavdpStart::~CGavdpStart()
	{
	Cancel();
	}
	
void CGavdpStart::DoCancel()
	{
	AvdtpRequester().CancelAll();
	}

	
void CGavdpStart::Begin()
	{
	AvdtpRequester().Ioctl(EStartStreaming, iStatus, &iSeidPckg, KSolBtAVDTPSignalling);
	SetActive();
	}
	
void CGavdpStart::RunL()
	{
	// at present since GAVDP only allows one seid to start at a time then there is
	// no need for extended info, AVDTP also makes this assumption
	CheckFailedL();
	DetachFromGavdp(KErrNone);
	
	User().GAVDP_StartStreamsConfirm();

	delete this;
	}




CGavdpSuspend* CGavdpSuspend::NewL(CGavdp& aGavdp,
					  		   MGavdpUser& aUser,
					  		   TSEID aSeid)
	{
	return new (ELeave) CGavdpSuspend(aGavdp, aUser, aSeid);
	}
	
CGavdpSuspend::CGavdpSuspend(CGavdp& aGavdp, MGavdpUser& aUser, TSEID aSeid)
: CGavdpHelper(aGavdp, aUser), iSeidPckg(aSeid)
	{
	}

CGavdpSuspend::~CGavdpSuspend()
	{
	Cancel();
	}

void CGavdpSuspend::DoCancel()
	{
	AvdtpRequester().CancelAll();
	}

void CGavdpSuspend::Begin()
	{
	AvdtpRequester().Ioctl(ESuspendStreaming, iStatus, &iSeidPckg, KSolBtAVDTPSignalling);
	SetActive();
	}
	
void CGavdpSuspend::RunL()
	{
	// at present since GAVDP only allows one seid to suspend at a time then there is
	// no need for extended info, AVDTP also makes this assumption
	CheckFailedL();
	DetachFromGavdp(KErrNone);
	
	User().GAVDP_SuspendStreamsConfirm();

	delete this;
	}



CGavdpSelectSEP* CGavdpSelectSEP::NewL(CGavdp& aGavdp, MGavdpUser& aUser)
	{
	return new (ELeave) CGavdpSelectSEP(aGavdp, aUser);
	}
	
CGavdpSelectSEP::CGavdpSelectSEP(CGavdp& aGavdp, MGavdpUser& aUser)
: CGavdpHelper(aGavdp, aUser)
	{
	}


CGavdpSelectSEP::~CGavdpSelectSEP()
	{
	Cancel();
	}
	
void CGavdpSelectSEP::DoCancel()
	{
	AvdtpRequester().CancelAll();
	}

void CGavdpSelectSEP::Begin()
	{
	AvdtpRequester().Ioctl(ESelectSEP, iStatus, &iRejDataBuf, KSolBtAVDTPSignalling);
	SetActive();
	}
	
void CGavdpSelectSEP::RunL()
	{
	// if the Ioctl itself failed then we might have an error in the iStatus...
	TInt result = iStatus.Int();
	CheckFailedL();
	DetachFromGavdp(KErrNone);	

	// the ioctl mechanism may have worked but the signalling failed - with extended data...
	result = iRejDataBuf().iError;
	if (result!=KErrNone)
		{
		// the error buffer contains extended failure data
		User().GAVDP_Error(result, iRejDataBuf);
		}
	else
		{
		User().GAVDP_ConfigurationConfirm();
		}
	delete this;
	}

	

CGavdpConnector* CGavdpConnector::NewL(CGavdp& aGavdp, MGavdpUser& aUser, const TBTDevAddr& aAddr, TBool aPassive)
	{
	return new (ELeave) CGavdpConnector(aGavdp, aUser, aAddr, aPassive);
	}

CGavdpConnector::CGavdpConnector(CGavdp& aGavdp, MGavdpUser& aUser, const TBTDevAddr& aAddr, TBool aPassive)
: CGavdpHelper(aGavdp, aUser)
	{
	iSocketAddress.SetBTAddr(aAddr);
	iSocketAddress.SetSession(ESignalling);
	iIsPassiveConnector = aPassive;
	}

CGavdpConnector::~CGavdpConnector()
	{
	Cancel();
	}

void CGavdpConnector::DoCancel()
	{
	//don't call cancel on the socket for passive connectors to prevent cancelling a listening socket
	if (!iIsPassiveConnector)
		{
		AvdtpRequester().CancelAll(); // ESOCK has better semantics to SAP than CancelConnect.
		}
	}

void CGavdpConnector::Begin()
	{
	// Ensure that once this is completed the RunL is scheduled
	// before handling completion of the indication ioctl.
	SetPriority(EPriorityStandard+1);
	AvdtpRequester().Connect(iSocketAddress, iStatus);
	SetActive();
	}

void CGavdpConnector::RunL()
	{
	// Do not call CGavdpHelper::CheckFailedL() here - avdtp will indicate a failure to connect, 
	// and GAVDP is informed via CGavdpIndicator::RunL().
	
	TInt error = iStatus.Int();
	DetachFromGavdp(error);	
	
	if (error == KErrNone)
		{
		User().GAVDP_ConnectConfirm(iSocketAddress.BTAddr());	// reuse the same callback to make life easier for app
		}
	else
		{
		User().GAVDP_Error(error, KNullDesC8);
		}
		
	delete this;
	}

CGavdpIndicator::~CGavdpIndicator()
	{
	Cancel();
	}
	
void CGavdpIndicator::DoCancel()
	{
	AvdtpIndicator().CancelAll();
	}

CGavdpIndicator* CGavdpIndicator::NewL(CGavdp& aGavdp, MGavdpUser& aUser)
	{
	return new (ELeave) CGavdpIndicator(aGavdp, aUser);
	}
	
CGavdpIndicator::CGavdpIndicator(CGavdp& aGavdp, MGavdpUser& aUser)
: CGavdpHelper(aGavdp, aUser)
	{
	}


void CGavdpIndicator::RunL()
	{
	// this service is not-one shot, so don't call DetachFromGavdp
	User::LeaveIfError(iStatus.Int());
		
	// reinterpret our now complete descriptor as a TAvdtpIndicationPckg
	// then raise appropriate indication
	
	const TUint8* ptr = iIndication.Ptr();
	const TInternalAvdtpIndication& indtype = *reinterpret_cast<const TInternalAvdtpIndication*>(ptr);

	if (indtype.iType == TInternalAvdtpIndication::EError)
		{
		const TInternalAvdtpErrorIndication& error = reinterpret_cast<const TInternalAvdtpErrorIndication&>(indtype);
		// if avdtp errors, so does Gavdp
		Gavdp().Error(error.iError); // GAVDP tells the user if needed: ESOCK DEF - that may delete GAVDP
		}
		
	else if (indtype.iType == TInternalAvdtpIndication::EObject)
		{
		const TInternalAvdtpObjectIndication& ind = *reinterpret_cast<const TInternalAvdtpObjectIndication*>(ptr);
		switch (ind.iObjectIndication)
			{
			case TInternalAvdtpObjectIndication::ESignallingReady:
			TBTDevAddr addr(ind.iBuf);
			Gavdp().BindSignallingL(addr);			
			break;
			
			}
		}
	
	else if (indtype.iType == TInternalAvdtpIndication::EAir)
		{		
		TInt res = SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(EAvdtpBadState);
		const TInternalAvdtpAirIndication& ind = *reinterpret_cast<const TInternalAvdtpAirIndication*>(ptr);

		switch (ind.iIndication)
			{
			// only ones the stack doesn't deal with completely
			case EAvdtpSetConfiguration:
			case EAvdtpReconfigure:
				{
				// app must have a say in determining if the configuration is OK
				// the remote has selected the sep it is interested in
				// and checked the caps the apps has already set
				// but stack would find it "hard" to semantically compare optional caps etc
				// need to get the payload from the stack
				const TInternalAvdtpConfigurationIndication& cfgInd=static_cast<const TInternalAvdtpConfigurationIndication&>(ind);
				HBufC8* resp = HBufC8::NewMaxLC(Max(cfgInd.iPayloadLength, sizeof(TInternalGetProposedConfiguration)));
				TPtr8 des = resp->Des();
				
				TInternalGetProposedConfiguration& getConfBuf = *const_cast<TInternalGetProposedConfiguration*>(reinterpret_cast<const TInternalGetProposedConfiguration*>(resp->Des().Ptr()));
				getConfBuf.iSEID = ind.iSEID;
								
				// get remainder of configuration from AVDTP
				TInt err = AvdtpRequester().GetOpt(EGetProposedConfiguration,
													KSolBtAVDTPSignalling,
													des);

				__ASSERT_DEBUG(err==KErrNone, Fault(EGavdpUnexpectedAvdtpError));
				
				TAvdtpServiceCategories cats;
				cats.SetCapability(EAllServiceCategories);
				
				CCapabilityParseVisitor* parser = new (ELeave) CCapabilityParseVisitor(cats);
				parser->Process(des);
				TCapabilitiesArray caps = parser->GetCapabilities();
				delete parser;
				
				// tell app they're abuot to be offered configuration details
				
				User().GAVDP_ConfigurationStartIndication(cfgInd.iSEID, cfgInd.iINTSEID);
				
				for (TInt i=0; i<ENumberOfServiceCategories; i++)
					{
					// go through each capability...
					if (EServiceCategoryMultiplexing==i)
						{
						// don't offer this one to user, it's not for them
						// as it's part of AVDTP adaptation services...
						continue;
						}
					TAvdtpServiceCapability*& cap = caps[i];
					if (cap)
						{
						// and tell client
						res = User().GAVDP_ConfigurationIndication(cap);
						CheckClientResult(res);
						if (res!=KErrNone)
							{
							TAvdtpServiceCategory cat = cap->Category();
							
							Gavdp().SendResponse(cfgInd, res, &cat);
							// protocol says we finish config when anything is rejected
							break;
							}
						else
							{
							// passed ownership for the error-free case
							cap = NULL;
							}
						}
					}
				if (res==KErrNone)
					{				
					// tell app there's no more
					res = User().GAVDP_ConfigurationEndIndication();
					CheckClientResult(res);
					// As we don't have a particular category to refer to we pass in
					// the null category (0x0) in case GAVDP_ConfigurationEndIndication 
					// returned an error. (Bluetooth Errata ID 332)
					TAvdtpServiceCategory nullcat(EServiceCategoryNull);
					// whatever user has said is now their final answer
					Gavdp().SendResponse(cfgInd, res, &nullcat);
					}

				CleanupStack::PopAndDestroy(resp);
				caps.DeleteAll();
				break;
				}
			
			case EAvdtpOpen:
				{
				 // need to process to stop deadlock if the other end doesnt do Start as INT
				 // which it might do annoyingly if it wrongly relies on AVRCP
				// we can just connect up sockets
				const TInternalAvdtpStreamReadyIndication& r = reinterpret_cast<const TInternalAvdtpStreamReadyIndication&>(ind);
			
				Gavdp().BindBearersL(r.iRemoteSEID, r.iReportingPresent, r.iRecoveryPresent);
				// no response to send
				break;
				}
				
			case EAvdtpRelease:
				{
				// just tell the user the stream has gone, AVDTP has already replied
				User().GAVDP_ReleaseIndication(ind.iSEID);
				break;
				}
		
			case EAvdtpStart:
				{
				// although the packet may contain muliple SEIDs by this point
				// each indication will have one as the ordering of the SEIDs
				// in the packet relative to GAVDP user / session ordering is
				// not specified.
				res = User().GAVDP_StartIndication(ind.iSEID);
				CheckClientResult(res);
				Gavdp().SendResponse(ind, res);
				break;
				}
			case EAvdtpSuspend:
				{
				// although the packet may contain muliple SEIDs by this point
				// each indication will have one as the ordering of the SEIDs
				// in the packet relative to GAVDP user / session ordering is
				// not specified.
				
				res = User().GAVDP_SuspendIndication(ind.iSEID);
				CheckClientResult(res);
				Gavdp().SendResponse(ind, res);
				break;
				}
			case EAvdtpSecurityControl:
				{
				TInt length = ind.iPayloadLength;
				if (length < sizeof(TInternalGetSecurityControl))
					{
					length = sizeof(TInternalGetSecurityControl);
					}
				
				HBufC8* resp = HBufC8::NewMaxLC(length);
				TPtr8 securityData = resp->Des();
				
				TInternalGetSecurityControl& getSecBuf = *const_cast<TInternalGetSecurityControl*>(reinterpret_cast<const TInternalGetSecurityControl*>(resp->Des().Ptr()));
				getSecBuf.iSEID = ind.iSEID;
								
				// get remainder of securitycontrol from AVDTP
				TInt err = AvdtpRequester().GetOpt(EGetSecurityControl,
													KSolBtAVDTPSignalling,
													securityData);

				__ASSERT_DEBUG(err==KErrNone, Fault(EGavdpUnexpectedAvdtpError));


				res = User().GAVDP_SecurityControlIndication(ind.iSEID, securityData);
				CheckClientResult(res);
				// the client is allowed to change the buffer contents to supply data in response
				Gavdp().SendResponse(ind, res, securityData.Ptr(), securityData.Length());
				CleanupStack::PopAndDestroy(resp);
				break;
				}
			case EAvdtpAbort:
				{
				// no return from client - there's nothing they can do
				User().GAVDP_AbortIndication(ind.iSEID);
				// stack already responded
				break;
				}
			default:
				{
				}
			};
		}
	
	// This service is never complete, reissue ioctl.  We do this once we have finished using
	// the iIndication data as once the ioctl has been issued the data may be overwritten.
	// If AVDTP has an indication to pass to us and an ioctl has not been issued the
	// indication will be queued within AVDTP until we have issued the ioctl again.
	Begin();
	}

/**
RunError overriden for CGavdpIndicator only. This GAVDP helper is used in its own right by GAVDP.
It is not treated as a 'general' GAVDP helper, and, unlike a 'general' GAVDP helper,
should live for the duration of GAVDP.
Note, however that hitting this RunError is deemed fatal, and so we kill GAVDP completely. The 
user will have to re-open it to recover.
*/
TInt CGavdpIndicator::RunError(TInt aError)
	{
	RDebug::Print(_L("GAVDP::Fatal Error %d"), aError);
	Gavdp().FatalError();
	
	//*****No code below here please - call to CGAVDP::FatalError will delete us****
	return KErrNone;
	}	

void CGavdpIndicator::CheckClientResult(TInt aResult)
	{
	__ASSERT_ALWAYS(aResult<=KErrNone, Panic(EGavdpClientHasRepliedInCorrectly));
	}
	
/**
@internalComponent
Start the Indicator helper
*/
void CGavdpIndicator::Begin()
	{
	AvdtpIndicator().Ioctl(EAwaitIndication, iStatus, &iIndication, KSolBtAVDTPSignalling);
	SetActive();	
	}


/**
@internalComponent
Create a User-plane bearer connector helper
*/
CGavdpUPlaneConnector* CGavdpUPlaneConnector::NewL(CGavdp& aGavdp,
												   MGavdpUser& aUser,
												   const TAvdtpSockAddr& aAddr,
											 	   TBool aRequireReporting,
											 	   TBool aRequireRecovery,
											 	   RSocketServ& aClientSession)
	{
	CGavdpUPlaneConnector* s = new (ELeave) CGavdpUPlaneConnector(aGavdp, aUser, aAddr,
																	aRequireReporting,
																	aRequireRecovery,
																	aClientSession);
	CleanupStack::PushL(s);
	s->ConstructL(aRequireReporting, aRequireRecovery);
	CleanupStack::Pop(s);																	
	return s;
	}
	
CGavdpUPlaneConnector::CGavdpUPlaneConnector(CGavdp& aGavdp, MGavdpUser& aUser,
											 const TAvdtpSockAddr& aSockAddr,
											 TBool aRequireReporting,
											 TBool aRequireRecovery,
						  					 RSocketServ& aClientSession)
: CGavdpHelper(aGavdp, aUser),
  iClientSession(aClientSession)
	{
	iAddress = aSockAddr;
	iNumBearersRequired = AvdtpInternalUtils::NumberOfTransportObjects(aRequireReporting, aRequireRecovery);
	}

void CGavdpUPlaneConnector::ConstructL(TBool aRequireReporting, TBool aRequireRecovery)
	{
	TAvdtpSockAddr addr = iAddress;
	addr.SetSession(EMedia);
	iSocketConnectors[0]=CSocketConnector::NewL(*this, AvdtpSession(), addr);

	if (aRequireReporting)
		{
		addr.SetSession(EReporting);
		iSocketConnectors[1]=CSocketConnector::NewL(*this, AvdtpSession(), addr);
		}

	if (aRequireRecovery)
		{
		addr.SetSession(ERecovery);
		iSocketConnectors[2]=CSocketConnector::NewL(*this, AvdtpSession(), addr);
		}
	}
	
CGavdpUPlaneConnector::~CGavdpUPlaneConnector()
	{
	Cancel();
	iSocketConnectors.DeleteAll();
	}

void CGavdpUPlaneConnector::DoCancel()
	{
	for (TInt i=0; i<iSocketConnectors.Count()-1; i++)
		{
		if (iSocketConnectors[i])
			{
			iSocketConnectors[i]->Cancel();
			}
		}
	}

void CGavdpUPlaneConnector::Begin()
	{
	// Ensure that once this is completed the RunL is scheduled
	// before handling completion of the indication ioctl.
	SetPriority(EPriorityStandard+1);
	for (TInt i=0; i<iSocketConnectors.Count()-1; i++)
		{
		if (iSocketConnectors[i])
			{
			iSocketConnectors[i]->Begin();
			}
		}
	}

void CGavdpUPlaneConnector::ConnectComplete(TInt aErr)
	{
	DoConnectComplete(aErr);

	if (aErr)
		{
		DoCancel();	// now not active, but we need to cancel the remaining socketconnectors
		}
	}
	
void CGavdpUPlaneConnector::DoConnectComplete(TInt aErr)
	{
	--iNumBearersRequired;
	
	if(aErr >= EGavdpMinPanicErr && aErr <= EGavdpMaxPanicErr)
		{
		Panic (static_cast<TGavdpPanic>(aErr));
		}
	
	if (!iNumBearersRequired || aErr!=KErrNone)
		{
		// service is complete
		TRequestStatus* ours = &iStatus;
		*ours = KRequestPending;
		SetActive();
		User::RequestComplete(ours, aErr);
		}
	}
	
void CGavdpUPlaneConnector::RunL()
	{
	CheckFailedL();

	// all required bearers connected, helper can detach from gavdp
	DetachFromGavdp(KErrNone);
	
	AllBearersReadyL();

	delete this;
	}
	
void CGavdpUPlaneConnector::AllBearersReadyL()
	{
	// transfer our sockets onto the client session
	for (TInt i=0; i<=iSocketConnectors.Count()-1; i++)
		{
		CSocketConnector* const connector = iSocketConnectors[i];
		if (connector)
			{
			RSocket& socket = connector->Socket();
			// take socket and transfer to client session
			TName socketName;
			User::LeaveIfError(socket.Name(socketName));

			RSocket clientSocket;

			//plat sect stuff to transfer sockets - to this same process
			TSecurityPolicy secPol(RProcess().SecureId());
			TPckgBuf<TSecurityPolicy> secPolBuf(secPol);
			User::LeaveIfError(socket.SetOpt(KSOEnableTransfer, KSOLSocket, secPolBuf));
			User::LeaveIfError(clientSocket.Transfer(iClientSession, socketName));
			
			User().GAVDP_BearerReady(clientSocket, connector->Address());
			}
		}
	}
	

CSocketConnector* CSocketConnector::NewL(CGavdpUPlaneConnector& aParent,
									RSocketServ& aSocketServer,
									const TAvdtpSockAddr& aSockAddr)

	{
	CSocketConnector* self = new(ELeave) CSocketConnector(aParent, aSocketServer, aSockAddr);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CSocketConnector::ConstructL()
	{
	User::LeaveIfError(iSocket.Open(iSocketServer, KAVDTPProtocolName));
	}
	
void CSocketConnector::Begin()
	{
	iSocket.Connect(iSockAddr, iStatus);
	SetActive();
	}
	
CSocketConnector::CSocketConnector(CGavdpUPlaneConnector& aParent,
									RSocketServ& aSocketServer,
									const TAvdtpSockAddr& aSockAddr)
: CActive(EPriorityStandard+1), iParent(aParent),
iSocketServer(aSocketServer), iSockAddr(aSockAddr)
	{
	CActiveScheduler::Add(this);
	}

CSocketConnector::~CSocketConnector()
	{
	Cancel();

	if (iSocket.SubSessionHandle())
		{
		iSocket.Close();
		}
	}
	
void CSocketConnector::RunL()
	{
	iParent.ConnectComplete(iStatus.Int());
	}
	
void CSocketConnector::DoCancel()
	{
	if (iSocket.SubSessionHandle())
		{
		iSocket.CancelAll();
		}
	}
	

CGavdpSecurityControl* CGavdpSecurityControl::NewL(CGavdp& aGavdp,
					  		  MGavdpUser& aUser,
					  		  TSEID aSeid,
					  		  const TDesC8& aSecurityControl)
	{
	return new (ELeave) CGavdpSecurityControl(aGavdp, aUser, aSeid, aSecurityControl);
	}
	
void CGavdpSecurityControl::Begin()
	{
	AvdtpRequester().Ioctl(ESendSecurityControl, iStatus, &iSecurityControlPckgBuf, KSolBtAVDTPSignalling);
	SetActive();		
	}
	
CGavdpSecurityControl::~CGavdpSecurityControl()
	{
	Cancel();
	}
	
CGavdpSecurityControl::CGavdpSecurityControl(CGavdp& aGavdp, MGavdpUser& aUser, TSEID aSeid, const TDesC8& aSecurityData)
: CGavdpHelper(aGavdp, aUser)
	{
	iSecurityControlPckgBuf().iRemoteSEID = aSeid;
	iSecurityControlPckgBuf().iSecurityControlInfo = aSecurityData;
	}
	
void CGavdpSecurityControl::RunL()
	{
	CheckFailedL();
	DetachFromGavdp(KErrNone);	

	User().GAVDP_SecurityControlConfirm(iSecurityControlPckgBuf().iSecurityControlInfo);

	delete this;
	}
	
void CGavdpSecurityControl::DoCancel()
	{
	AvdtpRequester().CancelAll();
	}
