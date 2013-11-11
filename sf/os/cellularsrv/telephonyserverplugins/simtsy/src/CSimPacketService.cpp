// Copyright (c) 2001-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// implementattion for CSimPacketService Class
// This file contains the implementation of the Similator TSY Packet Service functionality.  
// CSimPacketService class provides a simulated packet-switched connection to a packet 
// (GPRS and CDMA) network.
// 
//

/**
 @file
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSimPacketServiceTraces.h"
#endif

#include "CSimPacketService.h"
#include <pcktcs.h>
#include "CSimPhone.h"
#include "utils.h"
#include "CSimPacketContext.h"
#include "CSimPubSubChange.h"


#include "CSimIncomingContextManager.h"


_LIT(KApn, "ACME INTERNET PROVIDERS LTD");
_LIT(KProtocolAddress, "i-MODE GATEWAY");
_LIT(KUsername, "USER1");
_LIT(KPassword, "PASSWORD1");
_LIT(KChallenge, "CHALLENGE1");
_LIT(KResponse, "RESPONSE1");
_LIT(KPrimaryDNS, "PRIMARYSERVER1");
_LIT(KSecondaryDNS, "SECONDARYSERVER1");


// enum for caps
const TUint KPacketServiceCaps=(RPacketService::KCapsSuspendSupported |
								RPacketService::KCapsMSClassSupported |
								RPacketService::KCapsCDMA2000HighSpeedDataSupported |
								RPacketService::KCapsNetworkAvailabilitySupported |
								RPacketService::KCapsProtocolPPPSupported |
								RPacketService::KCapsHSDPASupported |
								RPacketService::KCapsEGPRSSupported |
								RPacketService::KCapsHSUPASupported );
								 
CSimPacketService* CSimPacketService::NewL(CSimPhone* aPhone)
/**
* Standard two phase constructor
*
* @param aPhone Pointer to the phone object (CSimPhone)
* @return CSimPacketService pointer to the packet service object created.
* @leave Leaves if no memory or object is not created for any reason.
*/
	{
	CSimPacketService* simPacket=new(ELeave) CSimPacketService(aPhone);
	CleanupStack::PushL(simPacket);
	simPacket->ConstructL();
	CleanupStack::Pop();
	return simPacket;
	}

void CSimPacketService::Init()
	{}

CSimPacketService::CSimPacketService(CSimPhone* aPhone)
	: iPhone(aPhone), iContextCount(0), iPrimaryContextNameInc(0),
	  iSecondaryContextNameInc(0), iAttachMode(RPacketService::EAttachWhenNeeded),
	  iCurrentEvent(EPacketEventNone), iPacketReleaseModeIndex(0), 
	  iPcktTimerBool(EFalse), iCurrentPrimaryContextIndex(0)
/**
* Trivial Constructor.  Initialises all the data members
*
* @param aPhone pointer to the phone object that owns this class.
*/
	{
	iState=RPacketService::EStatusUnattached;
	iMSClass = RPacketService::EMSClassSuspensionRequired;
	iNotifyStatusChange.iNotifyPending = EFalse;		
	iNotifyContextAdded.iNotifyPending = EFalse;
	iNotifyChangeOfNtwk.iNotifyPending = EFalse;
	iNotifyMSClassChange.iNotifyPending = EFalse;
	iNotifyChangeOfReleaseMode.iNotifyPending=EFalse;
	iNotifyChangeOfAttachMode.iNotifyPending=EFalse;
	iNotifyDynamicCapsChange.iNotifyPending = EFalse;
	iNotifyMbmsNetworkStatusChange.iNotifyPending = EFalse;
	iNotifyMbmsServAvailChange.iNotifyPending = EFalse;
	iSecondaryContext = EFalse;
	}


