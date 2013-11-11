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

#ifndef SDPPDU_H
#define SDPPDU_H

#include <e32base.h>
#include "sdpconsts.h"
#include "sdpclient.h"

/**
   Encapsulates an Sdp pdu.

   All SDP PDUs have the following format:
   @verbatim
   Field  |  PDU Id  |  Trans Id  |  ParamLength  |      Params...     |
   Length |  1 byte  |  2 bytes   |    2 bytes    | ParamLength bytes  | 
   @endverbatim

**/
NONSHARABLE_CLASS(CSdpPdu) : public CBase
	{
friend class CSdpClient;
public:
	CSdpPdu(CSdpNetDbProvider*	aSdpNetDbProv);
	virtual ~CSdpPdu();
	void SetPduId(TUint8 aPduId);
	TUint8 PduId();
	void SetTransid(TUint16 aTransId);
	TUint16 TransId();
	void SetLength();
	void PutByte(TUint8 aByte);
	void PutBigEndian16(TUint16 aShort);
	void PutBigEndian32(TUint32 aLong);
	void PutData(const TDesC8& aDes);
	TDesC8& Data();
	CSdpNetDbProvider*	NetDbProvider();
protected:
	// Fixme : this is the max packet size outbound
	TBuf8<KSdpMaxPduSize> iBuf;
	CSdpNetDbProvider*	iNetDbProvider;
	TDblQueLink iLink;
	};

#endif
