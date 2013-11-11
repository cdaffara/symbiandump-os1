// Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wapdgrmTraces.h"
#endif

#include "gsmubuf.h"
#include "gsmuset.h"
#include "Gsmuelem.h"
#include "Gsmumsg.h"
#include "gsmusar.h"
#include "gsmupdu.h"
#include "WAPDGRM.H"
#include "ws_main.h"
#include "smsstackutils.h"

//
// For incoming short message
//
CWapDatagram* CWapDatagram::NewL(const CSmsMessage& aSms)
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPDATAGRAM_NEWL_1, "CWapDatagram::NewL(): aSms=0x%08x", (TInt) &aSms);

    CWapDatagram* datagram = new (ELeave)CWapDatagram();

    CleanupStack::PushL(datagram);
    datagram->ConstructL(aSms);
    CleanupStack::Pop();

    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPDATAGRAM_NEWL_2, "CWapDatagram::NewL(): iFromPort: %d", datagram->iFromPort);
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPDATAGRAM_NEWL_3, "CWapDatagram::NewL(): iToPort: %d", datagram->iToPort);
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPDATAGRAM_NEWL_4, "CWapDatagram::NewL(): iReference: %d", datagram->iReference);
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPDATAGRAM_NEWL_5, "CWapDatagram::NewL(): iTotalSegments: %d", datagram->iTotalSegments);
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPDATAGRAM_NEWL_6, "CWapDatagram::NewL(): iSegmentNumber: %d", datagram->iSegmentNumber);
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPDATAGRAM_NEWL_7, "CWapDatagram::NewL(): iIsComplete: %d", datagram->iIsComplete);
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPDATAGRAM_NEWL_8, "CWapDatagram::NewL(): iReference: %d", datagram->iReference);
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPDATAGRAM_NEWL_9, "CWapDatagram::NewL(): iIsTextHeader: %d", datagram->iIsTextHeader);
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPDATAGRAM_NEWL_10, "CWapDatagram::NewL(): iLogServerId: %d", datagram->iLogServerId);
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPDATAGRAM_NEWL_11, "CWapDatagram::NewL(): i16BitPorts: %d", datagram->i16BitPorts);

    // assert destination port
	if (datagram->i16BitPorts)
		{
		if (datagram->iToPort>=0  &&  datagram->iToPort<=65535)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPDATAGRAM_NEWL_12, "iToPort OK");
			}
		else
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPDATAGRAM_NEWL_13, "iToPort FAILED");
			}
		}
    else
		{
        if (datagram->iToPort>=0  &&  datagram->iToPort<=255)
			{
		    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPDATAGRAM_NEWL_14, "iToPort OK");
			}
		else
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPDATAGRAM_NEWL_15, "iToPort FAILED");
			}
		}

	if (datagram->i16BitPorts)
		{
		if (datagram->iFromPort>=0  &&  datagram->iFromPort<=65535)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPDATAGRAM_NEWL_16, "iFromPort OK");
			}
		else
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPDATAGRAM_NEWL_17, "iFromPort FAILED");
			}
		}
    else
		{
        if (datagram->iFromPort>=0  &&  datagram->iFromPort<=255)
			{
		    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPDATAGRAM_NEWL_18, "iFromPort OK");
			}
		else
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPDATAGRAM_NEWL_19, "iFromPort FAILED");
			}
		}

    return datagram;
    } // CWapDatagram::NewL


//
// For outgoing short messages
//
CWapDatagram* CWapDatagram::NewL(const TDesC8& aSendBuffer)
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPDATAGRAM_NEWL1_1, "CWapDatagram::NewL(): aSendBuffer=0x%08x", (TInt) &aSendBuffer);

    CWapDatagram* datagram = new (ELeave)CWapDatagram();

    CleanupStack::PushL(datagram);
    datagram->Construct(aSendBuffer);
    CleanupStack::Pop();

    return datagram;
    } // CWapDatagram::NewL


