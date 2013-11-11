// Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the implementation of the Similator TSY Packet Context functionality.  
// 
//

/**
 @file
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSimPacketContextTraces.h"
#endif

#include "CSimPhone.h"
#include "CSimPacketContext.h"
#include "CSimPacketService.h"
#include "CSimPacketQoS.h"
#include "utils.h"

#include "CSimContextHelper.h"

const TInt KUnsetInteger = 0;

CPcktListReadAllAttempt* CPcktListReadAllAttempt::NewL(TClientId& aId, TTsyReqHandle aReqHandle)
/**
 * Standard two phase constructor.
 * @param aId The client ID (session and sub-session handles) that uniquely identifies a client performing a two phase read.
 * @param aReqHandle The TSY request handle of the client request associated with a two phase read.
 * @return CPcktListReadAllAttempt* The newly created object.
 */
	{
	CPcktListReadAllAttempt* read=new(ELeave) CPcktListReadAllAttempt(aId, aReqHandle);
	CleanupStack::PushL(read);
	read->ConstructL();
	CleanupStack::Pop();
	return read;
	}

CPcktListReadAllAttempt::CPcktListReadAllAttempt(TClientId& aId, TTsyReqHandle aReqHandle)
		: iReqHandle(aReqHandle)
/**
 * Trivial first phase constructor.
 * @param aId The client ID (session and sub-session handles) that uniquely identifies a client performing a two phase read.
 * @param aReqHandle The TSY request handle of the client request associated with a two phase read.
 */
	{
	iClient.iSessionHandle=aId.iSessionHandle;
	iClient.iSubSessionHandle=aId.iSubSessionHandle;
	}

void CPcktListReadAllAttempt::ConstructL()
/**
 * Trivial second phase constructor.
 */
	{
	User::LeaveIfError(iListBuf!=NULL);
	}

CPcktListReadAllAttempt::~CPcktListReadAllAttempt()
/**
 * Trivial destructor.
 */
	{
	delete iListBuf;
	}


CSimPacketContext* CSimPacketContext::NewL(CSimPhone* aPhone, CSimPacketService* aPacketService, const TDesC& aContextName)
/**
* Standard two phase constructor
*
* @param aPacketService Pointer to the Packet Service object (CSimPacketService)
* @param aContextName name  for this packet context
* @return CSimPacketContext pointer to the packet context object created.
* @leave Leaves if no memory or object is not created for any reason.
*/
	{
	CSimPacketContext* packetContext = new(ELeave) CSimPacketContext(aPhone, aPacketService, aContextName);
	CleanupStack::PushL(packetContext);
	packetContext->ConstructL();
	CleanupStack::Pop();
	return packetContext;
	}

void CSimPacketContext::Init()
	{}

CSimPacketContext::CSimPacketContext(CSimPhone* aPhone, CSimPacketService* aPacketService, const TName& aContextName)
	: iPhone(aPhone), iDeleted(EFalse),iIsActive(EFalse),iPacketService(aPacketService),iContextName(aContextName),
	  iTFTCreated(EFalse), iNumTFTsCreated(-1), iNumFiltersAdded(0),
      iContextConfigParamsIndex(0), iCommPortLoaned(EFalse),iQoSObjectCount(0),
      iNotifyContextConfigChangeArray(NULL), iSetConfigData(NULL), iSetConfigCallCount(0), iTFTChangeBool(0),
      iNotifyContextStatusChangeIndex(0),iState(RPacketContext::EStatusInactive), iLastError(KErrNone), iErrorCodeForGetLastErrorCause(KErrNone)
	  
/**
* Trivial Constructor.  Initialises all the data members
*
* @param aPacketService Pointer to the Packet Service object (CSimPacketService)
* @param aContextName name  for this packet context
*/
	{
	iNotifyStatusChange.iNotifyPending = EFalse;
	iNotifyConfigGPRS.iNotifyPending= EFalse;
	iNotifyConfigR99.iNotifyPending= EFalse;
	iNotifyConfigR5.iNotifyPending= EFalse;
	}

void CSimPacketContext::ConstructL()
/**
* Second phase of the 2-phase constructor.
* Constructs all the member data and retrieves all the data from the config file specific to this class.
*
* @leave Leaves no memory or any data member does not construct for any reason.
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_CONSTRUCTL_1, "CSimPacketContext: Entered constructor");

	iSetConfigData = new (ELeave) CArrayFixFlat<TSetConfigData>(1);
	iNotifyContextConfigChangeArray = new (ELeave) CArrayFixFlat<TNotifyContextConfigChangeData>(1);
	iContextConfigParams = new (ELeave) CArrayFixFlat<TContextConfigParam>(KNumberofConfigs);
	iAuthorizationTokenResponse = new (ELeave) CArrayFixFlat<TAuthorizationTokenResponse>(KNumberofConfigs);
	iMediaAuthorization = new (ELeave) CArrayFixFlat<RPacketContext::CTFTMediaAuthorizationV3>(KNumberofConfigs);

	iNtRas = CSimNtRas::NewL(iPhone);
	iTimer = CSimTimer::NewL(iPhone);
	iNotifyContextStatusChangeTimer = CSimTimer::NewL(iPhone);
	iSetConfigTimer = CSimTimer::NewL(iPhone);
	iInitContextTimer = CSimTimer::NewL(iPhone);
	iCreateTftTimer = CSimTimer::NewL(iPhone);
	iNotifyContextConfigChangeTimer= CSimTimer::NewL(iPhone);
	iContextStatusChange = CSimPubSubChange::NewL(this, CSimPubSub::TPubSubProperty(KUidPSSimTsyCategory, KPSSimtsyPacketContextStatusChange, KPSSimtsyPacketContextStatusChangeKeyType));

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_CONSTRUCTL_2, "Starting to Load and Parse Packet Context the Config File");

	GetCommSetupSettingsL();
	GetContextConfigParamSettingsL(KSetContextConfigGPRS());
	GetContextConfigParamSettingsL(KSetContextConfigRel99());
	GetContextConfigParamSettingsL(KSetContextConfigRel5());
	GetActivateSettings();
	GetSetConfigSettings();
	GetPacketFilterSettings();
	GetContextConfigRel99SettingsL();
	ConstructPacketFiltersL();
	GetContextStatusChangeSettingsL();
	GetContextConfigChangeSettingsL();
	
	TInt nestedItemNumber; // used for nested loop
	const CTestConfigItem* item2; // used for nested items

	TUint count = CfgFile()->ItemCount(KAuthorizationToken);		

	TUint itemPosition = 0;
	TUint itemOffSet = 0;
	for(TInt i=0;i<count;i++)							// set up AuthorizationToken info
		{
		const CTestConfigItem *item = CfgFile()->Item(KAuthorizationToken,i);
		if(!item)
			break;

		RPacketContext::CTFTMediaAuthorizationV3* mediaAuthorization = RPacketContext::CTFTMediaAuthorizationV3::NewL();
        CleanupStack::PushL(mediaAuthorization);		
		TAuthorizationTokenResponse authorizationTokenResponse;
		TPtrC8 authorizationToken;
		TInt desiredResponse;
		TInt numberOfFlowIds;
		
		TInt ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,authorizationToken);

		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_CONSTRUCTL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element CTFTMEDIAAUTHORIZATIONV3::AUTHORIZATIONTOKEN returned %d (element no. %d) from tag %s.",ret,0,KAuthorizationToken);
            CleanupStack::PopAndDestroy(mediaAuthorization);			
			continue;
			}
		else
			{
			mediaAuthorization->iAuthorizationToken.Copy(authorizationToken); // Set up for media authorization
			authorizationTokenResponse.iAuthorizationToken.Copy(authorizationToken); // Set up for token response
			}
			
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,desiredResponse);

		if(ret!=KErrNone || desiredResponse > 0)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_CONSTRUCTL_4, "WARNING - CONFIGURATION FILE PARSING - Reading element DESIREDRESPONSE returned %d (element no. %d) from tag %s.",ret,1,KAuthorizationToken);
	        CleanupStack::PopAndDestroy(mediaAuthorization);
			continue;
			}
		else
			{
			authorizationTokenResponse.iResponse = desiredResponse;
			}

		iAuthorizationTokenResponse->AppendL(authorizationTokenResponse);

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,numberOfFlowIds);
			
		for(nestedItemNumber=itemPosition+itemOffSet;nestedItemNumber<itemOffSet+numberOfFlowIds;nestedItemNumber++)
			{
			item2 = CfgFile()->Item(KFlowIdentifier,nestedItemNumber);
			
			if(!item2)
				break;

			RPacketContext::CTFTMediaAuthorizationV3::TFlowIdentifier iFlowIdentifier;
			TInt mediaComponentNumber;
			TInt IPFlowNumber;
		
			ret=CTestConfig::GetElement(item2->Value(),KStdDelimiter,0,mediaComponentNumber);

			if(ret!=KErrNone || mediaComponentNumber < 0)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_CONSTRUCTL_5, "WARNING - CONFIGURATION FILE PARSING - Reading element CTFTMEDIAAUTHORIZATIONV3::MEDIACOMPONENTNUMBER returned %d (element no. %d) from tag %s.",ret,0,KFlowIdentifier);
				continue;
				}
			else
				{
				iFlowIdentifier.iMediaComponentNumber = mediaComponentNumber;
				}
				
			ret=CTestConfig::GetElement(item2->Value(),KStdDelimiter,1,IPFlowNumber);

			if(ret!=KErrNone || IPFlowNumber < 0)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_CONSTRUCTL_6, "WARNING - CONFIGURATION FILE PARSING - Reading element CTFTMEDIAAUTHORIZATIONV3::IPFLOWNUMBER returned %d (element no. %d) from tag %s.",ret,1,KFlowIdentifier);
				continue;
				}
			else
				{
				iFlowIdentifier.iIPFlowNumber = IPFlowNumber;
				}
			
			mediaAuthorization->iFlowIds.Append(iFlowIdentifier);
			}
		itemOffSet+=numberOfFlowIds;
		iMediaAuthorization->AppendL(*mediaAuthorization); // Add completed media authorization to array
        CleanupStack::Pop(mediaAuthorization);		
		}

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_CONSTRUCTL_7, "...Finished parsing Packet Context config parameters...");
	}	

void CSimPacketContext::GetCommSetupSettingsL()
	{
	iCommSetupArray = new (ELeave) CArrayFixFlat<TCommSetupItem>(KNumberofConfigs);

	TInt count = CfgFile()->ItemCount(KCommSetup);
	if(count==0)
		{
		TCommSetupItem comm;
			
		comm.iCsyName.Copy(KDefaultCsyName);
		comm.iPortName.Copy(KDefaultPortName);
		comm.iConfig.iRate=KDefaultCommPortRate;
		comm.iConfig.iHandshake=KDefaultHandshake;	
		comm.iContextName=KDefaultSetting;
		comm.iCommReset = EFalse;
		comm.iNumInitializeFailures = 0;
		comm.iErrorCode = KErrNone;
		comm.iDelay = 0;
		iCommSetupArray->AppendL(comm);
		}

	for(TInt i=0;i<count;i++)
		{
		const CTestConfigItem* item = CfgFile()->Item(KCommSetup,i);

		if(item)
			{
			TCommSetupItem comm;
			TPtrC8 portName,csyName,contextName;
			TInt commReset, numInitializeFailures, errorCode, delay;

			TInt ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,csyName);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETCOMMSETUPSETTINGSL_1, "WARNING - CONFIGURATION FILE PARSING - Reading element COMMSETUP::CSYNAME returned %d (element no. %d) from tag %s.",ret,0,KCommSetup);
				comm.iCsyName.Copy(KDefaultCsyName);
				}
			else
				comm.iCsyName.Copy(csyName);

			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,portName);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETCOMMSETUPSETTINGSL_2, "WARNING - CONFIGURATION FILE PARSING - Reading element COMMSETUP::PORTNAME returned %d (element no. %d) from tag %s.",ret,1,KCommSetup);
				comm.iPortName.Copy(KDefaultPortName);
				}
			else
				comm.iPortName.Copy(portName);

			TInt dataRate,handshake;
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,dataRate);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETCOMMSETUPSETTINGSL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element COMMSETUP::DATARATE returned %d (element no. %d) from tag %s.",ret,2,KCommSetup);
				comm.iConfig.iRate=KDefaultCommPortRate;
				}
			else
				comm.iConfig.iRate=(TBps)dataRate;

			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,handshake);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETCOMMSETUPSETTINGSL_4, "WARNING - CONFIGURATION FILE PARSING - Reading element COMMSETUP::HANDSHAKE returned %d (element no. %d) from tag %s.",ret,3,KCommSetup);
				comm.iConfig.iHandshake=KDefaultHandshake;
				}
			else
				comm.iConfig.iHandshake=(TUint)handshake;
			
			
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,contextName);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETCOMMSETUPSETTINGSL_5, "WARNING - CONFIGURATION FILE PARSING - Reading element COMMSETUP::CONTEXTNAME returned %d (element no. %d) from tag %s.",ret,4,KCommSetup);
				comm.iContextName=KDefaultSetting;
				}
			else
				comm.iContextName.Copy(contextName);

			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,5,commReset);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETCOMMSETUPSETTINGSL_6, "WARNING - CONFIGURATION FILE PARSING - Reading element COMMSETUP::COMMRESET returned %d (element no. %d) from tag %s.",ret,5,KCommSetup);
				comm.iCommReset=EFalse;
				}
			else
				comm.iCommReset = commReset;
			
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,6,numInitializeFailures);
			if(ret!=KErrNone)
				{
				comm.iNumInitializeFailures = 0;
				}
			else
				comm.iNumInitializeFailures = numInitializeFailures;
			
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,7,errorCode);
			if(ret!=KErrNone)
				{
				comm.iErrorCode = KErrNone;
				}
			else
				comm.iErrorCode = errorCode;
			
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,8,delay);
			if(ret!=KErrNone)
				{
				comm.iDelay = 0;
				}
			else
				comm.iDelay = delay;
			
			iCommSetupArray->AppendL(comm);
			}
		}
	}



void CSimPacketContext::GetContextConfigParamSettingsL(TPtrC8 aTag)
	{		
	CSimContextHelper::GetContextConfigParamSettingsL( CfgFile(), aTag, iContextConfigParams );		
	}


void CSimPacketContext::GetActivateSettings()
	{
	TInt ret;
	const CTestConfigItem* item=CfgFile()->Item(KActivatePauseDuration); //< Find the Activate data item
	if(item)
		{										//< then retrieve the activation pause
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,iActivatePause);
		if(ret!=KErrNone)
			iActivatePause = KDefaultPauseDuration;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,iActivateFail);
		if(ret==KErrNotFound)
			iActivateFail = KErrNone;
		}

	item=CfgFile()->Item(KDeactivatePauseDuration);	//< Find the Deactivate data item
	if(item)
		{											//< then retrieve the deactivation pause
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,iDeactivatePause);
		if(ret!=KErrNone)
			iDeactivatePause = KDefaultPauseDuration;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,iDeactivateFail);
		if(ret==KErrNotFound)
			iDeactivateFail = KErrNone;
		}

	item=CfgFile()->Item(KDeletePauseDuration);	//< Find the Delete data item
	if(item)
		{										//< then retrieve the delete pause
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,iDeletePause);
		if(ret!=KErrNone)
			iDeletePause = KDefaultPauseDuration;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,iDeleteFail);
		if(ret==KErrNotFound)
			iDeleteFail = KErrNone;
		}
	}
	
void CSimPacketContext::GetSetConfigSettings()
	{
	iSetConfigDelay = 0;
	const CTestConfigItem* item=iPacketService->CfgFile()->Item(KSetConfigDelay);
	if(item)
		{
		TInt delay = 0;
		TInt ret = CTestConfig::GetElement(item->Value(),KStdDelimiter,0,delay);

		if(ret != KErrNone || delay < 0)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETSETCONFIGSETTINGS_1, "WARNING - CONFIGURATION FILE PARSING - Reading element SETCONFIGDELAY returned %d (element no. %d) from tag %s.",ret,0,KSetConfigDelay);
			}
		else
			{
			iSetConfigDelay = delay;
			}
		}

	item=iPacketService->CfgFile()->Item(KSetConfigFail);	// Set the SetConfigFail struct
	if(item)
		{
		TPtrC8 contextName, numberOfTimes, failErrorCode;
		TInt32 digit = 0;
		TInt ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,contextName);

		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETSETCONFIGSETTINGS_2, "WARNING - CONFIGURATION FILE PARSING - Reading element SETCONFIGFAIL::CONTEXTNAME returned %d (element no. %d) from tag %s.",ret,0,KSetConfigFail);
			}
		else
			{
				iSetConfigFail.iContextName.Copy(contextName);
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,numberOfTimes);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETSETCONFIGSETTINGS_3, "WARNING - CONFIGURATION FILE PARSING - Reading element SETCONFIGFAIL::NUMBEROFTIMES returned %d (element no. %d) from tag %s.",ret,1,KSetConfigFail);
			}
		else
			{
				if(AsciiToNum(numberOfTimes, digit)==KErrNone)
					iSetConfigFail.iNumberOfTimes = digit;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,failErrorCode);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETSETCONFIGSETTINGS_4, "WARNING - CONFIGURATION FILE PARSING - Reading element SETCONFIGFAIL::FAILERRORCODE returned %d (element no. %d) from tag %s.",ret,2,KSetConfigFail);
			}
		else
			{
				if(AsciiToNum(failErrorCode, digit)==KErrNone)
					iSetConfigFail.iFailErrorCode = digit;
			}
		}
	}
	
void CSimPacketContext::GetContextConfigChangeSettingsL()
	{
	TNotifyContextConfigChangeData configChangeData;
	
	TUint count = CfgFile()->ItemCount(KNotifyContextConfigChange);
	TInt index;
	for(index = 0; index < count; index++)
		{
		const CTestConfigItem *item = 
			CfgFile()->Item(KNotifyContextConfigChange,index);
		
		if(!item)
			{
			break;
			}
		
		//Get the Notify Context Config Change Delay value
		TInt ret = CTestConfig::GetElement(item->Value(),KStdDelimiter,0,
					configChangeData.iDelay);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETCONTEXTCONFIGCHANGESETTINGSL_1, "WARNING - CONFIGURATION FILE PARSING - Reading element NOTIFYCONTEXTCONFIGCHANGE::DELAY returned %d (element no. %d) from tag %s.",ret,0,KNotifyContextConfigChange);
			continue;
			}
			
		//Get the Notify Context Config Change, config index value
		ret = CTestConfig::GetElement(item->Value(),KStdDelimiter,1,
					configChangeData.iNewContextConfigIndex);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETCONTEXTCONFIGCHANGESETTINGSL_2, "WARNING - CONFIGURATION FILE PARSING - Reading element NOTIFYCONTEXTCONFIGCHANGE::NEWCONTEXTCONFIGINDEX returned %d (element no. %d) from tag %s.",ret,0,KNotifyContextConfigChange);
			continue;		
			}
	
		iNotifyContextConfigChangeArray->AppendL(configChangeData);
		}
	}

void CSimPacketContext::GetPacketFilterSettings()
	{
	TInt ret;
	const CTestConfigItem* item=iPacketService->CfgFile()->Item(KDeleteTftFail);
	if(item)
	{
		TPtrC8 contextName, numberOfTimes, failErrorCode;
		TInt32 digit = 0;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,contextName);

		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETPACKETFILTERSETTINGS_1, "WARNING - CONFIGURATION FILE PARSING - Reading element SETCONFIGFAIL::CONTEXTNAME returned %d (element no. %d) from tag %s.",ret,0,KDeleteTftFail);
			}
		else
			{
			iDeleteTftFail.iContextName.Copy(contextName);
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,numberOfTimes);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETPACKETFILTERSETTINGS_2, "WARNING - CONFIGURATION FILE PARSING - Reading element SETCONFIGFAIL::NUMBEROFTIMES returned %d (element no. %d) from tag %s.",ret,1,KDeleteTftFail);
			}
		else
		{
			if(AsciiToNum(numberOfTimes, digit)==KErrNone)
				iDeleteTftFail.iNumberOfTimes = digit;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,failErrorCode);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETPACKETFILTERSETTINGS_3, "WARNING - CONFIGURATION FILE PARSING - Reading element SETCONFIGFAIL::FAILERRORCODE returned %d (element no. %d) from tag %s.",ret,2,KDeleteTftFail);
			}
		else
			{
			if(AsciiToNum(failErrorCode, digit)==KErrNone)
				iDeleteTftFail.iFailErrorCode = digit;
			}
	}

	item=iPacketService->CfgFile()->Item(KCreateTftConfig);
	if(item)
	{
		TPtrC8 contextName, numberOfTimes, failErrorCode;
		TInt32 digit = 0;
		TInt delay = 0;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,contextName);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETPACKETFILTERSETTINGS_4, "WARNING - CONFIGURATION FILE PARSING - Reading element SETCONFIGFAIL::CONTEXTNAME returned %d (element no. %d) from tag %s.",ret,0,KCreateTftConfig);
			}
		else
			{
			iCreateTftConfig.iContextName.Copy(contextName);
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,numberOfTimes);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETPACKETFILTERSETTINGS_5, "WARNING - CONFIGURATION FILE PARSING - Reading element SETCONFIGFAIL::NUMBEROFTIMES returned %d (element no. %d) from tag %s.",ret,1,KCreateTftConfig);
			}
		else
			{
			if(AsciiToNum(numberOfTimes, digit)==KErrNone)
				iCreateTftConfig.iNumberOfFailures = digit;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,failErrorCode);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETPACKETFILTERSETTINGS_6, "WARNING - CONFIGURATION FILE PARSING - Reading element SETCONFIGFAIL::FAILERRORCODE returned %d (element no. %d) from tag %s.",ret,2,KCreateTftConfig);
			}
		else
			{
			if(AsciiToNum(failErrorCode, digit)==KErrNone)
				iCreateTftConfig.iFailErrorCode = digit;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,delay);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETPACKETFILTERSETTINGS_7, "WARNING - CONFIGURATION FILE PARSING - Reading element SETCONFIGFAIL::FAILERRORCODE returned %d (element no. %d) from tag %s.",ret,3,KCreateTftConfig);
			}
		else
			{
			iCreateTftConfig.iDelay = delay;
			}
	}

	item=iPacketService->CfgFile()->Item(KAddPacketFilter);
	if(item)
	{
		TPtrC8 contextName, numberOfTimes, failErrorCode;
		TInt32 digit = 0;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,contextName);

		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETPACKETFILTERSETTINGS_8, "WARNING - CONFIGURATION FILE PARSING - Reading element SETCONFIGFAIL::CONTEXTNAME returned %d (element no. %d) from tag %s.",ret,0,KAddPacketFilter);
			}
		else
			{
			iAddPacketFilter.iContextName.Copy(contextName);
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,numberOfTimes);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETPACKETFILTERSETTINGS_9, "WARNING - CONFIGURATION FILE PARSING - Reading element SETCONFIGFAIL::NUMBEROFTIMES returned %d (element no. %d) from tag %s.",ret,1,KAddPacketFilter);
			}
		else
			{
			if(AsciiToNum(numberOfTimes, digit)==KErrNone)
				iAddPacketFilter.iNumberOfTimes = digit;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,failErrorCode);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETPACKETFILTERSETTINGS_10, "WARNING - CONFIGURATION FILE PARSING - Reading element SETCONFIGFAIL::FAILERRORCODE returned %d (element no. %d) from tag %s.",ret,2,KAddPacketFilter);
			}
		else
			{
			if(AsciiToNum(failErrorCode, digit)==KErrNone)
				iAddPacketFilter.iFailErrorCode = digit;
			}

		}
	}
	
void CSimPacketContext::GetContextConfigRel99SettingsL()
	{
	iContextConfigsRel99 = new(ELeave) CArrayFixFlat<TRel99ContextConfig>(KNumberofConfigs);

	TInt count = CfgFile()->ItemCount(KContextConfigRel99);		// set up the Rel 99 context configuration parameters
	for(TInt i=0;i<count;i++)
		{
		const CTestConfigItem* item = CfgFile()->Item(KContextConfigRel99,i);
		if(!item)
			break;
			
		TPtrC8 contextName;
		TInt activatePause, activateErrorCode, aDeactivatePause, deactivateErrorCode, deletionPause;
		TInt deletionErrorCode;
		TBool connectToNtRas;
		TRel99ContextConfig contextConfigRel99;

		TInt ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,contextName);

		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETCONTEXTCONFIGREL99SETTINGSL_1, "WARNING - CONFIGURATION FILE PARSING - Reading element CONTEXTCONFIGREL99::CONTEXTNAME returned %d (element no. %d) from tag %s.",ret,0,KContextConfigRel99);
			}
		else
			{
				contextConfigRel99.iContextName.Copy(contextName);
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,activatePause);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETCONTEXTCONFIGREL99SETTINGSL_2, "WARNING - CONFIGURATION FILE PARSING - Reading element CONTEXTCONFIGREL99::ACTIVATEPAUSE returned %d (element no. %d) from tag %s.",ret,1,KContextConfigRel99);
			continue;
			}
		else
			contextConfigRel99.iActivatePause = activatePause;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,activateErrorCode);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETCONTEXTCONFIGREL99SETTINGSL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element CONTEXTCONFIGREL99::ACTIVATEERRORCODE returned %d (element no. %d) from tag %s.",ret,2,KContextConfigRel99);
			continue;
			}
		else
			contextConfigRel99.iActivateErrorCode = activateErrorCode;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,aDeactivatePause);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETCONTEXTCONFIGREL99SETTINGSL_4, "WARNING - CONFIGURATION FILE PARSING - Reading element CONTEXTCONFIGREL99::DEACTIVATEPAUSE returned %d (element no. %d) from tag %s.",ret,3,KContextConfigRel99);
			continue;
			}
		else
			contextConfigRel99.iDeactivatePause = aDeactivatePause;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,deactivateErrorCode);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETCONTEXTCONFIGREL99SETTINGSL_5, "WARNING - CONFIGURATION FILE PARSING - Reading element CONTEXTCONFIGREL99::DEACTIVATEERRORCODE returned %d (element no. %d) from tag %s.",ret,4,KContextConfigRel99);
			continue;
			}
		else
			contextConfigRel99.iDeactivateErrorCode = deactivateErrorCode;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,5,deletionPause);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETCONTEXTCONFIGREL99SETTINGSL_6, "WARNING - CONFIGURATION FILE PARSING - Reading element CONTEXTCONFIGREL99::DELETETIONPAUSE returned %d (element no. %d) from tag %s.",ret,5,KContextConfigRel99);
			continue;
			}
		else
			contextConfigRel99.iDeletionPause = deletionPause;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,6, deletionErrorCode);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETCONTEXTCONFIGREL99SETTINGSL_7, "WARNING - CONFIGURATION FILE PARSING - Reading element CONTEXTCONFIGREL99::DELETIONERRORCODE returned %d (element no. %d) from tag %s.",ret,6,KContextConfigRel99);
			continue;
			}
		else
			contextConfigRel99.iDeletionErrorCode =  deletionErrorCode;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,7, connectToNtRas);
		if(ret!=KErrNone)
			{
			contextConfigRel99.iConnectToNtRas =  ETrue;
			}
		else
			contextConfigRel99.iConnectToNtRas =  connectToNtRas;

		iContextConfigsRel99->AppendL(contextConfigRel99);
		}
	}

/**
 Retrieves the (aIndex+1)th packet context status with the current context's context name
 ie. if aIndex is 0, the 1st context status entry is returned and if aIndex is 1, the 2nd is returned
 */
