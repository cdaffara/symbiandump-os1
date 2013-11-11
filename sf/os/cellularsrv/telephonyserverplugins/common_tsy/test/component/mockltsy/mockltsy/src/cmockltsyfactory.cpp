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

#include "cmockltsyfactory.h"
#include "cmmmessagerouter.h"
#include "e32property.h"
#include "cmmtelephonyaudiocontrol.h"
#include <ctsy/pluginapi/mtelephonyaudiocontrolcallback.h>

CMockLtsyFactoryV1* CMockLtsyFactoryV1::NewL()
	{
	CMockLtsyFactoryV1* self = new (ELeave) CMockLtsyFactoryV1;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}

CMockLtsyFactoryV1::CMockLtsyFactoryV1()
	{
	}
	
void CMockLtsyFactoryV1::ConstructL()
	{
	}

CMockLtsyFactoryV1::~CMockLtsyFactoryV1()
	{
	delete iMessageRouter;
	}
	
MLtsyFactoryBase::TCtsyInterfaceVersion CMockLtsyFactoryV1::Version()
    {
    return MLtsyFactoryBase::EIfVersion1;
    }
	
void CMockLtsyFactoryV1::Release()
	{
	delete this;
	}

MMessageRouter* CMockLtsyFactoryV1::GetMessageRouter(MmMessageManagerCallback& aMessageManager)
	{
	if (iMessageRouter == NULL)
		{
		iMessageRouter = CMmMessageRouter::NewL(aMessageManager);
		}
	return iMessageRouter;
	}

#ifdef USING_CTSY_DISPATCHER
	CTsySatMessagingBase* CMockLtsyFactoryV1::GetSatMessagingService(MCtsySatService &aSatMessagingService)
		{
		return NULL;
		}
#else
	CTsySatMessagingBase* CMockLtsyFactoryV1::GetSatMessagingService( )
		{
		return NULL;
		}

#endif

CMockLtsyFactoryV2* CMockLtsyFactoryV2::NewL()
	{
	CMockLtsyFactoryV2* self = new (ELeave) CMockLtsyFactoryV2;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}

CMockLtsyFactoryV2::CMockLtsyFactoryV2():
iMessageRouter( NULL ),
iTelephonyAudioControl( NULL )
	{
	}
	
void CMockLtsyFactoryV2::ConstructL()
	{
	}

CMockLtsyFactoryV2::~CMockLtsyFactoryV2()
	{
	delete iTelephonyAudioControl;
	delete iMessageRouter;
	}
	
MLtsyFactoryBase::TCtsyInterfaceVersion CMockLtsyFactoryV2::Version()
    {
    return MLtsyFactoryBase::EIfVersion2;
    }
	
void CMockLtsyFactoryV2::Release()
	{
	delete this;
	}

MMessageRouter* CMockLtsyFactoryV2::GetMessageRouter(MmMessageManagerCallback& aMessageManager)
	{
	if (iMessageRouter == NULL)
		{
		TRAP_IGNORE( iMessageRouter = CMmMessageRouter::NewL(aMessageManager) );
		}
	return iMessageRouter;
	}

#ifdef USING_CTSY_DISPATCHER
CTsySatMessagingBase* CMockLtsyFactoryV2::GetSatMessagingService(MCtsySatService &aSatMessagingService)
	{
	return NULL;
	}
#else
CTsySatMessagingBase* CMockLtsyFactoryV2::GetSatMessagingService( )
	{
	return NULL;
	}
#endif

MTelephonyAudioControl* CMockLtsyFactoryV2::GetTelephonyAudioControl( MTelephonyAudioControlCallback* aTelephonyAudioControlCallback )
    {
	if (iTelephonyAudioControl == NULL)
		{
        TRAP_IGNORE( iTelephonyAudioControl = CMockTelephonyAudioControl::NewL( aTelephonyAudioControlCallback ) );
		}
    return iTelephonyAudioControl;
    }

