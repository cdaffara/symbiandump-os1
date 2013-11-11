// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#ifndef AVCTPPRIORITIES_H
#define AVCTPPRIORITIES_H

#include <e32base.h>

namespace SymbianAvctp
{

/**
This is the default priority for AVCTP Active Objects
*/
const TInt KDefaultPriority = CActive::EPriorityStandard;

/**
This is the priority for AVCTP Active Objects that deal with
sending and receiving user data.
*/
const TInt KDataPlanePriority = CActive::EPriorityStandard + 1;

} // end of namespace SymbianAvctp

#endif // AVCTPPRIORITIES_H 

