// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef PDUHANDLER_H
#define PDUHANDLER_H

#include <e32std.h>
#include <es_sock.h>
#include "sdpconsts.h"
#include "ServiceSearchVisitor.h"
#include "ExtractorVisitor.h"

// 4 byte offset, 4 byte full length, 2 byte CRC
#define KSdpContinuationStateLength 10

#define KContStateHeader 1

#define KRecHandleOffset 0
#define KRecHandleCountSize 2
#define KAttributeCountOffset 4
#define KRecAttribListOffset 6

#define KRspTotalCountOffset 0
#define KRspHandleCountSize 2
#define KRspHandleCountOffset 2
#define KRspAttrCountOffset 0
#define KRspAttributeCountSize 2
#define KRspAttributeCountOffset 0
#define KRspAttributeListOffset 2
#define KContContOff 0
#define KContTotOff 4
#define KContCrcOff 8

#define KAttributeIdSize 3
#define KAttrIdHeader 0x09

#define KMaxThreeByteDESSize 0xffff
#define KMaxTwoByteDESSize 0xff
#define KTwoByteDESSize 2
#define KThreeByteDESSize 3
#define KFiveByteDESSize 5

//class CSdpListener;
class CSdpConnection;
class TSdpPdu;

NONSHARABLE_CLASS(CSdpPDUHandler)
{
public:
	CSdpPDUHandler(){iDesSize=0;}
	static CSdpSearchPattern* UUIDListLC(const TDesC8& aPdu, const TInt aNextSize, TInt& aLen, TInt& aRem, TInt& aCount);
	static CSdpAttrIdMatchList* AttrListLC(const TDesC8& aPdu, TInt& aRem);
	TBool ContinuationL(const TDesC8& aPdu, const TInt aLen, const TInt aRem);
	void CompleteOPL(TDes8& aPdu, const TDesC8& aWritePdu, const TInt aMaxAttrCount);
	TUint32 ContinuationOffset();
	TUint32 FullLength();
	TUint16 ReqCRC();
	static TInt DesSize(TUint aDataSize);
private:
	TUint iDesSize;
	TUint iRespSize;	// continuation
	TUint32 iContinuation;
	TUint32 iLength;
	TUint16 iCRC;	// I wish
// epocsvr.cpp
//  line 703 update database size when record added.
// lines 730, update database when record deleted (this code is wrong, anyway)
//	lines 781, 831, update database when attribute amended, deleted.
//  
};

#endif
