// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// e32utils\setcap\setcap.h
// 
//

#ifndef __SETCAP_H__
// We need to build for both TOOLS2 and TOOLS for the moment
#if !defined(__TOOLS2_LINUX__)
#include <e32wins.h>
#include <emulator.h>
#endif
#include <stdlib.h>

#ifndef __WINS__
#include <f32image.h>
#endif


__ASSERT_COMPILE(sizeof(SCapabilitySet)==sizeof(TInt64));  // When parsing capabilities we assume that it's a 64bit quantity

GLREF_D TBool CapabilitySet;
GLREF_D SCapabilitySet Capability;
GLREF_D TBool SecureIdSet;
GLREF_D TSecureId SecureId;
GLREF_D TBool VendorIdSet;
GLREF_D TVendorId VendorId;

#if !defined(__EPOC32__) && !defined(__LINUX__)
GLREF_C TInt SetCap(HANDLE hFile);
#endif //!__EPOC32__

#ifndef __WINS__
GLREF_C TInt SetCap(E32ImageHeader* h);
#endif //!__WINS__

#endif //__SETCAP_H__

