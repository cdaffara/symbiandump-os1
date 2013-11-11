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
// Signalling session
// A signalling session represents one RGavdp object.  It is-a transport session.
// Each signalling session is bound to a signalling channel - a signalling channel can support many
// signalling sessions.
// The multiplexing allows for many applications to stream to the same remote device
// A signalling session also "manages" each of the stream endpoints registered by an RGavdp client.
// A RGavdp client may register many (local) seps.
// The signalling session receives indications from the signalling channel - since the signalling channel
// (intentionally) doesn't know which SEPs are managed by which session various protocol events are
// offered to all of the signalling sessions.
// The signalling sessions should then see if they manage a sep for which the protocol event has happened
// and "consume" the event.
// At certain stages in the protocol state machine the local seps may "grow" a stream.  Subsequent protocol
// primitives may then be called via the sep's stream.
// The specification does not define Signalling Sessions.  However they are required to fulfill the need to 
// support streams from multiple applications to the same target device. 
// the session is then "fixed" to that remote.  Therefore all SEPs managed by the session
// become available only for use with that remote.  The user *can* (if this is important)
// still use multiple SEPs by placing one SEP in one RGavdp (implies one signalling session)
// a future extension would be to see from which remote address a configure had been received
// Hence there is one SEP pointer recording the SEP undergoing configuration
// 
//

/**
 @note This design has a restriction at this point in time. Once connected to a remote
 @note At present this design allows for at most one SEP to undergo configuration at a time from all remote devices
*/

#ifndef AVDTPSIGNALLINGSESSION_H
#define AVDTPSIGNALLINGSESSION_H

#include "avdtpTransportSession.h"
#include "avdtpStream.h"


const TInt KAvdtpACPReleaseChannelCloseTimeout = 10000000;


class CTransportChannel;
class CAvdtpSAP;
class CAvdtpProtocol;
class CSignallingChannel;
class CSEPConfigurator;
class CLocalSEP;


