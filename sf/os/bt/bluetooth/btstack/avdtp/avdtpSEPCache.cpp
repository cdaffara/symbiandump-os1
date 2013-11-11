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
// Implements the avdtp SEP cache
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/logger.h>
#include <bluetoothav.h>
#include "avdtpSEPCache.h"
#include "gavdpinterface.h"
#include "avdtpSignallingMessages.h"
#include "avdtputil.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVDTP);
#endif

CRemoteSEP* CRemoteSEP::NewL(const TBTDevAddr& aAddr, const TAvdtpSEPInfo& aInfo)
	{
	LOG_STATIC_FUNC
	CRemoteSEP* s = new (ELeave) CRemoteSEP(aAddr, aInfo);
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop(s);
	return s;
	}
	
CRemoteSEP::CRemoteSEP(const TBTDevAddr& aAddr, const TAvdtpSEPInfo& aInfo)
: iRemoteDev(aAddr), iInfo(aInfo)
	{
	LOG_FUNC
	// age already set to zero
	 // only store remote sep details in the cache
	__ASSERT_DEBUG(!aInfo.SEID().IsLocal(), Panic(EAvdtpSEIDHasWrongDomain));
	}
	
CRemoteSEP::~CRemoteSEP()
	{
	LOG_FUNC
	delete iCapabilitiesBuf;
	}

	
void CRemoteSEP::ConstructL()
	{
	LOG_FUNC
	}

void CRemoteSEP::UpdateInfo(const TAvdtpSEPInfo& aInfo)
	{
	iInfo = aInfo;
	}
	
TBool CRemoteSEP::HasCapability(TAvdtpServiceCategory aCat)
	{
	LOG_FUNC
	TBool ret = EFalse;
	
	if (iCapabilitiesBuf)
		{
		CCapabilityPresentVisitor* presVisitor = new CCapabilityPresentVisitor(aCat);
		
		if (presVisitor)
			{
			TPtr8 ptr = iCapabilitiesBuf->Des();
			presVisitor->Process(ptr);
			ret = presVisitor->IsPresent();
			delete presVisitor;
			}
		}
	return ret;
	}
	

/*
Returns a pointer descriptor to SEP capability *in the protocol domain*
It is upto a client to get this parsed as needed
Motivation - to push more easilythrough ESOCK AND save unneccasry parsing
*/
TPtrC8 CRemoteSEP::GetCapabilityL(TAvdtpServiceCategory aCat)
	{
	LOG_FUNC
	// use visitor to extract relevant bit
	CCapabilityExtractorVisitor* extractor = new (ELeave) CCapabilityExtractorVisitor(aCat);
	TPtr8 ptr = iCapabilitiesBuf->Des();
	extractor->Process(ptr);
	
	TPtrC8 cap = extractor->GetCapability();
	delete extractor;
	return cap;
	}


void CRemoteSEP::SetCapabilities(HBufC8* aCapsAsProtocol)
	{
	LOG_FUNC
	delete iCapabilitiesBuf;
	iCapabilitiesBuf = aCapsAsProtocol; // takes ownership
	
	// set a visitor on it now to summarise caps
	CCapabilitySummaryVisitor* summariser = new CCapabilitySummaryVisitor;
	if (summariser)
		{
		TPtr8 des = iCapabilitiesBuf->Des();
		summariser->Process(des);
		iKnownCaps = summariser->CapabilitiesPresent();
		delete summariser;
		}
	}

CRemoteSEPCache* CRemoteSEPCache::NewL()
	{
	LOG_STATIC_FUNC
	CRemoteSEPCache* s = new (ELeave) CRemoteSEPCache();
	return s;	
	}
	
/**
Modifies SEP if already exists
*/
TInt CRemoteSEPCache::AddSEP(const TBTDevAddr& aAddr, const TAvdtpSEPInfo& aInfo)
	{
	LOG_FUNC
	TInt err = KErrNone;
	
	CRemoteSEP* sep = FindSEP(aAddr, aInfo.SEID());
	if (!sep)
		{
		TRAP(err, sep = CRemoteSEP::NewL(aAddr, aInfo));
		if (!err)
			{
			err = iRemoteSEPs.Append(sep);
			if (err)
	        	{
	       	    delete sep;
	        	}
			}
		}
	else
		{
		// modify the one there
		sep->UpdateInfo(aInfo);
		}
	return err;
	}
	
