// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "hcieventmodifiable.h"
#include <bluetooth/hci/event.h>

void THCIEventModifiable::SetErrorCode(THCIErrorCode aErrorCode)
	{
	iErrorCode = aErrorCode;
	}

TPtrC8& THCIEventModifiable::EventData()
	{
	return iEventData;
	}

THCIEventModifiable::THCIEventModifiable()
   : THCIEventBase(KNullDesC8)
	{
	// This constructor should never be called, as this
	// class is only used by casting from THCIEventBase object
	User::Panic(_L("THCIEventModifiable Constructor"), EIllegalConstructorUse);
	}

