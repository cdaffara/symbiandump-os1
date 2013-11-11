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

#ifndef __ESCOMMSBUF_H__
#define __ESCOMMSBUF_H__

#include <e32cmn.h>
#include <e32ver.h>
#include <e32base.h>

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

#include <comms-infras/commsbuf.h>
#include <comms-infras/commsbufchain.h>
#include <comms-infras/commsbufiter.h>
#include <comms-infras/commsbufasyncrequest.h>
#endif // __ESCOMMSBUF_H__