void CSimPacketService::ConstructL()
/**
* Second phase of the 2-phase constructor.
* Constructs all the member data and retrieves all the data from the config file specific to this class.
*
* @leave Leaves no memory or any data member does not construct for any reason.
* @panic Panics with SIMTSY EInvalidParameterFormatInConfigFile if an additional param data item is in an invalid format
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_CONSTRUCTL_1, "CSimPacketService: Entered ConstructL()");
	
	iTimer = CSimTimer::NewL(iPhone);
	iPcktRegTimer = CSimTimer::NewL(iPhone);
	iNtwkRegTimer = CSimTimer::NewL(iPhone);
	iReleaseModeTimer = CSimTimer::NewL(iPhone);
	iDynamicCapsTimer = CSimTimer::NewL(iPhone);
	iMbmsPcktTimer = CSimTimer::NewL(iPhone);
	User::LeaveIfError(iMutex.CreateGlobal(KNullDesC, EOwnerProcess));
	
	iNtwkRegStatusInfo = new(ELeave) CArrayFixFlat<TNtwkRegStatus>(KGranularity);
	iPcktRegStatusInfo=new(ELeave) CArrayFixFlat<TPacketRegStatus>(KGranularity);
	iPacketReleaseModeArray=new (ELeave) CArrayFixFlat<TPacketReleaseMode>(KGranularity);
	iDynamicCapsArray = new (ELeave) CArrayFixFlat<TPacketDynamicCaps>(KGranularity);
	iSecondaryContextCountArray = new(ELeave)CArrayFixFlat<TInt>(KGranularity);
	iMbmsBroadcastParamsArray = new(ELeave)CArrayFixFlat<TMbmsBroadCastParams>(KGranularity);
	iMbmsMonitorListParamsArray = new(ELeave)CArrayFixFlat<TMbmsMonitorListParams>(KGranularity);
	iMbmsServicesList = new CArrayPtrFlat<CPcktListReadAllAttempt>(1);
	
	iNtwkRegStatusChangeProperty = CSimPubSub::TPubSubProperty(KUidPSSimTsyCategory, KPSSimtsyPacketServiceNtwkRegStatusChange, KPSSimtsyPacketServiceNtwkRegStatusChangeKeyType);
	iNtwkRegStatusChange =  CSimPubSub::NewL(this,iNtwkRegStatusChangeProperty);
	iNtwkRegStatusChange->Start(); 
	
	iMbmsActivation = CSimPubSubChange::NewL(this,CSimPubSub::TPubSubProperty(KUidPSSimTsyCategory, KPSSimTsyMbmsService, KPSSimTsyMbmsServiceType));

	const CTestConfigItem* item=NULL;
	TInt ret=KErrNone;
	
	item=CfgFile()->Item(KAttachPauseDuration); //< Retrieves the attach pause duration number from the config file
	if(item)
		{
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,iAttachPause);
		if(ret!=KErrNone)
			iAttachPause = KDefaultPauseDuration;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,iAttachFail);
		if(ret==KErrNotFound)
			iAttachFail = KErrNone;
		}

	item=CfgFile()->Item(KDetachPauseDuration);	//< Retrieves the detach pause duration number from the config file
	if(item)
		{
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,iDetachPause);
		if(ret!=KErrNone)
			iDetachPause = KDefaultPauseDuration;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,iDetachFail);
		if(ret==KErrNotFound)
			iDetachFail = KErrNone;
		}

	TInt count=CfgFile()->ItemCount(KPacketRegStatus); //< Retrieves the packet registration info
	item=NULL;
	TInt i;
	for(i=0;i<count;i++)
		{
		item=CfgFile()->Item(KPacketRegStatus,i);
		if(!item)
			break;

		TInt time,regState;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,time);
		if(ret!=KErrNone)
			break;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,regState);
		if(ret!=KErrNone)
			break;

		TPacketRegStatus status;
		status.iDuration=time;
		status.iStatus=(RPacketService::TStatus)regState;
		TRAP_IGNORE(iPcktRegStatusInfo->AppendL(status));
		}

	iNtwkRegStatusNotificationType = ETimer;
	item=CfgFile()->Item(KNetworkRegStatusType);
	if(item)
		{
		TPtrC8 networkRegStatusType;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,networkRegStatusType);
		if(ret==KErrNone && networkRegStatusType.Compare(KNotificationTypePublishSubscribe)==0)
			{
			iNtwkRegStatusNotificationType = EPublishAndSubscribe;
			}
		}

	count=CfgFile()->ItemCount(KNetworkRegStatus); //< Retrieves the packet registration info
	item=NULL;
	for(i=0;i<count;i++)
		{
		item=CfgFile()->Item(KNetworkRegStatus,i);
		if(!item)
			break;

		TInt time,regState;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,time);
		if(ret!=KErrNone)
			break;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,regState);
		if(ret!=KErrNone)
			break;

		TNtwkRegStatus status;
		status.iDuration=time;
		status.iRegStatus=(RPacketService::TRegistrationStatus)regState;
		TRAP_IGNORE(iNtwkRegStatusInfo->AppendL(status));
		}

	if(iPcktRegStatusInfo->Count()!=0)
		{
		iPcktRegStatIndex=0;
		iState=(RPacketService::TStatus)iPcktRegStatusInfo->At(0).iStatus;
		iPcktRegTimer->Start(iPcktRegStatusInfo->At(0).iDuration,this,ETimerIdPcktServPacket);
		}

	if(iNtwkRegStatusInfo->Count()!=0)
		{
		iCurrentRegStatus=(RPacketService::TRegistrationStatus)iNtwkRegStatusInfo->At(0).iRegStatus;
		if (iNtwkRegStatusNotificationType == ETimer)
			{
			iNtwkRegStatIndex=0;
			iNtwkRegTimer->Start(iNtwkRegStatusInfo->At(0).iDuration,this,ETimerIdPcktServNtwkStatusChange);
			}
		}

	//< Read in all dynamic capabilities data
	count = CfgFile()->ItemCount(KDynamicCaps);
	item = NULL;
	for (i = 0; i < count; ++i)
		{
		item = CfgFile()->Item(KDynamicCaps,i);
		if (!item)
			break;
		
		TInt duration(0); 
		TInt dynamicCaps(0);
		ret = CTestConfig::GetElement(item->Value(),KStdDelimiter,0,duration);
		if (ret != KErrNone)
			break;
		ret = CTestConfig::GetElement(item->Value(),KStdDelimiter,1,dynamicCaps);
		if (ret != KErrNone)
			break;
		
		TPacketDynamicCaps dynamicCapsData;
		dynamicCapsData.iDuration = duration;
		dynamicCapsData.iDynamicCaps = dynamicCaps;
		
		TRAP_IGNORE(iDynamicCapsArray->AppendL(dynamicCapsData));
		}
	
	if (iDynamicCapsArray->Count() != 0)
		{
		iDynamicCapsIndex = 0;
		TPacketDynamicCaps initialCaps = iDynamicCapsArray->At(0);
		iCurrentDynamicCaps = initialCaps.iDynamicCaps;
		iDynamicCapsTimer->Start(initialCaps.iDuration, this, ETimerIdPcktServDynamicCaps);
		}
	// end of dynamic caps simulation setup
	
	//< Read in all the data for getting the default GPRS configuration parameters
	
	item = CfgFile()->Item(KDefaultContextparamGPRS);
	if(item) 
		{
		TInt protocolType,pdpCompression, anonymousAccess, useEdge;
		TPtrC8 gsnAddress, protocolAddress;
		TInt protocol;	
		TPtrC8 username,password, challenge, response, primaryDNS, secondaryDNS, iMiscBuffer;
		TInt id, numAdditionalParams;

		iGPRSDefaultConfig.iProtocolConfigOption.iMiscBuffer.Zero();
		
		TPtr8 pcoPtr(const_cast<TUint8*>(iGPRSDefaultConfig.iProtocolConfigOption.iMiscBuffer.Ptr()), 
			RPacketContext::KMiscProtocolBufferLength);

		// Attach TTlv to the buffer
		TTlvStruct<RPacketContext::TPcoId,RPacketContext::TPcoItemDataLength> tlvStruct(pcoPtr,0);

		RPacketContext::TPcoId pcoId;
		
		TUint itemOffSet = 0; // Used for nested Addional Param items		
		
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,protocolType);
		if(ret==KErrNone)
			iGPRSDefaultConfig.iProtocolType = protocolType;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,gsnAddress);
		if(ret==KErrNone)
			iGPRSDefaultConfig.iGsnAddress.Set(gsnAddress);

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,protocolAddress);
		if(ret==KErrNone)
			iGPRSDefaultConfig.iProtocolAddress.Set(protocolAddress);

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,pdpCompression);
		if(ret==KErrNone)
			iGPRSDefaultConfig.iPdpCompression = pdpCompression;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,anonymousAccess);
		if(ret==KErrNone)
			iGPRSDefaultConfig.iAnonymousAccess = anonymousAccess;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,5,useEdge);
		if(ret==KErrNone)
			iGPRSDefaultConfig.iUseEdge = useEdge;

		// Protocol configuration and DNS server parameters added
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,6,protocol);
		if(ret==KErrNone)
			iGPRSDefaultConfig.iProtocolConfigOption.iAuthProtocol = protocol;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,7,username);
		if(ret==KErrNone)
			iGPRSDefaultConfig.iProtocolConfigOption.iUsername.Set(username);

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,8,password);
		if(ret==KErrNone)
			iGPRSDefaultConfig.iProtocolConfigOption.iPassword.Set(password);

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,9,challenge);
		if(ret==KErrNone)
			iGPRSDefaultConfig.iProtocolConfigOption.iChallenge.Set(challenge);

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,10,response);
		if(ret==KErrNone)
			iGPRSDefaultConfig.iProtocolConfigOption.iResponse.Set(response);

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,11,primaryDNS);
		if(ret==KErrNone)
			iGPRSDefaultConfig.iProtocolConfigOption.iPrimaryDNS.Set(primaryDNS);

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,12,secondaryDNS);
		if(ret==KErrNone)
			iGPRSDefaultConfig.iProtocolConfigOption.iSecondaryDNS.Set(secondaryDNS);

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,13,id);
		if(ret==KErrNone)
			iGPRSDefaultConfig.iProtocolConfigOption.iId = id;
		
		//Get the Additional Params List
		//Get the number of items in the Additional Params List
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,14,numAdditionalParams);
		if(ret==KErrNone)
			{
			TInt nestedItemNumber;
			TUint16 digit, format;
			const CTestConfigItem* item2;
			
			for(nestedItemNumber=itemOffSet; nestedItemNumber<itemOffSet+numAdditionalParams; nestedItemNumber++)
				{
				item2 = CfgFile()->Item(KDefaultGPRSAdditionalParamItem, nestedItemNumber);
		
				if(item2)
					{
					TPtrC8 additionalParamId, additionalParamContent, additionalParamDataFormat;
				
					// Get ID
					ret=CTestConfig::GetElement(item2->Value(),KStdDelimiter,0,additionalParamId);
					if(ret==KErrNone)
						{
						if(AsciiToNum(additionalParamId, digit)==KErrNone)
							{
							pcoId.SetId(digit);
							}
						}
					
					// Read AdditionalParamDataFormat field to find out how
					// to interpret data in the AdditionalParamData field
					ret=CTestConfig::GetElement(item2->Value(),KStdDelimiter,2,additionalParamDataFormat);
					if (ret != KErrNone)
						{
						// AdditionalParamDataFormat not been specified,
						// default to plain ASCII
						OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_CONSTRUCTL_2, "WARNING - CONFIGURATION FILE PARSING - Reading element ADDITIONALPARAMITEM::ADDITIONALPARAMDATAFORMAT returned %d (element no. %d) from tag %s.",ret,2,KDefaultGPRSAdditionalParamItem);
						format = EConfigDataFormatAscii;
						}
					else
						{
						if (AsciiToNum(additionalParamDataFormat, format) != KErrNone)
							{
							OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_CONSTRUCTL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element ADDITIONALPARAMITEM::ADDITIONALPARAMDATAFORMAT returned %d (element no. %d) from tag %s.",KErrArgument,2,KDefaultGPRSAdditionalParamItem);
							format = EConfigDataFormatAscii;
							}

						// Check that the AdditionalParamDataFormat value
						// specified in the config file is a valid one,
						// default to ASCII if not.
						if (format >= EMaxConfigDataFormat)
							{
							OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_CONSTRUCTL_4, "WARNING - CONFIGURATION FILE PARSING - Reading element ADDITIONALPARAMITEM::ADDITIONALPARAMDATAFORMAT returned %d (element no. %d) from tag %s.",KErrArgument,2,KDefaultGPRSAdditionalParamItem);
							format = EConfigDataFormatAscii;
							}
						}
										
					// Get AdditionalParamData
					ret=CTestConfig::GetElement(item2->Value(),KStdDelimiter,1,additionalParamContent);
					if(ret==KErrNone)
						{
						// Add AdditionalParamData to PCO buffer depending on format
						switch (format)
							{
						case EConfigDataFormatMixedBinaryAndAscii:
							{
							// Parse the string here
							// Will store the parsed data when algorithm has completed
							TBuf8<RPacketContext::KMiscProtocolBufferLength> additionalParamDataBuffer;
							additionalParamDataBuffer.Copy (additionalParamContent);

							if (ParseMixedBinaryAsciiDataL(additionalParamDataBuffer) != KErrNone)
								{
								OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_CONSTRUCTL_5, "WARNING - CONFIGURATION FILE PARSING - Reading element ADDITIONALPARAMITEM::ADDITIONALPARAMCONTENT returned %d (element no. %d) from tag %s.",KErrArgument,1,KDefaultGPRSAdditionalParamItem);
								SimPanic(EInvalidParameterFormatInConfigFile);
								}

							TPtr8 paramPtr(const_cast<TUint8*>(additionalParamDataBuffer.Ptr()), 
								additionalParamDataBuffer.Length(), additionalParamDataBuffer.Length());
							tlvStruct.AppendItemL(pcoId, paramPtr);

							} // case EAdditionalParamDataFormatMixedBinaryAndAscii
							break;
						case EConfigDataFormatAscii:
						default:
							TPtr8 paramPtr(const_cast<TUint8*>(additionalParamContent.Ptr()), 
								additionalParamContent.Length(), additionalParamContent.Length());
							tlvStruct.AppendItemL(pcoId, paramPtr);

							break;
							} // switch (format)
						
						iGPRSDefaultConfig.iProtocolConfigOption.iMiscBuffer.SetLength(pcoPtr.Length());						
						} // if (ret == KErrNone)
					}
				}

				itemOffSet= itemOffSet + numAdditionalParams;;
			}
		else
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_CONSTRUCTL_6, "WARNING - CONFIGURATION FILE PARSING - Reading element PROTOCOLCONFIGOPTION::NUMADDITIONALPARAMS returned %d (element no. %d) from tag %s.",ret,14,KDefaultContextparamGPRS);
			} 
		} 

	//< Read in all the data for getting the default UMTS configuration parameters
	item = CfgFile()->Item(KDefaultContextparamRel99);
	if(item)
		{
		TInt protocolType,useEdge;
		TPtrC8 gsnAddress, protocolAddress;
		TInt protocol;
		TPtrC8 username,password, challenge, response, primaryDNS, secondaryDNS, iMiscBuffer;
		TInt id, numAdditionalParams;

		iR99_R4DefaultConfig.iProtocolConfigOption.iMiscBuffer.Zero();
		
		TPtr8 pcoPtr(const_cast<TUint8*>(iR99_R4DefaultConfig.iProtocolConfigOption.iMiscBuffer.Ptr()), 
			RPacketContext::KMiscProtocolBufferLength);

		// Attach TTlv to the buffer
		TTlvStruct<RPacketContext::TPcoId,RPacketContext::TPcoItemDataLength> tlvStruct(pcoPtr,0);

		RPacketContext::TPcoId pcoId;
		
		TUint itemOffSet = 0; // Used for nested Addional Param items

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,protocolType);
		if(ret==KErrNone)
			iR99_R4DefaultConfig.iProtocolType = protocolType;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,gsnAddress);
		if(ret==KErrNone)
			iR99_R4DefaultConfig.iGsnAddress.Set(gsnAddress);

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,protocolAddress);
		if(ret==KErrNone)
			iR99_R4DefaultConfig.iProtocolAddress.Set(protocolAddress);

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,useEdge);
		if(ret==KErrNone)
			iR99_R4DefaultConfig.iUseEdge = useEdge;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,protocol);
		if(ret==KErrNone)
			iR99_R4DefaultConfig.iProtocolConfigOption.iAuthProtocol = protocol;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,5,username);
		if(ret==KErrNone)
			iR99_R4DefaultConfig.iProtocolConfigOption.iUsername.Set(username);

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,6,password);
		if(ret==KErrNone)
			iR99_R4DefaultConfig.iProtocolConfigOption.iPassword.Set(password);

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,7,challenge);
		if(ret==KErrNone)
			iR99_R4DefaultConfig.iProtocolConfigOption.iChallenge.Set(challenge);

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,8,response);
		if(ret==KErrNone)
			iR99_R4DefaultConfig.iProtocolConfigOption.iResponse.Set(response);

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,9,primaryDNS);
		if(ret==KErrNone)
			iR99_R4DefaultConfig.iProtocolConfigOption.iPrimaryDNS.Set(primaryDNS);

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,10,secondaryDNS);
		if(ret==KErrNone)
			iR99_R4DefaultConfig.iProtocolConfigOption.iSecondaryDNS.Set(secondaryDNS);

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,11,id);
		if(ret==KErrNone)
			iR99_R4DefaultConfig.iProtocolConfigOption.iId = id;
		
		//Get the Additional Params List
		//Get the number of items in the Additional Params List
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,12,numAdditionalParams);
		if(ret==KErrNone)
			{
			TInt nestedItemNumber;
			TUint16 digit, format;
			const CTestConfigItem* item2;
			
			for(nestedItemNumber=itemOffSet; nestedItemNumber<itemOffSet+numAdditionalParams; nestedItemNumber++)
				{
				item2 = CfgFile()->Item(KDefaultAdditionalParamItemRel99, nestedItemNumber);
		
				if(item2)
					{
					TPtrC8 additionalParamId, additionalParamContent, additionalParamDataFormat;
				
					ret=CTestConfig::GetElement(item2->Value(),KStdDelimiter,0,additionalParamId);
					if(ret==KErrNone)
						{
						if(AsciiToNum(additionalParamId, digit)==KErrNone)
							{
							pcoId.SetId(digit);
							}
						}

					// Read AdditionalParamDataFormat field to find out how
					// to interpret data in the AdditionalParamData field
					ret=CTestConfig::GetElement(item2->Value(),KStdDelimiter,2,additionalParamDataFormat);
					if (ret != KErrNone)
						{
						// AdditionalParamDataFormat not been specified,
						// default to plain ASCII
						OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_CONSTRUCTL_7, "WARNING - CONFIGURATION FILE PARSING - Reading element NO ADDITIONALPARAMITEM::ADDITIONALPARAMDATAFORMAT returned %d (element no. %d) from tag %s.",ret,2,KDefaultAdditionalParamItemRel99);
						format = EConfigDataFormatAscii;
						}
					else
						{
						if (AsciiToNum(additionalParamDataFormat, format) != KErrNone)
							{
							OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_CONSTRUCTL_8, "WARNING - CONFIGURATION FILE PARSING - Reading element ADDITIONALPARAMITEM::ADDITIONALPARAMDATAFORMAT returned %d (element no. %d) from tag %s.",KErrArgument,2,KDefaultAdditionalParamItemRel99);
							format = EConfigDataFormatAscii;
							}

						// Check that the AdditionalParamDataFormat value
						// specified in the config file is a valid one,
						// default to ASCII if not.
						if (format >= EMaxConfigDataFormat)
							{
							OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_CONSTRUCTL_9, "WARNING - CONFIGURATION FILE PARSING - Reading element ADDITIONALPARAMITEM::ADDITIONALPARAMDATAFORMAT returned %d (element no. %d) from tag %s.",KErrArgument,2,KDefaultAdditionalParamItemRel99);
							format = EConfigDataFormatAscii;
							}
						}
						
					ret=CTestConfig::GetElement(item2->Value(),KStdDelimiter,1,additionalParamContent);
					if(ret==KErrNone)
						{
						// Add AdditionalParamData to PCO buffer depending on format
						switch (format)
							{
						case EConfigDataFormatMixedBinaryAndAscii:
							{
							// Parse the string here
							// Will store the parsed data when algorithm has completed
							TBuf8<RPacketContext::KMiscProtocolBufferLength> additionalParamDataBuffer;
							additionalParamDataBuffer.Copy (additionalParamContent);

							if (ParseMixedBinaryAsciiDataL(additionalParamDataBuffer) != KErrNone)
								{
								OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_CONSTRUCTL_10, "WARNING - CONFIGURATION FILE PARSING - Reading element ADDITIONALPARAMITEM::ADDITIONALPARAMCONTENT returned %d (element no. %d) from tag %s.",KErrArgument,1,KDefaultAdditionalParamItemRel99);
								SimPanic(EInvalidParameterFormatInConfigFile);
								}

							TPtr8 paramPtr(const_cast<TUint8*>(additionalParamDataBuffer.Ptr()), 
								additionalParamDataBuffer.Length(), additionalParamDataBuffer.Length());
							tlvStruct.AppendItemL(pcoId, paramPtr);

							} // case EAdditionalParamDataFormatMixedBinaryAndAscii
							break;
						case EConfigDataFormatAscii:
						default:
							TPtr8 paramPtr(const_cast<TUint8*>(additionalParamContent.Ptr()), 
								additionalParamContent.Length(), additionalParamContent.Length());
							tlvStruct.AppendItemL(pcoId, paramPtr);

							break;
							} // switch (format)
						iR99_R4DefaultConfig.iProtocolConfigOption.iMiscBuffer.SetLength(pcoPtr.Length());
						}
					}
				}

				itemOffSet= itemOffSet + numAdditionalParams;;
			}
		else
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_CONSTRUCTL_11, "WARNING - CONFIGURATION FILE PARSING - Reading element PROTOCOLCONFIGOPTION::NUMADDITIONALPARAMS returned %d (element no. %d) from tag %s.",ret,12,KDefaultContextparamRel99);
			} 		

		}

		//< Read in all the data for getting the default R5 configuration parameters
		item = CfgFile()->Item(KDefaultContextParamRel5);
		if(item)
			{
			TInt protocolType, useEdge, protocol, id;
			TInt pdpDataCompression, pdpHeaderCompression, numAdditionalParams;
			TPtrC8 gsnAddress, protocolAddress, username, password, challenge;
			TPtrC8 response, primaryDNS, secondaryDNS, iMiscBuffer;

			iR5DefaultConfig.iProtocolConfigOption.iMiscBuffer.Zero();

			TPtr8 pcoPtr(const_cast<TUint8*>(iR5DefaultConfig.iProtocolConfigOption.iMiscBuffer.Ptr()), 
				RPacketContext::KMiscProtocolBufferLength);

			// Attach the TTlv to the buffer
			TTlvStruct<RPacketContext::TPcoId,RPacketContext::TPcoItemDataLength> tlvStruct(pcoPtr,0);

			RPacketContext::TPcoId pcoId;
			
			TUint itemOffSet = 0; //Used for nested Addional Param items

			//Get the Protocol Type from the config file
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,protocolType);
			if(ret==KErrNone)
				{
				iR5DefaultConfig.iProtocolType = protocolType;	
				}
			
			//Get the GGSN Address from the config file
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,gsnAddress);
			if(ret==KErrNone)
				{
				iR5DefaultConfig.iGsnAddress.Set(gsnAddress);
				}

			//Get the ProtocolAddress from the config file
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,protocolAddress);
			if(ret==KErrNone)
				{
				iR5DefaultConfig.iProtocolAddress.Set(protocolAddress);
				}	
				
			//Get the UseEdge config param from the config file
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,useEdge);
			if(ret==KErrNone)
				{
				iR5DefaultConfig.iUseEdge = useEdge;	
				}
			
			
			//Get the Autorisation Protocol config param from the config file
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,protocol);
			if(ret==KErrNone)
				{
				iR5DefaultConfig.iProtocolConfigOption.iAuthProtocol = protocol;	
				}


			//Get the Username config param from the config file
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,5,username);
			if(ret==KErrNone)
				{
				iR5DefaultConfig.iProtocolConfigOption.iUsername.Set(username);	
				}

			//Get the psword config param from the config file
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,6,password);
			if(ret==KErrNone)
				{
				iR5DefaultConfig.iProtocolConfigOption.iPassword.Set(password);	
				}
			
			//Get the Challenge config param from the config file
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,7,challenge);
			if(ret==KErrNone)
				{
				iR5DefaultConfig.iProtocolConfigOption.iChallenge.Set(challenge);	
				}

			//Get the Response config param from the config file
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,8,response);
			if(ret==KErrNone)
				{
				iR5DefaultConfig.iProtocolConfigOption.iResponse.Set(response);	
				}

			//Get the Primary DNS config param from the config file
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,9,primaryDNS);
			if(ret==KErrNone)
				{
				iR5DefaultConfig.iProtocolConfigOption.iPrimaryDNS.Set(primaryDNS);	
				}

			//Get the Secondary DNS config param from the config file
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,10,secondaryDNS);
			if(ret==KErrNone)
				{
				iR5DefaultConfig.iProtocolConfigOption.iSecondaryDNS.Set(secondaryDNS);	
				}
				
			//Get the ID config param from the config file.
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,11,id);
			if(ret==KErrNone)
				{
				iR5DefaultConfig.iProtocolConfigOption.iId = id;	
				}

			//Get the PDP Header Compression params
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,12,pdpHeaderCompression);
			if(ret==KErrNone)
				{
				iR5DefaultConfig.iPdpHeaderCompression = pdpHeaderCompression;
				}
			
			//Get the PDP Data compression params
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,13,pdpDataCompression);
			if(ret==KErrNone)
				{
				iR5DefaultConfig.iPdpDataCompression = pdpDataCompression;
				}
				

			//Get the Additional Params List
			//Get the number of items in the Additional Params List
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,14,numAdditionalParams);
			if(ret==KErrNone)
				{
				TInt nestedItemNumber;
				TUint16 digit, format;
				const CTestConfigItem* item2;
				
				for(nestedItemNumber=itemOffSet; nestedItemNumber<itemOffSet+numAdditionalParams; nestedItemNumber++)
					{
					item2 = CfgFile()->Item(KDefaultAdditionalParamItem, nestedItemNumber);
			
					if(item2)
						{
						TPtrC8 additionalParamId, additionalParamContent, additionalParamDataFormat;
					
						ret=CTestConfig::GetElement(item2->Value(),KStdDelimiter,0,additionalParamId);
						if(ret==KErrNone)
							{
							if(AsciiToNum(additionalParamId, digit)==KErrNone)
								{
								pcoId.SetId(digit);
								}
							}

					// Read AdditionalParamDataFormat field to find out how
					// to interpret data in the AdditionalParamData field
					ret=CTestConfig::GetElement(item2->Value(),KStdDelimiter,2,additionalParamDataFormat);
					if (ret != KErrNone)
						{
						// AdditionalParamDataFormat not been specified,
						// default to plain ASCII
						OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_CONSTRUCTL_12, "WARNING - CONFIGURATION FILE PARSING - Reading element NO ADDITIONALPARAMITEM::ADDITIONALPARAMDATAFORMAT returned %d (element no. %d) from tag %s.",ret,2,KDefaultAdditionalParamItem);
						format = EConfigDataFormatAscii;
						}
					else
						{
						if (AsciiToNum(additionalParamDataFormat, format) != KErrNone)
							{
							OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_CONSTRUCTL_13, "WARNING - CONFIGURATION FILE PARSING - Reading element ADDITIONALPARAMITEM::ADDITIONALPARAMDATAFORMAT returned %d (element no. %d) from tag %s.",KErrArgument,2,KDefaultAdditionalParamItem);
							format = EConfigDataFormatAscii;
							}

						// Check that the AdditionalParamDataFormat value
						// specified in the config file is a valid one,
						// default to ASCII if not.
						if (format >= EMaxConfigDataFormat)
							{
							OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_CONSTRUCTL_14, "WARNING - CONFIGURATION FILE PARSING - Reading element ADDITIONALPARAMITEM::ADDITIONALPARAMDATAFORMAT returned %d (element no. %d) from tag %s.",KErrArgument,2,KDefaultAdditionalParamItem);
							format = EConfigDataFormatAscii;
							}
						}

						ret=CTestConfig::GetElement(item2->Value(),KStdDelimiter,1,additionalParamContent);
						if(ret==KErrNone)
							{
							// Add AdditionalParamData to PCO buffer depending on format
							switch (format)
								{
							case EConfigDataFormatMixedBinaryAndAscii:
								{
								// Parse the string here
								// Will store the parsed data when algorithm has completed
								TBuf8<RPacketContext::KMiscProtocolBufferLength> additionalParamDataBuffer;
								additionalParamDataBuffer.Copy (additionalParamContent);

								if (ParseMixedBinaryAsciiDataL(additionalParamDataBuffer) != KErrNone)
									{
									OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_CONSTRUCTL_15, "WARNING - CONFIGURATION FILE PARSING - Reading element ADDITIONALPARAMITEM::ADDITIONALPARAMCONTENT returned %d (element no. %d) from tag %s.",KErrArgument,1,KDefaultAdditionalParamItem);
									SimPanic(EInvalidParameterFormatInConfigFile);
									}

								TPtr8 paramPtr(const_cast<TUint8*>(additionalParamDataBuffer.Ptr()), 
									additionalParamDataBuffer.Length(), additionalParamDataBuffer.Length());
								tlvStruct.AppendItemL(pcoId, paramPtr);

								} // case EAdditionalParamDataFormatMixedBinaryAndAscii
								break;
							case EConfigDataFormatAscii:
							default:
								TPtr8 paramPtr(const_cast<TUint8*>(additionalParamContent.Ptr()), 
									additionalParamContent.Length(), additionalParamContent.Length());
								tlvStruct.AppendItemL(pcoId, paramPtr);

								break;
								} // switch (format)
							iR5DefaultConfig.iProtocolConfigOption.iMiscBuffer.SetLength(pcoPtr.Length());
							}
						}
					}

					itemOffSet= itemOffSet + numAdditionalParams;;
				}
			}

		count=CfgFile()->ItemCount(KPacketReleaseMode); //< Retrieves the packet registration info
		item=NULL;
		for(i=0;i<count;i++)
			{
			item=CfgFile()->Item(KPacketReleaseMode,i);
			if(!item)
				break;
			TInt release,duration;
			
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,release);
			if(ret!=KErrNone)
				break;
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,duration);
			if(ret!=KErrNone)
				break;

			TPacketReleaseMode releaseMode;
			releaseMode.iDuration=duration;
			releaseMode.iReleaseMode=release;
			TRAP_IGNORE(iPacketReleaseModeArray->AppendL(releaseMode));
			}

		item=NULL;
		count=CfgFile()->ItemCount(KMBMSBroadCastPacketServiceParams); //< Retrieves the Count of MBMS related information

		for(i=0;i<count;i++)
			{
			item = CfgFile()->Item(KMBMSBroadCastPacketServiceParams,i); //< Retrieves the MBMS related information

			TInt duration =0;
			TInt status =0;
			if(!item)
				{
				break;
				}

			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,duration);
			if(ret!=KErrNone)
				{
				break;
				}
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,status);
			if(ret!=KErrNone)
				{
				status = EMbmsSupportUnknown;
				}

			TMbmsBroadCastParams mbmsParams;
			mbmsParams.iDuration = duration;
			mbmsParams.iMbmsNetworkStatus = reinterpret_cast<TMbmsNetworkServiceStatus&> (status);
			TRAP_IGNORE(iMbmsBroadcastParamsArray->AppendL(mbmsParams));
			}

		item=NULL;
		count=CfgFile()->ItemCount(KMBMSBroadcastMonitorList); //< Retrieves the Count of MBMS related information

		for(i=0;i<count;i++)
			{
			item = CfgFile()->Item(KMBMSBroadcastMonitorList,i); //< Retrieves the MBMS related information

			TInt duration =0;
			TInt mnc=0;
			TInt mcc=0;
			TInt serviceId=0;
			TInt accessBearer=0;
			TInt serviceMode=0;
			TInt serviceAvailability=0;
			TInt entryValid=0;
			
			if(!item)
				{
				break;
				}

			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,duration);
			if(ret!=KErrNone)
				{
				break;
				}

			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,mcc);
			if(ret!=KErrNone)
				{
				break;
				}

			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,mnc);
			if(ret!=KErrNone)
				{
				break;
				}

			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,serviceId);
			if(ret!=KErrNone)
				{
				break;
				}

			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,accessBearer);
			if(ret!=KErrNone)
				{
				break;
				}

			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,5,serviceMode);
			if(ret!=KErrNone)
				{
				break;
				}

			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,6,serviceAvailability);
			if(ret!=KErrNone)
				{
				break;
				}

			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,7,entryValid);
			if(ret!=KErrNone)
				{
				entryValid = 0; //default Entry is Invalid
				}
			TMbmsMonitorListParams mbmsMonitorParams;

			mbmsMonitorParams.iDuration = duration;
			mbmsMonitorParams.iMCC = mcc;
			mbmsMonitorParams.iMNC = mnc;
			mbmsMonitorParams.iServiceId = serviceId;
			mbmsMonitorParams.iAccessBearer = reinterpret_cast<TMbmsScope&> (accessBearer);
			mbmsMonitorParams.iServiceMode = reinterpret_cast<TMbmsServiceMode&> (serviceMode);
			mbmsMonitorParams.iAvailStatus = reinterpret_cast<TMbmsAvailabilityStatus&> (serviceAvailability);
			mbmsMonitorParams.iIsEntryValid = reinterpret_cast<TBool&> (entryValid);

			TRAP_IGNORE(iMbmsMonitorListParamsArray->AppendL(mbmsMonitorParams));

			}

			if(iMbmsBroadcastParamsArray->Count()!=0)
				{
				iMbmsPcktTimer->Start(iMbmsBroadcastParamsArray->At(0).iDuration,this,ETimerIdMbmsPcktService);
				}
			else if (iMbmsMonitorListParamsArray->Count()!=0)
				{
				iMbmsPcktTimer->Start(iMbmsMonitorListParamsArray->At(0).iDuration,this,ETimerIdMbmsPcktService);
				}

			if(iPacketReleaseModeArray->Count()!=0)
				{
				iPacketReleaseModeIndex = 0;
				iCurrentPacketReleaseMode = iPacketReleaseModeArray->At(0).iReleaseMode;
				iReleaseModeTimer->Start(iPacketReleaseModeArray->At(0).iDuration,this,ETimerIdPcktServReleaseMode);
				}
			iIncomingContextManager = CSimIncomingContextManager::NewL(iPhone, this);
			iIncomingContextManager->LoadIncomingContextsL( KDelayIncomingContext() );
			iIncomingContextManager->SetUseTimerOn();
			}
	

CSimPacketService::~CSimPacketService()
/**
* Trivial Destructor
* Closes all CObject type objects and destroys all other objects created in the ConstructL()
*
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_DTOR_1, "CSimPacketService: Entered destructor");
		
	delete iIncomingContextManager;
	
	iMutex.Close();
	// delete nifs' contexts
	int count = iNifWrappers.Count();

	for(TInt i = 0; i<count; i++)
		{
		iNifWrappers[i].Close();
		}

	iNifWrappers.Close();
	
	if (iPcktRegStatusInfo != NULL)
		{
		iPcktRegStatusInfo->Delete(0,iPcktRegStatusInfo->Count());
		delete iPcktRegStatusInfo;
		}

	if(iNtwkRegStatusInfo != NULL)
		{
		iNtwkRegStatusInfo->Delete(0,iNtwkRegStatusInfo->Count());
		delete iNtwkRegStatusInfo;
		}

	delete iTimer;
	
	delete iPcktRegTimer;

	delete iNtwkRegTimer;

	delete iReleaseModeTimer;
	
	delete iDynamicCapsTimer;
	
	delete iMbmsPcktTimer;

	if (iPacketReleaseModeArray != NULL)
		{	
		iPacketReleaseModeArray->Delete(0,iPacketReleaseModeArray->Count());
		delete iPacketReleaseModeArray;
		}
	if (iDynamicCapsArray != NULL)
		{
		iDynamicCapsArray->Delete(0,iDynamicCapsArray->Count());
		delete iDynamicCapsArray;
		}
	
	if (iSecondaryContextCountArray != NULL)
		{
		iSecondaryContextCountArray->Delete(0,iSecondaryContextCountArray->Count());
		delete iSecondaryContextCountArray;
		}
	
	if(iMbmsBroadcastParamsArray != NULL)
		{
		iMbmsBroadcastParamsArray->Delete(0,iMbmsBroadcastParamsArray->Count());
		delete iMbmsBroadcastParamsArray;
		}
	
	if(iMbmsMonitorListParamsArray != NULL)
		{
		iMbmsMonitorListParamsArray->Delete(0,iMbmsMonitorListParamsArray->Count());
		delete iMbmsMonitorListParamsArray;
		}
	
	if(iMbmsServicesList)
		{
		iMbmsServicesList->ResetAndDestroy();
		}
	delete iMbmsServicesList;
	
	// delete nifs' contexts
	count = iMbmsNifWrappers.Count();

	for(TInt i = 0; i<count; i++)
		{
		iMbmsNifWrappers[i].Close();
		}

	iMbmsNifWrappers.Close();
	
	delete iNtwkRegStatusChange;
	
	delete iMbmsActivation;
	}

CSimPhone* CSimPacketService::PhonePtr() const
/**
* Returns a pointer to the phone (parent) object
*
* @return CSimPhone Pointer to the phone object
*/
	{
		return iPhone; 
	}

