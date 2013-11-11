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
//

/**
@file

Buffer Manager for Protocols
*/

#if !defined(__ES_MBUF_H__)
#define __ES_MBUF_H__

#include <e32cmn.h>
#include <e32ver.h>
#include <e32base.h>

#ifdef _DEBUG
// Include special test code
# define _MBUF_TEST
// Enables Alloc/Free sanity checks
#define _MBUF_CHECKS
#endif

//
// General class Cleanup stack support

#ifndef __KERNEL_MODE__
#define __DECLARE_CLEANUP \
	public: inline operator TCleanupItem() { return TCleanupItem(__Cleanup,this); } \
	private: IMPORT_C static void __Cleanup(TAny* aPtr);
#else
#define __DECLARE_CLEANUP
#endif

/**
General class Cleanup stack support
*/
#define __IMPLEMENT_CLEANUP(Class, Function) \
	EXPORT_C void Class::__Cleanup(TAny* aPtr) { ((Class*)aPtr)->Function(); }

class RMBuf;
class TMBufIter;
class RMBufChain;
class RMBufQ;
class RMBufPktQ;
class TMBufPktQIter;

#include <comms-infras/mbufpanic.h>
#include <comms-infras/mbufchain.h>
#include <comms-infras/mbuf.h>
#include <comms-infras/mbufq.h>
#include <comms-infras/mbufpktq.h>
#include <comms-infras/mbufasyncrequest.h>

#endif
