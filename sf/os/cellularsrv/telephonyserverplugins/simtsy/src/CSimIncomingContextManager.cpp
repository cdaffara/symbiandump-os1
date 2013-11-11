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
#include "CSimIncomingContextManagerTraces.h"
#endif

#include "CSimIncomingContextManager.h"

#include <testconfigfileparser.h>

#include "SimConstants.h"
#include "SimTsy.h"
#include "CSimPhone.h"
#include "csimtimer.h"
#include "CSimPacketService.h"


#include "CSimContextHelper.h"


CSimIncomingContextManager::CSimIncomingContextManager(CSimPhone* aPhone) :
	iPhone(aPhone)
	{
		
	}

CSimIncomingContextManager::~CSimIncomingContextManager()
	{
	
	delete iForceIncomingContextChange;
		
	if (iTimer != NULL)
		{
		iTimer->Cancel();		
		delete iTimer;
		}
		
	if (iIncomingContextConfigParams != NULL)
		{
		iIncomingContextConfigParams->Delete(0, iIncomingContextConfigParams->Count());
		delete iIncomingContextConfigParams;
		}

	if (iDelayIncomingContext != NULL)
		{
		iDelayIncomingContext->Delete(0, iDelayIncomingContext->Count());
		delete iDelayIncomingContext;
		}
	}

CSimIncomingContextManager* CSimIncomingContextManager::NewL(CSimPhone* aPhone, CSimPacketService* aSimPacketService)
	{
	CSimIncomingContextManager* delayIncomingContextManager=new(ELeave) CSimIncomingContextManager(aPhone);
	CleanupStack::PushL(delayIncomingContextManager);
	delayIncomingContextManager->ConstructL(aSimPacketService);
	CleanupStack::Pop();
	return delayIncomingContextManager;	
	}



void CSimIncomingContextManager::ConstructL(CSimPacketService* aSimPacketService)
/**
* Second phase of the 2-phase constructor.
* Constructs all the member data and retrieves all the data from the config file specific to this class.
*
* @leave Leaves no memory or any data member does not construct for any reason.
*/
	{	
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMINCOMINGCONTEXTMANAGER_CONSTRUCTL_1, "CSimIncomingContextManager: Entered ConstructL()");
	
	iSimPacketService = aSimPacketService;
		
	iDelayIncomingContext=new(ELeave) CArrayFixFlat<TDelayIncomingContext>(KMaxNumberofConfigsChanges);
	iCurrentDelayIndex=0;

	iTimer=CSimTimer::NewL(iPhone);

	iIncomingContextConfigParams = new (ELeave) CArrayFixFlat<TContextConfigParam>(KNumberofConfigs);

	CSimContextHelper::GetContextConfigParamSettingsL( CfgFile(), KSetContextConfigGPRS(), 
														   iIncomingContextConfigParams );
	CSimContextHelper::GetContextConfigParamSettingsL( CfgFile(), KSetContextConfigRel99(), 
														   iIncomingContextConfigParams );
	CSimContextHelper::GetContextConfigParamSettingsL( CfgFile(), KSetContextConfigRel5(), 
														   iIncomingContextConfigParams );

	iForceIncomingContextChangeProperty=CSimPubSub::TPubSubProperty(KUidPSSimTsyCategory, KPSSimTsyForceIncomingContextActivation, KPSSimTsyForceIncomingContextActivationType);
	iForceIncomingContextChange = CSimPubSub::NewL(iSimPacketService, iForceIncomingContextChangeProperty);
	iForceIncomingContextChange->Start();
	}


/**
* Read in the incomming PDP delay contexts.
* The format of the command is 
* DelayIncomingContext = Type , Index, Delay
* Type can be 
*   TPacketDataConfigBase::KConfigGPRS= 0x01  
 
*   TPacketDataConfigBase::KConfigRel99Rel4= 0x03
*   TPacketDataConfigBase::KConfigRel5= 0x04  
*
* Index
*	This the index for the ContextPDP.
*   All the ContextPDP are grouped by type (i.e. GPRS.)
*   So 
*     DelayIncomingContext = 1 , 2, 20
*   points to the PDP context which is GPRS (1), and 2 in the list.
* Delay
*   How long before the simulated incoming PDP becomes active.
*
* Stick to general theme of error detection of just make a note and keep going.
* Note only GPRS has been currently tested.
*/