TNotifyContextStatusChange& CSimPacketContext::GetContextStatusChangeAt(TInt aIndex)
	{
	TInt count = 0;
	for (TInt i = 0; i < iNotifyContextStatusChangeArray->Count(); i++)
		{
		if (iContextName.Compare(iNotifyContextStatusChangeArray->At(i).iContextName) == 0)
			{
			if (count == aIndex)
				{
				return iNotifyContextStatusChangeArray->At(i);
				}
			else
				{
				count++;
				}
			}
		}
	return iNotifyContextStatusChangeArray->At(0);
	}

/**
 Retrieves the number of context status changes entries configured for the current packet context
 */
TInt CSimPacketContext::GetContextStatusChangeCount()
	{
	TInt count = 0;
	for (TInt i = 0; i < iNotifyContextStatusChangeArray->Count(); i++)
		{
		if (iContextName.Compare(iNotifyContextStatusChangeArray->At(i).iContextName) == 0)
			{
			count++;
			}
		}
	return count;
	}
	
void CSimPacketContext::GetContextStatusChangeSettingsL()
	{
	TPtrC8 contextStatusChangeType;
	TInt ret;
	
	iNotifyContextStatusChangeArray = new (ELeave) CArrayFixFlat<TNotifyContextStatusChange>(KNumberofConfigs);

	// default to using timer to drive context status changes
	iNotifyContextStatusChangeType = ETimer;
	const CTestConfigItem* item = CfgFile()->Item(KNotifyContextStatusChangeType);
	if (item)
		{
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,contextStatusChangeType);
		if(ret==KErrNone && contextStatusChangeType.Compare(KNotificationTypePublishSubscribe)==0)
			iNotifyContextStatusChangeType = EPublishAndSubscribe;
		}
	
	TInt count = CfgFile()->ItemCount(KNotifyContextStatusChange);		

	for(TInt i=0;i<count;i++)							// set up NotifyContextStatusChange info
		{
		item = CfgFile()->Item(KNotifyContextStatusChange,i);
		if(!item)
			break;

		TNotifyContextStatusChange notifyContextStatusChange;
		TPtrC8 contextName;
		TInt duration, contextStatus;
		
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,contextName);

		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETCONTEXTSTATUSCHANGESETTINGSL_1, "WARNING - CONFIGURATION FILE PARSING - Reading element NOTIFYCONTEXTSTATUSCHANGE::CONTEXTNAME returned %d (element no. %d) from tag %s.",ret,0,KNotifyContextStatusChange);
			continue;
			}
		else
			{
				notifyContextStatusChange.iContextName.Copy(contextName);
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,duration);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETCONTEXTSTATUSCHANGESETTINGSL_2, "WARNING - CONFIGURATION FILE PARSING - Reading element NOTIFYCONTEXTSTATUSCHANGE::DURATION returned %d (element no. %d) from tag %s.",ret,1,KNotifyContextStatusChange);
			continue;
			}
		else
			notifyContextStatusChange.iDuration = duration;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,contextStatus);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETCONTEXTSTATUSCHANGESETTINGSL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element NOTIFYCONTEXTSTATUSCHANGE::DURATION returned %d (element no. %d) from tag %s.",ret,2,KNotifyContextStatusChange);
			continue;
			}
		else
			notifyContextStatusChange.iStatus = (RPacketContext::TContextStatus)contextStatus;		

		iNotifyContextStatusChangeArray->AppendL(notifyContextStatusChange);

		if(GetContextStatusChangeCount() != 0)
			{
			iCurrentNotifyContextStatusChange=(RPacketContext::TContextStatus)GetContextStatusChangeAt(0).iStatus;
			if (iNotifyContextStatusChangeType == ETimer)
				{
				iNotifyContextStatusChangeIndex=0;
				iNotifyContextStatusChangeTimer->Start(GetContextStatusChangeAt(0).iDuration,this,ETimerIdPcktContextStatusChange);
				}
			}
		}
	}

