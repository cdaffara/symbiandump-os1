/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



#ifndef TC_CONFIG_H
#define TC_CONFIG_H

// maximum descriptor length
#define MAX_LEN 256

// maximum number of variables, increase this if you need more
#define MAX_VAR_COUNT 20

// maximum number of arguments that methods can have
#define MAX_ARG_COUNT 15

// maximum number of variable factories, increase this if you import
// many testwrapper/monitor DLLs at the same time
#define MAX_IMPORT_COUNT 20

// Time when timer will expire (100 sec.)
// This is used to recovery when test case is stuck on a WaitForRequest
// that will never return. If you need to wait for request with wait command
// where wait time is very long increase this number.
#define EXPIRE_TIME  1000000000



#include <e32base.h>


#endif // TC_CONFIG_H
