/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:
*
*/

#include "T_DataSdpAgent.h"
#include "T_BTDevAddrData.h"
#include "T_DataCSdpSearchPattern.h"
#include "T_DataSdpAttrIdMatchList.h"
#include "T_BTUtil.h"
/*@{*/

//CSdpAgent commands
_LIT(KCmdNewL, 													"NewL");
_LIT(KCmdNewLC, 												"NewLC");
_LIT(KCmdSetRecordFilterL,										"SetRecordFilterL");
_LIT(KCmdSetAttributePredictorListL, 							"SetAttributePredictorListL");
_LIT(KCmdNextRecordRequestL, 									"NextRecordRequestL");
_LIT(KCmdAttributeRequestL, 									"AttributeRequestL");
_LIT(KCmdCancel, 												"Cancel");
_LIT(KCmdDestructor, 											"~");
_LIT(KCmdMSAN_ExtensionInterfaceL,								"MSAN_ExtensionInterfaceL");

//CSdpAgent Parameters
_LIT(KAttrid, 													"Attrid");
_LIT(KTBTDevAddr,												"tbtdevaddr");
_LIT(KIntDevAddr,												"integerdevaddr");
_LIT(KTBTSearchPattern,											"tbtsearchpattern");
_LIT(KTBTAttrIdList,											"tbtattridlist");
_LIT(KAttrValListName,											"tbtattrvaluelist");
_LIT(KListType, 			                                    "type");
_LIT(KExpectedRecordsCount,										"expectedServRecCount");

const TInt KDefaultCount = -1;
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_DataSdpAgent* CT_DataSdpAgent::NewL()
	{
	CT_DataSdpAgent*	ret=new (ELeave) CT_DataSdpAgent();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_DataSdpAgent::CT_DataSdpAgent()
:	iSdpAgent(NULL)
,	iAttrValueList(NULL)
,	iExpectedRecordsCount(KDefaultCount)
,	iAttrValArray()
,   iAttrReqIndex(0)
,	iNextRecReqIndex(0)
    {
    }

void CT_DataSdpAgent::ConstructL()
	{
	}

CT_DataSdpAgent::~CT_DataSdpAgent()
	{
	DestroyData();
	}

void CT_DataSdpAgent::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iSdpAgent = static_cast<CSdpAgent*> (aAny);
	}

void CT_DataSdpAgent::DisownObjectL()
	{
	iSdpAgent = NULL;
	}

void CT_DataSdpAgent::DestroyData()
	{
	delete iSdpAgent;
	iSdpAgent=NULL;
	
	//	Do not delete these as we do not own them
	iAttrValueList=NULL;
	}

inline TCleanupOperation CT_DataSdpAgent::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_DataSdpAgent::CleanupOperation(TAny* aAny)
	{
	CSdpAgent* agent=static_cast<CSdpAgent*>(aAny);
	delete agent;
	}

void CT_DataSdpAgent::AttributeRequestComplete(TSdpServRecordHandle aHandle, TInt aError)
	{
	INFO_PRINTF2(_L("MSdpAttributeValueVisitor::AttributeRequestComplete Call: aHandle = %d"),aHandle);
	DecOutstanding();
	if ( aHandle!=iSSRHandle)
		{
		ERR_PRINTF2(_L("Service record handle not as expected: expected = %d"), iSSRHandle);
		SetAsyncError(iAttrReqIndex, KErrGeneral);
		}			
	if ( aError!=KErrNone) 
		{
		ERR_PRINTF2(_L("AttributeRequestComplete Call failed: aError = %d"),aError);
		SetAsyncError(iAttrReqIndex, aError);
		}	
	}

void CT_DataSdpAgent::AttributeRequestResult(TSdpServRecordHandle aHandle,
									   TSdpAttributeID /*aAttrID*/, CSdpAttrValue* aAttrValue)
	{
	INFO_PRINTF2(_L("MSdpAttributeValueVisitor::AttributeRequestResult Call: aHandle = %d"),aHandle);
	if ( aHandle!=iSSRHandle)
		{
		ERR_PRINTF2(_L("Service record handle not as expected: expected = %d"), iSSRHandle);
		SetAsyncError(iAttrReqIndex, KErrGeneral);
		}
	if (aAttrValue)
		{
		INFO_PRINTF1(_L("Append to Array!"));	
		iAttrValArray.Append(aAttrValue);
   		}		
	}