CTelObject* CSimPacketService::OpenNewObjectByNameL(const TDesC& aName)
/**
* Returns a pointer to an existing Context object identified by name.
* If the object is deleted then reopen it then return a handle to it.
*
* @param aName name of the Context object to be opened
* @return CTelObject pointer to the CSimPacketContext object or null if not possible or not found
* @leave Leaves if object cannot be opened or created.
*/
	{
	CSimPacketContext*  packetContext = NULL;

	// go through every nif, and from there through every context in nif
	TInt counter = iNifWrappers.Count();
	
	for(TInt i=0; i<counter; i++)
		{
		// now go through all contexts of the nif at index i
		TInt totalContexts = iNifWrappers[i].NumberOfContexts();
		for (TInt x=0; x<totalContexts; x++)
			{
			packetContext = iNifWrappers[i].GetContext(x);
						
			// if context name matches parameter, reactivate
			if(aName.Compare(packetContext->ContextName())==KErrNone)
				{
				iCurrentPrimaryContextIndex = i;
				TInt err = packetContext->ReActivate(this,aName);
				if(err != KErrNone)
					{
					User::Leave(err);
					return NULL;
					}	
				return packetContext;
				}
			}
		}
	
	// if the control has reached here, then the context requested is not in the PDP context
	// hence lets search in the list of MBMS contexts.
	CSimMbmsPacketContext* pcktMbmsContext = NULL;
	
	counter = iMbmsNifWrappers.Count();
	
	for(TInt i=0;i<counter;i++)
		{
		TInt totalContexts = iMbmsNifWrappers[i].NumberOfContexts();
		for (TInt x=0; x<totalContexts; x++)
			{
			pcktMbmsContext = iMbmsNifWrappers[i].GetContext(x);
						
			// if context name matches parameter, reactivate
			if(aName.Compare(pcktMbmsContext->ContextName())==KErrNone)
				{
				TInt err = pcktMbmsContext->ReActivate(this,aName);
				if(err != KErrNone)
					{
					User::Leave(err);
					return NULL;
					}	
				return pcktMbmsContext;
				}
			}
		}
	
	User::Leave(KErrNotFound);
	return NULL;
	}

CTelObject* CSimPacketService::OpenNewObjectL(TDes& aNewName)
/**
* Creates a new CSimPacketContext object and returns a pointer to it.
*
* @param aName new name of the object created
* @return CTelObject pointer to the CSimPacketContext object created
* @leave Leaves if out of memory.
*/
	{
	// Check whether the context creation is for MBMS of PDP context

	if(aNewName.Compare(_L("MBMS_CONTEXT"))== KErrNone)  
		{
		// Processing for MBMS context creation starts
		CSimMbmsPacketContext* pcktMbmsContext = NULL;

		TInt ctr = iMbmsNifWrappers.Count();
		TInt mbmsCnxtCount = 0;
		for(TInt i=0;i<ctr; i++)
			{
			TInt mbmsContextsCount = iMbmsNifWrappers[i].NumberOfContexts();
			for(TInt j=0;j<mbmsContextsCount;j++)
				{
				pcktMbmsContext = iMbmsNifWrappers[i].GetContext(j);
				RPacketContext::TContextStatus tStat = pcktMbmsContext->ContextStatus();
				if(tStat != RPacketContext::EStatusDeleted)
					{
					mbmsCnxtCount++;
					}
				}
			}
		if ( mbmsCnxtCount > KMaxMbmsContextSupportedByPhone )
			{
			if(iNotifyContextAdded.iNotifyPending)
				{
				iNotifyContextAdded.iNotifyPending = EFalse;
				ReqCompleted(iNotifyContextAdded.iNotifyHandle,KErrUmtsMaxNumOfContextExceededByPhone);
				}
			User::Leave(KErrNoMemory);
			return NULL;
			}
		else if (mbmsCnxtCount > KMaxMbmsContextSupportedByNetwork)
			{
			if(iNotifyContextAdded.iNotifyPending)
				{
				iNotifyContextAdded.iNotifyPending = EFalse;
				ReqCompleted(iNotifyContextAdded.iNotifyHandle,KMaxMbmsContextSupportedByNetwork);
				}
			User::Leave(KErrNoMemory);
			return NULL;
			}
		
		aNewName.AppendNum(++iMbmsContextNameInc);  
		pcktMbmsContext = CSimMbmsPacketContext::NewL(iPhone, this, aNewName);
		CleanupStack::PushL(pcktMbmsContext);
		
		if(pcktMbmsContext->Open()==KErrNone)
			{
			TMbmsNifWrapper tMbmsNif;
			tMbmsNif.AddContext(pcktMbmsContext);
			iMbmsNifWrappers.Append(tMbmsNif);
			}
		iMbmsContextCount++;		

		// Check for a pending line state notification.
		if(iNotifyContextAdded.iNotifyPending)
			{
			iNotifyContextAdded.iNotifyPending=EFalse;
			*(reinterpret_cast<TName*>(iNotifyContextAdded.iNotifyData))=pcktMbmsContext->ContextName();
			ReqCompleted(iNotifyContextAdded.iNotifyHandle,KErrNone);
			}

		CleanupStack::Pop(pcktMbmsContext);
		return pcktMbmsContext;


		} // End of MBMS Context processing.

	else 
		{
		// Processing for PDP context creation starts
		
		// if there are more contexts than the maximum allowed, return error code
		CSimPacketContext*  packetContext = NULL;

		// go through every nif, and from there through every context in nif
		TInt counter = iNifWrappers.Count();
		TInt contextCount=0;

		for(TInt i=0; i<counter; i++)
			{
			// now go through all contexts of the nif at index i
			TInt totalContexts = iNifWrappers[i].NumberOfContexts();
			for (TInt x=0; x<totalContexts; x++)
				{
				packetContext = iNifWrappers[i].GetContext(x);	
				RPacketContext::TContextStatus tstat = packetContext->ContextStatus();
				if(tstat != RPacketContext::EStatusDeleted)
					contextCount++;
				}
			}

		if(contextCount >= KMaxContextPoolSize)
			{
			if(iNotifyContextAdded.iNotifyPending)
				{
				iNotifyContextAdded.iNotifyPending = EFalse;
				ReqCompleted(iNotifyContextAdded.iNotifyHandle,KErrNoMemory);
				}
			User::Leave(KErrNoMemory);
			return NULL;
			}

		CSimPacketContext* newContext;
		
		// if this is not a secondary context, give the new context the primary
		// context name increment
		if (!iSecondaryContext)
			{
			aNewName.Append(KSimPacketPrimaryContextName); 
			aNewName.AppendNum(++iPrimaryContextNameInc);

			newContext = CSimPacketContext::NewL(iPhone, this, aNewName);
			CleanupStack::PushL(newContext);
			if (newContext->Open() == KErrNone)
				{
				TNifWrapper nif;
				nif.AddContext(newContext);
				iNifWrappers.Append(nif);
				iCurrentPrimaryContextIndex = iNifWrappers.Count() - 1;
				}

			// for every primary context, add an element in the array so that a count
			// can be kept for each primary's secondary contexts
			iSecondaryContextCountArray->AppendL(iCurrentPrimaryContextIndex);
			iCurrentPrimaryContextIndex++;
			}
		else
			{ 
			// if this is a secondary context, name it accordingly
			// and increment the count of the secondary's primary context
			aNewName.Append(KSimPacketSecondaryContextName);
			iSecondaryContextCountArray->At(iCurrentPrimaryContextIndex)++;
			aNewName.AppendNum(iSecondaryContextCountArray->At(iCurrentPrimaryContextIndex));

			newContext = CSimPacketContext::NewL(iPhone, this, aNewName);
			CleanupStack::PushL(newContext);

			if(newContext->Open()==KErrNone)
				{
				iNifWrappers[iCurrentPrimaryContextIndex].AddContext(newContext);
				iSecondaryContextNameInc++;	// keep a global count of number of secondary contexts
				}
			}
		iContextCount++;

		// Check for a pending line state notification.
		if(iNotifyContextAdded.iNotifyPending)
			{
			iNotifyContextAdded.iNotifyPending=EFalse;
			*(TName*)iNotifyContextAdded.iNotifyData=newContext->ContextName();
			ReqCompleted(iNotifyContextAdded.iNotifyHandle,KErrNone);
			}

		// ie assume false until PrepareOpenSecondary() is called again
		iSecondaryContext = EFalse;
		CleanupStack::Pop(newContext);
		return newContext;
		}
	}

CTelObject::TReqMode CSimPacketService::ReqModeL(const TInt aIpc)
/**
* ReqModeL is called from the server's CTelObject::ReqAnalyserL
* in order to check the type of request it has.
* 
* 
* @param aIpc the ipc number that identifies the client request
* @return CTelObject::TReqMode The request mode to be used for this request
* @leave Leaves if not supported by this tsy
*/	
	{
	CTelObject::TReqMode ret=0;
	switch (aIpc)
		{
		case EPacketAttach:
		case EPacketDetach:
		case EPacketRejectActivationRequest:
		case EPacketSetMSClass:
		case EPacketSetAttachMode:
		case EPacketSetDefaultContextParams:
		case EPacketGetStatus:
		case EPacketGetContextInfo:
		case EPacketGetNtwkRegStatus:
		case EPacketGetMSClass:
		case EPacketGetStaticCaps:
		case EPacketGetDynamicCaps:
		case EPacketGetAttachMode:
		case EPacketGetDefaultContextParams:
		case EPacketEnumerateContexts:
		case EPacketGetCurrentReleaseMode:
		case EPacketEnumerateNifs:
		case EPacketGetNifInfo:
		case EPacketEnumerateContextsInNif:
		case EPacketGetContextNameInNif:
		case EPacketPrepareOpenSecondary:
		case EPacketGetMbmsNetworkServiceStatus:
		case EPacketUpdateMbmsMonitorServiceList:
		case EPacketGetMbmsMonitoredServicesPhase1:
		case EPacketGetMbmsMonitoredServicesPhase2:
		case EPacketEnumerateMbmsMonitorServiceList:
		case EPacketEnumerateMbmsActiveServiceList:
			break;

		case EPacketNotifyContextAdded:
		case EPacketNotifyStatusChange:
		case EPacketNotifyContextActivationRequested:
		case EPacketNotifyChangeOfNtwkRegStatus:
		case EPacketNotifyMSClassChange:
		case EPacketNotifyDynamicCapsChange:
		case EPacketNotifyReleaseModeChange:
		case EPacketNotifyAttachModeChange:
		case EPacketNotifyMbmsNetworkServiceStatusChange:
		case EPacketNotifyMbmsServiceAvailabilityChange:
			ret=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
			break;

		default:
			User::Leave(KErrNotSupported);
			break;
		}
	return ret;
	}

RHandleBase* CSimPacketService::GlobalKernelObjectHandle()
/**
* Implements CSubSessionExtBase virtual method, which provides
* handle of the mutex object to etel server that passes it up to clients.
* @return pointer to synchronisation mutex object
*/
	{
		return &iMutex;
	}

TInt CSimPacketService::RegisterNotification(const TInt aIpc)
/**
* RegisterNotification is called when the server recognises that this notification
* is being posted for the first time on this sub-session object.
* 
* It enables the TSY to "turn on" any regular notification messages that it may receive 
* from the phone
*
* @param aIpc the ipc number that identifies the client request
* @return err KErrNone if fine
*/
	{
	switch (aIpc)
		{
		case EPacketNotifyContextAdded:
		case EPacketNotifyStatusChange:
		case EPacketNotifyContextActivationRequested:
		case EPacketNotifyChangeOfNtwkRegStatus:
		case EPacketNotifyMSClassChange:
		case EPacketNotifyDynamicCapsChange:
		case EPacketNotifyReleaseModeChange:
		case EPacketNotifyAttachModeChange:
		case EPacketNotifyMbmsNetworkServiceStatusChange:
		case EPacketNotifyMbmsServiceAvailabilityChange:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_REGISTERNOTIFICATION_1, "CSimPacketService: RegisterNotification");
			return KErrNone;
		default:
			// Unknown or invalid IPC
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_REGISTERNOTIFICATION_2, "CSimPacketService: Register error, unknown IPC");
			return KErrNotSupported;
		}
	}

TInt CSimPacketService::DeregisterNotification(const TInt aIpc)
/**
* DeregisterNotification is called when the server recognises that this notification
* will not be posted again because the last client to have a handle on this sub-session
* object has just closed the handle.
*
* It enables the TSY to "turn off" any regular notification messages that it may 
* receive from the phone
*
* @param aIpc the ipc number that identifies the client request
* @return err KErrNone if fine
*/
	{
	switch (aIpc)
		{
		case EPacketNotifyContextAdded:
		case EPacketNotifyStatusChange:
		case EPacketNotifyContextActivationRequested:
		case EPacketNotifyChangeOfNtwkRegStatus:
		case EPacketNotifyMSClassChange:
		case EPacketNotifyDynamicCapsChange:
		case EPacketNotifyReleaseModeChange:
		case EPacketNotifyAttachModeChange:
		case EPacketNotifyMbmsNetworkServiceStatusChange:
		case EPacketNotifyMbmsServiceAvailabilityChange:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_DEREGISTERNOTIFICATION_1, "CSimPacketService: DeregisterNotification");
			return KErrNone;
		default:
			// Unknown or invalid IPC
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_DEREGISTERNOTIFICATION_2, "CSimPacketService: Deregister error, unknown IPC");
			return KErrNotSupported;
		}
	}

TInt CSimPacketService::NumberOfSlotsL(const TInt aIpc)
/**
* NumberOfSlotsL is called by the server when it is registering a new notification
* It enables the TSY to tell the server how many buffer slots to allocate for
* "repost immediately" notifications that may trigger before clients collect them
*
* @param aIpc the ipc number that identifies the client request
* @return err KErrNone if fine
*/
	{
	TInt numberOfSlots=1;
	switch (aIpc)
		{
		case EPacketNotifyStatusChange:
		case EPacketNotifyDynamicCapsChange:
		case EPacketNotifyMbmsNetworkServiceStatusChange:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_NUMBEROFSLOTSL_1, "CSimPacketService: Registered with 10 slots");
			numberOfSlots=10;
			break;
		case EPacketNotifyContextAdded:
		case EPacketNotifyContextActivationRequested:
		case EPacketNotifyChangeOfNtwkRegStatus:
		case EPacketNotifyMSClassChange:
		case EPacketNotifyReleaseModeChange:
		case EPacketNotifyAttachModeChange:
		case EPacketNotifyMbmsServiceAvailabilityChange:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_NUMBEROFSLOTSL_2, "CSimPacketService: Registered with 2 slots");
			numberOfSlots=2;
			break;
		default:
			// Unknown or invalid IPC
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_NUMBEROFSLOTSL_3, "CSimPacketService: Number of Slots error, unknown IPC");
			User::Leave(KErrNotSupported);
			break;
		}  
	return numberOfSlots;
	}


TInt CSimPacketService::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,
							  const TDataPackage& aPackage)
