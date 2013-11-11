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

#include <etelpckt.h>
#include "cmockpcktcontextmesshandler.h"
#include <pcktcs.h>
#include <ctsy/pluginapi/mmmessagemanagercallback.h>
#include "cmockltsyengine.h"
#include "tmockltsydata.h"

#include "CMmCommonStaticUtility.h"
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>

/**
Factory function
*/
CMockPcktContextMessHandler* CMockPcktContextMessHandler::NewL(CMmMessageRouter* aMessageRouter)
    {
    CMockPcktContextMessHandler* self = new (ELeave)CMockPcktContextMessHandler(aMessageRouter);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;    
    }

/**
Constructor
*/
CMockPcktContextMessHandler::CMockPcktContextMessHandler(CMmMessageRouter* aMessageRouter)
	:iMessageRouter(aMessageRouter)
    {
    }

/**
2nd phase contructor
*/
void CMockPcktContextMessHandler::ConstructL()
    {
    }

/**
Destructor
*/
CMockPcktContextMessHandler::~CMockPcktContextMessHandler()
    {
    iMbmsSessionLists.ResetAndDestroy();
    }

/**
Cleanup CArrayPtrFlat<RPacketContext::CTFTMediaAuthorizationV3>*
*/
void CMockPcktContextMessHandler::CleanupArrayPtrFlat(TAny* aSelf)
	{
	if(aSelf)
		{
		CArrayPtrFlat<RPacketContext::CTFTMediaAuthorizationV3>* array (NULL);
		array = static_cast<CArrayPtrFlat<RPacketContext::CTFTMediaAuthorizationV3>*>(aSelf);
		array->ResetAndDestroy();
		delete array;
		}	
	}

