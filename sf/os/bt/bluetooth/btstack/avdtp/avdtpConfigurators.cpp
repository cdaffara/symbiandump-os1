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
// Implements the avdtp configurators
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/logger.h>
#include "avdtpConfigurators.h"
#include "avdtpSignallingSession.h"
#include "avdtpSignallingChannel.h"
#include "avdtp.h"
#include "avdtputil.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVDTP);
#endif

CSEPConfigurator::CSEPConfigurator(CSignallingSession& aSignallingSession,
						 		   CAvdtpProtocol& aProtocol)
	: iSignallingSession(aSignallingSession), iProtocol(aProtocol)
	{
	LOG_FUNC
	}




CRemoteSEPConfigurator* CRemoteSEPConfigurator::NewL(CSignallingSession& aSignallingSession,
													 CAvdtpProtocol& aProtocol,
													 TSEID aLocalSEID,
													 TSEID aRemoteSEID,
													 TBool aIsReconfigure)
	{
	LOG_STATIC_FUNC
	CRemoteSEPConfigurator* s = new (ELeave) CRemoteSEPConfigurator(aSignallingSession,
																	aProtocol, aIsReconfigure);
	CleanupStack::PushL(s);
	s->ConstructL(aLocalSEID, aRemoteSEID);
	CleanupStack::Pop(s);
	return s;	
	}
	
	
void CRemoteSEPConfigurator::ConstructL(TSEID aLocalSEID, TSEID aRemoteSEID)
	{
	LOG_FUNC
	if (aLocalSEID.IsLocal() && !aRemoteSEID.IsLocal() && iProtocol.RemoteSEPCache().Exists(SignallingSession().RemoteAddress(), aRemoteSEID))
		{
		// check in cache...(i.e capabilities have been retrieved)
		// create the store for the configuration
		iSetConfigBuffer.CreateL(KAvdtpMinSetConfigLength);
		iLocalSEP = &iSignallingSession.FindLocalSEPL(aLocalSEID);
		iRemoteSEID = aRemoteSEID;	
		}
	else
		{
		User::Leave(KErrArgument);
		}
	}

CRemoteSEPConfigurator::CRemoteSEPConfigurator(CSignallingSession& aSignallingSession,
						 		  			   CAvdtpProtocol& aProtocol,
												TBool aIsReconfigure)
	: CSEPConfigurator(aSignallingSession, aProtocol), iIsReconfigure(aIsReconfigure)
	{
	LOG_FUNC
	}

CRemoteSEPConfigurator::~CRemoteSEPConfigurator()
	{
	LOG_FUNC
	iSetConfigBuffer.Close();
	}


TInt CRemoteSEPConfigurator::AddCapability(const TDesC8& aCapability)
/*
	Add in caps to SetConf - one per SetOpt
	When Ioctl comes in we have a complete packet to send to sigch
*/
	{
	LOG_FUNC
	TInt ret = KErrNone;
	
	// this has come in from GAVDP in "protocol" form (as an easy way to do IPC)
	// we sanity check this here before sending
	// run visitor over it
	CCapabilitySummaryVisitor* visitor = new CCapabilitySummaryVisitor();
	if (visitor)
		{
		TPtr8 ptr(const_cast<TUint8*>(aCapability.Ptr()), aCapability.Length(), aCapability.Length());

		visitor->Process(ptr);
		TAvdtpServiceCategories cats = visitor->CapabilitiesPresent();
		
		if (iIsReconfigure)
			{
			if (!(cats.CapabilityPresent(EServiceCategoryMediaCodec) || cats.CapabilityPresent(EServiceCategoryContentProtection)))
				{
				// check that if reconfiguring, the capability is reconfigurable (see 8.10.1, AVDTP)
				ret = KErrArgument;
				}
			}
		else
			{
			// check for interesting cases during non-reconfigure
			if (cats.CapabilityPresent(EServiceCategoryMultiplexing))
				{
				ret = KErrArgument; // only stack manage this capability
				}
			else if (cats.CapabilityPresent(EServiceCategoryReporting))
				{
				// this saves the stream having to reparse the set configuration buffer
				iReportingConfigured = ETrue;
				}
			else if (cats.CapabilityPresent(EServiceCategoryRecovery))
				{
				// this saves the stream having to reparse the set configuration buffer
				iRecoveryConfigured = ETrue;
				}
			}
		}
	else
		{
		ret = KErrNoMemory;
		}
		
	if (KErrNone==ret)
		{
		// ok to send, grow buffer to take this capability
		ret = iSetConfigBuffer.ReAlloc(iSetConfigBuffer.Length() + aCapability.Length());
		if (KErrNone==ret)
			{
			iSetConfigBuffer.Append(aCapability);
			}
		}
	delete visitor;

	return ret;
	}
	
	