void CSimPacketContext::ConstructPacketFiltersL()
	{
	iPacketFilterInfoArray = new (ELeave) CArrayFixFlat<TPacketFilterV3>(KNumberofConfigs);

	const CTestConfigItem* item;
	TInt count, ret;
	count = CfgFile()->ItemCount(KPacketFilterInfo);		

	for(TInt i=0;i<count;i++)							// set up Packet filter info
		{
		item = CfgFile()->Item(KPacketFilterInfo,i);
		if(!item)
			break;

		TPtrC8 srcAddr, srcAddrSubnetMask;
		TInt id, evaluationPrecedenceIndex, protocolNumberOrNextHeader;
		TInt srcPortMin, srcPortMax, destPortMin, destPortMax, ipSecSPI, toSorTrafficClass;
		TInt flowLabel, filterType; 
		TPacketFilterV3 packetFilter;

		// Check that the data structure is supported by the simulated TSY version
		if(iPhone->iSimTsyVersion < 3) // same version as TPacketFilterVx
			{
		 	(void)User::Leave(KErrNotSupported);
			}
		
		
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,id);

		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_CONSTRUCTPACKETFILTERSL_1, "WARNING - CONFIGURATION FILE PARSING - Reading element PACKETFILTERINFO::ID returned %d (element no. %d) from tag %s.",ret,0,KPacketFilterInfo);
			continue;	
			}
		else
			{
			packetFilter.iId= id;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,evaluationPrecedenceIndex);

		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_CONSTRUCTPACKETFILTERSL_2, "WARNING - CONFIGURATION FILE PARSING - Reading element PACKETFILTERINFO::EVALUATIONPRECEDENCEINDEX returned %d (element no. %d) from tag %s.",ret,1,KPacketFilterInfo);
			continue;	
			}
		else
			{
			packetFilter.iEvaluationPrecedenceIndex= evaluationPrecedenceIndex;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,srcAddr);

		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_CONSTRUCTPACKETFILTERSL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element PACKETFILTERINFO::SRCADDR returned %d (element no. %d) from tag %s.",ret,2,KPacketFilterInfo);
			continue;
			}
		else
			{
			TUint8 digit=0;
			TPtrC8 address;

			ret=CTestConfig::GetElement(srcAddr,'.',0, address);
			if((AsciiToNum(address, digit)==KErrNone)
				&& (ret == KErrNone))
				packetFilter.iSrcAddr[0] = digit;
			else
				packetFilter.iSrcAddr[0] = 0;
			ret=CTestConfig::GetElement(srcAddr,'.',1, address);
			if((AsciiToNum(address, digit)==KErrNone)
				&& (ret == KErrNone))
				packetFilter.iSrcAddr[1] = digit;
			else
				packetFilter.iSrcAddr[1] = 0;
			ret=CTestConfig::GetElement(srcAddr,'.',2, address);
			if((AsciiToNum(address, digit)==KErrNone)
				&& (ret == KErrNone))
				packetFilter.iSrcAddr[2] = digit;
			else
				packetFilter.iSrcAddr[2] = 0;
			ret=CTestConfig::GetElement(srcAddr,'.',3, address);
			if((AsciiToNum(address, digit)==KErrNone)
				&& (ret == KErrNone))
				packetFilter.iSrcAddr[3] = digit;
			else
				packetFilter.iSrcAddr[3] = 0;
			ret=CTestConfig::GetElement(srcAddr,'.',4, address);
			if((AsciiToNum(address, digit)==KErrNone)
				&& (ret == KErrNone))
				packetFilter.iSrcAddr[4] = digit;
			else
				packetFilter.iSrcAddr[4] = 0;
			ret=CTestConfig::GetElement(srcAddr,'.',5, address);
			if((AsciiToNum(address, digit)==KErrNone)
				&& (ret == KErrNone))
				packetFilter.iSrcAddr[5] = digit;
			else
				packetFilter.iSrcAddr[5] = 0;
			ret=CTestConfig::GetElement(srcAddr,'.',6, address);
			if((AsciiToNum(address, digit)==KErrNone)
				&& (ret == KErrNone))
				packetFilter.iSrcAddr[6] = digit;
			else
				packetFilter.iSrcAddr[6] = 0;
			ret=CTestConfig::GetElement(srcAddr,'.',7, address);
			if((AsciiToNum(address, digit)==KErrNone)
				&& (ret == KErrNone))
				packetFilter.iSrcAddr[7] = digit;
			else
				packetFilter.iSrcAddr[7] = 0;
			ret=CTestConfig::GetElement(srcAddr,'.',8, address);
			if((AsciiToNum(address, digit)==KErrNone)
				&& (ret == KErrNone))
				packetFilter.iSrcAddr[8] = digit;
			else
				packetFilter.iSrcAddr[8] = 0;
			ret=CTestConfig::GetElement(srcAddr,'.',9, address);
			if((AsciiToNum(address, digit)==KErrNone)
				&& (ret == KErrNone))
				packetFilter.iSrcAddr[9] = digit;
			else
				packetFilter.iSrcAddr[9] = 0;
			ret=CTestConfig::GetElement(srcAddr,'.',10, address);
			if((AsciiToNum(address, digit)==KErrNone)
				&& (ret == KErrNone))
				packetFilter.iSrcAddr[10] = digit;
			else
				packetFilter.iSrcAddr[10] = 0;
			ret=CTestConfig::GetElement(srcAddr,'.',11, address);
			if((AsciiToNum(address, digit)==KErrNone)
				&& (ret == KErrNone))
				packetFilter.iSrcAddr[11] = digit;
			else
				packetFilter.iSrcAddr[11] = 0;
			ret=CTestConfig::GetElement(srcAddr,'.',12, address);
			if((AsciiToNum(address, digit)==KErrNone)
				&& (ret == KErrNone))
				packetFilter.iSrcAddr[12] = digit;
			else
				packetFilter.iSrcAddr[12] = 0;
			ret=CTestConfig::GetElement(srcAddr,'.',13, address);
			if((AsciiToNum(address, digit)==KErrNone)
				&& (ret == KErrNone))
				packetFilter.iSrcAddr[13] = digit;
			else
				packetFilter.iSrcAddr[13] = 0;
			ret=CTestConfig::GetElement(srcAddr,'.',14, address);
			if((AsciiToNum(address, digit)==KErrNone)
				&& (ret == KErrNone))
				packetFilter.iSrcAddr[14] = digit;
			else
				packetFilter.iSrcAddr[14] = 0;
			ret=CTestConfig::GetElement(srcAddr,'.',15, address);
			if((AsciiToNum(address, digit)==KErrNone)
				&& (ret == KErrNone))
				packetFilter.iSrcAddr[15] = digit;
			else
				packetFilter.iSrcAddr[15] = 0;
			
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,srcAddrSubnetMask);

		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_CONSTRUCTPACKETFILTERSL_4, "WARNING - CONFIGURATION FILE PARSING - Reading element PACKETFILTERINFO::SRCADDRSUBNETMASK returned %d (element no. %d) from tag %s.",ret,3,KPacketFilterInfo);
			continue;
			}
		else
			{
			TUint8 digit=0;
			TPtrC8 address;

			ret=CTestConfig::GetElement(srcAddrSubnetMask,'.',0, address);
			if((AsciiToNum(address, digit)==KErrNone)
				&& (ret == KErrNone))
				packetFilter.iSrcAddrSubnetMask[0] = digit;
			else
				packetFilter.iSrcAddrSubnetMask[0] = 0;
			ret=CTestConfig::GetElement(srcAddrSubnetMask,'.',1, address);
			if((AsciiToNum(address, digit)==KErrNone)
				&& (ret == KErrNone))
				packetFilter.iSrcAddrSubnetMask[1] = digit;
			else
				packetFilter.iSrcAddrSubnetMask[1] = 0;
			ret=CTestConfig::GetElement(srcAddrSubnetMask,'.',2, address);
			if((AsciiToNum(address, digit)==KErrNone)
				&& (ret == KErrNone))
				packetFilter.iSrcAddrSubnetMask[2] = digit;
			else
				packetFilter.iSrcAddrSubnetMask[2] = 0;
			ret=CTestConfig::GetElement(srcAddrSubnetMask,'.',3, address);
			if((AsciiToNum(address, digit)==KErrNone)
				&& (ret == KErrNone))
				packetFilter.iSrcAddrSubnetMask[3] = digit;
			else
				packetFilter.iSrcAddrSubnetMask[3] = 0;
			ret=CTestConfig::GetElement(srcAddrSubnetMask,'.',4, address);
			if((AsciiToNum(address, digit)==KErrNone)
				&& (ret == KErrNone))
				packetFilter.iSrcAddrSubnetMask[4] = digit;
			else
				packetFilter.iSrcAddrSubnetMask[4] = 0;
			ret=CTestConfig::GetElement(srcAddrSubnetMask,'.',5, address);
			if((AsciiToNum(address, digit)==KErrNone)
				&& (ret == KErrNone))
				packetFilter.iSrcAddrSubnetMask[5] = digit;
			else
				packetFilter.iSrcAddrSubnetMask[5] = 0;
			ret=CTestConfig::GetElement(srcAddrSubnetMask,'.',6, address);
			if((AsciiToNum(address, digit)==KErrNone)
				&& (ret == KErrNone))
				packetFilter.iSrcAddrSubnetMask[6] = digit;
			else
				packetFilter.iSrcAddrSubnetMask[6] = 0;
			ret=CTestConfig::GetElement(srcAddrSubnetMask,'.',7, address);
			if((AsciiToNum(address, digit)==KErrNone)
				&& (ret == KErrNone))
				packetFilter.iSrcAddrSubnetMask[7] = digit;
			else
				packetFilter.iSrcAddrSubnetMask[7] = 0;
			ret=CTestConfig::GetElement(srcAddrSubnetMask,'.',8, address);
			if((AsciiToNum(address, digit)==KErrNone)
				&& (ret == KErrNone))
				packetFilter.iSrcAddrSubnetMask[8] = digit;
			else
				packetFilter.iSrcAddrSubnetMask[8] = 0;
			ret=CTestConfig::GetElement(srcAddrSubnetMask,'.',9, address);
			if((AsciiToNum(address, digit)==KErrNone)
				&& (ret == KErrNone))
				packetFilter.iSrcAddrSubnetMask[9] = digit;
			else
				packetFilter.iSrcAddrSubnetMask[9] = 0;
			ret=CTestConfig::GetElement(srcAddrSubnetMask,'.',10, address);
			if((AsciiToNum(address, digit)==KErrNone)
				&& (ret == KErrNone))
				packetFilter.iSrcAddrSubnetMask[10] = digit;
			else
				packetFilter.iSrcAddrSubnetMask[10] = 0;
			ret=CTestConfig::GetElement(srcAddrSubnetMask,'.',11, address);
			if((AsciiToNum(address, digit)==KErrNone)
				&& (ret == KErrNone))
				packetFilter.iSrcAddrSubnetMask[11] = digit;
			else
				packetFilter.iSrcAddrSubnetMask[11] = 0;
			ret=CTestConfig::GetElement(srcAddrSubnetMask,'.',12, address);
			if((AsciiToNum(address, digit)==KErrNone)
				&& (ret == KErrNone))
				packetFilter.iSrcAddrSubnetMask[12] = digit;
			else
				packetFilter.iSrcAddrSubnetMask[12] = 0;
			ret=CTestConfig::GetElement(srcAddrSubnetMask,'.',13, address);
			if((AsciiToNum(address, digit)==KErrNone)
				&& (ret == KErrNone))
				packetFilter.iSrcAddrSubnetMask[13] = digit;
			else
				packetFilter.iSrcAddrSubnetMask[13] = 0;
			ret=CTestConfig::GetElement(srcAddrSubnetMask,'.',14, address);
			if((AsciiToNum(address, digit)==KErrNone)
				&& (ret == KErrNone))
				packetFilter.iSrcAddrSubnetMask[14] = digit;
			else
				packetFilter.iSrcAddrSubnetMask[14] = 0;
			ret=CTestConfig::GetElement(srcAddrSubnetMask,'.',15, address);
			if((AsciiToNum(address, digit)==KErrNone)
				&& (ret == KErrNone))
				packetFilter.iSrcAddrSubnetMask[15] = digit;
			else
				packetFilter.iSrcAddrSubnetMask[15] = 0;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,protocolNumberOrNextHeader);

		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_CONSTRUCTPACKETFILTERSL_5, "WARNING - CONFIGURATION FILE PARSING - Reading element PACKETFILTERINFO::PROTOCOLNUMBERORNEXTNUMBER returned %d (element no. %d) from tag %s.",ret,4,KPacketFilterInfo);
			packetFilter.iProtocolNumberOrNextHeader = KUnsetInteger;				
			}
		else
			{
			packetFilter.iProtocolNumberOrNextHeader= protocolNumberOrNextHeader;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,5,srcPortMin);

		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_CONSTRUCTPACKETFILTERSL_6, "WARNING - CONFIGURATION FILE PARSING - Reading element PACKETFILTERINFO::SRCPORTMIN returned %d (element no. %d) from tag %s.",ret,5,KPacketFilterInfo);
			packetFilter.iSrcPortMin = KUnsetInteger;
			}
		else
			{
			packetFilter.iSrcPortMin= srcPortMin;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,6,srcPortMax);

		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_CONSTRUCTPACKETFILTERSL_7, "WARNING - CONFIGURATION FILE PARSING - Reading element PACKETFILTERINFO::SRCPORTMAX returned %d (element no. %d) from tag %s.",ret,6,KPacketFilterInfo);
			packetFilter.iSrcPortMax = KUnsetInteger;
			}
		else
			{
			packetFilter.iSrcPortMax= srcPortMax;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,7,destPortMin);

		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_CONSTRUCTPACKETFILTERSL_8, "WARNING - CONFIGURATION FILE PARSING - Reading element PACKETFILTERINFO::DESTPORTMIN returned %d (element no. %d) from tag %s.",ret,7,KPacketFilterInfo);
			packetFilter.iDestPortMin = KUnsetInteger;
			}
		else
			{
			packetFilter.iDestPortMin= destPortMin;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,8,destPortMax);

		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_CONSTRUCTPACKETFILTERSL_9, "WARNING - CONFIGURATION FILE PARSING - Reading element PACKETFILTERINFO::SRCPORTMAX returned %d (element no. %d) from tag %s.",ret,8,KPacketFilterInfo);
			packetFilter.iDestPortMax = KUnsetInteger;
			}
		else
			{
			packetFilter.iDestPortMax= destPortMax;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,9,ipSecSPI);

		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_CONSTRUCTPACKETFILTERSL_10, "WARNING - CONFIGURATION FILE PARSING - Reading element PACKETFILTERINFO::IPSECSPI returned %d (element no. %d) from tag %s.",ret,9,KPacketFilterInfo);
			packetFilter.iIPSecSPI = KUnsetInteger;
			}
		else
			{
			packetFilter.iIPSecSPI= ipSecSPI;
			}
	
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,10,toSorTrafficClass);

		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_CONSTRUCTPACKETFILTERSL_11, "WARNING - CONFIGURATION FILE PARSING - Reading element PACKETFILTERINFO::TOSORTRAFFICCLASS returned %d (element no. %d) from tag %s.",ret,10,KPacketFilterInfo);
			continue;
			}
		else
			{
			packetFilter.iTOSorTrafficClass = (TUint16)toSorTrafficClass;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,11,flowLabel);

		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_CONSTRUCTPACKETFILTERSL_12, "WARNING - CONFIGURATION FILE PARSING - Reading element PACKETFILTERINFO::FLOWLABEL returned %d (element no. %d) from tag %s.",ret,11,KPacketFilterInfo);
			packetFilter.iFlowLabel = KUnsetInteger;
			}
		else
			{
			packetFilter.iFlowLabel = flowLabel;
			}
		
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,12,filterType);

		if(ret!=KErrNone)
			{
			if(ret == KErrNotFound)
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_CONSTRUCTPACKETFILTERSL_13, "CONFIGURATION FILE PARSING - PacketFilterInfo::TPacketFilterV2 info encountered");
				packetFilter.iFilterType = RPacketContext::EPacketFilterTypeUnknown;
				}
			else
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_CONSTRUCTPACKETFILTERSL_14, "WARNING - CONFIGURATION FILE PARSING - Reading element PACKETFILTERINFO::FILTERTYPE returned %d (element no. %d) from tag %s.",ret,12,KPacketFilterInfo);
				continue;	
				}
			}
		else
			{
			packetFilter.iFilterType = static_cast<RPacketContext::TPacketFilterType>(filterType);
			}
					
		packetFilter.iAdded = EFalse;
		TRAP_IGNORE(iPacketFilterInfoArray->AppendL(packetFilter));
		}
	}

