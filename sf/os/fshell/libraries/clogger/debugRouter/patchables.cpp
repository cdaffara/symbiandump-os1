// patchables.cpp
// 
// Copyright (c) 2007 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <e32def.h>
#include <e32const.h>

/* Patchable constants, to specify the size of the chunk used to transfer trace data from the kernel to the cloggerserver,
 * and the size of the buffer used to buffer logging from ISRs
 *
 * To modify, include the following in your OBY file: (the following sets the chunk size to 128KB)
 *     patchdata clogger-debugrouter.ldd@KChunkSize 0x20000
 * Note: In order for this to work, the LDD must be in the core ROM image, ie the IBY file for
 * clogger-debugrouter.ldd must start device[VARID]=ABI_DIR\BUILD_DIR\clogger-debugrouter.ldd

 */

EXPORT_C extern const TInt KChunkSize = 64 * 1024; // 64KB for now
EXPORT_C extern const TInt KIsrBufSize = 2048;

EXPORT_C extern const TBool KEnableEarlyRdebug = EFalse; // Alternative to messing with the debug port - inventing new port numbers can confuse some baseports
