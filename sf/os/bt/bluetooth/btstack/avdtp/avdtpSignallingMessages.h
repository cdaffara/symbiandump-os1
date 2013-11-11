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
// Signalling message
// The signalling message encapsulates the parsing and generation of signalling messages as outlined
// in the AVDTP specification.
// The signalling channel is called once the incoming signalling messages have parsed themselves
// If a parse error occurs then:
// - for commands, the parser leaves.  The leave is trapped and an appropriate Rejection is sent by the
//	signalling channel	
// - for responses the error is sent to the signalling channel for delivery to the correct signalling session
//	so that client requests may be completed

/**
@file
@internalComponent
*/


#ifndef AVDTPSIGNALLINGMESSAGES_H
#define AVDTPSIGNALLINGMESSAGES_H

#include <bluetoothav.h>
#include "avdtpMessage.h"
#include "gavdpinterface.h"
#include "avdtpAllocators.h"
#include "avdtputil.h"

class CSignallingChannel;
class CRemoteSEPCache;
class CCapabilityVisitor;
class RAvdtpTransactionLabel;
class RMBufChain;

enum TPostSendAction
	{
	EKeepSetRTX,		//< commands with timeouts
	EKeepDontSetRTX,	//< commands without timeouts
	EDiscard			//< responses
	};

enum TAvdtpPacketType
	{
	ESingle			=0x00,
	EStart			=0x01,
	EContinue		=0x02,
	EEnd			=0x03,
	};

enum TAvdtpMessageType
	{
	ECommand		=0x00,
	EGeneralReject	=0x01,
	EResponseAccept	=0x02,
	EResponseReject	=0x03,
	};
	
const TUint KAvdtpMinimumSignallingSinglePacketLength = 2;
const TUint KAvdtpMinimumSignallingStartPacketLength = 3;
const TUint KAvdtpMinimumSignallingContinuePacketLength = 1;
const TUint KAvdtpMinimumSignallingEndPacketLength = 1;

