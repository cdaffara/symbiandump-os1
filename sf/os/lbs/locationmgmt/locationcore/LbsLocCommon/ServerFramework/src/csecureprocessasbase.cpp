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

#include "csecureprocessasbase.h"

/** Panic the secure process

@param aPanic A reference to TSecureProcessASBasePanic object
@see TSecureProcessASBasePanic
@internalTechnology
@released
 */
void CSecureProcessASBase::Panic(const TSecureProcessASBasePanic aPanic)
	{
	_LIT(KSecureProcessAsBase, "CSecureProcessASBase");
	User::Panic(KSecureProcessAsBase, aPanic);
	}
	
