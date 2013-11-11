// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CompatAids_h
#define __CompatAids_h

// The __PANIC_COMPAT macro is used to determine the action when client code calls an old mediasvr
// function that is not supported under the new Multimedia components (MMF/ICL etc). The default
// behaviour is to panic on debug mode but "return" KErrNotSupported for release. The macro can
// be changed this default.

#define __PANIC_COMPAT(result) __ASSERT_DEBUG((EFalse),(result))

// __PANIC_COMPAT_IS_ACTIVE should be defined for what ever scenarios __PANIC_COMPAT might actually
// call panic - eg. by default, for debug builds. This allows the Panic() calls of particular sub-modules
// to be only defined if they are calls - thus avoiding warnings that would otherwise result.

#if defined(_DEBUG)
#define __PANIC_COMPAT_IS_ACTIVE
#endif

#endif // __CompatAids_h
