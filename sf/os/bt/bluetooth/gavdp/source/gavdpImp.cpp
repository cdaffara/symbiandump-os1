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
// Implementation of Generic Audio-Video Distribution Profile - GAVDP
// The CGavdp class acts as a "Body" implementation to the RGavdp "Handle"
// 
//

/**
 @publishedPartner
*/


#include "gavdpInternal.h"

CGavdp* CGavdp::NewL(MGavdpUser& aServiceUser, RSocketServ& aSocketServer, RGavdp& aHandle)
	{
#ifndef BLUETOOTH_NO_AV
	CGavdp* gavdp = new(ELeave) CGavdp(aServiceUser, aSocketServer, aHandle);
	CleanupStack::PushL(gavdp);
	gavdp->ConstructL();
	CleanupStack::Pop(gavdp);
	return gavdp;
#else
	User::Leave(KErrNotSupported);
	return NULL; // not reached
#endif
	}
	
CGavdp::~CGavdp()
	{
	DeAllocSocketsAndHelpers();
	iSocketServer.Close();
	}
	
void CGavdp::DeAllocSocketsAndHelpers()
	{
	delete iRequesterHelper;
	iRequesterHelper = NULL;
	delete iIndicatorHelper;
	iIndicatorHelper = NULL;
	AvdtpRequester().Close();
	AvdtpIndicator().Close();
	}
	
void CGavdp::ConstructL()
	{
	User::LeaveIfError(iSocketServer.Connect());
	ConstructSocketsAndHelpersL();
	}
	
void CGavdp::Cancel()
	{
	if (iRequesterHelper)
		{
		delete iRequesterHelper;
		iRequesterHelper = NULL;
		iState = EIdle;
		// it is the client's job to recover state, so they should send an Abort.
		}
	// leave indications running.
	}

void CGavdp::ConstructSocketsAndHelpersL()
	{
	User::LeaveIfError(AvdtpRequester().Open(iSocketServer, KAVDTPProtocolName));

	// mark as for signalling purposes
	TAvdtpSockAddr addr;
	addr.SetSession(ESignalling);
	User::LeaveIfError(AvdtpRequester().Bind(addr));

	// start reading for AVDTP indications
	User::LeaveIfError(AvdtpIndicator().Open(iSocketServer, KAVDTPProtocolName));

	User::LeaveIfError(AvdtpIndicator().SetOpt(ESetAsSecondarySAP,
								  KSolBtAVDTPInternal,
								  KNullDesC8));
								  
	User::LeaveIfError(AvdtpRequester().SetOpt(EBindToSecondarySAP,
									  KSolBtAVDTPInternal,
									  KNullDesC8));
	iIndicatorHelper = CGavdpIndicator::NewL(*this, iServiceUser);
	iIndicatorHelper->Begin();
	}
	
CGavdp::CGavdp(MGavdpUser& aServiceUser, RSocketServ& aClientSocketServer, RGavdp& aHandle)
: iClientSocketServer(aClientSocketServer), iServiceUser(aServiceUser), iHandle(aHandle)
	{
	}
	
void CGavdp::Connect(const TBTDevAddr& aRemoteAddr)
	{
	__ASSERT_ALWAYS((iState == EIdle || iState == EListening), Panic(EGavdpBadState));
	__ASSERT_ALWAYS(iNumSEPsRegistered, Panic(EGavdpSEPMustBeRegisteredBeforeConnect));
	__ASSERT_ALWAYS(aRemoteAddr!=TBTDevAddr(0), Panic(EGavdpBadRemoteAddress));
	__ASSERT_ALWAYS(!iRequesterHelper, Panic(EGavdpBadState));
	
	AvdtpRequester().SetOpt(EStopAwaitPassiveSignallingChannel, KSolBtAVDTPSignalling, NULL);
		
	TRAPD(err, iRequesterHelper = CGavdpConnector::NewL(*this, iServiceUser, aRemoteAddr));
	
	if (err)
		{
		iServiceUser.GAVDP_Error(err, KNullDesC8);
		}
	else
		{
		iState = EConnecting;
		iRequesterHelper->Begin();	
		}	
	}