/**
Execute a request
*/
TInt CMockPcktContextMessHandler::ExtFuncL(TInt aIpc,const CMmDataPackage* aMmDataPackage)
	{
	switch (aIpc)
    	{
	   	case EPacketAddMediaAuthorization:
	   		{
	   		TInfoName* infoName = NULL;
	   		RPacketContext::CTFTMediaAuthorizationV3* mediaAuthorization = NULL;
	   		//TPacketContextConfigGPRS contextConfig;
	   		TInt configurationType;
	   		
	   		CMmPacketDataPackage* dataPacket = (CMmPacketDataPackage*) aMmDataPackage;
	   		dataPacket->UnPackData(&infoName, &mediaAuthorization);
	   		RPacketContext::TContextConfigGPRS contextConfig;
	   		dataPacket->GetConfig(&contextConfig, configurationType);	   		
	   		/*contextConfig.iPdpType              = pointerContextConfig.iPdpType;
	   		contextConfig.iAccessPointName      = pointerContextConfig.iAccessPointName;
	   		contextConfig.iPdpAddress           = pointerContextConfig.iPdpAddress;
	   		contextConfig.iPdpCompression       = pointerContextConfig.iPdpCompression;
	   		contextConfig.iAnonymousAccessReqd  = pointerContextConfig.iAnonymousAccessReqd;
	   		contextConfig.iUseEdge              = pointerContextConfig.iUseEdge;
	   		contextConfig.iProtocolConfigOption = pointerContextConfig.iProtocolConfigOption;
	   		contextConfig.iNWIContext           = pointerContextConfig.iNWIContext;*/
	   		
			TMockLtsyContextData2< TInfoName, RPacketContext::CTFTMediaAuthorizationV3* > 
				data(contextConfig, configurationType, *infoName, mediaAuthorization);
				
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
	   		}
	   		
    	case EPacketContextGetDataVolumeTransferred:
    	
    	case EPacketContextDelete:
    	case EPacketContextActivate:
    		{
     		TInfoName name;
   			aMmDataPackage->UnPackData(name);
			TMockLtsyData1< TInfoName > data(name);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    		}
    	case EPacketContextDeactivate:
	    	{
     		TInfoName name1;
      		aMmDataPackage->UnPackData(name1);  		

      		// @TODO: We should probably change this
      		if( 0 <= name1.Match(_L("*MBMS*") ) )
	      		{
	      		TInfoName name;
	      		RPacketMbmsContext::CMbmsSession* servicesArray;
	      		aMmDataPackage->UnPackData(name, servicesArray);  		
	      		TMockLtsyData2< TInfoName,  RPacketMbmsContext::CMbmsSession*> data(name, servicesArray);
	      		return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
	      		}
      		else
      			{
	      		TInfoName name;
	      		aMmDataPackage->UnPackData(name);  		
	      		TMockLtsyData1< TInfoName> data(name);
	      		return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
      			}
			}
	    		    	
	    	
    	case EPacketContextInitialiseContext:
    		{
     		TInfoName name1;
      		TInfoName name2;
	   	aMmDataPackage->UnPackData(name1, name2);
		TMockLtsyData2< TInfoName, TInfoName > data(name1, name2);
		return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    		}
    		
    	case ECtsyPacketMbmsInitialiseContextReq:
    		{
     		TInfoName name1;
   		aMmDataPackage->UnPackData(name1);

    		TMockLtsyData1< TInfoName> data(name1);
    	        return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    		}
    		    		
    	case EPacketContextModifyActiveContext:
    		{
     		CArrayFixFlat<RPacketContext::CTFTMediaAuthorizationV3>* mediaAuthorizationArray;
     		TContextConfiguration expectConfig;
   			aMmDataPackage->UnPackData(expectConfig, mediaAuthorizationArray);

     		CArrayPtrFlat<RPacketContext::CTFTMediaAuthorizationV3>* mediaAuthorizationArrayPtr(NULL);

   			if(mediaAuthorizationArray)
   				{
   				mediaAuthorizationArrayPtr = new(ELeave) CArrayPtrFlat<RPacketContext::CTFTMediaAuthorizationV3>(3);
   				CleanupStack::PushL(TCleanupItem(CleanupArrayPtrFlat, mediaAuthorizationArrayPtr));
   				for(TInt i=0;i<mediaAuthorizationArray->Count();i++)
   					{
   					RPacketContext::CTFTMediaAuthorizationV3* newEntry = RPacketContext::CTFTMediaAuthorizationV3::NewL();
   					CleanupStack::PushL(newEntry);
   					RPacketContext::CTFTMediaAuthorizationV3* entry = &mediaAuthorizationArray->At(i);
   					newEntry->iAuthorizationToken = entry->iAuthorizationToken;
   					for(TInt j=0;j<entry->iFlowIds.Count();j++)
   						{
   						RPacketContext::CTFTMediaAuthorizationV3::TFlowIdentifier fid;
   						fid.iIPFlowNumber = entry->iFlowIds[j].iIPFlowNumber;
   						fid.iMediaComponentNumber = entry->iFlowIds[j].iMediaComponentNumber;
   						newEntry->iFlowIds.Append(fid);
   						}
   					mediaAuthorizationArrayPtr->AppendL(newEntry);
   					CleanupStack::Pop(); //newentry
   					}
   				}  			

			TMockLtsyData2< TContextConfiguration, CArrayPtrFlat<RPacketContext::CTFTMediaAuthorizationV3>* >
															 data(expectConfig, mediaAuthorizationArrayPtr);
			TInt ret = iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
			if(mediaAuthorizationArrayPtr)
				CleanupStack::PopAndDestroy(); //mediaAuthorizationArrayPtr
			return ret;
    		}
    	
    	case EPacketContextSetConfig:
    		{
     		TPacketDataConfigBase base;
     		TInfoName name;
   			aMmDataPackage->UnPackData(base, name);
   			
   			if ( TPacketDataConfigBase::KConfigGPRS == base.ExtensionId() )
   				{
   				RPacketContext::TContextConfigGPRS config;
   				aMmDataPackage->UnPackData(config, name);   				
                    
   				TMockLtsyData2< RPacketContext::TContextConfigGPRS, TInfoName > data(config, name);
   				return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
   				}   
   								
   			if ( TPacketDataConfigBase::KConfigRel99Rel4 == base.ExtensionId() )					  
   				{
   				 RPacketContext::TContextConfigR99_R4 config;
   				 aMmDataPackage->UnPackData(config, name);   				 
                    
                 TMockLtsyData2< RPacketContext::TContextConfigR99_R4, TInfoName > data(config, name);
                 return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
	   			}
	   			
	   		if (TPacketDataConfigBase::KConfigRel5 == base.ExtensionId() )
	   			{
	   			RPacketContext::TContextConfig_R5 config;
	   			aMmDataPackage->UnPackData(config, name);   			
	   			
	   			TMockLtsyData2< RPacketContext::TContextConfig_R5, TInfoName > data(config, name);
	   			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
	   			}   	
	   		
   			if ( TPacketDataConfigBase::KConfigMBMS == base.ExtensionId() )
   				{
   				RPacketMbmsContext::TContextConfigMbmsV1 config;
   				aMmDataPackage->UnPackData(config, name);   				
                    
   				TMockLtsyData2< RPacketMbmsContext::TContextConfigMbmsV1, TInfoName > data(config, name);
   				return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
   				}  	   		
	   		
	   		// if unknown type	
	   		TMockLtsyData0 data;
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
 								
    		}
	    case EPacketContextUpdateMbmsSessionList:
            {
			TInfoName name;
			TMmMbmsActiveServiceListParams params;
	    	aMmDataPackage->UnPackData(name, params);
			
			TMockLtsyData2< TInfoName, TMmMbmsActiveServiceListParams > data(name, params);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);	
            }    	        		
    	case EPacketContextAddPacketFilter:
    		{
     		CArrayFixFlat<RPacketContext::TPacketFilterV2>* filterArray = NULL;
     		TInfoName* name = NULL;
     		TInt configurationType;
			//TPacketContextConfigGPRS* configGPRS(NULL);
			RPacketContext::TContextConfigGPRS configGPRS;
   			
   			aMmDataPackage->UnPackData(&name, &filterArray);
			(static_cast<const CMmPacketDataPackage*>(aMmDataPackage))->GetConfig( &configGPRS, configurationType );
						
			//if ( configGPRS )
			//	{
			TMockLtsyContextData2< TInfoName, CArrayFixFlat< RPacketContext::TPacketFilterV2 >* > 
					data(configGPRS, configurationType, *name, filterArray);
	    			
	    	return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
			//	}
				
			//return KErrCorrupt;
    		}
    		
    	case EPacketContextRemovePacketFilter:
    	default:
    		{
		    TMockLtsyData0 data;
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    		}
    	}
    }

