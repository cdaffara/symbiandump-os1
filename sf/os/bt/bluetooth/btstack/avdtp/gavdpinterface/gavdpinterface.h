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
// Contains internalTechnology types, values etc to allow AVDTP and GAVDP to communicate
// 
//

/**
 @file
 @internalTechnology
*/


#ifndef _GAVDPINTERFACE_H
#define _GAVDPINTERFACE_H

#include <bluetoothav.h>
#include <gavdp.h>

using namespace SymbianBluetoothAV;

class AvdtpInternalUtils
	{
public:
	static inline TInt NumberOfTransportObjects(TBool aReporting, TBool aRecovery);
/*
@return SymbianOS error code
@param AVDTP error code
*/
	static inline TAvdtpSignallingErrorCode SymbianErrorToAvdtpError(TInt aSymbianError);		
	static inline void PacketErrorLeaveL(TAvdtpSignallingErrorCode aCode);
	};
	

enum TBTAVDTPOptions
/*
@internalTechnology
*/
	{
	EDiscoverSEPs,
	EGetCapabilities,
	ESelectSEP,
	ESetConfiguration,
	EStartStreaming,
	EStartConfiguringLocal,
	EStartConfiguringRemote,
	EAddCapabilitySelection,
	ESuspendStreaming,
	EAbortStream,
	EGetProposedConfiguration,
	EGetSecurityControl,
	EGetAVDTPCapabilityResponse,
	ERegisterSEP,
	EAwaitPassiveSignallingChannel,
	ESetAsSecondarySAP,
	EBindToSecondarySAP,
	EAwaitIndication,
	ESendSetConfigurationResponse,
	ESendSecurityControlResponse,
	ESendStartResponse,
	ESendSuspendResponse,
	ESendConfigurationResponse,
	ESendSecurityControl,
	EStopAwaitPassiveSignallingChannel,
	EGetReportingAndRecoveryConfig,
	EShutdown,
	};

/**
@internalTechnology
*/
const TUint KMaximumRealSEID = KMaxTUint6;

/**
@internalTechnology
*/
const TTCID KInvalidTCID = KMaxTUint8;

/**
@internalTechnology
*/
const TTCID KDirectChannelTCID = KMaxTUint8-1; // the spec (unclearly) says that Direct channels have *no* TCID, not even one unseen by remote

/**
@internalTechnology
*/
const TTCID KRequestTCID = 0;

/**
@internalTechnology
*/
const TTCID KMinimumRealTCID = 1;

/**
@internalTechnology
*/
const TTSID KInvalidTSID = KMaxTUint8;

/**
@internalTechnology
*/
const TTCID KMinimumRealTSID = 1;

/**
@internalTechnology
*/
const TTCID KMinimumRealSEID = 1;

/**
@internalTechnology
*/
const TTSID KMaximumRealTSID = KMaxTUint5;

/**
@internalTechnology
*/
const TTCID KMaximumRealTCID = KMaxTUint5;

/**
@internalTechnology
*/
const TUint KAvdtpPacketSEIDOffset = 2; // the SEIDs are all in top 6 bits when in packets

/**
@internalTechnology
*/
const TUint KAvdtpPacketSEIDMask = 0x3f;
		
/**
@internalTechnology
*/
static const TUint KAvdtpServiceCapabilitiesHeaderLen = 2;
/**
@internalTechnology
*/
static const TUint8 KUnknownLOSC = KMaxTUint8;
/**
@internalTechnology
Assign a matching error code here equal to the panic value. This can then be returned and used to panic the client.
New panic errors can be added by assigning a value between -994 and -999 in the TGavdp enum.
*/
const TInt KErrPanicAvdtpOpenInBadState = EGavdpOpenInBadState;

enum TAvdtpMessage
/*
@internalTechnology
*/
	{
	EAvdtpNull				=0xff, //This is a special value for asserting on
	EReserved				=0x00, 
	EAvdtpDiscover			=0x01,
	EAvdtpGetCapabilities	=0x02,
	EAvdtpSetConfiguration	=0x03,
	EAvdtpGetConfiguration	=0x04,
	EAvdtpReconfigure		=0x05,
	EAvdtpOpen				=0x06,
	EAvdtpStart				=0x07,
	EAvdtpRelease			=0x08,
	EAvdtpSuspend			=0x09,
	EAvdtpAbort				=0x0a,		
	EAvdtpSecurityControl	=0x0b,
	/* This should be have the largest valid signal identifier value in TAvdtpMessage. 
	 * In order to acheive this all the new signal identifier should be added before it. 
	 */  
	EAvdtpLargestValidSignalIndentifier
	};

