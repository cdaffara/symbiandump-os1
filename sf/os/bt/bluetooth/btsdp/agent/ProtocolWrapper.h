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

#ifndef BTSDPAGENTWRAPPER_H
#define BTSDPAGENTWRAPPER_H

#include <e32base.h>
#include <bttypes.h>
#include "sdpkey.h"


/**
   Interface, internal to SDP agent DLL, used for direct prodding of a remote database.
**/
NONSHARABLE_CLASS(RSdpAgent)
	{
public:
	RSdpAgent();
	TInt Open(RSdpSession& aSession);
	void Close();
	TBool IsOpen();
	void Connect(TBTDevAddr aRemoteAddr, TRequestStatus& aStatus);
	void SdpSearchRequestL(TInt& aResultSize, 
						   CSdpAttrValueDES& aUUIDList,
						   TUint16 aMaxRecCount, 
						   const TDesC8& aContState,
						   TRequestStatus& aStatus);
	void SdpSearchRequestL(TInt& aResultSize, 
						   CSdpSearchPattern& aUUIDFilter,
						   TUint16 aMaxRecCount, 
						   const TDesC8& aContState,
						   TRequestStatus& aStatus);
	void SdpAttributeRequestL(TInt& aResultSize, 
							  TSdpServRecordHandle aHandle, 
						      TUint16 aMaxAttrByteCount,
							  CSdpAttrIdMatchList& aMatchList,
							  const TDesC8& aContState,
							  TRequestStatus& aStatus);
	TInt RetrieveResult(TDes8& aResult);
	void Cancel();

private:
	TBTDevAddr iDevAddr;
	HBufC8* iRequestBuf;
	TPckg<TInt> iResultSize;
	RNetDatabase iNetDb;
	TSDPConnectBuf iConnectBuf;
	};

#endif