void CSimIncomingContextManager::LoadIncomingContextsL( TPtrC8 aTag )
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMINCOMINGCONTEXTMANAGER_LOADINCOMINGCONTEXTSL_1, "CSimIncomingContextManager: Entered LoadIncomingContextsL()");
		
	TDelayIncomingContext incomingContext;
	TInt error=KErrNone;
	
	TInt count = CfgFile()->ItemCount(aTag);
	
	for(TInt i=0;i<count;i++)
		{
		const CTestConfigItem* item = CfgFile()->Item(aTag,i);
		if(!item)
			{
			error = KErrArgument;	
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMINCOMINGCONTEXTMANAGER_LOADINCOMINGCONTEXTSL_2, "WARNING - CONFIGURATION FILE PARSING - Reading element DELAYINCOMINGCONTEXT::NO PARAMETERS IN TAG returned %d (element no. %d) from tag %s.",error,0,aTag);
			break;
			}
		
		GetConfigType( (*item), 0, incomingContext.iExtensionId, error );
		if (error)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMINCOMINGCONTEXTMANAGER_LOADINCOMINGCONTEXTSL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element DELAYINCOMINGCONTEXT::CONFIGTYPE returned %d (element no. %d) from tag %s.",error,0,aTag);
			}
		GetContextIndex( (*item), 1, incomingContext.iIndex, error );
		if (error)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMINCOMINGCONTEXTMANAGER_LOADINCOMINGCONTEXTSL_4, "WARNING - CONFIGURATION FILE PARSING - Reading element DELAYINCOMINGCONTEXT::INDEX returned %d (element no. %d) from tag %s.",error,0,aTag);
			}
		GetDelay( (*item),  2, incomingContext.iDelay, error );
		if (error)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMINCOMINGCONTEXTMANAGER_LOADINCOMINGCONTEXTSL_5, "WARNING - CONFIGURATION FILE PARSING - Reading element DELAYINCOMINGCONTEXT::DELAY returned %d (element no. %d) from tag %s.",error,0,aTag);
			}
			
		iDelayIncomingContext->AppendL(incomingContext);
						
		}		
	
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMINCOMINGCONTEXTMANAGER_LOADINCOMINGCONTEXTSL_6, "Finished parsing DelayIncomingContext config parameters...%d items found",count);
	}


void CSimIncomingContextManager::StartFirstIncomingEvent( )
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMINCOMINGCONTEXTMANAGER_STARTFIRSTINCOMINGEVENT_1, "CSimIncomingContextManager: Entered StartFirstIncomingEvent()" );
				
	if(iDelayIncomingContext->Count()!=0)
		{
		iCurrentDelayIndex = 0;				
		const TDelayIncomingContext& delayIncomingContext = iDelayIncomingContext->At(0);
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMINCOMINGCONTEXTMANAGER_STARTFIRSTINCOMINGEVENT_2, "CSimIncomingContextManager: Entered StartFirstIncomingEvent() delay = %d", delayIncomingContext.iDelay);
		iTimer->Start(delayIncomingContext.iDelay, iSimPacketService, ETimerIdContextActivationRequestedChange);
		}	
	}

		