/**
* ExtFunc is called by the server when it has a "extended", i.e. non-core ETel request
* for the TSY to process.
* A request handle, request type and request data are passed to the TSY
* 
* @param aTsyReqHandle  The request handle for completing the request 
* @param aIpc Ipc representing the request
* @param aPackage any data associated with the request
* @return err KErrNone if request completes ok
*/
	{
	TAny* dataPtr=aPackage.Ptr1();
	TAny* dataPtr2=aPackage.Ptr2();

	switch (aIpc)
		{
		case EPacketNotifyContextAdded:
			return NotifyContextAdded(aTsyReqHandle, aPackage.Des1u());
		case EPacketGetStatus:
			return GetStatus(aTsyReqHandle,
					REINTERPRET_CAST(RPacketService::TStatus*, dataPtr));
		case EPacketNotifyStatusChange:
			return NotifyStatusChange(aTsyReqHandle,
					REINTERPRET_CAST(RPacketService::TStatus*, dataPtr));
		case EPacketNotifyContextActivationRequested:
			return NotifyContextActivationRequested(aTsyReqHandle, 
					aPackage.Des1n());
		case EPacketEnumerateContexts:
			return EnumerateContexts(aTsyReqHandle,
					REINTERPRET_CAST(TInt*, dataPtr),
					REINTERPRET_CAST(TInt*, dataPtr2));
		case EPacketGetContextInfo:
			return GetContextInfo(aTsyReqHandle,
					REINTERPRET_CAST(TInt*, dataPtr),
					REINTERPRET_CAST(RPacketService::TContextInfo*, dataPtr2));
		case EPacketGetNtwkRegStatus:
			return GetNtwkRegStatus(aTsyReqHandle,
					REINTERPRET_CAST(RPacketService::TRegistrationStatus*, dataPtr));
		case EPacketNotifyChangeOfNtwkRegStatus:
			return NotifyChangeOfNtwkRegStatus(aTsyReqHandle,
					REINTERPRET_CAST(RPacketService::TRegistrationStatus*, dataPtr));
		case EPacketGetMSClass:
			return GetMSClass(aTsyReqHandle,
					REINTERPRET_CAST(RPacketService::TMSClass*, dataPtr),
					REINTERPRET_CAST(RPacketService::TMSClass*, dataPtr2));
		case EPacketSetMSClass:
			return SetMSClass(aTsyReqHandle,
					REINTERPRET_CAST(RPacketService::TMSClass*, dataPtr));
		case EPacketNotifyMSClassChange:
			return NotifyMSClassChange(aTsyReqHandle,
					REINTERPRET_CAST(RPacketService::TMSClass*, dataPtr));
		case EPacketGetStaticCaps:
			return GetStaticCaps(aTsyReqHandle, 
					REINTERPRET_CAST(TUint*, dataPtr),
					REINTERPRET_CAST(RPacketContext::TProtocolType*, dataPtr2));
		case EPacketGetDynamicCaps:
			return GetDynamicCaps(aTsyReqHandle,
					REINTERPRET_CAST(RPacketService::TDynamicCapsFlags*, dataPtr));
		case EPacketNotifyDynamicCapsChange:
			return NotifyDynamicCapsChange(aTsyReqHandle,
					REINTERPRET_CAST(RPacketService::TDynamicCapsFlags*, dataPtr));
		case EPacketSetAttachMode:
			return SetAttachMode(aTsyReqHandle,
					REINTERPRET_CAST(RPacketService::TAttachMode*, dataPtr));
		case EPacketGetAttachMode:
			return GetAttachMode(aTsyReqHandle,
					REINTERPRET_CAST(RPacketService::TAttachMode*, dataPtr));
		case EPacketNotifyAttachModeChange:
			return NotifyAttachModeChange(aTsyReqHandle,
					REINTERPRET_CAST(RPacketService::TAttachMode*, dataPtr));
		case EPacketAttach:
			return Attach(aTsyReqHandle);
		case EPacketDetach:
			return Detach(aTsyReqHandle);
		case EPacketRejectActivationRequest:
			return RejectActivationRequest(aTsyReqHandle);
		case EPacketSetDefaultContextParams:
			return SetDefaultContextParams(aTsyReqHandle, aPackage.Des1n());
		case EPacketGetDefaultContextParams:
			return GetDefaultContextParams(aTsyReqHandle, aPackage.Des1n());
		case EPacketGetCurrentReleaseMode:
			return GetCurrentReleaseMode(aTsyReqHandle, REINTERPRET_CAST(RPacketService::TPacketReleaseMode*, dataPtr));
		case EPacketNotifyReleaseModeChange:
			return NotifyReleaseModeChange(aTsyReqHandle,
					REINTERPRET_CAST(RPacketService::TPacketReleaseMode*, dataPtr));
		case EPacketEnumerateNifs:
			return EnumerateNifs(aTsyReqHandle, *(REINTERPRET_CAST(TInt*, dataPtr)));
		case EPacketGetNifInfo:
			return GetNifInfo(aTsyReqHandle, *REINTERPRET_CAST(TInt*, dataPtr), aPackage.Des2n());
		case EPacketEnumerateContextsInNif:
			return EnumerateContextsInNif(aTsyReqHandle, aPackage.Des2u(), *REINTERPRET_CAST(TInt*, dataPtr));			
		case EPacketGetContextNameInNif:
			return GetContextNameInNif(aTsyReqHandle, aPackage.Des2u(), *REINTERPRET_CAST(TInt*, dataPtr), aPackage.Des2u());
		case EPacketPrepareOpenSecondary:
			//return PrepareOpenSecondary(aTsyReqHandle, *REINTERPRET_CAST(TDesC*, dataPtr));
			return PrepareOpenSecondary(aTsyReqHandle, aPackage.Des1u());
		case EPacketGetNifInfoCancel:
			return GetNifInfoCancel(aTsyReqHandle);
		case EPacketGetMbmsNetworkServiceStatus:
			return GetMbmsNetworkServiceStatus(aTsyReqHandle, reinterpret_cast<TBool> (dataPtr),
					reinterpret_cast<TMbmsNetworkServiceStatus*> (dataPtr2));
		case EPacketNotifyMbmsNetworkServiceStatusChange:
			return NotifyMbmsNetworkServiceStatusChange(aTsyReqHandle,
					reinterpret_cast<TMbmsNetworkServiceStatus*>(dataPtr));
		case EPacketNotifyMbmsServiceAvailabilityChange:
			return NotifyMbmsServiceAvailabilityChange(aTsyReqHandle);
		case EPacketUpdateMbmsMonitorServiceList:
			TInt retValue;
			TInt err;
			if(dataPtr2)
				{
				TRAP(err, retValue = UpdateMbmsMonitorServiceListL(aTsyReqHandle,
					reinterpret_cast<TMbmsAction*>(dataPtr),aPackage.Des2n()));
				}
			else
				{
				TRAP(err, retValue = UpdateMbmsMonitorServiceListL(aTsyReqHandle,
					reinterpret_cast<TMbmsAction*>(dataPtr)));
				}
			if(err != KErrNone)
				{
				retValue = err;
				}
			return retValue;
		case EPacketGetMbmsMonitoredServicesPhase1:
			return GetMbmsMonitoredServicesListPhase1(aTsyReqHandle, 
					reinterpret_cast<TClientId*>(dataPtr), 
					reinterpret_cast<TInt*>(dataPtr2));
		case EPacketGetMbmsMonitoredServicesPhase2:
			return GetMbmsMonitoredServicesListPhase2(aTsyReqHandle, 
					reinterpret_cast<TClientId*>(dataPtr), aPackage.Des2n());	
		case EPacketEnumerateMbmsMonitorServiceList:
			return EnumerateMbmsMonitorServiceList(aTsyReqHandle,
					reinterpret_cast<TInt*>(dataPtr),
					reinterpret_cast<TInt*>(dataPtr2));
		case EPacketEnumerateMbmsActiveServiceList:
			return EnumerateMbmsActiveServiceList(aTsyReqHandle,
					reinterpret_cast<TInt*>(dataPtr),
					reinterpret_cast<TInt*>(dataPtr2));
		default:
			return KErrNotSupported;
		}
	}

TInt CSimPacketService::CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle)
/**
* CancelService is called by the server when it is "cleaning-up" any still outstanding
* asynchronous requests before closing a client's sub-session.
* This will happen if a client closes its R-class handle without cancelling outstanding asynchronous requests.
* 
*  This function will be called when a client explicitly cancels
*
* @param aTsyReqHandle  The request handle for completing the request 
* @param aIpc Ipc representing the request
* @return err KErrNone if request completes ok
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_CANCELSERVICE_1, "CSimPacketService: - CancelService called");
	switch (aIpc)
		{
		case EPacketNotifyContextAdded:
			return NotifyContextAddedCancel(aTsyReqHandle);
		case EPacketAttach:
			return AttachCancel(aTsyReqHandle);
		case EPacketDetach:
			return DetachCancel(aTsyReqHandle);
		case EPacketNotifyStatusChange:
			return NotifyStatusChangeCancel(aTsyReqHandle);
		case EPacketNotifyContextActivationRequested:
			return NotifyContextActivationRequestedCancel(aTsyReqHandle);
		case EPacketRejectActivationRequest:
			return RejectActivationRequestCancel(aTsyReqHandle);
		case EPacketGetContextInfo:
			return GetContextInfoCancel(aTsyReqHandle);
		case EPacketNotifyChangeOfNtwkRegStatus:
			return NotifyChangeOfNtwkRegStatusCancel(aTsyReqHandle);
		case EPacketGetMSClass:
			return GetMSClassCancel(aTsyReqHandle);
		case EPacketSetMSClass:
			return SetMSClassCancel(aTsyReqHandle);
		case EPacketNotifyMSClassChange:
			return NotifyMSClassChangeCancel(aTsyReqHandle);
		case EPacketNotifyDynamicCapsChange:
			return NotifyDynamicCapsChangeCancel(aTsyReqHandle);
		case EPacketEnumerateContextsCancel:
			return EnumerateContextsCancel(aTsyReqHandle);
		case EPacketGetCurrentReleaseMode:
			return GetCurrentReleaseModeCancel(aTsyReqHandle);
		case EPacketNotifyReleaseModeChange:
			return NotifyReleaseModeChangeCancel(aTsyReqHandle);
		case EPacketEnumerateNifs:
			return EnumerateNifsCancel(aTsyReqHandle);
		case EPacketGetNifInfo:
			return GetNifInfoCancel(aTsyReqHandle);
		case EPacketEnumerateContextsInNif:	
			return EnumerateContextsInNifCancel(aTsyReqHandle);
		case EPacketGetContextNameInNif:
			return GetContextNameInNifCancel(aTsyReqHandle);
		case EPacketNotifyAttachModeChange:
			return NotifyAttachModeChangeCancel(aTsyReqHandle);
		case EPacketGetMbmsNetworkServiceStatus:
			return GetMbmsNetworkServiceStatusCancel(aTsyReqHandle);
		case EPacketNotifyMbmsNetworkServiceStatusChange:
			return NotifyMbmsNetworkServiceStatusChangeCancel(aTsyReqHandle);
		case EPacketNotifyMbmsServiceAvailabilityChange:
			return NotifyMbmsServiceAvailabilityChangeCancel(aTsyReqHandle);
		case EPacketUpdateMbmsMonitorServiceList:
			return UpdateMbmsMonitorServiceListCancel(aTsyReqHandle);
		case EPacketGetMbmsMonitoredServicesPhase1:
		case EPacketGetMbmsMonitoredServicesPhase2:
			return GetMbmsMonitoredServicesListCancel(aTsyReqHandle);
		case EPacketEnumerateMbmsMonitorServiceList:
			return EnumerateMbmsMonitorServiceListCancel(aTsyReqHandle);
		case EPacketEnumerateMbmsActiveServiceList:
			return EnumerateMbmsActiveServiceListCancel(aTsyReqHandle);

		default:
			return KErrGeneral; 
		} 
	}

TInt CSimPacketService::NotifyContextAdded(const TTsyReqHandle aTsyReqHandle, TDes* aContextId)
/**
* Record a client's interest in being notified when a new context is added to the network.
* First check that there isn't already a notification pending (the ETel Server should protect
* against this) and then record the information necessary to complete the request later, when
* the status does actually change.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aContextId pointer to the packet context data
* @return KErrNone
*/
	{
	__ASSERT_ALWAYS(!iNotifyContextAdded.iNotifyPending,SimPanic(ENotificationAlreadyPending));

	iNotifyContextAdded.iNotifyPending = ETrue;
	iNotifyContextAdded.iNotifyHandle = aTsyReqHandle;
	iNotifyContextAdded.iNotifyData = aContextId;
	return KErrNone;
	}

TInt CSimPacketService::NotifyContextAddedCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Cancel a client's interest in being notified when a new context is added.
* This is acheived simply by resetting the flag that indicates a notification is pending.
* 
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_NOTIFYCONTEXTADDEDCANCEL_1, "CSimPacketService::NotifyContextAddedCancel called");
	if(iNotifyContextAdded.iNotifyPending)
		{
		iNotifyContextAdded.iNotifyPending=EFalse;
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	return KErrNone;
	}

TInt CSimPacketService::Attach(const TTsyReqHandle aTsyReqHandle)
/**
* Client calls this method to attach to the service network.
* In this TSY, the request is registered then the state machine (ActionEvent) is called
* to handle the request making sure that the class state is updated and the phone is informed.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @return TInt KErrnone if request completes ok
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_ATTACH_1, "CSimPacketService::Attach called");
	iAttachRequestHandle=aTsyReqHandle;
	return ActionEvent(EPacketEventAttach);
	}

TInt CSimPacketService::AttachCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Cancels a clients interest in attaching to a network.
* this can only be achieved if we're not already attached otherwise the request is ignored.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @return TInt KErrnone if request completes ok
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_ATTACHCANCEL_1, "CSimPacketService::AttachCancel called");
	if((iState == RPacketService::EStatusUnattached) && (iCurrentEvent==EPacketEventAttach))
		{
		iTimer->Cancel();
		iCurrentEvent= EPacketEventNone;
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	return KErrNone;
	}

/***
* This function will detach the packet service and all its contexts from the network
***/
TInt CSimPacketService::DoDetach()
	{
	TInt ret=KErrNone;

	switch(iState)
		{
		case RPacketService::EStatusUnattached:
			break;

		case RPacketService::EStatusAttached:
			ret=ChangeState(RPacketService::EStatusUnattached);
			ReqCompleted(iDetachRequestHandle,ret);
			break;

		case RPacketService::EStatusActive:
		case RPacketService::EStatusSuspended:
			{
			TInt totalWrappers = iNifWrappers.Count();
				
			// deactivate all contexts in every nif
			for(TInt j=0; j < totalWrappers; j++)
				{
				TInt totalContexts = iNifWrappers[j].NumberOfContexts();
				for(TInt x =0; x<totalContexts; x++)
					{
					ret = iNifWrappers[j].GetContext(x)->AttemptDeactivateImmediately();
					if(ret!=KErrNone)
						{
						break;
						}
					}
				}
	
			if(ret==KErrNone)
				{
					ret=ChangeState(RPacketService::EStatusUnattached);
				}
				ReqCompleted(iDetachRequestHandle,ret);
			}
			break;
		default:
			break;
		}
	return ret;
	}
TInt CSimPacketService::Detach(const TTsyReqHandle aTsyReqHandle)
/**
* Client calls this method to detach from the service network.
* In this TSY, the request is registered then the state machine (ActionEvent) is called
* to handle the request making sure that the class state is updated and the phone is informed.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @return TInt KErrnone if request completes ok
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_DETACH_1, "CSimPacketService::Detach called");
	iDetachRequestHandle=aTsyReqHandle;
	return ActionEvent(EPacketEventDetach);
	}

TInt CSimPacketService::DetachCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Cancels a clients interest in detaching from the network.
* this can only be achieved if we're not already attached and in the process of detaching
* but not detached yet otherwise the request is ignored.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @return TInt KErrnone if request completes ok
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_DETACHCANCEL_1, "CSimPacketService::DetachCancel called");
	if((iState == RPacketService::EStatusAttached) && (iCurrentEvent==EPacketEventDetach))
			{
			iTimer->Cancel();
			iCurrentEvent= EPacketEventNone;
			ReqCompleted(aTsyReqHandle,KErrCancel);
			}
	return KErrNone;
	}

TInt CSimPacketService::GetStatus(const TTsyReqHandle aTsyReqHandle, RPacketService::TStatus* aStatus)
/**
* Retrieves and returns the status of the Packet service
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aStatus pointer to the status
* @return TInt err KErrNone if ok
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_GETSTATUS_1, "CSimPacketService::GetStatus called");
	*aStatus = iState;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimPacketService::NotifyStatusChange(const TTsyReqHandle aTsyReqHandle, RPacketService::TStatus* aStatus)
/**
* Record a client's interest in being notified of the state of the packet network and contexts.
* First check that there isn't already a notification pending (the ETel Server should protect
* against this) and then record the information necessary to complete the request later, when
* the status does actually change.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aStatus pointer to the packet service status
* @return KErrNone
*/
	{
 	__ASSERT_ALWAYS(!iNotifyStatusChange.iNotifyPending,SimPanic(ENotificationAlreadyPending));

	iNotifyStatusChange.iNotifyPending = ETrue;
	iNotifyStatusChange.iNotifyHandle = aTsyReqHandle;
	iNotifyStatusChange.iNotifyData = aStatus;
	return KErrNone;
	}

TInt CSimPacketService::NotifyStatusChangeCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Cancel a client's interest in being notified when the call status changes.
* This is acheived simply by resetting the flag that indicates a notification is pending.
* 
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_NOTIFYSTATUSCHANGECANCEL_1, "CSimPacketService::NotifyStatusChangeCancel called");
	if(iNotifyStatusChange.iNotifyPending)
		{
		iNotifyStatusChange.iNotifyPending=EFalse;
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	return KErrNone;
	}



TInt CSimPacketService::NotifyContextActivationRequested(const TTsyReqHandle aTsyReqHandle, TDes8* aContextParameters)
/**
* Record a client's interest in being notified when a context is activated.
* First check that there isn't already a notification pending (the ETel Server should protect
* against this) and then record the information necessary to complete the request later, when
* the status does actually change.
*
* Hurricane does not support this method
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aStatus pointer to the packet service status
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_NOTIFYCONTEXTACTIVATIONREQUESTED_1, "CSimPacketService::NotifyContextActivationRequested called");
	
	iConfigData.iTsyReqHandle = aTsyReqHandle;
	iConfigData.iData = aContextParameters;

	if ( iIncomingContextManager->IsUseTimerOn() )
		{ // Timer use is on, so we can use it.	
		// 1st time by the reguest for notifictaion, 
		// 2nd time when do ReqCompleted(aTsyReqHandle,KErrCancel);
		    
		iIncomingContextManager->StartFirstIncomingEvent();
			// When timer is done we have completed the request to activate a PDP.
		iIncomingContextManager->SetUseTimerOff();
		}
		
	return KErrNone;
	}

TInt CSimPacketService::NotifyContextActivationRequestedCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Cancel a client's interest in being notified when a context is activated.
* This is acheived simply by resetting the flag that indicates a notification is pending.
* 
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_NOTIFYCONTEXTACTIVATIONREQUESTEDCANCEL_1, "CSimPacketService::NotifyContextActivationRequestedCancel called");

	iIncomingContextManager->Cancel();	
	iIncomingContextManager->SetUseTimerOff();	
	
	ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}

TInt CSimPacketService::RejectActivationRequest(const TTsyReqHandle aTsyReqHandle)
/**
* This method is called when a client wishes to reject an Activation request from the network
* This is currently not supported in this version of the simulator tsy.
*
* Hurricane does not support this method
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @return KErrNone
*/
	{
	ReqCompleted(aTsyReqHandle, KErrNotSupported);
	return KErrNone;
	}

