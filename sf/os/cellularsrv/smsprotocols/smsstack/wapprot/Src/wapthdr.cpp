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
// This file contains the implementation of the TWapTextMessage.
// TWapTextMessage encodes and decodes 7-bit WAP text headers
// 
//

/**
 @file
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wapthdrTraces.h"
#endif

#include <e32std.h>
#include "wapthdr.h"
#include "ws_main.h"

const TInt TWapTextMessage::KHeaderCount = 6;

_LIT8(KSCKHeaderLong, "//SCKL"); // standard NBS header for 16-bit adresses
_LIT8(KSCKHeaderShort,"//SCK");  // standard NBS header for 8-bit adresses


/**
 *  Indices into different types header for each of the header elements
 *  (dest port, source port, reference, total segments, current segment)
 */
const TWapTextMessage::TIndexInfo TWapTextMessage::KElemIndexes[TWapTextMessage::KHeaderCount] =
{
// Long header ("//SCKL") indices
// destination port (, other header)
    { 1,{ 6, 10, 0, 0, 0, 0} },
// destination port, source port (, other header)
    { 2,{ 6, 10, 14, 0, 0, 0} },
// destination port, source port,
// reference, total segments, current segment(, other header)
    { 5,{ 6, 10, 14, 16, 18, 20}},

// Short header ("//SCK") indices
// destination port (, other header)
    { 1, { 5, 7, 0, 0, 0, 0}},
// destination port, source port (, other header)
    { 2, { 5, 7, 9, 0, 0, 0}},
// destination port, source port,
// reference, total segments, current segment(, other header)
    { 5,{ 5, 7, 9, 11, 13, 15}},
};


/**
 *  C'tor
 *  
 *  @param aWapMessage WAP message to decode/encode
 *  @note simply use KNullDesC8, if you are going to encode.
 */
TWapTextMessage::TWapTextMessage(const TDesC8& aWapMessage)
    :iIsWapTextMessage(EFalse)
    ,iWAPMessage(aWapMessage)
    ,iIs16Bit(EFalse)
    ,iDestinationPort(-1)
    ,iSourcePort(-1)
    ,iReference(0)
    ,iTotalSegments(0)
    ,iSegmentNumber(0)
    ,iOtherHeader(0)
    ,iOtherHeaderLength(0)
    ,iData(0)
    ,iDataLength(0)
    ,iRefOtherHeader(KNullDesC8)
    ,iRefData(KNullDesC8)
    {
    } // TWapTextMessage::TWapTextMessage


/**
 *  Panic is raised, if the header with other header and terminating
 *  ' ' is > 159 characters. Then 1 byte is left for payload !
 *  what will be the array element type ?
 */
