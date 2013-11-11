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
// Description: Define Patchable constants 
//

#include <e32std.h>

//Care should be taken not to define the exported data in the same source file in which
//they are referred, lets the compiler inline's it. If inlined, the data cannot be patched.
//Hence, the definition of data must be put in a separate source file and they must be 
//referred in other source files by using 'extern' qualifier

#ifdef SYMBIAN_SSM_GRACEFUL_SHUTDOWN
EXPORT_C extern const TUint32 KSsmGracefulShutdown = 0x0;
#endif //SYMBIAN_SSM_GRACEFUL_SHUTDOWN

EXPORT_C extern const TUint32 KSsmGracefulOffline = 0x0;

EXPORT_C extern const TUint32 KSsmInvalidSim = 0x0;