CWapDatagram::~CWapDatagram()
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPDATAGRAM_DTOR_1, "CWapDatagram::~CWapDatagram()");

	delete iRecvbuf;
    delete iBuffer;
    delete iSegment;
    } // CWapDatagram::~CWapDatagram


//
// What about service centre address ?
// aSmsMessageArray contains CSmsMessage objects
//
void CWapDatagram::EncodeConcatenatedMessagesL(RFs& aFs, CArrayPtr<CSmsMessage>& aSmsMessageArray)
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPDATAGRAM_ENCODECONCATENATEDMESSAGESL_1, "CWapDatagram::EncodeConcatenatedMessagesL(): %d messages", aSmsMessageArray.Count());

    // Couple of checkings makes sense
    __ASSERT_DEBUG(iToPort >=0 && iToAddress.Length()>=0
            && (iUserDataSettings.Alphabet()==TSmsDataCodingScheme::ESmsAlphabet8Bit
                 || iUserDataSettings.Alphabet()==TSmsDataCodingScheme::ESmsAlphabet7Bit),
            Panic(KPanicUsageError));

    // Determine CSmsMessage encoding by character width
    if (iUserDataSettings.Alphabet() == TSmsDataCodingScheme::ESmsAlphabet8Bit)
        {
        CSmsBufferBase* SmsBuffer = CSmsBuffer::NewL();

        CleanupStack::PushL(SmsBuffer);
        ConvertL(iSendBuffer,*SmsBuffer);
        CleanupStack::Pop(); // SmsBuffer, popped here since it is pushed again in the following NewL call
        CSmsMessage* SmsMessage = CSmsMessage::NewL(aFs, CSmsPDU::ESmsSubmit,SmsBuffer,EFalse);
        CleanupStack::PushL(SmsMessage);
        // one and only object
        aSmsMessageArray.AppendL(SmsMessage);
        CleanupStack::Pop(); // SmsMessage

        SetSmsMessageSettingsL(*SmsMessage,ETrue);
        }
    else
        {
        // contruct TWapTextMessage object
        TWapTextMessage* Segment = new (ELeave) TWapTextMessage(KNullDesC8);
        CleanupStack::PushL(Segment);
        CArrayPtrFlat<HBufC8>* SegmentArray = new (ELeave) CArrayPtrFlat<HBufC8> (8);
        CleanupStack::PushL(SegmentArray);
		// coverity[double_push]
        CleanupResetAndDestroyPushL(*SegmentArray);

        Segment->SetDestinationPort(iToPort,i16BitPorts);
        Segment->SetSourcePort(iFromPort,i16BitPorts);
        Segment->SetReferenceNumber(iReference);
        Segment->SetUserData(iSendBuffer);
        Segment->SetOtherHeader(iOtherHeader);
        Segment->EncodeSegmentsL(*SegmentArray);

        // contruct array of CSmsMessages
        TInt Count = SegmentArray->Count();
        for (TInt i=0; i<Count; i++)
            {
            CSmsBufferBase* SmsBuffer = CSmsBuffer::NewL();

            CleanupStack::PushL(SmsBuffer);
            ConvertL(*(SegmentArray->At(i)),*SmsBuffer);
            CleanupStack::Pop(); // SmsBuffer, popped here since it is pushed again in the following NewL call
            CSmsMessage* SmsMessage=CSmsMessage::NewL(aFs, CSmsPDU::ESmsSubmit,SmsBuffer,EFalse);
            CleanupStack::PushL(SmsMessage);
            aSmsMessageArray.AppendL(SmsMessage);
            CleanupStack::Pop(); // SmsMessage

            SetSmsMessageSettingsL(*SmsMessage,EFalse);
            }
        CleanupStack::PopAndDestroy(3, Segment);  // SegmentArray elements (Reset and Destroy), SegmentArray, Segment

        }
    } // CWapDatagram::EncodeConcatenatedMessagesL

