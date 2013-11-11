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

#include <comms-infras/commsbufponddbg.h>
#include <comms-infras/commsbufpond.h>
#include "commsbufpondintf.h"

EXPORT_C TCommsBufPondDbg::TCommsBufPondDbg(RCommsBufPond& aPond)
: iCommsBufPondDbg(aPond.iPondImpl->CommsBufPondDbg())
	{
	}

EXPORT_C RCommsBuf* TCommsBufPondDbg::__DbgBufChain()
	{
	return iCommsBufPondDbg.__DbgBufChain();
	}

EXPORT_C RCommsBuf* TCommsBufPondDbg::__DbgBufChain(TUint aBufSize)
	{
	return iCommsBufPondDbg.__DbgBufChain(aBufSize);
	}

EXPORT_C void TCommsBufPondDbg::__DbgSetPoolLimit(TInt aCount)
	{
	iCommsBufPondDbg.__DbgSetPoolLimit(aCount);
	}

EXPORT_C void TCommsBufPondDbg::__DbgSetPoolLimit(TInt aCount, TUint aBufSize)
	{
	iCommsBufPondDbg.__DbgSetPoolLimit(aCount, aBufSize);
	}

EXPORT_C void TCommsBufPondDbg::__DbgSetFailAfter(TInt aCount/*=0*/)
	{
	iCommsBufPondDbg.__DbgSetFailAfter(aCount);
	}

EXPORT_C TUint TCommsBufPondDbg::__DbgGetBufSpace()
	{
	return iCommsBufPondDbg.__DbgGetBufSpace();
	}

EXPORT_C TUint TCommsBufPondDbg::__DbgGetBufSpace(TUint aBufSize)
	{
	return iCommsBufPondDbg.__DbgGetBufSpace(aBufSize);
	}

EXPORT_C TUint TCommsBufPondDbg::__DbgGetBufTotal()
	{
	return iCommsBufPondDbg.__DbgGetBufTotal();
	}

EXPORT_C TUint TCommsBufPondDbg::__DbgGetBufTotal(TUint aMufSize)
	{
	return iCommsBufPondDbg.__DbgGetBufTotal(aMufSize);
	}

EXPORT_C TInt TCommsBufPondDbg::__DbgGetHeapSize()
	{
	return iCommsBufPondDbg.__DbgGetHeapSize();
	}
