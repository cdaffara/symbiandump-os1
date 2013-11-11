// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file UMTSNifControlIf.h
// Header file for UMTS-NIF Control API
//

#ifndef __UMTSNIF_CONTROL__
#define __UMTSNIF_CONTROL__

#include <nifman.h>
#include <es_prot.h>
#if MM_ETEL_API
#include "uscl_packet.h"	// RPacketContext from UMTSSim
#include "uscl_qos.h"		// RPacketQoS from UMTSSim
#else
#include "etelpckt.h"		// 
#include "etelqos.h"		// 
#endif

const TUint KMaxNumberOfPacketFilterElements = 5;		// From UMTS standards
const TUint KMaxNumberOfPacketFilters = 8;				// From UMTS standards

/**
 *
 * @publishedPartner
 * @released
 */
class MNifEvent 
{
public:
	virtual TInt Event(CProtocolBase *aProtocol, TUint aEvent, TDes8& aOption, TAny* aSource=0) = 0;
};

/**
 *
 * @publishedPartner
 * @released
 */
class TEvent 
{
public:
	MNifEvent  *iEvent;
};




// List of filters for TTFTInfo
typedef TSglQue<RPacketContext::TPacketFilterV2> FilterQueue;
typedef TSglQueIter<RPacketContext::TPacketFilterV2> FilterQueueIterator;

/**
 * Class to handle one Traffic Flow Template
 * Includes 1 - 8 Filters
 *
 * @publishedPartner
 * @released
 */
class TTFTInfo
{
public :	

	IMPORT_C explicit TTFTInfo();
	IMPORT_C ~TTFTInfo();
	IMPORT_C TInt Set(const TTFTInfo& aTFTInfo);
	IMPORT_C TInt AddPacketFilter(RPacketContext::TPacketFilterV2 aFilter);
	IMPORT_C TInt GetPacketFilter(RPacketContext::TPacketFilterV2& aFilter);
	IMPORT_C TInt RemovePacketFilter(RPacketContext::TPacketFilterV2 aFilter);
	IMPORT_C TInt NextPacketFilter(RPacketContext::TPacketFilterV2& aFilter);
	IMPORT_C void SetToFirst();
	IMPORT_C TUint8 FilterCount();
	

    IMPORT_C TTFTInfo& operator=(const TTFTInfo& aTFTInfo);
	// Additional Functions for the SBLP Params Handling
    IMPORT_C TInt AddSblpToken ( RPacketContext::TAuthorizationToken aAuthorizationToken,
                                    RArray<RPacketContext::CTFTMediaAuthorizationV3::TFlowIdentifier> aFlowIds);
	IMPORT_C TInt GetSblpToken ( RPacketContext::CTFTMediaAuthorizationV3 &aSblpParams ) const;
	IMPORT_C TInt RemovSblpToken ();

#ifdef SYMBIAN_NETWORKING_UMTSR5	
	IMPORT_C TInt SetIMCNSubsystemflag(TBool aIMCNSubsystemflag);
	IMPORT_C TInt GetIMCNSubsystemflag(TBool &aIMCNSubsystemflag) const;
#endif
		
private :	
	TUint8 iFilterCount; 
	TUint8 iLastReturnedFilter;	
	RPacketContext::TPacketFilterV2 iPacketFilters[KMaxNumberOfPacketFilters];
	TBool iFilterSlots[KMaxNumberOfPacketFilters];
	// Adding ETel Member for Holding the SBLP Parameters
    RPacketContext::CTFTMediaAuthorizationV3 *iSblpParams;
#ifdef SYMBIAN_NETWORKING_UMTSR5
    TBool iIMCNSubsystemflag;
#endif    
};

//
// Type of the context. Mainly used for upkeeping and consistency checking.
//
enum TContextType
	{
	EPrimaryContext,			// Primary context
	ESecondaryContext,			// Secondary context
	ENetworkInitiatedContext,	// Initiated from network
	EContextTypeUnknown			// Default. Used to catch errors. This should be filled by the Nif when returning context info
	};
//
// Type: context identifier
//
typedef TInt8 TContextId;					// Nif's representation 

/**
 * Class contains all configuration info of a single context
 * - Configuration:	RPacketContext::TContextConfigGPRS
 * - Requested QoS:	RPacketQoS::TQoSR99_R4Requested
 * - Negotiated QoS:	RPacketQoS::TQoSR99_R4Negotiated
 * - TFT:				TTFTInfo
 *
 * @publishedPartner
 * @released
 */
