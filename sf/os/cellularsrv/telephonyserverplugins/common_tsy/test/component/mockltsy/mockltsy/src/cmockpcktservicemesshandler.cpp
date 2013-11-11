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

//#include <in_sock.h>
#include "cmockpcktservicemesshandler.h"
#include "mmockmesshandlerbase.h"
#include <ctsy/pluginapi/mmmessagemanagercallback.h>
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <pcktcs.h>
#include "cmockltsyengine.h"
#include "tmockltsydata.h"

/**
Factory function
*/
CMockPcktServiceMessHandler* CMockPcktServiceMessHandler::NewL(CMmMessageRouter* aMessageRouter)
    {
    CMockPcktServiceMessHandler* self = new(ELeave)CMockPcktServiceMessHandler(aMessageRouter);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

/**
Constructor
*/
CMockPcktServiceMessHandler::CMockPcktServiceMessHandler(CMmMessageRouter* aMessageManager)
	: iMessageRouter(aMessageManager)
    {
    }

/**
2nd phase contructor
*/
void CMockPcktServiceMessHandler::ConstructL()
    {      
    }

/**
Destructor
*/
CMockPcktServiceMessHandler::~CMockPcktServiceMessHandler()
    {
    iPcktMbmsMonitoredServiceLists.ResetAndDestroy();
    }

/**
Execute a request
*/
TInt CMockPcktServiceMessHandler::ExtFuncL(TInt aIpc, const CMmDataPackage* aDataPackage)     
    {
     switch (aIpc)
    	{    	    	
    	case EPacketGetAttachMode:
    		{
    		RPacketService::TPreferredBearer prefBearer;
			aDataPackage->UnPackData(prefBearer);
			TMockLtsyData1<RPacketService::TPreferredBearer> data(prefBearer);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data );    		
    		}    		
    		
    	case EPacketSetPrefBearer:
    		{
     		TBool initState;
   			aDataPackage->UnPackData(initState);
			TMockLtsyData1<TBool> data(initState);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    		}
    	
    	case EPacketEnumerateMbmsActiveServiceList:
    		{
     		TMockLtsyData0 data;
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    		}    	
    	
    	case EPacketSetDefaultContextParams:
    		{
    		
    		TPacketDataConfigBase base;    		
    		aDataPackage->UnPackData(base);
    		
    		switch ( base.ExtensionId() )
    			{
    			case TPacketDataConfigBase::KConfigGPRS:
    				{
    				RPacketContext::TContextConfigGPRS configGPRS;    	
   					aDataPackage->UnPackData(configGPRS);
					TMockLtsyData1<RPacketContext::TContextConfigGPRS> data(configGPRS);
					return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    				}
    			
    			case TPacketDataConfigBase::KConfigRel99Rel4:
    				{
    				RPacketContext::TContextConfigR99_R4 configRel99;    	
   					aDataPackage->UnPackData(configRel99);
					TMockLtsyData1<RPacketContext::TContextConfigR99_R4> data(configRel99);
					return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);    				
    				}  			
    			
    			case TPacketDataConfigBase::KConfigCDMA:
    				{
    				RPacketContext::TContextConfigCDMA configCDMA;    	
   					aDataPackage->UnPackData(configCDMA);
					TMockLtsyData1<RPacketContext::TContextConfigCDMA> data(configCDMA);
					return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);    				
    				}   
    				
    			case TPacketDataConfigBase::KConfigRel5:
    				{
    				RPacketContext::TContextConfig_R5 configRel5;    	
   					aDataPackage->UnPackData(configRel5);
					TMockLtsyData1<RPacketContext::TContextConfig_R5> data(configRel5);
					return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);    				
    				}
    				
    			default:
    				{
		    		TMockLtsyData0 data;
 					return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    				}
    			}     		
    		}
    	
    		
    	case EPacketSetAttachMode:
    		{
     		RPacketService::TAttachMode attachMode;
   			aDataPackage->UnPackData(attachMode);
			TMockLtsyData1<RPacketService::TAttachMode> data(attachMode);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    		}
    		
    	case EPacketUpdateMbmsMonitorServiceList:
    		{
         	TMmMbmsMonitorServiceListParams params;
         	aDataPackage->UnPackData(params);
			TMockLtsyData1<TMmMbmsMonitorServiceListParams> data(params);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);
    		}
    	case EPacketGetMbmsNetworkServiceStatus:
    		{
         	TBool aAttemptAttach;
    		aDataPackage->UnPackData(aAttemptAttach);
    		TMockLtsyData1<TBool> data(aAttemptAttach);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    		}
        case EPacketGetStaticCaps:
        {
        	return 0;
        }
        case EPacketEnumerateMbmsMonitorServiceList:
        {
        	TMockLtsyData0 data;
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
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
void CMockPcktServiceMessHandler::CompleteL(TInt aIpc, const TDesC8& aData, TInt aResult)
	{
	CMmDataPackage dataPackage;
	switch (aIpc)
		{
        case EPacketEnumerateMbmsActiveServiceList:
            {
			TMockLtsyData1Buf<TInt> ret;
			ret.DeserialiseL(aData);
			dataPackage.PackData(ret.Data1Ptr());            
            }
            break;
		case EPacketNotifyStatusChange:
			{			
		//	TBool resumed;
		//	RPacketService::TStatus status;
			TMockLtsyData2Buf<RPacketService::TStatus , TBool> ret;
			ret.DeserialiseL(aData);
			dataPackage.PackData(ret.Data1Ptr(), ret.Data2Ptr());
			}
			break;
		case EPacketGetAttachMode:
			{			
			//RPacketService::TAttachMode mode;
			TMockLtsyData1Buf<RPacketService::TAttachMode> ret;
			ret.DeserialiseL(aData);
			dataPackage.PackData(ret.Data1Ptr());
			}
			break;
		case EPacketNotifyChangeOfNtwkRegStatus:
			{			
			TMockLtsyData1Buf<RPacketService::TRegistrationStatus> reg;
			reg.DeserialiseL(aData);
			dataPackage.PackData(reg.Data1Ptr());
			}
			break;			
		case EPacketGetNtwkRegStatus:
			{			
			TMockLtsyData1Buf<RPacketService::TRegistrationStatus> reg;
			reg.DeserialiseL(aData);
			dataPackage.PackData(reg.Data1Ptr());
			}
			break;			
		case EPacketNotifyContextAdded:
			{
			TMockLtsyData2Buf<TInfoName, TInt > ret;
			ret.DeserialiseL(aData);
			dataPackage.PackData(ret.Data1Ptr(), ret.Data2Ptr());			
			}
			break;		
			
		case EPacketNotifyMSClassChange:
			{
			TMockLtsyData1Buf< RPacketService::TMSClass> ret;
			ret.DeserialiseL(aData);
			dataPackage.PackData(ret.Data1Ptr());			
			}
			break;
			
		case EPacketNotifyDynamicCapsChange:
			{
			TMockLtsyData2Buf< TBool, TBool> ret;
			ret.DeserialiseL(aData);
			dataPackage.PackData(ret.Data1Ptr(), ret.Data2Ptr());		
			}
			break;				
				
		case EPacketNotifyContextActivationRequested:
			{ 
            TMockLtsyData1Buf<RPacketContext::TContextConfigGPRS> gprsBuf;
            gprsBuf.DeserialiseL(aData);   
            RPacketContext::TContextConfigGPRS* configGPRS = gprsBuf.Data1Ptr();     
            TInt extensionId = configGPRS->ExtensionId();      
           
            switch ( extensionId )
                {
                case TPacketDataConfigBase::KConfigGPRS:
                    {
                    dataPackage.PackData(configGPRS);
                    break;                 
                    }
                case TPacketDataConfigBase::KConfigCDMA:
                    {
                    TMockLtsyData1Buf<RPacketContext::TContextConfigCDMA> cdmaBuf;
                    cdmaBuf.DeserialiseL(aData);                                   
                    dataPackage.PackData(cdmaBuf.Data1Ptr()); 
                    break;                                                 
                    }
                case TPacketDataConfigBase::KConfigRel99Rel4:
                    {                    
                    TMockLtsyData1Buf<RPacketContext::TContextConfigR99_R4> r99Buf;
                    r99Buf.DeserialiseL(aData);                                    
                    dataPackage.PackData(r99Buf.Data1Ptr());
                    break; 
                    }                    
                case TPacketDataConfigBase::KConfigRel5:
                    {                     
                    TMockLtsyData1Buf<RPacketContext::TContextConfig_R5> r5Buf;
                    r5Buf.DeserialiseL(aData);                                     
                    dataPackage.PackData(r5Buf.Data1Ptr());
                    break;           
                    }                          
                case TPacketDataConfigBase::KConfigMBMS:   
                    {                         
                    TMockLtsyData1Buf<RPacketMbmsContext::TContextConfigMbmsV1> mbmsBuf;
                    mbmsBuf.DeserialiseL(aData);                                   
                    dataPackage.PackData(mbmsBuf.Data1Ptr());
                    break;        
                    }         
                default:
                    //Do nothing
                    break;
                }
			}
			break;
			
		case EPacketNotifyAttachModeChange:
			{			
			TMockLtsyData1Buf<RPacketService::TPreferredBearer> ret;
			ret.DeserialiseL(aData);
			dataPackage.PackData(ret.Data1Ptr());
			}
			break;
			
		case EPacketNotifyTransferCapsIPC: 
			{			
			TMockLtsyData1Buf<TDynamicTransferCapsFlags> ret;
			ret.DeserialiseL(aData);
			dataPackage.PackData(ret.Data1Ptr());
			}
			break;
			
		case EMmTsyNotifyEGprsInfoChangeIPC:
			{			
			TMockLtsyData1Buf<TBool> ret;
			ret.DeserialiseL(aData);
			dataPackage.PackData(ret.Data1Ptr());
			}
			break;
			
		case EPacketNotifyReleaseModeChange:
			{				
			TMockLtsyData1Buf<RMobilePhone::TMobilePhoneNetworkMode> ret;
			ret.DeserialiseL(aData);
			dataPackage.PackData(ret.Data1Ptr());
			}
			break;
			
		case EPacketGetMbmsNetworkServiceStatus:
			{			
			TMockLtsyData1Buf<TMbmsNetworkServiceStatus> reg;
			reg.DeserialiseL(aData);
			dataPackage.PackData(reg.Data1Ptr());
			}
			break;
			
		case EPacketNotifyMbmsNetworkServiceStatusChange:
			{			
			TMockLtsyData1Buf<TMbmsNetworkServiceStatus> reg;
			reg.DeserialiseL(aData);
			dataPackage.PackData(reg.Data1Ptr());
			}
			break;
		case EPacketGetStaticCaps:
			{			
			TMockLtsyData1Buf<TBool> reg;
			reg.DeserialiseL(aData);
			dataPackage.PackData(reg.Data1Ptr());
			}
			break;					
		case EPacketEnumerateMbmsMonitorServiceList:
			{
			TMockLtsyData1Buf<TInt> reg;
			reg.DeserialiseL(aData);
			dataPackage.PackData(reg.Data1Ptr());
			}
			break;
		case EPacketUpdateMbmsMonitorServiceList:
		case EPacketNotifyMbmsServiceAvailabilityChange:
			{
			TMockLtsyData1Buf< CPcktMbmsMonitoredServiceList *> completeData;
			completeData.DeserialiseL(aData);
			if (completeData.Data1())
				{
				completeData.PushL();
				iPcktMbmsMonitoredServiceLists.AppendL(completeData.Data1());
				CleanupStack::Pop(&completeData);
				}
			dataPackage.PackData(completeData.Data1());
			}
			break;							
		default:
			// complete event without data
			break;
		}
	iMessageRouter->MessageManagerCallback()->Complete(aIpc, &dataPackage, aResult);
	}