struct TInternalGetSEPCapability
/*
@internalTechnology - for communication from GAVDP to AVDTP
*/
	{
	TSEID	iSEID;
	TAvdtpServiceCategory iServiceCategory;
	};

struct TInternalGetProposedConfiguration
/*
@internalTechnology - for communication from GAVDP to AVDTP
This is used for casting purposes - to retrieve the configuration
an a descripto large enough (see indication) is created. This struct
is then overload into the HBuf to send information for the GetOpt
The outward direction contains the capability.
*/
	{
	TSEID	iSEID; //inward
	};

struct TInternalGetSecurityControl
/*
@internalTechnology - for communication from GAVDP to AVDTP
This is used for casting purposes - to retrieve the securitycontrol
an a descripto large enough (see indication) is created. This struct
is then overload into the HBuf to send information for the GetOpt
The outward direction contains the securitycontrol.
*/
	{
	TSEID	iSEID;
	};
	
struct TInternalSelectRemoteSEP
/*
@internalTechnology - for communication from GAVDP to AVDTP
*/
	{
	TSEID	iLocalSEID;
	TSEID	iRemoteSEID;
	};

const TInt KAvdtpMaxInternalIndicationDataSize = 32;
	
NONSHARABLE_CLASS(TInternalAvdtpIndication)
/**
@internalTechnology
*/
	{
public:
	enum TInternalAvdtpIndicationType
		{
		EError,		// internal initiated avdtp error needs propagation to gavdp
		EAir,		// inbound signalling command needs propagation to gavdp
		EObject,	// internal object wishes to communicate with gavdp
		};
	TInternalAvdtpIndicationType iType;
	};
	
NONSHARABLE_CLASS(TInternalAvdtpErrorIndication) : public TInternalAvdtpIndication
	{
public:
	inline TInternalAvdtpErrorIndication();
public:
	TInt iError;
	};
	
NONSHARABLE_CLASS(TInternalAvdtpObjectIndication) : public TInternalAvdtpIndication
	{
public:
	enum TInternalAvdtpObjectIndicationType
		{
		ESignallingReady,
		};
public:
	TInternalAvdtpObjectIndicationType			iObjectIndication;
	TBuf8<KAvdtpMaxInternalIndicationDataSize>	iBuf;
protected:
	inline TInternalAvdtpObjectIndication();
	};
	
	
NONSHARABLE_CLASS(TInternalAvdtpAirIndication) : public TInternalAvdtpIndication
/*
@internalTechnology
*/
	{
public:
	inline TInternalAvdtpAirIndication();
public:
	TSEID			iSEID;
	TAvdtpMessage	iIndication;
	TInt			iPayloadLength;
	TAvdtpTransactionLabel		iTransactionLabel;
	};

/*
@internalTechnology
Used for SetConfiguration and Reconfigure
*/
NONSHARABLE_CLASS(TInternalAvdtpConfigurationIndication) : public TInternalAvdtpAirIndication
	{
public:
	TSEID	iINTSEID;
	};
	
NONSHARABLE_CLASS(TInternalAvdtpStreamReadyIndication) : public TInternalAvdtpAirIndication
/*
@internalTechnology
*/
	{
public:
	TInternalAvdtpStreamReadyIndication() 
		{
		iPayloadLength = 0;
		iIndication = EAvdtpOpen;
		};
	TBool			iReportingPresent;
	TBool			iRecoveryPresent;
	TSEID			iRemoteSEID;
	};
	

NONSHARABLE_CLASS(TInternalAvdtpSignallingReadyIndication) : public TInternalAvdtpObjectIndication
/*
@internalTechnology
*/
	{
public:
	TInternalAvdtpSignallingReadyIndication() 
		{
		iObjectIndication = ESignallingReady;
		};
	};