void CWapDatagram::DecodeConcatenatedMessagesL(CArrayPtr<TSegmentData>& aSmsMessageArray)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPDATAGRAM_DECODECONCATENATEDMESSAGESL_1, "CWapDatagram::DecodeConcatenatedMessagesL()");

    // The TSegmentData elements are in the random order in the array
    TInt Count = aSmsMessageArray.Count();
    TInt i=0;

    if (Count > 0)
        {
        TInt DataLength = 0;  // total length of data
        TInt TempLength = 0;

        // Every TSegmentData should be equal length except last segment
        // Use that 'constant' segment length as indexes
        // Count the actual data length
        for(i=0; i<Count; i++)
            {
            TempLength = aSmsMessageArray.At(i)->iData.Length();
            DataLength += TempLength;
            }

        TSegmentData* Segment = NULL;

        if (iBuffer)
            {
            delete iBuffer;
            iBuffer = NULL;
            }
        iBuffer = HBufC8::NewL(DataLength );

        TPtr8 BufferPtr(iBuffer->Des());
        BufferPtr.SetLength(DataLength );

        TInt segmentStartPosition=0;

        for(i=0; i<Count; i++)
            {
              if (segmentStartPosition >= DataLength)  
                  {
                  // Once the start position is out of range,
                  // there is no point in continuing reconstructing the
                  // wapdatagram.
                  break;
				 }			
			
            Segment = aSmsMessageArray.At(i);
			TPtr8 CopyBufferPtr(&(BufferPtr[segmentStartPosition]),0,160);
			CopyBufferPtr.Copy(Segment->iData);
 			segmentStartPosition+=Segment->iData.Length();
            }
        }
    iIsComplete = ETrue;
    } // CWapDatagram::DecodeConcatenatedMessagesL


void CWapDatagram::InternalizeL(RReadStream& aStream)
    {
    aStream >> iUserDataSettings;
    aStream >> iToAddress;
    iToPort = aStream.ReadInt32L();
    aStream >> iFromAddress;
    iFromPort = aStream.ReadInt32L();
    TInt64 time;
    aStream >> time;
    iTime = time;
    iUTCOffset = aStream.ReadInt32L();
    iIsTextHeader = aStream.ReadInt32L();
    iReference = aStream.ReadInt32L();
    iTotalSegments = aStream.ReadInt32L();
    iSegmentNumber = aStream.ReadInt32L();
    iLogServerId = aStream.ReadInt32L();
    iVersionNumber = aStream.ReadInt32L();
    iSpare1 =  aStream.ReadInt32L();
    iSpare2 =  aStream.ReadInt32L();
    iSpare3 =  aStream.ReadInt32L();
	
    // Required for version 1, which is reading and internalizing extra SMS parameters from stream
    if(iVersionNumber == EFirstVersion)
    	{
    	TInt length = aStream.ReadInt32L();
    	if(length>0)
    		{
    		HBufC8* smsBuffer=HBufC8::NewMaxLC(length);
			TPtr8 recvbuftmp = smsBuffer->Des();
			aStream >> recvbuftmp;
			if(!iRecvbuf)
				{
				iRecvbuf = CBufFlat::NewL(KSmsBufferExpansion);
				}
			iRecvbuf->ResizeL(recvbuftmp.Size());
			iRecvbuf->Write(0, recvbuftmp);
			CleanupStack::PopAndDestroy(smsBuffer);
			}
    	}
    
    } // CWapDatagram::InternalizeL


//
//	Reads iBuffer from stream (for 8Bit messages)
//
void CWapDatagram::InternalizeBufferL(RReadStream& aStream)
	{
	TInt length;
	length=aStream.ReadInt32L();
	iBuffer=HBufC8::NewMaxL(length);
	TPtr8 tmp = iBuffer->Des();
	aStream >> tmp;
	iIsComplete = ETrue;
	}


