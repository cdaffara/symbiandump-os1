// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @internalTechnology
 
*/
#ifndef RRLP_MESSAGE_BASE_H
#define RRLP_MESSAGE_BASE_H

#include "suplpospayload.h"
#include <lbs/lbsassistancedatabuilderset.h>
#include <lbs/lbsassistancereferencelocation.h>
#include <lbs/lbsassistancereferencetime.h>
#include <lbs/lbsassistancenavigationmodel.h>
#include <lbs/lbsassistanceionosphericmodel.h>
#include <lbs/lbsassistanceutcmodel.h>
#include <lbs/lbsassistancealmanac.h>
#include <lbs/lbsassistanceaquisitionassistance.h>
#include <lbs/lbsassistancebadsatlist.h>

// forward declarations
class ASN1T_PDU;
class ASN1C_PDU;
class ASN1PEREncodeBuffer;
class ASN1T_ControlHeader;
class ASN1T_NavModelElement;
class ASN1T_UncompressedEphemeris;
class ASN1T_AlmanacElement;
class ASN1T_AcquisElement;

/** 
Highest valid value for RRLP Reference Number session identifier

@internalTechnology
*/
const TInt KMaxRrlpReferenceNumber = 7;


/** 
Constant for adjustment for zero offset differences.

For ASN1 "CHOICE" types, the ASN1 compiler generates type defines with a +1
offset to more easily access union elements describing the choice content.
This is not an issue programatically, however must be adjusted when the CHOICE
type is directly stored as a datum, for example SatStatus in the Navigation
Model assistance data element.

@internalTechnology
*/
const TInt EZeroOffsetAdjust = 1;

/**
RRLP Session Reference.

Extended in RRLP Release 5 to contain optional extension parameters.
These are populated if and only if the initial message sent by the SLP
contains the extension parameters.

@internalTechnology
*/
NONSHARABLE_CLASS(TRrlpReference)
	{
	public:
	TInt  aRefNum;            // 0..7
	TBool aRel5EntendedRefPresent;
	TInt  aRel5SmlcCode;      // 0..63
	TInt  aRel5TransactionId; // 0..262143
	};


/**
RRLP Message Base Class

@internalTechnology

*/
NONSHARABLE_CLASS(CRrlpMessageBase) : public CSuplPosPayload
	{
protected:
	CRrlpMessageBase(TSuplPosPayloadType aType, TBool aIsOutgoingMsg);
	void ConstructL();

public:
	virtual ~CRrlpMessageBase();
	
public:
	/** Set the reference component (for outgoing messages) */
	IMPORT_C TInt SetReference(const TRrlpReference& aRrlpRef);
	
	/** Populates aRrlpRef with the received reference component (for received messages) */
	IMPORT_C TInt GetReference(TRrlpReference& aRrlpRef);
	
	/** Encode a populated (outgoing) RRLP message */
	IMPORT_C TInt EncodeToL(TPtr8& aBuf);	
	
public:
	/** Assign decoded ASN1 data, for received messages. 
	    Not intended for DLL export */
	void SetDecodedData(ASN1T_PDU* aData, ASN1C_PDU* aControl);
	
	/** Prints the content payload data structure to the logger */
	void LogMessageContent();

private:
	/** Prohibit copy constructor */
	CRrlpMessageBase(const CRrlpMessageBase&);

	/** Prohibit assigment operator */
	CRrlpMessageBase& operator= (const CRrlpMessageBase&);

protected:
	/** Methods for delivering received assistance data */
	TInt BuildAssistanceData(TLbsAsistanceDataGroup& aDataMask, RLbsAssistanceDataBuilderSet& aAssistanceDataBuilderSet, ASN1T_ControlHeader& aGpsData);
	void WriteReferenceLocationData(RReferenceLocationBuilder* aReferenceLocationBuilder, ASN1T_ControlHeader& aGpsData);
	void WriteReferenceTimeData(RUEPositioningGpsReferenceTimeBuilder* aReferenceTimeBuilder, ASN1T_ControlHeader& aGpsData);
	void WriteNavigationModelData(RUEPositioningGpsNavigationModelBuilder* aGpsNavigationModelBuilder, ASN1T_ControlHeader& aGpsData);
	void WriteNavigationModelSatInfo(RNavigationModelSatInfoBuilder* aNavigationModelSatInfoBuilder, ASN1T_NavModelElement& aSatInfo);
	void WriteSatInfoEphemeris(RNavigationModelSatInfoBuilder* aNavigationModelSatInfoBuilder, ASN1T_UncompressedEphemeris& aSatEphemeris);
	void WriteIonosphericModelData(RUEPositioningGpsIonosphericModelBuilder* aGpsIonosphericModelBuilder, ASN1T_ControlHeader& aGpsData);
	void WriteUtcModelData(RUEPositioningGpsUtcModelBuilder* aGpsUtcModelBuilder, ASN1T_ControlHeader& aGpsData);
	void WriteAlmanacData(RUEPositioningGpsAlmanacBuilder* aGpsAlmanacBuilder, ASN1T_ControlHeader& aGpsData);
	void WriteAlmanacSatInfo(RAlmanacSatInfoBuilder* aAlmanacSatInfoBuilder, ASN1T_AlmanacElement& aAlmanacElement);
	void WriteAcquisitionAssistanceData(RUEPositioningGpsAcquisitionAssistanceBuilder* aGpsAcquisitionAssistanceBuilder, ASN1T_ControlHeader& aGpsData);
	void WriteAcquisAssistSatInfo(RAcquisitionSatInfoBuilder* aAcquisitionSatInfoBuilder, ASN1T_AcquisElement& aAcquisAssistElement);
	void WriteRealTimeIntegrityData(RBadSatListBuilder* aBadSatListBuilder, ASN1T_ControlHeader& aGpsData);
	
	/** virtual methods for setting/getting extended reference fields
	    Default implementation is empty */
	virtual TInt SetExtendedReference(const TRrlpReference& aRrlpRef);
	virtual TInt GetExtendedReference(TRrlpReference& aRrlpRef);
	
	/** Checks control structure for error, and leaves if one has occurred */
	void LeaveIfAllocErrorL();
	
	/** Translates errors returned by the ASN1 runtime library */
	TInt ProcessAsn1Error(TInt aError);

protected:

	/** Message data content */
	ASN1T_PDU* iData;
	ASN1C_PDU* iControl;

	// Buffer management object for encoding ASN.1 PER data stream
	ASN1PEREncodeBuffer* iEncodeBuffer;

	};

#endif //RRLP_MESSAGE_BASE_H