// This sets up the aPckg according to the current delay index.
// And then sets the index	
void CSimIncomingContextManager::NextIncomingEvent( TDes8* aPckg )
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMINCOMINGCONTEXTMANAGER_NEXTINCOMINGEVENT_1, "CSimIncomingContextManager: Entered NextIncomingEvent()" );
	
	if (iDelayIncomingContext->Count()==0)
		{ // We have no incoming events. This is an error.
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMINCOMINGCONTEXTMANAGER_NEXTINCOMINGEVENT_2, "CSimIncomingContextManager: Entered NextIncomingEvent() No events at all!" );
		SimPanic(EGeneral);		
		return;
		}
	if(iCurrentDelayIndex >= iDelayIncomingContext->Count())
		{ // No more incoming contexts. .
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMINCOMINGCONTEXTMANAGER_NEXTINCOMINGEVENT_3, "CSimIncomingContextManager: Entered NextIncomingEvent() No next event" );
		return;		
		}
	// else we have an incoming context.

	const TDelayIncomingContext* delayIncomingContext = 
		&(iDelayIncomingContext->At(iCurrentDelayIndex));

	// Lets deal with the incoming first.	
	const TContextConfigParam* context = FindContext( iIncomingContextConfigParams, 
		delayIncomingContext->iExtensionId, delayIncomingContext->iIndex );
	if ( !context )
		{ // Unable to find a valid pre allocated context that we can use.
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMINCOMINGCONTEXTMANAGER_NEXTINCOMINGEVENT_4, "CSimIncomingContextManager: Entered NextIncomingEvent() failed unable to identify a valid context." );
		SimPanic(EGeneral);		
	  	return ;
		}

	if ( GetContextInCorrectFormatForEtel(context, aPckg) ) 
		{ // Okay got data. This will be sent to ETEL to activate the context.
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMINCOMINGCONTEXTMANAGER_NEXTINCOMINGEVENT_5, "CSimIncomingContextManager: Entered NextIncomingEventt() delay = %d", delayIncomingContext->iDelay);
		}
	else
		{ // Error. Unable to format the data.
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMINCOMINGCONTEXTMANAGER_NEXTINCOMINGEVENT_6, "CSimIncomingContextManager: Entered NextIncomingEvent() failed unable format data for choosn context" );
		SimPanic(EGeneral);		
		return;
		}
	
	// Set up for next event.	
	if(iCurrentDelayIndex < iDelayIncomingContext->Count())
		{
		iCurrentDelayIndex++;		
		}
			
	}
	
	
void CSimIncomingContextManager::Cancel( )
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMINCOMINGCONTEXTMANAGER_CANCEL_1, "CSimIncomingContextManager::Cancel called");
	iTimer->Cancel();	
	}
	
	
void CSimIncomingContextManager::ForcedIncoming(TInt aIndex, TDes8* aPckg )
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMINCOMINGCONTEXTMANAGER_FORCEDINCOMING_1, "CSimIncomingContextManager::ForcedIncoming called");

	if(iDelayIncomingContext->Count()<=aIndex )
		{ 
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMINCOMINGCONTEXTMANAGER_FORCEDINCOMING_2, "CSimIncomingContextManager::ForcedIncoming invalid context");
		return;		
		}
	// else we have a valid incoming context.

	const TDelayIncomingContext* delayIncomingContext = 
		&(iDelayIncomingContext->At(aIndex));
		// Note we will do use (type, index) to detmine which context to use.
		// We would have difficultly with pdp context index, as they grouped by type.
		// This is consistant with how we do it for timers.

	const TContextConfigParam* context = FindContext( iIncomingContextConfigParams, 
		delayIncomingContext->iExtensionId, delayIncomingContext->iIndex );

	if ( !context )
		{ // Unable to find a valid pre allocated context that we can use.
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMINCOMINGCONTEXTMANAGER_FORCEDINCOMING_3, "CSimIncomingContextManager::ForcedIncoming invalid type, index leading to invalid context");
		SimPanic(EGeneral);
	  	return ;
		}

	if ( GetContextInCorrectFormatForEtel(context, aPckg) ) 
		{ // Okay got data. This will be sent to ETEL to activate the context.
		}
	else
		{ // Error. Unable to format the data.		
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMINCOMINGCONTEXTMANAGER_FORCEDINCOMING_4, "CSimIncomingContextManager: Entered NextIncomingEvent() failed unable format data for choosn context" );
		SimPanic(EGeneral);		
		return;
		}

	iTimer->Cancel();
		// No more timer.
							
	}

TBool CSimIncomingContextManager::IsForcedIncoming(const CSimPubSub::TPubSubProperty aProperty )
	{
		TBool ret = (aProperty == iForceIncomingContextChangeProperty);
		return ret;
	}
	
	