// min/max lengths for signalling packets - not including the signalling header
const TUint KAvdtpSignallingUnboundedMaximumLength = KMaxTUint;
// Discover
const TUint KAvdtpDiscoverCommandMinimumLength = 0;
const TUint KAvdtpDiscoverCommandMaximumLength = 0;
const TUint KAvdtpDiscoverAcceptMinimumLength = 2;
const TUint KAvdtpDiscoverAcceptMaximumLength = 64; // 32 SEPs
const TUint KAvdtpDiscoverRejectMinimumLength = 1;
const TUint KAvdtpDiscoverRejectMaximumLength = 1;
// Getcaps
const TUint KAvdtpGetCapsCommandMinimumLength = 1;
const TUint KAvdtpGetCapsCommandMaximumLength = 1;
const TUint KAvdtpGetCapsAcceptMinimumLength = KAvdtpServiceCapabilitiesHeaderLen;
const TUint KAvdtpGetCapsAcceptMaximumLength = KAvdtpSignallingUnboundedMaximumLength;
const TUint KAvdtpGetCapsRejectMinimumLength = 1;
const TUint KAvdtpGetCapsRejectMaximumLength = 1;
// SetConfig
const TUint KAvdtpSetConfigCommandMinimumLength = 2+KAvdtpServiceCapabilitiesHeaderLen;
const TUint KAvdtpSetConfigCommandMaximumLength = KAvdtpSignallingUnboundedMaximumLength;
const TUint KAvdtpSetConfigAcceptMinimumLength = 0;
const TUint KAvdtpSetConfigAcceptMaximumLength = 0;
const TUint KAvdtpSetConfigRejectMinimumLength = 2;
const TUint KAvdtpSetConfigRejectMaximumLength = 2;
// GetConfig
const TUint KAvdtpGetConfigCommandMinimumLength = 1;
const TUint KAvdtpGetConfigCommandMaximumLength = 1;
const TUint KAvdtpGetConfigAcceptMinimumLength = KAvdtpServiceCapabilitiesHeaderLen;
const TUint KAvdtpGetConfigAcceptMaximumLength = KAvdtpSignallingUnboundedMaximumLength;
const TUint KAvdtpGetConfigRejectMinimumLength = 1;
const TUint KAvdtpGetConfigRejectMaximumLength = 1;
// ReConfig
const TUint KAvdtpReConfigCommandMinimumLength = 1+KAvdtpServiceCapabilitiesHeaderLen;
const TUint KAvdtpReConfigCommandMaximumLength = KAvdtpSignallingUnboundedMaximumLength;
const TUint KAvdtpReConfigAcceptMinimumLength = 0;
const TUint KAvdtpReConfigAcceptMaximumLength = 0;
const TUint KAvdtpReConfigRejectMinimumLength = 2;
const TUint KAvdtpReConfigRejectMaximumLength = 2;
// Open
const TUint KAvdtpOpenCommandMinimumLength = 1;
const TUint KAvdtpOpenCommandMaximumLength = 1;
const TUint KAvdtpOpenAcceptMinimumLength = 0;
const TUint KAvdtpOpenAcceptMaximumLength = 0;
const TUint KAvdtpOpenRejectMinimumLength = 1;
const TUint KAvdtpOpenRejectMaximumLength = 1;
// Close
const TUint KAvdtpCloseCommandMinimumLength = 1;
const TUint KAvdtpCloseCommandMaximumLength = 1;
const TUint KAvdtpCloseAcceptMinimumLength = 0;
const TUint KAvdtpCloseAcceptMaximumLength = 0;
const TUint KAvdtpCloseRejectMinimumLength = 1;
const TUint KAvdtpCloseRejectMaximumLength = 1;
// Start
const TUint KAvdtpStartCommandMinimumLength = 1;
const TUint KAvdtpStartCommandMaximumLength = 32; // all SEIDs
const TUint KAvdtpStartAcceptMinimumLength = 0;
const TUint KAvdtpStartAcceptMaximumLength = 0;
const TUint KAvdtpStartRejectMinimumLength = 2;
const TUint KAvdtpStartRejectMaximumLength = 2;
// Suspend
const TUint KAvdtpSuspendCommandMinimumLength = 1;
const TUint KAvdtpSuspendCommandMaximumLength = 32; // all SEIDs
const TUint KAvdtpSuspendAcceptMinimumLength = 0;
const TUint KAvdtpSuspendAcceptMaximumLength = 0;
const TUint KAvdtpSuspendRejectMinimumLength = 2;
const TUint KAvdtpSuspendRejectMaximumLength = 2;
// Abort
const TUint KAvdtpAbortCommandMinimumLength = 1;
const TUint KAvdtpAbortCommandMaximumLength = 1;
const TUint KAvdtpAbortAcceptMinimumLength = 0;
const TUint KAvdtpAbortAcceptMaximumLength = 0;
// no Abort reject 
// SecurityControl
const TUint KAvdtpSecurityControlCommandMinimumLength = 1;
const TUint KAvdtpSecurityControlCommandMaximumLength = KAvdtpSignallingUnboundedMaximumLength;
const TUint KAvdtpSecurityControlAcceptMinimumLength = 1;
const TUint KAvdtpSecurityControlAcceptMaximumLength = KAvdtpSignallingUnboundedMaximumLength;
const TUint KAvdtpSecurityControlRejectMinimumLength = 1;
const TUint KAvdtpSecurityControlRejectMaximumLength = 1;
// General reject
const TUint KAvdtpGeneralRejectLength = 0;


// and ones for minimum and maximum LOSC for service categories in Set/Reconfig and GetCaps
const TUint KAvdtpLOSCUnboundedMaximumLength = KMaxTUint;

const TUint KAvdtpCapabilityMediaTransportMinimumLOSC = 0;
const TUint KAvdtpCapabilityMediaTransportMaximumLOSC = 0;
const TUint KAvdtpCapabilityReportingMinimumLOSC = 0;
const TUint KAvdtpCapabilityReportingMaximumLOSC = 0;
const TUint KAvdtpCapabilityRecoveryMinimumLOSC = 3;
const TUint KAvdtpCapabilityRecoveryMaximumLOSC = 3;
const TUint KAvdtpCapabilityMediaCodecMinimumLOSC = 2;
const TUint KAvdtpCapabilityMediaCodecMaximumLOSC = KAvdtpLOSCUnboundedMaximumLength;
const TUint KAvdtpCapabilityContentProtectionMinimumLOSC = 2;
const TUint KAvdtpCapabilityContentProtectionMaximumLOSC = KAvdtpLOSCUnboundedMaximumLength;
const TUint KAvdtpCapabilityHeaderCompressionMinimumLOSC = 1;
const TUint KAvdtpCapabilityHeaderCompressionMaximumLOSC = 1;
const TUint KAvdtpCapabilityMultiplexingMinimumLOSC = 3;
const TUint KAvdtpCapabilityMultiplexingMaximumLOSC = 7;

