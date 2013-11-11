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

//  INCLUDE FILES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CmmcustommesshandlergsmextTraces.h"
#endif

#include "CMmCustomMessHandlerGsmExt.h"
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <ctsy/pluginapi/mmmessagemanagercallback.h>
#include "cmmcustomstubext.h"
#include "cmockltsyengine.h"

// ======== MEMBER FUNCTIONS ========

CMmCustomMessHandlerGsmExt::CMmCustomMessHandlerGsmExt()
    {
    }

void CMmCustomMessHandlerGsmExt::ConstructL()
    {
    }

CMmCustomMessHandlerGsmExt* CMmCustomMessHandlerGsmExt::NewL( 
    CMmMessageRouter* aMessageRouter ) 
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMMESSHANDLERGSMEXT_NEWL_1, "TSY: CMmCustomMessHandlerGsmExt::NewL.\n" );
    
    CMmCustomMessHandlerGsmExt* self = new( ELeave ) 
        CMmCustomMessHandlerGsmExt();

    CleanupStack::PushL( self );
    self->iMessageRouter = aMessageRouter;
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

CMmCustomMessHandlerGsmExt::~CMmCustomMessHandlerGsmExt()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMMESSHANDLERGSMEXT_DTOR_1, "TSY: CMmCustomMessHandlerGsmExt::~CMmCustomMessHandlerGsmExt.\n");
    }