void TWapTextMessage::EncodeSegmentsL(CArrayPtr<HBufC8>& aSegmentArray)
    {
    TBuf8<KMaxSmsChars>  header;        // buffer for a modifiable header
    TInt        headerLength = 0;
    TInt        segmentNumberIndex = 0; // index of segment number field in the header
    TBuf8<2>    hexSegmentNumber;
    TInt        dataSegmented = 0;

    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TWAPTEXTMESSAGE_ENCODESEGMENTSL_1, "TWapTextMessage::EncodeSegmentsL()");

    iSegmentNumber = 0;
    do
        {
        //
        // Create the segment and add it to the array...
        //
        HBufC8*  segment = HBufC8::NewL(KMaxSmsChars); // 160 characters
        CleanupStack::PushL(segment);
        TPtr8  ptr(segment->Des());
        aSegmentArray.AppendL(segment);
        CleanupStack::Pop(); // segment

        //
        // Calculate length of header and copy it...
        //
        if (iSegmentNumber==0)
            {
            headerLength = CreateHeader(header, segmentNumberIndex);
            }

        ptr.Copy(header);
        if (iTotalSegments>255)
            {
            User::Leave(KErrOverflow);
            }

        //
        // Set segment number...
        //
        if (segmentNumberIndex != 0)
            {
            hexSegmentNumber.NumFixedWidthUC(iSegmentNumber+1, EHex, 2); // two bytes wide
            ptr.Insert(segmentNumberIndex, hexSegmentNumber);
            }

        //
        // Count any escaped characters we can be sure that the converted data
        // size fits inside the remaining length (e.g. so that non-7bit characters
        // when converted by the SMS Stack will still fit).
        //
        TInt  segmentSize = iRefData.Length() - dataSegmented;

        if (segmentSize > KMaxSmsChars - headerLength)
            {
            segmentSize = KMaxSmsChars - headerLength;
            }

        while (segmentSize > 1)
            {
            TPtrC8  segmentData(iRefData.Mid(dataSegmented, segmentSize));
            TInt  non7bitCharEscapes = 0;
            
            //
            // Count all non-7bit characters that will be escaped (many non-7bit
            // characters are not escaped, but converted to "?"). The ones
            // that are known to be escaped are list below:
            //
            //    12  [Form Feed].
            //    91  "["
            //    92  "\"
            //    93  "]"
            //    94  "^"
            //   123  "{"
            //   124  "|"
            //   125  "}"
            //   126  "~"
            //
            for (TInt  ch = 0;  ch < segmentSize;  ch++)
                {
                if (segmentData[ch] == 12  ||
                    (segmentData[ch] >= 91  &&  segmentData[ch] <= 94)  ||
                    (segmentData[ch] >= 123  &&  segmentData[ch] <= 126))
                    {
                    non7bitCharEscapes++;
                    }
                }
            
            //
            // Can it fit? If so store it, otherwise reduce the size...
            //
            if (segmentData.Length() + non7bitCharEscapes <= KMaxSmsChars - headerLength)
                {
                ptr.Append(segmentData);
                break;
                }
            
            segmentSize--;
            }
        
            dataSegmented += segmentSize;
            iSegmentNumber++;
        }
    while (dataSegmented < iRefData.Length());

    __ASSERT_DEBUG(iTotalSegments == aSegmentArray.Count(), Panic(KPanicEncodingError));
    } // TWapTextMessage::EncodeSegmentsL


/**
 *  Returns true, if the short message starts with
 *  WAP text message header set by SetWapTextMessage.
 */
TBool TWapTextMessage::Parse()
    {
    TInt waplength = iWAPMessage.Length();
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TWAPTEXTMESSAGE_PARSE_1, "TWapTextMessage::Parse [%d bytes]", waplength);
    if(waplength != 0)
        {
        // check whether long or short header
        TInt iBaseIndex = -1;

        // minimum length is 8 "//SCKxx "
        if(waplength >= 8)
            {
            TPtrC8 HeaderIdPart(iWAPMessage.Left(6));
            if(!HeaderIdPart.Compare(KSCKHeaderLong))
                {
                iBaseIndex = 0;
                iIs16Bit = ETrue; // Yes, the ports are 16 bit wide
                }
            else
                HeaderIdPart.Set(iWAPMessage.Left(5));

            if(iBaseIndex == (-1) && !HeaderIdPart.Compare(KSCKHeaderShort) )
                iBaseIndex = 3;
            // else  no match
            }

        if(iBaseIndex >= 0)
            {
            // check which of the TIndexInfos match
            for(TInt i=iBaseIndex; i<iBaseIndex+3; i++)
                {
                // Get length (index) of text header variant
                TInt lastIndex = KElemIndexes[i].iIndexes[KElemIndexes[i].iLastIndex];
                if (iWAPMessage.Length() <= lastIndex)
                    {
                    // no hope about match, because other variants are longer
                    iIsWapTextMessage = EFalse;
                    break;
                    }

                TInt LastChar = iWAPMessage[lastIndex];
                if (LastChar == '/' || LastChar == ' ' || LastChar == '\n')
                    {
                    // reinitialize, because following maybe absent
                    iTotalSegments = 1;
                    iSegmentNumber = 1;
                    iIsWapTextMessage = ParseWapTextHeader(KElemIndexes[i]);
                    break;
                    }

                }

            if (!iIsWapTextMessage)
                {
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TWAPTEXTMESSAGE_PARSE_2, "WARNING! illegal incoming WAP message");
                }
            }
        }

    return iIsWapTextMessage;
    } // TWapTextMessage::Parse