TInt CSimPacketService::RejectActivationRequestCancel(const TTsyReqHandle aTsyReqHandle)
/**
* This method cancels a clients' request  in rejecting an activation request.
* The TSY is not supporting this method
*
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	ReqCompleted(aTsyReqHandle, KErrCancel);
	return KErrNone;
	}

TInt CSimPacketService::EnumerateContexts(const TTsyReqHandle aTsyReqHandle, TInt* aCount, TInt* aMaxAllowed)
/**
* Returns the number of contexts opened on the packet service
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aCount pointer to the number of contexts opened.
* @param aMaxAllowed pointer to the maximum number of contexts that can be opened on the packet service
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_ENUMERATECONTEXTS_1, "CSimPacketService::EnumerateContexts called");
	// this should enumerate all primary and secondary contexts
	*aCount = iPrimaryContextNameInc + iSecondaryContextNameInc;
	*aMaxAllowed = KMaxContextPoolSize;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}
	
TInt CSimPacketService::EnumerateContextsCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
* This method cancels a clients' request in Enmerationg the contexts
* This does nothing as Enumerate context is processed synchronously
* 
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_ENUMERATECONTEXTSCANCEL_1, "CSimPacketService::Enumerate Context Cancel called");
	return KErrNone;
	}

TInt CSimPacketService::GetContextInfo(const TTsyReqHandle aTsyReqHandle, TInt* aIndex,RPacketService::TContextInfo* aInfo)
/**
* Retrieves a specific context info specified by the index variable
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aIndex pointer to the index of the context the info is requested from
* @param aInfo pointer to the context info. 
* @return KErrNone if context is found in the list of contexts 
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_GETCONTEXTINFO_1, "CSimPacketService::GetContextInfo called");
	CSimPacketContext*  packetContext;

	if((*aIndex >= iContextCount) || (*aIndex < 0))
		ReqCompleted(aTsyReqHandle,KErrArgument);
	else {
		// this method is used in conjunction with EnumerateContexts() which for Jetstream
		// now returns the number of all contexts - not just the number of primary contexts as
		// was done in Hurricane. To that end, the index passed as a parameter needs to be translated
		// to take into account the nifwrappers array's contexts.
		TInt realIndex=*aIndex;
		TInt i;
		TInt wrappersCount = iNifWrappers.Count();
		for(i = 0; i<wrappersCount; i++)
			{
			if(iNifWrappers[i].NumberOfContexts() <= realIndex)
				{
				realIndex-=iNifWrappers[i].NumberOfContexts();
				continue;
				}
			else
				{
				break;
				}
			}
		packetContext = iNifWrappers[i].GetContext(realIndex);
		aInfo->iName.Copy(packetContext->ContextName());
		aInfo->iStatus = packetContext->ContextStatus();
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	return KErrNone;
	}

TInt CSimPacketService::GetContextInfoCancel(const TTsyReqHandle /*aTsyReqHandle*/) 
/**
* Cancels a clients request for a context info
*
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	return KErrNone;
	}

TInt CSimPacketService::GetNtwkRegStatus(const TTsyReqHandle aTsyReqHandle, RPacketService::TRegistrationStatus* aRegistrationStatus)
/**
* Returns the Network registration status to the client.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aRegistrationStatus pointer to the registration status info requested
* @return KErrNone 
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_GETNTWKREGSTATUS_1, "CSimPacketService::GetNtwkRegStatus called");
	*aRegistrationStatus = (RPacketService::TRegistrationStatus)iCurrentRegStatus;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimPacketService::NotifyChangeOfNtwkRegStatus(const TTsyReqHandle aTsyReqHandle, RPacketService::TRegistrationStatus* aRegistrationStatus)
/**
* Record a client's interest in being notified of a change in the Nk registration status.
* First check that there isn't already a notification pending (the ETel Server should protect
* against this) and then record the information necessary to complete the request later, when
* the status does actually change.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aRegistrationStatus pointer to the Network registration status
* @return KErrNone
*/
	{
	if(iNtwkRegStatusInfo->Count()==0)
		{
		ReqCompleted(aTsyReqHandle,KErrNotSupported);
		return KErrNone;
		}

	__ASSERT_ALWAYS(!iNotifyChangeOfNtwk.iNotifyPending,SimPanic(ENotificationReqAlreadyOutstanding));
	iNotifyChangeOfNtwk.iNotifyPending=ETrue;
	iNotifyChangeOfNtwk.iNotifyHandle=aTsyReqHandle;
	iNotifyChangeOfNtwk.iNotifyData=aRegistrationStatus;
	return KErrNone;
	}

TInt CSimPacketService::NotifyChangeOfNtwkRegStatusCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Cancel a client's interest in being notified of a change in the Nk registration status.
* This is acheived simply by resetting the flag that indicates a notification is pending.
* 
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_NOTIFYCHANGEOFNTWKREGSTATUSCANCEL_1, "CSimPacketService::NotifyChangeOfNtwkRegStatusCancel called");
	if(iNotifyChangeOfNtwk.iNotifyPending)
		{
		iNotifyChangeOfNtwk.iNotifyPending=EFalse;
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	return KErrNone;
	}

TInt CSimPacketService::GetMSClass(const TTsyReqHandle aTsyReqHandle, RPacketService::TMSClass* aCurrentClass, RPacketService::TMSClass* aMaxClass)
/**
* Returns the Current MSClass supported by the TSY.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aCurrentClass pointer to the current MSClass
* @param aMaxClass the maximum MSClass this TSY can support
* @return KErrNone 
*/
	{
	//This version of the Simulator TSY will only support CLASS B i.e EMSClassSuspensionRequired
	*aCurrentClass = iMSClass;
	*aMaxClass = RPacketService::EMSClassSuspensionRequired;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimPacketService::GetMSClassCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
* Cancels a clients request in retrieving the current MSClass supported by the TSY.
*
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	return KErrNone;
	}

TInt CSimPacketService::SetMSClass(const TTsyReqHandle aTsyReqHandle, RPacketService::TMSClass* aClass)
/**
* Sets the MSClass for the TSY. This TSY current support Class B only and cannot be reset.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aClass pointer to the MSClass to set the TSY to support
* @return KErrNone
*/
	{
	if(*aClass != RPacketService::EMSClassSuspensionRequired)
		ReqCompleted(aTsyReqHandle,KErrNotSupported);
	else {
		iMSClass = *aClass;
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	return KErrNone;
	}

TInt CSimPacketService::SetMSClassCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
* Cancels a clients interest in setting the MSClass support for the TSY.
*
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	return KErrNone;
	}

TInt CSimPacketService::NotifyMSClassChange(const TTsyReqHandle aTsyReqHandle, RPacketService::TMSClass* /*aNewClass*/)
/**
* Record a client's interest in being notified of a change of MSClass supported.
* First check that there isn't already a notification pending (the ETel Server should protect
* against this) and then record the information necessary to complete the request later, when
* the status does actually change.
* 
* Currently the TSY will only support Class B and thus this method is not supported.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aNewClass pointer to the MSClass to set to
* @return KErrNone
*/
	{
	ReqCompleted(aTsyReqHandle,KErrNotSupported);
	return KErrNone;
	}

TInt CSimPacketService::NotifyMSClassChangeCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
* Cancel a client's interest in being notified of a change of MSClass supported.
* This is acheived simply by resetting the flag that indicates a notification is pending.
* 
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	return KErrNone;
	}

TInt CSimPacketService::GetStaticCaps(const TTsyReqHandle aTsyReqHandle, TUint* aCaps,RPacketContext::TProtocolType* aPdpType)
/**
* Returns the Static capabilities supported by the TSY
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aCaps pointer to the TSY's capability
* @param aPdpType protocol type used by the TSY
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_GETSTATICCAPS_1, "CSimPacketService::GetStaticCaps called");
	*aCaps = KPacketServiceCaps;
	*aPdpType = RPacketContext::EPdpTypePPP;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

/**
* Returns the Dynamic capabilities supported by the TSY
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aCaps Pointer to be populated with the dynamic capabilities of the TSY
* @return KErrNone
*/
TInt CSimPacketService::GetDynamicCaps(const TTsyReqHandle aTsyReqHandle, RPacketService::TDynamicCapsFlags* aCaps)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_GETDYNAMICCAPS_1, "CSimPacketService::GetDynamicCaps called");
	
	if (iDynamicCapsArray->Count() == 0)
		{
		ReqCompleted(aTsyReqHandle, KErrNotSupported);
		return KErrNone;
		}
	*aCaps = iCurrentDynamicCaps;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

/**
* Registers a client interest of change in the Dynamic capabilities change.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aCaps Pointer to be populated with the dynamic capabilities of the TSY
* @return KErrNone
*/
TInt CSimPacketService::NotifyDynamicCapsChange(const TTsyReqHandle aTsyReqHandle, RPacketService::TDynamicCapsFlags* aCaps)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_NOTIFYDYNAMICCAPSCHANGE_1, "CSimPacketService::NotifyDynamicCapsChange called");
	__ASSERT_ALWAYS(!iNotifyDynamicCapsChange.iNotifyPending,SimPanic(ENotificationAlreadyPending));
 
	if (iDynamicCapsArray->Count() == 0)
		{
		ReqCompleted(aTsyReqHandle,KErrNotSupported);
		return KErrNone;
		}
	
	iNotifyDynamicCapsChange.iNotifyPending = ETrue;
	iNotifyDynamicCapsChange.iNotifyHandle = aTsyReqHandle;
	iNotifyDynamicCapsChange.iNotifyData = aCaps;
	return KErrNone;
	}

/**
* Cancels a clients interest in the change of dynamic caps.
*
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
TInt CSimPacketService::NotifyDynamicCapsChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_NOTIFYDYNAMICCAPSCHANGECANCEL_1, "CSimPacketService::NotifyDynamicCapsChangeCancel called");
	if( (iNotifyDynamicCapsChange.iNotifyPending) && (aTsyReqHandle == iNotifyDynamicCapsChange.iNotifyHandle))
		{
		iNotifyDynamicCapsChange.iNotifyPending=EFalse;
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	return KErrNone;
	}

/**
* Timer callback function to simulate a change in the dynamic capabilities of the TSY
*/
void CSimPacketService::TimerCallBackDynamicCaps()
	{
	if (++iDynamicCapsIndex >= iDynamicCapsArray->Count())
		{
		return;
		}
	iCurrentDynamicCaps = iDynamicCapsArray->At(iDynamicCapsIndex).iDynamicCaps;
	if (iNotifyDynamicCapsChange.iNotifyPending)
		{
		iNotifyDynamicCapsChange.iNotifyPending = EFalse;
		*(TInt*)iNotifyDynamicCapsChange.iNotifyData = iCurrentDynamicCaps;
		ReqCompleted(iNotifyDynamicCapsChange.iNotifyHandle,KErrNone);
		}
	
	iDynamicCapsTimer->Start(iDynamicCapsArray->At(iDynamicCapsIndex).iDuration, this, ETimerIdPcktServDynamicCaps);
	}