void CGavdp::DiscoverRemoteSEPs()
	{
	__ASSERT_ALWAYS(!iRequesterHelper, Panic(EGavdpBadState));
	
	TRAPD(err, iRequesterHelper = CGavdpDiscover::NewL(*this, iServiceUser));
	
	if (err)
		{
		iServiceUser.GAVDP_Error(err, KNullDesC8);
		}
	else
		{
		iRequesterHelper->Begin();	
		}
	}

void CGavdp::GetRemoteSEPCapabilities(TSEID aSEID, const TAvdtpServiceCategories& aInterestingCategories)
	{
	__ASSERT_ALWAYS(!iRequesterHelper /*&& iServiceUser*/, Panic(EGavdpBadState));
	CheckSEID(aSEID);
	
	TRAPD(err, iRequesterHelper = CGavdpGetCaps::NewL(*this,
											 iServiceUser,
											 aSEID,
											 aInterestingCategories));
	
	if (err)
		{
		iServiceUser.GAVDP_Error(err, KNullDesC8);
		}
	else
		{
		iRequesterHelper->Begin();	
		}
	}
	
TInt CGavdp::BeginConfiguringRemoteSEP(TSEID aRemoteSEID, TSEID aLocalSEID)
	{
	__ASSERT_ALWAYS(iState!=EConfiguringRemoteSEP, Panic(EGavdpBadState));
	CheckSEID(aRemoteSEID);
	CheckSEID(aLocalSEID);

	TInternalSelectRemoteSEP select;
	select.iLocalSEID = aLocalSEID;
	select.iRemoteSEID = aRemoteSEID;
	
	TPckg<TInternalSelectRemoteSEP> seidpckg(select);
	TInt err = AvdtpRequester().SetOpt(EStartConfiguringRemote, KSolBtAVDTPSignalling, seidpckg);
	if (err==KErrNone)
		{
		iState = EConfiguringRemoteSEP;
		}
	return err;
	}
	
TInt CGavdp::BeginConfiguringLocalSEP(TSEID aSEID)
	{
	__ASSERT_ALWAYS(iState!=EConfiguringLocalSEP, Panic(EGavdpBadState));
	TPckg<TSEID> seidpckg(aSEID);
	TInt err = AvdtpRequester().SetOpt(EStartConfiguringLocal, KSolBtAVDTPSignalling, seidpckg);
	if (err==KErrNone)
		{
		iState = EConfiguringLocalSEP;
		}
	return err;
	}
	
	
TInt CGavdp::AddSEPCapability(const TAvdtpServiceCapability& aCapability)
	{
	__ASSERT_ALWAYS(iState==EConfiguringLocalSEP || iState==EConfiguringRemoteSEP,
																Panic(EGavdpBadState));

	// convert into protocol form - this is a handy way to get a descriptor for IPC purposes
	// if AVDTP requires the reverse operation it is of course allowed to get back to the T class form
	RBuf8 buf;
	TInt err = aCapability.AsProtocol(buf);
	if (err==KErrNone)
		{
		err = AvdtpRequester().SetOpt(EAddCapabilitySelection,KSolBtAVDTPSignalling, buf);
		}
		
	buf.Close();
	return err; 
	}

void CGavdp::CommitSEPConfiguration()
	{
	__ASSERT_ALWAYS(iState == EConfiguringLocalSEP || iState == EConfiguringRemoteSEP, Panic(EGavdpBadState));
	__ASSERT_ALWAYS(!iRequesterHelper, Panic(EGavdpBadState));
	
	TRAPD(err, iRequesterHelper = CGavdpSelectSEP::NewL(*this, iServiceUser));
	
	if (err)
		{
		iServiceUser.GAVDP_Error(err, KNullDesC8);
		}
	else
		{
		iRequesterHelper->Begin();
		}
	}