/**
 *  Parses a string of hex characters representing a number
 *  
 *  @param aInValue descriptor containing the number
 *  @param aBigEndian true if number is big endian
 *  @param aRadix Radixbase; 16, 10 etc
 *  @return TInt the parsed number
 *  @leave Panics with KPanicInvalidParseNumber error code, if not a
 *  number. Maximum width of the hex value can be 4.
 *  
 *  TODO use TLex instead
 */
TInt TWapTextMessage::ParseNumber(const TDesC8& aInValue,
                                  TBool   aBigEndian,
                                  TInt    aRadix)
    {
    OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TWAPTEXTMESSAGE_PARSENUMBER_1, "TWapTextMessage::ParseNumber [%s]", aInValue);
    // least significant byte first
    TInt Values[4] = {0,0,0,0};
    TInt Temp = 0;
    TInt length = aInValue.Length();
    TInt i = 0;
    TInt Value = 0;

    __ASSERT_DEBUG(length<5,Panic(KPanicInvalidParseNumber));
    if( length >= 5 )
        return KErrNotFound;
    for(i=0; i<length; i++)
        {
        Temp = aInValue[i];
        if (Temp>='0' && Temp<='9')
            Temp-='0';
        else if (Temp>='A' && Temp<='Z')
            Temp = Temp - 'A'+10;
        else if (Temp>='a' && Temp<='z')
            Temp = Temp - 'a'+10;
        else
            return KErrNotFound;
        if (aBigEndian)
            Values[(length-1)-i]=Temp;
        else
            Values[i]=Temp;
        }

    // build the value
    Value=Values[0];
    TInt Base=1;
    for(i=1; i<length; i++)
        {
        Base*=aRadix;
        Value+=(Base)*Values[i];
        }

    return Value;
    } // TWapTextMessage::ParseNumber


/**
 *  parse the WAP text header
 *  
 *  use ElemIndexes to retrieve the index
 *  an starting position of an element in iWapMessage
 *  Length of element is calculated by
 *  subtracting current index value from the next one
 *  except for KOtherHeader and user data, of course
 */
TBool TWapTextMessage::ParseWapTextHeader(const TIndexInfo& aIndexArray)
    {
    OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TWAPTEXTMESSAGE_PARSEWAPTEXTHEADER_1, "TWapTextMessage::ParseWapTextHeader: %s", iWAPMessage );

    TInt ParsedNumber = 0;

    // parse the header
    TInt ElemIndexCount=aIndexArray.iLastIndex+1;
    for(TInt i=0; i<ElemIndexCount; i++)
        {
        if (i<ElemIndexCount-1)
            {
            // all the elems have a length defined in advance
            if (iWAPMessage.Length() >= aIndexArray.iIndexes[i+1])
                {
                // the header fits into the wap datagram
                TPtrC8 Elem(iWAPMessage.Mid(aIndexArray.iIndexes[i],
                                            aIndexArray.iIndexes[i+1]-
                                            aIndexArray.iIndexes[i]));

                ParsedNumber = ParseNumber(Elem,ETrue,16);
                if( ParsedNumber == KErrNotFound )
                    return EFalse;
                switch(i)
                    {
                  case KIndexDestinationPort:
                      iDestinationPort = ParsedNumber;

                      break;
                  case KIndexSourcePort:
                      iSourcePort = ParsedNumber;
                      break;
                  case KIndexReferenceNumber:
                      iReference = ParsedNumber;
                      break;
                  case KIndexTotalSegments:
                      iTotalSegments = ParsedNumber;
                      break;
                  case KIndexSegmentNumber:
                      iSegmentNumber = ParsedNumber;
                      break;
                  default:
                      OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TWAPTEXTMESSAGE_PARSEWAPTEXTHEADER_2, "Hm. unhandled WAP index [%d]", i );
                      break;
                    }
                }
            }
        else
            {
            // elems have not a length defined in advance
            iOtherHeader = 0;
            iOtherHeaderLength = 0;

            // Search the terminating character ' '
            iData = iWAPMessage.Locate(' ');
            TInt dataTmp = iWAPMessage.Locate('\n');

            if (iData == KErrNotFound)
                {
                if (dataTmp == KErrNotFound)
                    return EFalse;
                else
                    iData = dataTmp;
                }
            else if (dataTmp != KErrNotFound)
                iData = Min(iData, dataTmp);

            // check the existence of other header
            // at least "// " should be there
            if (   iWAPMessage.Length() > aIndexArray.iIndexes[i]+2
                   && iWAPMessage[aIndexArray.iIndexes[i]] == '/'
                   && iWAPMessage[aIndexArray.iIndexes[i]+1] == '/')
                {
                iOtherHeader = aIndexArray.iIndexes[i];
                iOtherHeaderLength=iData-iOtherHeader;
                }

            // data: check if any characters after ' '
            iDataLength = 0;
            iData++;
            if (iWAPMessage.Length() > iData)
                {
                iDataLength = iWAPMessage.Length() - iData;
                }

            // That's it
            } // end of other header and data
        }// end of for loop
    return ETrue;
    } // TWapTextMessage::ParseWapTextHeader