class TContextConfig
{	
public:
	IMPORT_C ~TContextConfig();

	IMPORT_C TInt Set(const TContextConfig& aConfig);
	IMPORT_C void Reset();
	IMPORT_C TContextConfig();

	// Set-methods: Used to set the corresponding member variable
	IMPORT_C TInt SetTFTInfo(const TTFTInfo& aTFTInfo);	
	IMPORT_C TInt SetUMTSQoSReq(const RPacketQoS::TQoSR99_R4Requested& aUMTSQoSRequest);
	IMPORT_C TInt SetUMTSQoSNeg(const RPacketQoS::TQoSR99_R4Negotiated& aUMTSQoS);
#ifdef SYMBIAN_NETWORKING_UMTSR5	
    IMPORT_C TInt SetUMTSQoSReq(const RPacketQoS::TQoSR5Requested& aR5QoSReq);
	IMPORT_C TInt SetUMTSQoSNeg(const RPacketQoS::TQoSR5Negotiated& aR5QoSNeg);
#endif	
	IMPORT_C TInt SetContextConfig(const RPacketContext::TContextConfigGPRS& aUMTSConfig);
	IMPORT_C TInt SetPdpCompression(TUint aPdpCompression);
	IMPORT_C TInt SetPdpType(RPacketContext::TProtocolType aPdpType);
	IMPORT_C TInt SetAccessPointName(const RPacketContext::TGSNAddress& aAccessPointName);
	IMPORT_C TInt SetPdpAddress(const RPacketContext::TProtocolAddress& aPdpAddress);


	// Get-methods: Used to fetch the corresponding member variable
	IMPORT_C TInt GetUMTSQoSReq(RPacketQoS::TQoSR99_R4Requested& aUMTSQoSRequest)	const;
	IMPORT_C TInt GetTFTInfo(TTFTInfo& aTFTInfo)									const;
	IMPORT_C TInt GetUMTSQoSNeg(RPacketQoS::TQoSR99_R4Negotiated& aUMTSQoS)			const;
	IMPORT_C TInt GetContextConfig(RPacketContext::TContextConfigGPRS& aUMTSConfig) const;
	IMPORT_C TInt GetPdpCompression(TUint& aPdpCompression)							const;
	IMPORT_C TInt GetPdpType(RPacketContext::TProtocolType& aPdpType)				const;
	IMPORT_C TInt GetAccessPointName(RPacketContext::TGSNAddress& aAccessPointName) const;
	IMPORT_C TInt GetPdpAddress(RPacketContext::TProtocolAddress& aPdpAddress)		const;
#ifdef SYMBIAN_NETWORKING_UMTSR5
	IMPORT_C TInt GetUMTSQoSReq(RPacketQoS::TQoSR5Requested& aR5QoSReq) const;
	IMPORT_C TInt GetUMTSQoSNeg(RPacketQoS::TQoSR5Negotiated& aR5QoSNeg) const;	
#endif    	

protected: 
	RPacketContext::TContextConfigGPRS	iContextConfig;		// Configuration
#ifdef SYMBIAN_NETWORKING_UMTSR5
	RPacketQoS::TQoSR5Requested iUMTSQoSReq;              // Requested R5 QoS
	RPacketQoS::TQoSR5Negotiated iUMTSQoS;             // Negotiated R5 QoS
#else
	RPacketQoS::TQoSR99_R4Requested iUMTSQoSReq;              // Requested R99/R4 QoS
	RPacketQoS::TQoSR99_R4Negotiated iUMTSQoS;             // Negotiated R99/R4 QoS
#endif	
 	TTFTInfo							iTFTInfo;			// Traffic Flow Template
};


//
// Operation result codes. Removed, now using standard codes + Etel codes
//
//typedef TInt TContextReasonCode;
//const TInt KContextOk						=  0x00;	// OK	KErrNone
//const TInt KErrContextBusy					= -0x03;	// Another operation pending KErrInUse
//const TInt KErrContextInvalidId				= -0x05;	// Context id out of range: FATAL KErrBadHandle
//const TInt KErrContextUnusedId				= -0x06;	// Context with given ID missing KErrNotReady
//const TInt KErrContextCreateFailed			= -0x07;	// Error while creating context // Not needed
//const TInt KErrMaxNumberOfContextsReached	= -0x0a;	// Device configuration does not support more contexts //KErrNotSupported
//const TInt KErrMemory						= -0x0b;	// General out of memory error //KErrNoMemory
//const TInt KErrContextStatus				= -0x0c;	// Context status/requested operation mismatch
//const TInt KErrContextGeneral				= -0x0d;	// General error //KErrGeneral

