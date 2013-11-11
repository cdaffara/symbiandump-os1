// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <btsdp.h>

// Template class TSdpIntBuf

EXPORT_C TSdpIntBuf<TUint64>::TSdpIntBuf(const TUint64 &aRef)
	{
	DoSetLength(sizeof(TUint64));
	TUint64 num(aRef);
	for(TInt i=7;i>=0;i--)
		{
		iBuf[i]=TUint8(num & 0xff);
		num>>=8;
		}
	}


EXPORT_C TSdpIntBuf<TUint32>::TSdpIntBuf(const TUint32 &aRef)
: TSdpIntBuf<TUint>(aRef)
	{
	}

EXPORT_C TSdpIntBuf<TUint16>::TSdpIntBuf(const TUint16 &aRef)
: TSdpIntBuf<TUint>(0)
	{
	DoSetLength(sizeof(TUint16));
	TUint16 num(aRef);
	iBuf[1] = TUint8(num & 0xff);
	num >>= 8;
	iBuf[0] = TUint8(num & 0xff);
	}

EXPORT_C TSdpIntBuf<TUint8>::TSdpIntBuf(const TUint8 &aRef)
: TSdpIntBuf<TUint>(0)
	{
	DoSetLength(sizeof(TUint8));
	TUint8 num(aRef);
	iBuf[0] = TUint8(num & 0xff);
	}
	
	
// EOF
