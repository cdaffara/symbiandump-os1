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
// @file ctlbs.h
// This is a common header file for all test steps
// 
//

#ifndef __CT_LBS_H__
#define __CT_LBS_H__


// Can't include the internal header file for this, so must define here
const TUint KLbsModuleManagerPairUidValue		= 0x10281D44;

// lbsroot stuff (not ideal, currently defined in internal header file lbsrootcenrepdefs.h
const TUint KLbsCenRepUidValue					= 0x10282266;
const TUid  KLbsCenRepUid						= {KLbsCenRepUidValue};

const TUint32 KLbsRootProcessNameFirstKey			= 0x00001000;
const TUint32 KLbsRootProcessUidFirstKey			= 0x00002000;
const TUint32 KLbsRootProcessIsServerFirstKey		= 0x00003000;

const TUint32 KRootKeyMask							= 0xFFFFF000;	// don't care what the last 3 digits are

_LIT(KLbsTestPanic, "LBS Test Suite Panic");

#endif //__CT_LBS_H__