const TUint KAvdtpPacketTypeOffset = 0x02;
const TUint KAvdtpPacketTypeMask = 0x03 << KAvdtpPacketTypeOffset;

const TUint KAvdtpTransactionLabelOffset = 0x04;
const TUint KAvdtpTransactionLabelMask = 0x0f << KAvdtpTransactionLabelOffset;

const TUint KAvdtpMessageTypeOffset = 0x00;
const TUint KAvdtpMessageTypeMask = 0x3 << KAvdtpMessageTypeOffset;

const TUint KAvdtpSignalIdentifierMask = 0x3f;

const TUint KAvdtpMediaTypeOffset = 4; // see 8.6.2
const TUint KAvdtpTSEPOffset = 3;
const TUint KAvdtpPacketSEPInfoSize = 2; // see 8.6.2
const TUint KAvdtpInUseFlagOffset = 1;

NONSHARABLE_CLASS(TAvdtpFragmentationInfo)
	{
public:
	TAvdtpFragmentationInfo() : iFragmented(EFalse) {}
	TBool	iFragmented;
	TInt	iTotalPackets;
	TInt	iPacketNumber;
	};

NONSHARABLE_CLASS(CAvdtpSignallingMessage) : public CAvdtpMessage
	{
public:
	void SetType(TAvdtpMessageType aType,TAvdtpMessage aMessage);
	inline TAvdtpMessage Signal() const;
protected:
	CAvdtpSignallingMessage();
protected:
	TAvdtpMessageType		iMessageType;
	TAvdtpMessage			iSignal;
	};

inline TAvdtpMessage CAvdtpSignallingMessage::Signal() const
	{
	return iSignal;
	}
	
NONSHARABLE_CLASS(CAvdtpInboundSignallingMessage) : public CAvdtpSignallingMessage
/**
represents outgoing signalling messages, and has parsing methods for inbound packets
*/
	{
public:
	explicit CAvdtpInboundSignallingMessage(CSignallingChannel& aSignallingChannel);
	virtual TInt NewData(TUint aCount);
private:	
	static TAvdtpMessage SignalIdentifier(const RMBufChain& aMessage);
	static RMBufChain MessageL(RMBufChain& aPacket, const TAvdtpPacketType aPacketType);
	static TUint8 NumberSignalPackets(const RMBufChain& aChain);
	
private:
	TInt CheckPacketType(RMBufChain& aFragment,TAvdtpPacketType& aPacketType, TAvdtpMessageType& aMessageType, TAvdtpTransactionLabel& aTransactionLabel);
	void Process(TAvdtpTransactionLabel aLabel);

	static void HandleDiscoverL(CAvdtpInboundSignallingMessage& aThat, TAvdtpTransactionLabel aLabel);
	static void HandleGetCapsL(CAvdtpInboundSignallingMessage& aThat, TAvdtpTransactionLabel aLabel);
	static void HandleSetConfigL(CAvdtpInboundSignallingMessage& aThat, TAvdtpTransactionLabel aLabel);
	static void HandleGetConfigL(CAvdtpInboundSignallingMessage& aThat, TAvdtpTransactionLabel aLabel);
	static void HandleReconfigL(CAvdtpInboundSignallingMessage& aThat, TAvdtpTransactionLabel aLabel);
	static void HandleOpenL(CAvdtpInboundSignallingMessage& aThat, TAvdtpTransactionLabel aLabel);
	static void HandleStartL(CAvdtpInboundSignallingMessage& aThat, TAvdtpTransactionLabel aLabel);
	static void HandleReleaseL(CAvdtpInboundSignallingMessage& aThat, TAvdtpTransactionLabel aLabel);
	static void HandleSuspendL(CAvdtpInboundSignallingMessage& aThat, TAvdtpTransactionLabel aLabel);
	static void HandleAbortL(CAvdtpInboundSignallingMessage& aThat, TAvdtpTransactionLabel aLabel);
	static void HandleSecurityControlL(CAvdtpInboundSignallingMessage& aThat, TAvdtpTransactionLabel aLabel);

	void DoHandleDiscoverL(TAvdtpTransactionLabel aLabel);
	void DoHandleGetCapsL(TAvdtpTransactionLabel aLabel);
	void DoHandleSetConfigL(TAvdtpTransactionLabel aLabel);
	void DoHandleGetConfigL(TAvdtpTransactionLabel aLabel);
	void DoHandleReconfigL(TAvdtpTransactionLabel aLabel);
	void DoHandleOpenL(TAvdtpTransactionLabel aLabel);
	void DoHandleStartL(TAvdtpTransactionLabel aLabel);
	void DoHandleReleaseL(TAvdtpTransactionLabel aLabel);
	void DoHandleSuspendL(TAvdtpTransactionLabel aLabel);
	void DoHandleAbortL(TAvdtpTransactionLabel aLabel);
	void DoHandleSecurityControlL(TAvdtpTransactionLabel aLabel);
	
	TSEID LocalSEIDL();
	TSEID RemoteSEIDL();
	TSEID SEIDL(TBool aIsSemanticallyLocal); // common helper

	TInt CheckPacketLength(TUint aMin, TUint aMax);
	void CheckPacketLengthL(TUint aMin, TUint aMax);

private:
	enum TPacketBuildState
		{
		EAwaitingNew,  // single or start
		EAwaitingContinue,
		EAwaitingEnd,
		};
	TPacketBuildState		iPacketBuildState;
	TAvdtpFragmentationInfo	iFragmentationInfo;
	CSignallingChannel&		iSignallingChannel;
	TAvdtpTransactionLabel	iPacketBuildLabel;
	};