TInt CRemoteSEPConfigurator::Finalise()
	{
	LOG_FUNC
	// the client has now sent all SetOpts for config
	// we need to finalise - ask stream to finalise out SetConfigBuffer
	TRAPD(res, SignallingSession().DoConfigureStreamL(iSetConfigBuffer,
														*iLocalSEP,
														iRemoteSEID,
														iReportingConfigured,
														iRecoveryConfigured));
		
	if (res!=KErrNone)
		{
		iSetConfigBuffer.Close();
		}
	else
		{
		// for remote seps we really do go async
		res = KRequestPending;
		// transferred ownership of iSetConfigBuffer, so don't close, just zero
		iSetConfigBuffer.Assign(NULL);
		}
	return res;
	}
	

CLocalSEPConfigurator* CLocalSEPConfigurator::NewL(CSignallingSession& aSignallingSession,
													 CAvdtpProtocol& aProtocol,
													 TSEID aSEID)
	{
	LOG_STATIC_FUNC
	CLocalSEPConfigurator* s = new (ELeave) CLocalSEPConfigurator(aSignallingSession,
																	aProtocol);
	CleanupStack::PushL(s);
	s->ConstructL(aSEID);
	CleanupStack::Pop(s);
	return s;	
	}

void CLocalSEPConfigurator::ConstructL(TSEID aSEID)
	{
	LOG_FUNC
	if (!aSEID.IsLocal())
		{
		User::Leave(KErrArgument);
		}
	// check in local sep queue (i.e. has been registered)
	iLocalSEP = &SignallingSession().FindLocalSEPL(aSEID);
	TAvdtpServiceCategories cats;
	cats.SetCapability(EAllServiceCategories);
	iCapabilityParseVisitor = new (ELeave) CCapabilityParseVisitor(cats);
	}


CLocalSEPConfigurator::CLocalSEPConfigurator(CSignallingSession& aSignallingSession,
						   CAvdtpProtocol& aProtocol)
	: CSEPConfigurator(aSignallingSession, aProtocol)
	{
	LOG_FUNC
	}
	
CLocalSEPConfigurator::~CLocalSEPConfigurator()
	{
	LOG_FUNC
	delete iCapabilityParseVisitor;
	}

TInt CLocalSEPConfigurator::AddCapability(const TDesC8& aCapabilityInProtocolForm)
/*
Add in caps to SetConf - one per SetOpt
When finalise Ioctl comes in we have a complete packet to send to sigch
*/
	{
	LOG_FUNC
	// convert into T-class form to santiy check this is goign to end up as a good configuration
	TPtr8 ptr(const_cast<TUint8*>(aCapabilityInProtocolForm.Ptr()), aCapabilityInProtocolForm.Length(), aCapabilityInProtocolForm.Length());
	iCapabilityParseVisitor->Process(ptr);

	return KErrNone;
	}
	
TInt CLocalSEPConfigurator::Finalise()
	{
	LOG_FUNC
	// get all the set capabilities
	TCapabilitiesArray caps = iCapabilityParseVisitor->GetCapabilities();
	
	 // muxing shouldnt be set yet
	__ASSERT_DEBUG(!caps[EServiceCategoryMultiplexing], Panic(EAvdtpConfiguratorsMultiplexingUnexpectedlySet));
	
	// at this point we put in support for muxing as all Symbian local SEPs to
#ifndef __SYMBIAN_AVDTP_HIDE_MUX 

	TAvdtpMultiplexingCapability* mux = new TAvdtpMultiplexingCapability;
	if (mux)
		{
		TAvdtpMultiplexingCapabilityHelper helper(*mux, !!caps[EServiceCategoryReporting], !!caps[EServiceCategoryRecovery]);
		// set all relevant transport ids to zero to request values from INT
		// see AVDTP spec p90
		// if we are an INT we'll update this capability later in statespace
		helper.SetMediaSID(0);
		helper.SetMediaCID(0);
		
		if (caps[EServiceCategoryReporting])
			{
			helper.SetReportingSID(0);
			helper.SetReportingCID(0);
			}
		// if recovery exposed then set to 0, else leave
		if (caps[EServiceCategoryRecovery])
			{
			helper.SetRecoverySID(0);
			helper.SetRecoveryCID(0);
			}

		caps[EServiceCategoryMultiplexing] = mux; // ownership transferred	
		}
#endif
	// else forgot about trying to do muxing - it was an optimisation
	
	for (TInt i=0; i<ENumberOfServiceCategories; i++)
		{
		if (caps[i])
			{
			iLocalSEP->AddCapability(caps[i]); // transfer ownership of element
			}
		}
		
	return KErrNone;
	}