void CSimIncomingContextManager::GetConfigType( const CTestConfigItem& aItem , 
	TInt aIndex, TInt& aType, TInt& aError )
	{
	
	aError = KErrNone;
	aType = TPacketDataConfigBase::KConfigGPRS;
		// Have a default value even if error.

	
	TInt ret=CTestConfig::GetElement(aItem.Value(),KStdDelimiter,aIndex,aType);
	if(ret!=KErrNone)
		{
		aError=KErrArgument;
		}
	else
		{
			if ( (1 <= aType) && ( aType <= TPacketDataConfigBase::KConfigRel5 ) )
			{ //Okay we have a valid type.
			}
			else
			{	// Else not one of the recognised types.
				aError=KErrOverflow;
			}		
		}
	}



void CSimIncomingContextManager::GetContextIndex( const CTestConfigItem& aItem, 
	TInt aIndex, TInt& aContextIndex, TInt& aError )
	{

	aError = KErrNone;
	aContextIndex = 0;
		
	TInt ret=CTestConfig::GetElement(aItem.Value(),KStdDelimiter,aIndex,aContextIndex);
	if(ret!=KErrNone)
		{
		aError=KErrArgument;
		}		
	}



void CSimIncomingContextManager::GetDelay( const CTestConfigItem& aItem, 
	TInt aIndex, TInt& aDelay, TInt& aError )
	{

	aError = KErrNone;
	aDelay = 1;
	
	TInt ret=CTestConfig::GetElement(aItem.Value(),KStdDelimiter,aIndex,aDelay);
	if(ret!=KErrNone)
		{
		aError=KErrArgument;
		}
		
	}
	
	
	
const TContextConfigParam* CSimIncomingContextManager::FindContext( const CArrayFixFlat<TContextConfigParam>* aIncomingContextConfigParams, 
		const TInt aExtensionId, const TInt aIndex )
	{
	__ASSERT_ALWAYS(aIncomingContextConfigParams, SimPanic(EIllegalNullPtrParameter));

	const TContextConfigParam* ret=NULL;
	TInt incomingConfigIndex=0;
	
	for(TInt index=0; index < aIncomingContextConfigParams->Count(); index++)
		{
		const TContextConfigParam& thisConfig = aIncomingContextConfigParams->At(index);
		
		if ( thisConfig.iProtocolType == aExtensionId )
			{
			if ( aIndex==incomingConfigIndex )
				{ // We have found the context.
				ret=&(thisConfig);
				break;
				}
				
			incomingConfigIndex++;
				// Only gets incremented for aExtensionId ids we are looking for.
			}		
		}
		// if ret = 0 then we could not find the context.
		// This may or may not be an error, depending on usage.
									
	return ret;
	}
	


TBool CSimIncomingContextManager::GetContextInCorrectFormatForEtel(const TContextConfigParam* aContextConfig,
				TDes8* aData)
	{
		
		if (!aContextConfig)	
			{ // Has no context.
			return EFalse;
			}
		else if (aContextConfig->iProtocolType == TPacketDataConfigBase::KConfigGPRS)
			{			
				TPckg<RPacketContext::TContextConfigGPRS>& contextConfigGPRSPckg = *( (TPckg<RPacketContext::TContextConfigGPRS>*)aData );
				RPacketContext::TContextConfigGPRS& contextConfigGPRS = (contextConfigGPRSPckg)();
				CSimContextHelper::ConvertConfigParams( (*aContextConfig), contextConfigGPRS );
				return ETrue;
			}
			
		#if 0	// Note some may be in a totally different table.
		else if (aContextConfig->iProtocolType == TPacketDataConfigBase::KConfigRel99Rel4)
			{
			
				//ConvertConfigParams(const TContextConfigParam& aInput, RPacketContext::TContextConfigGPRS& aOutput);
				return ETrue;
			}
		else if (aContextConfig->iProtocolType == TPacketDataConfigBase::KConfigRel5)
			{
			
				//ConvertConfigParams(const TContextConfigParam& aInput, RPacketContext::TContextConfigGPRS& aOutput);
				return ETrue;
			}
		#endif 
					
		return EFalse;
	}
			


	
/**
 * Returns a pointer to the current configuration file section.
 *
 * @return CTestConfigSection	A pointer to the current configuration file section.
 */
const CTestConfigSection* CSimIncomingContextManager::CfgFile()
	{
	return iPhone->CfgFile();
	}