/**
 *  Length of header is returned
 *  Sets internally the iTotalSegments
 *  Does not set the segment number into the aFixedHeader
 *  
 *  Length of complete header is returned.
 *  On return the header is not complete, if SAR is needed, because the segment
 *  number is not set. Thus aFixedHeader.Length() != (return value of this function) is true.
 *  The segment number can be set by inserting it into aSegmentNumberIndex position
 *  If SAR is not needed aSegmentNumberIndex = 0 and
 *  aFixedHeader.Length() == (return value of this function) is true
 */
TInt TWapTextMessage::CreateHeader(TDes8& aFixedHeader, TInt& aSegmentNumberIndex)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TWAPTEXTMESSAGE_CREATEHEADER_1, "TWapTextMessage::CreateHeader");

    // Index into KElemIndexes indicating which header elements are present
    TInt elemIndex;
    // Index into Indexes, for current header element
    TInt minorIndex=KIndexDestinationPort;
    TBuf8<4> hexNumber;

    // Segment number length is set, if SAR is needed
    TInt segmentNumberLength = 0;
    aSegmentNumberIndex = 0;

    // Determine whether long or short form is used
    if (iIs16Bit || iSourcePort > 255 || iDestinationPort>255)
        {
        elemIndex = 0;
        aFixedHeader.Copy(KSCKHeaderLong);
        }
    else
        {
        elemIndex = 3;
        aFixedHeader.Copy(KSCKHeaderShort);
        }

    // Set destination port
    hexNumber.NumFixedWidthUC(iDestinationPort,EHex,
                              KElemIndexes[elemIndex].iIndexes[minorIndex+1]-KElemIndexes[elemIndex].iIndexes[minorIndex]);
    aFixedHeader.Append(hexNumber);

    // Don't set the source port,
    // 1) if it is not set or it same as destination port and
    // 2) data (and other header) fits in one segment
    if (!((iSourcePort==-1 || iDestinationPort==iSourcePort)
          && CalculateTotalSegments(KElemIndexes[elemIndex].iIndexes[minorIndex+1])==1))
        {
        // Source port is present
        elemIndex++;
        minorIndex++;
        if (iSourcePort==-1)
            iSourcePort = iDestinationPort;

        // Set source port
        hexNumber.NumFixedWidthUC(iSourcePort,EHex,
                                  KElemIndexes[elemIndex].iIndexes[minorIndex+1]-KElemIndexes[elemIndex].iIndexes[minorIndex]);
        aFixedHeader.Append(hexNumber);

        // Add the SAR info when source port is set
        elemIndex++;

        // Set reference
        minorIndex++;
        hexNumber.NumFixedWidthUC(iReference,EHex,
                                  KElemIndexes[elemIndex].iIndexes[minorIndex+1]-KElemIndexes[elemIndex].iIndexes[minorIndex]);
        aFixedHeader.Append(hexNumber);

        // Set fragment count
        minorIndex++;
        CalculateTotalSegments(KElemIndexes[elemIndex].iIndexes[KElemIndexes[elemIndex].iLastIndex]);
        hexNumber.NumFixedWidthUC(iTotalSegments,EHex,
                                  KElemIndexes[elemIndex].iIndexes[minorIndex+1]-KElemIndexes[elemIndex].iIndexes[minorIndex]);
        aFixedHeader.Append(hexNumber);

        // Return the index for segment number in the header
        minorIndex++;
        aSegmentNumberIndex = KElemIndexes[elemIndex].iIndexes[minorIndex];
        segmentNumberLength = 2;
        }

    aFixedHeader.Append(iRefOtherHeader);
    aFixedHeader.Append(_L(" "));

    return aFixedHeader.Length()+segmentNumberLength;
    } // TWapTextMessage::CreateHeader