TInt CSimPacketService::SetAttachMode(const TTsyReqHandle aTsyReqHandle, RPacketService::TAttachMode* aMode)
/**
* Sets the attach mode to be used by the tsy.
* Currently The TSY supports EAttachWhenNeeded only.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aMode pointer to the mode to change to
* @return KErrNone 
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_SETATTACHMODE_1, "CSimPacketService::SetAttachMode called");
	if(*aMode != RPacketService::EAttachWhenNeeded)
		{
		ReqCompleted(aTsyReqHandle, KErrNotSupported);
		}
	else 
		{
		iAttachMode = *aMode;
		// Check for a pending attach mode notification.
		if(iNotifyChangeOfAttachMode.iNotifyPending)
			{
			iNotifyChangeOfAttachMode.iNotifyPending=EFalse;
			*(RPacketService::TAttachMode*)iNotifyChangeOfAttachMode.iNotifyData = iAttachMode;
			ReqCompleted(iNotifyChangeOfAttachMode.iNotifyHandle,KErrNone);
			}	
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	return KErrNone;
	}

TInt CSimPacketService::GetAttachMode(const TTsyReqHandle aTsyReqHandle, RPacketService::TAttachMode* aMode)
/**
* Retrieves and returns the current attach mode for the TSY.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aMode pointer to the mode to change
* @return KErrNone 
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_GETATTACHMODE_1, "CSimPacketService::GetAttachMode called");
	*aMode = iAttachMode;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}
	
TInt CSimPacketService::NotifyAttachModeChange(const TTsyReqHandle aTsyReqHandle, RPacketService::TAttachMode* aMode)
/**
* Set notification request for change of current attach mode
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aMode pointer to the mode to change
* @return KErrNone 
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_NOTIFYATTACHMODECHANGE_1, "CSimPacketService::NotifyAttachModeChange called");
	
	__ASSERT_ALWAYS(!iNotifyChangeOfAttachMode.iNotifyPending,SimPanic(ENotificationAlreadyPending));
	
	iNotifyChangeOfAttachMode.iNotifyPending=ETrue;
	iNotifyChangeOfAttachMode.iNotifyHandle=aTsyReqHandle;
	iNotifyChangeOfAttachMode.iNotifyData=aMode;
		
	return KErrNone;
	}

TInt CSimPacketService::SetDefaultContextParams(const TTsyReqHandle aTsyReqHandle, const TDesC8* aPckg)
/**
* Sets the Default Context Parameters to be used when attaching and activating a context.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aPckg pointer containing the parameters to set to.
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_SETDEFAULTCONTEXTPARAMS_1, "CSimPacketService::SetDefaultContextParams called");
	
	TPckg<TPacketDataConfigBase>* configBase = (TPckg<TPacketDataConfigBase>*)aPckg;
	TPacketDataConfigBase& configBaseV1 = (*configBase)();

		
	// GPRS
	if(configBaseV1.ExtensionId()==TPacketDataConfigBase::KConfigGPRS)
		{
		iPhone->UpdateNetworkMode(RMobilePhone::ENetworkModeGsm);
		TPckg<RPacketContext::TContextConfigGPRS>* defaultContextGPRSPckg = (TPckg<RPacketContext::TContextConfigGPRS>*)aPckg;
		RPacketContext::TContextConfigGPRS& defaultContextGPRSV1 = (*defaultContextGPRSPckg)();

		TBuf8<RPacketContext::KGSNNameLength> accessPointName;
		accessPointName.Copy(defaultContextGPRSV1.iAccessPointName);

		TBuf8<RPacketContext::KMaxPDPAddressLength> dpAddress;
		dpAddress.Copy(defaultContextGPRSV1.iPdpAddress);

		TBuf8<RPacketContext::KMaxAuthDataLength> password;
		password.Copy(defaultContextGPRSV1.iProtocolConfigOption.iAuthInfo.iPassword);

		TBuf8<RPacketContext::KMaxAuthDataLength> username;
		username.Copy(defaultContextGPRSV1.iProtocolConfigOption.iAuthInfo.iUsername);

		TBuf8<RPacketContext::KMaxAuthDataLength> challenge;
		challenge.Copy(defaultContextGPRSV1.iProtocolConfigOption.iChallenge);
		
		TBuf8<RPacketContext::KMaxAuthDataLength> response;
		response.Copy(defaultContextGPRSV1.iProtocolConfigOption.iResponse);

		TBuf8<RPacketContext::KMaxPDPAddressLength> primaryDNS;
		primaryDNS.Copy(defaultContextGPRSV1.iProtocolConfigOption.iDnsAddresses.iPrimaryDns);

		TBuf8<RPacketContext::KMaxAuthDataLength> secondaryDNS;
		secondaryDNS.Copy(defaultContextGPRSV1.iProtocolConfigOption.iDnsAddresses.iSecondaryDns);

		TBuf8<RPacketContext::KMiscProtocolBufferLength> miscBuffer;
		miscBuffer.Copy(defaultContextGPRSV1.iProtocolConfigOption.iMiscBuffer);

		if ((defaultContextGPRSV1.iPdpType != iGPRSDefaultConfig.iProtocolType)||
			(iGPRSDefaultConfig.iGsnAddress.Compare(accessPointName) != KErrNone) ||
			(iGPRSDefaultConfig.iProtocolAddress.Compare(dpAddress) != KErrNone) ||
			(defaultContextGPRSV1.iPdpCompression != iGPRSDefaultConfig.iPdpCompression) ||
			(defaultContextGPRSV1.iAnonymousAccessReqd != iGPRSDefaultConfig.iAnonymousAccess) ||
			(defaultContextGPRSV1.iUseEdge != iGPRSDefaultConfig.iUseEdge) ||
			(defaultContextGPRSV1.iProtocolConfigOption.iAuthInfo.iProtocol != iGPRSDefaultConfig.iProtocolConfigOption.iAuthProtocol) ||
			((iGPRSDefaultConfig.iProtocolConfigOption.iPassword).Compare(password) != KErrNone) ||
			((iGPRSDefaultConfig.iProtocolConfigOption.iUsername).Compare(username) != KErrNone) ||
			((iGPRSDefaultConfig.iProtocolConfigOption.iChallenge).Compare(challenge) != KErrNone) ||
			((iGPRSDefaultConfig.iProtocolConfigOption.iResponse).Compare(response) != KErrNone) ||
			((iGPRSDefaultConfig.iProtocolConfigOption.iPrimaryDNS).Compare(primaryDNS) != KErrNone) ||
			((iGPRSDefaultConfig.iProtocolConfigOption.iSecondaryDNS).Compare(secondaryDNS) != KErrNone) ||
			(iGPRSDefaultConfig.iProtocolConfigOption.iId != defaultContextGPRSV1.iProtocolConfigOption.iId))
				ReqCompleted(aTsyReqHandle,KErrArgument);
		else
				ReqCompleted(aTsyReqHandle,KErrNone);
		}
	// UMTS
	else if (configBaseV1.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4)
		{
		iPhone->UpdateNetworkMode(RMobilePhone::ENetworkModeGsm);
		TPckg<RPacketContext::TContextConfigR99_R4>* defaultContextR99_R4Pckg = (TPckg<RPacketContext::TContextConfigR99_R4>*)aPckg;
		RPacketContext::TContextConfigR99_R4& defaultContextR99_R4V1 = (*defaultContextR99_R4Pckg)();

		TBuf8<RPacketContext::KGSNNameLength> accessPointName;
		accessPointName.Copy(defaultContextR99_R4V1.iAccessPointName);

		TBuf8<RPacketContext::KMaxPDPAddressLength> dpAddress;
		dpAddress.Copy(defaultContextR99_R4V1.iPdpAddress);

		TBuf8<RPacketContext::KMaxAuthDataLength> password;
		password.Copy(defaultContextR99_R4V1.iProtocolConfigOption.iAuthInfo.iPassword);

		TBuf8<RPacketContext::KMaxAuthDataLength> username;
		username.Copy(defaultContextR99_R4V1.iProtocolConfigOption.iAuthInfo.iUsername);

		TBuf8<RPacketContext::KMaxAuthDataLength> challenge;
		challenge.Copy(defaultContextR99_R4V1.iProtocolConfigOption.iChallenge);
		
		TBuf8<RPacketContext::KMaxAuthDataLength> response;
		response.Copy(defaultContextR99_R4V1.iProtocolConfigOption.iResponse);

		TBuf8<RPacketContext::KMaxPDPAddressLength> primaryDNS;
		primaryDNS.Copy(defaultContextR99_R4V1.iProtocolConfigOption.iDnsAddresses.iPrimaryDns);

		TBuf8<RPacketContext::KMaxAuthDataLength> secondaryDNS;
		secondaryDNS.Copy(defaultContextR99_R4V1.iProtocolConfigOption.iDnsAddresses.iSecondaryDns);

		TBuf8<RPacketContext::KMiscProtocolBufferLength> miscBuffer;
		miscBuffer.Copy(defaultContextR99_R4V1.iProtocolConfigOption.iMiscBuffer);
		
		if ((defaultContextR99_R4V1.iPdpType != iR99_R4DefaultConfig.iProtocolType)||
				(iR99_R4DefaultConfig.iGsnAddress.Compare(accessPointName) != KErrNone) ||
				(iR99_R4DefaultConfig.iProtocolAddress.Compare(dpAddress) != KErrNone) ||
				(defaultContextR99_R4V1.iUseEdge != iR99_R4DefaultConfig.iUseEdge) ||
			   (defaultContextR99_R4V1.iProtocolConfigOption.iAuthInfo.iProtocol != iR99_R4DefaultConfig.iProtocolConfigOption.iAuthProtocol) ||
			   ((iR99_R4DefaultConfig.iProtocolConfigOption.iPassword).Compare(password) != KErrNone) ||
			   ((iR99_R4DefaultConfig.iProtocolConfigOption.iUsername).Compare(username) != KErrNone) ||
			   ((iR99_R4DefaultConfig.iProtocolConfigOption.iChallenge).Compare(challenge) != KErrNone) ||
			   ((iR99_R4DefaultConfig.iProtocolConfigOption.iResponse).Compare(response) != KErrNone) ||
			   ((iR99_R4DefaultConfig.iProtocolConfigOption.iPrimaryDNS).Compare(primaryDNS) != KErrNone) ||
			   ((iR99_R4DefaultConfig.iProtocolConfigOption.iSecondaryDNS.Compare(secondaryDNS) != KErrNone) ||
			   (iR99_R4DefaultConfig.iProtocolConfigOption.iId != defaultContextR99_R4V1.iProtocolConfigOption.iId)))

			ReqCompleted(aTsyReqHandle,KErrArgument);
		else
			ReqCompleted(aTsyReqHandle,KErrNone);
		}
	//R5
	else if(configBaseV1.ExtensionId()==TPacketDataConfigBase::KConfigRel5)
		{
		iPhone->UpdateNetworkMode(RMobilePhone::ENetworkModeGsm);
		TPckg<RPacketContext::TContextConfig_R5>* defaultContext_R5Pckg = (TPckg<RPacketContext::TContextConfig_R5>*)aPckg;
		RPacketContext::TContextConfig_R5& defaultContextR5 = (*defaultContext_R5Pckg)();

		TBuf8<RPacketContext::KGSNNameLength> accessPointName;
		accessPointName.Copy(defaultContextR5.iAccessPointName);

		TBuf8<RPacketContext::KMaxPDPAddressLength> dpAddress;
		dpAddress.Copy(defaultContextR5.iPdpAddress);

		TBuf8<RPacketContext::KMaxAuthDataLength> password;
		password.Copy(defaultContextR5.iProtocolConfigOption.iAuthInfo.iPassword);

		TBuf8<RPacketContext::KMaxAuthDataLength> username;
		username.Copy(defaultContextR5.iProtocolConfigOption.iAuthInfo.iUsername);

		TBuf8<RPacketContext::KMaxAuthDataLength> challenge;
		challenge.Copy(defaultContextR5.iProtocolConfigOption.iChallenge);
		
		TBuf8<RPacketContext::KMaxAuthDataLength> response;
		response.Copy(defaultContextR5.iProtocolConfigOption.iResponse);

		TBuf8<RPacketContext::KMaxPDPAddressLength> primaryDNS;
		primaryDNS.Copy(defaultContextR5.iProtocolConfigOption.iDnsAddresses.iPrimaryDns);

		TBuf8<RPacketContext::KMaxAuthDataLength> secondaryDNS;
		secondaryDNS.Copy(defaultContextR5.iProtocolConfigOption.iDnsAddresses.iSecondaryDns);

		TBuf8<RPacketContext::KMiscProtocolBufferLength> miscBuffer;
		miscBuffer.Copy(defaultContextR5.iProtocolConfigOption.iMiscBuffer);

		if ((defaultContextR5.iPdpType != iR5DefaultConfig.iProtocolType)||
			(iR5DefaultConfig.iGsnAddress.Compare(accessPointName) != KErrNone) ||
			(iR5DefaultConfig.iProtocolAddress.Compare(dpAddress) != KErrNone) ||
			(defaultContextR5.iUseEdge != iR5DefaultConfig.iUseEdge) ||
			(defaultContextR5.iProtocolConfigOption.iAuthInfo.iProtocol != iR5DefaultConfig.iProtocolConfigOption.iAuthProtocol) ||
			((iR5DefaultConfig.iProtocolConfigOption.iPassword).Compare(password) != KErrNone) ||
			((iR5DefaultConfig.iProtocolConfigOption.iUsername).Compare(username) != KErrNone) ||
			((iR5DefaultConfig.iProtocolConfigOption.iChallenge).Compare(challenge) != KErrNone) ||
			((iR5DefaultConfig.iProtocolConfigOption.iResponse).Compare(response) != KErrNone) ||
			((iR5DefaultConfig.iProtocolConfigOption.iPrimaryDNS).Compare(primaryDNS) != KErrNone) ||
			((iR5DefaultConfig.iProtocolConfigOption.iSecondaryDNS).Compare(secondaryDNS) != KErrNone) ||
			(iR5DefaultConfig.iProtocolConfigOption.iId != defaultContextR5.iProtocolConfigOption.iId) ||
			(iR5DefaultConfig.iPdpHeaderCompression != defaultContextR5.iPdpHeaderCompression) ||
			(iR5DefaultConfig.iPdpDataCompression != defaultContextR5.iPdpDataCompression))
			{
			ReqCompleted(aTsyReqHandle,KErrArgument);
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		}
	else 
		{
		iPhone->UpdateNetworkMode(RMobilePhone::ENetworkModeUnknown);
		}
	return KErrNone;
	}


TInt CSimPacketService::GetGPRSDefaultContextParams(const TTsyReqHandle aTsyReqHandle, TDes8* aPckg)
/**
* Returns the Default Context Parameters set by the client.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aPckg pointer containing the parameters to return to client.
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_GETGPRSDEFAULTCONTEXTPARAMS_1, "CSimPacketService::GetDefaultContextParams called");

	TPckg<RPacketContext::TContextConfigGPRS>* contextConfigGPRSPckg = (TPckg<RPacketContext::TContextConfigGPRS>*)aPckg;
	RPacketContext::TContextConfigGPRS& contextConfigV1 = (*contextConfigGPRSPckg)();

	contextConfigV1.iAccessPointName.Copy(KApn);
	contextConfigV1.iAnonymousAccessReqd = RPacketContext::ERequired;
	contextConfigV1.iPdpAddress.Copy(KProtocolAddress);
	contextConfigV1.iPdpCompression = 1;
	contextConfigV1.iPdpType =  RPacketContext::EPdpTypeIPv6;  

	contextConfigV1.iUseEdge = ETrue;
	contextConfigV1.iProtocolConfigOption.iAuthInfo.iUsername.Copy(KUsername);
	contextConfigV1.iProtocolConfigOption.iAuthInfo.iPassword.Copy(KPassword);
	contextConfigV1.iProtocolConfigOption.iChallenge.Copy(KChallenge);
	contextConfigV1.iProtocolConfigOption.iResponse.Copy(KResponse);
	contextConfigV1.iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Copy(KPrimaryDNS);
	contextConfigV1.iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Copy(KSecondaryDNS);
	contextConfigV1.iProtocolConfigOption.iId = 1;
	contextConfigV1.iProtocolConfigOption.iMiscBuffer.Copy(_L(""));
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}
	
TInt CSimPacketService::GetDefaultContextParams(const TTsyReqHandle aTsyReqHandle, TDes8* aPckg)
/**
* Returns the Default Context Parameters set by the client.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aPckg pointer containing the parameters to return to client.
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_GETDEFAULTCONTEXTPARAMS_1, "CSimPacketService::GetDefaultContextParams called");
	TPckg<TPacketDataConfigBase>* configBase = (TPckg<TPacketDataConfigBase>*)aPckg;
	TPacketDataConfigBase& configBaseV1 = (*configBase)();

	// GPRS
	if(configBaseV1.ExtensionId()==TPacketDataConfigBase::KConfigGPRS)
		{
		TPckg<RPacketContext::TContextConfigGPRS>* contextConfigGPRSPckg = (TPckg<RPacketContext::TContextConfigGPRS>*)aPckg;
		RPacketContext::TContextConfigGPRS& contextConfigV1 = (*contextConfigGPRSPckg)();

		contextConfigV1.iAccessPointName.Copy(iGPRSDefaultConfig.iGsnAddress);// = STATIC_CAST(RPacketContext::TGSNAddress,accessPointName);
		contextConfigV1.iAnonymousAccessReqd = STATIC_CAST(RPacketContext::TAnonymousAccess,iGPRSDefaultConfig.iAnonymousAccess);
		contextConfigV1.iPdpAddress.Copy(iGPRSDefaultConfig.iProtocolAddress);// = STATIC_CAST(RPacketContext::TProtocolAddress, dpAddress);
		contextConfigV1.iPdpCompression = STATIC_CAST(TUint, iGPRSDefaultConfig.iPdpCompression);
		contextConfigV1.iPdpType = STATIC_CAST(RPacketContext::TProtocolType, iGPRSDefaultConfig.iProtocolType);
		contextConfigV1.iUseEdge = iGPRSDefaultConfig.iUseEdge;
		contextConfigV1.iProtocolConfigOption.iAuthInfo.iProtocol = STATIC_CAST(RPacketContext::TAuthProtocol, iGPRSDefaultConfig.iProtocolConfigOption.iAuthProtocol);
		contextConfigV1.iProtocolConfigOption.iAuthInfo.iUsername.Copy(iGPRSDefaultConfig.iProtocolConfigOption.iUsername);
		contextConfigV1.iProtocolConfigOption.iAuthInfo.iPassword.Copy(iGPRSDefaultConfig.iProtocolConfigOption.iPassword);
		contextConfigV1.iProtocolConfigOption.iChallenge.Copy(iGPRSDefaultConfig.iProtocolConfigOption.iChallenge);
		contextConfigV1.iProtocolConfigOption.iResponse.Copy(iGPRSDefaultConfig.iProtocolConfigOption.iResponse);
		contextConfigV1.iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Copy(iGPRSDefaultConfig.iProtocolConfigOption.iPrimaryDNS);
		contextConfigV1.iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Copy(iGPRSDefaultConfig.iProtocolConfigOption.iSecondaryDNS);
		contextConfigV1.iProtocolConfigOption.iId = STATIC_CAST(TUint8, iGPRSDefaultConfig.iProtocolConfigOption.iId);
		contextConfigV1.iProtocolConfigOption.iMiscBuffer.Copy(iGPRSDefaultConfig.iProtocolConfigOption.iMiscBuffer);
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	// UMTS
	else if(configBaseV1.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4)
		{
		TPckg<RPacketContext::TContextConfigR99_R4>* contextConfigR99_R4Pckg = (TPckg<RPacketContext::TContextConfigR99_R4>*)aPckg;
		RPacketContext::TContextConfigR99_R4& contextConfigV1 = (*contextConfigR99_R4Pckg)();

		contextConfigV1.iAccessPointName.Copy(iR99_R4DefaultConfig.iGsnAddress);// = STATIC_CAST(RPacketContext::TGSNAddress,accessPointName);
		contextConfigV1.iPdpAddress.Copy(iR99_R4DefaultConfig.iProtocolAddress);// = STATIC_CAST(RPacketContext::TProtocolAddress, dpAddress);
		contextConfigV1.iPdpType = STATIC_CAST(RPacketContext::TProtocolType, iR99_R4DefaultConfig.iProtocolType);
		contextConfigV1.iUseEdge = iR99_R4DefaultConfig.iUseEdge;
		contextConfigV1.iProtocolConfigOption.iAuthInfo.iProtocol = STATIC_CAST(RPacketContext::TAuthProtocol, iR99_R4DefaultConfig.iProtocolConfigOption.iAuthProtocol);
		contextConfigV1.iProtocolConfigOption.iAuthInfo.iUsername.Copy(iR99_R4DefaultConfig.iProtocolConfigOption.iUsername);
		contextConfigV1.iProtocolConfigOption.iAuthInfo.iPassword.Copy(iR99_R4DefaultConfig.iProtocolConfigOption.iPassword);
		contextConfigV1.iProtocolConfigOption.iChallenge.Copy(iR99_R4DefaultConfig.iProtocolConfigOption.iChallenge);
		contextConfigV1.iProtocolConfigOption.iResponse.Copy(iR99_R4DefaultConfig.iProtocolConfigOption.iResponse);
		contextConfigV1.iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Copy(iR99_R4DefaultConfig.iProtocolConfigOption.iPrimaryDNS);
		contextConfigV1.iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Copy(iR99_R4DefaultConfig.iProtocolConfigOption.iSecondaryDNS);
		contextConfigV1.iProtocolConfigOption.iId = STATIC_CAST(TUint8, iR99_R4DefaultConfig.iProtocolConfigOption.iId);
		contextConfigV1.iProtocolConfigOption.iMiscBuffer.Copy(iR99_R4DefaultConfig.iProtocolConfigOption.iMiscBuffer);
		ReqCompleted(aTsyReqHandle,KErrNone);
		}

	//R5
	else if(configBaseV1.ExtensionId()==TPacketDataConfigBase::KConfigRel5)
		{
		TPckg<RPacketContext::TContextConfig_R5>* contextConfigR5Pckg = (TPckg<RPacketContext::TContextConfig_R5>*)aPckg;
		RPacketContext::TContextConfig_R5& contextConfigV1 = (*contextConfigR5Pckg)();

		contextConfigV1.iAccessPointName.Copy(iR5DefaultConfig.iGsnAddress);
		contextConfigV1.iPdpAddress.Copy(iR5DefaultConfig.iProtocolAddress);
		contextConfigV1.iPdpType = static_cast<RPacketContext::TProtocolType>(iR5DefaultConfig.iProtocolType);
		contextConfigV1.iUseEdge = iR5DefaultConfig.iUseEdge;
		contextConfigV1.iPdpHeaderCompression = 
			static_cast<RPacketContext::THeaderCompression>(iR5DefaultConfig.iPdpHeaderCompression);
		contextConfigV1.iPdpDataCompression = 
			static_cast<RPacketContext::TDataCompression>(iR5DefaultConfig.iPdpDataCompression);
		contextConfigV1.iProtocolConfigOption.iAuthInfo.iProtocol = 
			static_cast<RPacketContext::TAuthProtocol>(iR5DefaultConfig.iProtocolConfigOption.iAuthProtocol);
		contextConfigV1.iProtocolConfigOption.iAuthInfo.iUsername.Copy(
			iR5DefaultConfig.iProtocolConfigOption.iUsername);
		contextConfigV1.iProtocolConfigOption.iAuthInfo.iPassword.Copy(
			iR5DefaultConfig.iProtocolConfigOption.iPassword);
		contextConfigV1.iProtocolConfigOption.iChallenge.Copy(
			iR5DefaultConfig.iProtocolConfigOption.iChallenge);
		contextConfigV1.iProtocolConfigOption.iResponse.Copy(
			iR5DefaultConfig.iProtocolConfigOption.iResponse);
		contextConfigV1.iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Copy(
			iR5DefaultConfig.iProtocolConfigOption.iPrimaryDNS);
		contextConfigV1.iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Copy(
			iR5DefaultConfig.iProtocolConfigOption.iSecondaryDNS);
		contextConfigV1.iProtocolConfigOption.iId = 
			static_cast<TUint8>(iR5DefaultConfig.iProtocolConfigOption.iId);
		contextConfigV1.iProtocolConfigOption.iMiscBuffer.Copy(
			iR5DefaultConfig.iProtocolConfigOption.iMiscBuffer);

		ReqCompleted(aTsyReqHandle,KErrNone);
		}

	return KErrNone;
	}


void CSimPacketService::TimerCallBack(TInt aId)
/**
* Timer callback function.  When the timer goes off, it will call back into this
* function for further processing.
*
* @param aId an id identifying which timer callback is being called
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_TIMERCALLBACK_1, ">>CSimPacketService::TimerCallBack");

	if(aId == ETimerIdPcktServPacket)
		{
		TimerCallBackPcktRegStatus();
		}
	else if (aId== ETimerIdPcktServReleaseMode)
		{
		TimerCallBackReleaseMode();
		}
	else if (aId == ETimerIdPcktServNtwkStatusChange)
		{
		TimerCallBackNtwkRegStatus();
		}
	else if (aId == ETimerIdPcktServStatusSuspend || 
			 aId == ETimerIdPcktServStatusDetach  ||
			 aId == ETimerIdPcktServStatusUnattach)
		{
		TimerCallBackDeactivate();	
		}
	else if (aId == ETimerIdPcktServDynamicCaps)
		{
		TimerCallBackDynamicCaps();
		}		
	else if(aId == ETimerIdContextActivationRequestedChange)
		{


		iIncomingContextManager->NextIncomingEvent(iConfigData.iData);
			// Set up for the next incoming PDP if any.
			// Note at moment can only handle 1 GPRS incoming.
			// There is no support for 2ndary contexts, so can not have 2 incoming (of the same type).
			// And at the moment there is no support for any other type thanm GPRS.

		iIncomingContextManager->Cancel();	
			// No more timers.
								
		iIncomingContextManager->SetUseTimerOff();								
		ReqCompleted(iConfigData.iTsyReqHandle,KErrNone);
		iIncomingContextManager->SetUseTimerOn();								
						
		}
	else if(aId == ETimerIdMbmsPcktService)
		{
		if(iNotifyMbmsNetworkStatusChange.iNotifyPending)
			{
			iNotifyMbmsNetworkStatusChange.iNotifyPending = EFalse;
			*(static_cast<TMbmsNetworkServiceStatus*>(iNotifyMbmsNetworkStatusChange.iNotifyData))=
			iMbmsBroadcastParamsArray->At(iCurrentMbmsParamsIndex).iMbmsNetworkStatus;
			ReqCompleted(iNotifyMbmsNetworkStatusChange.iNotifyHandle,KErrNone);
			}

		if(iNotifyMbmsServAvailChange.iNotifyPending)
			{
			iNotifyMbmsServAvailChange.iNotifyPending = EFalse;
			ReqCompleted(iNotifyMbmsServAvailChange.iNotifyHandle,KErrNone);
			}

		if(iMbmsBroadcastParamsArray->Count() > iCurrentMbmsParamsIndex)
			{
			iMbmsPcktTimer->Start(iMbmsBroadcastParamsArray->At(iCurrentMbmsParamsIndex).iDuration,this,ETimerIdMbmsPcktService);
			}
		else if (iMbmsMonitorListParamsArray->Count() > iCurrentMbmsMonitorListIndex)
			{
			iMbmsPcktTimer->Start(iMbmsMonitorListParamsArray->At(iCurrentMbmsMonitorListIndex).iDuration,this,ETimerIdMbmsPcktService);
			}

		}
	else
		{
		TInt ret = ActionEvent(EPacketEventTimeOut);
		__ASSERT_ALWAYS(ret==KErrNone,SimPanic(ETimeOutEventActionFailed));	// Note: this is very crude error handling and should be replaced by something rather more elegant.
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_TIMERCALLBACK_2, "<<CSimPacketService::TimerCallBack");
	}

void CSimPacketService::TimerCallBackDeactivate()
{
	// deactivate all contexts in every nif
	TInt totalWrappers = iNifWrappers.Count();
	for(TInt j=0; j < totalWrappers; j++)
		{
		TInt totalContexts = iNifWrappers[j].NumberOfContexts();
		for(TInt x =0; x<totalContexts; x++)
			{
			iNifWrappers[j].GetContext(x)->AttemptDeactivateImmediately();
			}
		}
}

void CSimPacketService::TimerCallBackReleaseMode()
/**
* called when release mode timer callback occurs
*/
	{
	if(iPacketReleaseModeArray->Count()<=iPacketReleaseModeIndex)
		return;

	iCurrentPacketReleaseMode=iPacketReleaseModeArray->At(iPacketReleaseModeIndex).iReleaseMode;

	// if event is pending complete request
	if(iNotifyChangeOfReleaseMode.iNotifyPending)
	{
	iNotifyChangeOfReleaseMode.iNotifyPending=EFalse;
	*(RPacketService::TPacketReleaseMode*)iNotifyChangeOfReleaseMode.iNotifyData=(RPacketService::TPacketReleaseMode)iCurrentPacketReleaseMode;
	ReqCompleted(iNotifyChangeOfReleaseMode.iNotifyHandle,KErrNone);
	}

	iPacketReleaseModeIndex++;
	if (iPacketReleaseModeArray->Count() > (iPacketReleaseModeIndex))
		{	
		// start timer again
		iReleaseModeTimer->Start(iPacketReleaseModeArray->At(iPacketReleaseModeIndex).iDuration,this,ETimerIdPcktServReleaseMode);
		}
	}