NONSHARABLE_CLASS(CAvdtpOutboundSignallingMessage) : public CAvdtpSignallingMessage
	{
friend class CSignallingChannel; //que'd in the SignallingChannel
public:
	TInt AllocateTransactionLabel(TTransactionLabelManager& aManager);
	void SetTransactionLabel(TAvdtpTransactionLabel aLabel);
	void AddSEPInfoL(const TAvdtpSEPInfo& aSEPInfo);
	void AddSEPCapabilityL(const TAvdtpServiceCapability& aCapability);
	TBool GetNextOutboundFragmentL(RMBufChain& aDest, TInt aMaxSize);
	TPostSendAction PostSendAction() const;
	inline TAvdtpTransactionLabel TransactionLabel() const;
	virtual ~CAvdtpOutboundSignallingMessage();
private:
	void Deque();
	static void PrependSignallingHeaderL(RMBufChain& aDest,
									const TAvdtpPacketType& aPacketType,
									const TAvdtpTransactionLabel& aTransLabel,
									const TAvdtpMessageType& aMessageType,
									TAvdtpMessage aSigId=EAvdtpNull,
									TInt aNoSigPkts=0
									);
	virtual TInt NewData(TUint aCount);		// pure - panic if called in this class
private:
	TDblQueLink				iLink;			//so we can be put in ques	
	TAvdtpFragmentationInfo	iFragInfo;
	// next 2 are logically a union, but the R-class has a ctor, so cannot be a C++ union
	RAvdtpTransactionLabel	iCommandLabel;
	TAvdtpTransactionLabel	iResponseLabel;
	};


inline TAvdtpTransactionLabel CAvdtpOutboundSignallingMessage::TransactionLabel() const
	{
	//OutBound Signalling message should never be EReserved
	__ASSERT_DEBUG(Signal() != EReserved,Panic(EAvdtpInvalidReservedValueInOutboundSignallingMessage));
	return (iMessageType==ECommand) ? iCommandLabel.TransactionLabel() : iResponseLabel;
	}

using namespace SymbianBluetoothAV;

/*static*/ namespace AvdtpSignallingMessage
	{
	static TInt AppendSEID(CAvdtpOutboundSignallingMessage& aSignallingMessage,TSEID aACPSEID);

	NONSHARABLE_CLASS(Command)
		{
		};
	NONSHARABLE_CLASS(Response)
		{
	protected:
		static TInt AppendResult(CAvdtpOutboundSignallingMessage& aSignallingMessage, TBluetoothAvDistributionError aResult);
		};
	NONSHARABLE_CLASS(Reject)
		{
	public:	
		static TInt Format(CAvdtpOutboundSignallingMessage& aSignallingMessage, 
					TBluetoothAvDistributionError aRejectionCode, const TDesC8* aRejectionData=NULL);
	private:
		static void FormatL(CAvdtpOutboundSignallingMessage& aSignallingMessage, 
					TBluetoothAvDistributionError aRejectionCode, const TDesC8* aRejectionData=NULL);
		};
	}