void CWapDatagram::ExternalizeL(RWriteStream& aStream) const
    {
    aStream << iUserDataSettings;
    aStream << iToAddress;
    aStream.WriteInt32L(iToPort);

    aStream << iFromAddress;
    aStream.WriteInt32L(iFromPort);
    aStream << iTime.Int64();
    aStream.WriteInt32L(iUTCOffset.Int());

    aStream.WriteInt32L(iIsTextHeader);
    aStream.WriteInt32L(iReference);
    aStream.WriteInt32L(iTotalSegments);
    aStream.WriteInt32L(iSegmentNumber);

    aStream.WriteInt32L(iLogServerId);

    aStream.WriteInt32L(iVersionNumber);
    aStream.WriteInt32L(iSpare1);
    aStream.WriteInt32L(iSpare2);
    aStream.WriteInt32L(iSpare3);
	
	// Externalizing the SMS params if exists
	if(iRecvbuf)
		{
		TPtr8 recvbuftmp = iRecvbuf->Ptr(0);
		aStream.WriteInt32L(recvbuftmp.Length());
		aStream << recvbuftmp;
		}
	else
		{
		aStream.WriteInt32L(0);
		}
    }


//
// writes iBuffer to writeStream for 8-bit incomming messages
//
void CWapDatagram::ExternalizeBufferL(RWriteStream& aStream) const
	{
	TInt length= WapDatagramLength();
	aStream.WriteInt32L(length);
	TPtr8 tmp= iBuffer->Des();
	aStream << tmp;

	} // CWapDatagram::ExternalizeBufferL


//
// Outgoing
//
void CWapDatagram::Construct(const TDesC8& aSendBuffer)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPDATAGRAM_CONSTRUCT_1, "CWapDatagram::Construct()");
	
	// Set version number to 1, as we have had to make 
	// changes to CWapDatagram for CR0929
	iVersionNumber = EFirstVersion;
	
    iSendBuffer.Set(aSendBuffer);
    } // CWapDatagram::Construct


//
// Incoming
// code actualy supports 7-bit characters and port numbers
// encoded in information elements (IE) although it is not
// a requirement
//
void CWapDatagram::ConstructL(const CSmsMessage& aSms)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPDATAGRAM_CONSTRUCTL_1, "CWapDatagram::ConstructL()");

	// Set version number to 1, as we have had to make 
	// changes to CWapDatagram for CR0929
	iVersionNumber = EFirstVersion;

	iRecvbuf = CBufFlat::NewL(KSmsBufferExpansion);

	RBufWriteStream writestream(*iRecvbuf);
	writestream.Open(*iRecvbuf);
	CleanupClosePushL(writestream);
	
	// Externalizing everything within CSMSMessage except buffer
	aSms.ExternalizeWithoutBufferL(writestream);
	
	CleanupStack::PopAndDestroy();  //  writestream

	iRecvbuf->Compress();

    const CSmsBufferBase& SmsBufferBase = aSms.Buffer();

    GetDatagramSettings(aSms);

    // Get the data from CSmsMessage and convert
    // SmsBuffer store Unicode short message from CSmsMessage
    ConvertL(SmsBufferBase,&iBuffer);
	GetDatagramSettingsL();

    iIsComplete = ETrue;
    if (iUserDataSettings.Alphabet() == TSmsDataCodingScheme::ESmsAlphabet7Bit)
        {
        // this is a good candidate to include a text header.
        if (iIsTextHeader)
            {
            // there is still a minor chance that message is complete:
            // one fragment long datagram
            if (iTotalSegments == 1)
                {
                // Get the segment data
                TPtr8 PtrNarrow = iBuffer->Des();
                PtrNarrow.Zero();
                iSegment->UserData(PtrNarrow);
                }
            else
                {
                iIsComplete = EFalse;
                // delete incomplete message from iBuffer;
                delete iBuffer;
                iBuffer = 0;
                }
            }
        else
            // This means that IEs with 7-bits are used
            // Happily we support it
            iIsComplete = ETrue;

        }
    } // CWapDatagram::ConstructL