void CGavdp::StartStream(TSEID aSEID)
	{
	__ASSERT_ALWAYS(!iRequesterHelper, Panic(EGavdpBadState));
	
	TRAPD(err, iRequesterHelper = CGavdpStart::NewL(*this, iServiceUser, aSEID));
	
	if (err)
		{
		iServiceUser.GAVDP_Error(err, KNullDesC8);
		}
	else
		{
		iRequesterHelper->Begin();	
		}	
	}

void CGavdp::SuspendStream(TSEID aSEID)
	{
	__ASSERT_ALWAYS(!iRequesterHelper, Panic(EGavdpBadState));
	
	TRAPD(err, iRequesterHelper = CGavdpSuspend::NewL(*this, iServiceUser, aSEID));
	
	if (err)
		{
		iServiceUser.GAVDP_Error(err, KNullDesC8);
		}
	else
		{
		iRequesterHelper->Begin();	
		}	
	}

void CGavdp::AbortStream(TSEID aSEID)	
	{
	__ASSERT_ALWAYS(!iRequesterHelper, Panic(EGavdpBadState));
	CheckSEID(aSEID);
	
	TRAPD(err, iRequesterHelper = CGavdpAbort::NewL(*this, iServiceUser, aSEID));
	
	if (err)
		{
		iServiceUser.GAVDP_Error(err, KNullDesC8);
		}
	else
		{
		iRequesterHelper->Begin();	
		}
	}

void CGavdp::SendSecurityControl(TSEID aSEID, const TDesC8& aSecurityData)	
	{
	__ASSERT_ALWAYS(!iRequesterHelper, Panic(EGavdpBadState));
	CheckSEID(aSEID);
	
	TRAPD(err, iRequesterHelper = CGavdpSecurityControl::NewL(*this, iServiceUser, aSEID, aSecurityData));
	
	if (err)
		{
		iServiceUser.GAVDP_Error(err, KNullDesC8);
		}
	else
		{
		iRequesterHelper->Begin();	
		}
	}
	

TInt CGavdp::RegisterSEP(TAvdtpSEPInfo& aInfo)
	{
	// note this is a GetOpt - but it also sets:
	// in goes the info, without SEID
	// out comes the same structure with SEID filled in
	TPckg<TAvdtpSEPInfo> sepInfo(aInfo);
	TInt err = AvdtpRequester().GetOpt(ERegisterSEP, KSolBtAVDTPSignalling, sepInfo);
	if (err==KErrNone)
		{
		iNumSEPsRegistered++;
		}
	return err;
	}


/**
Called when gavdp client has registed all seps and is ready
May be short lived as they may try to connect out quickly
*/
TInt CGavdp::Listen()
	{
	__ASSERT_ALWAYS(iNumSEPsRegistered, Panic(EGavdpSEPMustBeRegisteredBeforeListen));
	TInt ret = KErrNone;
	
	if (iState!=EConnected)
		{
		// start awaiting for signalling channel as as SEP has been successfully registered
		ret = AvdtpRequester().SetOpt(EAwaitPassiveSignallingChannel, KSolBtAVDTPSignalling, NULL);
		if (KErrNone==ret)
			{
			iState = EListening;
			}
		}
	else
		{
		ret = KErrInUse;
		}
	return ret;
	}
	

// private

