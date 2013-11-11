/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*    Implementation for the CDosEventManager class
*
*/


#include <e32base.h>
#include "doseventmanager.h"

//
// ---------------------------------------------------------
// CDosEventManager::NewL
// ---------------------------------------------------------
//

CDosEventManager* CDosEventManager::NewL()
{
	CDosEventManager* result = new (ELeave) CDosEventManager;
	return result;
}

//
// ---------------------------------------------------------
// CDosEventManager Raising Event functions
// ---------------------------------------------------------
//

EXPORT_C TInt CDosEventManager::GeneralEvent(TInt32 aEvent,TAny* aParameter,TInt aParSize)
{
	return RaiseEvent(aEvent,aParameter,aParSize);
}