//
// Set WAP datagram private members from a text based concatenated message
// Copies contents of iBuffer to iSmsBuffer
// Allocates iSegment object
//
void CWapDatagram::GetDatagramSettingsL()
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPDATAGRAM_GETDATAGRAMSETTINGSL_1, "CWapDatagram::GetDatagramSettingsL()");

	/* The WAP stack always receives 8 bit WAP datagrams from the SMS Stack as a single CSmsMessage.
	This is because 8 bit WAP messages which cannot be encoded into a single PDU are sent in a segmented SMS
	message which is reassembled inside the SMS Stack; the SMS Stack always passes the WAP stack
	a single CSmsMessage containing a single WAP message regardless of the message length.
	The WAP stack can receive 7 Bit WAP datagrams either as a single CSmsMessage or as a number of CSmsMessages
	which need to be reassembled into a single WAP message by the WAP stack.  The latter method is provided to maintain backward
	compatibility with early versions of the WAP stack which were implemented before the concatenation function was
	available in the SMS stack.*/

	/* 160 is the maximum number of character that can be fitted in a single PDU using 7 bit encoding */
	if((iUserDataSettings.Alphabet()== TSmsDataCodingScheme::ESmsAlphabet8Bit) ||
	(iUserDataSettings.Alphabet()== TSmsDataCodingScheme::ESmsAlphabet7Bit && iBuffer->Length()>160 ))
		{
		iSegment = new (ELeave)TWapTextMessage(*iBuffer);
		}
	else if (iBuffer->Length()<=160)
        {
        iSmsBuffer = *iBuffer;
		iSegment = new (ELeave)TWapTextMessage(iSmsBuffer);
        iIsTextHeader = iSegment->Parse();

        if (iIsTextHeader)
            {
            iFromPort = iSegment->SourcePort(&i16BitPorts);
            iToPort = iSegment->DestinationPort(&i16BitPorts);
            iReference = iSegment->ReferenceNumber();
            iTotalSegments = iSegment->TotalSegments();
            iSegmentNumber = iSegment->SegmentNumber();
            }
        }
	else
		{
        delete iSegment;
        iSegment = 0;
		}
     } // CWapDatagram::GetDatagramSettingsL


//
// Set WAP datagram private members from information element structures
//
void CWapDatagram::GetDatagramSettings(const CSmsMessage& aSms)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPDATAGRAM_GETDATAGRAMSETTINGS_1, "CWapDatagram::GetDatagramSettings()");

    const CSmsPDU& Pdu = aSms.SmsPDU();

    __ASSERT_DEBUG(Pdu.Type()==CSmsPDU::ESmsDeliver || Pdu.Type()==CSmsPDU::ESmsSubmit, // this line is testing purposes only
			Panic(KPanicUsageError));

    aSms.UserDataSettings(iUserDataSettings);

    Pdu.ApplicationPortAddressing(iToPort,iFromPort,&i16BitPorts);
    if (iFromPort == (-1))
        iFromPort = iToPort;

    // From WAP datagram point of view following numbers are applicable
    iReference = 0;
    iTotalSegments = 1;
    iSegmentNumber = 1;

    iFromAddress = aSms.ToFromAddress();
    iToAddress = KNullDesC;
    iTime = aSms.Time();

    TBool result = SetUTCOffset(aSms.UTCOffset());
    __ASSERT_DEBUG(result, Panic(KPanicSmsMsgTimeZoneOutOfRange));
    } // CWapDatagram::GetDatagramSettings


