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
// Implements the avdtp local SEP
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/logger.h>
#include "avdtpLocalSEP.h"
#include "avdtpStream.h"
#include "avdtp.h"
#include "avdtputil.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVDTP);
#endif

/*
@param aSEPInfo is an in and out parameter
inbound there is no SEID set, but this class on construction writes back the assigned SEID
*/
CLocalSEP* CLocalSEP::NewL(TAvdtpSEPInfo& aSEPInfo, TSEIDManager& aSEIDManager)
	{
	LOG_STATIC_FUNC
	CLocalSEP* sep = new (ELeave) CLocalSEP;
	CleanupStack::PushL(sep);
	sep->ConstructL(aSEPInfo, aSEIDManager);
	CleanupStack::Pop(sep);
	return sep;
	}
	
CLocalSEP::CLocalSEP() : iPreConfigured(ETrue)
	{
	LOG_FUNC
	}
	
void CLocalSEP::ConstructL(TAvdtpSEPInfo& aSEPInfo, TSEIDManager& aSEIDManager)
	{
	LOG_FUNC
	
	// store client's settings of sink-ness, codec type etc
	iInfo = aSEPInfo;
	// assign RSEID to us, and return for client knowledge
	User::LeaveIfError(aSEIDManager.GetSEID(iLocalSEID));
	
	// store for remote-acquisition
	iInfo.SetSEID(iLocalSEID.SEID());
	
	// update client with seid details
	aSEPInfo = iInfo;
	
	LOG1(_L("Assigned SEID %d"), aSEPInfo.SEID().SEID());
	__ASSERT_DEBUG(iInfo.SEID().IsLocal(), Panic(EAvdtpSEIDHasWrongDomain));
	}
		
/*
add or change a capability
*/
void CLocalSEP::AddCapability(const TAvdtpServiceCapability* aCapability)
	{
	LOG_FUNC
	// take heap copy to persist this blob of data
	__ASSERT_DEBUG(aCapability, Panic(EAvdtpBadLocalSEPUpdate));
	
	TAvdtpServiceCategory c = aCapability->Category();
	if (iCapabilities[c])
		{
		delete iCapabilities[c];
		}
	iCapabilities[c] = const_cast<TAvdtpServiceCapability*>(aCapability);
	iCapabilityCategories.SetCapability(c);
	}
		
CLocalSEP::~CLocalSEP()
	{
	LOG_FUNC
	iLocalSEID.Close();
	iCapabilities.DeleteAll();
	iConfiguration.DeleteAll();
	if (iStream)
		{
		iStream->LocalSEPDestroyed(*this);
		}
	iPendingConfigurationProtocolDomain.Close();
	delete iSecurityControl;
	}

/*
Is the Local SEP in a state where it will accept pending configuration to be set.
Returns KErrNone if a subsequent call to SetPendingConfiguration() will succeed
otherwise an AVDTP error code.
*/
TInt CLocalSEP::CheckConfigurationState(TBool aIsReconfig) const
	{
	LOG_FUNC
	TInt ret = KErrNone;
	if (InUse() && !aIsReconfig)
		{
		ret = SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(EAvdtpSEPInUse);
		}
	else if (!InUse() && aIsReconfig)
		{
		ret = SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(EAvdtpSepNotInUse);
		}
	else
		{
		if (iPendingConfigurationProtocolDomain.Length())
			{
			// remote sent us while already setting configuration
			ret = SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(EAvdtpBadState);
			}
		}
	return ret;	
	}
	
/*
The pending configuration is set for a period of time before the configuration
is committed.

For an incoming (re)configure, the pending configuration is set when AVDTP 
receives the (re)configure request and committed when GAVDP processes the 
resulting indication and sends the (re)configure response.

For an outgoing (re)configure, the pending configuration is set when we send 
the (re)configure request and committed when we receive the (re)configure 
response.
*/	
TInt CLocalSEP::SetPendingConfiguration(const RBuf8& aConfigProtocolDomain, TBool aIsReconfig)
	{
	LOG_FUNC
	TInt ret = CheckConfigurationState(aIsReconfig);

	if (ret == KErrNone)
		{
		// take ownership of config
		iPendingConfigurationProtocolDomain.Assign(aConfigProtocolDomain);
		}
		
	return ret;
	}


void CLocalSEP::SetRemoteSEID(TSEID aINTSEID)
	{
	LOG_FUNC
	iRemoteSEID = aINTSEID;	
	__ASSERT_DEBUG(!iRemoteSEID.IsLocal(), Panic(EAvdtpSEIDHasWrongDomain));
	}


void CLocalSEP::CommitPendingConfigurationL(TBool aSuccess, TBool aReconfig)
	{
	LOG_FUNC
	if (aSuccess)
		{
		TAvdtpServiceCategories cats;
		cats.SetCapability(EAllServiceCategories);
		CCapabilityParseVisitor* parser = new(ELeave) CCapabilityParseVisitor(cats);
		iConfiguration.DeleteAll();
		parser->Process(iPendingConfigurationProtocolDomain);
		iConfiguration = parser->GetCapabilities();
		delete parser;

		iPreConfigured = EFalse;
		}
		
	// if we're configuring we need to set in use if success
	// if we're reconfiguring we don't want to clear in use if fail
	if (aReconfig)
		{
		if (!InUse())
			{
			// don't want to clear in use for failed reconfigurations
			SetInUse(aSuccess);
			}
		}
	else
		{
		SetInUse(aSuccess);
		}		
	// we were given ownership, close in either case
	iPendingConfigurationProtocolDomain.Close();		
	}
	
void CLocalSEP::SetSecurityControl(const HBufC8* aSecurityControl)
	{
	LOG_FUNC
	delete iSecurityControl;
	iSecurityControl = aSecurityControl;
	}
	
CAVStream& CLocalSEP::CreateStreamL(const TAvdtpSockAddr& aAddr,
									MAvdtpStreamNotify& aStreamNotify,
									CAvdtpProtocol& aProtocol)
	{
	LOG_FUNC
	if (!iStream)
		{
		iStream = CAVStream::NewL(aAddr, aStreamNotify, aProtocol, *this);
		aProtocol.StreamCreated(*iStream);
		}
	else
		{
		// client is trying to use same local sep for >1 stream. Wrong!
		User::Leave(KErrInUse);
		}
	return *Stream();
	}
	
void CLocalSEP::ExpectStreamL(const TBTDevAddr& aBTAddr,
								MAvdtpStreamNotify& aStreamNotify,
								CAvdtpProtocol& aProtocol)
	{
	LOG_FUNC
	TAvdtpSockAddr addr;
	addr.SetBTAddr(aBTAddr);
	addr.SetSEID(iRemoteSEID);
	CreateStreamL(addr, aStreamNotify, aProtocol).Configured();
	}

void CLocalSEP::SetInUse(TBool aInUse)
	{
	LOG_FUNC
	// clients must check that any bound stream will require erroring
	iInfo.SetInUse(aInUse);
	}

TBool CLocalSEP::MultiplexingConfigured() const
	{
	return static_cast<TAvdtpMultiplexingCapability*>(Configuration()[EServiceCategoryMultiplexing]) ?
			ETrue : EFalse;
	}
