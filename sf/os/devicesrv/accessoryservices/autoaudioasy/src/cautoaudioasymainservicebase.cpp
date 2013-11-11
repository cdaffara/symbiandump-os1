/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Service base for autoaudioasy
*
*/
// global headers
#include <accessoryservicesinternalpskeys.h>
#include <autoaudiopskeys.h>

// local headers
#include "cautoaudioasymainservicebase.h"
#include "cautoaudioasyservice.h"
#include "acc_debug.h"


_LIT_SECURITY_POLICY_C1(KPSAutoAudioReadPolicy, ECapabilityReadDeviceData);
_LIT_SECURITY_POLICY_C1(KPSAutoAudioWritePolicy, ECapabilityWriteDeviceData);

// ----------------------------------------------------------------------------------
// CAutoAudioAsyMainServiceBase::CAutoAudioAsyMainServiceBase ()
// ----------------------------------------------------------------------------------
CAutoAudioAsyMainServiceBase::CAutoAudioAsyMainServiceBase ()
    {
    COM_TRACE_ ( "[AccFW:AutoAudioAsy] CAutoAudioAsyMainServiceBase::CAutoAudioAsyMainServiceBase()");
    }

// ----------------------------------------------------------------------------------
// CAutoAudioAsyMainServiceBase::NewL ()
// ----------------------------------------------------------------------------------
CAutoAudioAsyMainServiceBase* CAutoAudioAsyMainServiceBase::NewL ()
    {
    CAutoAudioAsyMainServiceBase* self = new ( ELeave ) CAutoAudioAsyMainServiceBase ();
    CleanupStack::PushL ( self );
    self->ConstructL ();
    CleanupStack::Pop ( self );
    return self;
    }

void CAutoAudioAsyMainServiceBase::ConstructL ()
    {
    iService = CAutoAudioAsyService::NewL ();
    }

// ----------------------------------------------------------------------------------
// CAutoAudioAsyMainServiceBase::~CAutoAudioAsyMainServiceBase ()
// ----------------------------------------------------------------------------------
CAutoAudioAsyMainServiceBase::~CAutoAudioAsyMainServiceBase ()
    {
    COM_TRACE_ ( "[AccFW:AutoAudioAsy] CAutoAudioAsyMainServiceBase::~CAutoAudioAsyMainServiceBase" );
    delete iService;	
	RProperty::Delete ( KPSUidAccessoryServices, KPSAutoKeyRTPStreamingConnectionStatus );
    }

// ----------------------------------------------------------------------------------
// CAutoAudioAsyMainServiceBase::StartL ()
// In this method we define & start listening for property changes
// ----------------------------------------------------------------------------------
TInt CAutoAudioAsyMainServiceBase::StartL ()
	{
    COM_TRACE_ ( "[AccFW:AutoAudioAsy] CAutoAudioAsyMainServiceBase::StartL" );
	InitPSAutoAudioL ();
	
	iService->StartL ();
	
	Signal ();
    
	return KErrNone;
	}
	
// ----------------------------------------------------------------------------------
// CAutoAudioAsyMainServiceBase::InitPSAutoAudioL ()
// In this method we define and intialise P&S keys
// ----------------------------------------------------------------------------------
void CAutoAudioAsyMainServiceBase::InitPSAutoAudioL ()
	{
    COM_TRACE_ ( "[AccFW:AutoAudioAsy] CAutoAudioAsyMainServiceBase::InitPSAutoAudioL" );
	TInt ret = RProperty::Define ( KPSUidAccessoryServices,
				KPSAutoKeyRTPStreamingConnectionStatus, RProperty::EInt,				
				KPSAutoAudioReadPolicy, KPSAutoAudioWritePolicy );

	if ( ret != KErrNone )
		{
		COM_TRACE_1 ( "[AccFW:AutoAudioAsy] CAutoAudioAsyMainServiceBase::InitPSAutoAudioL RTP Streaming Property Define failed with %d", ret );
        User::Leave ( ret );
		}
	
	ret = RProperty::Set ( KPSUidAccessoryServices, KPSAutoKeyRTPStreamingConnectionStatus, EAudioConnectionStatusNone );
	if ( ret != KErrNone )
	    {
        COM_TRACE_1 ( "[AccFW:AutoAudioAsy] CAutoAudioAsyMainServiceBase::InitPSAutoAudioL RTP Streaming key init failed with %d", ret );
        User::Leave ( ret );
	    }
	}