namespace AvdtpSignallingMessageDiscover
	{
	NONSHARABLE_CLASS(Response)
		{
	public:
		static TInt ParseL(const RMBufChain& aMessageData,
									   TDes8& aClientBuffer,
									   CRemoteSEPCache& aSEPCache,
									   const TBTDevAddr& aAddr);
		};
	}
	
namespace AvdtpSignallingMessageOpen
	{
	NONSHARABLE_CLASS(Command) : public AvdtpSignallingMessage::Command
		{
	public:
		inline static TInt Format(CAvdtpOutboundSignallingMessage& aSignallingMessage, TSEID aACPSEID);	
		};
	NONSHARABLE_CLASS(Response) : public AvdtpSignallingMessage::Response
		{
	public:
		inline static TInt Format(CAvdtpOutboundSignallingMessage& aSignallingMessage, TBluetoothAvDistributionError aResult);
		};
	}


namespace AvdtpSignallingMessageStreamRelease
	{
	NONSHARABLE_CLASS(Command) : public AvdtpSignallingMessage::Command
		{
	public:
		inline static TInt Format(CAvdtpOutboundSignallingMessage& aSignallingMessage, TSEID aACPSEID);
		};
	NONSHARABLE_CLASS(Response) :  public AvdtpSignallingMessage::Response
		{
	public:
		inline static TInt Format(CAvdtpOutboundSignallingMessage& aSignallingMessage, TBluetoothAvDistributionError aResult);
		};
	}

namespace AvdtpSignallingMessageSecurityControl
	{
	NONSHARABLE_CLASS(Command) : public AvdtpSignallingMessage::Command
		{
	public:
		inline static TInt Format(CAvdtpOutboundSignallingMessage& aSignallingMessage, TSEID aACPSEID, const TDesC8& aSecurityData);
		};
	NONSHARABLE_CLASS(Accept) : public AvdtpSignallingMessage::Response
		{
	public:
		inline static TInt Format(CAvdtpOutboundSignallingMessage& aSignallingMessage, const TDesC8* aSecurityData = NULL);
		};
	}

	
namespace AvdtpSignallingMessageAbort
	{
	NONSHARABLE_CLASS(Command) : public AvdtpSignallingMessage::Command
		{
	public:
		inline static TInt Format(CAvdtpOutboundSignallingMessage& aSignallingMessage, TSEID aACPSEID);
		};
	}
	
namespace AvdtpSignallingMessageGetCapabilities
	{
	NONSHARABLE_CLASS(Command) : public AvdtpSignallingMessage::Command
		{
	public:
		inline static TInt Format(CAvdtpOutboundSignallingMessage& aSignallingMessage, TSEID aACPSEID);
		};
	}

/**
Common stuff for Set and Re configure responses
*/
namespace AvdtpSignallingMessageConfigure
	{
	NONSHARABLE_CLASS(Response) :  public AvdtpSignallingMessage::Response
		{
	public:
		inline static TInt Format(CAvdtpOutboundSignallingMessage& aMessage, TAvdtpServiceCategory aCategory);
		};
	}
	
namespace AvdtpSignallingMessageSetConfiguration
	{
	NONSHARABLE_CLASS(Command) : public AvdtpSignallingMessage::Command
		{
	public:
		inline static TInt Format(CAvdtpOutboundSignallingMessage& aSignallingMessage, TSEID aACPSEID, TSEID aINTSEID, const RBuf8& aConfiguration);
	private:
		static void FormatL(CAvdtpOutboundSignallingMessage& aSignallingMessage, TSEID aACPSEID, TSEID aINTSEID, const RBuf8& aConfiguration);
		};
	NONSHARABLE_CLASS(Response) : public AvdtpSignallingMessageConfigure::Response
		{
		};
	}

namespace AvdtpSignallingMessageReconfigure
	{
	NONSHARABLE_CLASS(Command) : public AvdtpSignallingMessage::Command
		{
	public:
		inline static TInt Format(CAvdtpOutboundSignallingMessage& aSignallingMessage, TSEID aACPSEID, const RBuf8& aConfiguration);
	private:
		static void FormatL(CAvdtpOutboundSignallingMessage& aSignallingMessage, TSEID aACPSEID, const RBuf8& aConfiguration);
		};
	NONSHARABLE_CLASS(Response) : public AvdtpSignallingMessageConfigure::Response
		{
		};
	}