void CT_DataSdpAgent::NextRecordRequestComplete(TInt aError,
										 TSdpServRecordHandle aHandle, TInt aTotalRecordsCount)
	{
	DecOutstanding();
	INFO_PRINTF2(_L("MSdpAttributeValueVisitor::NextRecordRequestComplete Call ! Number of records: %d"),aTotalRecordsCount);
	if (aError!=KErrNone)
		{
		ERR_PRINTF2(_L("Disconnected: %d"), aError);
		SetAsyncError(iNextRecReqIndex, aError);
		}
	else
		{ 
		if(aTotalRecordsCount!=iExpectedRecordsCount && iExpectedRecordsCount!=KDefaultCount)
			{
			ERR_PRINTF2(_L("Number of service records not as expected: expected = %d"),iExpectedRecordsCount);
			SetBlockResult(EFail);
			}
		if(aTotalRecordsCount!=0)
			{
			INFO_PRINTF1(_L("Found Service."));
			iSSRHandle = aHandle;
			}
		}
	}
	
	
const RPointerArray<CSdpAttrValue>& CT_DataSdpAgent::GetAttrValArray()
	{
	return iAttrValArray;
	}
	

/**
 * Process a command read from the ini file
 *
 * @param aCommand			The command to process
 * @param aSection			The section in the ini containing data for the command
 * @param aAsyncErrorIndex	Command index for async calls to return errors to
 *
 * @return					ETrue if the command is processed
 *
 * @leave					System wide error
 */
TBool CT_DataSdpAgent::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	ret = ETrue;

	//-------------------------------------------------------------------------
	// CSdpAgent
	//-------------------------------------------------------------------------
	if ( aCommand==KCmdNewL )
		{
        DoCmdNewL(aSection);
		}
	else if ( aCommand==KCmdNewLC )
		{
        DoCmdNewLC(aSection);
		}
	else if ( aCommand==KCmdSetRecordFilterL )
		{
        DoCmdSetRecordFilterL(aSection);
		}
	else if ( aCommand==KCmdNextRecordRequestL )
		{
        DoCmdNextRecordRequestL(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdSetAttributePredictorListL )
		{
        DoCmdSetAttributePredictorListL(aSection);
		}
	else if ( aCommand==KCmdAttributeRequestL )
		{
        DoCmdAttributeRequestL(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdCancel )
		{
        DoCmdCancel();
		}
	else if ( aCommand==KCmdDestructor )
		{
		DoCmdDestructor();
		}
	else if ( aCommand==KCmdMSAN_ExtensionInterfaceL )
		{
		DoCmdMSAN_ExtensionInterfaceL();
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}
//CSdpAgent helper methods

void CT_DataSdpAgent::DoCmdNewL(const TDesC& aSection)
	{
	DestroyData();
	TPtrC	tBTDevAddrName;
	TBool	foundParameter=EFalse;	
	INFO_PRINTF1(_L("CSdpAgent NewL call"));
	if( GetStringFromConfig(aSection, KTBTDevAddr(), tBTDevAddrName))
		{
		foundParameter=ETrue;			
		CT_BTDevAddrData* tBTDevAddrData=static_cast<CT_BTDevAddrData*>(GetDataWrapperL(tBTDevAddrName));
		const TBTDevAddr* btDevAddr = tBTDevAddrData->GetAddress();
		TRAPD(err, iSdpAgent = CSdpAgent::NewL(*this, *btDevAddr));
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("CSdpAgent NewL failed with error %d"), err);
			SetError(err);
			}
   		}
   	TInt	intBtDevAddr=0;	
	if( GetIntFromConfig(aSection, KIntDevAddr(), intBtDevAddr))
		{
		foundParameter=ETrue;			
		const TBTDevAddr btDevAddr(intBtDevAddr);
		TRAPD(err, iSdpAgent = CSdpAgent::NewL(*this, btDevAddr));
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("CSdpAgent NewL failed with error %d"), err);
			SetError(err);
			}
   		}  	
   		
	if( !foundParameter )
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KIntDevAddr());
		SetBlockResult(EFail);
		}
	}

