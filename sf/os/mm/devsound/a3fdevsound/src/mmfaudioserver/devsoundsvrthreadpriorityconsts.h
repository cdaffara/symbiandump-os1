// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef DEVSOUNDSVRTHREADPRIORITYCONSTS_H
#define DEVSOUNDSVRTHREADPRIORITYCONSTS_H

/**
@SYMPatchable
@publishedPartner
 
Patchable constant for licencee to set Devsound server thread priority
The constant can be changed at ROM build time using patchdata OBY keyword.
 */
IMPORT_C extern const TThreadPriority KDevsoundSvrPriority;

#endif // DEVSOUNDSVRTHREADPRIORITYCONSTS_H

// End of File
