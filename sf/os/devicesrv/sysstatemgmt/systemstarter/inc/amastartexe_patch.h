// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __AMASTARTEXE_PATCH_H__
#define __AMASTARTEXE_PATCH_H__
/**
This patchable constant contains the SID of the program that can call amastart.exe.
@publishedPartner
@released
*/
IMPORT_C extern const TInt KSysStartSID; // SID of the calling process - can be patched

#endif // __AMASTARTEXE_PATCH__
