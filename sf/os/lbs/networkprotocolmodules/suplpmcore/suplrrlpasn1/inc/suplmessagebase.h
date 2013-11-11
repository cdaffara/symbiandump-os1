// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef LBS_SUPL_MESSAGE_BASE_H
#define LBS_SUPL_MESSAGE_BASE_H

#include "suplmessagecommon.h"

// forward declarations
class TLbsNetPosCapabilities;
class TPositionInfoBase;
class TCourse;
class ASN1T_ULP_PDU;
class ASN1C_ULP_PDU;
class ASN1PEREncodeBuffer;
class ASN1T_SETCapabilities;
class ASN1T_Position;
class ASN1T_LocationId;
class ASN1T_Velocity;


/**
CSuplMessageBase is an abstract class providing a base definition for SUPL
messages either received or constructed for sending. Concrete classes derived 
from this class represent each of the supported SUPL message types, and provide
methods for accessing and setting their member parameters.

When building outgoing messages, the constructor must be called with 
aIsOutgoingMessage set to ETrue. This results in the internal data members 
necessary for building and encoding an outgoing message.

For received messages, the SUPL ASN1 Decoder object (see csuplasn1decoder.h)
calls SetDecodedData(), passing the decoded data structures (ownership is passed
immediately on entrance to this method). Note that these data structures are 
defined by compilation of the SUPL definition using a third party ASN1 compiler
and therefore do not conform to standard Symbian naming conventions. 

Types encapsulating common parts for operation within the SUPL Protocol Module
are described in file suplmessagecommon.h

@internalTechnology
*/
NONSHARABLE_CLASS(CSuplMessageBase) : public CBase
	{
public:
	
	/** TSuplMessageType typedef */
	typedef TUint32 TSuplMessageType;

	enum _TSuplMessageType
		{	
	  	ESuplInit = 0,
	  	ESuplStart,
	  	ESuplResponse,
	  	ESuplPosInit,
	  	ESuplPos,
	  	ESuplEnd,
	  	ESuplAuthReq,
	  	ESuplAuthResp,
	  	
	  	ESuplMessageLimit	
		};
		
protected:
	CSuplMessageBase(TSuplMessageType aType, TBool aIsOutgoingMessage);

public:
	virtual ~CSuplMessageBase();
	
public:
	/** Encode a populated outgoing message */
	IMPORT_C TInt EncodeToL(TPtr8& aBuf);
	
	/** Methods for populating common parameters for outgoing messages */

	/** Set the SUPL version used (for outgoing messages) */
	IMPORT_C void SetVersion(CSuplVersion& aVersion);

	/** Set the SUPL Session ID (for outgoing messages) */
	IMPORT_C TInt SetSessionId(CSuplSessionId& aSessionId);
	
	/** Methods for accessing content of decoded received messages */
	
	/** Returns the message type */
	IMPORT_C CSuplMessageBase::TSuplMessageType MessageType();

	/** Populates aVersion with the SUPL version (for received messages) */
	IMPORT_C TInt GetVersion(CSuplVersion& aVersion);

	/** Populates aSessionId with the SUPL Session ID (for received messages) */
	IMPORT_C TInt GetSessionId(CSuplSessionId& aSessionId);
	
	/** Prints the content of the data structure to the logger */
	IMPORT_C void LogMessageContent();

public:
	/** Assign decoded ASN1 data. (for received messages) 
	    Not intended for DLL export */
	void SetDecodedData(ASN1T_ULP_PDU* aData, ASN1C_ULP_PDU* aControl);

private:
	/** Prohibit copy constructor */
	CSuplMessageBase(const CSuplMessageBase&);

	/** Prohibit assigment operator */
	CSuplMessageBase& operator= (const CSuplMessageBase&);
	
protected:
	/** second stage constructor */
	void ConstructL();

	/** checks for memory allocation failure and leaves */
	void LeaveIfAllocErrorL();
	
	/** translates ASN1 run-time errors */
	TInt ProcessAsn1Error(TInt aError);
	
	/** methods for outgoing SUPL parameters common to multiple messages */
	TInt PopulateSetCapabilities(const TLbsNetPosCapabilities& aCapsSource, ASN1T_SETCapabilities& aCapsTarget);
	TInt PopulateLocationId(const CSuplLocationId& aLocSource, ASN1T_LocationId& aLocTarget);
	TInt PopulatePosition(const TPositionInfoBase& aPosSource, ASN1T_Position& aPosTarget);
	TInt PopulateVelocity(const TCourse& aCourse, ASN1T_Velocity& aVelTarget);
	
	/** common calculation methods */
	TInt Uncertainty(const TReal32& aDistance);
	TInt UncertaintyAltitude(const TReal32& aDistance);
	TInt EncodeAltitude(const TReal32& aAltitude);

protected:
	// Message type identifier
	TSuplMessageType iSuplMessageType;
	
	TBool iIsOutgoingMessage;

	// SUPL message data structure and C++ management class for the data.
	ASN1T_ULP_PDU* iData;
	ASN1C_ULP_PDU* iControl;		

	// Buffer management object for encoding ASN.1 PER data stream
	ASN1PEREncodeBuffer* iEncodeBuffer;
	
	// Buffer for storing timestamp translated to UTC format YYMMDDhhmmssZ
	TBuf8<13> iUtcTime; 
	};

#endif // LBS_SUPL_MESSAGE_BASE_H