/**
 *  Calculates count of segments to send a used data (based on iRefData)
 *  The values is assigned to iTotalSegments
 *  The affect of terminating ' ' is taken into inside the method
 */
TInt TWapTextMessage::CalculateTotalSegments(TInt aFixedLength)
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TWAPTEXTMESSAGE_CALCULATETOTALSEGMENTS_1, "TWapTextMessage::CalculateTotalSegments [aFixedLength=%d]", aFixedLength);

    // '+1': length of terminating ' '
    TInt  length        = aFixedLength + iRefOtherHeader.Length() + 1;
    TInt  remain        = KMaxSmsChars - length;
    TInt  dataSegmented = 0;

    __ASSERT_DEBUG(remain > 0, Panic(KPanicTextHeaderTooLong));

    iTotalSegments = 0;
    do
        {
        iTotalSegments++;

        //
        // Count any escaped characters we can be sure that the converted data
        // size fits inside the remaining length (e.g. so that non-7bit characters
        // when converted by the SMS Stack will still fit).
        //
        TInt  segmentSize = iRefData.Length() - dataSegmented;

        if (segmentSize > remain)
            {
            segmentSize = remain;
            }

        while (segmentSize > 1)
            {
            TPtrC8  segmentData(iRefData.Mid(dataSegmented, segmentSize));
            TInt  non7bitCharEscapes = 0;
            
            //
            // Count all non-7bit characters that will be escaped (many non-7bit
            // characters are not escaped, but converted to "?"). The ones
            // that are known to be escaped are list below:
            //
            //    12  [Form Feed].
            //    91  "["
            //    92  "\"
            //    93  "]"
            //    94  "^"
            //   123  "{"
            //   124  "|"
            //   125  "}"
            //   126  "~"
            //
            for (TInt  ch = 0;  ch < segmentSize;  ch++)
                {
                if (segmentData[ch] == 12  ||
                    (segmentData[ch] >= 91  &&  segmentData[ch] <= 94)  ||
                    (segmentData[ch] >= 123  &&  segmentData[ch] <= 126))
                    {
                    non7bitCharEscapes++;
                    }
                }
            
            //
            // Can it fit? If so store it, otherwise reduce the size...
            //
            if (segmentData.Length() + non7bitCharEscapes <= remain)
                {
                break;
                }
            
            segmentSize--;
            }
        
            dataSegmented += segmentSize;
        }
    while (dataSegmented < iRefData.Length());
    
    //
    // At least one fragment is needed...
    //
    if (iTotalSegments == 0)
        {
        iTotalSegments = 1;
        }

    return iTotalSegments;
    } // TWapTextMessage::CalculateTotalSegments

// EOF - WAPTHDR.CPP
