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
// This file contains the inline file of WAPDGRM.CPP
// 
//

/**
 @file
 @internalAll 
*/

#include <es_mbuf.h>

inline TBool CWapDatagram::IsComplete() const
    {
    return iIsComplete;
    }

inline void CWapDatagram::SegmentData(TSegmentData& aSegmentData) const
    {
    __ASSERT_DEBUG(iSegment != NULL,Panic(KPanicUsageError));
    if( iSegment )
        {
        iSegment->UserData(aSegmentData.iData);
        aSegmentData.iSegmentNumber = iSegmentNumber;
        }
    }

inline TInt CWapDatagram::WapDatagramLength() const
    {
	return (*iBuffer).Length();
    }

inline TInt CWapDatagram::WapDatagram(RMBufChain& aBufChain, TUint aMaxLength) const
    {
    __ASSERT_DEBUG(iIsComplete,Panic(KPanicUsageError));

 	TInt err = KErrNone;
    if (iBuffer)
        {
        TInt Length = Min(iBuffer->Length(), aMaxLength);
 		TRAP(err, aBufChain.AllocL(Length));
 		if(err == KErrNone)
 		    {
 			aBufChain.CopyIn(*iBuffer, 0);
 			err = 1;	// wrote one datagram
 			}
        }
 	return err;
    }

inline TPtrC CWapDatagram::ToAddress() const
    {
    return iToAddress;
    }

inline void CWapDatagram::SetToAddress(const TDesC& aToAddress)
    {
    iToAddress = aToAddress;
    }

inline TPtrC CWapDatagram::FromAddress() const
    {
    return iFromAddress;
    }

inline void CWapDatagram::SetFromAddress(const TDesC& aFromAddress)
    {
    iFromAddress = aFromAddress;
    }

inline void CWapDatagram::Ports(TInt& aFromPort,TInt& aToPort,TBool* aIs16Bit) const
    {
    aFromPort = iFromPort;
    aToPort = iToPort;
    if (aIs16Bit)
        *aIs16Bit = i16BitPorts;
    }

inline void CWapDatagram::SetPorts(TInt aFromPort, TInt aToPort,TBool aIs16Bit)
    {
    i16BitPorts = aIs16Bit;
    iFromPort = aFromPort;
    iToPort = aToPort;
    }

inline void CWapDatagram::OtherHeader(TDes8& aOtherHeader) const
    {
    aOtherHeader.Zero();
    if (iIsTextHeader)
        iSegment->OtherHeader(aOtherHeader);
    }

inline const TTime& CWapDatagram::Time() const
    {
    return iTime;
    }

inline TInt CWapDatagram::NumConcatenatedMessages() const
    {
    return iTotalSegments;
    }

inline TInt CWapDatagram::ConcatenatedMessageReference(TBool* aIs16Bit) const
    {
    if (aIs16Bit)
        *aIs16Bit = EFalse;
    return iReference;
    }

inline void CWapDatagram::
                SetConcatenatedMessageReference(TInt    aReference)
    {
    iReference = aReference;
    }

inline TSmsDataCodingScheme::TSmsAlphabet CWapDatagram::Alphabet() const
    {
    return iUserDataSettings.Alphabet();
    }

inline void CWapDatagram::SetAlphabet(TSmsDataCodingScheme::TSmsAlphabet aAlphabet)
    {
    __ASSERT_DEBUG(   aAlphabet == TSmsDataCodingScheme::ESmsAlphabet8Bit
                    || aAlphabet == TSmsDataCodingScheme::ESmsAlphabet7Bit,
                    Panic(KPanicUsageError));
    iUserDataSettings.SetAlphabet(aAlphabet);
    }

inline void CWapDatagram::SetStatusReportScheme(TSmsStatusReportScheme aScheme)
	{
    iScheme = aScheme;
	}