void CT_DataSdpAgent::DoCmdNewLC(const TDesC& aSection)
	{
	DestroyData();		
	TPtrC	tBTDevAddrName;
	TBool	foundParameter=EFalse;

	if( GetStringFromConfig(aSection, KTBTDevAddr(), tBTDevAddrName))
		{
		foundParameter=ETrue;			
		CT_BTDevAddrData* tBTDevAddrData=static_cast<CT_BTDevAddrData*>(GetDataWrapperL(tBTDevAddrName));
		TBTDevAddr btDevAddr = *tBTDevAddrData->GetAddress();
		TBuf<KMaxSockAddrSize> tmpBuf2;
		btDevAddr.GetReadable(tmpBuf2);

		TRAPD(err, iSdpAgent = CSdpAgent::NewLC(*this, btDevAddr); CleanupStack::Pop(iSdpAgent));
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("CSdpAgent NewLC failed with error %d"), err);
			SetError(err);
			}
		}
		
   	TInt	intBtDevAddr=0;	
	if( GetIntFromConfig(aSection, KIntDevAddr(), intBtDevAddr))
		{
		foundParameter=ETrue;			
		const TBTDevAddr btDevAddr(intBtDevAddr);
		TRAPD(err, iSdpAgent = CSdpAgent::NewLC(*this, btDevAddr));
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("CSdpAgent NewLC failed with error %d"), err);
			SetError(err);
			}
   		}  	
   		
	if( !foundParameter )
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KIntDevAddr());
		SetBlockResult(EFail);
		}
	}

void CT_DataSdpAgent::DoCmdSetRecordFilterL(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("CSdpAgent SetRecordFilterL call"));
	TPtrC	searchPatternName;
	if( GetStringFromConfig(aSection, KTBTSearchPattern(), searchPatternName))
		{
		CSdpSearchPattern*	searchPattern = static_cast<CSdpSearchPattern*>(GetDataObjectL(searchPatternName));
		// Set the search pattern on the agent
		TRAPD(err, 	iSdpAgent->SetRecordFilterL(*searchPattern));
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("CSdpAgent SetRecordFilterL failed with error %d"), err);
			SetError(err);
			}
		}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KTBTSearchPattern());
		SetBlockResult(EFail);
		}
	}

void CT_DataSdpAgent::DoCmdNextRecordRequestL(const TDesC& aSection, const TInt aIndex)
	{
	INFO_PRINTF1(_L("CSdpAgent NextRecordRequestL call"));
	iExpectedRecordsCount = KDefaultCount;
	TRAPD(err, iSdpAgent->NextRecordRequestL());
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("CSdpAgent NextRecordRequestL failed with error %d"), err);
		SetError(err);
		}
	else
		{
		IncOutstanding();
		iNextRecReqIndex = aIndex;
		/*
		The expected count is optional. 
		This API will be called in most of test cases and it's probably not neccessary to
		check the count everytime. Especially when this class isn't the focus of testing.
		*/
		GetIntFromConfig(aSection, KExpectedRecordsCount(), iExpectedRecordsCount);
		}
	}

void CT_DataSdpAgent::DoCmdSetAttributePredictorListL(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("CSdpAgent SetAttributePredictorListL call"));
	TPtrC	dataAttrIdListName;

	if( GetStringFromConfig(aSection, KTBTAttrIdList(), dataAttrIdListName))
		{
		CSdpAttrIdMatchList*	attrIdList = static_cast<CSdpAttrIdMatchList*>(GetDataObjectL(dataAttrIdListName));

 		TRAPD(err, iSdpAgent->SetAttributePredictorListL(*attrIdList));
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("CSdpAgent SetAttributePredictorListL failed with error %d"), err);
			SetError(err);
			}
		}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KTBTAttrIdList());
		SetBlockResult(EFail);
		}
	}

void CT_DataSdpAgent::DoCmdCancel()
	{
	iSdpAgent->Cancel();
	}

void CT_DataSdpAgent::DoCmdDestructor()
	{
	DestroyData();
	}