namespace AvdtpSignallingMessageStart
	{
	NONSHARABLE_CLASS(Command) : public AvdtpSignallingMessage::Command
		{
	public:
		inline static TInt Format(CAvdtpOutboundSignallingMessage& aSignallingMessage, TSEID aACPSEID);
		};
	NONSHARABLE_CLASS(Response) : public AvdtpSignallingMessage::Response
		{
	public:
		inline static TInt Format(CAvdtpOutboundSignallingMessage& aSignallingMessage, TBluetoothAvDistributionError aResult);
		};
	}

namespace AvdtpSignallingMessageSuspend
	{
	NONSHARABLE_CLASS(Command) : public AvdtpSignallingMessage::Command
		{
	public:
		inline static TInt Format(CAvdtpOutboundSignallingMessage& aSignallingMessage, TSEID aACPSEID);
		};
	NONSHARABLE_CLASS(Response) : public AvdtpSignallingMessage::Response
		{
	public:
		inline static TInt Format(CAvdtpOutboundSignallingMessage& aSignallingMessage, TBluetoothAvDistributionError aResult);
		};
	}



/*static*/
inline TInt AvdtpSignallingMessage::Reject::Format(CAvdtpOutboundSignallingMessage& aSignallingMessage, 
					TBluetoothAvDistributionError aRejectionCode, const TDesC8* aRejectionData/*=NULL*/)
	{
	TRAPD(err, FormatL(aSignallingMessage, aRejectionCode, aRejectionData));
	return err;
	}


/*static*/
inline TInt AvdtpSignallingMessageStart::Command::Format(CAvdtpOutboundSignallingMessage& aSignallingMessage, TSEID aACPSEID)
	{
	return AvdtpSignallingMessage::AppendSEID(aSignallingMessage, aACPSEID);
	}
	
inline TInt AvdtpSignallingMessageStart::Response::Format(CAvdtpOutboundSignallingMessage& aSignallingMessage,
													TBluetoothAvDistributionError aResult)
	{
	return Response::AppendResult(aSignallingMessage, aResult);
	}

/*static*/
inline TInt AvdtpSignallingMessageSuspend::Command::Format(CAvdtpOutboundSignallingMessage& aSignallingMessage, TSEID aACPSEID)
	{
	return AvdtpSignallingMessage::AppendSEID(aSignallingMessage, aACPSEID);
	}
	
inline TInt AvdtpSignallingMessageSuspend::Response::Format(CAvdtpOutboundSignallingMessage& aSignallingMessage,
														TBluetoothAvDistributionError aResult)
	{
	return Response::AppendResult(aSignallingMessage, aResult);
	}

/*static*/
inline TInt AvdtpSignallingMessageStreamRelease::Command::Format(CAvdtpOutboundSignallingMessage& aSignallingMessage,
															TSEID aACPSEID)
	{
	return AvdtpSignallingMessage::AppendSEID(aSignallingMessage, aACPSEID);
	}

/*static*/
inline TInt AvdtpSignallingMessageSecurityControl::Command::Format(CAvdtpOutboundSignallingMessage& aSignallingMessage,
																	TSEID aACPSEID, const TDesC8& aSecurityData)
	{
	TInt res = AvdtpSignallingMessage::AppendSEID(aSignallingMessage, aACPSEID);
	if (res==KErrNone)
		{
		TRAP(res, aSignallingMessage.AppendDataL(aSecurityData));
		}
	return res;
	}

/*static*/
inline TInt AvdtpSignallingMessageSecurityControl::Accept::Format(CAvdtpOutboundSignallingMessage& aSignallingMessage,
																	  const TDesC8* aSecurityData)
	{
	TInt res=KErrNone;
	
	if (aSecurityData)
		{
		TRAP(res, aSignallingMessage.AppendDataL(*aSecurityData));
		}
	return res;
	}
	
/*static*/
inline TInt AvdtpSignallingMessageAbort::Command::Format(CAvdtpOutboundSignallingMessage& aSignallingMessage,
																	TSEID aACPSEID)
	{
	return AvdtpSignallingMessage::AppendSEID(aSignallingMessage, aACPSEID);
	}