CSimPacketContext::~CSimPacketContext()
/**
* Trivial Destructor
* Closes all CObject type objects and destroys all other objects created in the ConstructL()
*
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_DTOR_1, "CSimPacketContext: Entered destructor");
	
	if (iContextConfigParams != NULL)
	{
		iContextConfigParams->Delete(0, iContextConfigParams->Count());
		delete iContextConfigParams;
	}
	if (iContextConfigsRel99 != NULL)
	{
		iContextConfigsRel99->Delete(0,iContextConfigsRel99->Count());
		delete iContextConfigsRel99;
	}
	if (iCommSetupArray != NULL)
	{
		iCommSetupArray->Delete(0, iCommSetupArray->Count());
		delete iCommSetupArray;
	}
	if (iNtRas != NULL)
		delete iNtRas;
	
	if (iTimer != NULL)
		delete iTimer;

	if (iSetConfigTimer != NULL)
		delete iSetConfigTimer;

	if (iInitContextTimer != NULL)
		delete iInitContextTimer;

	if (iCreateTftTimer != NULL)
		delete iCreateTftTimer;

	if (iNotifyContextStatusChangeTimer != NULL)
		delete iNotifyContextStatusChangeTimer;
	
	if (iNotifyContextConfigChangeTimer != NULL)
		delete iNotifyContextConfigChangeTimer;

	if (iPacketFilterInfoArray != NULL)
	{
		iPacketFilterInfoArray->Delete(0, iPacketFilterInfoArray->Count());
		delete iPacketFilterInfoArray;
	}
	if (iNotifyContextStatusChangeArray != NULL)
	{
		iNotifyContextStatusChangeArray->Delete(0, iNotifyContextStatusChangeArray->Count());
		delete iNotifyContextStatusChangeArray;
	}
	iPacketService->DecrementContextCount();

	if (iAuthorizationTokenResponse != NULL)
		{
		iAuthorizationTokenResponse->Delete(0, iAuthorizationTokenResponse->Count());
		delete iAuthorizationTokenResponse;
		}
	
	if (iMediaAuthorization != NULL)
		{
		for(TUint i=0; i<iMediaAuthorization->Count(); i++)
			{
			if (iMediaAuthorization->At(i).iFlowIds.Count() > 0)
				{
				iMediaAuthorization->At(i).iFlowIds.Reset();				
				}
			}
		iMediaAuthorization->Delete(0, iMediaAuthorization->Count());
		delete iMediaAuthorization;
		}
	
	delete iContextStatusChange;
	
	if(iSetConfigData !=NULL )
		{
		iSetConfigData->Delete(0, iSetConfigData->Count());
		delete iSetConfigData;
		}
	
	if(iNotifyContextConfigChangeArray !=NULL)
		{
		iNotifyContextConfigChangeArray->Delete(0,iNotifyContextConfigChangeArray->Count());
		delete iNotifyContextConfigChangeArray;
		}
}

CTelObject* CSimPacketContext::OpenNewObjectByNameL(const TDesC& aName)
/**
* Returns a pointer to an existing QoS object identified by name.
* This is not support and if called will leave.
*
* @param aName name of the QoS object to be opened
* @return CTelObject will return null and leave
* @leave Leaves with KErrNotSupported if the name is not as expected.
*/
	{
		if (aName.Compare(iQoSName) == KErrNone)
			{
				return iPacketqos;
			}

		User::Leave(KErrNotSupported);
		return NULL;
	}


CTelObject* CSimPacketContext::OpenNewObjectL(TDes& aNewName)
/**
* Creates a new CSimPacketQoS object and returns a pointer to it.
*
* @param aNewName new name of the object created
* @return CTelObject pointer to the CSimPacketQos object created
* @leave Leaves if out of memory.
*/
	{
	aNewName.Append(KSimPacketQosName);
	aNewName.AppendNum(++iQoSObjectCount);
	iQoSName.Copy(aNewName);
	iPacketqos = CSimPacketQoS::NewL(iPhone, this);
	return iPacketqos;
	}

CTelObject::TReqMode CSimPacketContext::ReqModeL(const TInt aIpc)
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
		case EPacketContextSetConfig:
		case EPacketContextActivate:
		case EPacketContextDeactivate:
		case EPacketContextDelete:
		case EPacketContextLoanCommPort:
		case EPacketContextRecoverCommPort:
		case EPacketContextGetConfig:
		case EPacketContextGetStatus:
		case EPacketContextGetDataVolumeTransferred:
		case EPacketContextGetLastErrorCause:
		case EPacketContextInitialiseContext:
		case EPacketContextEnumeratePacketFilters:
		case EPacketContextGetPacketFilterInfo:
		case EPacketContextAddPacketFilter:
		case EPacketContextRemovePacketFilter:
		case EPacketContextModifyActiveContext:
		case EPacketContextGetProfileName:
		case EPacketContextGetDNSInfo:
		case EPacketAddMediaAuthorization:
		case EPacketRemoveMediaAuthorization:
		case EPacketContextCreateNewTFT:
		case EPacketContextDeleteTFT:
			break;

		case EPacketContextNotifyConfigChanged:
		case EPacketContextNotifyStatusChange:
		case EPacketContextNotifyDataTransferred:
			ret=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
			break;

		default:
			User::Leave(KErrNotSupported);
			break;
		}

	return ret;
	}

TInt CSimPacketContext::GetProfileName(const TTsyReqHandle aTsyReqHandle, TName* aQoSProfile)
	{
/**
* This synchronous method allows a client to retrieve the name 
* of the QoS profile. The profile name is passed back in the aQoSProfile parameter
*/
	aQoSProfile->Copy(iQoSName);
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone; 
	}

TInt CSimPacketContext::RegisterNotification(const TInt aIpc)
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
		case EPacketContextNotifyConfigChanged:
		case EPacketContextNotifyStatusChange:
		case EPacketContextNotifyDataTransferred:
			return KErrNone;
		default:
			// Unknown or invalid IPC
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_REGISTERNOTIFICATION_1, "CSimPacketContext: Register error, unknown IPC");
			return KErrNotSupported;
		}
	}

TInt CSimPacketContext::DeregisterNotification(const TInt aIpc)
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
		case EPacketContextNotifyConfigChanged:
		case EPacketContextNotifyStatusChange:
		case EPacketContextNotifyDataTransferred:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_DEREGISTERNOTIFICATION_1, "CSimPacketContext: DeregisterNotification");
			return KErrNone;
		default:
			// Unknown or invalid IPC
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_DEREGISTERNOTIFICATION_2, "CSimPacketContext: Deregister error, unknown IPC");
			return KErrNotSupported;
		}
	}

TInt CSimPacketContext::NumberOfSlotsL(const TInt aIpc)
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
		case EPacketContextNotifyConfigChanged:
		case EPacketContextNotifyStatusChange:
		case EPacketContextNotifyDataTransferred:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_NUMBEROFSLOTSL_1, "CGprsDGprsTsy: Registered with 5 slots");
			numberOfSlots=5;
			break;
		default:
			// Unknown or invalid IPC
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_NUMBEROFSLOTSL_2, "CSimPacketContext: Number of Slots error, unknown IPC");
			User::Leave(KErrNotSupported);
			break;
		}  
	return numberOfSlots;
	}


TInt CSimPacketContext::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,
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
	//< if this context has been deleted then the client can not do anything with it
	//< if the client wants to use this class, the client must open a new object by 
	//< name with the name for this context and this will reactivate the context.
 	if(iDeleted && (aIpc != EPacketContextGetStatus))  
		{
		ReqCompleted(aTsyReqHandle,KErrNotReady);
		return KErrNone;
		}

	TAny* dataPtr=aPackage.Ptr1();
	TAny* dataPtr2=aPackage.Ptr2();

	switch (aIpc)
		{
		case EPacketContextSetConfig:
			{
			if (iSetConfigCallCount <= iSetConfigFail.iNumberOfTimes
				|| iSetConfigFail.iNumberOfTimes==0)
				{
				if (iSetConfigDelay > 0)
					{
					TSetConfigData setConfigData;
					setConfigData.iTsyReqHandle = aTsyReqHandle;
					setConfigData.iData = aPackage.Des1n();

					TRAPD(ret, iSetConfigData->AppendL(setConfigData) );
					if (ret != KErrNone)
                		{
                		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_EXTFUNC_1, "CSimPacketContext::ExtFunc AppendL call fail");
                		return ret;
                		}
                		
					iSetConfigTimer->Start(iSetConfigDelay,this,ETimerIdPcktContextSetConfig);
					}
				else
					{
					SetConfig(aTsyReqHandle, aPackage.Des1n());
					}
				iSetConfigCallCount++;
				return KErrNone;
			}
			else
				{
				iSetConfigCallCount=0;
				iSetConfigFail.iNumberOfTimes = 0;

				ReqCompleted(aTsyReqHandle, iSetConfigFail.iFailErrorCode);
				return KErrNone;				// if SetConfig() has been called the number of times allowed in SetConfigFail, fail request
				}
			}
		case EPacketContextGetProfileName:
			return GetProfileName(aTsyReqHandle, REINTERPRET_CAST(TName*,dataPtr));
		case EPacketContextSetConfigCancel:
			return SetConfigCancel(aTsyReqHandle);
		case EPacketContextGetConfig:
			return GetConfig(aTsyReqHandle, aPackage.Des1n());
		case EPacketContextGetConfigCancel:
			return GetConfigCancel(aTsyReqHandle);
		case EPacketContextNotifyConfigChanged:
			return NotifyConfigChanged(aTsyReqHandle, aPackage.Des1n());
		case EPacketContextNotifyConfigChangedCancel:
			return NotifyConfigChangedCancel(aTsyReqHandle);
		case EPacketContextActivate:
			{	//< Ensures that theres only one context active as only one active context is 
				//<permitted at any one time
			if(iIsActive)
				{
				ReqCompleted(aTsyReqHandle,KErrEtelCallAlreadyActive);
				return KErrNone;
				}
			else
				return Activate(aTsyReqHandle);
			}
		case EPacketContextActivateCancel:
			return ActivateCancel(aTsyReqHandle);
		case EPacketContextDeactivate:
			return Deactivate(aTsyReqHandle);
		case EPacketContextDeactivateCancel:
			return DeactivateCancel(aTsyReqHandle);
		case EPacketContextDelete:
			return Delete(aTsyReqHandle);
		case EPacketContextDeleteCancel:
			return DeleteCancel(aTsyReqHandle);
		case EPacketContextLoanCommPort:
			return LoanCommPort(aTsyReqHandle,
			REINTERPRET_CAST(RCall::TCommPort*, dataPtr));
		case EPacketContextLoanCommPortCancel:
			return LoanCommPortCancel(aTsyReqHandle);
		case EPacketContextRecoverCommPort:
			return RecoverCommPort(aTsyReqHandle);
		case EPacketContextRecoverCommPortCancel:
			return RecoverCommPortCancel(aTsyReqHandle);
		case EPacketContextGetStatus:
			return GetStatus(aTsyReqHandle,
			REINTERPRET_CAST(RPacketContext::TContextStatus*, dataPtr));
		case EPacketContextNotifyStatusChange:
			return NotifyStatusChange(aTsyReqHandle,
			REINTERPRET_CAST(RPacketContext::TContextStatus*, dataPtr));
		case EPacketContextNotifyStatusChangeCancel:
			return NotifyStatusChangeCancel(aTsyReqHandle);
		case EPacketContextGetDataVolumeTransferred:
			return GetDataVolumeTransferred(aTsyReqHandle,
			REINTERPRET_CAST(RPacketContext::TDataVolume*, dataPtr));
		case EPacketContextNotifyDataTransferred:
			return NotifyDataTransferred(aTsyReqHandle,
			REINTERPRET_CAST(RPacketContext::TDataVolume*, dataPtr),
			REINTERPRET_CAST(RPacketContext::TNotifyDataTransferredRequest*, dataPtr2));
		case EPacketContextNotifyDataTransferredCancel:
			return NotifyDataTransferredCancel(aTsyReqHandle);
		case EPacketContextGetLastErrorCause:
			return GetLastErrorCause(aTsyReqHandle,
			REINTERPRET_CAST(TInt*, dataPtr));
		case EPacketContextInitialiseContextCancel:
			return InitialiseContextCancel(aTsyReqHandle);
		case EPacketContextEnumeratePacketFiltersCancel:
			return EnumeratePacketFiltersCancel(aTsyReqHandle);
		case EPacketContextGetPacketFilterInfoCancel:
			return GetPacketFilterInfoCancel(aTsyReqHandle);
		case EPacketContextAddPacketFilterCancel:
			return AddPacketFilterCancel(aTsyReqHandle);
		case EPacketContextRemovePacketFilterCancel:
			return RemovePacketFilterCancel(aTsyReqHandle);
		case EPacketContextModifyActiveContextCancel:
			return ModifyActiveContextCancel(aTsyReqHandle);		
		case EPacketContextInitialiseContext:
			return InitialiseContext(aTsyReqHandle, aPackage.Des1n());
		case EPacketContextEnumeratePacketFilters:
			return EnumeratePacketFilters(aTsyReqHandle, *REINTERPRET_CAST(TInt*, dataPtr));
		case EPacketContextGetPacketFilterInfo:
			TInt retValue;
			TRAPD(err, retValue = GetPacketFilterInfoL(aTsyReqHandle, *REINTERPRET_CAST(TInt*, dataPtr), aPackage.Des2n()));
			if(err != KErrNone)
				{
				retValue = err;
				}
			return retValue;
		case EPacketContextAddPacketFilter:
			return AddPacketFilter(aTsyReqHandle, aPackage.Des1n());
		case EPacketContextRemovePacketFilter:
			return RemovePacketFilter(aTsyReqHandle, *REINTERPRET_CAST(TInt*, dataPtr));
		case EPacketContextModifyActiveContext:
			return ModifyActiveContext(aTsyReqHandle);
		case EPacketContextGetDNSInfo:
			return GetDnsInfo(aTsyReqHandle, aPackage.Des1n());
		case EPacketContextGetDNSInfoCancel:
			return GetDnsInfoCancel(aTsyReqHandle);
		case EPacketAddMediaAuthorization:
			return GetAddMediaAuthorization(aTsyReqHandle, aPackage.Des1n());
		case EPacketAddMediaAuthorizationCancel:
			return GetAddMediaAuthorizationCancel(aTsyReqHandle);
		case EPacketRemoveMediaAuthorization:
			return GetRemoveMediaAuthorization(aTsyReqHandle, reinterpret_cast<RPacketContext::TAuthorizationToken*>(dataPtr));
		case EPacketRemoveMediaAuthorizationCancel:
			return GetRemoveMediaAuthorizationCancel(aTsyReqHandle);
		case EPacketContextCreateNewTFT:
			return CreateNewTFT(aTsyReqHandle, REINTERPRET_CAST(TInt*, dataPtr));
		case EPacketContextCreateNewTFTCancel:
			return CreateNewTFTCancel(aTsyReqHandle);
		case EPacketContextDeleteTFT:
			return DeleteTFT(aTsyReqHandle);
		case EPacketContextDeleteTFTCancel:
			return DeleteTFTCancel(aTsyReqHandle);
		default:
			return KErrNotSupported;
		}
	}

