// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmockpcktqosmesshandlerTraces.h"
#endif

#include "cmockpcktqosmesshandler.h"
#include <pcktcs.h>
#include <ctsy/pluginapi/mmmessagemanagercallback.h>
#include "cmockltsyengine.h"
#include "tmockltsydata.h"


/**
Factory function
*/
CMockPcktQoSMessHandler* CMockPcktQoSMessHandler::NewL(CMmMessageRouter* aMessageRouter)
    {
    CMockPcktQoSMessHandler* self = new(ELeave)CMockPcktQoSMessHandler(aMessageRouter);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

/**
Constructor
*/
CMockPcktQoSMessHandler::CMockPcktQoSMessHandler(CMmMessageRouter* aMessageRouter)
	:iMessageRouter(aMessageRouter)
    {
    }

/**
2nd phase contructor
*/
void CMockPcktQoSMessHandler::ConstructL()
    {
    }

/**
Destructor
*/
CMockPcktQoSMessHandler::~CMockPcktQoSMessHandler()
    {
    }

/**
Execute a request
*/
TInt CMockPcktQoSMessHandler::ExtFuncL(TInt aIpc, const CMmDataPackage* aMmDataPackage)
    {
     switch (aIpc)
    	{
    	case EPacketQoSSetProfileParams:
    		{
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMOCKPCKTQOSMESSHANDLER_EXTFUNCL_1, "Mock: CMockPcktQoSMessHandler. aIpc: %d  aMmDataPackage: 0x%08x", aIpc, (TUint)aMmDataPackage );
    		TDes8* profile = NULL;
    		TContextParams contextParams; 
    		
    		aMmDataPackage->UnPackData(contextParams, profile);

   			TPckg<TPacketDataConfigBase>* profilePckg = ( TPckg<TPacketDataConfigBase>* ) profile;
 		    TPacketDataConfigBase& base = ( *profilePckg )();
 		    
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMOCKPCKTQOSMESSHANDLER_EXTFUNCL_2, "Mock: CMockPcktQoSMessHandler. base.ExtensionId(): %d", base.ExtensionId() );

 		    switch(base.ExtensionId())
 		    	{
 		    	case TPacketDataConfigBase::KConfigGPRS:
 		    		{
	 		    	TPckg< RPacketQoS::TQoSGPRSRequested >* pckg = 
 		    	           ( TPckg< RPacketQoS::TQoSGPRSRequested >* ) profile;
 			    	RPacketQoS::TQoSGPRSRequested& packetGPRS = ( *pckg )();

	    			TMockLtsyData2< TContextParams, RPacketQoS::TQoSGPRSRequested > 
	    		                        data(contextParams, packetGPRS);

					return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
 		    		}
 		    	case TPacketDataConfigBase::KConfigRel99Rel4:
 		    		{
	 		    	TPckg< RPacketQoS::TQoSR99_R4Requested >* pckg = 
	 		    	           ( TPckg< RPacketQoS::TQoSR99_R4Requested >* ) profile;
	 		    	RPacketQoS::TQoSR99_R4Requested& packetR99_R4 = ( *pckg )();

		    		TMockLtsyData2< TContextParams, RPacketQoS::TQoSR99_R4Requested > 
		    		                        data(contextParams, packetR99_R4);

					return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
 		    		}
 		    	case TPacketDataConfigBase::KConfigRel5:
					{
	 		    	TPckg< RPacketQoS::TQoSR5Requested >* pckg = 
	 		    	           ( TPckg< RPacketQoS::TQoSR5Requested >* ) profile;
	 		    	RPacketQoS::TQoSR5Requested& packetR5 = ( *pckg )();

		    		TMockLtsyData2< TContextParams, RPacketQoS::TQoSR5Requested > 
		    		                        data(contextParams, packetR5);

					return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
					} 		    	
 		    	default:
					{
				    TMockLtsyData0 data;
 				
 					return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
					} 		    	
 		    	}
    		}
    	default:
    		{
		    TMockLtsyData0 data;
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    		}
    	}
    }

/**
Complete a request
*/
void CMockPcktQoSMessHandler::CompleteL(TInt aIpc, const TDesC8& aData, TInt aResult)
	{
	CMmPacketDataPackage dataPackage;	
	switch (aIpc)
		{
		case EPacketQoSNotifyProfileChanged:
			{
			TMockLtsyPacketQoSData1Buf< TInfoName > ret;
			ret.DeserialiseL(aData);
			
			dataPackage.SetProfileChanged(&ret.QoSGPRSNegotiated(), 
			                              &ret.QoSR99_R4Negotiated(),
			                              &ret.QoSR5Negotiated());

			dataPackage.PackData(ret.Data1Ptr());
			}
			break;
			case EPacketQoSSetProfileParams:
			{			
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMOCKPCKTQOSMESSHANDLER_COMPLETEL_1, "Mock: Complete. aIpc = %d, aData = %s", aIpc, aData);
				TMockLtsyData1Buf< TInfoName > ret;
				ret.DeserialiseL(aData);
				dataPackage.PackData(ret.Data1Ptr());
			}
			break;
			
			default:
			// complete event without data
			break;
		}
		
	iMessageRouter->MessageManagerCallback()->Complete(aIpc, &dataPackage, aResult);
	
	}