void CSimPacketService::TimerCallBackPcktRegStatus()
/**
* Timer Call back for the Network Registration status.
*
*/
	{
	iState=(RPacketService::TStatus)iPcktRegStatusInfo->At(iPcktRegStatIndex).iStatus;

	if(iNotifyStatusChange.iNotifyPending)
		{
		iNotifyStatusChange.iNotifyPending=EFalse;
		*(RPacketService::TStatus*)iNotifyStatusChange.iNotifyData=(RPacketService::TStatus)iState;
		ReqCompleted(iNotifyStatusChange.iNotifyHandle,KErrNone);
		}
	
	// please note that this bool is in case the test code is setting 
	// iState to another value, in which case the request will not complete
	// unless this bool is set and therefore the request will complete
	// in ActionEvent() 
	if (iCurrentEvent != EPacketEventNone)
		iPcktTimerBool = ETrue;

	iPcktRegStatIndex++;
	if (iPcktRegStatusInfo->Count() > iPcktRegStatIndex)
		{
		iPcktRegTimer->Start(iPcktRegStatusInfo->At(iPcktRegStatIndex).iDuration,this,ETimerIdPcktServPacket);
		}
	}

void CSimPacketService::SimPSEvent(const CSimPubSub::TPubSubProperty aProperty, TInt aStatus)
	{

	
	if (aProperty == iNtwkRegStatusChangeProperty)
		{ // The call is for the network.
		TInt index = aStatus;
		TInt length = iNtwkRegStatusInfo->Count();
		if (index < length)
			{
			TNtwkRegStatus& ntwkRegStatus = iNtwkRegStatusInfo->At(index);
			CompleteNtwkRegStatus(ntwkRegStatus.iRegStatus);
			}
		}
	else if ( iIncomingContextManager->IsForcedIncoming( aProperty ) )
		{ //  Lets force the completion of the PDP, rather than waiting around 
		  // for a large time to finish.
		TInt index = aStatus;
		iIncomingContextManager->ForcedIncoming( index, iConfigData.iData );
		iIncomingContextManager->SetUseTimerOff();								
		ReqCompleted(iConfigData.iTsyReqHandle,KErrNone);
			// Force the change through as if the timer had timed out.
		iIncomingContextManager->SetUseTimerOn();								
			// May get more timeouts for other primary PDP's.
		}
	else
		{  // Error. Some sort of stange request. Hopefully impossible.
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_SIMPSEVENT_1, "CSimPacketService::SimPSEvent property mismatch.");
		SimPanic(EFailedToSetProperty);
		}
	
	}

void CSimPacketService::PubSubCallback(TInt aIndex)
	{

	if(aIndex ==0 )
		{
		ReqCompleted(iGetMbmsNetworkReqHandle,KErrNone);	
		}

	else if (aIndex ==1)
		{
		if(iNotifyMbmsServAvailChange.iNotifyPending)
			{
			iNotifyMbmsServAvailChange.iNotifyPending = EFalse;
			ReqCompleted(iNotifyMbmsServAvailChange.iNotifyHandle,KErrNone);
			}
		}

	else if(aIndex ==2)
		{
		if(iUpdateMbmsServiceReqHandle)
			{
			ReqCompleted(iUpdateMbmsServiceReqHandle,iUpdateMbmsServiceError);
			}
		}

	else if(aIndex ==3)
		{
		ReqCompleted(iEnumerateActiveListReqHandle,KErrNone);
		}

	else if(aIndex ==4)
		{
		ReqCompleted(iEnumerateMonitorListReqHandle,KErrNone);
		}

	else {}
}

void CSimPacketService::TimerCallBackNtwkRegStatus()
	{
	if(iNtwkRegStatusInfo->Count()<=iNtwkRegStatIndex)
		return;

	TNtwkRegStatus& ntwkRegStatus = iNtwkRegStatusInfo->At(iNtwkRegStatIndex);
	CompleteNtwkRegStatus(ntwkRegStatus.iRegStatus);

	iNtwkRegStatIndex++;
	if(iNtwkRegStatusInfo->Count()>iNtwkRegStatIndex)
		iNtwkRegTimer->Start(iNtwkRegStatusInfo->At(iNtwkRegStatIndex).iDuration,this,ETimerIdPcktServNtwkStatusChange);
	}
	
void CSimPacketService::CompleteNtwkRegStatus(RPacketService::TRegistrationStatus aRegistrationStatus)
	{
	iCurrentRegStatus = aRegistrationStatus;

	if(iNotifyChangeOfNtwk.iNotifyPending)
		{
		iNotifyChangeOfNtwk.iNotifyPending=EFalse;
		*(RPacketService::TRegistrationStatus*)iNotifyChangeOfNtwk.iNotifyData=(RPacketService::TRegistrationStatus)iCurrentRegStatus;
		ReqCompleted(iNotifyChangeOfNtwk.iNotifyHandle,KErrNone);
		}

	if (//iCurrentRegStatus == RPacketService::ENotRegisteredSearching || 
		iCurrentRegStatus == RPacketService::ERegistrationDenied ||
		iCurrentRegStatus == RPacketService::ENotRegisteredButAvailable ||
		iCurrentRegStatus == RPacketService::ENotRegisteredAndNotAvailable)
	{
		// deactivate all contexts in every nif
		TInt totalWrappers = iNifWrappers.Count();
		for(TInt j=0; j < totalWrappers; j++)
			{
			TInt totalContexts = iNifWrappers[j].NumberOfContexts();
			for(TInt x =0; x<totalContexts; x++)
				{
				iNifWrappers[j].GetContext(x)->AttemptDeactivateImmediately();
				}
			}
	}
}

TInt CSimPacketService::ChangeState(RPacketService::TStatus aNewState)
/**
* Attempt to change state.
* First validate that the requested state change is ok.  If it is then proceed to change
* the state and complete any pending state change notification.
*
* @param aState the new state to change to
* @return Error indication if change of state is successful or not
*/
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_CHANGESTATE_1, ">>CSimPacketService::ChangeState [newState=%d]", aNewState);
	if(iState == aNewState)
		return KErrNone;

	TInt ret=KErrGeneral;
	const TPacketStateChangeValidity* stateChangePnt=KPacketStateChangeValidity;
	while(stateChangePnt->iOldState!=KPacketStateTableTerminator)
		{
		if((stateChangePnt->iOldState==iState) && (stateChangePnt->iNewState==aNewState))
			{
			ret=stateChangePnt->iError;
			break;
			}
		stateChangePnt++;
		}

	if(ret!=KErrNone)
		return ret;

//Request permission from the phone to change the state of the packet connection
	ret = iPhone->ValidateChangeState(aNewState);
	if(ret!=KErrNone)
		return ret;

// Actually change the state.
	iState=aNewState;

// if suspended or unattached take appropriate action
	if (iState==RPacketService::EStatusSuspended || iState == RPacketService::EStatusUnattached)
		{
			// deactivate all contexts in every nif
			TInt totalWrappers = iNifWrappers.Count();
			for(TInt j=0; j < totalWrappers; j++)
				{
				TInt totalContexts = iNifWrappers[j].NumberOfContexts();
				for(TInt x =0; x<totalContexts; x++)
					{
					ret = iNifWrappers[j].GetContext(x)->AttemptDeactivateImmediately();
					if(ret!=KErrNone)
						{
						break;
						}
					}
				}
		}

// Check for a pending line state notification.
	if(iNotifyStatusChange.iNotifyPending)
		{
		iNotifyStatusChange.iNotifyPending=EFalse;
		*(RPacketService::TStatus*)iNotifyStatusChange.iNotifyData=iState;
		ReqCompleted(iNotifyStatusChange.iNotifyHandle,KErrNone);
		}

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_CHANGESTATE_2, "<<CSimPacketService::ChangeState");
	return KErrNone;
	}

TInt CSimPacketService::ActionEvent(TPacketEvent aEvent)
/**
* Entry point when an event has occured that may advance the state machine.
* The aEvent parameter describes the event.
*
* This function contains the main state machine for the packet service.  
*
* @param aEvent The Packet Service event to handle
* @return value represents the error state caused by the attempted state machine jump.
*/
	{
	TInt ret=KErrNone;

	switch(aEvent)
		{
	case EPacketEventAttach:
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_ACTIONEVENT_1, ">>CSimPacketService::ActionEvent = [EPacketEventAttach]");
		if(iState==RPacketService::EStatusUnattached)
			{
			iCurrentEvent=EPacketEventAttach;
			iTimer->Start(iAttachPause,this);
			}
		else
			ReqCompleted(iAttachRequestHandle, KErrNone);
		break;

	case EPacketEventDetach:
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_ACTIONEVENT_2, ">>CSimPacketService::ActionEvent = [EPacketEventDetach]");
		if(iState !=RPacketService::EStatusUnattached)
			{
			iCurrentEvent=EPacketEventDetach;
			iTimer->Start(iDetachPause,this);
			}
		else
			{
			ReqCompleted(iDetachRequestHandle, KErrNone);
			}
		break;

	case EPacketEventTimeOut:
		switch(iCurrentEvent)
			{
		case EPacketEventNone:
			break;
		case EPacketEventAttach:
			if(iAttachFail != KErrNone)
				{
				ReqCompleted(iAttachRequestHandle,iAttachFail);
				iCurrentEvent=EPacketEventNone;
				iAttachFail = KErrNone;
				break;
				}
			if(iState==RPacketService::EStatusUnattached)
				{
				ret=ChangeState(RPacketService::EStatusAttached);
				ReqCompleted(iAttachRequestHandle,ret);
				}
			else if (iPcktTimerBool)
				{	
				ReqCompleted(iAttachRequestHandle,KErrNone);
				iPcktTimerBool= EFalse;
				}
			iCurrentEvent=EPacketEventNone;
			break;
		case EPacketEventDetach:
			if(iDetachFail != KErrNone)
				{
				ReqCompleted(iDetachRequestHandle,iDetachFail);
				iCurrentEvent=EPacketEventNone;
				iDetachFail = KErrNone;
				break;
				}
			ret = DoDetach();
			if (iPcktTimerBool)
				{	
				ReqCompleted(iDetachRequestHandle,KErrNone);
				iPcktTimerBool= EFalse;
				}
			iCurrentEvent=EPacketEventNone;
			break;
		default:
			break;
			}
		break;

	default:
		break;
		}
	return ret;
	}

const CTestConfigSection* CSimPacketService::CfgFile()
/**
*	Return a pointer to the Configuration File Section
*
* @return CTestConfigSection	pointer to the configuration file section
*/
	{
	return iPhone->CfgFile();
	}

/**
* check that there is a primary context and that opening a secondary
* is a valid action
**/
TInt CSimPacketService::PrepareOpenSecondary(const TTsyReqHandle aTsyReqHandle, const TDesC* aOriginalContextName)
	{
	TInt i = 0;
	TInt err = KErrNone;

	for(i = 0; i<iNifWrappers.Count(); i++)
		{
		TNifWrapper& nifWrapper = iNifWrappers[i];
		for (TInt j = 0; j < nifWrapper.NumberOfContexts(); j++)
			{
			if (nifWrapper.GetContext(j)->ContextName().Compare(*aOriginalContextName) == KErrNone)
				{
				iSecondaryContext = ETrue;
				iCurrentPrimaryContextIndex = i;
				break;
				}
			}
		}
	
	if (iSecondaryContext == EFalse)
		err = KErrNotSupported;
	
	ReqCompleted(aTsyReqHandle, err);
	return err;
	}

TInt CSimPacketService::GetCurrentReleaseMode(const TTsyReqHandle aTsyReqHandle,RPacketService::TPacketReleaseMode* aReleaseMode)
// return current release mode
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_GETCURRENTRELEASEMODE_1, "CSimPacketService::GetCurrentReleaseMode called");
	*aReleaseMode = (RPacketService::TPacketReleaseMode)iCurrentPacketReleaseMode;
	ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}
	
TInt CSimPacketService::NotifyReleaseModeChange(const TTsyReqHandle aTsyReqHandle, RPacketService::TPacketReleaseMode* aReleaseMode)
	{
	// if release mode has changed
	if(iPacketReleaseModeArray->Count()==0)
		{
		ReqCompleted(aTsyReqHandle,KErrNotSupported);
		return KErrNotSupported;
		}

	__ASSERT_ALWAYS(!iNotifyChangeOfReleaseMode.iNotifyPending,SimPanic(ENotificationReqAlreadyOutstanding));

	iNotifyChangeOfReleaseMode.iNotifyPending=ETrue;
	iNotifyChangeOfReleaseMode.iNotifyHandle=aTsyReqHandle;
	iNotifyChangeOfReleaseMode.iNotifyData=&aReleaseMode;
	return KErrNone;
	}

TInt CSimPacketService::EnumerateNifs(const TTsyReqHandle aTsyReqHandle, TInt& aCount) 
	{
// return number of nifs
	aCount = iNifWrappers.Count();
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimPacketService::GetNifInfo(const TTsyReqHandle aTsyReqHandle, TInt aIndex, TDes8* aNifInfoV2) 
	{
// get info for nif
	TInt range = iNifWrappers.Count()-1;
	if (aIndex < 0 || aIndex > range)
		{
		ReqCompleted(aTsyReqHandle,KErrArgument);
		return KErrArgument;
		}

	RPacketService::TNifInfoV2Pckg* nifInfoV2Pckg = (RPacketService::TNifInfoV2Pckg*)aNifInfoV2;
	RPacketService::TNifInfoV2& nifInfoV2 = (*nifInfoV2Pckg)();

	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(nifInfoV2);
	if(err != KErrNone)
		{
		iPhone->ReqCompleted(aTsyReqHandle, err);
		return KErrNone;
		}

	nifInfoV2.iContextName.Copy(iNifWrappers[aIndex].GetContext(0)->ContextName());
	nifInfoV2.iContextType = STATIC_CAST(RPacketService::TContextType, iNifWrappers[aIndex].GetContext(0)->ContextType());

	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

/***
* This function takes the name passed in the parameter aExistingContextName and 
* returns the number of contexts sharing its Nif. 
***/
TInt CSimPacketService::EnumerateContextsInNif(const TTsyReqHandle aTsyReqHandle, const TDesC* aExistingContextName, TInt& aCount) 
	{
	// find contexts sharing name aExistingContextName, ie primary and secondary as well
	TInt number = KErrNotFound;
	TInt wrappersCount = iNifWrappers.Count();

	for(TInt i = 0; i<wrappersCount; i++)
		{
		TInt contextsTotal = iNifWrappers[i].NumberOfContexts();
		for(TInt j = 0; j<contextsTotal; j++)
			{
			if(iNifWrappers[i].GetContext(j)->ContextName().Compare(*aExistingContextName) == KErrNone)
				{
				number = iNifWrappers[i].NumberOfContexts();
				break;
				}
			}
		}

	// ie couldn't find context of aExistingContextName
	if (number == KErrNotFound)
		{
		ReqCompleted(aTsyReqHandle, KErrArgument);
		return KErrArgument;
		}

	// count primary context as well!
	aCount = number;
	ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}

TInt CSimPacketService::GetContextNameInNif(const TTsyReqHandle aTsyReqHandle, const TDesC* aExistingContextName, TInt aIndex, TDes* aContextName) 
	{
// get for context name specified, find nif's context name at index given
	// first, check aExistingContextName exists in contexts
	TInt number = KErrNotFound;
	TInt wrappersCount =  iNifWrappers.Count();

	for(TInt i = 0; i<wrappersCount; i++)
		{
		TInt contextsTotal = iNifWrappers[i].NumberOfContexts();
		for(TInt j = 0; j<contextsTotal; j++)
			{
			if (iNifWrappers[i].GetContext(j)->ContextName().Compare(*aExistingContextName) == KErrNone)
				{
				number = i;
				break;
				}
			}
		}

	// total no. of contexts with name is number of secondary contexts plus primary, so
	// number + 1
	// so range should be within number+1-1 ie number.
	if (number == KErrNotFound || aIndex < 0 || aIndex > number)
		{
		ReqCompleted(aTsyReqHandle, KErrArgument);
		return KErrArgument;
		}
	
	*aContextName = iNifWrappers[number].GetContext(aIndex)->ContextName();
	ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}

/**
* Cancel method, returns KErrNone
*/
TInt CSimPacketService::GetCurrentReleaseModeCancel(const TTsyReqHandle /*aTsyReqHandle*/)
	{
	return KErrNone;
	}

/**
* Cancel method, returns KErrNone
*/
TInt CSimPacketService::NotifyReleaseModeChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_NOTIFYRELEASEMODECHANGECANCEL_1, "CSimPacketService::NotifyChangeOfNtwkRegStatusCancel called");
	if(iNotifyChangeOfReleaseMode.iNotifyPending)
		{
		iNotifyChangeOfReleaseMode.iNotifyPending=EFalse;
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	return KErrNone;
	}

/**
* Cancel method, returns KErrNone
*/
TInt CSimPacketService::EnumerateNifsCancel(const TTsyReqHandle /*aTsyReqHandle*/)
	{
	return KErrNone;
	}

/**
* Cancel method, returns KErrNone
*/
TInt CSimPacketService::GetNifInfoCancel(const TTsyReqHandle /*aTsyReqHandle*/)
	{
	return KErrNone;
	}

/**
* Cancel method, returns KErrNone
*/
TInt CSimPacketService::EnumerateContextsInNifCancel(const TTsyReqHandle /*aTsyReqHandle*/)
	{
	return KErrNone;
	}

/**
* Cancel method, returns KErrNone
*/	
TInt CSimPacketService::GetContextNameInNifCancel(const TTsyReqHandle /*aTsyReqHandle*/)
	{
	return KErrNone;
	}
	
/**
* Cancel method, returns KErrNone
*/
TInt CSimPacketService::NotifyAttachModeChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_NOTIFYATTACHMODECHANGECANCEL_1, "CSimPacketService::NotifyAttachModeChangeCancel called");
	if(iNotifyChangeOfAttachMode.iNotifyPending)
		{
		iNotifyChangeOfAttachMode.iNotifyPending=EFalse;
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	return KErrNone;
	}