TInt CSimPacketContext::CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle)
/**
* CancelService is called by the server when it is "cleaning-up" any still outstanding
* asynchronous requests before closing a client's sub-session.
* This will happen if a client closes its R-class handle without cancelling outstanding asynchronous requests.
* 
* @param aTsyReqHandle  The request handle for completing the request 
* @param aIpc Ipc representing the request
* @return err KErrNone if request completes ok
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_CANCELSERVICE_1, "CSimPacketContext: - CancelService called");
	switch (aIpc)
		{
		case EPacketContextSetConfig:
			return SetConfigCancel(aTsyReqHandle);
		case EPacketContextGetConfig:
			return GetConfigCancel(aTsyReqHandle);
		case EPacketContextNotifyConfigChanged:
			return NotifyConfigChangedCancel(aTsyReqHandle);
		case EPacketContextActivate:
			return ActivateCancel(aTsyReqHandle);
		case EPacketContextDeactivate:
			return DeactivateCancel(aTsyReqHandle);
		case EPacketContextDelete:
			return DeleteCancel(aTsyReqHandle);
		case EPacketContextLoanCommPort:
			return LoanCommPortCancel(aTsyReqHandle);
		case EPacketContextRecoverCommPort:
			return RecoverCommPortCancel(aTsyReqHandle);
		case EPacketContextNotifyStatusChange:
			return NotifyStatusChangeCancel(aTsyReqHandle);
		case EPacketContextNotifyDataTransferred:
			return NotifyDataTransferredCancel(aTsyReqHandle);
		case EPacketContextInitialiseContext:
			return InitialiseContextCancel(aTsyReqHandle);
		case EPacketContextEnumeratePacketFilters:
			return EnumeratePacketFiltersCancel(aTsyReqHandle);
		case EPacketContextGetPacketFilterInfo:
			return GetPacketFilterInfoCancel(aTsyReqHandle);
		case EPacketContextAddPacketFilter:
			return AddPacketFilterCancel(aTsyReqHandle);
		case EPacketContextRemovePacketFilter:
			return RemovePacketFilterCancel(aTsyReqHandle);
		case EPacketContextModifyActiveContext:
			return ModifyActiveContextCancel(aTsyReqHandle);
		case EPacketContextGetDNSInfo:
			return GetDnsInfoCancel(aTsyReqHandle);
		case EPacketAddMediaAuthorization:
			return GetAddMediaAuthorizationCancel(aTsyReqHandle);
		case EPacketRemoveMediaAuthorization:
			return GetRemoveMediaAuthorizationCancel(aTsyReqHandle);		
		case EPacketContextCreateNewTFT:
			return CreateNewTFTCancel(aTsyReqHandle);
		case EPacketContextDeleteTFT:
			return DeleteTFTCancel(aTsyReqHandle);
		default:
			return KErrGeneral; 
		} 
	}

TInt CSimPacketContext::TSetConfigData::Find(CArrayFixFlat<TSetConfigData>* aSetConfigArray, TTsyReqHandle aTsyReqHandle, TInt& aIndex)
	{
	for (TInt i = 0; i < aSetConfigArray->Count(); i++)
		{
		if (aSetConfigArray->At(i).iTsyReqHandle == aTsyReqHandle)
			{
			aIndex = i;
			return KErrNone;
			}
		}
	aIndex = -1;
	return KErrNotFound;
	}

TInt CSimPacketContext::SetConfig(const TTsyReqHandle aTsyReqHandle,const TDesC8* aConfig)
/**
* Sets the configuration
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aConfig pointer containing the parameters to set to.
* @return KErrNone

*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_SETCONFIG_1, "CSimPacketContext::SetConfig called");

	TPckg<TPacketDataConfigBase>* configBase = (TPckg<TPacketDataConfigBase>*)aConfig;
	TPacketDataConfigBase& configBaseV1 = (*configBase)();

	TInt err = KErrGeneral;

	//Extract the config params to be set
	TContextConfigParam contextConfigToSet;

	if(configBaseV1.ExtensionId()==TPacketDataConfigBase::KConfigGPRS)
		{
		TPckg<RPacketContext::TContextConfigGPRS>* configParam = (TPckg<RPacketContext::TContextConfigGPRS>*)aConfig;
		RPacketContext::TContextConfigGPRS& contextConfigV1 = (*configParam)();

		CSimContextHelper::SetTContextConfigParamFromTContextConfigGPRS( contextConfigToSet, contextConfigV1 );		
		}
		
	if(configBaseV1.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4)
		{
		TPckg<RPacketContext::TContextConfigR99_R4>* configParam = (TPckg<RPacketContext::TContextConfigR99_R4>*)aConfig;
		RPacketContext::TContextConfigR99_R4& contextConfigV1 = (*configParam)();

		CSimContextHelper::SetTContextConfigParamFromTContextConfigR99_R4( contextConfigToSet, contextConfigV1 );		
		}
		
	if(configBaseV1.ExtensionId()==TPacketDataConfigBase::KConfigRel5)
		{
		TPckg<RPacketContext::TContextConfig_R5>* configParam = (TPckg<RPacketContext::TContextConfig_R5>*)aConfig;
		RPacketContext::TContextConfig_R5& contextConfigV1 = (*configParam)();

		CSimContextHelper::SetTContextConfigParamFromTContextConfig_R5( contextConfigToSet, contextConfigV1 );
		}

	// Check the context config params to be set with the pre-defined 
	// list of allowed config params
	if((configBaseV1.ExtensionId()==TPacketDataConfigBase::KConfigGPRS)||
		configBaseV1.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4||
		configBaseV1.ExtensionId()==TPacketDataConfigBase::KConfigRel5)
		{
		
		TInt index;
		for(index=0; index < iContextConfigParams->Count(); index++)
			{
			TContextConfigParam thisConfig = iContextConfigParams->At(index);

			if ( !CSimContextHelper::EqualTContextConfigParam(contextConfigToSet, thisConfig ) )
				{
				err = KErrCorrupt;
				}
			else
				{
				err = KErrNone;
				iContextConfigParamsIndex = index;
				
				//Check for and complete any pending notifications
				if(iNotifyConfigGPRS.iNotifyPending)
					{
					iNotifyConfigGPRS.iNotifyPending=EFalse;
					CSimContextHelper::ConvertConfigParams(thisConfig, *(RPacketContext::TContextConfigGPRS*)iNotifyConfigGPRS.iNotifyData);
					ReqCompleted(iNotifyConfigGPRS.iNotifyHandle,KErrNone);
					}
					
				if(iNotifyConfigR99.iNotifyPending)
					{
					iNotifyConfigR99.iNotifyPending=EFalse;
					CSimContextHelper::ConvertConfigParams(thisConfig, *(RPacketContext::TContextConfigR99_R4*)iNotifyConfigR99.iNotifyData);
					ReqCompleted(iNotifyConfigR99.iNotifyHandle,KErrNone);						
					}
					
				if(iNotifyConfigR5.iNotifyPending)
					{
					iNotifyConfigR5.iNotifyPending=EFalse;
					CSimContextHelper::ConvertConfigParams(thisConfig, *(RPacketContext::TContextConfig_R5*)iNotifyConfigR5.iNotifyData);
					ReqCompleted(iNotifyConfigR5.iNotifyHandle,KErrNone);						
					}
				break;
				}
			}//end for loop
		ReqCompleted(aTsyReqHandle,err);
		}//end if((configBaseV1....

	else
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}

	return KErrNone;
	}

TInt CSimPacketContext::SetConfigCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Cancels the Setconfig request.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @return err KErrNone 
*/
	{
	TInt index  = 0;
	if (TSetConfigData::Find(iSetConfigData, aTsyReqHandle, index) == KErrNone)
		{
		iSetConfigData->Delete(index);
		}
	if (iSetConfigData->Count() == 0)
		iSetConfigTimer->Cancel();
	
	ReqCompleted(aTsyReqHandle, KErrCancel);
	
	return KErrNone;
	}

TInt CSimPacketContext::GetConfig(const TTsyReqHandle aTsyReqHandle,TDes8* aConfig)
/**
* Retrieves and returns the configuration
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aConfig pointer to the data to retrieve
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETCONFIG_1, "CSimPacketContext::GetConfig called");
	TPckg<TPacketDataConfigBase>* configBase = (TPckg<TPacketDataConfigBase>*)aConfig;
	TPacketDataConfigBase& configBaseV1 = (*configBase)();
	
	// GPRS configuration
	if(configBaseV1.ExtensionId()==TPacketDataConfigBase::KConfigGPRS)
		{
		TContextConfigParam config = iContextConfigParams->At(iContextConfigParamsIndex);

		TPckg<RPacketContext::TContextConfigGPRS>* contextConfigGPRSPckg = (TPckg<RPacketContext::TContextConfigGPRS>*)aConfig;
		RPacketContext::TContextConfigGPRS& contextConfigV1 = (*contextConfigGPRSPckg)();

		CSimContextHelper::ConvertConfigParams(config, contextConfigV1);
		
		ReqCompleted(aTsyReqHandle,KErrNone);
		}

	// R99/R4 configuration
	else if (configBaseV1.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4)
		{
		TContextConfigParam config = iContextConfigParams->At(iContextConfigParamsIndex);

		TPckg<RPacketContext::TContextConfigR99_R4>* configRel99Pckg = (TPckg<RPacketContext::TContextConfigR99_R4>*)aConfig;
		RPacketContext::TContextConfigR99_R4& configRel99 = (*configRel99Pckg)();
	
		CSimContextHelper::ConvertConfigParams(config, configRel99);
		
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
		
	// R5 configuration
	else if (configBaseV1.ExtensionId()==TPacketDataConfigBase::KConfigRel5)
		{
		TContextConfigParam config = iContextConfigParams->At(iContextConfigParamsIndex);

		TPckg<RPacketContext::TContextConfig_R5>* configRel5Pckg = (TPckg<RPacketContext::TContextConfig_R5>*)aConfig;
		RPacketContext::TContextConfig_R5& configRel5 = (*configRel5Pckg)();

		CSimContextHelper::ConvertConfigParams(config, configRel5);
		
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	return KErrNone;
	}

TInt CSimPacketContext::GetConfigCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
* Cancels the Getconfig request.  This method will do nothing in this version of SimTSY.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @return err KErrNone 
*/
	{
	return KErrNone;
	}

TInt CSimPacketContext::NotifyConfigChanged(const TTsyReqHandle aTsyReqHandle, TDes8* aConfig)
/**
* Records a client interest in being notified of a change in the Context Configuration.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aConfig pointer to the context config to store the config info when the config changes.
* @return KErrNone
*/
	{
	TPckg<TPacketDataConfigBase>* configBase = (TPckg<TPacketDataConfigBase>*)aConfig;
	TPacketDataConfigBase& configBaseV1 = (*configBase)();
	
	if(configBaseV1.ExtensionId() == TPacketDataConfigBase::KConfigGPRS)
		{
		__ASSERT_ALWAYS(!iNotifyConfigGPRS.iNotifyPending,SimPanic(ENotificationAlreadyPending));

		iNotifyConfigGPRS.iNotifyPending = ETrue;
		iNotifyConfigGPRS.iNotifyHandle = aTsyReqHandle;
		iNotifyConfigGPRS.iNotifyData = &configBaseV1;
		}
	else if(configBaseV1.ExtensionId() == TPacketDataConfigBase::KConfigRel99Rel4)
		{
		__ASSERT_ALWAYS(!iNotifyConfigR99.iNotifyPending,SimPanic(ENotificationAlreadyPending));

		iNotifyConfigR99.iNotifyPending = ETrue;
		iNotifyConfigR99.iNotifyHandle = aTsyReqHandle;
		iNotifyConfigR99.iNotifyData = &configBaseV1;
		}
	else if(configBaseV1.ExtensionId() == TPacketDataConfigBase::KConfigRel5)
		{
		__ASSERT_ALWAYS(!iNotifyConfigR5.iNotifyPending,SimPanic(ENotificationAlreadyPending));

		iNotifyConfigR5.iNotifyPending = ETrue;
		iNotifyConfigR5.iNotifyHandle = aTsyReqHandle;
		iNotifyConfigR5.iNotifyData = &configBaseV1;
		}		

	return KErrNone;
	}

TInt CSimPacketContext::NotifyConfigChangedCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Cancel a client's interest in being notified when the context configuration changes
* This is acheived simply by resetting the flag that indicates a notification is pending.
* 
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_NOTIFYCONFIGCHANGEDCANCEL_1, "CSimPacketContext::NotifyConfigChangedCancel called");
	if(iNotifyConfigGPRS.iNotifyPending &&
		iNotifyConfigGPRS.iNotifyHandle == aTsyReqHandle)
		{
		iNotifyConfigGPRS.iNotifyPending=EFalse;
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	else if(iNotifyConfigR99.iNotifyPending &&
		iNotifyConfigR99.iNotifyHandle == aTsyReqHandle)
		{
		iNotifyConfigR99.iNotifyPending=EFalse;
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	else if(iNotifyConfigR5.iNotifyPending &&
		iNotifyConfigR5.iNotifyHandle == aTsyReqHandle)
		{
		iNotifyConfigR5.iNotifyPending=EFalse;
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}
		
	return KErrNone;
	}

TInt CSimPacketContext::AttemptDeactivateImmediately()
/**
* Called when client wishes to Detach when there is an active context.
* This will deactivate the context immediately from the network.
* 
* @return KErrNone if deactivated successfully.
*/
	{
	if((iState==RPacketContext::EStatusActive) || (iState==RPacketContext::EStatusActivating))
		{
		iCurrentEvent=EContextEventDeactivate;
		TInt ret = ChangeState(RPacketContext::EStatusDeactivating);
			if(ret!=KErrNone)
				return ret;
		return ChangeState(RPacketContext::EStatusInactive);
		}
	return KErrNone;
	}

TInt CSimPacketContext::ReActivate(CSimPacketService* aPacketService, const TName& aContextName)
/**
* This method is called by the packet Service to Re-Initialise a Deleted context.
* If the context is deleted then the client cannot retrieve any 
* infomation or use the context in any way.
* In order for the client to use the context again, the client must 
* open an object by name passing
* the name of this context as a pararemter.
*
* @param aPacketService pointer to the parent class CSimPacketService.
* @param aContextName Must be the same name as the name for this context.
* @return KErrNone if no problem in re-initialising this context.
*/
	{
	if(iContextName.Compare(aContextName) != KErrNone)
		{
		return KErrBadName;
		}
		
	iPacketService = aPacketService;

	if(iDeleted)
		{

		TInt ret = ChangeState(RPacketContext::EStatusInactive);
		if (ret != KErrNone)
			{
			return ret;
			}

		iDeleted = EFalse;
		iContextConfigParamsIndex = 0;
		iNotifyStatusChange.iNotifyPending = EFalse;
		iNotifyConfigGPRS.iNotifyPending = EFalse;
		iNotifyConfigR99.iNotifyPending = EFalse;
		iNotifyConfigR5.iNotifyPending = EFalse;

		}
	return KErrNone;
	}

TInt CSimPacketContext::DoDeactivate()
/**
* This method is called when timer events for deactivate context has completed.
* It will then try to terminate the NTRas Connection.
*
* @return KErrNone
*/
	{
	iNotifyContextConfigChangeTimer->Cancel();
	iNtRas->Terminate();
	return KErrNone;
	}

TInt CSimPacketContext::Deactivate(const TTsyReqHandle aTsyReqHandle)
/**
* Client calls this method to try to Deactivate an active context.
* In this method, we'll store the TsyReqHandle for later completion then call ActionEvent
* state machine to handle the event which will kick off a timer.
* The Actual Deactivation will take place in the DoDeactivate method.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_DEACTIVATE_1, "CSimPacketContext::Deactivate called");
	iDeactivateRequestHandle = aTsyReqHandle;
	TInt ret;
	ret = ActionEvent(EContextEventDeactivate,KErrNone);
	return ret;
	}

TInt CSimPacketContext::DeactivateCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Client calls this method to try and cancel a Deactivate request.
* This will only complete successfully if and only if the Deactivate request has not completed.
* i.e. if we haven't already deactivated the context.
*
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_DEACTIVATECANCEL_1, "CSimPacketContext::DeactivateCancel called");
	if(((iState == RPacketContext::EStatusInactive) || (iState == RPacketContext::EStatusDeactivating)) && 
					(iCurrentEvent==EContextEventDeactivate))
		{
		iTimer->Cancel();
		iCurrentEvent= EContextEventNone;
		TInt ret = ChangeState(RPacketContext::EStatusActive);
		if(ret!= KErrNone)
			ReqCompleted(aTsyReqHandle,ret);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}

	return KErrNone;
	}
	
TInt CSimPacketContext::DoActivate()
/**
* This method is called when timer events for Activate context has completed.
* It will then try to Set up the NtRas Connection and open a port for client use.
*
* @return KErrNone
*/
	{
	TCommConfigV01 commConfig;
	TCommSetupItem item = FindCommSettings();

	if (iPacketqos)
		{
		iPacketqos->ClearChangeQoSPending();
		}
	commConfig.iRate=item.iConfig.iRate;
	commConfig.iDataBits=EData8;
	commConfig.iStopBits=EStop1;
	commConfig.iHandshake=item.iConfig.iHandshake;
	iCommReset=item.iCommReset;
	TInt ras = iNtRas->Connect(item.iCsyName,item.iPortName,commConfig,this);
	
	TContextConfigParam thisConfig = iContextConfigParams->At(iContextConfigParamsIndex);
	
	// Check for and complete any pending notifications
	if(iNotifyConfigGPRS.iNotifyPending)
		{
		iNotifyConfigGPRS.iNotifyPending=EFalse;
		CSimContextHelper::ConvertConfigParams(thisConfig, *(RPacketContext::TContextConfigGPRS*)iNotifyConfigGPRS.iNotifyData);
		ReqCompleted(iNotifyConfigGPRS.iNotifyHandle,KErrNone);
		}
		
	if(iNotifyConfigR99.iNotifyPending)
		{
		iNotifyConfigR99.iNotifyPending=EFalse;
		CSimContextHelper::ConvertConfigParams(thisConfig, *(RPacketContext::TContextConfigR99_R4*)iNotifyConfigR99.iNotifyData);
		ReqCompleted(iNotifyConfigR99.iNotifyHandle,KErrNone);						
		}
		
	if(iNotifyConfigR5.iNotifyPending)
		{
		iNotifyConfigR5.iNotifyPending=EFalse;
		CSimContextHelper::ConvertConfigParams(thisConfig, *(RPacketContext::TContextConfig_R5*)iNotifyConfigR5.iNotifyData);
		ReqCompleted(iNotifyConfigR5.iNotifyHandle,KErrNone);						
		}
	
	// start the timer for the simulated network-side context parameter changes
	if(iNotifyContextConfigChangeArray->Count() > 0)
		{
		TInt delay = (iNotifyContextConfigChangeArray->At(0)).iDelay;
		iNotifyContextConfigChangeTimer->Start(delay,this,ETimerIdPcktContextConfigChange);
		}

	return ras;
	}


TInt CSimPacketContext::Activate(const TTsyReqHandle aTsyReqHandle)
/**
* Client calls this method to try to Activate a context.
* In this method, we'll store the TsyReqHandle for later completion then call ActionEvent
* state machine to handle the event which will kick off a timer.
* The Actual Activation will take place in the DoActivate method.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @return KErrNone
*/
	{
	iActivateRequestHandle = aTsyReqHandle;
	TInt ret;
	ret = ActionEvent(EContextEventActivate,KErrNone);
	return ret;
	}


TInt CSimPacketContext::ActivateCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Client calls this method to try and cancel an Activate request.
* This will only complete successfully if and only if the Activate request has not completed.
* i.e. if we haven't already activated the context.
*
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_ACTIVATECANCEL_1, "CSimPacketContext::ActivateCancel called");
	if(((iState == RPacketContext::EStatusInactive) || (iState == RPacketContext::EStatusActivating)) && 
					(iCurrentEvent==EContextEventActivate))
		{
		iTimer->Cancel();
		iCurrentEvent= EContextEventNone;
		TInt ret = ChangeState(RPacketContext::EStatusInactive);
		if(ret!= KErrNone)
			ReqCompleted(aTsyReqHandle,ret);
		else
			ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	else if (iState == RPacketContext::EStatusDeleted)
		{
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}

	return KErrNone;
	}