NONSHARABLE_CLASS(CSignallingSession) : public CTransportSession, public XAvdtpSignalReceiver, public MAvdtpStreamNotify
	{
friend class CLocalSEPConfigurator;
friend class CRemoteSEPConfigurator;

public:
	static CSignallingSession* NewLC(CAvdtpProtocol& aProtocol, CAvdtpSAP& aSAP);
	static CSignallingSession* NewL(CAvdtpProtocol& aProtocol, CAvdtpSAP& aSAP);

	virtual TInt SetOption(TUint aLevel, TUint aName, const TDesC8 &aOption);
	virtual TInt GetOption(TUint aLevel, TUint aName, TDes8& aOption) const; 
	virtual void Ioctl(TUint aLevel, TUint aName, const TDesC8* aOption);
	virtual void CancelIoctl(TUint aLevel, TUint aName);
	virtual TInt Send(RMBufChain& aData, TUint aOptions, TSockAddr* aAddr);
	virtual TInt GetData(RMBufChain& aData);
	virtual void DoShutdown();

	virtual TInt ActiveOpen();
	~CSignallingSession();

private:
//from signal receiver - we're interested in most, but not all service upcalls
	virtual TInt DiscoverIndication(TAvdtpTransactionLabel aLabel, CAvdtpOutboundSignallingMessage& aDiscoverResponsePacket);
	virtual TInt GetCapsIndication(TAvdtpTransactionLabel aLabel,
									TSEID aSEID,
									CAvdtpOutboundSignallingMessage& aGetCapsResponseMessage);
	virtual TInt SetConfigIndication(TAvdtpTransactionLabel aLabel, TSEID aLocalSEID,
											TSEID aINTSEID, RBuf8& aConfigData);
	virtual TInt GetConfigIndication(TAvdtpTransactionLabel aLabel, TSEID aLocalSEID,
									CAvdtpOutboundSignallingMessage& /*aGetConfigResponseMessage*/);
	virtual TInt ReconfigIndication(TAvdtpTransactionLabel aLabel,
													TSEID aLocalSEID,
													RBuf8& aNewConfig);
	virtual TInt OpenIndication(TSEID aLocalSEID);
	virtual TInt StartIndication(TAvdtpTransactionLabel aLabel, TSEID aLocalSEID);
	virtual TInt ReleaseIndication(TAvdtpTransactionLabel aLabel, TSEID aLocalSEID);
	virtual TInt SuspendIndication(TAvdtpTransactionLabel aLabel, TSEID aLocalSEID);
	virtual TInt AbortIndication(TAvdtpTransactionLabel aLabel, TSEID aLocalSEID);
	virtual TInt SecurityControlIndication(TAvdtpTransactionLabel aLabel, TSEID aLocalSEID, const HBufC8* aSecurityData);

	virtual void SignallingChannelError(TInt aError);
// protocol confirms
// no labels needed

	virtual void DiscoverConfirm(TInt aResult, const TAvdtpInternalDiscoverConfirm* const aConfirm);
	virtual void GetCapsConfirm(TInt aResult, TSEID aRemoteSEID, TAvdtpServiceCatBitMask aSeen);
	virtual void AbortConfirm(TSEID aRemoteSEID);
	virtual void SecurityControlConfirm(TInt aResult, TSEID aRemoteSEID, const TDesC8& aResponseData);

// interface from stream
	virtual void StreamStarted(TSEID aLocalSEID);
	virtual void StreamSuspended(TSEID aLocalSEID);
	virtual void StreamAccepted(TSEID aLocalSEID, TSEID aRemoteSEID, TBool aWithReporting, TBool aWithRecovery);
	virtual void StreamConfigured(TSEID aLocalSEID, TInt aError, TAvdtpServiceCategory aFailedCategory);
	virtual void StreamInitiatedServiceFailed(const CAVStream& aStream, TInt aError);
	virtual void StreamReadyForStartIndication(const TAvdtpTransactionLabel aLabel, const TSEID aSEID);

// callbacks from signalling channel
	virtual void ServiceError(TInt aError);
	virtual void SignallingChannelReady(CSignallingChannel& aNewChannel);

private:
	void DoConfigureStreamL(RBuf8& aConfigBuffer,
							CLocalSEP& aLocalSEP,
							TSEID aRemoteSEID,
							TBool aReportingConfigured,
							TBool aRecoveryConfigured);
	TInt SendAbort(TSEID aRemoteSEID);
	void CreateLocalSEPL(TAvdtpSEPInfo& aSEPInfo); // note, not const ref
	CLocalSEP& FindLocalSEPL(TSEID aSEID) const;
	CLocalSEP* FindLocalSEP(TSEID aSEID) const;
 
	CSignallingSession(CAvdtpProtocol& aProtocol, CAvdtpSAP& aSAP);
	void ConstructL();
	TInt DoAddCapabilitySetOpt(const TDesC8& aOption);
	inline void CompleteBasicService(TInt aErr);
	
	CAVStream* FindStreamFromRemoteSEID(const TSEID& aRemoteSEID) const;
	void ClearSignallingChannel();
	void DestroyLocalSEPs();
	void DestroyStream(CAVStream* aStream, TInt aError);
	
private:
	CSignallingChannel*		iSignallingChannel; // non-owned
	CSEPConfigurator*		iSEPConfigurator;
	mutable TDblQue<CLocalSEP>	iLocalSEPs; // those awaiting discover (by "our" app)

	TBool					iIsListening;
	};
	
/*
@internalComponent
*/
// goes through to see if remote has proposed unsupported local categories
NONSHARABLE_CLASS(CCapabilityNotAllowedVisitor) : public CCapabilityVisitor
	{
public:
	CCapabilityNotAllowedVisitor(TAvdtpServiceCategories aLocallySupported);
	virtual TBool Capability(TAvdtpServiceCategory aCat);
	inline TBool IsValid() const;
	inline TAvdtpServiceCategory WrongCategory() const;
private:
	TAvdtpServiceCategories iLocallySupported;
	TBool	iIsValid;
	TAvdtpServiceCategory iWrongCategory;
	};

	
#endif //AVDTPSIGNALLINGSESSION_H