/**
@internalTechnology
Base struct for returning responses to indications from GAVDP to AVDTP
*/
struct TAvdtpInternalResponse
	{
	TSEID iSEID;
	TAvdtpTransactionLabel	iTransactionLabel;
	TInt					iResult;
	};
	

/**
@internalTechnology
Used to return from GC the result of a SetConfiguration or Reconfiguration indication
*/
struct TAvdtpInternalConfigurationResponse : public TAvdtpInternalResponse
	{
	TAvdtpServiceCategory	iCategory;
	TBool				iIsReconfigure;
	};
	
/*
@internalTechnology
Used for sending security control command
*/
struct TAvdtpInternalSendSecurityControl
	{
	TSEID						iRemoteSEID;
	TAvdtpSecurityControlInfo	iSecurityControlInfo;
	};


/*
@internalTechnology
Used for sending security control response
*/
struct TAvdtpInternalSecurityControlResponse : public TAvdtpInternalResponse
	{
	TAvdtpSecurityControlInfo	iSecurityControlInfo;
	};

/*
@internalTechnology
*/	
static const TInt KMaxAvdtpNumSEPs = 32;
	
/*
@internalTechnology
*/
struct TAvdtpInternalDiscoverConfirm
	{
	TInt iResult;
	TInt iNumSEPs;
	TPckgBuf<TFixedArray<TAvdtpSEPInfo, KMaxAvdtpNumSEPs> > iDiscoveredSEPs;
	};
	
/*
@internalTechnology
*/
struct TAvdtpInternalStartResponse : public TAvdtpInternalResponse
	{
	};


/*
@internalTechnology
*/
struct TAvdtpInternalSuspendResponse : public TAvdtpInternalResponse
	{
	};


// visitors for processing the config PDUs
NONSHARABLE_CLASS(CCapabilityVisitor) : public CBase
	{
public:
	/*
	Return ETrue if visitor believes it's not worth iterating further
	*/
	virtual TBool Capability(TAvdtpServiceCategory aCat)=0;

	// begin the whole shebang
	IMPORT_C void Process(TDes8& aCapabilityBuffer);
	
protected:
	// Constructor
	IMPORT_C CCapabilityVisitor();
	
	// Get a non-modifiable descriptor to the payload of the current capability
	// (excluding Service Category and LOSC)
	IMPORT_C const TDesC8& CapabilityPayload();
	
	// Get a modifiable descriptor to the current capability data
	// (including Service Category and LOSC)
	IMPORT_C TDes8& CapabilityDes();

private:
	TPtrC8 iCapabilityPayload;
	TPtr8 iCapabilityPtr;
	};

/*
@internalTechnology
*/
NONSHARABLE_CLASS(CCapabilityPresentVisitor) : public CCapabilityVisitor
	{
public:
	IMPORT_C CCapabilityPresentVisitor(TAvdtpServiceCategory aRequiredCapability);
	virtual TBool Capability(TAvdtpServiceCategory aCat);
	inline TBool IsPresent() const 
		{
		return iPresent;
		}
private:
	TAvdtpServiceCategory iRequiredCapability;	
	TBool	iPresent;
	};
	
/*
@internalTechnology
*/
typedef TFixedArray<TAvdtpServiceCapability*, ENumberOfServiceCategories> TCapabilitiesArray;
	

/*
@internalTechnology
*/
NONSHARABLE_CLASS(CCapabilityParseVisitor) : public CCapabilityVisitor
	{
public:
	IMPORT_C CCapabilityParseVisitor(TAvdtpServiceCategories aRequiredCategories);
	IMPORT_C TCapabilitiesArray GetCapabilities();
	virtual TBool Capability(TAvdtpServiceCategory aCat);
	virtual ~CCapabilityParseVisitor();
private:
	TCapabilitiesArray 			iCapabilities;
	TAvdtpServiceCategories		iRequiredCategories;
	};
		
/*
@internalTechnology
*/
NONSHARABLE_CLASS(CCapabilitySummaryVisitor) : public CCapabilityVisitor
	{
public:
	IMPORT_C CCapabilitySummaryVisitor();
	virtual TBool Capability(TAvdtpServiceCategory aCat);
	IMPORT_C TAvdtpServiceCategories CapabilitiesPresent() const;
private:
	TAvdtpServiceCategories iSeen;
	};

