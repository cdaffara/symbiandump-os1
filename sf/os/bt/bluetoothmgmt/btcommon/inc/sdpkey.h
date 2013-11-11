// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SDPKEY_H
#define SDPKEY_H

#include <e32base.h>
#include <bttypes.h>
#include "sdpconsts.h"
//#include "sdpclient.h"


//
// SDP Query specifications
//

const static TUint KSDPConnectQuery				= 1;
const static TUint KSDPServiceQuery				= 2;
const static TUint KSDPAttributeQuery			= 3;
const static TUint KSDPEncodedQuery				= 4;
const static TUint KSDPCancelQuery				= 5;
const static TUint KSDPRetrieveResultQuery		= 0x7f;
const static TUint KSDPBufferResult             = 0x80;

struct TSDPConnectQuery
    {
	TUint		iQueryType;	//< Must be set to KSDPConnectQuery
	TBTDevAddr	iAddr;		//< The address to contact
    };
typedef TPckgBuf<TSDPConnectQuery> TSDPConnectBuf;

struct TSDPServiceSearchKey
    {
    TUint		iQueryType;  //< Must be set to KSDPServiceQuery
    TUint16  	iMaxCount;   //< Maximum number of records to return
    TUUID		iUUID;       //< The UUID to match
	TUint8		iStateLength;//< Length of the continuation state
    TUint8		iContinuationState[KSdpContinuationStateMaxLength];//< 0 to 16 bytes of continuation state
	};
typedef TPckgBuf<TSDPServiceSearchKey> TSDPServiceSearchKeyBuf;

struct TSDPAttributeKey
	{
    TUint		iQueryType;   //< Must be set to KSDPAttributeQuery
	TUint       iServiceRecordHandle;//<Handle returned from ServiceSearch Query
	TUint16		iMaxLength;   //< Max number of bytes to return
	TBool		iRange;       //< Is the attribute number a range?
	/** Attribute - interpreted as a range if iRange is true,
		else bottom 16 bits are the attribute number **/
	TUint		iAttribute;
	TUint8		iStateLength;//< Length of the continuation state
	TUint8		iContinuationState[KSdpContinuationStateMaxLength];//< 0 to 16 bytes of continuation state
	};

typedef TPckgBuf<TSDPAttributeKey> TSDPAttributeKeyBuf;

struct TSDPEncodedKey
	{
	TUint iQueryType;
	TUint8 iPduId;
	};

typedef TPckgBuf<TSDPEncodedKey> TSDPEncodedKeyBuf;


#endif