void CMockPcktContextMessHandler::CompleteL(TInt aIpc, const TDesC8& aData, TInt aResult)
	{
	CMmDataPackage dataPackage;
	switch (aIpc)
		{
		case EPacketContextUpdateMbmsSessionList:
		    {		        		        
			TMockLtsyData1Buf< TInfoName > ret;
			ret.DeserialiseL(aData);
			dataPackage.PackData(ret.Data1Ptr());
			}
			break;		
		case EPacketContextInitialiseContext:	
			{
			TMockLtsyData1Buf<TInfoName> ret1;
			ret1.DeserialiseL(aData);
			dataPackage.PackData(ret1.Data1Ptr());
			}
			break;
		
		case ECtsyPacketMbmsInitialiseContextComp:	
			{
			TMockLtsyData2Buf<TInfoName, RPacketContext::TDataChannelV2> ret1;
			ret1.DeserialiseL(aData);
			dataPackage.PackData(ret1.Data1Ptr(), ret1.Data2Ptr());
			}
			break;
			
		case EPacketContextDeactivate:	
			{			
			if( KErrMbmsImpreciseServiceEntries == aResult )
				{
				TMockLtsyData2Buf< TInfoName, RPacketMbmsContext::CMbmsSession *> ret;
				ret.DeserialiseL(aData);
				CleanupStack::PushL(ret.Data2());
				iMbmsSessionLists.AppendL(ret.Data2());
				CleanupStack::Pop();
				dataPackage.PackData(ret.Data1Ptr(), ret.Data2() );
				}
			else
				{
				TMockLtsyData1Buf< TInfoName > ret;
				ret.DeserialiseL(aData);
				dataPackage.PackData(ret.Data1Ptr());
				}	
			}
			break;
		case EPacketContextSetConfig:
		case EPacketContextDelete:
			{			
			TMockLtsyData1Buf< TInfoName > ret;
			ret.DeserialiseL(aData);
			dataPackage.PackData(ret.Data1Ptr());
			}
			break;
		
		case EPacketContextNotifyStatusChange:
			{
			TMockLtsyData2Buf< TInfoName, TContextMisc > ret;
			ret.DeserialiseL(aData);
			dataPackage.PackData(ret.Data1Ptr(), ret.Data2Ptr());
			
			}
			break;	
		
		case EPacketContextNotifyConfigChanged:
			{	
			TMockLtsyData2Buf< TInfoName, RPacketContext::TContextConfigGPRS> ret;
   			ret.DeserialiseL(aData);
   			dataPackage.PackData(ret.Data1Ptr(), ret.Data2Ptr());					
			}
			break;
			
		case ECtsyPacketMbmsContextNotifyConfigChanged:
			{	
			TMockLtsyData2Buf< TInfoName, RPacketMbmsContext::TContextConfigMbmsV1> ret;
   			ret.DeserialiseL(aData);
   			dataPackage.PackData(ret.Data1Ptr(), ret.Data2Ptr());					
			}
			break;

		case EPacketContextActivate:
			{
			TMockLtsyData2Buf< TInfoName, RPacketContext::TContextConfigGPRS> ret;
			ret.DeserialiseL(aData);
			dataPackage.PackData(ret.Data1Ptr(), ret.Data2Ptr());			
			}
			break;
		case EPacketGetMbmsSessionListPhase1:
		case EPacketContextNotifyConnectionSpeedChange:
			{
			TMockLtsyData2Buf< TInfoName, TInt> ret;
			ret.DeserialiseL(aData);
			dataPackage.PackData(ret.Data1Ptr(), ret.Data2Ptr());
			}
			break;
		
		case EPacketContextModifyActiveContext:
			{			
			TMockLtsyData2Buf< TInfoName, TBuf8 <1> > ret;
			ret.DeserialiseL(aData);
			dataPackage.PackData(ret.Data1Ptr(), ret.Data2Ptr());
			}
			break;	
		
		case EPacketContextGetDataVolumeTransferred:
		case EPacketContextNotifyDataTransferred:
			{
			TMockLtsyData2Buf< TInfoName, RPacketContext::TDataVolume> ret;
			ret.DeserialiseL(aData);
			dataPackage.PackData(ret.Data1Ptr(), ret.Data2Ptr());			
			}
			break;
		case EPacketContextNotifyConnectionInfoChange:
			{
			TMockLtsyData2Buf<TInfoName, RPacketContext::TConnectionInfoV1> ret;
			ret.DeserialiseL(aData);
			dataPackage.PackData(ret.Data1Ptr(), ret.Data2Ptr());			
			}
			break;						
		case EPacketContextAddPacketFilter:
		case EPacketContextRemovePacketFilter:
		default:
			// complete event without data
			break;		
		}

	iMessageRouter->MessageManagerCallback()->Complete(aIpc, &dataPackage, aResult);
	}