void CT_DataSdpAgent::DoCmdMSAN_ExtensionInterfaceL()
	{
	INFO_PRINTF1(_L("MSdpAgentNotifier MSAN_ExtensionInterfaceL call"));
	void* tmpVoid;
	TRAPD(err, this->MSAN_ExtensionInterfaceL(KNullUid, tmpVoid));
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("MSdpAgentNotifier MSAN_ExtensionInterfaceL failed with error %d"), err);
		SetError(err);
		}
	}

void CT_DataSdpAgent::DoCmdAttributeRequestL(const TDesC& aSection, const TInt aIndex)
	{
	TInt 	attrId;
	TPtrC	attrValListName;
	TPtrC	theType;
	TPtrC	dataAttrIdListName;
	TPtrC	expectedAttrValListName;
	CSdpAttrIdMatchList*	attrIdList=NULL;
	TBool	ifAttrIdExist=EFalse;
	TBool	ifAttrValListExist=EFalse;
	TBool	ifAttrIdListExist=EFalse;

	
	// Get AttrId from config (test data) file
	if( GetIntFromConfig(aSection, KAttrid(), attrId) )
	   	{
		ifAttrIdExist=ETrue;
		}

	//Get dictionary name of AttrValueList Wrapper
	if( GetStringFromConfig(aSection, KAttrValListName(), attrValListName) )
		{
		//Get the list type to cast to
		if( GetStringFromConfig(aSection, KListType(), theType) )
			{
			ifAttrValListExist=ETrue;
			iAttrValueList = CT_BTUtil::AttrValueListCastL(*this, attrValListName, theType);
			}
		}

	//Get the dictionary name of AttrIdMatchList Wrapper
	if( GetStringFromConfig(aSection, KTBTAttrIdList(), dataAttrIdListName) )
		{
		ifAttrIdListExist=ETrue;
		attrIdList = static_cast<CSdpAttrIdMatchList*>(GetDataObjectL(dataAttrIdListName));
		}

	TInt	err=KErrNone;
	iAttrReqIndex = aIndex;
	if(!ifAttrValListExist && ifAttrIdExist)
		{
		INFO_PRINTF1(_L("CSdpAgent AttributeRequestL(TSdpServRecordHandle, TSdpAttributeID) call"));
		TRAP(err, iSdpAgent->AttributeRequestL(iSSRHandle, attrId));
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("CSdpAgent AttributeRequestL failed with error %d"), err);
			SetError(err);
			}	
		else
			{	
			IncOutstanding();
			}
		}
	else if(ifAttrValListExist && ifAttrIdExist)
		{
		INFO_PRINTF1(_L("CSdpAgent AttributeRequestL(MSdpElementBuilder, TSdpServRecordHandle, TSdpAttributeID) call"));
		TRAP(err, iSdpAgent->AttributeRequestL(iAttrValueList, iSSRHandle, attrId));
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("CSdpAgent AttributeRequestL failed with error %d"), err);
			SetError(err);
			}	
		else
			{	
			IncOutstanding();
			}
		}
	else if(!ifAttrValListExist && ifAttrIdListExist)
		{
		INFO_PRINTF1(_L("CSdpAgent AttributeRequestL(TSdpServRecordHandle, const CSdpAttrIdMatchList&) call"));
 		TRAP(err, iSdpAgent->AttributeRequestL(iSSRHandle, *attrIdList));
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("CSdpAgent AttributeRequestL failed with error %d"), err);
			SetError(err);
			}	
		else
			{	
			IncOutstanding();
			}
		}
	else if(ifAttrValListExist && ifAttrIdListExist)
		{
		INFO_PRINTF1(_L("CSdpAgent AttributeRequestL(MSdpElementBuilder, TSdpServRecordHandle, const CSdpAttrIdMatchList&) call"));
 		TRAP(err, iSdpAgent->AttributeRequestL(iAttrValueList, iSSRHandle, *attrIdList));
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("CSdpAgent AttributeRequestL failed with error %d"), err);
			SetError(err);
			}	
		else
			{	
			IncOutstanding();
			}
		}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KTBTAttrIdList());
		SetBlockResult(EFail);
		}
	}
