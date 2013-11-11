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
 
#ifndef MMFDEVSOUNDSERVERCONSTS_H
#define MMFDEVSOUNDSERVERCONSTS_H

//#include <e32std.h>

/**
@SYMPatchable
@publishedPartner
 
Patchable constant for licencee to set devsound server thread priority
Licencees should ensure that the devsound server thread priority is equal to the system server priority for better responsiveness.
If SYMBIAN_CURB_SYSTEMSERVER_PRIORITIES is defined, setting devsound server thread priority to EPriorityAbsoluteHigh will ensure
devsound server's thread priority is set to system server's priority.
If the macro is unavailable it is recommended to use EPriorityAbsoluteRealTime1. Usage of EPriorityAbsoluteRealTime1 requires ProtServ capability.
The constant can be changed at ROM build time using patchdata OBY keyword.
 */

IMPORT_C extern const TInt KDevsoundServerPriority;

 #endif // MMFDEVSOUNDSERVERCONSTS_H

//End of file