TInt CSimPacketContext::DoDelete()
/**
* This method is called when timer events for Delete context has completed.
* It will then try to Terminate the NtRas Connection and then set the context as deleted.
* Once the context is deleted then the client cannot retrieve any information about the context 
* or use the context.  calling ReActivateContext  will reinstate this context.
*
* @return KErrNone
*/
	{
	iDeleted = ETrue;
	iNtRas->Terminate();
	iQoSObjectCount--;
	return KErrNone;
	}


TInt CSimPacketContext::Delete(const TTsyReqHandle aTsyReqHandle)
/**
* Client calls this method to try to Delete this context.
* In this method, we'll store the TsyReqHandle for later completion then call ActionEvent
* state machine to handle the event which will kick off a timer.
* The Actual Delete will take place in the DoDelete method.
*
* This request will only succeed if the context already exist (had been created) and also not
* previously deleted.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_DELETE_1, "CSimPacketContext::Delete called");
	iDeleteRequestHandle = aTsyReqHandle;
	TInt ret;
	ret = ActionEvent(EContextEventDelete,KErrNone);
	return ret;
	}

TInt CSimPacketContext::DeleteCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Client calls this method to try and cancel a Delete context request.
* This will only complete successfully if and only if the Delete request has not completed.
* i.e. if we haven't already Deleted the context.
*
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_DELETECANCEL_1, "CSimPacketContext::DeleteCancel called");
	if((iState == RPacketContext::EStatusInactive) && (iCurrentEvent==EContextEventDelete))
		{
		iTimer->Cancel();
		iCurrentEvent= EContextEventNone;
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	return KErrNone;
	}

TInt CSimPacketContext::LoanCommPort(const TTsyReqHandle aTsyReqHandle,RCall::TCommPort* aDataPort)
/**
* The method will return to the client the port it has opened previously in the Activate method
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aDataPort Pointer to the port  and csy name for the opened port.
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_LOANCOMMPORT_1, "CSimPacketContext::LoanCommPort called");

	if(iState != RPacketContext::EStatusActive)
		{
		ReqCompleted(aTsyReqHandle, KErrEtelCallNotActive);
		return KErrNone;
		}
	if (iCommPortLoaned)
		{
		ReqCompleted(aTsyReqHandle, KErrEtelPortAlreadyLoaned);
		return KErrNone;
		}

		TCommSetupItem item = FindCommSettings();
		aDataPort->iCsy.Copy(item.iCsyName);
		aDataPort->iPort.Copy(item.iPortName);
		iCommPortLoaned = ETrue;
		ReqCompleted(aTsyReqHandle,KErrNone);
	
	return KErrNone;
	}

TInt CSimPacketContext::LoanCommPortCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
* Cancels a client loan port request.  This method will do nothing. 
*
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_LOANCOMMPORTCANCEL_1, "CSimPacketContext::LoanCommPortCancel called");
	return KErrNone;
	}

TInt CSimPacketContext::RecoverCommPort(const TTsyReqHandle aTsyReqHandle)
/**
* In this method we'll try and terminate the NtRas connection and reover the port.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_RECOVERCOMMPORT_1, "CSimPacketContext::RecoverCommPort called");
	if (!iCommPortLoaned)
		{
		ReqCompleted(aTsyReqHandle, KErrEtelPortNotLoanedToClient);
		return KErrNone;
		}
	iNtRas->Terminate();
	iCommPortLoaned = EFalse;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimPacketContext::RecoverCommPortCancel(const TTsyReqHandle /*aTsyReqHandle*/)	
/**
* Attempt to cancel a RecoverCommPort request.  This method does nothing 
*
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_RECOVERCOMMPORTCANCEL_1, "CSimPacketContext::RecoverCommPortCancel called");
	return KErrNone;
	}

TInt CSimPacketContext::GetStatus(const TTsyReqHandle aTsyReqHandle,RPacketContext::TContextStatus* aContextStatus)
/**
* Retrieves and returns the current status of the Packet context in the aContextStatus parameter.
*
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @param aContextStatus pointer to the status of the packet context.
* @return KerrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETSTATUS_1, "CSimPacketContext::GetStatus called");
	*aContextStatus = iState;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimPacketContext::NotifyStatusChange(const TTsyReqHandle aTsyReqHandle,RPacketContext::TContextStatus* aContextStatus)
/**
* Record a client's interest in being notified of the state of the packet contexts.
* First check that there isn't already a notification pending (the ETel Server should protect
* against this) and then record the information necessary to complete the request later, when
* the status does actually change.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aContextStatus pointer to the packet context status
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_NOTIFYSTATUSCHANGE_1, "CSimPacketContext::NotifyStatusChange called");
	__ASSERT_ALWAYS(!iNotifyStatusChange.iNotifyPending,SimPanic(ENotificationAlreadyPending));
	iNotifyStatusChange.iNotifyPending = ETrue;
	iNotifyStatusChange.iNotifyHandle = aTsyReqHandle;
	iNotifyStatusChange.iNotifyData = aContextStatus;
	return KErrNone;
	}



TInt CSimPacketContext::NotifyStatusChangeCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Cancel a client's interest in being notified when the packet context status changes.
* This is acheived simply by resetting the flag that indicates a notification is pending.
* 
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_NOTIFYSTATUSCHANGECANCEL_1, "CSimPacketContext::NotifyStatusChangeCancel called");
	if(iNotifyStatusChange.iNotifyPending)
		{
		iNotifyStatusChange.iNotifyPending=EFalse;
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	return KErrNone;
	}

TInt CSimPacketContext::GetDataVolumeTransferred(const TTsyReqHandle aTsyReqHandle,RPacketContext::TDataVolume* /*aVolume*/)
/**
* Retrieves and returns the to the client the amount of data sent over the serial port.
* This method is not supported and will request complete with KErrNotSupported
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aVolume pointer to the volume of data trasferred so far.
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETDATAVOLUMETRANSFERRED_1, "CSimPacketContext::GetDataVolumeTransferred called");
	ReqCompleted(aTsyReqHandle,KErrNotSupported);
	return KErrNone;
	}

TInt CSimPacketContext::NotifyDataTransferred(const TTsyReqHandle aTsyReqHandle, RPacketContext::TDataVolume* /*aVolume*/, RPacketContext::TNotifyDataTransferredRequest* /*aData*/)
/**
* Notifies the client when data is sent or received over the comm port
* This method is not supported in this version of the TSY.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aVolume pointer to the volume of data trasferred so far.
* @param aData indicates to the client whether the data was sent or received.
* @return KErrNone
*/
	{
	ReqCompleted(aTsyReqHandle,KErrNotSupported);
	return KErrNone;
	}

TInt CSimPacketContext::NotifyDataTransferredCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
* Cancels the NotifyDataTransferred request
*
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KerrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_NOTIFYDATATRANSFERREDCANCEL_1, "CSimPacketContext::NotifyDataTransferredCancel called");
	return KErrNone;
	}

TInt CSimPacketContext::GetLastErrorCause(const TTsyReqHandle aTsyReqHandle, TInt* aError)
/**
* This method is not supported in this TSY
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aError pointer to the last error cause
* @return KerrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETLASTERRORCAUSE_1, "CSimPacketContext::GetLastErrorCause called");
	*aError = iLastError;
	ReqCompleted(aTsyReqHandle,iErrorCodeForGetLastErrorCause);
	iErrorCodeForGetLastErrorCause = KErrNone;
	return KErrNone;
	}

void CSimPacketContext::TimerCallBack(TInt aId)
/**
* Timer callback function.  When the timer goes off, it will call back into this
* function for further processing.
*/
	{
	TInt newIndex = KErrNotFound;
	
	switch(aId)
		{
		case ETimerIdPcktContextStatusChange:
			TimerCallBackNotifyContextStatusChange();
			break;
		case ETimerIdPcktContextSetConfig:
			SetConfig(iSetConfigData->At(0).iTsyReqHandle, iSetConfigData->At(0).iData);
			iSetConfigData->Delete(0);
			if (iSetConfigData->Count() != 0)
				iSetConfigTimer->Start(iSetConfigDelay,this,ETimerIdPcktContextSetConfig);
			break;
		case ETimerIdPcktContextConfigChange:
			//Check the validity of the new context config param index before applying it
			newIndex = (iNotifyContextConfigChangeArray->At(0)).iNewContextConfigIndex;
			if(newIndex <= iContextConfigParams->Count() - 1)
				{
				//Change the context config param index to point to the new context configuration
				iContextConfigParamsIndex = newIndex;
				
				// Check for and complete any pending notifications
				if(iNotifyConfigGPRS.iNotifyPending)
					{
					iNotifyConfigGPRS.iNotifyPending=EFalse;
					CSimContextHelper::ConvertConfigParams(iContextConfigParams->At(iContextConfigParamsIndex), 
							*(RPacketContext::TContextConfigGPRS*)iNotifyConfigGPRS.iNotifyData);
					ReqCompleted(iNotifyConfigGPRS.iNotifyHandle,KErrNone);
					}

				if(iNotifyConfigR99.iNotifyPending)
					{
					iNotifyConfigR99.iNotifyPending=EFalse;
					CSimContextHelper::ConvertConfigParams(iContextConfigParams->At(iContextConfigParamsIndex), 
							*(RPacketContext::TContextConfigR99_R4*)iNotifyConfigR99.iNotifyData);
					ReqCompleted(iNotifyConfigR99.iNotifyHandle,KErrNone);						
					}
				
				if(iNotifyConfigR5.iNotifyPending)
					{
					iNotifyConfigR5.iNotifyPending=EFalse;
					CSimContextHelper::ConvertConfigParams(iContextConfigParams->At(iContextConfigParamsIndex), 
							*(RPacketContext::TContextConfig_R5*)iNotifyConfigR5.iNotifyData);
					ReqCompleted(iNotifyConfigR5.iNotifyHandle,KErrNone);						
					}	
				}

			else
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_TIMERCALLBACK_1, ">>CSimPacketContext.cpp: Invalid new context config param index");
				}
				
			//Delete the current NotifyContextConfigChange item and set the next one if possible
			iNotifyContextConfigChangeArray->Delete(0);
			
			if(iNotifyContextConfigChangeArray->Count() > 0)
				{
				TInt delay = (iNotifyContextConfigChangeArray->At(0)).iDelay;
				iNotifyContextConfigChangeTimer->Start(delay,this, ETimerIdPcktContextConfigChange);
				}
			break;

		case ETimerIdPcktContextInitContext:
			DoInitialiseContext();
			break;
		case ETimerIdPcktContextCreateTft:
			DoCreateNewTFT();
			break;
		default:
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_TIMERCALLBACK_2, ">>CSimPacketContext::TimerCallBack");
			TInt ret=ActionEvent(EContextEventTimeOut,KErrNone);
			__ASSERT_ALWAYS(ret==KErrNone,SimPanic(ETimeOutEventActionFailed));	// Note: this is very crude error handling and should be replaced by something rather more elegant.
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_TIMERCALLBACK_3, "<<CSimPacketContext::TimerCallBack");
			iTFTChangeBool = EFalse;
			break;
			}		
		}
	}

void CSimPacketContext::PubSubCallback(TInt aIndex)
	{
	TInt length = iNotifyContextStatusChangeArray->Count();
	if (aIndex < length)
		{
		TNotifyContextStatusChange& statusChange = iNotifyContextStatusChangeArray->At(aIndex);
		if (statusChange.iContextName.Compare(iContextName) == 0)
			{
			CompleteContextStatusChange(statusChange.iStatus);
			}
		}
	}
	
/**
*	This is for the NotifyContextStatusChange timer callback
*/
void CSimPacketContext::TimerCallBackNotifyContextStatusChange()
	{
	if(GetContextStatusChangeCount() <= iNotifyContextStatusChangeIndex)
		{
		// the index is out of bounds
		return;
		}
	TNotifyContextStatusChange& statusChange = GetContextStatusChangeAt(iNotifyContextStatusChangeIndex);

	CompleteContextStatusChange(statusChange.iStatus);

	iNotifyContextStatusChangeIndex++;
	if (GetContextStatusChangeCount() > iNotifyContextStatusChangeIndex)
	{
		// start timer again
		iNotifyContextStatusChangeTimer->Start(GetContextStatusChangeAt(iNotifyContextStatusChangeIndex).iDuration,this,ETimerIdPcktContextStatusChange);
		}
	}

void CSimPacketContext::CompleteContextStatusChange(RPacketContext::TContextStatus& aStatus)
		{	
	iCurrentNotifyContextStatusChange = aStatus;

	ChangeState(iCurrentNotifyContextStatusChange);
	if (RPacketContext::EStatusDeleted == iCurrentNotifyContextStatusChange)
		{
		DoDelete();
		}
	}

void CSimPacketContext::NTRasCallBack(TInt aStatus)
/**
 * NTRas callback function.  This function will be called when the NTRAS
 * Server has responded.
 * 
 * @param aStatus	Standard error value, indicating the success or failure of the NTRAS
 *					connection.
 */
	{
		__ASSERT_ALWAYS(ActionEvent(EContextEventNtRas,aStatus) == KErrNone,SimPanic(EGeneral));
		if(iCommReset)
			iNtRas->Terminate();
	}

RPacketService::TStatus CSimPacketContext::ConvertToPacketServiceStatus(RPacketContext::TContextStatus aNewState)
/**
* Converts Packet Context Status (RPacketContext::TContextStatus) to 
*          Packet Service Status (RPacketService::TStatus)
*
* @param aNewState the Context status to convert to packet service status
* @return RPacketService::TStatus The packet service status conversion
*/
	{
	RPacketService::TStatus serviceStatus;
	switch (aNewState)
		{
	case RPacketContext::EStatusInactive:
	case RPacketContext::EStatusActivating:
	case RPacketContext::EStatusDeleted:
		serviceStatus = RPacketService::EStatusAttached;
		break;
	case RPacketContext::EStatusActive:
	case RPacketContext::EStatusDeactivating:
		serviceStatus = RPacketService::EStatusActive;
		break;
	case RPacketContext::EStatusSuspended:
		serviceStatus = RPacketService::EStatusSuspended;
		break;
	default:
		serviceStatus = RPacketService::EStatusUnattached;
		break;
		}
	return serviceStatus;
	}


TInt CSimPacketContext::ChangeState(RPacketContext::TContextStatus aNewState)
/**
* Attempt to change state.
* First validate that the requested state change is ok.  If it is then proceed to change
* the state and complete any pending state change notification.
*
* @param aNewState the new state to change to
* @return Error indication if change of state is successful or not
*/
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_CHANGESTATE_1, ">>CSimPacketContext::ChangeState [newState=%d]", aNewState);
	__ASSERT_ALWAYS(aNewState!=RPacketContext::EStatusUnknown,SimPanic(ECallStatusUnknownIllegal));

	if(iState==aNewState)
		return KErrNone;

	TInt ret=KErrGeneral;
	const struct TContextStateChangeValidity* stateChangePnt=KContextStateChangeValidity;
	while(stateChangePnt->iOldState!=KContextStateTableTerminator)
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
	ret = iPacketService->ChangeState(ConvertToPacketServiceStatus(aNewState));
	if(ret!=KErrNone)
		return ret;

// Actually change the state.
	iState=aNewState;

// Check for a pending line state notification.
	if(iNotifyStatusChange.iNotifyPending)
		{
		iNotifyStatusChange.iNotifyPending=EFalse;
		*(RPacketContext::TContextStatus*)iNotifyStatusChange.iNotifyData=iState;
		ReqCompleted(iNotifyStatusChange.iNotifyHandle,KErrNone);
		}


	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_CHANGESTATE_2, "<<CSimPacketContext::ChangeState");
	return KErrNone;
	}