void CGavdp::SendResponse( TInternalAvdtpAirIndication aIndication,
						  TInt aResult,
						  const TAny* aResponseData/*=NULL*/,
						  TInt aResponseDataLen/*=0*/)
	{
	TInt err = KErrNone;
	
	switch (aIndication.iIndication)
		{
		case EAvdtpSetConfiguration:
		case EAvdtpReconfigure:
			{
			TAvdtpInternalConfigurationResponse rsp;
			rsp.iSEID = aIndication.iSEID;
			rsp.iTransactionLabel = aIndication.iTransactionLabel;
			rsp.iResult = aResult;
			rsp.iIsReconfigure = aIndication.iIndication==EAvdtpReconfigure;
			
			if (aResult!=KErrNone)
				{
				__ASSERT_DEBUG(aResponseData, Panic(EGavdpResponseDataExpected));
				rsp.iCategory = *static_cast<const TAvdtpServiceCategory*>(aResponseData);
				}
	
			TPckg<TAvdtpInternalConfigurationResponse> pckg(rsp);
			
			err = AvdtpRequester().SetOpt(ESendConfigurationResponse, KSolBtAVDTPSignalling, pckg);
			
			break;
			}
		case EAvdtpSecurityControl:
			{
			// optional to send data in response
			TAvdtpInternalSecurityControlResponse rsp;
			rsp.iSEID = aIndication.iSEID;
			rsp.iTransactionLabel = aIndication.iTransactionLabel;
			rsp.iResult = aResult;

			if (aResult==KErrNone)
				{
				rsp.iSecurityControlInfo.Copy(static_cast<const TUint8*>(aResponseData), aResponseDataLen);
				}
	
			TPckg<TAvdtpInternalSecurityControlResponse> pckg(rsp);			
			err = AvdtpRequester().SetOpt(ESendSecurityControlResponse, KSolBtAVDTPSignalling, pckg);
			
			break;
			}
		case EAvdtpStart:
			{
			TAvdtpInternalStartResponse rsp;
			rsp.iSEID = aIndication.iSEID;
			rsp.iTransactionLabel = aIndication.iTransactionLabel;
			rsp.iResult = aResult;

			TPckg<TAvdtpInternalStartResponse> pckg(rsp);
			err = AvdtpRequester().SetOpt(ESendStartResponse, KSolBtAVDTPSignalling, pckg);		
			
			break;
			}
		case EAvdtpSuspend:
			{
			TAvdtpInternalSuspendResponse rsp;
			rsp.iSEID = aIndication.iSEID;
			rsp.iTransactionLabel = aIndication.iTransactionLabel;
			rsp.iResult = aResult;

			TPckg<TAvdtpInternalSuspendResponse> pckg(rsp);
			err = AvdtpRequester().SetOpt(ESendSuspendResponse, KSolBtAVDTPSignalling, pckg);
			
			break;
			}
		}	

	if (err!=KErrNone)
		{
		iServiceUser.GAVDP_Error(err, KNullDesC8);
		}
	}
		
	
void CGavdp::ServiceComplete(TInt aResult)
	{
	__ASSERT_DEBUG(iRequesterHelper, Fault(EGavdpHelperCompletingBadly));
	iRequesterHelper = NULL;
	
	if (aResult==KErrNone)
		{
		switch (iState)
			{
			case EConfiguringRemoteSEP:
			case EListening:
			case EConnecting:
				iState = EConnected;
				break;
			case EConfiguringLocalSEP:
				{
				iState = EIdle;
				break;
				}
				
			}
		}
	else
		{
		iState = EIdle; // or perhaps errored? 
		}
	}
	
void CGavdp::Error(TInt aError)
	{
	//here we do whatever we need to tidy ourselves
	delete iRequesterHelper;
	iRequesterHelper = NULL;
	
	if (iState == EListening)
		{
		//if we're in the listening state and we get a KErrDisconnected error, this is
		//because a connection completed and was disconnected straight away before we got
		//the chance to pickup the connection indicator. We need to re-listen and throw
		//away this error. Let's replace the error with the result of the re-listen.
		if (aError == KErrDisconnected)
			{
			aError = Listen();
			}
		
		//when listening, it doesn't make sense to pass this error to the user
		if (aError == KErrCouldNotConnect)
			{
			aError = KErrNone;
			}
		}
	
	//the error might have been replaced due to the condition mentioned above so
	//this needs to be checked.
	if (aError != KErrNone)
		{
		iServiceUser.GAVDP_Error(aError, KNullDesC8);
		
		//update the state because an error did occur
		iState = EIdle;
		}
	}

