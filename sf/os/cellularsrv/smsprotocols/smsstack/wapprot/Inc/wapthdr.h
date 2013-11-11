// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains the header file of the TWapTextMessage.
// 
//

/**
 @file
 @internalAll
*/

#if !defined (WAPTHDR_H__)
#define WAPTHDR_H__

#include <e32base.h>

/**
 *  @internalComponent
 */
const TInt KMaxSmsChars = 160;  //< maximum number of characters in one SMS PDU


/**
 *  TWapTextMessage encodes and decodes 7-bit WAP text headers
 *  @internalComponent
 */
class TWapTextMessage
	{
public:
	TWapTextMessage(const TDesC8& aWAPMessage);
	TBool Parse();
    void EncodeSegmentsL(CArrayPtr<HBufC8>& aSegmentArray);
	inline TInt DestinationPort(TBool* aIs16Bit=NULL) const;
	inline void SetDestinationPort(TInt aToPort, TBool aIs16Bit=EFalse);
	inline TInt SourcePort(TBool* aIs16Bit=NULL) const;
	inline void SetSourcePort(TInt aFromPort, TBool aIs16Bit=EFalse);
	inline TInt ReferenceNumber() const;
	inline void SetReferenceNumber(const TInt aReference);
	inline TInt TotalSegments() const;
	inline TInt SegmentNumber() const;
	inline void OtherHeader(TDes8& aOtherHeader) const;
	inline void SetOtherHeader(const TDesC8& aOtherHeader);
	inline void UserData(TDes8& aWAPUserData) const;
	inline void SetUserData(const TDesC8& aWAPUserData);
	static TInt ParseNumber(const TDesC8& aInValue, TBool aBigEndian, TInt aRadix);

private:
	enum TElemIndexes
	{
		KIndexDestinationPort = 0,
		KIndexSourcePort,
		KIndexReferenceNumber,
		KIndexTotalSegments,
		KIndexSegmentNumber,
		KIndexOtherHeader
	};

	enum
	{
		KElemIndexCount = 6,
	};

	// Made following public because of easy access
	// for bith segmenting and reassembly
	static const TInt KHeaderCount;

	typedef TInt IndexValues[KElemIndexCount];

	struct TIndexInfo
		{
		TInt           iLastIndex;
		IndexValues    iIndexes;
		};

	static const TIndexInfo KElemIndexes[];

	TBool ParseWapTextHeader(const TIndexInfo& IndexArray);
	TInt CreateHeader(TDes8& aFixedHeader, TInt& aSegmentNumberIndex);
	TInt CalculateTotalSegments(TInt aFixedLength);

	// Incoming only
	TBool      iIsWapTextMessage;
	TPtrC8     iWAPMessage;

	// header elements
	TBool      iIs16Bit;
	// constructor initialises the port values to (-1)
	TInt       iDestinationPort;
	TInt       iSourcePort;
	TInt       iReference;
	TInt       iTotalSegments;
	TInt       iSegmentNumber;

	// Incoming only: about other header and data
	TInt       iOtherHeader;   // index
	TInt       iOtherHeaderLength;
	TInt       iData;          // index
	TInt       iDataLength;

	// Outgoing only
	TPtrC8     iRefOtherHeader;
	TPtrC8     iRefData;
	};


#include "wapthdr.inl"

#endif // !defined WAPTHDR_H__