//
// Set Alphabet information of iUserDataSettings before calling the method
// Converts from 7/8-bit to UNICODE
//
void CWapDatagram::ConvertL(const TDesC8& aNarrowChars,CSmsBufferBase& aSmsBuffer) const
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPDATAGRAM_CONVERTL_1, "CWapDatagram::ConvertL()");

	// Convert the data in segments of specified max size
	const TInt KMaxSegmentSize=CSmsBufferBase::EMaxBufLength;

	// Create converter and reassembler
	RFs fs;
	CCnvCharacterSetConverter* charConv=CCnvCharacterSetConverter::NewLC();
	CSmsAlphabetConverter* converter=CSmsAlphabetConverter::NewLC(*charConv,fs,iUserDataSettings.Alphabet(),ETrue);
	TSmsBufferReassembler reassembler(*converter,aSmsBuffer);

	// Rassemble
	TInt elementsRemaining=aNarrowChars.Length();
	while (elementsRemaining)
		{
		TInt segmentLength=Min(KMaxSegmentSize,elementsRemaining);
		TPtrC8 ptr(aNarrowChars.Ptr()+aNarrowChars.Length()-elementsRemaining,segmentLength);
		reassembler.ReassembleNextL(ptr, ESmsEncodingNone,
									elementsRemaining==segmentLength);
		elementsRemaining-=segmentLength;
		}

	CleanupStack::PopAndDestroy(2);	// charConv,
    } // CWapDatagram::ConvertL


//
// Converts from UNICODE to 7/8-bit
//
void CWapDatagram::ConvertL(const CSmsBufferBase& aSmsBuffer,HBufC8** aNarrowChars) const
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPDATAGRAM_CONVERTL1_1, "CWapDatagram::ConvertL()");

	// Convert the data in segments of specified max size
	const TInt KMaxSegmentSize=CSmsBufferBase::EMaxBufLength;

	// Delete the existing buffer
	if (*aNarrowChars)
		{
		delete *aNarrowChars;
		*aNarrowChars=NULL;
		}

	// Create converter and segmenter
	RFs fs;
	CCnvCharacterSetConverter* charConv=CCnvCharacterSetConverter::NewLC();
	CSmsAlphabetConverter* converter=CSmsAlphabetConverter::NewLC(*charConv,fs,iUserDataSettings.Alphabet(),ETrue);
	CSmsBufferSegmenter* segmenter=CSmsBufferSegmenter::NewLC(*converter,aSmsBuffer,KMaxSegmentSize);

	// Create a new buffer based on converted length
	TInt convertedLength=segmenter->TotalConvertedLengthL(ESmsEncodingNone);
	*aNarrowChars=HBufC8::NewMaxL(((convertedLength+KMaxSegmentSize-1)/KMaxSegmentSize)*KMaxSegmentSize);
	TPtr8 narrowPtr=(*aNarrowChars)->Des();

	// Now do the conversion
	TInt elementsConverted=0;
	TInt unconvertedChars=0;
	TInt downgradedChars=0;
	TBool complete=EFalse;
	while (elementsConverted<convertedLength)
		{
		__ASSERT_DEBUG(elementsConverted<convertedLength,Panic(KPanicTooLongData));
		TPtr8 ptr((TUint8*)narrowPtr.Ptr()+elementsConverted,0,KMaxSegmentSize);
		complete=segmenter->SegmentNextL(ptr, unconvertedChars, downgradedChars, ESmsEncodingNone);
		elementsConverted+=ptr.Length();
		}
    if((!complete || (convertedLength!=elementsConverted)) &&  convertedLength)
        User::Leave( KErrCorrupt );
	narrowPtr.SetLength(convertedLength);

	CleanupStack::PopAndDestroy(3);	// charConv,converter,segmenter
    } // CWapDatagram::ConvertL