/*static*/ inline TInt AvdtpInternalUtils::NumberOfTransportObjects(TBool aReporting, TBool aRecovery)
	{
	TInt num = 1;
	num+= aReporting ? 1:0;
	num+= aRecovery ? 1:0;
	return num;
	}

class TAvdtpMultiplexingCapability : public TAvdtpServiceCapability
/*
@internalTechnology - used by bt_v2.prt and test code
*/
	{
public:
	inline TAvdtpMultiplexingCapability();
	IMPORT_C virtual TInt AsProtocol(RBuf8& aBuffer) const;	// write into buffer as per protocol
	// this implementation always can do mux and frag
	IMPORT_C virtual void Parse(const TDesC8& aDes);
	IMPORT_C void Reset();
public:
	TBool	iFrag;

	// cannot always know the semantics of each entry in the capability
	// so it's up to the user to consult with objects to find that
	// 0 will always be media
	// 1 is either reporting or recovery
	// 2 is always recovery if present
	// a helper class is provided - see below
	TFixedArray<TTSID,3> iSIDs;
	TFixedArray<TTCID,3> iCIDs;

private:
	enum TAvdtpMultiplexingCapabilityPanic
		{
		EInvalidCapabilityDataLength,
		};
	
private:
	inline void Panic(TAvdtpMultiplexingCapabilityPanic aPanic);
	};

inline void TAvdtpMultiplexingCapability::Panic(TAvdtpMultiplexingCapabilityPanic aPanic)
	{
	_LIT(KAvdtpMuxHelperPanic, "TAvdtpMultiplexingCapabilityPanic");
	User::Panic(KAvdtpMuxHelperPanic, aPanic);
	}

inline TAvdtpMultiplexingCapability::TAvdtpMultiplexingCapability()
: TAvdtpServiceCapability(EServiceCategoryMultiplexing)
  	{
  	// have to set packet length later for this capability
  	Reset();
 	}

/**
Helps getting/setting the right TCID and TSID in the capability
@see TAvdtpMultiplexingCapability
*/
NONSHARABLE_CLASS(TAvdtpMultiplexingCapabilityHelper)
	{
public:
	inline TAvdtpMultiplexingCapabilityHelper(TAvdtpMultiplexingCapability& aMuxCap,
										TBool aRequireReporting,
										TBool aRequireRecovery);
	inline TTCID MediaCID() const;
	inline TTSID MediaSID() const;
	inline TTCID ReportingCID() const;
	inline TTSID ReportingSID() const;
	inline TTCID RecoveryCID() const;
	inline TTSID RecoverySID() const;
	inline void SetMediaCID(TTCID aTCID);
	inline void SetMediaSID(TTSID aTSID);
	inline void SetReportingCID(TTCID aTCID);
	inline void SetReportingSID(TTSID aTSID);
	inline void SetRecoveryCID(TTCID aTCID);
	inline void SetRecoverySID(TTSID aTSID);
											
private:
	TAvdtpMultiplexingCapability& iMuxCap;
	const TInt iMediaIndex;
	const TInt iReportingIndex;
	TInt iRecoveryIndex;
#ifdef _DEBUG
	TBool iRequireReporting;
	TBool iRequireRecovery;
	enum TAvdtpMuxHelperPanic
		{
		EAvdtpMuxHelperWronglyUsed,
		};
	inline void Panic(TAvdtpMuxHelperPanic aPanic);
#endif
	};

/**
Class used for communicating reporting and recovery configurations between Gavdp and Avdtp
@internalTechnology
*/
NONSHARABLE_CLASS(TReportingAndRecovery) 
	{
public:
	TSEID iSEID;
	TBool iReporting;
	TBool iRecovery;
	};

const TInt KAvdtpMaxInternalIndicationSize = sizeof(TInternalAvdtpObjectIndication);


inline TAvdtpMultiplexingCapabilityHelper::TAvdtpMultiplexingCapabilityHelper(
										TAvdtpMultiplexingCapability& aMuxCap,
										TBool aRequireReporting,
										TBool aRequireRecovery)
: iMuxCap(aMuxCap), iMediaIndex(0), iReportingIndex(1)
	{
#ifdef _DEBUG
	// to assert against later
	iRequireReporting = aRequireReporting;
	iRequireRecovery = aRequireRecovery;
#endif
	iRecoveryIndex = aRequireReporting & aRequireRecovery ? 2 : 1;
	}

