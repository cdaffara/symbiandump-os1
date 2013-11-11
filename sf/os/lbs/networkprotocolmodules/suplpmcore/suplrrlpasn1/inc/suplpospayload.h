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
#ifndef LBS_SUPL_POS_PAYLOAD_H
#define LBS_SUPL_POS_PAYLOAD_H

#include <e32base.h>


/**
CSuplPosPayload is an abstract class providing a base definition for SUPL positioning
payload messages either received or under construction for sending. SUPL supports
positioning using the RRLP, TIA-801 and RRC protocols. This reference SUPL 
Protocol Manager implementation supports only the RRLP positioning protocol.

Concrete classes derived from this class represent each of the supported 
positioning protocol message types, and provide methods for accessing and setting
their parameters.

@internalTechnology
*/
NONSHARABLE_CLASS(CSuplPosPayload) : public CBase
	{
public:

	/** Pos Payload identifiers	*/
	typedef TUint32 TSuplPosPayloadType;
	enum _TSuplPosPayloadType
		{	
	  	ERrlpAssistanceData = 0,
	  	ERrlpAssistanceDataAck,
	  	ERrlpMeasurePositionReq,
	  	ERrlpMeasurePositionResp,
	  	ERrlpProtocolError,
	  	
	  	EPosPayloadMessageTypeLimit
		};
		
protected:
	CSuplPosPayload(TSuplPosPayloadType aType, TBool aIsOutgoingMsg);

public:
	virtual ~CSuplPosPayload();
	
public:

	/** Returns the message type */
	IMPORT_C CSuplPosPayload::TSuplPosPayloadType MessageType();

	/** Decodes a pos payload and returns a CSuplPosPayload object containing the decoded payload */
	IMPORT_C static CSuplPosPayload* DecodePosPayloadL(const HBufC8* aPosPayload, TInt& aError);
	
	/** Encode a populated outgoing message */
	virtual TInt EncodeToL(TPtr8& aBuf);
	
	/** Prints the content of the data structure to the logger */
	virtual void LogMessageContent();

private:

	/** Prohibit copy constructor */
	CSuplPosPayload(const CSuplPosPayload&);

	/** Prohibit assigment operator */
	CSuplPosPayload& operator= (const CSuplPosPayload&);

protected:

	/** Message type identifiers */
	TSuplPosPayloadType iMessageType;
	TBool iIsOutgoingMessage;
	};

#endif // LBS_SUPL_POS_PAYLOAD_H
