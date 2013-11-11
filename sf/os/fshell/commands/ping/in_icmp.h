// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef __IN_ICMP_H__
#define __IN_ICMP_H__

#include "in_ip.h"

class ThdrICMP_Echo
{
public:
	inline static ThdrICMP_Echo* Ptr(ThdrICMP* aHdr);
	inline TUint16 NetGetId();
	inline void NetSetId(TUint16 aId);
	inline TInt GetId();
	inline void SetId(TUint16 aId);
	inline TUint16 NetGetSeq();
	inline void NetSetSeq(TUint16 aSeq);
	inline TInt GetSeq();
	inline void SetSeq(TUint16 aSeq);
protected:
	union
		{
		TUint8	iData8[8];
		TUint16	iData16[4];
		TUint32	iData32[2];
		} u;
};


// specific for ECHO calls. (id and seq fields)
//Net order
inline TUint16 ThdrICMP_Echo::NetGetId()
	{ return u.iData16[2]; }
inline void ThdrICMP_Echo::NetSetId(TUint16 aId)
	{ u.iData16[2] = aId; }
//Host order
inline TInt ThdrICMP_Echo::GetId()
	{ 
	return (u.iData8[4] << 8) + u.iData8[5];}
inline void ThdrICMP_Echo::SetId(TUint16 aId)
	{
		u.iData8[4] = (TUint8)(aId >> 8);
		u.iData8[5] = (TUint8)aId;
	}
//Net order
inline TUint16 ThdrICMP_Echo::NetGetSeq()
	{ return u.iData16[3]; }
inline void ThdrICMP_Echo::NetSetSeq(TUint16 aSeq)
	{ u.iData16[3] = aSeq; }
//Host order
inline TInt ThdrICMP_Echo::GetSeq()
	{ return (u.iData8[6] << 8) + u.iData8[7];}
inline void ThdrICMP_Echo::SetSeq(TUint16 aSeq)
	{
		u.iData8[6] = (TUint8)(aSeq >> 8);
		u.iData8[7] = (TUint8)aSeq;
	}


#endif