inline TTCID TAvdtpMultiplexingCapabilityHelper::MediaCID() const
	{
	return iMuxCap.iCIDs[iMediaIndex];	
	}

inline TTSID TAvdtpMultiplexingCapabilityHelper::MediaSID() const
	{
	return iMuxCap.iSIDs[iMediaIndex];	
	}

inline TTCID TAvdtpMultiplexingCapabilityHelper::ReportingCID() const
	{
	return iMuxCap.iCIDs[iReportingIndex];	
	}

inline TTSID TAvdtpMultiplexingCapabilityHelper::ReportingSID() const
	{
	return iMuxCap.iSIDs[iReportingIndex];		
	}

inline TTCID TAvdtpMultiplexingCapabilityHelper::RecoveryCID() const
	{
	return iMuxCap.iCIDs[iRecoveryIndex];		
	}

inline TTSID TAvdtpMultiplexingCapabilityHelper::RecoverySID() const
	{
	return iMuxCap.iSIDs[iRecoveryIndex];
	}

inline void TAvdtpMultiplexingCapabilityHelper::SetMediaCID(TTCID aTCID)
	{
	iMuxCap.iCIDs[iMediaIndex] = aTCID;
	}

inline void TAvdtpMultiplexingCapabilityHelper::SetMediaSID(TTSID aTSID)
	{
	iMuxCap.iSIDs[iMediaIndex] = aTSID;	
	}

inline void TAvdtpMultiplexingCapabilityHelper::SetReportingCID(TTCID aTCID)
	{
	__ASSERT_DEBUG(iRequireReporting, Panic(EAvdtpMuxHelperWronglyUsed));
	iMuxCap.iCIDs[iReportingIndex] = aTCID;
	}

inline void TAvdtpMultiplexingCapabilityHelper::SetReportingSID(TTSID aTSID)
	{
	__ASSERT_DEBUG(iRequireReporting, Panic(EAvdtpMuxHelperWronglyUsed));
	iMuxCap.iSIDs[iReportingIndex] = aTSID;	
	}

inline void TAvdtpMultiplexingCapabilityHelper::SetRecoveryCID(TTCID aTCID)
	{
	__ASSERT_DEBUG(iRequireReporting, Panic(EAvdtpMuxHelperWronglyUsed));
	iMuxCap.iCIDs[iRecoveryIndex] = aTCID;	
	}

inline void TAvdtpMultiplexingCapabilityHelper::SetRecoverySID(TTSID aTSID)
	{
	__ASSERT_DEBUG(iRequireReporting, Panic(EAvdtpMuxHelperWronglyUsed));
	iMuxCap.iSIDs[iRecoveryIndex] = aTSID;	
	}


inline TInternalAvdtpErrorIndication::TInternalAvdtpErrorIndication()
 	{
 	iType = TInternalAvdtpIndication::EError;
 	}

inline TInternalAvdtpAirIndication::TInternalAvdtpAirIndication()
 	{
	iType = TInternalAvdtpIndication::EAir;
 	}

inline TInternalAvdtpObjectIndication::TInternalAvdtpObjectIndication()
 	{
	iType = TInternalAvdtpIndication::EObject;
 	}

#ifdef _DEBUG
inline void TAvdtpMultiplexingCapabilityHelper::Panic(TAvdtpMuxHelperPanic aPanic)
	{
	_LIT(KAvdtpMuxHelperPanic, "AVDTPMuxHelperPanic");
	User::Panic(KAvdtpMuxHelperPanic, aPanic);
	}
#endif

inline TAvdtpSignallingErrorCode AvdtpInternalUtils::SymbianErrorToAvdtpError(TInt aSymbianError)
	{
	return aSymbianError == KErrNotFound ? EAvdtpBadACPSEID :
				static_cast<TAvdtpSignallingErrorCode>(KErrAvdtpSignallingErrorBase-aSymbianError);
	}

inline void AvdtpInternalUtils::PacketErrorLeaveL(TAvdtpSignallingErrorCode aCode)
	{
	User::Leave(ConvertToSymbianError::AvdtpError(aCode));
	}

#endif
