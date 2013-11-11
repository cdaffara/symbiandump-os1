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
// Contains the header file of the CWapDatagram
// 
//

/**
 @file
 @internalAll
*/

#if !defined (WAPDGRM_H__)
#define WAPDGRM_H__

#include <e32def.h>
#include <e32std.h>
#include <s32buf.h>
#include <s32mem.h>
#include "gsmubuf.h"
#include "gsmuset.h"
#include "Gsmuelem.h"
#include "Gsmumsg.h"
#include "wapthdr.h"

class RMBufChain;

const TInt KSmsBufferExpansion=0x100;

// Notes !
// - Character sets
//   Incoming (up stack):
//      This class converts Unicode characters of a CSmsMessage object
//      to either 7 or 8 bit.
//   Outgoing (down stack): This class converts 7 or 8 bit input data to Unicode.
//
// - Text based headers
//   Concerning 7 bit characters:
//      port information is ALWAYS encoded in //SCK style.

/**
 *  @internalComponent
 */
class CWapDatagram : public CBase
    {
public:
	
	enum TWapDatagramVersion
		{
		EBaseVersion=0,
		EFirstVersion=1,
		};
	
    struct TSegmentData
        {
        TInt       iSegmentNumber;
        TBuf8<KMaxSmsChars> iData; //< application data
        };

        // Contructor for incoming short messages
        // Set the character width 7- or 8-bit in the parameter aAlphabet,
        // which you expect to get through WapDatagram().
        // UNICODE is not supported.
        // If the WAP datagram is complete (IsComplete()),
        // call the  WapDatagram() to retrieve the data.

        // If the datagram is not complete, concatenate
        // the WAP datagram, when all the segments are available
        // through DecodeConcatenatedMessagesL().
    static CWapDatagram* NewL( const CSmsMessage&      aSms);

        // for outgoing short messages
    static CWapDatagram* NewL( const TDesC8&   aBuffer);

    ~CWapDatagram();

        // Only 7-bit text mode socket header may return false
        // when the datagram is not complete.
    inline TBool IsComplete() const;

        // Get the length of the datagram
    inline TInt WapDatagramLength() const;

        // Copies the user data of incoming WAP datagram to the aBufChain
        // Prior to calling this method IsComplete() should return true
        // and/or DecodeConcatenatedMessagesL() should have been called.
		// aMaxLength indicates largest size that will be passed atomically
		// to the client: generally the whole datagram should be returned
		// but this can be used for truncation if required.
        // Returns error code
    inline TInt WapDatagram(RMBufChain& aBufChain, TUint aMaxLength) const;

        // if CWapDatagram object is incomplete, get the
        // TSegmentData information of the short message
    inline void SegmentData(TSegmentData& aSegmentData) const;

        // from WAP spec: destination address
    inline TPtrC ToAddress() const;
    inline void SetToAddress(const TDesC& aToAddress);

        // from WAP spec: source address
    inline TPtrC FromAddress() const;
    inline void SetFromAddress(const TDesC& aFromAddress);

        // from WAP spec: destination port
        // 16bit width port addressing is used automatically, if
        // the port value is > 255
    inline void Ports(TInt& aFromPort,TInt& aToPort,TBool* aIs16Bit = NULL) const;
    inline void SetPorts(TInt aFromPort, TInt aToPort,TBool aIs16BIt = EFalse);

        // from WAP spec: other header support by text header only
        // no idea what it is used for
    inline void OtherHeader(TDes8& aOtherHeader) const;
    inline void SetOtherHeader(const TDesC8& aOtherHeader);

        // from GSM SMS spec
        // NT: time of first CSmsMessage received
        // MO: validity period
    inline const TTime& Time() const;
    //inline void SetTime(const TTime& aTime);

    inline TTimeIntervalSeconds UTCOffset() const;
    TBool SetUTCOffset(const TTimeIntervalSeconds& aUTCOffset);

    // May return > 1, if the socket text headers are used
    inline TInt NumConcatenatedMessages() const;

    inline TInt ConcatenatedMessageReference(TBool* aIs16BIt = NULL) const;
        // concerns socket text headers only
        // otherwise ignored
    inline void SetConcatenatedMessageReference(TInt    aReference);

        // what about service centre address ?
        // aSmsMessageArray contains CSmsMessage objects
        // aSmsBufferArray contains respective CSmsMessage objects
    void EncodeConcatenatedMessagesL(RFs& aFs, CArrayPtr<CSmsMessage>& aSmsMessageArray);

    void DecodeConcatenatedMessagesL(CArrayPtr<TSegmentData>& aSmsMessageArray);

        // from GSM spec: Alphabet setting of outgoing datagram only
        // set the alphabet on incoming datagrams in the contructor
    inline TSmsDataCodingScheme::TSmsAlphabet Alphabet() const;
    inline void SetAlphabet(TSmsDataCodingScheme::TSmsAlphabet aAlphabet);

    void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream) const;

	// for 8-bit datagrams. Reads/writes iSmsBuffer from/to stream
	void InternalizeBufferL(RReadStream& aStream);
	void ExternalizeBufferL(RWriteStream& aStream) const;

	inline TInt LogServerId() const;
    inline void SetLogServerId(TInt aId);
    
    inline TSmsStatusReportScheme	Scheme() const;
    inline void SetStatusReportScheme(TSmsStatusReportScheme aScheme);
    
    CBufFlat* SmsExternalisedStream() const;