TInt CSimPacketService::GetMbmsNetworkServiceStatus(const TTsyReqHandle aTsyReqHandle, TBool aAttemptAttach,TMbmsNetworkServiceStatus* aStatus)
/**
* Retrieves and returns the status of the MBMS Packet service
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aStatus pointer to the status
* @return TInt err KErrNone if ok
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_GETMBMSNETWORKSERVICESTATUS_1, "CSimPacketService::GetMbmsNetworkServiceStatus called");
	*aStatus = (iMbmsBroadcastParamsArray->At(iCurrentMbmsParamsIndex).iMbmsNetworkStatus);
	if(aAttemptAttach)
		{
		if(*aStatus == EMbmsSupportUnknown)
			{
			*aStatus = EMbmsSupported;
			}
		}
	iGetMbmsNetworkReqHandle = aTsyReqHandle;
	iCurrentMbmsParamsIndex++;
	TInt totCount = iMbmsBroadcastParamsArray->Count();
	if(iCurrentMbmsParamsIndex == totCount)
		{
		iCurrentMbmsParamsIndex--;
		}
	return KErrNone;
	}

TInt CSimPacketService::GetMbmsNetworkServiceStatusCancel(const TTsyReqHandle aTsyReqHandle)
/* 
 * Cancels the GetMBMSNetworkServiceStatus request.
 * In fact this is not needed- just to avoid Panic if called, this implementation is done.
 */
	{
	if(iGetMbmsNetworkReqHandle == aTsyReqHandle)
		{
		ReqCompleted(iGetMbmsNetworkReqHandle,KErrCancel);
		}
	else
		{
		ReqCompleted(iGetMbmsNetworkReqHandle,KErrCorrupt);
		}
	return KErrNone;
	}

TInt CSimPacketService::NotifyMbmsNetworkServiceStatusChange(const TTsyReqHandle aTsyReqHandle, TMbmsNetworkServiceStatus* aStatus)
/**
* Record a client's interest in being notified of the state of the MBMS packet network and contexts.
* First check that there isn't already a notification pending (the ETel Server should protect
* against this) and then record the information necessary to complete the request later, when
* the status does actually change.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aStatus pointer to the mbms packet service status
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_NOTIFYMBMSNETWORKSERVICESTATUSCHANGE_1, "CSimPacketService::NotifyMbmsNetworkServiceStatusChange called");
 	__ASSERT_ALWAYS(!iNotifyMbmsNetworkStatusChange.iNotifyPending,SimPanic(ENotificationAlreadyPending));

 	iNotifyMbmsNetworkStatusChange.iNotifyPending = ETrue;
	iNotifyMbmsNetworkStatusChange.iNotifyHandle = aTsyReqHandle;
 	iNotifyMbmsNetworkStatusChange.iNotifyData = aStatus;
	return KErrNone;
	}

TInt CSimPacketService::NotifyMbmsNetworkServiceStatusChangeCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Cancel a client's interest in being notified when the mbms network status changes.
* This is acheived simply by resetting the flag that indicates a notification is pending.
* 
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_NOTIFYMBMSNETWORKSERVICESTATUSCHANGECANCEL_1, "CSimPacketService::NotifyMbmsNetworkServiceStatusChangeCancel called");
	if(iNotifyMbmsNetworkStatusChange.iNotifyPending)
		{
		iNotifyMbmsNetworkStatusChange.iNotifyPending=EFalse;
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	return KErrNone;
	}

TInt CSimPacketService::NotifyMbmsServiceAvailabilityChange(const TTsyReqHandle aTsyReqHandle)
/**
* Record a client's interest in being notified of the service availability of the MBMS packet network.
* First check that there isn't already a notification pending (the ETel Server should protect
* against this) and then record the information necessary to complete the request later, when
* the status does actually change.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_NOTIFYMBMSSERVICEAVAILABILITYCHANGE_1, "CSimPacketService::NotifyMbmsServiceAvailabilityChange called");
 	__ASSERT_ALWAYS(!iNotifyMbmsNetworkStatusChange.iNotifyPending,SimPanic(ENotificationAlreadyPending));

 	iNotifyMbmsServAvailChange.iNotifyPending = ETrue;
 	iNotifyMbmsServAvailChange.iNotifyHandle = aTsyReqHandle;
 	(void)iNotifyMbmsServAvailChange.iNotifyData;
	return KErrNone;
	}

TInt CSimPacketService::NotifyMbmsServiceAvailabilityChangeCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Cancel a client's interest in being notified when the mbms service availability changes.
* This is acheived simply by resetting the flag that indicates a notification is pending.
* 
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_NOTIFYMBMSSERVICEAVAILABILITYCHANGECANCEL_1, "CSimPacketService::NotifyMbmsServiceAvailabilityChangeCancel called");
	if(iNotifyMbmsServAvailChange.iNotifyPending)
		{
		iNotifyMbmsServAvailChange.iNotifyPending=EFalse;
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	return KErrNone;
	}

TInt CSimPacketService::UpdateMbmsMonitorServiceListL(const TTsyReqHandle aTsyReqHandle,TMbmsAction* aAction, TDes8* aBuffer)
/**
* client's interest in updating the MBMS Service Monitoring list.
* The update action is also passed as the MBMS Action parameter.
* This will update the MBMS Service Monitoring List in TSY.
* 
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @param aAction TMbmsAction to be performed
* @param aBuffer list of TMbmsServiceAvailabilityV1 class passed.
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_UPDATEMBMSMONITORSERVICELISTL_1, "CSimPacketService::UpdateMbmsMonitorServiceListL called");
	CPcktMbmsMonitoredServiceList* monServList = CPcktMbmsMonitoredServiceList::NewL();
	CleanupStack::PushL(monServList); 

	TInt error=KErrNone;
	TInt i=0;
	TInt j=0;
	TInt ii=0;
	TInt countAdd=0;
	TInt countRem=0;
	RPacketService::TMbmsServiceAvailabilityV1 entry;
	
	switch(*aAction)
		{
		case SIMTSY_PACKET_MBMS_ADD_ENTRIES:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_UPDATEMBMSMONITORSERVICELISTL_2, "CSimPacketService::UpdateMbmsMonitorServiceListL Action: ADD ");
			monServList->RestoreL(*aBuffer);
			countAdd = monServList->Enumerate();
			if(countAdd != SIMTSY_PACKET_MBMS_ADD_ENTRIES_COUNT)
				{
				ReqCompleted(aTsyReqHandle,KErrCorrupt);
				CleanupStack::PopAndDestroy(); // monServList
				return KErrNone;
				}
			for(;i<SIMTSY_PACKET_MBMS_ADD_ENTRIES_COUNT;i++)
				{
				TRAPD(ret,entry=monServList->GetEntryL(i));
				if (ret != KErrNone)
					{
					break;
					}
				TBool entryFound=EFalse;
				
				for(ii=0;ii<iMbmsMonitorListParamsArray->Count();ii++)
					{
					 if((entry.iMbmsServiceMode == iMbmsMonitorListParamsArray->At(ii).iServiceMode) &&
						(entry.iMbmsAccessBearer == iMbmsMonitorListParamsArray->At(ii).iAccessBearer) &&
						(entry.iTmgi.GetMCC() == iMbmsMonitorListParamsArray->At(ii).iMCC) &&
						(entry.iTmgi.GetMNC() == iMbmsMonitorListParamsArray->At(ii).iMNC) &&
						(entry.iTmgi.GetServiceId() == iMbmsMonitorListParamsArray->At(ii).iServiceId))		
							{
							entryFound =  ETrue;
							break;
							}
					}
					
				if(!entryFound)
					{
					error = KErrMbmsImpreciseServiceEntries;
					}
					
				else
					{
					entry.iMbmsAvailabilityStatus = iMbmsMonitorListParamsArray->At(ii).iAvailStatus ;
					}
				 	
				}
			iMbmsMonitorListCount += i;
			if(iMbmsMonitorListCount > KMaxMbmsMonitorContexts)
				{
				error =KErrNoMemory;
				iMbmsMonitorListCount -= i;
				}
			
			ReqCompleted(aTsyReqHandle, error);
			CleanupStack::PopAndDestroy(); // monServList
			break;

		case SIMTSY_PACKET_MBMS_REM_ENTRIES:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_UPDATEMBMSMONITORSERVICELISTL_3, "CSimPacketService::UpdateMbmsMonitorServiceListL Action: REMOVE ");
			monServList->RestoreL(*aBuffer);
			countRem = monServList->Enumerate();
			if(countRem != SIMTSY_PACKET_MBMS_REM_ENTRIES_COUNT)
				{
				ReqCompleted(aTsyReqHandle,KErrCorrupt);
				CleanupStack::PopAndDestroy(); // monServList
				return KErrNone;
				}
	
			for(;i<SIMTSY_PACKET_MBMS_REM_ENTRIES_COUNT;i++)
				{
				TRAPD(ret,entry=monServList->GetEntryL(i));
				
				if (ret != KErrNone)
					{
					break;
					}
					
				TBool entryFound=EFalse;
				
				for(ii=0;ii<iMbmsMonitorListParamsArray->Count();ii++)
					{
					 if((entry.iMbmsServiceMode == iMbmsMonitorListParamsArray->At(ii).iServiceMode) &&
						(entry.iMbmsAccessBearer == iMbmsMonitorListParamsArray->At(ii).iAccessBearer) &&
						(entry.iTmgi.GetMCC() == iMbmsMonitorListParamsArray->At(ii).iMCC) &&
						(entry.iTmgi.GetMNC() == iMbmsMonitorListParamsArray->At(ii).iMNC) &&
						(entry.iTmgi.GetServiceId() == iMbmsMonitorListParamsArray->At(ii).iServiceId))		
					 		{
					 		entryFound =  ETrue;
					 		break;
					 		}
					}
					
				if(!entryFound)
					{
					j++;
					error = KErrMbmsImpreciseServiceEntries;
					}
					
				else
					{
					entry.iMbmsAvailabilityStatus = iMbmsMonitorListParamsArray->At(ii).iAvailStatus ;
					iMbmsMonitorListParamsArray->Delete(ii);
					}
				 	
				}
				if(j == SIMTSY_PACKET_MBMS_REM_ENTRIES_COUNT)
					{
					error = KErrNotFound;					
					}
					
			if(iMbmsMonitorListCount >=j)
				{
				iMbmsMonitorListCount -= j;
				}
				
			iUpdateMbmsServiceError = error;
			iUpdateMbmsServiceReqHandle = aTsyReqHandle;
			
			CleanupStack::PopAndDestroy(); // monServList
			break;
			
		case SIMTSY_PACKET_MBMS_REM_ALL_ENTRIES:
			iMbmsMonitorListCount = 0;
			iMbmsMonitorListParamsArray->Delete(0,iMbmsMonitorListParamsArray->Count());
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_UPDATEMBMSMONITORSERVICELISTL_4, "CSimPacketService::UpdateMbmsMonitorServiceListL Action: REMOVE_ALL ");
			iUpdateMbmsServiceError = KErrNone;
			iUpdateMbmsServiceReqHandle = aTsyReqHandle;
			CleanupStack::PopAndDestroy(); // monServList
			break;
			
		default:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_UPDATEMBMSMONITORSERVICELISTL_5, "CSimPacketService::UpdateMbmsMonitorServiceListL Action: Default ");
			ReqCompleted(aTsyReqHandle,KErrNotFound);
			CleanupStack::PopAndDestroy(); // monServList
			break;
		}
	return KErrNone;
	}

TInt CSimPacketService::UpdateMbmsMonitorServiceListCancel(const TTsyReqHandle aTsyReqHandle)
/**
* client's interest in cancelling the updation MBMS Service Monitoring list.
* 
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_UPDATEMBMSMONITORSERVICELISTCANCEL_1, "CSimPacketService::UpdateMbmsMonitorServiceListCancel Called ");
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

// CHANGES FOR 2 Phase retrieval of MBMS Monitored Services list starts here.

TInt CSimPacketService::GetMbmsMonitoredServicesListPhase1(const TTsyReqHandle aTsyReqHandle, TClientId* aClient, TInt* aBufSize)
/**
* client's interest in retrieving the MBMS Service Monitoring list.
* This function is the call for the first phase of 2 phase retrieval.
* 
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @param aAction TClientId to be used
* @param aBufSize Size to be allocated for retrieving the actual list in second phase.
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_GETMBMSMONITOREDSERVICESLISTPHASE1_1, "CSimPacketService::GetMbmsMonitoredServicesListPhase1 called");
	TInt ret=KErrNone;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, ret=ProcessGetMbmsMonitoredServicesListPhase1L(aTsyReqHandle, aClient, aBufSize););
	if (leaveCode != KErrNone)
		{
		ReqCompleted(aTsyReqHandle,leaveCode);
		}
	return ret;
	}	
	
TInt CSimPacketService::ProcessGetMbmsMonitoredServicesListPhase1L(const TTsyReqHandle aTsyReqHandle, 
													 TClientId* aClient, 
													 TInt* aBufSize)
/**
* Local function to perform the first phse of the 2 phase retrieval.
* 
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @param aAction TClientId to be used
* @param aBufSize Size to be allocated for retrieving the actual list in second phase.
* @return KErrNone
*/
	{
	
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_PROCESSGETMBMSMONITOREDSERVICESLISTPHASE1L_1, "CSimPacketService::ProcessGetMbmsMonitoredServicesListPhase1L called");
	CPcktMbmsMonitoredServiceList* list=CPcktMbmsMonitoredServiceList::NewL();
	CleanupStack::PushL(list);

	RPacketService::TMbmsServiceAvailabilityV1 entry;
	
	for (TInt i=0; i <iMbmsMonitorListParamsArray->Count(); i++)
		{
			entry.iMbmsServiceMode = iMbmsMonitorListParamsArray->At(i).iServiceMode;
			entry.iMbmsAccessBearer = iMbmsMonitorListParamsArray->At(i).iAccessBearer;
			entry.iMbmsAvailabilityStatus = iMbmsMonitorListParamsArray->At(i).iAvailStatus;
			entry.iTmgi.SetMCC(iMbmsMonitorListParamsArray->At(i).iMCC);
			entry.iTmgi.SetMNC(iMbmsMonitorListParamsArray->At(i).iMNC);
			entry.iTmgi.SetServiceId(iMbmsMonitorListParamsArray->At(i).iServiceId);
			// Add the entry into the list, at the next empty location
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_PROCESSGETMBMSMONITOREDSERVICESLISTPHASE1L_2, "Adding the Entry no. %d to the List",i+1);
			list->AddEntryL(entry);
		}
	
	// Store the  streamed list and the client ID
	CPcktListReadAllAttempt* read=CPcktListReadAllAttempt::NewL(*aClient,aTsyReqHandle);
	CleanupStack::PushL(read);
	
	read->iListBuf = list->StoreLC();
	CleanupStack::Pop(); // pop the CBufBase allocated by StoreLC
	
	iMbmsServicesList->AppendL(read);
	CleanupStack::Pop(); // pop the CPcktListReadAllAttempt
	
	// return the CBufBase size to client
	*aBufSize=(read->iListBuf)->Size();
	
	// Complete first phase of list retrieval
	ReqCompleted(aTsyReqHandle,KErrNone);
	CleanupStack::PopAndDestroy(); // pop&destroy list
	
	return KErrNone;
	}

TInt CSimPacketService::GetMbmsMonitoredServicesListPhase2(const TTsyReqHandle aTsyReqHandle,TClientId* aClient, TDes8* aBuf)
/**
* client's interest in retrieving the MBMS Service Monitoring list.
* This function is the call for the second phase of 2 phase retrieval.
* 
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @param aAction TClientId to be used
* @param aBuf buffer for the actual list to be populated.
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_GETMBMSMONITOREDSERVICESLISTPHASE2_1, "CSimPacketService::GetMbmsMonitoredServicesListPhase2 called");
	CPcktListReadAllAttempt* read=NULL;
	// Find the get MBMS Monitor Service List from this client
	for (TInt i=0; i<iMbmsServicesList->Count(); ++i)
		{
		read = iMbmsServicesList->At(i);
		if ((read->iClient.iSessionHandle==aClient->iSessionHandle) &&
		    (read->iClient.iSubSessionHandle==aClient->iSubSessionHandle))
			{
			TPtr8 bufPtr((read->iListBuf)->Ptr(0));
			// Copy the streamed list to the client
			aBuf->Copy(bufPtr);
			delete read;
			
			iMbmsServicesList->Delete(i);
			ReqCompleted(aTsyReqHandle,KErrNone);
			return KErrNone;
			}
		}
	// Should handle error case of not finding the matching client from read all phase 1
	return KErrNotFound;
	}
	
TInt CSimPacketService::GetMbmsMonitoredServicesListCancel(const TTsyReqHandle aTsyReqHandle)
/**
* client's interest in canceling the retrieval of the MBMS Service Monitoring list.
* 
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_GETMBMSMONITOREDSERVICESLISTCANCEL_1, "CSimPacketService::GetMbmsMonitoredServicesListCancel called");
	// Remove the read all attempt from iGetMbmsMonitoredServices

	CPcktListReadAllAttempt* read=NULL;
	for (TInt i=0; i<iMbmsServicesList->Count(); ++i)
		{
		read = iMbmsServicesList->At(i);
		if (read->iReqHandle == aTsyReqHandle)
			{
			delete read;
			iMbmsServicesList->Delete(i);
			break;
			}
		}
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CSimPacketService::EnumerateMbmsMonitorServiceList(const TTsyReqHandle aTsyReqHandle, TInt* aCount, TInt* aMaxAllowed)
/**
* client's interest in retrieving the information regarding the MBMS Monitoring Service list.
* 
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @param aCount count of the MBMS monitoring services in the list
* @param aMaxAllowed maximum MBMS monitoring services allowed
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_ENUMERATEMBMSMONITORSERVICELIST_1, "CSimPacketService::EnumerateMbmsMonitorServiceList called");
	*aCount = iMbmsMonitorListCount;
	*aMaxAllowed = KMaxMbmsMonitorContexts;
	iEnumerateMonitorListReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSimPacketService::EnumerateMbmsMonitorServiceListCancel(const TTsyReqHandle aTsyReqHandle)
/**
* client's interest in canceling the enumeration the MBMS Service Monitoring list.
* This function is just a dummy function.
* 
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	if(iEnumerateMonitorListReqHandle == aTsyReqHandle)
		{
		ReqCompleted(iEnumerateMonitorListReqHandle,KErrCancel);
		}
	else
		{
		ReqCompleted(iEnumerateMonitorListReqHandle,KErrCorrupt);
		}
	return KErrNone;
	}
	
TInt CSimPacketService::EnumerateMbmsActiveServiceList(const TTsyReqHandle aTsyReqHandle, TInt* aCount, TInt* aMaxAllowed)
/**
* client's interest in retrieving the information regarding the MBMS Active Service list.
* 
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @param aCount count of the MBMS active services in the list
* @param aMaxAllowed maximum MBMS active services allowed
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETSERVICE_ENUMERATEMBMSACTIVESERVICELIST_1, "CSimPacketService::EnumerateMbmsActiveServiceList called");
	*aCount = iMbmsNifWrappers.Count(); // The NifWrapper contains the list of mbmsContexts that are activated.
	*aMaxAllowed = KMaxMbmsActiveContexts;
	iEnumerateActiveListReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSimPacketService::EnumerateMbmsActiveServiceListCancel(const TTsyReqHandle aTsyReqHandle)
/**
* client's interest in canceling the enumeration the MBMS Service active list.
* This function is just a dummy function.
* 
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	if(iEnumerateActiveListReqHandle == aTsyReqHandle)
		{
		ReqCompleted(iEnumerateActiveListReqHandle,KErrCancel);
		}
	else
		{
		ReqCompleted(iEnumerateActiveListReqHandle,KErrCorrupt);
		}
	return KErrNone;
	}	
		
/**
* A CSimPacketContext object has been destroyed - this method will be called in its 
* destructor
*/
void CSimPacketService::DecrementContextCount()
	{
	iContextCount--;
	}

/**
* A CSimPacketContext object has been destroyed - this method will be called in its 
* destructor
*/
void CSimPacketService::DecrementMbmsContextCount()
	{
	iMbmsContextCount--;
	}