/*static*/
inline TInt AvdtpSignallingMessageGetCapabilities::Command::Format(CAvdtpOutboundSignallingMessage& aSignallingMessage,
																	TSEID aACPSEID)
	{
	return AvdtpSignallingMessage::AppendSEID(aSignallingMessage, aACPSEID);
	}

/*static*/
inline TInt AvdtpSignallingMessageSetConfiguration::Command::Format(CAvdtpOutboundSignallingMessage& aSignallingMessage,
																	TSEID aACPSEID,
																	TSEID aINTSEID,
																	const RBuf8& aConfiguration)
	{
	TRAPD(err, FormatL(aSignallingMessage, aACPSEID, aINTSEID, aConfiguration));
	return err;
	}

/*static*/
inline TInt AvdtpSignallingMessageReconfigure::Command::Format(CAvdtpOutboundSignallingMessage& aSignallingMessage,
																	TSEID aACPSEID,
																	const RBuf8& aConfiguration)
	{
	TRAPD(err, FormatL(aSignallingMessage, aACPSEID, aConfiguration));
	return err;
	}

/*static*/
inline TInt AvdtpSignallingMessageConfigure::Response::Format(CAvdtpOutboundSignallingMessage& aSignallingMessage,
											TAvdtpServiceCategory aCategory)
	{
	TRAPD(err, aSignallingMessage.AppendDataL(aCategory));
	return err;
	}

/*static*/
inline TInt AvdtpSignallingMessageOpen::Response::Format(CAvdtpOutboundSignallingMessage& aSignallingMessage,
													TBluetoothAvDistributionError aResult)
	{
	return Response::AppendResult(aSignallingMessage, aResult);
	}

/*static*/
inline TInt AvdtpSignallingMessageOpen::Command::Format(CAvdtpOutboundSignallingMessage& aSignallingMessage,
																	TSEID aACPSEID)
	{
	return AvdtpSignallingMessage::AppendSEID(aSignallingMessage, aACPSEID);
	}


/*static*/
inline TInt AvdtpSignallingMessage::AppendSEID(CAvdtpOutboundSignallingMessage& aSignallingMessage,
																	TSEID aACPSEID)
	{
	TRAPD(err, aSignallingMessage.AppendDataL(aACPSEID.PacketValue()));
	return err;
	}

/*static*/
inline TInt AvdtpSignallingMessage::Response::AppendResult(CAvdtpOutboundSignallingMessage& aSignallingMessage,
													TBluetoothAvDistributionError aResult)
	{
	TRAPD(err, aSignallingMessage.AppendDataL(aResult));
	return err;
	}

// internalComponent stack capability visitors	
	

/*
@internalComponent
*/
NONSHARABLE_CLASS(CCapabilityExtractorVisitor) : public CCapabilityVisitor
	{
public:
	CCapabilityExtractorVisitor(TAvdtpServiceCategory aRequiredCapability);
	TPtrC8 GetCapability() const;
	TBool Capability(TAvdtpServiceCategory aCat);
private:
	TAvdtpServiceCategory	iRequiredCapability;	
	TPtrC8					iPtr;
	};
	



/*
@internalComponent
goes through to see if spec unknown capabilities present
*/
NONSHARABLE_CLASS(CCapabilityValidateVisitor) : public CCapabilityVisitor
	{
public:
	virtual TBool Capability(TAvdtpServiceCategory aCat);
	inline TInt Result() const;
	inline TAvdtpServiceCategory InvalidCategory() const;
private:
	inline TBool IsLOSCValid(TUint aLOSC, TUint aMin, TUint aMax);
	TInt	iResult; // the error to use if invalid, or KErrNone
	TAvdtpServiceCategory	iInvalidCategory;
	};

/*
@internalComponent
goes through to remove spec unknown capabilities
*/
NONSHARABLE_CLASS(CCapabilityRemoveUnknownVisitor) : public CCapabilityVisitor
	{
public:
	virtual TBool Capability(TAvdtpServiceCategory aCat);
	};

#ifdef _DEBUG
/*
@internalComponent
*/
NONSHARABLE_CLASS(CCapabilityFlogVisitor) : public CCapabilityVisitor
	{
public:
	virtual TBool Capability(TAvdtpServiceCategory aCat);
	};
#endif
		

#endif //AVDTPSIGNALLINGMESSAGES_H
