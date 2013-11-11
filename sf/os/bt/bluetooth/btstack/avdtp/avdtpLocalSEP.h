// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Defines the avdtp local SEP class
// 
//

/**
 @file
 @internalComponent
*/


#ifndef AVDTPLOCALSEP_H
#define AVDTPLOCALSEP_H

#include <bluetoothav.h>
#include "gavdpinterface.h"
#include "avdtpAllocators.h"

class CAVStream;
class CAvdtpProtocol;
class MAvdtpStreamNotify;

NONSHARABLE_CLASS(CLocalSEP) : public CBase
	{
public:
	static CLocalSEP* NewL(TAvdtpSEPInfo& aSEPInfo, TSEIDManager& aSEIDManager);
	inline TSEID SEID() const;
	inline TAvdtpSEPInfo Info() const;

	void AddCapability(const TAvdtpServiceCapability* aCapability); //transfers ownership
	inline TAvdtpServiceCategories Categories() const;
	TInt CheckConfigurationState(TBool aIsReconfig) const;
	TInt SetPendingConfiguration(const RBuf8& aConfigurationProtocolDomain, TBool aIsReconfig);
	void SetRemoteSEID(TSEID aSEID);
	inline TPtrC8 PendingConfiguration() const;
	void SetSecurityControl(const HBufC8* aSecurityControl);
	inline TPtrC8 SecurityControl() const;
	void CommitPendingConfigurationL(TBool aSuccess, TBool aReconfig);
	inline const TCapabilitiesArray& Configuration() const; // returns in non-protocol domain
	inline const TCapabilitiesArray& Capabilities() const; // returns in non-protocol domain	
	void SetInUse(TBool aInUse);
	inline TBool InUse() const;
	void PrepareInitialConfiguration();
	inline TBool PreConfigured() const;
	TBool MultiplexingConfigured() const;
	
	~CLocalSEP();
	
	CAVStream& CreateStreamL(const TAvdtpSockAddr& aAddr,
							 MAvdtpStreamNotify& aStreamNotify,
							 CAvdtpProtocol& aProtocol);
							 
	void ExpectStreamL(const TBTDevAddr& aBTAddr,
						MAvdtpStreamNotify& aStreamNotify,
						CAvdtpProtocol& aProtocol);
							 
	inline void StreamDestroyed(const CAVStream& aStream);
	inline CAVStream* Stream() const;
public:
	TDblQueLink		iSignallingSessionLink;
private:
	void ConstructL(TAvdtpSEPInfo& aSEPInfo, TSEIDManager& aSEIDManager);
	CLocalSEP();
private:
	RSEID			iLocalSEID;
	TAvdtpSEPInfo	iInfo;
	
	TCapabilitiesArray iCapabilities;
	TCapabilitiesArray iConfiguration;

	RBuf8	iPendingConfigurationProtocolDomain;	
	const HBufC8*	iSecurityControl;
	
	TAvdtpServiceCategories iCapabilityCategories;
	CAVStream*	iStream; // the stream we may or may not end up in, owned
	TSEID	iRemoteSEID;
	TBool	iPreConfigured;
	};

inline TBool CLocalSEP::PreConfigured() const
	{
	return iPreConfigured;
	}

inline TSEID CLocalSEP::SEID() const
	{
	return iInfo.SEID();
	}
		
inline TAvdtpSEPInfo CLocalSEP::Info() const
	{
	return iInfo;
	}
	
inline const TCapabilitiesArray& CLocalSEP::Capabilities() const
	{
	return iCapabilities;
	}

inline const TCapabilitiesArray& CLocalSEP::Configuration() const
	{
	return iConfiguration;
	}
	
inline TAvdtpServiceCategories CLocalSEP::Categories() const
	{
	return iCapabilityCategories;
	}

inline TBool CLocalSEP::InUse() const
	{
	return iInfo.InUse();
	}
	
inline CAVStream* CLocalSEP::Stream() const
	{
	// doesn't return ownership
	return iStream;
	}
	
inline TPtrC8 CLocalSEP::PendingConfiguration() const
	{
	return iPendingConfigurationProtocolDomain;
	}
	
inline TPtrC8 CLocalSEP::SecurityControl() const
	{
	return *iSecurityControl;
	}

#endif //AVDTPLOCALSEP

#ifndef _AVDTPLOCALSEPSTREAM
#define _AVDTPLOCALSEPSTREAM
#include "avdtpStream.h"

#ifdef _DEBUG
inline void CLocalSEP::StreamDestroyed(const CAVStream& aStream)
#else
inline void CLocalSEP::StreamDestroyed(const CAVStream& /*aStream*/)
#endif
	{
	if (iStream) __ASSERT_DEBUG(&aStream == iStream, User::Panic(_L("avdtplocalsep.h"), __LINE__));
	iStream = NULL;
	SetInUse(EFalse);
	}

#endif // _AVDTPLOCALSEPSTREAM

