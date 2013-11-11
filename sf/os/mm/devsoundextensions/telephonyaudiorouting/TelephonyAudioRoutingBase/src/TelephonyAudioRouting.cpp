/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Base class for client to telephony audio routing/
*
*/



// INCLUDE FILES

#include "TelephonyAudioRouting.h"
#include "TelephonyAudioRoutingProxy.h"


EXPORT_C CTelephonyAudioRouting* CTelephonyAudioRouting::NewL(MTelephonyAudioRoutingObserver& aObserver)
    {
		CTelephonyAudioRoutingProxy* audioRoutingProxy = CTelephonyAudioRoutingProxy::NewL(aObserver);
		return audioRoutingProxy;         
    }




