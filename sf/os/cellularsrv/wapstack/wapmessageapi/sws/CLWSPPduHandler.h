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
//

#ifndef __CLWSPPDUHANDLER_H__
#define __CLWSPPDUHANDLER_H__
#include <e32std.h>
#include <wapmessage.h>


/** 
The connectionless WSP PDU type definition.
@internalComponent
*/
enum TWSPPduType
	{
	/** the Reply PDU type assignment 
	*/
	EReply			=0x04,
	/** the Push PDU type assignment 
	*/
	EPush			=0x06,
	/** the Get PDU type assignment 
	*/
	EGet			=0x40,
	/** the GetOption PDU type assignment 
	*/
	EGetOptions		=0x41,
	/** the Gethead PDU type assignment 
	*/
	EGetHead		=0x42,
	/** the GetDelete PDU type assignment 
	*/
	EGetDelete		=0x43,
	/** the GetTrace PDU type assignment 
	*/
	EGetTrace		=0x44,
	/** the Post PDU type assignment 
	*/
	EPost			=0x60,
	/** the PostPut PDU type assignment 
	*/
	EPostPut		=0x61
	};


	/** 
	The offset of the transactionID field in WSP PDU.
	@internalComponent
	 */
	const TUint KPDUHeaderTransactionIDOffSet	=0;
	/** 
	The offset of the WSP PDU type field in WSP PDU.
	@internalComponent
	 */
	const TUint KPDUHeaderWSPPDUTypeOffSet		=1;
	/** 
	The offset of WSP field.
	@internalComponent
	*/
	const TUint KPDUFieldOffset					=2;

	/** 
	The length of transactionID+PDUType
	@internalComponent
	*/
	const TUint8 KPDUTransactionIDAndPDUTypeLength	=2;
	/** 
	The length of WSP status
	@internalComponent
	*/
	const TUint8 KWSPStatusLength					=1;

	/**
	The mask of top 3 bits in varible length Unsigned integer
	@internalComponent
	*/
	const TUint8 KTop3BitSet		= 0x70;

	/**
	The mask to valide each byte in varible length Unsigned integer
	@internalComponent
	*/
	const TUint8 KWapQuote			= 0x7F;

	/**
	The mask of carry bit in each byte in varible length Unsigned integer
	@internalComponent
	*/
	const TUint8 KCarryBitMask		= 0x80;

	/**
	Maximum length for varible length Unsigned integer
	@internalComponent
	*/
	const TUint8 KMaxUintVarLength	= 5;

	/**
	shift 7 bits for each byte in Uint32
	@internalComponent
	*/
	const TUint8 KUIntVarOctetShift	= 7;



class CCLWSPPduHandler
/** 
The Connectionless WSP PDU Handler. This class provides a set of static interfaces 
for WAP message APIs to pack and unpack the WSP PDU.   
For error codes returned by methods in this class, see <wapmsgerr.h>.  Most methods can 
return a set of general errors, with some returning additional specific errors.
*/
	{
public: 



public: // Public Interfaces
	static void UnpackWSPPduL(HBufC8* aWSPPdu, TWSPPduType& aType, HBufC8*& aWSPHeader, HBufC8*& aBody, TUint8& aId, TWSPStatus& aStatus);
	static void PackWSPPduL(HBufC8*& aWSPPdu, TWSPPduType aType, const TDesC& aURI, const TDesC8& aWSPHeader, const TDesC8& aBody, const TUint8 aId);

private: // private methods
	static void UnpackPushPduL(HBufC8* aWSPPdu, HBufC8*& aWSPHeader, HBufC8*& aBody);
	static void UnpackReplyPduL(HBufC8* aWSPPdu, HBufC8*& aWSPHeader, HBufC8*& aBody, TWSPStatus& aStatus);
	static void PackPostPduL(HBufC8*& aWSPPdu, TWSPPduType aType, const TDesC& aURI, const TDesC8& aWSPHeader, const TDesC8& aBody, const TUint8 aId);
	static void PackGetPduL(HBufC8*& aWSPPdu, TWSPPduType aType, const TDesC& aURI, const TDesC8& aWSPHeader, const TUint8 aId);
	static TInt UintVar(TUint32& aVal, TPtrC8& aBuffer, TUint aOffset);
	static HBufC8* UintVarL(const TUint32 aInt);
	};
#endif // __CLWSPPDUHANDLER_H__