TInt CSimPacketContext::ActionEvent(TContextEvent aEvent,TInt aStatus)
/**
* Entry point when an event has occured that may advance the state machine.
* The aEvent parameter describes the event.
*
* This function contains the main state machine for the packet Context.  
*
* @param aEvent The Packet Context event to handle
* @return value represents the error state caused by the attempted state machine jump.
*/
	{
	TInt ret=KErrNone;
	__ASSERT_ALWAYS(iState!=RPacketContext::EStatusUnknown,SimPanic(ECallStatusUnknownIllegal));

	TBool found = EFalse;
	TInt i = 0;
	TInt count = iContextConfigsRel99->Count();
	for(i = 0; i<count; i++)
		{
		if(iContextName.Compare(iContextConfigsRel99->At(i).iContextName)==0)
			{
			found = ETrue;
			break;
			}
		}

	switch(aEvent)
		{
		case EContextEventActivate:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_ACTIONEVENT_1, ">>CSimPacketContext::ActionEvent = [EContextEventActivate]");
			if(iState==RPacketContext::EStatusInactive)
				{
				iCurrentEvent=EContextEventActivate;
				ret = ChangeState(RPacketContext::EStatusActivating);
					if(ret!=KErrNone)
						ReqCompleted(iActivateRequestHandle,ret);
					else 
						{
						if(!found)
							iTimer->Start(iActivatePause,this);
						else
							iTimer->Start(iContextConfigsRel99->At(i).iActivatePause,this);
						}
				}
			else
				ReqCompleted(iActivateRequestHandle, KErrGprsActivationRejected);
			break;

		case EContextEventDeactivate:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_ACTIONEVENT_2, ">>CSimPacketContext::ActionEvent = [EContextEventDeactivate]");
			if(iState==RPacketContext::EStatusActive)
				{
				iCurrentEvent=EContextEventDeactivate;
				ret = ChangeState(RPacketContext::EStatusDeactivating);
					if(ret!=KErrNone)
						ReqCompleted(iDeactivateRequestHandle,ret);
					else
						{
						if (!found)
							iTimer->Start(iDeactivatePause,this);
						else
							iTimer->Start(iContextConfigsRel99->At(i).iDeactivatePause,this);
						}
				}
			else
				ReqCompleted(iDeactivateRequestHandle, KErrNone);
			break;

		case EContextEventDelete:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_ACTIONEVENT_3, ">>CSimPacketContext::ActionEvent = [EContextEventDelete]");
			if(iState==RPacketContext::EStatusInactive)
				{
				iCurrentEvent=EContextEventDelete;

				if (!found)
					iTimer->Start(iDeletePause,this);
				else
					iTimer->Start(iContextConfigsRel99->At(i).iDeletionPause,this);
				}
			else
				ReqCompleted(iDeleteRequestHandle, KErrInUse);
			break;

		case EContextEventNtRas:
			if((iState==RPacketContext::EStatusActivating) && (iCurrentEvent==EContextEventNtRas))
				{
				if(aStatus!=KErrNone)
					{
					ReqCompleted(iActivateRequestHandle,aStatus);
					ret=ChangeState(RPacketContext::EStatusInactive);
					__ASSERT_ALWAYS(ret==KErrNone,SimPanic(EIllegalStateInconsistancy));
					return KErrNone;
					}
				else
					{
					ret=ChangeState(RPacketContext::EStatusActive);
					ReqCompleted(iActivateRequestHandle,ret);
					iIsActive = ETrue;
					return KErrNone;
					}
				}
			iCurrentEvent=EContextEventNone;
			break;

		case EContextEventTimeOut:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_ACTIONEVENT_4, ">>CSimPacketContext::ActionEvent = [EContextEventTimeOut]");
			switch(iCurrentEvent)
				{
			case EContextEventNone:
				break;
			case EContextEventActivate:
				if(iState==RPacketContext::EStatusActivating)
					{
						TInt activateValue;
						if (found)
							activateValue = iContextConfigsRel99->At(i).iActivateErrorCode;
						else
							activateValue = iActivateFail;	
						if(activateValue == KContextGoToInactiveWithErrorWhenTryingToGetLastErrorCode)
						    {
						    //this will allow context activation move to the next state but with context being inactive 
						    ret=ChangeState(RPacketContext::EStatusInactive);
						    iCurrentEvent=EContextEventNone;
						    iLastError = activateValue;
						    iErrorCodeForGetLastErrorCause=KErrGeneral;
						    
						    iContextConfigsRel99->At(i).iActivateErrorCode = iLastError;
						    ReqCompleted(iActivateRequestHandle, KErrNone);
						    
						    break;
						    }
						if(activateValue <= KContextGoToInactiveWithExtendedErrorCode && activateValue > KContextGoToInactiveWithNormalErrorCode)
                            {
                            //this will allow context activation move to the next state but with context being inactive     
                            //tsy extended error case
                            ret=ChangeState(RPacketContext::EStatusInactive);
                            iCurrentEvent=EContextEventNone;
                            iLastError = activateValue - KContextGoToInactiveWithExtendedErrorCode;
                            //move the error code to the higher 16 bit.
                            iLastError<<=16;
                            
                            iContextConfigsRel99->At(i).iActivateErrorCode = iLastError;
                            ReqCompleted(iActivateRequestHandle, KErrNone);
                            
                            break;
                            }
						if(activateValue <= KContextGoToInactiveWithNormalErrorCode && activateValue > KLimitForErrorCodeForContextGoToInactive)
						    {
						    //this will allow context activation move to the next state but with context being inactive
						    //normal error case
						    ret=ChangeState(RPacketContext::EStatusInactive);
						    iCurrentEvent=EContextEventNone;
						    iLastError = activateValue - KContextGoToInactiveWithNormalErrorCode;
						    
						    iContextConfigsRel99->At(i).iActivateErrorCode = iLastError;
						    ReqCompleted(iActivateRequestHandle, KErrNone);
						    
						    break;
						    }
						else
						if(activateValue != KErrNone)
							{
							ReqCompleted(iActivateRequestHandle,activateValue);
							ret=ChangeState(RPacketContext::EStatusInactive);
							iCurrentEvent=EContextEventNone;
							activateValue = KErrNone;

							if (found)
								iContextConfigsRel99->At(i).iActivateErrorCode = activateValue;
							else
								iActivateFail = activateValue;		
							break;
							}
						else 
							{
						if (found)
							{
							if (iContextConfigsRel99->At(i).iConnectToNtRas)
								{
								ret = DoActivate();
								if (ret == KErrNone)
									iCurrentEvent=EContextEventNtRas;
								else
									iCurrentEvent=EContextEventNone;
								}
							else
								{
								iCurrentEvent=EContextEventNone;
								ReqCompleted(iActivateRequestHandle,activateValue);
								ret=ChangeState(RPacketContext::EStatusActive);
								iIsActive = ETrue;
								}
							}
						else
							{
							ret = DoActivate();
							if (ret == KErrNone)
								iCurrentEvent=EContextEventNtRas;
							else
								iCurrentEvent=EContextEventNone;
							}
						}
					}
				break;
			case EContextEventDeactivate:
				TInt deActivateValue;
				if (found)
					deActivateValue = iContextConfigsRel99->At(i).iDeactivateErrorCode;
				else
					deActivateValue = iDeactivateFail;	

				if(iState==RPacketContext::EStatusDeactivating)
					{
					if(deActivateValue != KErrNone)
						{
						ReqCompleted(iDeactivateRequestHandle,deActivateValue);
						ret=ChangeState(RPacketContext::EStatusActive);
						deActivateValue = KErrNone;

						if (found)
								iContextConfigsRel99->At(i).iDeactivateErrorCode = deActivateValue;
							else
								iDeactivateFail = deActivateValue;		
						break;
						}
					else {
						ret=ChangeState(RPacketContext::EStatusInactive);
						if(ret==KErrNone)
							{
							ret = DoDeactivate();
							if(ret==KErrNone)
								{	
								ReqCompleted(iDeactivateRequestHandle,deActivateValue);
								iIsActive=EFalse;
								}
							else
								ReqCompleted(iDeactivateRequestHandle,ret);
							}
						else
							ReqCompleted(iDeactivateRequestHandle,ret);
						}
					}
				iCurrentEvent=EContextEventNone;
				break;
			case EContextEventDelete:
				TInt deleteValue;
				if (found)
					deleteValue = iContextConfigsRel99->At(i).iDeletionErrorCode;
				else
					deleteValue = iDeleteFail;	

				if(iState==RPacketContext::EStatusInactive)
					{
					if(deleteValue != KErrNone)
						{
						ReqCompleted(iDeleteRequestHandle,deleteValue);
						deleteValue = KErrNone;
						if (found)
							iContextConfigsRel99->At(i).iDeletionErrorCode = deleteValue;
						else
							iDeleteFail = deleteValue;	
						break;
						}
					else {
						ret=ChangeState(RPacketContext::EStatusDeleted);
						if(ret==KErrNone)
							{
							ret = DoDelete();
							if(ret==KErrNone)
								ReqCompleted(iDeleteRequestHandle,deleteValue);
							else
								ReqCompleted(iDeleteRequestHandle,ret);
							}
						else
							ReqCompleted(iDeleteRequestHandle,ret);
						}
					}
				iCurrentEvent=EContextEventNone;
				break;
			default:	//in case of EContextEventTimeOut
				break;
				}
			break;

		default:
			break;
			}
	return ret;
	}

const CTestConfigSection* CSimPacketContext::CfgFile()
/**
* Return a pointer to the Configuration File Section
*
* @return CTestConfigSection	pointer to the configuration file section
*/
	{
	return iPacketService->CfgFile();
	}

TInt CSimPacketContext::ModifyActiveContext(const TTsyReqHandle aTsyReqHandle)
	{
	if (iState==RPacketContext::EStatusInactive)
		{
		ReqCompleted(aTsyReqHandle, KErrNotReady);
		return KErrNone;
		}

	// check if QoS profile has changed or TFTs have changed
	if (iPacketqos != NULL)
		{
		if (iPacketqos->PerformProfileChange() == KErrNone || iTFTChangeBool)
			{
			iTFTChangeBool = EFalse;
			ReqCompleted(aTsyReqHandle, KErrNone);
			return KErrNone;
			}
		else
			{
			ReqCompleted(aTsyReqHandle, KErrNotReady);
			return KErrNone;
			}
		}
	else if (iTFTChangeBool)
		{
		iTFTChangeBool = EFalse;
		ReqCompleted(aTsyReqHandle, KErrNone);
		return KErrNone;
		}	
	else
		{
		ReqCompleted(aTsyReqHandle, KErrNotReady);
		return KErrNone;
		}
	}

/**
*	This function enumerates the number of packet filters
*/
TInt CSimPacketContext::EnumeratePacketFilters(const TTsyReqHandle aTsyReqHandle, TInt& aCount)
	{
	aCount = iNumFiltersAdded;
	ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}

/**
* This function returns info regarding the packet filter specified by the index passed - 
* used in conjunction with EnumeratePacketFilters()
*/
TInt CSimPacketContext::GetPacketFilterInfoL(const TTsyReqHandle aTsyReqHandle, TInt aIndex, TDes8* aPacketFilterInfo)
	{
	// if index is invalid, return error code
	if (aIndex < 0 || aIndex > (iNumFiltersAdded -1))
		{
		ReqCompleted(aTsyReqHandle, KErrArgument);
		return KErrNone;
		}
	else
		{
		TInt ii;
		TInt addedFilterCount = -1;
		
		//find the requested filter
		for(ii = 0; ii < iPacketFilterInfoArray->Count(); ii++)
			{
			if(iPacketFilterInfoArray->At(ii).iAdded)
				{
				addedFilterCount++;

				//when the filter is found write the data to the client				
				if(addedFilterCount == aIndex)
					{
					TPacketFilterV3 theFilter = iPacketFilterInfoArray->At(ii);

					// Check that the data structure is supported by the simulated TSY version
					TInt err = KErrNone;
					if(iPhone->iSimTsyVersion < 3) // same version as TPacketFilterVx
						{
					 	(void)User::Leave(KErrNotSupported);
						}
										
					RPacketContext::TPacketFilterV2Pckg* clientFilterPckg = 
						(RPacketContext::TPacketFilterV2Pckg*)aPacketFilterInfo; 
					RPacketContext::TPacketFilterV2& clientFilter = (*clientFilterPckg)();

					// Check that the data structure is supported by the simulated TSY version
					err = iPhone->CheckSimTsyVersion(clientFilter);
					if(err != KErrNone)
						{
						iPhone->ReqCompleted(aTsyReqHandle, err);
						return KErrNone;
						}
									
					if(clientFilter.ExtensionId()==KETelExtPcktV2)
						{
						clientFilter.iDestPortMax = theFilter.iDestPortMax;
						clientFilter.iDestPortMin = theFilter.iDestPortMin;
						clientFilter.iEvaluationPrecedenceIndex = theFilter.iEvaluationPrecedenceIndex;
						clientFilter.iFlowLabel = theFilter.iFlowLabel;
						clientFilter.iId = theFilter.iId;
						clientFilter.iIPSecSPI = theFilter.iIPSecSPI;
						clientFilter.iProtocolNumberOrNextHeader = theFilter.iProtocolNumberOrNextHeader;
						clientFilter.iSrcPortMax = theFilter.iSrcPortMax;
						clientFilter.iSrcPortMin = theFilter.iSrcPortMin;
						clientFilter.iTOSorTrafficClass = theFilter.iTOSorTrafficClass;
						
						TInt xx;
						for(xx = 0;  xx<RPacketContext::KIPAddressSize; xx++)
							{
							clientFilter.iSrcAddr[xx] = theFilter.iSrcAddr[xx];
							clientFilter.iSrcAddrSubnetMask[xx] = theFilter.iSrcAddrSubnetMask[xx];
							}
						ReqCompleted(aTsyReqHandle, KErrNone);
						return KErrNone;
						}
					else if((clientFilter.ExtensionId()==KETelExtPcktV3) &&
							theFilter.iFilterType != RPacketContext::EPacketFilterTypeUnknown)
						{
						RPacketContext::TPacketFilterV3Pckg* clientFilterV3Pckg = 
							(RPacketContext::TPacketFilterV3Pckg*)aPacketFilterInfo; 
						RPacketContext::TPacketFilterV3& clientFilterV3 = (*clientFilterV3Pckg)();
						
						// Check that the data structure is supported by the simulated TSY version
						TInt err = iPhone->CheckSimTsyVersion(clientFilterV3);
						if(err != KErrNone)
							{
							iPhone->ReqCompleted(aTsyReqHandle, err);
							return KErrNone;
							}

						RPacketContext::TPacketFilterV3 filterV3(theFilter.iFilterType);

						// Check that the data structure is supported by the simulated TSY version
						err = iPhone->CheckSimTsyVersion(filterV3);
						if(err != KErrNone)
							{
							iPhone->ReqCompleted(aTsyReqHandle, err);
							return KErrNone;
							}

						filterV3.iId = theFilter.iId;
						filterV3.iEvaluationPrecedenceIndex = theFilter.iEvaluationPrecedenceIndex;
						filterV3.SetTOSorTrafficClass(theFilter.iTOSorTrafficClass);

						TInt yy;
						for(yy = 0;  yy<RPacketContext::KIPAddressSize; yy++)
							{
							filterV3.SetSrcAddr(theFilter.iSrcAddr);
							filterV3.SetSrcAddrSubnetMask(theFilter.iSrcAddrSubnetMask);
							}
						if((theFilter.iFilterType == RPacketContext::EPacketFilterTypeOne) ||
							(theFilter.iFilterType == RPacketContext::EPacketFilterTypeTwo))
							{
							filterV3.SetProtocolNumberOrNextHeader(theFilter.iProtocolNumberOrNextHeader);
							}
						
						if(theFilter.iFilterType == RPacketContext::EPacketFilterTypeOne)
							{
							filterV3.SetDestPortMin(theFilter.iDestPortMin);
							filterV3.SetDestPortMax(theFilter.iDestPortMax);
							filterV3.SetSrcPortMin(theFilter.iSrcPortMin);
							filterV3.SetSrcPortMax(theFilter.iSrcPortMax);
							}

						if((theFilter.iFilterType == RPacketContext::EPacketFilterTypeTwo))
							{
							filterV3.SetSPI(theFilter.iIPSecSPI);
							}
						
						if((theFilter.iFilterType == RPacketContext::EPacketFilterTypeThree))
							{
							filterV3.SetFlowLabel(theFilter.iFlowLabel);
							}
						
						clientFilterV3 = filterV3;
						ReqCompleted(aTsyReqHandle, KErrNone);
						return KErrNone;
						}
					else
						{
						//cannot convert a filterV2 in the TSY to the filterV3 data typed passed in by client
						ReqCompleted(aTsyReqHandle, KErrCorrupt);
						return KErrNone;
						}
					}//end if(addedFilterCount == aIndex)
				}//end if(iPacketFilterInfoArray->At(ii).iAdded)
			else
				{
				continue;
				}
				
			}//end for loop
		//filter was not found
		ReqCompleted(aTsyReqHandle, KErrCorrupt);
		return KErrNone;
		}//end else
	}