/**
 * Single Context's identification info
 *
 * @publishedPartner
 * @released
 */
class TContextInfo
{
public:
	TContextId iContextId;					// Context id in Nif
	RPacketContext::TContextStatus iStatus;	// The current status from Etel
};


//
// Events the Nif sends to upper layers
//
const TUint KContextDeleteEvent				= 0x01;	// When deletion is initiated from network.
const TUint KContextActivateEvent 			= 0x02;	// Response to KContextActivate
const TUint KContextParametersChangeEvent	= 0x03;	// Contexts parameters/status have changed.
const TUint KContextBlockedEvent			= 0x04;	// A PDP context becomes blocked
const TUint KContextUnblockedEvent			= 0x05;	// A PDP context becomes unblocked
const TUint KNetworkStatusEvent				= 0x06;	// For different network indications	
const TUint KContextQoSSetEvent				= 0x07;	// Response to KContextQoSSet
const TUint KContextTFTModifiedEvent		= 0x08;	// Response to KContextTFTModify
const TUint KPrimaryContextCreated			= 0x09;	// Primary context at startup created
const TUint KSecondaryContextCreated		= 0x0a;	// Secondary context created
const TUint KContextModifyActiveEvent		= 0x0b; // Response to KModifyActive
const TUint KGetNegQoSEvent                 = 0x0c; // Response to KGetNegQoS. ** Used internally by NIF **


//
// Control operations used to signal the UMTSNif
//
const TUint KContextCreate					= 0x01;	// Create a new context
const TUint KContextDelete					= 0x02;	// Delete an existing context
const TUint KContextActivate				= 0x03;	// Activate an existing context	
const TUint KRegisterEventHandler			= 0x04;	// Option to enable event method registration
const TUint KContextSetEvents				= 0x05;	// Option to enable / disable event notification to upper layers
const TUint KContextQoSSet					= 0x06;	// Set QoS
const TUint KContextModifyActive			= 0x07;	// Modify an active context
const TUint KContextTFTModify				= 0x08;	// Traffic Flow Template modification
const TUint KNifSetDefaultQoS				= 0x09;	// Set the QoS of the primary context created at startup
const TUint KStartupPrimaryContextCreation	= 0x0a;	// Standalone pdp context creation. ** Used internally by Nif **	
const TUint KGetNegQoS                      = 0x0b; // Retrieve negotiated QoS from the TSY. ** Used internally by NIF **

//
// Codes to identify requested TFT-operation when issuing KContextTFTModify-command
//
typedef TInt TTFTOperationCode;	
const TInt KAddFilters		    = 0x01;		// Add packet filters(s)
const TInt KRemoveFilters	    = 0x02;		// Remove packet filters(s)
const TInt KAddSblpParameter	= 0x03;		// Add sblp parameter
const TInt KRemoveSblpParameter	= 0x04;		// Remove sblp parameter
const TInt KDeleteTFT		    = 0x05;		// Delete the Traffic Flow Template
#ifdef SYMBIAN_NETWORKING_UMTSR5
const TInt KAddIMCNSubsystemflag	= 0x06;		// Add IM CN Subsystem flag
const TInt KRemoveIMCNSubsystemflag	= 0x07;		// Remove IM CN Subsystem flag
#endif

/**
 * Structure used when passing context related parameters to/from the Nif
 *
 * @publishedPartner
 * @released
 */
class TContextParameters 
{
public:
	IMPORT_C TContextParameters();
	TContextType iContextType;			// UP:		Primary / Secondary
	TContextInfo iContextInfo;			// UP&DOWN: Context Id and status
    TContextConfig iContextConfig;		// UP&DOWN:	QoS, TFT and configuration
	TTFTOperationCode iTFTOperationCode;// DOWN:	Indicating requested TFT operation
	TInt iReasonCode;					// UP:		Operation successful/failed
};

//
// List of Network status codes 
// These are used to signal the upper layers of status changes
//
typedef TUint TNetworkEventCode;
const TUint KNetworkConnectionLost	= 0x01;	// No connection available
const TUint KNetworkInterfaceDown	= 0x02;	// Nif is going down -> Do not access it anymore

/**
 *
 * @publishedPartner
 * @released
 */
class TNetworkParameters
{
public:
	TNetworkEventCode iNetworkEventCode;	// Event code indicating what happened
	RPacketService::TStatus iNetworkStatus;	// Unattached, Attached, Active, Suspended
};

#endif // __UMTSNIF_CONTROL__
