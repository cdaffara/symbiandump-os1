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
// This file contains the inline methods of TWapTextMessage.
// TWapTextMessage encodes and decodes 7-bit WAP text headers
// 
//

/**
 @file
 @internalAll
*/

#include "wapmain.h"


/**
 *  return the destination port of this WAP message
 *  
 *  @param aIs16Bit Pointer. If true on return, destination port is 16 bits
 *  @return the destination port
 */
inline TInt TWapTextMessage::DestinationPort(TBool* aIs16Bit) const
    {
    __ASSERT_DEBUG(iIsWapTextMessage, Panic(KPanicTextHeaderNotTextHeader));
    if (aIs16Bit)
        *aIs16Bit = iIs16Bit;
    return iDestinationPort;
    }


/**
 *  set the destination port of this WAP message
 *  
 *  @param aToPort Destination port
 *  @param aIs16Bit TRUE if the dest port is a 16-bit value
 */
inline void TWapTextMessage::SetDestinationPort(const TInt aToPort, const TBool aIs16Bit)
    {
    iDestinationPort = aToPort;
    // Don't override a true value
    if (!iIs16Bit)
        iIs16Bit = aIs16Bit;
    }


/**
 *  Return the source port of this WAP message
 *  if not present, value of destination port is returned
 *  
 *  @param aIs16Bit Pointer. If true on return, source port is 16 bits
 *  @return the source port
 */
inline TInt TWapTextMessage::SourcePort(TBool* aIs16Bit) const
    {
    __ASSERT_DEBUG(iIsWapTextMessage, Panic(KPanicTextHeaderNotTextHeader));
    if (aIs16Bit)
        *aIs16Bit = iIs16Bit;
    if (iSourcePort == (-1))
        return iDestinationPort;
    return iSourcePort;
    }


/**
 *  Set the source port of this WAP message
 *  
 *  do not set the source port or set it to -1,
 *  if you choose the omit and assume that the wap datagram would
 *  fit in one segment
 *  
 *  @param aSourcePort Source port
 *  @param aIs16Bit TRUE if the dest port is a 16-bit value
 */
inline void TWapTextMessage::SetSourcePort(const TInt aSourcePort, const TBool aIs16Bit)
    {
    iSourcePort = aSourcePort;
    // Don't override a true value
    if (!iIs16Bit)
        iIs16Bit = aIs16Bit;
    }


/**
 *  Return the reference number for this WAP message
 *  
 *  @note 0 is a valid number
 */
inline TInt TWapTextMessage::ReferenceNumber() const
    {
    __ASSERT_DEBUG(iIsWapTextMessage,Panic(KPanicTextHeaderNotTextHeader));
    return iReference;
    }


/**
 *  Set the reference number for this WAP message
 *  
 *  @note 0 is a valid number
 */
inline void TWapTextMessage::SetReferenceNumber(const TInt aReference)
    {
    iReference = aReference%256; // 8 bit ref number
    }


/**
 *  Return the total number of segments for this WAP message
 *  1 is returned, if SAR is not present (but port info is)
 */
inline TInt TWapTextMessage::TotalSegments() const
    {
    __ASSERT_DEBUG(iIsWapTextMessage,Panic(KPanicTextHeaderNotTextHeader));
    return iTotalSegments;
    }


/**
 *  return the segment number for this WAP message
 *  
 *  Valid values start from 1.
 *  1 is returned,if SAR is not present (but port info is)
 */
inline TInt TWapTextMessage::SegmentNumber() const
    {
    __ASSERT_DEBUG(iIsWapTextMessage,Panic(KPanicTextHeaderNotTextHeader));
    return iSegmentNumber;
    }


/**
 *  from WAP spec: other header specified by text mode of WAP spec
 *  
 *  Descriptor lenght is set 0, if not present
 *  '//' is included
 */
inline void TWapTextMessage::OtherHeader(TDes8&      aOtherHeader) const
    {
    __ASSERT_DEBUG(iIsWapTextMessage,Panic(KPanicTextHeaderNotTextHeader));
    aOtherHeader.Append(iWAPMessage.Mid(iOtherHeader,iOtherHeaderLength));
    }


/**
 *  Set the 'other header' field
 */
inline void TWapTextMessage::SetOtherHeader(const TDesC8& aOtherHeader)
    {
    iRefOtherHeader.Set(aOtherHeader);
    }


/**
 *  Gets the user data
 *  If the descriptor contains already data, the new data is appended.
 */
inline void TWapTextMessage::UserData(TDes8& aWAPUserData) const
    {
    aWAPUserData.Append(iWAPMessage.Mid(iData,iDataLength));
    }


/**
 *  Set the user data
 */
inline void TWapTextMessage::SetUserData(const TDesC8& aWAPUserData)
    {
    iRefData.Set(aWAPUserData);
    }


// EOF - WAPTHDR.INL