// ---------------------------------------------------------------------------
// CMmCustomMessHandlerGsmExt::ExtFuncL
// Handles stub requests
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TInt CMmCustomMessHandlerGsmExt::ExtFuncL( 
    TInt aIpc,
    const CMmDataPackage* aDataPackage )
    {
    TInt ret( KErrNotSupported );
    TBool complete ( EFalse );

    switch ( aIpc )
        {
       
		case ECustomCheckAlsPpSupportIPC:
            {
            TMockLtsyData0 data;
      		return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);
  			}
          
        case ECustomTerminateCallIPC:
			{
			TInt callId(0);
			TBool isError(EFalse);
			aDataPackage->UnPackData(callId, isError);
			TMockLtsyData2< TInt, TBool > data(callId, isError);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);
			}

        case ECustomSimLockActivateIPC:    
        case ECustomSimLockDeActivateIPC:
        	{
        	RMmCustomAPI::TSimLockPassword password;
    		RMmCustomAPI::TLockNumber lockNumber;    		
   			aDataPackage->UnPackData(password, lockNumber);
			TMockLtsyData2< RMmCustomAPI::TSimLockPassword, RMmCustomAPI::TLockNumber > 
					 data(password, lockNumber);
					 
			return 	iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);
        	}
        
        case ECustomSetAlsBlockedIPC:
        	{
        	RMmCustomAPI::TSetAlsBlock blockStatus;    	 		
   			aDataPackage->UnPackData(blockStatus);
			TMockLtsyData1< RMmCustomAPI::TSetAlsBlock >	 data(blockStatus);
					 
			return 	iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);
        	}
            
        case ECustomSetAlwaysOnMode:
        	{			
			RMmCustomAPI::TSetAlwaysOnMode mode;
   			aDataPackage->UnPackData(mode);
			TMockLtsyData1< RMmCustomAPI::TSetAlwaysOnMode > data(mode);
			return 	iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);
        	}
        	        	
        case ECustomSetDriveModeIPC:  
        	{
        	 RMmCustomAPI::TSetDriveMode modeStatus;
        	 aDataPackage->UnPackData(modeStatus);
        	 TMockLtsyData1< RMmCustomAPI::TSetDriveMode > data(modeStatus);
			 return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);        	 
        	}      
        
        case ECustomWriteHSxPAStatusIPC:
        	{
        	RMmCustomAPI::THSxPAStatus status;
        	aDataPackage->UnPackData(status);
        	TMockLtsyData1< RMmCustomAPI::THSxPAStatus > data(status);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);        	 
        	}
        	
        case EWriteViagHomeZoneUHZIUESettingsIPC:
			{
        	RMmCustomAPI::TViagUHZIUESettings settings;
        	aDataPackage->UnPackData(settings);
        	TMockLtsyData1< RMmCustomAPI::TViagUHZIUESettings > data(settings);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);        	 
			}
			
        case ECustomSetSimMessageStatusReadIPC:
        	{
        	TTime time;
        	TInt timeZone;
        	aDataPackage->UnPackData(time, timeZone);
        	TMockLtsyData2< TTime, TInt > data(time, timeZone);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);        	 
        	}
        	
        case ECustomGetSimAuthenticationDataIPC:
        	{
        	RMmCustomAPI::TSimAuthenticationBase simBase;
			TInt rfState;
			aDataPackage->UnPackData(simBase, rfState);
			        	
			if (simBase.ExtensionId() == RMmCustomAPI::TSimAuthenticationBase::EEapSim)
				{
				RMmCustomAPI::TSimAuthenticationEapSim tmp;
				aDataPackage->UnPackData(tmp);
				RMmCustomAPI::TSimAuthenticationEapSim* tmpPtr = &tmp;
				TMockLtsyData1< RMmCustomAPI::TSimAuthenticationEapSim* > data(tmpPtr);
				return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
				}
			else if (simBase.ExtensionId() == RMmCustomAPI::TSimAuthenticationBase::EEapAka)
				{
				RMmCustomAPI::TSimAuthenticationEapAka tmp;
				aDataPackage->UnPackData(tmp);
				RMmCustomAPI::TSimAuthenticationEapAka* tmpPtr = &tmp;
				TMockLtsyData1< RMmCustomAPI::TSimAuthenticationEapAka* > data(tmpPtr);
				return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
				}
			else if (simBase.ExtensionId() == RMmCustomAPI::TSimAuthenticationBase::EGbaBootstrap)
				{
				RMmCustomAPI::TSimAuthenticationGbaBootstrap tmp;
				aDataPackage->UnPackData(tmp);
				RMmCustomAPI::TSimAuthenticationGbaBootstrap* tmpPtr = &tmp;
				TMockLtsyData1< RMmCustomAPI::TSimAuthenticationGbaBootstrap* > data(tmpPtr);
				return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
				}
			else if (simBase.ExtensionId() == RMmCustomAPI::TSimAuthenticationBase::EGbaBootstrapUpdate)
				{
				RMmCustomAPI::TSimAuthenticationGbaBootstrapUpdate tmp;
				aDataPackage->UnPackData(tmp);
				RMmCustomAPI::TSimAuthenticationGbaBootstrapUpdate* tmpPtr = &tmp;
				TMockLtsyData1< RMmCustomAPI::TSimAuthenticationGbaBootstrapUpdate* > data(tmpPtr);
				return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
				}
			else if (simBase.ExtensionId() == RMmCustomAPI::TSimAuthenticationBase::EGbaBootstrapNafDerivation)
				{
				RMmCustomAPI::TSimAuthenticationGbaNafDerivation tmp;
				aDataPackage->UnPackData(tmp);
				RMmCustomAPI::TSimAuthenticationGbaNafDerivation* tmpPtr = &tmp;
				TMockLtsyData1< RMmCustomAPI::TSimAuthenticationGbaNafDerivation* > data(tmpPtr);
				return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
				}
			else if (simBase.ExtensionId() == RMmCustomAPI::TSimAuthenticationBase::EMgvMskUpdate)
				{
				RMmCustomAPI::TSimAuthenticationMgvMskUpdate tmp;
				aDataPackage->UnPackData(tmp);
				RMmCustomAPI::TSimAuthenticationMgvMskUpdate* tmpPtr = &tmp;
				TMockLtsyData1< RMmCustomAPI::TSimAuthenticationMgvMskUpdate* > data(tmpPtr);
				return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
				}
			else if (simBase.ExtensionId() == RMmCustomAPI::TSimAuthenticationBase::EMgvMtkGeneration)
				{
				RMmCustomAPI::TSimAuthenticationMgvMtkGeneration tmp;
				aDataPackage->UnPackData(tmp);
				RMmCustomAPI::TSimAuthenticationMgvMtkGeneration* tmpPtr = &tmp;
				TMockLtsyData1< RMmCustomAPI::TSimAuthenticationMgvMtkGeneration* > data(tmpPtr);
				return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
				}
			else if (simBase.ExtensionId() == RMmCustomAPI::TSimAuthenticationBase::EMgvMskDeletion)
				{
				RMmCustomAPI::TSimAuthenticationMgvMskDeletion tmp;
				aDataPackage->UnPackData(tmp);
				RMmCustomAPI::TSimAuthenticationMgvMskDeletion* tmpPtr = &tmp;
				TMockLtsyData1< RMmCustomAPI::TSimAuthenticationMgvMskDeletion* > data(tmpPtr);
				return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
				}
			else        // default
			    {
			    return KErrCorrupt;       	 
			    }       	
        	}
        	
        case ECustomGetSimCardReaderStatusIPC:
        	{
        	RMmCustomAPI::TSimCardReaderStatus* statusPtr;
        	aDataPackage->UnPackData(statusPtr);
        	TMockLtsyData1< RMmCustomAPI::TSimCardReaderStatus* > data(statusPtr);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);        	 
        	}
        	
        case ECustomGetATRIPC:
        	{        	
        	TDes8* atr;
        	aDataPackage->UnPackData(atr);
        	TDesC8* ptr = static_cast<TDesC8*>(atr);
        	TMockLtsyData1< TDesC8* > data(ptr);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);        	 
        	}
        	
        case ECustomSetSystemNetworkModeIPC:
        	{
        	RMmCustomAPI::TNetworkModeCaps caps;
        	aDataPackage->UnPackData(caps);
        	TMockLtsyData1< RMmCustomAPI::TNetworkModeCaps > data(caps);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);        	 
        	}

        case ECustomGetSystemNetworkModesIPC:
        	{
        	TBool useCurrentRats;
        	aDataPackage->UnPackData(useCurrentRats);
        	TMockLtsyData1< TBool > data(useCurrentRats);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);        	 
        	}

        case ECustomReadSimFileIPC:
        	{
        	RMmCustomAPI::TSimFileInfo info;
        	aDataPackage->UnPackData(info);
        	TMockLtsyData1< RMmCustomAPI::TSimFileInfo > data(info);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);        	 
        	}
        	
        case ECustomDisablePhoneLockIPC:
        	{
        	RMobilePhone::TMobilePassword pswd;
        	aDataPackage->UnPackData(pswd);
        	TMockLtsyData1< RMobilePhone::TMobilePassword > data(pswd);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);        	 
        	}
        	
        case ECustomSendAPDUReqIPC:
        	{
        	RMmCustomAPI::TApdu apdu;
        	aDataPackage->UnPackData(apdu);
        	TMockLtsyData1< RMmCustomAPI::TApdu > data(apdu);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);        	 
        	}
        	
        case ECustomSendAPDUReqV2IPC:
            {
            RMmCustomAPI::TApduParameters *apdu = NULL;
            aDataPackage->UnPackData(apdu);
            TMockLtsyData1< RMmCustomAPI::TApduParameters > data(*apdu);
            return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);            
            }
        	
        case ECustomDeleteSimCbTopicIPC:
        	{
        	TUint index;
        	TBool deleteFlag;
        	aDataPackage->UnPackData(index, deleteFlag);
        	TMockLtsyData2< TUint, TBool > data(index, deleteFlag);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);        	 
        	}
        	
        case ECustomGetProgrammableOperatorLogoIPC:
        	{
        	RMmCustomAPI::TOperatorId oper;
        	aDataPackage->UnPackData(oper);
        	TMockLtsyData1< RMmCustomAPI::TOperatorId > data(oper);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);        	 
        	}

        case ECustomCheckEmergencyNumberIPC:
			{
        	RMmCustomAPI::TEmerNumberCheckMode mode;
        	aDataPackage->UnPackData(mode);
        	TMockLtsyData1< RMmCustomAPI::TEmerNumberCheckMode > data(mode);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);        	 
			}
			
        case ECustomCheckSecurityCodeIPC:
        	{
        	TUint8 code;
        	RMobilePhone::TMobilePassword pswd;
        	aDataPackage->UnPackData(code, pswd);
        	TMockLtsyData2< TUint8, RMobilePhone::TMobilePassword > data(code, pswd);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);        	 
        	}
        	
        case ECustomCheckSecurityCodeCancelIPC:
        	{
        	TUint8 code;
        	TBool flag;;
        	aDataPackage->UnPackData(code, flag);
        	TMockLtsyData2< TUint8, TBool > data(code, flag);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);        	 
        	}
        	
        case ECustomIsBlockedIPC:
        	{
        	TUint8 code;
        	aDataPackage->UnPackData(code);
        	TMockLtsyData1< TUint8 > data(code);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);        	 
        	}
		
        case EReadViagHomeZoneCacheIPC:
        	{
        	RMmCustomAPI::TViagCacheRecordId currentlyRetreivedCache;
        	aDataPackage->UnPackData(currentlyRetreivedCache);
        	TMockLtsyData1<RMmCustomAPI::TViagCacheRecordId> data(currentlyRetreivedCache);
        	return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);        	
        	}

        case EWriteViagHomeZoneCacheIPC:
        	{
        	RMmCustomAPI::TViagCacheRecordId recId;
        	RMmCustomAPI::TViagCacheRecordContent recContent;
        	aDataPackage->UnPackData(recId,recContent);
        	TMockLtsyData2<RMmCustomAPI::TViagCacheRecordId,
        				RMmCustomAPI::TViagCacheRecordContent> data(recId,recContent);
        	return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);
        	}
        	
        case ECustomSetBandSelectionIPC:
        	{
        	RMmCustomAPI::TBandSelection band;
        	RMmCustomAPI::TNetworkModeCaps mode;
        	aDataPackage->UnPackData(band,mode);
        	TMockLtsyData2<RMmCustomAPI::TBandSelection,
        				RMmCustomAPI::TNetworkModeCaps> data(band,mode);
        	return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);
        	}
        	
        case ECustomNetWakeupIPC:
        case ECustomGetCipheringInfoIPC:
        case ECustomGetAlsBlockedIPC:
        case EReadViagHomeZoneParamsIPC:
        case ECustomGetCellInfoIPC:
        case ECustomNotifyIccCallForwardingStatusChangeIPC:
        case ECustomGetIccCallForwardingStatusIPC:
        case ECustomNotifyHSxPAStatusIPC:
        case ECustomReadHSxPAStatusIPC:
        case ECustomNotifyRauEventIPC:
        case ECustomSimWarmResetIPC:
        case ECustomPowerSimOffIPC:
        case ECustomPowerSimOnIPC:
        case ECustomGetCurrentSystemNetworkModesIPC:
        case ECustomGetLifeTimeIPC: 
        case ECustomNotifyNetworkConnectionFailureIPC:
        case ECustomStartSimCbTopicBrowsingIPC:
        case ECustomResetNetServerIPC:
        case ECustomCheckTwoDigitDialSupportIPC:
        case ECustomGetOperatorNameIPC:
        case ECustomGetNetworkProviderNameIPC:
        case EMmTsySecurityGetSimActivePinStateIPC:
        case ECustomNotifySsRequestCompleteIPC:
        case ECustomClearCallBlackListIPC:
        case ECustomGetBandSelectionIPC:
        	{
        	TMockLtsyData0 data;
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
        	}	
        case ECustomGetServiceTableSupportbyApplicationIPC:
        	{
        	RMmCustomAPI::TAppSupport usimSupport;
        	aDataPackage->UnPackData(usimSupport);
        	TMockLtsyData1<TInt> data(usimSupport.iAppNum);
        	return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);
        	}
        default:
            ret = KErrNotSupported;
            break;    
        }

    if( complete )
        {
        iMessageRouter->MessageManagerCallback()->Complete( aIpc, ret );

        return KErrNone; 
        }

    return ret;
    }

