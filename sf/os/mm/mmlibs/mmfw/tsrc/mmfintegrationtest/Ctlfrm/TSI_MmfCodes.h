// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// Standard EPOC32 includes required by this header file
#include <e32base.h>
#include <e32std.h>


#ifndef __TSI_MMFCODES_H__
#define __TSI_MMFCODES_H__

// These are Codes which are passed to and from functions via the Controller Framework.

const TInt KGetAudioQuality = 1;
const TInt KSetAudionQuality = 2;
const TInt KDummyFunc1 = 3;
const TInt KDummyFunc1Return = -4;
const TInt KDummyFunc2 = 5;
const TInt KDummyFunc2Return = -6;
const TInt KLogFunction = 7;
const TInt KFuncComplete = 0;
const TInt KModeFunction = 8;

const TInt KHeapAddressFunction = 21;
const TInt KThreadIdFunction = 22;
const TInt KPluginUidFunction = 23;



//-----------------------------//
// Leave Codes (Failure Tests) //
//-----------------------------//

const TInt KOpenFailure = 201;
const TInt KAddDataSourceFailure = 202;
const TInt KAddDataSinkFailure = 203;
const TInt KRemoveDataSourceFailure = 204;
const TInt KRemoveDataSinkFailure = 205;
const TInt KPrimeFailure = 206;
const TInt KFailureCode1 = 210;
const TInt KFailureCode2 = 211;


#endif // __TSI_MMFCODES_H__