void CWapDatagram::SetSmsMessageSettingsL(CSmsMessage& aSmsMessage, TBool aSetPorts)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPDATAGRAM_SETSMSMESSAGESETTINGSL_1, "CWapDatagram::SetSmsMessageSettingsL()");

    CSmsPDU& Pdu = aSmsMessage.SmsPDU();

    aSmsMessage.SetToFromAddressL(iToAddress);
    aSmsMessage.SetUserDataSettingsL(iUserDataSettings);
    aSmsMessage.SetTime(iTime);

    TBool result = aSmsMessage.SetUTCOffset(iUTCOffset);
    __ASSERT_DEBUG(result, Panic(KPanicWapDgrmTimeZoneOutOfRange));
    
    if (aSetPorts)
        {
        if (iFromPort == (-1))
            iFromPort = iToPort;

        // Determine whether long or short form is used
        if (iFromPort > 255 || iToPort>255)
            i16BitPorts = ETrue;

	    Pdu.SetApplicationPortAddressingL(ETrue,iToPort,iFromPort,i16BitPorts);
		// WAP implementation guidelines recommend DCS of 0x15, however
		// some gateways / SCs will only accept DCS 0xf5
		Pdu.SetBits7To4(TSmsDataCodingScheme::ESmsDCSTextUncompressed7BitOr8Bit);
		Pdu.SetAlphabet(TSmsDataCodingScheme::ESmsAlphabet8Bit);
		Pdu.SetClass(ETrue,TSmsDataCodingScheme::ESmsClass1);
        }
    } // CWapDatagram::SetSmsMessageSettingsL


CWapDatagram::CWapDatagram():
    iUserDataSettings(),
    iToAddress(KNullDesC),
    iFromAddress(KNullDesC),
    i16BitPorts(EFalse),
    iFromPort(-1),
    iToPort(-1),
    iTime(),
    iUTCOffset(0),
    iIsComplete(EFalse),
    iReference(0),
    iTotalSegments(0),
    iSegmentNumber(0),
    iIsTextHeader(EFalse),
    iSegment(NULL),
    iSmsBuffer(KNullDesC8),
    iBuffer(NULL),
    iSendBuffer(KNullDesC8),
    iOtherHeader(KNullDesC8),
    iLogServerId(0),
    iVersionNumber(EBaseVersion),// Set version number to EBaseVersion by default.
    iSpare1(0),
    iSpare2(0),
    iSpare3(0),
    iRecvbuf(NULL)
    {
    iTime.UniversalTime();

    TBool result = SetUTCOffset(User::UTCOffset());
   __ASSERT_DEBUG(result, Panic(KPanicUserSuppliedTimeZoneOutOfRange));

    iUserDataSettings.SetAlphabet(TSmsDataCodingScheme::ESmsAlphabet8Bit);
    __DECLARE_NAME(_S("CWapDatagram"));
    } // CWapDatagram::CWapDatagram


//
// Return the location of the link
//
TInt CWapDatagram::LinkOffset()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPDATAGRAM_LINKOFFSET_1, "CWapDatagram::LinkOffset()");


	return _FOFF(CWapDatagram,iLink);
	} // CWapDatagram::LinkOffset


TBool CWapDatagram::SetUTCOffset(const TTimeIntervalSeconds& aUTCOffset)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPDATAGRAM_SETUTCOFFSET_1, "CWapDatagram::SetUTCOffset()");

    TBool rc = ETrue;
    TInt utcOffset = aUTCOffset.Int();

    if ((utcOffset <=  CSmsMessage::EMaximumSeconds) &&
        (utcOffset >= -CSmsMessage::EMaximumSeconds))
       {
       iUTCOffset = utcOffset;
       }
    else
       {
       OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPDATAGRAM_SETUTCOFFSET_2, "CWapDatagram:SetUTCOffset offset [out of range] = %d",utcOffset);
       rc = EFalse;
       }

    return rc;
    } // CWapDatagram::SetUTCOffset
    


CBufFlat* CWapDatagram::SmsExternalisedStream() const
	{
	return iRecvbuf;
	}