private:
    CWapDatagram();

        // incoming
    void ConstructL(const CSmsMessage& aSms);

        // outgoing
    void Construct(const TDesC8&   aSendBuffer);

        // Set underlying CWapDatagram private member variables
        // from CSmsMessage object + any common settings
    void GetDatagramSettings(const CSmsMessage& aSms);

        // Sets underlying WAP datagram private member variables
        // from a text header short message
    void GetDatagramSettingsL();

        // converts from const type variable to other type
    void ConvertL(const TDesC8&         aNarrowChars,
                  CSmsBufferBase&       aSmsBufferBase) const;

    void ConvertL(const CSmsBufferBase& aSmsBufferBase,
                  HBufC8**				aNarrowChars) const;

    void SetSmsMessageSettingsL(CSmsMessage& aSmsMessage,
                                TBool  aSetPorts);
private:

	// WAP datagram specific settings
    TSmsUserDataSettings iUserDataSettings;
    TBuf<32>         iToAddress;
    TBuf<32>         iFromAddress;
    TBool             i16BitPorts;
        // Port numbers are initialized to (-1)
    TInt              iFromPort;
    TInt              iToPort;
    TTime             iTime;
    TTimeIntervalSeconds iUTCOffset;
        // SAR information
    TBool             iIsComplete;
    TInt              iReference;
    TInt              iTotalSegments;
    TInt              iSegmentNumber;

    TBool             iIsTextHeader;
                        // member iSegment contains a reference
                        // - Incoming messages: iSmsBuffer
                        // - Outgoing messages:
    TWapTextMessage*  iSegment;
                        // for storing text based concatenated message segment
    TBuf8<KMaxSmsChars> iSmsBuffer;
                        // Incoming: complete wap datagram
    HBufC8*           iBuffer;

                        // Outgoing:
    TPtrC8            iSendBuffer;
    TPtrC8            iOtherHeader;

    TInt              iLogServerId;
    TInt              iVersionNumber;
    TInt              iSpare1; // spare fields
    TInt              iSpare2; // spare fields
    TInt              iSpare3; // spare fields
    TSmsStatusReportScheme iScheme;
    
    CBufFlat* iRecvbuf;

public:
	static TInt LinkOffset();
private:
	TSglQueLink iLink;
    };

#include "wapdgrm.inl"

#endif // WAPDGRM_H__