TPtrC8 CRemoteSEPCache::GetCapabilityL(const TBTDevAddr& aAddr, TSEID aSEID, TAvdtpServiceCategory aCat)
	{
	LOG_FUNC
	CRemoteSEP* sep = FindSEP(aAddr, aSEID);
	
	if (sep)
		{
		return sep->GetCapabilityL(aCat);
		}
		
	User::Leave(KErrNotFound);
	return KNullDesC8();// never reached
	}


TAvdtpServiceCategories CRemoteSEPCache::GetCapabilitiesL(const TBTDevAddr& aAddr, TSEID aSEID) const
	{
	LOG_FUNC
	CRemoteSEP* sep = FindSEP(aAddr, aSEID);
	
	if (sep)
		{
		return sep->GetCapabilities();
		}
		
	User::Leave(KErrNotFound);
	return TAvdtpServiceCategories();
	}


TBool CRemoteSEPCache::HasCapability(const TBTDevAddr& aAddr, TSEID aSEID, TAvdtpServiceCategory aCat)
	{
	LOG_FUNC
	CRemoteSEP* sep = FindSEP(aAddr, aSEID);
	
	if (sep)
		{
		return sep->HasCapability(aCat);
		}
		
	return EFalse; // unknown => false
	}



void CRemoteSEPCache::SetCapabilities(const TBTDevAddr& aAddr, TSEID aSEID, HBufC8* aCapabilitiesAsProtocol)
/**
Sets capabilites from the RMBufChain passed in on the relevant CRemoteSEP in our
cache.

@return a bitmask of the service categories we saw.
*/
	{
	LOG_FUNC
	CRemoteSEP* sep = FindSEP(aAddr, aSEID);
	
	if (sep)
		{
		sep->SetCapabilities(aCapabilitiesAsProtocol);
		}
	else
		{
		// would be nice to cache this, but if we don't know the SEP at this point
		// the user hadn't done a Discover - which is ok for them, but for us we don't know if the
		// SEP is in use or Audio/Video etc....We keep the capabilities as the stack finds
		// them useful later
		TAvdtpSEPInfo info;
		info.SetSEID(aSEID);
		if (AddSEP(aAddr, info)==KErrNone)
			{
			SetCapabilities(aAddr, aSEID, aCapabilitiesAsProtocol);
			}
		}
	}

CRemoteSEPCache::~CRemoteSEPCache()
	{
	LOG_FUNC
	iRemoteSEPs.ResetAndDestroy();
	}

TBool CRemoteSEPCache::Exists(const TBTDevAddr& aAddr, TSEID aSEID) const
	{
	LOG_FUNC
	return (FindSEP(aAddr, aSEID)!=NULL);
	}
	
CRemoteSEP* CRemoteSEPCache::FindSEP(const TBTDevAddr& aAddr, TSEID aSEID) const
	{
	LOG_FUNC
	__ASSERT_DEBUG(!aSEID.IsLocal(), Panic(EAvdtpSEIDHasWrongDomain));
	
	for (TInt i=0; i<iRemoteSEPs.Count(); i++)
		{
		CRemoteSEP& sep = *iRemoteSEPs[i];
		if (sep.iInfo.SEID() == aSEID && sep.iRemoteDev == aAddr)
			{
			return iRemoteSEPs[i]; // does not transfer ownership
			}
		}
	return NULL;
	}

void CRemoteSEPCache::InvalidateSEPs(const TBTDevAddr& aAddr)
	{
	LOG_FUNC
	// typically called when signalling channel has gone down
	// in future we could age the seps and save doing a GetCaps, but this does
	// risk the use of invalid capabilities (but would still be compliant with GAVDP)	
	CRemoteSEP* sep = NULL;
	for (TInt i=0; i<iRemoteSEPs.Count(); i++)
		{
		sep = iRemoteSEPs[i];
		if (sep->iRemoteDev == aAddr)
			{
			iRemoteSEPs.Remove(i);
			delete sep;
			sep = NULL;
			}
		}
	}