void CGavdp::FatalError()
	{
	//Unbind from the body before informing the user
	iHandle.UnbindBody();
	//Now tell the user we are DEAD
	iServiceUser.GAVDP_Error(KErrDied, KNullDesC8);
	//Cleanup: we've unbound from the body, We are no longer owned. Delete ourselves
	delete this;
	}

/*
Forms the binding between passively created signalling and sockets
*/
void CGavdp::BindSignallingL(const TBTDevAddr& aRemoteAddress)
	{
	// use the same helper as if we originated the signalling channel
	if (iRequesterHelper)
		{
		// the client has just issued another command
		// this is likely to be something that renders the connections invalid anyway
		// eg Abort, Config
		User::Leave(KErrInUse);
		}
	
	//create a passive connector helper
	iRequesterHelper = CGavdpConnector::NewL(*this, iServiceUser, aRemoteAddress, ETrue);
	iRequesterHelper->Begin();
	}

/*
Forms the binding between passively created transport sessions and sockets
*/
void CGavdp::BindBearersL(TSEID aSEID, TBool aRequireReporting, TBool aRequireRecovery)
	{
	// which bearers? spy on set config to see if rec&rep in use
	// or from the indication?
	//certainly media
	CheckSEID(aSEID);
	
	TAvdtpSockAddr addr;
	AvdtpRequester().RemoteName(addr);	
	addr.SetSEID(aSEID);
	
	if (iRequesterHelper)
		{
		// the client has just issued another command
		// this is likely to be something that renders the connections invalid anyway
		// eg Abort, Config
		User::Leave(KErrInUse);
		}
	
	iRequesterHelper = CGavdpUPlaneConnector::NewL(*this,
										  iServiceUser,
										  addr,
										  aRequireReporting,
										  aRequireRecovery,
										  iClientSocketServer
										  );
	iRequesterHelper->Begin();
	}

TInt CGavdp::CreateBearerSockets(TSEID aSEID)
	{	
	// form socket address and connect
	// should be in state listening. if we are told to create bearers then of course stop listening
	CheckSEID(aSEID);
	
	TAvdtpSockAddr addr;
	
	// get remote BDAddr for this session
	AvdtpRequester().RemoteName(addr);
	
	// now set SEID and Session type
	addr.SetSEID(aSEID);
	
	// ask avdtp if reporting and recovery are configured
	TReportingAndRecovery rar;
	rar.iSEID = aSEID;
	TPckg<TReportingAndRecovery> rarBuf(rar);
	TInt err = AvdtpRequester().GetOpt(EGetReportingAndRecoveryConfig, KSolBtAVDTPSignalling, rarBuf);
	if(err == KErrNone)
		{
		// helper sets session types	
		TRAP(err, iRequesterHelper = CGavdpUPlaneConnector::NewL(*this,
																iServiceUser,
																addr,
																rar.iReporting,
																rar.iRecovery,
																iClientSocketServer));
		if (err)
			{
			iServiceUser.GAVDP_Error(err, KNullDesC8);
			}
		else
			{
			iRequesterHelper->Begin();	
			}
		}
	else
		{
		iServiceUser.GAVDP_Error(err, KNullDesC8);
		}
	return err;	
	}


inline TBool CGavdp::CheckSEID(TSEID aSEID)
	{
	TBool ok = aSEID.IsValid();
	__ASSERT_ALWAYS(ok, Panic(EGavdpUserSuppliedBadSEID));
	return ok;
	}
	

TInt CGavdp::Shutdown()
	{
	Cancel();
	iState=EIdle;
	return AvdtpRequester().SetOpt(EShutdown, KSolBtAVDTPSignalling, NULL);
	}