void CMmCustomMessHandlerGsmExt::CompleteL(TInt aIpc, const TDesC8& aData, TInt aResult)
	{
	 	CMmDataPackage dataPackage;
	 	
		switch (aIpc)
			{
			case ECustomSetAlwaysOnMode:
				{				
				TMockLtsyData1Buf< TInt > ret;
				ret.DeserialiseL(aData);
				dataPackage.PackData(ret.Data1Ptr());
				}
				break;
				
			case ECustomNotifyDtmfEventIPC:
				{
				TMockLtsyData1Buf <RMmCustomAPI::TDtmfInfo> ret;
				ret.DeserialiseL(aData);
				dataPackage.PackData(ret.Data1Ptr());				
				}
				break;
				
			case ECustomGetDiagnosticOctetsIPC:
				{
				TMockLtsyData2Buf <TInt, TInt> ret;
				ret.DeserialiseL(aData);
				dataPackage.PackData(ret.Data1Ptr(), ret.Data2Ptr());
				}
				break;
				
			case ECustomGetAlsBlockedIPC:
				{			
				TMockLtsyData1Buf <RMmCustomAPI::TGetAlsBlockStatus> ret;
				ret.DeserialiseL(aData);
				dataPackage.PackData(ret.Data1Ptr());
				}
				break;
				
			case ECustomCheckAlsPpSupportIPC:
				{			
                TMockLtsyData1Buf<RMmCustomAPI::TAlsSupport> alsSupport;
                alsSupport.DeserialiseL(aData);
                dataPackage.PackData(alsSupport.Data1Ptr());
				}
				break;
			case ECustomNotifyNSPSStatusIPC:
			case ECustomGetCipheringInfoIPC:
				{
				TBool indReq;
				TPckg<TBool> pckg(indReq);
				pckg.Copy(aData);
				dataPackage.PackData(&indReq);
				}
				break;
				
			case ECustomGetRemoteAlertingToneStatusIPC:
				{
				TMockLtsyData1Buf <TBool> ret;
				ret.DeserialiseL(aData);
				dataPackage.PackData(ret.Data1Ptr());
				}
				break;

			case ECustomNotifyCipheringInfoChangeIPC:
				{
				RMmCustomAPI::TCipheringInfo strInfo;
				TPckg<RMmCustomAPI::TCipheringInfo> pckg(strInfo);
				pckg.Copy(aData);
				dataPackage.PackData(&strInfo.iIndStatus, &strInfo.iCiphStatus);
				}
				break;
				
			case ECustomGetCellInfoIPC:
			case ECustomNotifyCellInfoChangeIPC:
				{
				RMmCustomAPI::TMmCellInfo info;
				TPckg<RMmCustomAPI::TMmCellInfo> pckg(info);
				
				pckg.Copy(aData);
				
				RMmCustomAPI::TMmCellInfo* cellPtr = &info;
				
				dataPackage.PackData(cellPtr);
				}
				break;
				
			case ECustomNotifyIccCallForwardingStatusChangeIPC:
			case ECustomGetIccCallForwardingStatusIPC:
				{
				RMmCustomAPI::TCFIndicators info;
				TPckg<RMmCustomAPI::TCFIndicators> pckg(info);
			
				pckg.Copy(aData);
				
				RMmCustomAPI::TCFIndicators* indiPtr = &info;
				
				dataPackage.PackData(indiPtr);
				}
				break;
			case EReadViagHomeZoneParamsIPC:
				{
				TMockLtsyData2Buf <RMmCustomAPI::TViagParams, RMmCustomAPI::TViagElements*> ret;
				CleanupClosePushL(ret);
				ret.DeserialiseL(aData);
				dataPackage.PackData(ret.Data1Ptr(), ret.Data2Ptr());				
				iMessageRouter->MessageManagerCallback()->Complete(aIpc, &dataPackage, aResult);
				CleanupStack::PopAndDestroy(1); 				
				return;
				}
			case EReadViagHomeZoneCacheIPC:
				{
				RMmCustomAPI::TViagCacheRecordContent cacheRecord;
				TPckg<RMmCustomAPI::TViagCacheRecordContent> cacheRecordPckg(cacheRecord);

				cacheRecordPckg.Copy(aData);

				dataPackage.PackData(&cacheRecord);
				}
				break;
				
			case ECustomNotifyHSxPAStatusIPC:
			case ECustomReadHSxPAStatusIPC:
				{
				RMmCustomAPI::THSxPAStatus paStatus;
				TPckg<RMmCustomAPI::THSxPAStatus> statusPckg(paStatus);
				
				statusPckg.Copy(aData);

				dataPackage.PackData(&paStatus);
				}
				break;		
				
			case ECustomNotifyRauEventIPC:
				{
				RMmCustomAPI::TRauEventStatus status;
				TPckg<RMmCustomAPI::TRauEventStatus> statusPckg(status);
				
				statusPckg.Copy(aData);

				dataPackage.PackData(&status);
				}
				break;
				
			case ECustomGetSimAuthenticationDataIPC:
				{
				RMmCustomAPI::TSimAuthenticationBase base;
				TPckg<RMmCustomAPI::TSimAuthenticationBase> basePckg(base);
				
				if(aData.Size() >= sizeof(RMmCustomAPI::TSimAuthenticationBase))
					{						
					basePckg.Copy(aData.Ptr(), sizeof(RMmCustomAPI::TSimAuthenticationBase));
					
					if(base.ExtensionId() == RMmCustomAPI::TSimAuthenticationBase::EEapSim)
						{
						RMmCustomAPI::TSimAuthenticationEapSim tmp;
						TPckg<RMmCustomAPI::TSimAuthenticationEapSim> tmpPckg(tmp);
						tmpPckg.Copy(aData);
						RMmCustomAPI::TSimAuthenticationEapSim* ptr(&tmp);
						dataPackage.PackData(ptr);
						}
					else if(base.ExtensionId() == RMmCustomAPI::TSimAuthenticationBase::EEapAka) 
						{
						RMmCustomAPI::TSimAuthenticationEapAka tmp;
						TPckg<RMmCustomAPI::TSimAuthenticationEapAka> tmpPckg(tmp);
						tmpPckg.Copy(aData);
						RMmCustomAPI::TSimAuthenticationEapAka* ptr(&tmp);
						dataPackage.PackData(ptr);
						}
					else if(base.ExtensionId() == RMmCustomAPI::TSimAuthenticationBase::EGbaBootstrap) 
						{
						RMmCustomAPI::TSimAuthenticationGbaBootstrap tmp;
						TPckg<RMmCustomAPI::TSimAuthenticationGbaBootstrap> tmpPckg(tmp);
						tmpPckg.Copy(aData);
						RMmCustomAPI::TSimAuthenticationGbaBootstrap* ptr(&tmp);
						dataPackage.PackData(ptr);
						}
					else if(base.ExtensionId() == RMmCustomAPI::TSimAuthenticationBase::EGbaBootstrapUpdate) 
						{
						RMmCustomAPI::TSimAuthenticationGbaBootstrapUpdate tmp;
						TPckg<RMmCustomAPI::TSimAuthenticationGbaBootstrapUpdate> tmpPckg(tmp);
						tmpPckg.Copy(aData);
						RMmCustomAPI::TSimAuthenticationGbaBootstrapUpdate* ptr(&tmp);
						dataPackage.PackData(ptr);
						}
					else if(base.ExtensionId() == RMmCustomAPI::TSimAuthenticationBase::EGbaBootstrapNafDerivation) 
						{
						RMmCustomAPI::TSimAuthenticationGbaNafDerivation tmp;
						TPckg<RMmCustomAPI::TSimAuthenticationGbaNafDerivation> tmpPckg(tmp);
						tmpPckg.Copy(aData);
						RMmCustomAPI::TSimAuthenticationGbaNafDerivation* ptr(&tmp);
						dataPackage.PackData(ptr);
						}
					else if(base.ExtensionId() == RMmCustomAPI::TSimAuthenticationBase::EMgvMskUpdate) 
						{
						RMmCustomAPI::TSimAuthenticationMgvMskUpdate tmp;
						TPckg<RMmCustomAPI::TSimAuthenticationMgvMskUpdate> tmpPckg(tmp);
						tmpPckg.Copy(aData);
						RMmCustomAPI::TSimAuthenticationMgvMskUpdate* ptr(&tmp);
						dataPackage.PackData(ptr);
						}
					else if(base.ExtensionId() == RMmCustomAPI::TSimAuthenticationBase::EMgvMtkGeneration) 
						{
						RMmCustomAPI::TSimAuthenticationMgvMtkGeneration tmp;
						TPckg<RMmCustomAPI::TSimAuthenticationMgvMtkGeneration> tmpPckg(tmp);
						tmpPckg.Copy(aData);
						RMmCustomAPI::TSimAuthenticationMgvMtkGeneration* ptr(&tmp);
						dataPackage.PackData(ptr);
						}
					else if(base.ExtensionId() == RMmCustomAPI::TSimAuthenticationBase::EMgvMskDeletion) 
						{
						RMmCustomAPI::TSimAuthenticationMgvMskDeletion tmp;
						TPckg<RMmCustomAPI::TSimAuthenticationMgvMskDeletion> tmpPckg(tmp);
						tmpPckg.Copy(aData);
						RMmCustomAPI::TSimAuthenticationMgvMskDeletion* ptr(&tmp);
						dataPackage.PackData(ptr);
						}
					else
						{
						RMmCustomAPI::TSimAuthenticationBase* ptr(&base);
						dataPackage.PackData(ptr);
						}					
					}
				
				}
				break;
				
			case ECustomGetSimCardReaderStatusIPC:
				{
				RMmCustomAPI::TSimCardReaderStatus status;
				TPckg<RMmCustomAPI::TSimCardReaderStatus> statusPckg(status);
				
				statusPckg.Copy(aData);
				
				TPtrC8 ptr(statusPckg);

				dataPackage.PackData(&ptr);
				}
				break;
				
			case ECustomNotifySimCardStatusIPC:
				{
				RMmCustomAPI::TSIMCardStatus status;
				TPckg<RMmCustomAPI::TSIMCardStatus> statusPckg(status);
				
				statusPckg.Copy(aData);

				dataPackage.PackData(&status);
				}
				break;
				
			case ECustomGetATRIPC:
				{
				TPtrC8 ptr(aData);

				dataPackage.PackData(&ptr);
				}
				break;
				
			case ECustomGetCurrentSystemNetworkModesIPC:
			case ECustomGetSystemNetworkModesIPC:
				{
				TUint32 modes;
				TPckg<TUint32> modesPckg(modes);
				
				modesPckg.Copy(aData);

				dataPackage.PackData(&modes);
				}
				break;

	        case ECustomGetLifeTimeIPC:
	        	{
				RMmCustomAPI::TLifeTimeData data;
				TPckg<RMmCustomAPI::TLifeTimeData> dataPckg(data);
				
				dataPckg.Copy(aData);

				dataPackage.PackData(&data);
	        	}				
				break;
					
			case ECustomReadSimFileIPC:
				{
				TPtrC8 ptr(aData);
				TPtrC8* ptrPtr(&ptr);

				dataPackage.PackData(&ptrPtr);
				}
				break;
				
			case ECustomSendAPDUReqIPC:
				{
				TMockLtsyData2Buf <TDesC8*, TDesC8*> ret;
				ret.DeserialiseL(aData);
				
        		TPtrC8 infoPtr(*(ret.Data1()));
        		TPtrC8 dataPtr(*(ret.Data2()));
        		
				dataPackage.PackData(&infoPtr, &dataPtr);
				
				}
				break;
				
			case ECustomSendAPDUReqV2IPC:
			    {
			    RMmCustomAPI::TApduParameters data;
			    TPckg<RMmCustomAPI::TApduParameters> dataPckg(data);
			    dataPckg.Copy(aData);
                dataPackage.PackData(&data);
			    }
			    break;
				
			case ECustomStartSimCbTopicBrowsingIPC:
				{				
				TMockLtsyData1Buf <CArrayFixFlat<RMmCustomAPI::TSimCbTopic>* > ret;
				CleanupClosePushL(ret);
				ret.DeserialiseL(aData);
				dataPackage.PackData(ret.Data1Ptr());
				iMessageRouter->MessageManagerCallback()->Complete(aIpc, &dataPackage, aResult);
				CleanupStack::PopAndDestroy(1); 				
				return;
				}
				
			case ECustomCheckTwoDigitDialSupportIPC:
				{
				RMmCustomAPI::TTwoDigitDialSupport data;
				TPckg<RMmCustomAPI::TTwoDigitDialSupport> dataPckg(data);
				
				dataPckg.Copy(aData);

				dataPackage.PackData(&data);
				}
				break;
				
			case ECustomNotifySsNetworkEventIPC:
				{
				TMockLtsyData2Buf <RMmCustomAPI::TOperatorId, RMmCustomAPI::TSsInfo> ret;
				ret.DeserialiseL(aData);
				dataPackage.PackData(ret.Data1Ptr(), ret.Data2Ptr());
				}
				break;
				
			case ECustomGetProgrammableOperatorLogoIPC:
				{
				TMockLtsyData2Buf <RMmCustomAPI::TOperatorId, RMmCustomAPI::TOperatorLogo> ret;
				ret.DeserialiseL(aData);
				dataPackage.PackData(ret.Data1Ptr(), ret.Data2Ptr());
				}
				break;
				
			case ECustomGetOperatorNameIPC:
				{
				RMmCustomAPI::TOperatorNameInfo data;
				TPckg<RMmCustomAPI::TOperatorNameInfo> dataPckg(data);
				
				dataPckg.Copy(aData);

				dataPackage.PackData(&data);
				}
				break;
				
			case ECustomCheckEmergencyNumberIPC:
				{
				TMockLtsyData1Buf < RMmCustomAPI::TMobileTelNumber*> ret;
				CleanupClosePushL(ret);
				ret.DeserialiseL(aData);
				dataPackage.PackData(ret.Data1Ptr());
				iMessageRouter->MessageManagerCallback()->Complete(aIpc, &dataPackage, aResult);
				CleanupStack::PopAndDestroy(1); 				
				return;
				}
				
			case ECustomGetNetworkProviderNameIPC:
				{
				TName name;
				TPckg<TName> dataPckg(name);
				
				dataPckg.Copy(aData);
				
				TDes* desPtr = reinterpret_cast<TDes*>(&name);
 
				dataPackage.PackData(&desPtr);
				}
				break;

	        case EMmTsySecurityGetSimActivePinStateIPC:
	        	{
	        	RMobilePhone::TMobilePhoneSecurityCode data;
				TPckg<RMobilePhone::TMobilePhoneSecurityCode> dataPckg(data);
				
				dataPckg.Copy(aData);

				dataPackage.PackData(&data);
	        	}
				break;
	        	
			case ECustomIsBlockedIPC:
				{
				TBool data;
				TPckg<TBool> dataPckg(data);
				
				dataPckg.Copy(aData);

				dataPackage.PackData(&data);
				}
				break;
				
			case ECustomNotifySsRequestCompleteIPC:
				{
				TInt data;
				TPckg<TInt> dataPckg(data);
				
				dataPckg.Copy(aData);

				dataPackage.PackData(&data);
				}
				break;
				
			case ECustomSsAdditionalInfoNotificationIPC:
				{					
				RMmCustomAPI::TSsAdditionalInfo data;
				TPckg<RMmCustomAPI::TSsAdditionalInfo> dataPckg(data);
				
				dataPckg.Copy(aData);
				
				RMmCustomAPI::TSsAdditionalInfo* ptr(&data);

				dataPackage.PackData(&ptr);
				}
				break;
				
			case ECustomGetBandSelectionIPC:
				{
				TMockLtsyData2Buf <RMmCustomAPI::TBandSelection, RMmCustomAPI::TNetworkModeCaps> ret;
				ret.DeserialiseL(aData);
				dataPackage.PackData(ret.Data1Ptr(), ret.Data2Ptr());
				}
				break;
			
			case ECustomGetServiceTableSupportbyApplicationIPC:
				{
				RMmCustomAPI::TAppSupport data;
				TPckg<RMmCustomAPI::TAppSupport> dataPckg(data);
				
				dataPckg.Copy(aData);
				dataPackage.PackData(&data);
				}
				break;
			
			case ECustomWriteHSxPAStatusIPC:
			case ECustomNetWakeupIPC:
			case ECustomSetDriveModeIPC:
			case ECustomTerminateCallIPC:
			case EWriteViagHomeZoneUHZIUESettingsIPC:
			case ECustomSimWarmResetIPC:
			case ECustomPowerSimOffIPC:
			case ECustomPowerSimOnIPC:
			case ECustomSetSystemNetworkModeIPC:
			case ECustomDisablePhoneLockIPC:
			case ECustomNotifyNetworkConnectionFailureIPC:
			case ECustomDeleteSimCbTopicIPC:
			case ECustomResetNetServerIPC:
			case ECustomSatRefreshCompleteNotificationIPC:
			case ECustomCheckSecurityCodeIPC:
			case ECustomCheckSecurityCodeCancelIPC:
			case ECustomClearCallBlackListIPC:
			case ECustomSetBandSelectionIPC:
			
			default:
				// complete event without data
				break;
			}
		
		iMessageRouter->MessageManagerCallback()->Complete(aIpc, &dataPackage, aResult);
	}

//  End of File 