CMockLtsyFactoryV3* CMockLtsyFactoryV3::NewL()
	{
	CMockLtsyFactoryV3* self = new (ELeave) CMockLtsyFactoryV3;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}
	
CMockLtsyFactoryV3::CMockLtsyFactoryV3():
iMessageRouter( NULL ),
iTelephonyAudioControl( NULL )
	{
	}
	
void CMockLtsyFactoryV3::ConstructL()
	{
	}

CMockLtsyFactoryV3::~CMockLtsyFactoryV3()
	{
	delete iTelephonyAudioControl;
	delete iMessageRouter;
	}

MLtsyFactoryBase::TCtsyInterfaceVersion CMockLtsyFactoryV3::Version()
    {
    return MLtsyFactoryBase::EIfVersion3;
    }
	
void CMockLtsyFactoryV3::Release()
	{
	delete this;
	}
	
MMessageRouter* CMockLtsyFactoryV3::GetMessageRouter(MmMessageManagerCallback& aMessageManager)
	{
	if (iMessageRouter == NULL)
		{
		TRAP_IGNORE( iMessageRouter = CMmMessageRouter::NewL(aMessageManager) );
		}
	return iMessageRouter;
	}

#ifdef USING_CTSY_DISPATCHER
CTsySatMessagingBase* CMockLtsyFactoryV3::GetSatMessagingService(MCtsySatService &aSatMessagingService)
	{
	return NULL;
	}
#else
CTsySatMessagingBase* CMockLtsyFactoryV3::GetSatMessagingService( )
	{
	return NULL;
	}
#endif

MTelephonyAudioControl* CMockLtsyFactoryV3::GetTelephonyAudioControl( MTelephonyAudioControlCallback* aTelephonyAudioControlCallback )
    {
	if (iTelephonyAudioControl == NULL)
		{
        TRAP_IGNORE( iTelephonyAudioControl = CMockTelephonyAudioControl::NewL( aTelephonyAudioControlCallback ) );
		}
    return iTelephonyAudioControl;
    }
   
TUint32 CMockLtsyFactoryV3::GetLTSYStaticCapabilities( TUint32 aRequestedCapabilitySet )
	{
	TUint32 ret = 0;
        
	if ( KStaticCapsPacketService == aRequestedCapabilitySet ) 
		{
		const TUint KPacketServiceStaticCaps
        	= RPacketService::KCapsSuspendSupported
        	//| RPacketService::KCapsAASupported
        	| RPacketService::KCapsNetworkAvailabilitySupported
        	| RPacketService::KCapsSetDefaultContextSupported
        	| RPacketService::KCapsChangeAttachModeSupported
        	| RPacketService::KCapsGetDataTransferredSupported
        	//| RPacketService::KCapsNotifyDataTransferredSupported
        	| RPacketService::KCapsPreferredBearerSupported
        	| RPacketService::KCapsPdpDataCompSupported
        	| RPacketService::KCapsPdpHeaderCompSupported
        	| RPacketService::KCapsMSClassSupported
        	| RPacketService::KCapsNotifyMSClassSupported
        	//| RPacketService::KCapsHSDPASupported
        	//| RPacketService::KCapsEGPRSSupported
        	| RPacketService::KCapsHSUPASupported;
		ret = KPacketServiceStaticCaps;
		}
	return ret;
	}

EXPORT_C MLtsyFactoryBase* LTsyFactoryL()
	{
	TInt version(0);
	MLtsyFactoryBase* factory = NULL;
	RProperty::Get( KMockLtsyFactoryProperty, EVersionProperty, version );
    switch ( version )
        {
        case EVersion3:
        	factory = CMockLtsyFactoryV3::NewL();
            break;
        case EVersion2:
            factory = CMockLtsyFactoryV2::NewL();
            break;
        case EVersion1:
        default:
            factory = CMockLtsyFactoryV1::NewL(); 
            break;
        }
        
    return factory;
	}