/**
*	Create TFT
*/
TInt CSimPacketContext::CreateNewTFT(const TTsyReqHandle aTsyReqHandle, const TInt* aSize)
	{
	if (iCreateTftConfig.iNumberOfFailures > 0)
		{
		iCreateTftConfig.iNumberOfFailures--;
		ReqCompleted(aTsyReqHandle, iCreateTftConfig.iFailErrorCode);
		return KErrNone;
		}
	iCreateTftTimer->Start(iCreateTftConfig.iDelay, this, ETimerIdPcktContextCreateTft);
	iCreateTftData.iTsyReqHandle = aTsyReqHandle;
	iCreateTftData.iSize = *aSize;
	return KErrNone;
	}

TInt CSimPacketContext::DoCreateNewTFT()
	{
	iTFTCreated = ETrue;
	iNumTFTsCreated = iCreateTftData.iSize;
	iTFTChangeBool = ETrue;
	for (TInt i = 0; i < iPacketFilterInfoArray->Count(); i++)
		{
		iPacketFilterInfoArray->At(i).iAdded = EFalse;
		}
	iNumFiltersAdded = 0;
	ReqCompleted(iCreateTftData.iTsyReqHandle, KErrNone);
	return KErrNone;
	}

/**
*	Delete TFT
*/
TInt CSimPacketContext::DeleteTFT(const TTsyReqHandle aTsyReqHandle)
	{
	if (iDeleteTftFail.iNumberOfTimes > 0)
		{
		iDeleteTftFail.iNumberOfTimes--;
		ReqCompleted(aTsyReqHandle, iDeleteTftFail.iFailErrorCode);
		return KErrNone;
		}
	iTFTCreated = EFalse;
	iNumTFTsCreated = -1;
	iTFTChangeBool = ETrue;
	for (TInt i = 0; i < iPacketFilterInfoArray->Count(); i++)
		{
		iPacketFilterInfoArray->At(i).iAdded = EFalse;
		}
	iNumFiltersAdded = 0;
	ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}

/**
*	This method adds the packet filter to the array if it is not already there
*/
TInt CSimPacketContext::AddPacketFilter(const TTsyReqHandle aTsyReqHandle, const TDesC8* aPacketFilterInfo)
	{
	if (iAddPacketFilter.iNumberOfTimes > 0)
		{
		iAddPacketFilter.iNumberOfTimes--;
		ReqCompleted(aTsyReqHandle, iAddPacketFilter.iFailErrorCode);
		return KErrNone;
		}

	TPckg<RPacketContext::TPacketFilterV2>* packetFilter = (TPckg<RPacketContext::TPacketFilterV2>*)aPacketFilterInfo;
	RPacketContext::TPacketFilterV2& packetFilterBase = (*packetFilter)();

	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(packetFilterBase);
	if(err != KErrNone)
		{
		iPhone->ReqCompleted(aTsyReqHandle, err);
		return KErrNone;
		}

	TBool found = EFalse;
	
	TInt ii;
	for(ii = 0; ii<iPacketFilterInfoArray->Count(); ii++)
		{
		TPacketFilterV3& configFilter = iPacketFilterInfoArray->At(ii);
		const TInt KReservedPortZero = 0;
		
		if((configFilter.iId == packetFilterBase.iId) &&
			(configFilter.iEvaluationPrecedenceIndex == packetFilterBase.iEvaluationPrecedenceIndex) &&
			(configFilter.iProtocolNumberOrNextHeader == packetFilterBase.iProtocolNumberOrNextHeader) &&
			(configFilter.iSrcPortMin == packetFilterBase.iSrcPortMin) &&
			(configFilter.iSrcPortMax == packetFilterBase.iSrcPortMax) &&
			((configFilter.iDestPortMin == KReservedPortZero) || (configFilter.iDestPortMin == packetFilterBase.iDestPortMin)) &&
			((configFilter.iDestPortMax == KReservedPortZero) || (configFilter.iDestPortMax == packetFilterBase.iDestPortMax)) &&
			(configFilter.iIPSecSPI == packetFilterBase.iIPSecSPI) &&
			(configFilter.iTOSorTrafficClass == packetFilterBase.iTOSorTrafficClass) &&
			(configFilter.iFlowLabel == packetFilterBase.iFlowLabel))
			{
			TInt xx;
			for(xx = 0;  xx<RPacketContext::KIPAddressSize; xx++)
				{
				if((configFilter.iSrcAddr[xx] == packetFilterBase.iSrcAddr[xx]) &&
				  (configFilter.iSrcAddrSubnetMask[xx] == packetFilterBase.iSrcAddrSubnetMask[xx]))
					{
					found=ETrue;
					}
				else
					{
					found=EFalse;
					break;
					}
				}

			if(found)
				{
			    // checking the maximum number of packet filter per a pdp context
                if(iNumFiltersAdded >= KMaxNumberOfTftFilters )
                    {
                    ReqCompleted(aTsyReqHandle, KErrOverflow);
                    return KErrNone;
                    }
				iTFTChangeBool = ETrue;
				configFilter.iAdded = ETrue;
				iNumFiltersAdded++ ;
				ReqCompleted(aTsyReqHandle, KErrNone);
				return KErrNone;
				}
			}
		}
		ReqCompleted(aTsyReqHandle, KErrGeneral);
		return KErrNone;
	}

/**
*	This function removes the packet filter from the array if the packet filter is found in the array
*/
TInt CSimPacketContext::RemovePacketFilter(const TTsyReqHandle aTsyReqHandle, TInt aId)
	{
	TInt total= iPacketFilterInfoArray->Count();
	TBool found = EFalse;
	
	for (TInt i = 0; i<total; i++)
		{
		if ((iPacketFilterInfoArray->At(i).iId == aId) &&
			(iPacketFilterInfoArray->At(i).iAdded))
			{
			iPacketFilterInfoArray->At(i).iAdded = EFalse;
			iNumFiltersAdded-- ;
			ReqCompleted(aTsyReqHandle, KErrNone);
			found = ETrue;
			iTFTChangeBool = ETrue;
			break;
			}
		}

	if (!found)
		{
		ReqCompleted(aTsyReqHandle, KErrArgument);
		return KErrNone;
		}
	return KErrNone;
	}

/**
*	This function initialises the context
*/
TInt CSimPacketContext::InitialiseContext(const TTsyReqHandle aTsyReqHandle, TDes8* aDataChannelV2Pckg)
	{
	TCommSetupItem item = FindCommSettings();
	if (item.iNumInitializeFailures != 0)
		{
		item.iNumInitializeFailures--;
		ReqCompleted(aTsyReqHandle, item.iErrorCode);
		}
	else
		{
		iInitContextData.iTsyReqHandle = aTsyReqHandle;
		iInitContextData.iChannel = aDataChannelV2Pckg;
		iInitContextTimer->Start(item.iDelay, this, ETimerIdPcktContextInitContext);
		}
	return KErrNone;
	}
	
TInt CSimPacketContext::DoInitialiseContext()
	{
	RPacketContext::TDataChannelV2Pckg* channel = (RPacketContext::TDataChannelV2Pckg*)iInitContextData.iChannel;
	RPacketContext::TDataChannelV2& channelV2 = (*channel)();

	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(channelV2);
	if(err != KErrNone)
		{
		ReqCompleted(iInitContextData.iTsyReqHandle, err);
		return KErrNone;
		}

	TCommSetupItem item = FindCommSettings();
	channelV2.iPort = item.iPortName;
	channelV2.iCsy = item.iCsyName;
	channelV2.iChannelId = item.iPortName;
	
	ReqCompleted(iInitContextData.iTsyReqHandle, KErrNone);

	return KErrNone;
	}

/**
* Cancel method, returns KErrNone
*/
TInt CSimPacketContext::EnumeratePacketFiltersCancel(const TTsyReqHandle /*aTsyReqHandle*/)
	{
	return KErrNone;
	}

/**
* Cancel method, returns KErrNone
*/
TInt CSimPacketContext::GetPacketFilterInfoCancel(const TTsyReqHandle /*aTsyReqHandle*/)
	{
	return KErrNone;
	}

/**
* Cancel method, returns KErrNone
*/
TInt CSimPacketContext::AddPacketFilterCancel(const TTsyReqHandle /*aTsyReqHandle*/)
	{
	return KErrNone;
	}

/**
* Cancel method, returns KErrNone
*/	
TInt CSimPacketContext::CreateNewTFTCancel(const TTsyReqHandle aTsyReqHandle)
	{
	iCreateTftTimer->Cancel();
	
	ReqCompleted(aTsyReqHandle, KErrCancel);
	
	return KErrNone;
	}

/**
* Cancel method, returns KErrNone
*/
TInt CSimPacketContext::DeleteTFTCancel(const TTsyReqHandle /*aTsyReqHandle*/)
	{
	return KErrNone;
	}

/**
* Cancel method, returns KErrNone
*/	
TInt CSimPacketContext::RemovePacketFilterCancel(const TTsyReqHandle /*aTsyReqHandle*/)
	{
	return KErrNone;
	}

/**
* Cancel method, returns KErrNone
*/
TInt CSimPacketContext::ModifyActiveContextCancel(const TTsyReqHandle /*aTsyReqHandle*/)
	{
	return KErrNone;
	}

/**
* Cancel method, returns KErrNone
*/
TInt CSimPacketContext::InitialiseContextCancel(const TTsyReqHandle aTsyReqHandle)
	{
	iInitContextTimer->Cancel();
	
	ReqCompleted(aTsyReqHandle, KErrCancel);
	
	return KErrNone;
	}

CSimPacketContext::TCommSetupItem CSimPacketContext::FindCommSettings()
	{
	TInt count=iCommSetupArray->Count();
	TCommSetupItem def;

	//Do a default initialization.
	def.iCsyName.Copy(KDefaultCsyName);
	def.iPortName.Copy(KDefaultPortName);
	def.iConfig.iRate=KDefaultCommPortRate;
	def.iConfig.iHandshake=KDefaultHandshake;	
	def.iContextName=KDefaultSetting;
	def.iCommReset = EFalse;
	def.iNumInitializeFailures = 0;
	def.iErrorCode = KErrNone;
	def.iDelay = 0;

	for(TInt i=0; i<count; i++)
		{
		TCommSetupItem comm = iCommSetupArray->At(i);
		if(comm.iContextName.Compare(iContextName)==KErrNone)
			return comm;
		if(comm.iContextName.Compare(KDefaultSetting)==KErrNone)
			def=comm;
		}
	return def;
	}

TName CSimPacketContext::ContextName() const 
	{
	return iContextName; 
	}

TInt CSimPacketContext::GetDnsInfo(const TTsyReqHandle aTsyReqHandle, const TDesC8* aDnsInfo)
	{
/**
 * GetDnsInfo method to extract DNS server names from context
 *
 * @param aStatus TRequestStatus object with which error code is returned
 * @param aDnsInfo packaged TDnsInfoV2 object passed as TDes8
 *
 * GetDNSInfo() is needed when the client uses dynamic IPv4 PDP addresses. 
 * After the context is activated the client needs to know the actual primary 
 * and secondary DNS addresses to use when sending packet data. As the network 
 * may have supplied these addresses there is a need for an API method to get 
 * these addresses from the TSY.
 */
	RPacketContext::TTDnsInfoV2Pckg* dnsInfoV2Pckg = (RPacketContext::TTDnsInfoV2Pckg*)aDnsInfo;
	RPacketContext::TDnsInfoV2& dnsInfoV2 = (*dnsInfoV2Pckg)();

	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(dnsInfoV2);
	if(err != KErrNone)
		{
		iPhone->ReqCompleted(aTsyReqHandle, err);
		return KErrNone;
		}

	TContextConfigParam config = iContextConfigParams->At(iContextConfigParamsIndex);

	dnsInfoV2.iPrimaryDns.Copy(config.iProtocolConfigOption.iPrimaryDNS);
	dnsInfoV2.iSecondaryDns.Copy(config.iProtocolConfigOption.iSecondaryDNS);

	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimPacketContext::GetDnsInfoCancel(const TTsyReqHandle /*aTsyReqHandle*/) 
/**
 * GetDnsInfoCancel - Cancels an asynchronous GetDnsInfo method.
 */
	{
	return KErrNone;
	}

TInt CSimPacketContext::GetAddMediaAuthorization(const TTsyReqHandle aTsyReqHandle, TDes8* aBuffer)
/**
 * GetAddMediaAuthorization method to verify provided media authorization and return desired result
 *
 * @param aTsyReqHandle The request handle for completing the request
 * @param aBuffer Packaged CTFTMediaAuthorizationV3 object passed as TDes8
 *
 * GetAddMediaAuthorization() checks the provided media authorization against a predefined authorization
 * set up from the configuration file. If the authorization is valid the function will return the desired
 * response, also set up from the configuration file. If there is a mismatch between authorizations, KErrCorrupt
 * returned.
 */ 	
	{
	TBool validMediaAuthorization = TRUE;
	RPacketContext::CTFTMediaAuthorizationV3* mediaAuthV3 = NULL;

	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(*mediaAuthV3);
	if(err != KErrNone)
		{
		iPhone->ReqCompleted(aTsyReqHandle, err);
		return KErrNone;
		}
	
	TRAPD(ret, mediaAuthV3 = RPacketContext::CTFTMediaAuthorizationV3::NewL());
	if (ret != KErrNone)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETADDMEDIAAUTHORIZATION_1, "CSimPacketContext::GetAddMediaAuthorization NewL call fail");
		return ret;
		}		

	TRAP(ret, mediaAuthV3->InternalizeL(*aBuffer) );
	if (ret != KErrNone)
		{
		delete mediaAuthV3;
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPACKETCONTEXT_GETADDMEDIAAUTHORIZATION_2, "CSimPacketContext::GetAddMediaAuthorization InternalizeL call fail");
		return ret;
		}

	for (TUint i=0; i<iAuthorizationTokenResponse->Count(); i++)
		{
		if (!(mediaAuthV3->iAuthorizationToken).Compare(iAuthorizationTokenResponse->At(i).iAuthorizationToken))
			{
			for (TUint j=0; j<iMediaAuthorization->At(i).iFlowIds.Count(); j++)
				{
				if ((mediaAuthV3->iFlowIds[j].iMediaComponentNumber != iMediaAuthorization->At(i).iFlowIds[j].iMediaComponentNumber) ||
					(mediaAuthV3->iFlowIds[j].iIPFlowNumber != iMediaAuthorization->At(i).iFlowIds[j].iIPFlowNumber))
					{
					validMediaAuthorization = FALSE;
					}
				}
			if (validMediaAuthorization)
				{
				ReqCompleted(aTsyReqHandle,iAuthorizationTokenResponse->At(i).iResponse);
				delete mediaAuthV3;
				return KErrNone;
				}
			}
		}
		
	// No match found, respond with corrupted		
	ReqCompleted(aTsyReqHandle,KErrCorrupt);
	delete mediaAuthV3;	

	return KErrNone;
 	}

TInt CSimPacketContext::GetRemoveMediaAuthorization(const TTsyReqHandle aTsyReqHandle, RPacketContext::TAuthorizationToken* aAuthorizationToken)
/**
 * GetRemoveMediaAuthorization method to verify provided media authorization and return desired result
 *
 * @param aTsyReqHandle The request handle for completing the request
 * @param aAuthorizationToken Authorization token to be checked
 *
 * GetRemoveMediaAuthorization() checks the provided authorization token against predefined tokens and responses 
 * from config file. If a match is found the associated desired response is returned, otherwise KErrCorrupt is reported.
 */ 	
	{	
	for (TUint i=0; i<iAuthorizationTokenResponse->Count(); i++)
		{
		if (!((*aAuthorizationToken).Compare(iAuthorizationTokenResponse->At(i).iAuthorizationToken)))
			{
			ReqCompleted(aTsyReqHandle,iAuthorizationTokenResponse->At(i).iResponse);
			return KErrNone;
			}
		}
		
	// No match found, respond with corrupted		
	ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CSimPacketContext::GetAddMediaAuthorizationCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
* Cancel method, returns KErrNone
*/
	{
	return KErrNone;
	}

TInt CSimPacketContext::GetRemoveMediaAuthorizationCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
* Cancel method, returns KErrNone
*/
	{
	return KErrNone;	
	}


