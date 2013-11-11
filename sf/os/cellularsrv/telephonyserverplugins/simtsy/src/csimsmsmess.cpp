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
//

/**
 @file
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "csimsmsmessTraces.h"
#endif

#include "csimsmsmess.h"
#include <testconfigfileparser.h>
#include <etelmm.h>
#include "CSimTsyMode.h"

//macro
#define UNPACK_PCKG(target,source,datatype)  datatype& target=  (*(TPckg<datatype>*)(source))()

const TInt KSmsSettingGranularity=5;	// < Granularity of SMS parameter list arrays.
const TInt KSmsStoreGranularity=2;	// < Granularity of SMS store list array.
const TInt KNoMessageReferenceInCofigurationFile=0;//the reference number returned to the client is 0 if
													//no value is found in the configuration file
			
//
// CSimSmsMessaging
//

void CSimSmsMessaging::CloseSmsObj(TAny* aObj)
/**
 *	A utility function for cleaning up the stack.
 */
	{
	((CObject*)aObj)->Close();
	}

CSimSmsMessaging* CSimSmsMessaging::NewL(CSimPhone* aPhone)
/**
 *	Standard two phase constructor.
 */
	{
	CSimSmsMessaging* smsMess=new(ELeave) CSimSmsMessaging(aPhone);
	TCleanupItem newObjClose(CloseSmsObj,smsMess);
	CleanupStack::PushL(newObjClose);
	smsMess->ConstructL();
	CleanupStack::Pop();
	return smsMess;
	}

CSimSmsMessaging::CSimSmsMessaging(CSimPhone* aPhone)
		: iPhone(aPhone), iSmspBusy(EFalse)
	{}

void CSimSmsMessaging::ConstructL()
/**
 * Retrieve the SMS-related tags from the configuration file.
 *
 * 
 * If there are no constraints any SMS specified in the configuration file and the "incoming SMS event" timer will
 * be started.
 */
    {
    InitializeL();
    }

void CSimSmsMessaging::InitializeL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_INITIALIZEL_1, "Starting to Load and Parse Sms Messaging Config ");
	iRxTimer=CSimTimer::NewL(iPhone);
	iTxTimer=CSimTimer::NewL(iPhone);
	iSmspTimer=CSimTimer::NewL(iPhone);
		
	iSmsRxParameterListGsm=new(ELeave) CArrayFixFlat<TSmsRxParametersGsm>(KSmsSettingGranularity);	
		
		
	FindAndCreateRxAttributesL();
	
	iSmsTxParametersListGsm=new(ELeave) CArrayFixFlat<TSmsTxParametersGsm>(KSmsSettingGranularity);		
			
	FindAndCreateTxAttributesL();
	iSmsStores=new(ELeave) CArrayFixFlat<CSimSmsStore*>(KSmsStoreGranularity);
	FindAndCreateSmsStoresL(iPhone);
	FindAndCreateConstraints();
	iSmspReadAll=new(ELeave) CArrayPtrFlat<CListReadAllAttempt>(KSmsStoreGranularity);
	FindAndCreateSmsParamsL();

	// NOTE - no need to start iRxTimer; this will be started once a EMobileSmsMessagingReceiveMessage
	// request is received and/or Send Sms contrainst satisfied (see CompleteTxPendingReq)
	
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_INITIALIZEL_2, "Finished parsing SMS Messaging config parameters");
	}


void CSimSmsMessaging::Reset()
    {
 	if(iSmsRxParameterListGsm)
		{
		iSmsRxParameterListGsm->Delete(0,iSmsRxParameterListGsm->Count());
		delete iSmsRxParameterListGsm;
        iSmsRxParameterListGsm = NULL;
		}
		
	if(iSmsTxParametersListGsm)
		{
		iSmsTxParametersListGsm->Delete(0,iSmsTxParametersListGsm->Count());
		delete iSmsTxParametersListGsm;
        iSmsTxParametersListGsm = NULL;
		}
			
    delete iSmspEntries;
    iSmspEntries = NULL;
    
    if (iSmsStores)
        {
        TInt storeCount=iSmsStores->Count();    
        for(TInt i=0;i<storeCount;i++)
            {
            iSmsStores->At(i)->Close();
            }
        delete iSmsStores;
        iSmsStores = NULL;
        }

    if (iSmspReadAll)
        {
        iSmspReadAll->ResetAndDestroy();
        delete iSmspReadAll;
        iSmspReadAll = NULL;
        }
        
    iConstraints.Reset();
    
    iSmsTxCnt = 0;
    iSmsRxCnt = 0;
    iCurrentConstraint = 0;
    iConstraintRxCnt = 0;
    
    iSmspBusy   = EFalse;
    iRxState    = ESmsRxStateIdle;
    iTxState    = ESmsTxStateIdle;
       
    delete iRxTimer;
    iRxTimer = NULL;

    delete iTxTimer;
    iTxTimer = NULL;
    
    delete iSmspTimer;
    iSmspTimer = NULL;
    }

CSimSmsMessaging::~CSimSmsMessaging()
/**
 *	Standard destructor.  Any objects created by the ::ConstructL() function should be destroyed here.
 */
	{
	Reset();
	}

void CSimSmsMessaging::FindAndCreateRxAttributesL()
/**
 * Extract values from the tags
 * SmsRx, SmsRxPeriod, SmsAckNackPause, SmsResumePause, SmsDeliveryReport
 *
 */	{
	TInt ret=KErrNone;
	TInt count=CfgFileSection()->ItemCount(KSmsRx);
	const CTestConfigItem* item=NULL;
	TInt i;
	TSmsRxParametersGsm smsRxParameterGsm;
	
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_FINDANDCREATERXATTRIBUTESL_1, "CSimSmsMessaging::FindAndCreateRxAttributesL IN [count=%d]", count);

	// Need to do this *before* entering the loop (so that we know 
	// whether or not to read a delivery report pdu off the config file).
	iSmsControlCaps=RMobileSmsMessaging::TMobileSmsControlCaps(CfgFileSection()->ItemValue(KSmsControlCaps,KDefaultSmsControlCaps));

	for(i=0;i<count;i++)
		{
		item=CfgFileSection()->Item(KSmsRx,i);
		if(!item)
			break;

		TPtrC8 smsPdu,sca;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,smsPdu);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_FINDANDCREATERXATTRIBUTESL_2, "WARNING - CONFIGURATION FILE PARSING - Reading element SMSPDU returned %d (element no. %d) from tag %s.",ret,0,KSmsRx);
			continue;
			}
		else
			{
			smsRxParameterGsm.iPdu.Zero();
			ConvertAsciiSms(smsPdu,smsRxParameterGsm.iPdu);
			}
		
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,sca);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_FINDANDCREATERXATTRIBUTESL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element SCA returned %d (element no. %d) from tag %s.",ret,1,KSmsRx);
			continue;
			}
		else
			{
			smsRxParameterGsm.iSca=sca;
			}
			
		//get delivery report pdu 
		smsRxParameterGsm.iDeliveryReport.Zero(); 	
		if (!(iSmsControlCaps & RMobileSmsMessaging::KCapsReceiveUnstoredClientAck))
			{
			TPtrC8 deliveryReportPdu;
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,deliveryReportPdu);
			if( (ret!=KErrNone) && (ret!=KErrGeneral) )
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_FINDANDCREATERXATTRIBUTESL_4, "WARNING - CONFIGURATION FILE PARSING - Reading element DELIVERYREPORTPDU returned %d (element no. %d) from tag %s.",ret,2,KSmsRx);
				}
			else
				{
				if (CSimTsyMode::GetMode() != CSimTsyMode::ECdmaV1)			
					{
  					ConvertAsciiSms(deliveryReportPdu,smsRxParameterGsm.iDeliveryReport); 
  					}

				}
			}

		// add to list of params
		iSmsRxParameterListGsm->AppendL(smsRxParameterGsm);	
	
		}// for
	
	iSmsRxPeriod=CfgFileSection()->ItemValue(KSmsRxPeriod,KDefaultSmsRxPeriod);
	iAckNackCompletePause=CfgFileSection()->ItemValue(KSmsAckNackPause,KDefaultSmsAckNackPause);
	iResumeCompletePause=CfgFileSection()->ItemValue(KSmsResumePause,KDefaultSmsResumePause);

	if (iSmsControlCaps & RMobileSmsMessaging::KCapsReceiveUnstoredClientAck)
		iSmsReceiveMode=RMobileSmsMessaging::EReceiveUnstoredClientAck;
	else iSmsReceiveMode=RMobileSmsMessaging::EReceiveModeUnspecified;
	OstTraceDefExt4(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_FINDANDCREATERXATTRIBUTESL_5, "iSmsRxPeriod =%d, iAckNackCompletePause=%d, iResumeCompletePause=%d, iSmsControlCaps=%d",iSmsRxPeriod, iAckNackCompletePause, iResumeCompletePause, iSmsControlCaps);
	OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_FINDANDCREATERXATTRIBUTESL_6, "iSmsModeCaps =%d, iSmsRxStartDelay = %d, iSmsReceiveMode = %d",iSmsModeCaps , iSmsRxStartDelay, iSmsReceiveMode);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_FINDANDCREATERXATTRIBUTESL_7, "CSimSmsMessaging::FindAndCreateRxAttributesL OUT [count=%d]", iSmsRxParameterListGsm->Count());
	}

void CSimSmsMessaging::FindAndCreateTxAttributesL()
/**
 * Extract values from the tags
 * SmsTx, SmsTxPause
 *
 */
	{
	TInt ret=KErrNone;
	TInt count=CfgFileSection()->ItemCount(KSmsTx);
		
	TSmsTxParametersGsm   smsTxParametersGsm;
	
	const CTestConfigItem* item=NULL;
	for(TInt i=0;i<count;i++)
		{
		item=CfgFileSection()->Item(KSmsTx,i);//Tx
		if(!item)
			break;

		//get pdu
		TPtrC8 smsTx;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,smsTx);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_FINDANDCREATETXATTRIBUTESL_1, "WARNING - CONFIGURATION FILE PARSING - Reading element SMSTX returned %d (element no. %d) from tag %s.",ret,0,KSmsTx);
			continue;
			}
		else
			{			
			smsTxParametersGsm.iPdu.Zero();
			ConvertAsciiSms(smsTx,smsTxParametersGsm.iPdu);
			}

		//get Sca
		TPtrC8 sca;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,sca);
		if (ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_FINDANDCREATETXATTRIBUTESL_2, "WARNING - CONFIGURATION FILE PARSING - Reading element SCA returned %d (element no. %d) from tag %s.",ret,1,KSmsTx);
			continue;
			}
		else
			{			
			smsTxParametersGsm.iSca.Copy(sca);				
			}
			
		//get reference
		TInt reference;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,reference);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_FINDANDCREATETXATTRIBUTESL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element REFERENCE returned %d (element no. %d) from tag %s.",ret,2,KSmsTx);
			smsTxParametersGsm.iRef=KNoMessageReferenceInCofigurationFile;	
			}
		else
			{	
			smsTxParametersGsm.iRef=reference;				
			}
	
		//get submit report pdu
		TPtrC8 submitReportPdu;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,submitReportPdu);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_FINDANDCREATETXATTRIBUTESL_4, "WARNING - CONFIGURATION FILE PARSING - Reading element SUBMITREPORTPDU returned %d (element no. %d) from tag %s.",ret,3,KSmsTx);
			continue;
			}
		else
			{			
			smsTxParametersGsm.iSubmitReport.Zero(); 	
			ConvertAsciiSms(submitReportPdu,smsTxParametersGsm.iSubmitReport); 
			}
		
		//get expected error code
		TInt errorCode;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,errorCode);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_FINDANDCREATETXATTRIBUTESL_5, "WARNING - CONFIGURATION FILE PARSING - Reading element ERRORCODE returned %d (element no. %d) from tag %s.",ret,4,KSmsTx);
			continue;
			}
		else
			{		
			smsTxParametersGsm.iExpectedError=errorCode;					
			}		
		
		iSmsTxParametersListGsm->AppendL(smsTxParametersGsm);			

		} //end for SmsTx Tag
	
	iSmsTxPause=CfgFileSection()->ItemValue(KSmsTxPause,KDefaultSmsTxPause);
	}

void CSimSmsMessaging::FindAndCreateConstraints()
/**
 * Extract values from the tags
 * SmsStartRxDelay
 *
 */
	{
	TInt ret=KErrNone;
	TInt count=CfgFileSection()->ItemCount(KSmsStartRxDelay);
	const CTestConfigItem* item=NULL;
	TConstraintEntry constraint;
	for(TInt i=0;i<count;i++)//Rx delay
		{
		item=CfgFileSection()->Item(KSmsStartRxDelay,i);
		if(!item)
			break;
		
		TInt ipc,noBefore,noAfter;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,ipc);//only :SendMessage() is supported
		if(ret!=KErrNone || ipc!=4207)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_FINDANDCREATECONSTRAINTS_1, "WARNING - CONFIGURATION FILE PARSING - Reading element IPC returned %d (element no. %d) from tag %s.",ret,0,KSmsStartRxDelay);
			continue;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,noBefore);
		if(ret==KErrNone)
			constraint.iIpcCnt=noBefore;
		else
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_FINDANDCREATECONSTRAINTS_2, "WARNING - CONFIGURATION FILE PARSING - Reading element NOBEFORE returned %d (element no. %d) from tag %s.",ret,1,KSmsStartRxDelay);
			continue;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,noAfter);
		if(ret==KErrNone)
			constraint.iRxCnt=noAfter;
		else
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_FINDANDCREATECONSTRAINTS_3, "WARNING - CONFIGURATION FILE PARSING - Reading element NOAFTER returned %d (element no. %d) from tag %s.",ret,2,KSmsStartRxDelay);
			continue;
			}
		
		__ASSERT_ALWAYS(iConstraints.Append(constraint) == KErrNone,SimPanic(EGeneral));
		}
	}

void CSimSmsMessaging::FindAndCreateSmsStoresL(CSimPhone* aPhone)
/**
 *	Creates sms stores as defined in the comfiguation files
 *	Extract values from the tags
 *	SmsStore, SmsStoreEntry, SmsStoreIndividualReqPause, SmsStoreBatchReqPause
 */
	{
	TInt count=CfgFileSection()->ItemCount(KSmsStore);
	const CTestConfigItem* item=NULL;
	TInt ret=KErrNone;

	for(TInt i=0;i<count;i++)
		{
		item=CfgFileSection()->Item(KSmsStore,i);
		if(!item)
			break;

		TPtrC8 storeName;
		TInt maxNumSlots;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,storeName);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_FINDANDCREATESMSSTORESL_1, "WARNING - CONFIGURATION FILE PARSING - Reading element STORENAME returned %d (element no. %d) from tag %s.",ret,0,KSmsStore);
			continue;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,maxNumSlots);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_FINDANDCREATESMSSTORESL_2, "WARNING - CONFIGURATION FILE PARSING - Reading element MAXNUMSLOTS returned %d (element no. %d) from tag %s.",ret,1,KSmsStore);
			continue;
			}

		CSimSmsStore* smsStore=CSimSmsStore::NewL(this,storeName,maxNumSlots,aPhone);
		TCleanupItem newObjClose(CloseSmsObj,smsStore);
		CleanupStack::PushL(newObjClose);
		iSmsStores->AppendL(smsStore);
		CleanupStack::Pop();
		}

	for(TInt j=0;j<iSmsStores->Count();j++)
		{
		iSmsStores->At(j)->PopulateStoreFromConfigFile();
		}
	}

void CSimSmsMessaging::FindAndCreateSmsParamsL()
/**
 *Populates the sms parameter List as defined in the configuration file
 *Extract values from the tags
 *SmsParamEntry, SmsParamEntry, SmsParamBatchReqPause, SmsParamMaxNumSlots
 */
	{
	iSmspEntries= CMobilePhoneSmspList::NewL();

	const TInt maxSlots = CfgFileSection()->ItemValue(KSmspMaxNumSlots ,KDefaultSmspMaxNumSlots);
	iSmspEntries->SetMaxNumberEntries(maxSlots);

	iSmspBatchPause = CfgFileSection()->ItemValue(KSmspBatchReqPause,KDefaultSmspBatchReqPause);

	const TInt numberOfSmspEntries = Min(CfgFileSection()->ItemCount(KSmsParamEntry), maxSlots);
	const CTestConfigItem* item=NULL;
	TInt ret=KErrNone;
	
	for(TInt i=0;i < numberOfSmspEntries;i++)
		{
		item=CfgFileSection()->Item(KSmsParamEntry,i);

		if(!item)
			break;

		RMobileSmsMessaging::TMobileSmspEntryV1 entry = RMobileSmsMessaging::TMobileSmspEntryV1();

		TInt index, pid, dcs, validityPeriod;
		TPtrC8 destAddress, sca, smspName;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,index);
		if((ret==KErrNone)&&(index < maxSlots))
			entry.iIndex=index;
		else
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_FINDANDCREATESMSPARAMSL_1, "WARNING - CONFIGURATION FILE PARSING - Reading element INDEX returned %d (element no. %d) from tag %s.",ret,0,KSmsParamEntry);
			continue;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,smspName);
		if(ret==KErrNone)
			entry.iText.Copy(smspName);
		else
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_FINDANDCREATESMSPARAMSL_2, "WARNING - CONFIGURATION FILE PARSING - Reading element SMSPNAME returned %d (element no. %d) from tag %s.",ret,1,KSmsParamEntry);
			continue;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,pid);
		if(ret==KErrNone)
			{
			entry.iProtocolId=TUint8(pid);
			entry.iValidParams |= RMobileSmsMessaging::KProtocolIdIncluded;
			}
		else
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_FINDANDCREATESMSPARAMSL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element PID returned %d (element no. %d) from tag %s.",ret,2,KSmsParamEntry);
			continue;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,dcs);
		if(ret==KErrNone)
			{
			entry.iDcs=TUint8(dcs);
			entry.iValidParams |= RMobileSmsMessaging::KDcsIncluded;
			}
		else
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_FINDANDCREATESMSPARAMSL_4, "WARNING - CONFIGURATION FILE PARSING - Reading element DCS returned %d (element no. %d) from tag %s.",ret,3,KSmsParamEntry);
			continue;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,validityPeriod);
		if(ret==KErrNone)
			{
			entry.iValidityPeriod=TUint8(validityPeriod);
			entry.iValidParams |= RMobileSmsMessaging::KValidityPeriodIncluded;
			}
		else
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_FINDANDCREATESMSPARAMSL_5, "WARNING - CONFIGURATION FILE PARSING - Reading element VALIDITYPERIOD returned %d (element no. %d) from tag %s.",ret,4,KSmsParamEntry);
			continue;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,5,destAddress);
		if(ret==KErrNone)
			{
			RecordDestination(destAddress, entry);
			}
		else
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_FINDANDCREATESMSPARAMSL_6, "WARNING - CONFIGURATION FILE PARSING - Reading element DESTADDRESS returned %d (element no. %d) from tag %s.",ret,5,KSmsParamEntry);
			continue;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,6,sca);
		if(ret==KErrNone)
			{
			RecordSca(sca, entry);
			}
		else
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_FINDANDCREATESMSPARAMSL_7, "WARNING - CONFIGURATION FILE PARSING - Reading element SCA returned %d (element no. %d) from tag %s.",ret,6,KSmsParamEntry);
			continue;
			}

		iSmspEntries->AddEntryL(entry);
		}
	}

void CSimSmsMessaging::RecordSca(const TDesC8& aAsciiAddr, RMobileSmsMessaging::TMobileSmspEntryV1& aEntry)
/**
 * Used to set the sca number in the smsp
 *
 * @param aAsciiSca Ascii representation of a telephone number such as the one found in the configuration file
 * @param aEntry reference to the actual smspentry in which the sca is going to be recorded
 */
	{
	if(aAsciiAddr.Length()>0)//only support for international numbers
		{
		aEntry.iServiceCentre.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
		aEntry.iServiceCentre.iTypeOfNumber=RMobilePhone::EInternationalNumber;
		aEntry.iServiceCentre.iTelNumber.Copy(aAsciiAddr);
		aEntry.iValidParams |= RMobileSmsMessaging::KSCAIncluded;
		}
	}

void CSimSmsMessaging::RecordDestination(const TDesC8& aAsciiAddr, RMobileSmsMessaging::TMobileSmspEntryV1& aEntry)
/**
 * Used to set the destination number in the smsp
 *
 * @param aAsciiAddr Ascii representation of a telephone number such as the one found in the configuration file
 * @param aEntry reference to the actual smspentry in which the destination is going to be recorded
 */
	{
	if(aAsciiAddr.Length()>0)//only support for international numbers
		{
		aEntry.iDestination.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
		aEntry.iDestination.iTypeOfNumber=RMobilePhone::EInternationalNumber;
		aEntry.iDestination.iTelNumber.Copy(aAsciiAddr);
		aEntry.iValidParams |= RMobileSmsMessaging::KDestinationIncluded;
		}	
	}

#if (OST_TRACE_CATEGORY & OST_TRACE_CATEGORY_DEBUG)	// to stop the UREL build warnings
void CSimSmsMessaging::LogRequest(const TBool aEntering, TInt aIpc, TInt aError)
#else
void CSimSmsMessaging::LogRequest(const TBool aEntering, TInt aIpc, TInt /*aError*/)
#endif
/**
 * This method logs client requests to the t_reg.txt
 * @param aDirection Tells if the request is coming into simtsy tsy or completing
 * @param aIpc IPC number of request
 * @param aError Error code that the request has
 */
 	{
	TBuf8<64> ipcBuf;

	switch (aIpc)
		{
		case EEtelServerLoadPhoneModule:
			ipcBuf = _L8("ServerLoadPhoneModule");
			break;

		case EEtelPhoneGetStatus:
			ipcBuf = _L8("PhoneGetStatus");
			break;

		case EEtelPhoneNotifyModemDetected:
			ipcBuf = _L8("PhoneNotifyModemDetected");
			break;
		case EEtelPhoneInitialise:
			ipcBuf = _L8("PhoneInitialise");
			break;
		case EMobileSmsMessagingGetCaps:
			ipcBuf = _L8("GetCaps");
			break;
		case EMobileSmsMessagingGetReceiveMode:
			ipcBuf = _L8("GetReceiveMode");
			break;
		case EMobileSmsMessagingGetMoSmsBearer:
			ipcBuf = _L8("GetMoSmsBearer");
			break;
		case EMobileSmsMessagingEnumerateMessageStores:
			ipcBuf = _L8("EnumerateMessageStores");
			break;
		case EMobileSmsMessagingGetMessageStoreInfo:
			ipcBuf = _L8("GetMessageStoreInfo");
			break;
		case EMobileSmsMessagingGetSmspListPhase2:
			ipcBuf = _L8("GetSmspListPhase2");
			break;
		case EMobileSmsMessagingSetReceiveMode:
			ipcBuf = _L8("SetReceiveMode");
			break;
		case EMobileSmsMessagingSetMoSmsBearer:
			ipcBuf = _L8("SetMoSmsBearer");
			break;
		case EMobileSmsMessagingAckSmsStored:
			ipcBuf = _L8("AckSmsStored");
			break;
		case EMobileSmsMessagingNackSmsStored:
			ipcBuf = _L8("NackSmsStored");
			break;
		case EMobileSmsMessagingResumeSmsReception:
			ipcBuf = _L8("ResumeSmsReception");
			break;
		case EMobileSmsMessagingSendMessage:
			ipcBuf = _L8("SendMessage");
			break;
		case EMobileSmsMessagingGetSmspListPhase1:
			ipcBuf = _L8("GetSmspListPhase1");
			break;
		case EMobileSmsMessagingStoreSmspList:
			ipcBuf = _L8("StoreSmspList");
			break;
		case EMobileSmsMessagingReceiveMessage:
			ipcBuf = _L8("ReceiveMessage");
			break;
		case EMobileSmsMessagingNotifyReceiveModeChange:
			ipcBuf = _L8("NotifyReceiveModeChange");
			break;
		case EMobileSmsMessagingNotifyMoSmsBearerChange:
			ipcBuf = _L8("NotifyMoSmsBearerChange");
			break;
		case EMobileSmsMessagingNotifySmspListChange:
			ipcBuf = _L8("NotifySmspListChange");
			break;
		case EMobileSmsMessagingSetReceiveModeCancel:
			ipcBuf = _L8("SetReceiveModeCancel");
			break;
		case EMobileSmsMessagingNotifyReceiveModeChangeCancel:
			ipcBuf = _L8("NotifyReceiveModeChangeCancel");
			break;
		case EMobileSmsMessagingSetMoSmsBearerCancel:
			ipcBuf = _L8("SetMoSmsBearerCancel");
			break;
		case EMobileSmsMessagingNotifyMoSmsBearerChangeCancel:
			ipcBuf = _L8("NotifyMoSmsBearerChangeCancel");
			break;
		case EMobileSmsMessagingAckSmsStoredCancel:
			ipcBuf = _L8("AckSmsStoredCancel");
			break;
		case EMobileSmsMessagingNackSmsStoredCancel:
			ipcBuf = _L8("NackSmsStoredCancel");
			break;
		case EMobileSmsMessagingResumeSmsReceptionCancel:
			ipcBuf = _L8("ResumeSmsReceptionCancel");
			break;
		case EMobileSmsMessagingSendMessageCancel:
			ipcBuf = _L8("SendMessageCancel");
			break;
		case EMobileSmsMessagingReceiveMessageCancel:
			ipcBuf = _L8("ReceiveMessageCancel");
			break;
		case EMobileSmsMessagingGetMessageStoreInfoCancel:
			ipcBuf = _L8("GetMessageStoreInfoCancel");
			break;
		case EMobileSmsMessagingGetSmspListCancel:
			ipcBuf = _L8("GetSmspListCancel");
			break;
		case EMobileSmsMessagingStoreSmspListCancel:
			ipcBuf = _L8("StoreSmspListCancel");
			break;
		case EMobileSmsMessagingNotifySmspListChangeCancel:
			ipcBuf = _L8("NotifySmspListChangeCancel");
			break;
		default:
			ipcBuf = _L8("OTHER");
			break;
		}

	if (aEntering!=EFalse)
		{
		OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_LOGREQUEST1_1, ">>%d,CSimSmsMessaging::%s",aIpc, ipcBuf );
		}
	else
		{
#if (OST_TRACE_CATEGORY & OST_TRACE_CATEGORY_DEBUG)    // to stop the UREL build warnings	
		OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_LOGREQUEST1_2, "<<%d, CSimSmsMessaging::%s with error %d",aIpc, ipcBuf, aError);
#endif		
		}
	}


TInt CSimSmsMessaging::ExtFunc(const TTsyReqHandle aReqHandle,const TInt aIpc, const TDataPackage& aPckg)
/**
 * Sms Read/Send Dispatch Function.
 *
 * If this method returns with an KErr code apart from KErrNone then Etel will 
 * complete and destory the clients request for us. 
 *
 * @param aTsyReqHandle The handle of the request which started the IPC
 * @param aIpc The IPC being requested
 * @param aPackage Package of parameters associated with the IPC
 * @return Standard KErr... codes
 */	{
	TInt ret=KErrNone;//error is only used with LogRequest when at the end of ExtFunc
	LogRequest(ETrue, aIpc, ret);

	switch(aIpc)
		{
	case EMobileSmsMessagingReceiveMessage://4211
		{
		TRAPD(leaveCode, ret = ReceiveMessageL(aReqHandle,aPckg.Des1n(),aPckg.Des2n()));
		if (leaveCode != KErrNone)
			{
			ret = leaveCode;
			}
		}
		break;
		
	case EMobileSmsMessagingGetCaps:
		ret = GetCaps(aReqHandle,aPckg.Des1n());
		break;

	case EMobileSmsMessagingEnumerateMessageStores:
		ret = EnumerateMessagingStores(aReqHandle,aPckg.Des1n());
		break;

	case EMobileSmsMessagingGetMessageStoreInfo:
		ret = GetMessageStoreInfo(aReqHandle,aPckg.Des1n(),aPckg.Des2n());
		break;

	case EMobileSmsMessagingGetReceiveMode:
		ret = GetReceiveMode(aReqHandle,aPckg.Des1n());
		break;
		
	case EMobileSmsMessagingSetReceiveMode:
		ret = SetReceiveMode(aReqHandle,aPckg.Des1n());//4202
		break;

	case EMobileSmsMessagingAckSmsStored:
		ret = AckSmsStored(aReqHandle,aPckg.Des1n(),aPckg.Des2n());
		break;

	case EMobileSmsMessagingNackSmsStored:
		ret = NackSmsStored(aReqHandle,aPckg.Des1n(),aPckg.Des2n());
		break;

	case EMobileSmsMessagingResumeSmsReception:
		ret = ResumeSmsReception(aReqHandle);
		break;

	case EMobileSmsMessagingGetSmspListPhase1:
		ret = GetSmspListPhase1(aReqHandle,aPckg.Des1n(), aPckg.Des2n());
		break;

	case EMobileSmsMessagingGetSmspListPhase2:
		ret = GetSmspListPhase2(aReqHandle,aPckg.Des1n(), aPckg.Des2n());
		break;

	case EMobileSmsMessagingStoreSmspList:
		ret = StoreSmspList(aReqHandle,aPckg.Des1n());
		break;

	case EMobileSmsMessagingSendMessage://4207
		{
		TRAPD(leaveCode, ret = SendMessageL(aReqHandle,aPckg.Des1n(),aPckg.Des2n()));
		if (leaveCode != KErrNone)
			{
			ret = leaveCode;
			}
		}
		break;
	
	//This is used to re-read the configuration without having to restart the TSY
	//For testing purposes
	case EMobileSmsMessagingGetMoSmsBearer:
		//  FALLTHRU

	default:
		ret=KErrNotSupported;
		break;
		}
	LogRequest(EFalse, aIpc, ret);
	return ret;
	}

TBool CSimSmsMessaging::ConstraintEllapsed() const
/**
 * Check if the current contraint has ellapsed - i.e. check that that the number of received SMS
 * messages has reached the expected number for the current constraint.
 * @return ETrue if a constraint has ellapsed
 */
	{
	TBool ellapsed = EFalse;
	if( iConstraints.Count()>0 )
		{
		//check if the number of rx mess has been reached
		ellapsed=(iConstraintRxCnt==iConstraints[iCurrentConstraint].iRxCnt);
		}
	return ellapsed;
	}

TBool CSimSmsMessaging::IpcMatch()
/**
 * Check if the number of Sms sent matches a constraint i.e. the number of messages sent reaches the number
 * of sms to be sent to allow incoming messages to start being simulated.
 * @return ETrue if a match is found
 */
	{
	TInt i;
	TBool constraintEllapsed=ConstraintEllapsed();
	
	// NOTE - call ConstraintEllapsed() before doing loop below as iCurrentConstraint
	// is updated in the loop and so can result in ConstraintEllapsed() giving a 
	// different result
	TInt count = iConstraints.Count(); 
	for(i=0;i<count;++i)
		{
		if(iSmsTxCnt==(iConstraints[i].iIpcCnt))
			{
			iCurrentConstraint=i;
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_IPCMATCH_1, "New Constraint : %d", iCurrentConstraint);
			if(i!=0)
				{
				if( !constraintEllapsed )
					{
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_IPCMATCH_2, "Panic The constraints are overlapping...Compare test code and config file");
					}
				__ASSERT_ALWAYS(constraintEllapsed!=EFalse,SimPanic(EConstraintsOverlapping));
				}
			return ETrue;
			}
		}
	return EFalse;
	}

CTelObject* CSimSmsMessaging::OpenNewObjectByNameL(const TDesC& aName)
/**
 *
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_OPENNEWOBJECTBYNAMEL_1, ">>CSimSmsMessaging::OpenNewObjectByNameL");
	TBuf8<KMaxName> name;
	name.Copy(aName);		// Do simple 16 bit to 8 bit conversion
	for(TInt i=0;i<iSmsStores->Count();i++)
		{
		if(name.MatchF(iSmsStores->At(i)->Name())==0)
			{
			// Base class open
			(void)iSmsStores->At(i)->Open();
			return iSmsStores->At(i);
			}
		}
	User::Leave(KErrNotFound);
	return NULL;
	}

CTelObject* CSimSmsMessaging::OpenNewObjectL(TDes&)
/**
 *
 */
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject::TReqMode CSimSmsMessaging::ReqModeL(const TInt aIpc)
/**
 *	This function returns the Request Mode for the request with the passed IPC value.
 *	The ETel Server provides a function for returning the standard request modes for
 *	the Core API requests.
 */
	{
	CTelObject::TReqMode ret=0;	

	switch(aIpc)
		{
	case EMobileSmsMessagingReceiveMessage:
		ret=KReqModeRePostImmediately;
		break;

	case EMobileSmsMessagingGetCaps:
	case EMobileSmsMessagingSetReceiveMode:
	case EMobileSmsMessagingGetReceiveMode:
	case EMobileSmsMessagingAckSmsStored:
	case EMobileSmsMessagingNackSmsStored:
	case EMobileSmsMessagingResumeSmsReception:
	case EMobileSmsMessagingEnumerateMessageStores:
	case EMobileSmsMessagingGetMessageStoreInfo:
	case EMobileSmsMessagingSendMessage:
	case EMobileSmsMessagingGetSmspListPhase1:
	case EMobileSmsMessagingGetSmspListPhase2:
	case EMobileSmsMessagingStoreSmspList:
		ret=0;
		break;
	case EMobileSmsMessagingGetMoSmsBearer:
		// FALLTHRU
	default:
		User::Leave(KErrNotSupported);
		break;
		}

	return ret;
	}

TInt CSimSmsMessaging::RegisterNotification(const TInt /*aIpc*/)
/**
	The ETel Server calls this function when the first client makes a notification
	request.  If supported by the underlying protocol controlling the
	signalling stack, this can be used to start requesting updates for the relevant
	service.
*/
	{
	return KErrNone;
	}

TInt CSimSmsMessaging::DeregisterNotification(const TInt /*aIpc*/)
/**
	The ETel Server calls this function when the last client that had previously
	made a notification request closes its ETel Server handle.  If supported by
	the underlying protocol controlling the	signalling stack, this can be used
	to stop requesting updates for the relevant service.
*/
	{
	return KErrNone;
	}

TInt CSimSmsMessaging::NumberOfSlotsL(const TInt /*aIpc*/)
/**
 * Return the number of slots that the ETel Server should allocate for buffering requests
 * of the given IPC number.
 */
	{
	return KDefaultNumberOfSlots;
	}

TInt CSimSmsMessaging::CancelService(const TInt aIpc,const TTsyReqHandle /*aTsyReqHandle*/)
/**
 * Cancel an outstanding request.
 */
	{
	switch(aIpc)
		{
	case EMobileSmsMessagingReceiveMessage:
		ReceiveMessageCancel();
		break;
	case EMobileSmsMessagingSendMessage:
		SendMessageCancel();
		break;
	case EMobileSmsMessagingGetSmspListPhase1:
		StoreSmspListCancel();
		break;
	case EMobileSmsMessagingStoreSmspList:
		GetSmspListCancel();
		break;
	case EMobileSmsMessagingAckSmsStored:
	case EMobileSmsMessagingNackSmsStored:
		AckNackCancel();
		break;
	case EMobileSmsMessagingResumeSmsReception:
		ResumeSmsReceptionCancel();
		break;
// These are actually implemented in a synchronous manner, so there's no cancel to perform.
	case EMobileSmsMessagingSetReceiveMode:
	case EMobileSmsMessagingGetReceiveMode:	
		break;
	default:
		break;
		}
	return KErrNone;
	}

void CSimSmsMessaging::Init()
/**
	This function can be used to perform any necessary synchronous initialisation.
*/
	{
	}

TBool CSimSmsMessaging::CanStartRxEvent() const
/** Check to see if the Rx event can be started. Conditions are that there are no constraints
 * and Rx PDUs have been defined, OR SMS messages have been sent and current constraint has 
 * not ellapsed
 * @return ETrue if the Rx event can be started
 */ 
	{
	TBool canStart = EFalse;
	if( (iConstraints.Count()==0 && iSmsRxParameterListGsm->Count()>0) ||
		(iConstraints.Count()>0 && (iSmsTxCnt == iConstraints[iCurrentConstraint].iIpcCnt) && !ConstraintEllapsed()) )
		{
		// No Constraints listed OR current constraint has not ellapsed
		canStart = ETrue;
		}
	return canStart;	
	}

TInt CSimSmsMessaging::ActionRxEventUnstoredClientAck(TSmsRxEvent aRxEvent)
/**
 *	This function, triggered by sms receive events, manages the stated machine of 
 *  receiving Unstored clientacked sms messages
 *  
 */
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_ACTIONRXEVENTUNSTOREDCLIENTACK_1, ">>CSimSmsMessaging::ActionRxEventUnstoredClientAck [iRxState=%d aRxEvent=%d]", iRxState, aRxEvent);
	switch(iRxState)
		{
	case ESmsRxStateIdle:
		{
		if(aRxEvent==ESmsEventPostedRxReq)
			{
			iRxState=ESmsRxStateWaitingForSmsRx;
			if( CanStartRxEvent() )
				{
				// Start the Rx timer to simulate received SMS from network
				StartSmsMtTimer();	
				}
			return KErrNone;
			}
		return KErrGeneral;
		}
	case ESmsRxStateWaitingForSmsRx:
		{
		if(aRxEvent==ESmsEventRxTimer )
			{
			iRxState=ESmsRxStateWaitingForAckNack;
			TInt ret = AttemptSmsRxComplete();	//If successful Etel will repost the request and the state machine may be re-entered
			return ret;
			}
		return KErrGeneral;
		} 
	case ESmsRxStateWaitingForAckNack:
		{
		__ASSERT_ALWAYS(aRxEvent!=ESmsEventRxTimer,SimPanic(EIllegalSmsRxEvent, __LINE__));
		if(aRxEvent==ESmsEventRxAckNack)
			{
			iRxState=ESmsRxStateWaitingForNetworkAckNackResponse;
			if (!iRxTimer->IsActive() && !iRxTimer->Running())
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_ACTIONRXEVENTUNSTOREDCLIENTACK_2, ">>CSimSmsMessaging::ActionRxEventUnstoredClientAck Starting Rx Timer");
				iRxTimer->Start(iAckNackCompletePause,this, ETimerIdSmsMessRx);
				}

			return KErrNone;
			}
		else if(aRxEvent==ESmsEventResume)
			{
			return KErrGeneral;
			}
		} break;
	case ESmsRxStateWaitingForNetworkAckNackResponse:
		if(aRxEvent==ESmsEventRxTimer)
			{
			if(iSmsStoreFull)
				iRxState=ESmsRxStateSuspend;
			else if(iSmsRxReqOutstanding)
				{
				iRxState=ESmsRxStateWaitingForSmsRx;
				StartSmsMtTimer();
				}
			else
				{
				iRxState=ESmsRxStateIdle;
				StartSmsMtTimer();
				}
			CompletePendingReq();
			return KErrNone;
			}
		if(aRxEvent==ESmsEventPostedRxReq)
			return KErrNone;
		return KErrGeneral;
	case ESmsRxStateSuspend:
		{
		__ASSERT_ALWAYS(aRxEvent!=ESmsEventRxTimer,SimPanic(EIllegalSmsRxEvent, __LINE__));
		if(aRxEvent==ESmsEventResume)
			{
			iRxState=ESmsRxStateWaitingForNetworkResumeResponse;
			iRxTimer->Start(iResumeCompletePause,this, ETimerIdSmsMessResumeReception);
			return KErrNone;
			}
		return KErrGeneral;
		}
	case ESmsRxStateWaitingForNetworkResumeResponse:
		{
		if(aRxEvent==ESmsEventPostedRxReq)
			return KErrNone;
		return KErrGeneral;
		}
	default:
		return KErrGeneral;
		}
	return KErrNone;
	}

TInt CSimSmsMessaging::ActionRxEventUnstoredPhoneAck(TSmsRxEvent aRxEvent)
/**
 *	This function, triggered by sms receive events, manages the stated machine of 
 *  receiving Unstored Phone acked sms messages
 *  
 */
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_ACTIONRXEVENTUNSTOREDPHONEACK_1, ">> ActionRxEventUnstoredPhoneAck Enter function. Event=%d, State=%d",aRxEvent,iRxState);
	
	TInt ret = KErrGeneral;
	
	switch(iRxState)
		{
	case ESmsRxStateIdle:
		{
		if(aRxEvent==ESmsEventPostedRxReq)
			{
			iRxState=ESmsRxStateWaitingForSmsRx;
			if( CanStartRxEvent() )
				{
				// Start the Rx timer to simulate received SMS from network
				StartSmsMtTimer();	
				}
			ret = KErrNone;
			}
		} break;
	case ESmsRxStateWaitingForSmsRx:
		{
		if(aRxEvent==ESmsEventRxTimer )
			{
			iRxState=ESmsRxStateIdle;
			ret = AttemptSmsRxComplete();	//If successful Etel will repost the request and the state machine may be re-entered
			}
		} break; 
	default: 
		// Do nothing - return default value of KErrGeneral
		break;
		}
	return ret;
	}

TInt CSimSmsMessaging::ActionRxEventStored(TSmsRxEvent aRxEvent)
/**
 *	This function, triggered by sms receive events, manages the stated machine of 
 *  receiving stored Phone acked  sms messages
 *  
 */
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_ACTIONRXEVENTSTORED_1, ">> ActionRxEventStored Enter function. Event=%d, State=%d",aRxEvent,iRxState);
	TInt ret = KErrGeneral;
	
	switch(iRxState)
		{
	case ESmsRxStateIdle:
		{
		if(aRxEvent==ESmsEventPostedRxReq)
			{
			iRxState=ESmsRxStateWaitingForSmsRx;
			if( CanStartRxEvent() )
				{
				// Start the Rx timer to simulate received SMS from network
				StartSmsMtTimer();	
				}
			ret = KErrNone;
			}
		} break;
	case ESmsRxStateWaitingForSmsRx:
		{
		if(aRxEvent==ESmsEventRxTimer )
			{
			iRxState=ESmsRxStateIdle;
			ret = AttemptSmsRxComplete();	//If successful Etel will repost the request and the state machine may be re-entered
			if( ret != KErrNone )
				{
				//the message wasn't stored ->ignore it and re-run the network event
				iRxState=ESmsRxStateWaitingForSmsRx;
				StartSmsMtTimer();
				}
			ret = KErrNone;	
			}
		} break; 
	default: 
		// Do nothing - return default value of KErrGeneral
		break;
		}
	return ret;
	}

TInt CSimSmsMessaging::ReceiveMessageL(const TTsyReqHandle aReqHandle, TDes8* aSmsPdu, TDes8* aParam2)
/**
 * Process a Receive Message request.  This boils down to recording the parameters for later completion.
 * The mode of operation where first an SMS receive request is made and then later an SMS
 * receive event is simulated is supported.  If an SMS receive event is simulated and there is
 * no receive request outstanding, the simulated incoming SMS will be discarded.  This assumption
 * may need to be revisited later.
 */
	{
	if (iSmsRxReqOutstanding!=EFalse)//Check if another client posts the request
		{
		ReqCompleted(aReqHandle, KErrInUse);
		return KErrNone;
		}
	iSmsRxReqHandle=aReqHandle;
	iSmsRxPdu=aSmsPdu;
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg* aAttribPckg=(RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg*)aParam2;
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1& attrib=(*aAttribPckg)();

	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(attrib);
	if(err != KErrNone)
		{
		iPhone->ReqCompleted(aReqHandle, err);
		return KErrNone;
		}

	iSmsRxAttrib=&attrib;
	iSmsRxReqOutstanding=ETrue;
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_RECEIVEMESSAGEL_1, ">>ReceiveMessageL. aSmsPdu&=0x%x, aParam2&=0x%x",(TUint)aSmsPdu,(TUint)iSmsRxAttrib);
	
	//  Print received PDU to simTSY log.
	__ASSERT_ALWAYS(iSmsReceiveMode!=RMobileSmsMessaging::EReceiveModeUnspecified,SimPanic(EMobileSmsMessagingPhoneNotSetToAReceiveMode));
	TInt ret=0;
	switch (iSmsReceiveMode)//swich which state machine to enter
		{
	case RMobileSmsMessaging::EReceiveUnstoredClientAck:
		ret=ActionRxEventUnstoredClientAck(ESmsEventPostedRxReq);
		break;
	case RMobileSmsMessaging::EReceiveUnstoredPhoneAck:
		ret=ActionRxEventUnstoredPhoneAck(ESmsEventPostedRxReq);
		break;
	case RMobileSmsMessaging::EReceiveStored:
		ret=ActionRxEventStored(ESmsEventPostedRxReq);
		break;
	default:
		ret=KErrNotSupported;
		}//end switch

	if(ret==KErrNone)
		{
		
		}
	else
		ReqCompleted(aReqHandle,ret);
	return KErrNone;
	}

void CSimSmsMessaging::ReceiveMessageCancel()
/*
 * Cancel an outstanding Receive Message request.
 */
	{
	if(iSmsRxReqOutstanding)
		{
		iRxTimer->Cancel();
		iSmsRxReqOutstanding=EFalse;
		iRxState=ESmsRxStateIdle;
		ReqCompleted(iSmsRxReqHandle,KErrCancel);
		}
	}

TInt CSimSmsMessaging::AckSmsStored(const TTsyReqHandle aReqHandle,TDes8* aMsg,TDes8* aFullFlagPckg)
/**
 *	In response to an incoming sms message the sms stack Acks/Nacks the message
 *  This function implements the Ack action.
 *  The flag aFullFlagPckg if set informs the SC that the client has no space to store this message
 *  and hence that it should retry once instructed by the ME
 */
	{
	TPckg<TBool>* fullFlagPckg=(TPckg<TBool>*)aFullFlagPckg;
	TBool& fullFlag=(*fullFlagPckg)();

	//  SMS-DELIVER-REPORT TPDU check 
	_LIT8(emptyDesc,"");
	if(aMsg->Compare(emptyDesc) != KErrNone)
		{
		RMobileSmsMessaging::TMobileSmsGsmTpdu reportPdu;
		reportPdu.Zero();
		if (CSimTsyMode::GetMode() != CSimTsyMode::ECdmaV1)
  			{
 			reportPdu = iSmsRxParameterListGsm->At(iSmsRxCnt-1).iDeliveryReport;
  			}

					
		if (reportPdu.Length() > 0)	// check if SMS-DELIVER-REPORT TPDU in config file is present
			{
			if(aMsg->Match(reportPdu)!=0)//check if the transmitted pdu and the one in config file are not identical
				{
				ReqCompleted(aReqHandle,KErrCorrupt);
				return KErrNone;
				}
			}
		}

	iSmsStoreFull=fullFlag;		// Record the "Store Full" status for future state transitions.
	TInt ret=ActionRxEventUnstoredClientAck(ESmsEventRxAckNack);

	// Handle any state transition errors now.  Simulated Ack/Nack failures are not supported yet...
	if(ret!=KErrNone)
		{
		ReqCompleted(aReqHandle,ret);
		return KErrNone;
		}
	iPendingReqHandle=aReqHandle;
	return KErrNone;
	}

TInt CSimSmsMessaging::NackSmsStored(const TTsyReqHandle aReqHandle,TDes8* aMsg,TDes8* aRpCausePckg)
/**
 *	In response to an incoming sms message the sms stack Acks/Nacks the message
 *  This function implements the NAck action.
 *  aRpCausePckg must be filled in with the reason
 */
	{
	TPckg<TInt>* rpCausePckg=(TPckg<TInt>*)aRpCausePckg;
	TInt& rpCause=(*rpCausePckg)();

	//  SMS-DELIVER-REPORT TPDU check 
	_LIT8(emptyDesc,"");
	if(aMsg->Compare(emptyDesc) != KErrNone)
		{
		RMobileSmsMessaging::TMobileSmsGsmTpdu reportPdu;
		reportPdu.Zero();
		if (CSimTsyMode::GetMode() != CSimTsyMode::ECdmaV1)
  			{
  			reportPdu = iSmsRxParameterListGsm->At(iSmsRxCnt-1).iDeliveryReport;
  			}

			
		if (reportPdu.Length() > 0)	// check if SMS-DELIVER-REPORT TPDU in config file is present
			{
			if(aMsg->Match(reportPdu)!=0)//check if the transmitted pdu and the one in config file are not identical
				{
				ReqCompleted(aReqHandle,KErrCorrupt);
				return KErrNone;
				}
			}
		}

	if(rpCause==KErrGsmSMSMemoryCapacityExceeded)
		iSmsStoreFull=ETrue;
	else
		iSmsStoreFull=EFalse;

	TInt ret=ActionRxEventUnstoredClientAck(ESmsEventRxAckNack);

	// Handle any state transition errors now.  Simulated Ack/Nack failures are not supported yet...
	if(ret!=KErrNone)
		{
		ReqCompleted(aReqHandle,ret);
		return KErrNone;
		}

	iPendingReqHandle=aReqHandle;
	return KErrNone;
	}


void CSimSmsMessaging::AckNackCancel()
/*
 * Cancel an outstanding Receive Message request.
 */
	{
	//This method does nothing,, a complex implementation would have to be used to simulate a real tsy
	}


TInt CSimSmsMessaging::ResumeSmsReception(const TTsyReqHandle aReqHandle)
/**
 *  After the sms reception has been suspended(store full), if the client frees some space
 *  the ME will inform the SC that reception can resume
 *
 */
	{
	TInt ret=ActionRxEventUnstoredClientAck(ESmsEventResume);
// In case of state transition error, return the error code.
// Resume failures cannot be simulated by the tsy yet...
	if(ret!=KErrNone)
		{
		ReqCompleted(aReqHandle,ret);
		return KErrNone;
		}
	iPendingReqHandle=aReqHandle;
	return KErrNone;
	}

void CSimSmsMessaging::ResumeSmsReceptionCancel()
	{
	//Does nothing, a complex implementation would have to be used to simulate a real tsy
	}

//**********************************************
//send
//*********************************************


TInt CSimSmsMessaging::ActionTxEvent(TSmsTxEvent aTxEvent)
/**
 *  This function actions the sms message sending state machine. It is triggered by sms sending events
 *
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_ACTIONTXEVENT_1, ">>ActionTxEvent ");
	switch(iTxState)
	{
	case ESmsTxStateIdle:
		__ASSERT_ALWAYS(aTxEvent!=ESmsEventSubmitReportReceived,SimPanic(EIllegalSmsTxEvent));
		if(aTxEvent==ESmsEventSendReq)
			{
			iTxState=ESmsTxStateWaitingForSubmitReport;
			iTxTimer->Start(iSmsTxPause,this, ETimerIdSmsMessTx);
			}
		break;
	case ESmsTxStateWaitingForSubmitReport:
		__ASSERT_ALWAYS(aTxEvent!=ESmsEventSendReq,SimPanic(EIllegalSmsTxEvent));	
		if(aTxEvent==ESmsEventSubmitReportReceived)
			{
			iTxState=ESmsTxStateIdle;

			PopulateSmsTxAttrib(iSmsTxAttrib);				
			CompleteTxPendingReq(KErrNone);			
			}
		break;
	}
	return KErrNone;
	}

TInt CSimSmsMessaging::SendMessageL(const TTsyReqHandle aReqHandle,TDes8* aSmsPdu,TDes8* aParam2)
/**
 *	This function simulates the transmission of an sms from the ME to the SC.
 *  It returns directly if 
 *  -The Sms message description(SmsTx)tag in the config file specifies that this sms message should return with this error 
 *  -The Pdu is corrupted
 *  Otherwise it start the sms message sending state machine
 * 
 */

	{
    iSmsTxReqHandle=aReqHandle;
    RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg* aAttribPckg=(RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg*)aParam2;
	RMobileSmsMessaging::TMobileSmsSendAttributesV1& attrib=(*aAttribPckg)();
		
	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(attrib);
	if(err != KErrNone)
		{
		iPhone->ReqCompleted(aReqHandle, err);
		return KErrNone;
		}
		
	iSmsTxAttrib=&attrib;
#if (OST_TRACE_CATEGORY & OST_TRACE_CATEGORY_DEBUG)
		LogTMobileSmsAttributesV1(*iSmsTxAttrib);
#endif // _DEBUG
		

	//check if messages defined in cfg file
	TInt count = 0;	
	count = iSmsTxParametersListGsm->Count();  
	
	if (count<=iSmsTxCnt)
		return KErrTotalLossOfPrecision;

	//check if expected error
	//TInt err = KErrNone;
	err = KErrNone;
	err = iSmsTxParametersListGsm->At(iSmsTxCnt).iExpectedError;		
	if (err!=KErrNone)
		{
		PopulateSmsTxAttrib(iSmsTxAttrib);			
		CompleteTxPendingReq(err);
		return KErrNone;
		}

		//check if a non null pdu is defined in the config file
	RMobileSmsMessaging::TMobileSmsGsmTpdu octetPdu;
	octetPdu = iSmsTxParametersListGsm->At(iSmsTxCnt).iPdu;

	RMobileSmsMessaging::TMobileSmsGsmTpdu pduWith0;
	TUint8 zero=0;
	pduWith0.Append(&zero, 1);
	if(octetPdu!=pduWith0)//if the pdu in config file is 00, skip the pdu check
		{
			if(aSmsPdu->Match(octetPdu)!=0)//check if the transmitted pdu and the one in config file are identical
				{
				CompleteTxPendingReq(KErrCorrupt);
				return KErrNone;
				}
		}	

	ActionTxEvent(ESmsEventSendReq);
	return KErrNone;
	}


void CSimSmsMessaging::SendMessageCancel()
/*
 * Cancel an outstanding send Message request.
 */
	{
	if(iTxState==ESmsTxStateWaitingForSubmitReport)
		{
		iTxTimer->Cancel();
		iTxState=ESmsTxStateIdle;
		CompleteTxPendingReq(KErrCancel);
		}
	}

void CSimSmsMessaging::CompletePendingReq()
	{
	ReqCompleted(iPendingReqHandle,KErrNone);
	}

TInt CSimSmsMessaging::AttemptSmsRxComplete()
/**
 * Complete an outstanding SMS message receive request if one is outstanding.  If there is
 * no request outstanding, the message will be discared.
 * update the constraint count
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_ATTEMPTSMSRXCOMPLETE_1, ">>AttemptSmsRxComplete ");

	if (CSimTsyMode::GetMode() != CSimTsyMode::ECdmaV1)
		{
		iConstraintRxCnt++;
		}

	TInt ret=KErrNone;
	if(iSmsRxReqOutstanding)
		{
		if (iSmsReceiveMode==RMobileSmsMessaging::EReceiveStored)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_ATTEMPTSMSRXCOMPLETE_2, ">>Populating SMS Structures. iSmsRxPdu&=0x%x, iSmsRxAttrib&=0x%x, iSmsRxCnt=%d.",(TUint)iSmsRxPdu,(TUint)iSmsRxAttrib,iSmsRxCnt);
			RMobileSmsStore::TMobileGsmSmsEntryV1 sms;

			*iSmsRxPdu=iSmsRxParameterListGsm->At(iSmsRxCnt).iPdu;
			
			sms.iMsgData=RMobileSmsMessaging::TMobileSmsGsmTpdu(*iSmsRxPdu);
			sms.iServiceCentre.iTelNumber.Copy(iSmsRxParameterListGsm->At(iSmsRxCnt).iSca);
		
			sms.iServiceCentre.iTypeOfNumber=RMobilePhone::EInternationalNumber;//SmsMessaging only deals with international numbers
			sms.iServiceCentre.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
			sms.iMsgStatus=RMobileSmsStore::EStoredMessageUnread;
			ret = iSmsStores->At(0)->StoreIncomingMessage(&sms);//todo if several stores find the right one
			if (ret==KErrNone)
				{
				iSmsRxAttrib->iStore.Copy(iSmsStores->At(0)->Name());
				iSmsRxAttrib->iStoreIndex=sms.iIndex;
				}
			}
		else
			{			
			*iSmsRxPdu = iSmsRxParameterListGsm->At(iSmsRxCnt).iPdu;				
			}

		if (ret==KErrNone)
			{
			iSmsRxReqOutstanding=EFalse;			
			PopulateSmsRxAttrib(iSmsRxParameterListGsm->At(iSmsRxCnt).iSca,iSmsRxAttrib);	
							
			ReqCompleted(iSmsRxReqHandle,ret);
			iSmsRxCnt++;
			}
		}
	return ret;
	}

void CSimSmsMessaging::CompleteTxPendingReq(TInt aError)
/**
 * Complete a SmsTx request and increments the SmsTx counter
 *
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_COMPLETETXPENDINGREQ_1, ">>CompleteTxPendingReq ");
	iSmsTxCnt++;
	if(IpcMatch())
		{
		iConstraintRxCnt=0;
		if( iRxState == ESmsRxStateSuspend )
		    {
		    // A previous rx message was NACKed due memory full and 
		    // SIM TSY is waiting for resume event from client - as
		    // a new message needs to be received, change states
		    // to allow receipt of the message.
		    if( iSmsRxReqOutstanding )
		        {
		        iRxState = ESmsRxStateWaitingForSmsRx;
		        }
		    else
		        {
		        iRxState = ESmsRxStateIdle;
		        }
		    }
		if( iSmsRxReqOutstanding )
			{
			// Client has a pending receive request - safe to start Rx timer
			// to simulate received SMS from network.
			StartSmsMtTimer();
			}
		else
			{
			// No pending client receive request - need to wait for it before
			// simulating received SMS from network.
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_COMPLETETXPENDINGREQ_2, " - no pending receive req from client - do not start Rx timer");
			}
		}
	ReqCompleted(iSmsTxReqHandle, aError);
	}

void CSimSmsMessaging::StartSmsMtTimer()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_STARTSMSMTTIMER_1, ">>StartSmsMtTimer ");
	TInt count = 0;
	count = iSmsRxCnt<iSmsRxParameterListGsm->Count();
			
		if(count)		// Check that there are more messages defined in the config file.
			{
			if((iConstraints.Count()==0) ||	(iConstraintRxCnt<iConstraints[iCurrentConstraint].iRxCnt))	// If there are no constraints, or there are constraints and they're not exhausted.
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_STARTSMSMTTIMER_2, ">>StartSmsMtTimer Starting");
				iRxTimer->Start(iSmsRxPeriod,this, ETimerIdSmsMessRx);
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_STARTSMSMTTIMER_3, ">>StartSmsMtTimer Started");
				}
			}
	}


void CSimSmsMessaging::PopulateSmsRxAttrib(const TDesC8& aAsciiScaAddr,RMobileSmsMessaging::TMobileSmsReceiveAttributesV1* aAttrib)
/**
 * Populate the SMS Rx Attributes from an ASCII respresentation, such as that stored in the configuration file.
 * This involves checking the address for a leading '+' character and setting the TON and NPI
 * accordingly.  The address can then be copied into the iTelNumber structure.
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_POPULATESMSRXATTRIB_1, ">>PopulateSmsRxAttrib, ");
	aAttrib->iOriginator.iTelNumber.SetLength(0);
	
	switch (iSmsReceiveMode)
		{
	case RMobileSmsMessaging::EReceiveUnstoredClientAck:
		aAttrib->iStatus=RMobileSmsMessaging::EMtMessageUnstoredClientAck;
		aAttrib->iStore.SetLength(0);
		aAttrib->iStoreIndex=-1;
		break;
	case RMobileSmsMessaging::EReceiveUnstoredPhoneAck:
		aAttrib->iStatus=RMobileSmsMessaging::EMtMessageUnstoredPhoneAck;
		aAttrib->iStore.SetLength(0);
		aAttrib->iStoreIndex=-1;
		break;
	case RMobileSmsMessaging::EReceiveStored:
		aAttrib->iStatus=RMobileSmsMessaging::EMtMessageStored;
		break;
	
	default:
		break;
		}
	aAttrib->iFlags=RMobileSmsMessaging::KGsmServiceCentre | RMobileSmsMessaging::KSmsDataFormat | RMobileSmsMessaging::KIncomingStatus;
	aAttrib->iDataFormat=RMobileSmsMessaging::EFormatGsmTpdu;

	if(aAsciiScaAddr.Length()>0)
		{
		aAttrib->iGsmServiceCentre.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
		aAttrib->iGsmServiceCentre.iTypeOfNumber=RMobilePhone::EInternationalNumber;
		aAttrib->iGsmServiceCentre.iTelNumber.Copy(aAsciiScaAddr);
		}
	}


void CSimSmsMessaging::PopulateSmsTxAttrib(RMobileSmsMessaging::TMobileSmsAttributesV1* aAttrib)
/**
 * Populate the SMS Tx Attributes for SMS from an ASCII respresentation, such as that stored in the configuration file.
 * - Message Identifier
 * - TL Ack
 */
	{
	if (iSmsModeCaps == RMobileSmsMessaging::KCapsGsmSms)
		{
		RMobileSmsMessaging::TMobileSmsSendAttributesV1* attrib = static_cast<RMobileSmsMessaging::TMobileSmsSendAttributesV1*>(aAttrib);
		attrib->iMsgRef = TUint16(iSmsTxParametersListGsm->At(iSmsTxCnt).iRef);	

		attrib->iFlags = RMobileSmsMessaging::KMessageReference;
		
		if (iSmsControlCaps & RMobileSmsMessaging::KCapsSendWithAck)
			{			
			attrib->iSubmitReport=iSmsTxParametersListGsm->At(iSmsTxCnt).iSubmitReport;
			attrib->iFlags |= RMobileSmsMessaging::KGsmSubmitReport;
			}
		}
	}

void CSimSmsMessaging::PopulateSmsTxAttrib(RMobileSmsMessaging::TMobileSmsSendAttributesV1* aAttrib)
/**
 * Populate the SMS Tx Attributes from an ASCII respresentation, such as that stored in the configuration file.
 * -Message Reference
 * -SUBMIT_REPORT_TPDU
 */
	{
	 aAttrib->iMsgRef = TUint16(iSmsTxParametersListGsm->At(iSmsTxCnt).iRef);
	 aAttrib->iFlags = RMobileSmsMessaging::KMessageReference;
	
	 if (iSmsControlCaps & RMobileSmsMessaging::KCapsSendWithAck)
		{
		aAttrib->iSubmitReport=iSmsTxParametersListGsm->At(iSmsTxCnt).iSubmitReport;
		aAttrib->iFlags |= RMobileSmsMessaging::KGsmSubmitReport;
		}
	}

TInt CSimSmsMessaging::GetCaps(const TTsyReqHandle aReqHandle,TDes8* aPckg)
/**
 * Process a request to retrieve the caps.  Currently, only the KCapsReceiveUnstoredClientAck
 * mode is supported.
 */
	{
	RMobileSmsMessaging::TMobileSmsCapsV1Pckg* capsPckg=(RMobileSmsMessaging::TMobileSmsCapsV1Pckg*)aPckg;
	RMobileSmsMessaging::TMobileSmsCapsV1& caps=(*capsPckg)();

	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(caps);
	if(err != KErrNone)
		{
		iPhone->ReqCompleted(aReqHandle, err);
		return KErrNone;
		}

	 	caps.iSmsMode = RMobileSmsMessaging::KCapsGsmSms;
		
	caps.iSmsControl=iSmsControlCaps;
	ReqCompleted(aReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimSmsMessaging::GetReceiveMode(const TTsyReqHandle aReqHandle,TDes8* aPckg)
/**
 * Process a request to retrieve the current SMS receive mode.  Only the
 * KCapsReceiveUnstoredClientAck mode is supported.
 */
	{
	TPckg<RMobileSmsMessaging::TMobileSmsReceiveMode>* modePckg=(TPckg<RMobileSmsMessaging::TMobileSmsReceiveMode>*)aPckg;
	RMobileSmsMessaging::TMobileSmsReceiveMode& mode=(*modePckg)();
	mode=iSmsReceiveMode;
	ReqCompleted(aReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimSmsMessaging::SetReceiveMode(const TTsyReqHandle aReqHandle,TDes8* aPckg)
/**
 * Process a request to set the current SMS receive mode.  Only the
 * KCapsReceiveUnstoredClientAck mode is supported.
 */
	{
	TPckg<RMobileSmsMessaging::TMobileSmsReceiveMode>* modePckg=(TPckg<RMobileSmsMessaging::TMobileSmsReceiveMode>*)aPckg;
	RMobileSmsMessaging:: TMobileSmsReceiveMode& mode=(*modePckg)();

	TInt result(KErrNone);

	switch (mode)
		{
		case RMobileSmsMessaging::EReceiveUnstoredClientAck:
			{
			if(iSmsControlCaps & RMobileSmsMessaging::KCapsReceiveUnstoredClientAck)
				{
				if ((iRxState==ESmsRxStateIdle) || (iRxState==ESmsRxStateWaitingForSmsRx))//can only change mode in these states
					{
					iSmsReceiveMode=RMobileSmsMessaging::EReceiveUnstoredClientAck;
					ReqCompleted(aReqHandle,KErrNone);
					}
				else 
					{
					if(iSmsReceiveMode==RMobileSmsMessaging::EReceiveUnstoredClientAck)//already in this state
						ReqCompleted(aReqHandle,KErrNone);
					else
						ReqCompleted(aReqHandle,KErrNotSupported);
					}
				}
			else ReqCompleted(aReqHandle,KErrNotSupported); 
			break;
			}

		case RMobileSmsMessaging::EReceiveUnstoredPhoneAck:
			{
			if(iSmsControlCaps & RMobileSmsMessaging:: KCapsReceiveUnstoredPhoneAck)
				{
				if ((iRxState==ESmsRxStateIdle) || (iRxState==ESmsRxStateWaitingForSmsRx))//can only change mode in these states
					{
					iSmsReceiveMode=RMobileSmsMessaging::EReceiveUnstoredPhoneAck;
					ReqCompleted(aReqHandle,KErrNone);
					}
				else
					{
					if (iSmsReceiveMode==RMobileSmsMessaging::EReceiveUnstoredPhoneAck)//already in this state
						ReqCompleted(aReqHandle,KErrNone);
					else
						ReqCompleted(aReqHandle,KErrNotSupported);
					}
				}
			else
				ReqCompleted(aReqHandle,KErrNotSupported); 	
			break;
			}

		case RMobileSmsMessaging::EReceiveStored:
			{
			if(iSmsControlCaps & RMobileSmsMessaging:: KCapsReceiveStored)
				{
				if ((iRxState==ESmsRxStateIdle) || (iRxState==ESmsRxStateWaitingForSmsRx))//can only change mode in these states
					{
					iSmsReceiveMode=RMobileSmsMessaging::EReceiveStored;
					ReqCompleted(aReqHandle,KErrNone);
					}
				else
					{
					if((iSmsReceiveMode==RMobileSmsMessaging::EReceiveStored))//already in this state
						ReqCompleted(aReqHandle,KErrNone);
					else
						ReqCompleted(aReqHandle,KErrNotSupported);
					}
				}
			else ReqCompleted(aReqHandle,KErrNotSupported); 
			break;
			}
		default:
			result = KErrNotSupported;
		}
	
	return result;
	}

TInt CSimSmsMessaging::EnumerateMessagingStores(TTsyReqHandle aReqHandle,TDes8* aPckg)
/**
 * Process a request to retrieve the number of message stores supported.
 * @param aReqHandle	The TSY request handle associated with this request.
 * @param aPckg			The parameter package containing the count variable to be populated and
 *						returned.
 * @return TInt			Standard error value.
 */
	{
	TPckg<TInt>* countPckg=(TPckg<TInt>*)aPckg;
	TInt& count=(*countPckg)();
	count=iSmsStores->Count();
	ReqCompleted(aReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimSmsMessaging::GetMessageStoreInfo(TTsyReqHandle aReqHandle,TDes8* aPckg1, TDes8* aPckg2)
/**
 * Retrieve information about an indexed SMS Message Store.
 * @param aReqHandle	The TSY request handle associated with this request.
 * @param aPckg1		The parameter package containing the index of the SMS Store for which
 *						information is going to be retrieved.
 * @param aPckg2		The parameter package in which the retrieved SMS Store information will
 *						be passed back to the client.
 * @return TInt			Standard error value.
 */
	{
	TPckg<TInt>* indexPckg=(TPckg<TInt>*)aPckg1;
	TInt& index=(*indexPckg)();
	TPckg<RMobilePhoneStore::TMobilePhoneStoreInfoV1>* infoPckg=(TPckg<RMobilePhoneStore::TMobilePhoneStoreInfoV1>*)aPckg2;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1& info=(*infoPckg)();

	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(info);
	if(err != KErrNone)
		{
		iPhone->ReqCompleted(aReqHandle, err);
		return KErrNone;
		}

	if((index<0) || (index>=iSmsStores->Count()))
		{
		ReqCompleted(aReqHandle,KErrArgument);
		return KErrNone;
		}

	info.iType=RMobilePhoneStore::EShortMessageStore;
	info.iTotalEntries=iSmsStores->At(index)->MaxSlots();
	info.iUsedEntries=iSmsStores->At(index)->UsedEntries();
	info.iCaps= iSmsStores->At(index)->StoreCaps();
	info.iName.Copy(iSmsStores->At(index)->Name());
	ReqCompleted(aReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimSmsMessaging::GetSmspListPhase1(const TTsyReqHandle aTsyReqHandle,
                                            TDes8* aParam1,TDes8* aParam2)
/** Get SMSP List Phase 1 
 *
 * If the GetSmspListPhase1L should leave this method takes care of that and 
 * makes a premature ReqCompleted to the client.
 *
 * @param aTsyReqHandle the request ID 
 * @param aClient The client sends down a handle that is saved together with the 
 *				  list so the list can be returned to the right client in phase 2.
 * @param aBufSiz The size of the retrieved network list. The size is set in 
 * @return error code. 
 */
	{
	if(iSmspBusy==EFalse)
		{
		iSmspBusy=ETrue;
		UNPACK_PCKG(clientId,aParam1,RMobilePhone::TClientId);
		UNPACK_PCKG(bufSize,aParam2,TInt);
		TRAPD(leaveCode,GetSmspListPhase1L(aTsyReqHandle,clientId,bufSize));
		if (leaveCode)
			return leaveCode;
		return KErrNone;
		}
	else
		return KErrInUse;

	}

void CSimSmsMessaging::GetSmspListPhase1L(TTsyReqHandle aTsyReqHandle, 
											 RMobilePhone::TClientId& aClientId, 
											 TInt& aBufSize)
/** Get SMSP List Phase 1 
 *
 * @param aTsyReqHandle the request ID 
 * @param aClient The client sends down a handle that is saved together with the 
 *				  list so the list can be returned to the right client in phase 2.
 * @param aBufSiz The size of the smsp list. The size is set in 
 * @return error code. 
 */
	{
	// just check and remove if there are already existing entries from the 
	// same client
	TInt numberOfLists = iSmspReadAll->Count();
	// Find the get smsplists attempts from this client, starting from end.  
	for (TInt i = numberOfLists-1; i >= 0; --i)
		{
		CListReadAllAttempt* readOld=iSmspReadAll->At(i);
		if ((readOld->iClient.iSessionHandle==aClientId.iSessionHandle) &&
		    (readOld->iClient.iSubSessionHandle==aClientId.iSubSessionHandle))
			{
			iSmspReadAll->Delete(i);
			}
		}
    // once we have cleaned all then we can proceed...
	iSmspReqHandle=aTsyReqHandle;
	// Store the streamed list and the client ID
	
	CListReadAllAttempt* read=CListReadAllAttempt::NewL(aClientId, aTsyReqHandle);
	CleanupStack::PushL(read);
	
	read->iListBuf = iSmspEntries->StoreLC();
	CleanupStack::Pop(read->iListBuf); // pop the CBufFlat allocated by StoreLC

	iSmspReadAll->AppendL(read);
	CleanupStack::Pop(read); // pop the CListReadAllAttempt

// return the CBufFlat's size to client
	aBufSize=(read->iListBuf)->Size();

//	CleanupStack::PopAndDestroy(); // pop&destroy list

	iSmspTimer->Start(iSmspBatchPause,this, ETimerIdSmsMessSmsp);
	}

TInt CSimSmsMessaging::GetSmspListPhase2(const TTsyReqHandle aTsyReqHandle,
                                            TDes8* aParam1,TDes8* aParam2)
/** Get SMSP List phase 2 
 *
 * In this metod the list which was retrieved during phase 1 is copied to 
 * the memory which the client has allocated for this purose.
 * @param aTsyReqHandle		Const pointer to the request ID 
 * @param aClient			Handle to the client which list we are looking for.
 * @param aBuf				Pointer to the memory that the etelmm has allocated. 
 * @return error code. 
 */
	{	
	UNPACK_PCKG(clientId,aParam1,RMobilePhone::TClientId);
	TInt numberOfLists = iSmspReadAll->Count();
	// Find the get smsplists attempts from this client  
	for (TInt i = 0; i < numberOfLists; ++i)
		{
		CListReadAllAttempt* read=iSmspReadAll->At(i);
		if ((read->iClient.iSessionHandle==clientId.iSessionHandle) &&
		    (read->iClient.iSubSessionHandle==clientId.iSubSessionHandle))
			{
			TPtr8 bufPtr((read->iListBuf)->Ptr(0));
			aParam2->Copy(bufPtr);						// Copy the streamed list to the client
			delete read;
			iSmspReadAll->Delete(i);
			ReqCompleted(aTsyReqHandle,KErrNone);	// Completes the retrieval of a network list succesfully.
			return KErrNone;
			}
		}
	return(KErrNotFound);
	}

TInt CSimSmsMessaging::StoreSmspList(const TTsyReqHandle aTsyReqHandle, TDes8* aBuffer)
/**
 *	This function stores a new smsp list 
 *  warning the new smsp list will only exist at run time. If the tsy is reloaded the smsp list 
 *  will go back to the one defined in the configuration file
 *  @param aBuffer Descriptor containing the new smsp list  
 */
	{
	if (iSmspBusy==EFalse)
		{
		iSmspBusy=ETrue;
		TRAPD(leaveCode,iSmspEntries->RestoreL(*aBuffer));
		if (leaveCode)
			return leaveCode;
		iSmspReqHandle=aTsyReqHandle;
		iSmspTimer->Start(iSmspBatchPause,this, ETimerIdSmsMessSmsp);
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_STORESMSPLIST_1, "<<StoreSmsList,Exit function");
		return KErrNone;
		}
	else
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_STORESMSPLIST_2, "<<StoreSmsList,Exit function");
		return KErrInUse;
	
	}

void CSimSmsMessaging::StoreSmspListCancel()
/*
 * Cancel an outstanding store smspList request
 * as iSmspBusy mutexes get and store, only 1 request handle wil exist at a time 
 */
	{
	if(iSmspBusy != EFalse)
		{
		iSmspTimer->Cancel();
		iSmspBusy=EFalse;
		ReqCompleted(iSmspReqHandle,KErrNone);//The timer is cancelled but data has already been stored
		}
	}

void CSimSmsMessaging::GetSmspListCancel()
/*
 * Cancel an outstanding get smspList request.
 * as iSmspBusy mutexes get and store, only 1 request handle wil exist at a time 
 */
	{
	if(iSmspBusy != EFalse)
		{
		iSmspTimer->Cancel();
		iSmspBusy=EFalse;
		ReqCompleted(iSmspReqHandle,KErrCancel);
		}
	}

void CSimSmsMessaging::TimerCallBack(TInt aId)
/**
 * Process a timer call back event.  
 * @param aId Contains the Id of the timer that triggered the event
 * 
 */
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_TIMERCALLBACK_1, ">>CSimSmsMesaging::TimerCallBack IN [aId=%d iSmsReceiveMode=%d]", aId, iSmsReceiveMode);
	switch(aId)
		{
		case ETimerIdSmsMessTx:
			{
			TInt ret=ActionTxEvent(ESmsEventSubmitReportReceived);
			__ASSERT_ALWAYS(ret==KErrNone,SimPanic(EIllegalSmsTxEvent));	// There should be no error from this action, but to check...
			break;
			}
		case ETimerIdSmsMessSmsp:
			{
			iSmspBusy=EFalse;
			ReqCompleted(iSmspReqHandle, KErrNone);
			break;
			}
		case ETimerIdSmsMessRx:
			{
			TInt ret=0;
			
			switch (iSmsReceiveMode)//swich which state machine to enter
				{
			case RMobileSmsMessaging::EReceiveUnstoredClientAck:
				ret=ActionRxEventUnstoredClientAck(ESmsEventRxTimer);
				break;
			case RMobileSmsMessaging::EReceiveUnstoredPhoneAck:
				ret=ActionRxEventUnstoredPhoneAck(ESmsEventRxTimer);
				break;
			case RMobileSmsMessaging::EReceiveStored:
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_TIMERCALLBACK_2, "Recieve Stored SMS Rx Event.");
				ret=ActionRxEventStored(ESmsEventRxTimer);
				break;
			default://other receive modes cannot be set
				break;
				}//end switch

			if (ret != KErrNone)
				{
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_TIMERCALLBACK_3, "ERROR: Unexpected ret code %d", ret);
				__ASSERT_ALWAYS(ret==KErrNone,SimPanic(EIllegalSmsRxEvent, __LINE__));	// There should be no error from this action, but to check...
				}

			break;
			}

		case ETimerIdSmsMessResumeReception:
			{
			if (iSmsRxReqOutstanding)
				{
				iRxState = ESmsRxStateWaitingForSmsRx;
				}
			else
				{
				iRxState = ESmsRxStateIdle;
				}
			StartSmsMtTimer();
			CompletePendingReq();
			break;
			}

		default:
			break;
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_TIMERCALLBACK_4, ">>CSimSmsMesaging::TimerCallBack OUT");
	}

const CTestConfigSection* CSimSmsMessaging::CfgFileSection()
/**
* Returns a pointer to the config file section
*
* @return CTestConfigSection a pointer to the configuration file data section
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_CFGFILESECTION_1, ">>CSimSmsMessaging::CfgFileSection");
	return iPhone->CfgFile();
	}


void CSimSmsMessaging::ReloadConfigurationSettingsL()
/**
 *  This function reloads settings from the config file
 */
    {
    Reset();
    InitializeL();
    }

HBufC8* CSimSmsMessaging::PduToAscii(TDesC8& aSmsPdu)
/**
 * Converts the contents of a TDes8 to their Hex representation
 * @param aSmsPdu Reference to the descriptor to convert
 * @return A pointer to an HBufC8 containing the Hex representation of aSmsPdu. The caller is responsible for freeing the object.
 * @return Null if the HBufC8 was not successfuly created.
 */
	{
	HBufC8* hexBuf = HBufC8::New(aSmsPdu.Length()*2+1);
	if (hexBuf == NULL) return NULL;
	TPtr8 des = hexBuf->Des();
	des.FillZ();
	des.Zero();
	if (hexBuf != NULL)
		{
		for (TInt i = 0; i < aSmsPdu.Length(); i++)
			{
			TInt left = (aSmsPdu[i] & 0xF0) >> 4;
			TInt right = aSmsPdu[i] & 0x0F;
			if (left < 10)
				{
				des.Append(0x30 + left);
				}
			else
				{
				des.Append(0x41 + (left - 10));
				}
			if (right < 10)
				{
				des.Append(0x30 + right);
				}
			else
				{
				des.Append(0x41 + (right - 10));
				}
			}
		}
	des.Append(0);
	return hexBuf;
	}

#if (OST_TRACE_CATEGORY & OST_TRACE_CATEGORY_DEBUG) // to prevent UREL build warnings
void CSimSmsMessaging::DumpPdu(const TDesC8& aText, TDesC8& aSmsPdu, HBufC8* aPduInAscii)
#else
void CSimSmsMessaging::DumpPdu(const TDesC8& /*aText*/, TDesC8& aSmsPdu, HBufC8* aPduInAscii)
#endif
/**
Print PDU in a loop, 150 chars per line
@param aText - a header line about the PDU.
@param aSmsPdu - PDU to log.
@param aPduInAscii - aSmsPdu in ASCII format (default NULL).
*/
    {
#if (OST_TRACE_CATEGORY & OST_TRACE_CATEGORY_DEBUG)    // to stop the UREL build warnings    
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_DUMPPDU1_1, "%s pdu length=%d", aText, aSmsPdu.Length());
#endif	

    if( !aSmsPdu.Length() )
        return;

	// In sendPdu case, caller needs to call PduToAscii for comparison.
	// Then aPduInAscii is non-zero.
	HBufC8* hexBuf = (aPduInAscii) ? aPduInAscii : PduToAscii(aSmsPdu);
	if (! hexBuf)
	    return;

	//Print the pdu in a loop because LOGTEXT can only print up to 150 characters
	for (TInt i = 0; i < hexBuf->Length(); i+=100)
	    {
		TInt len = Min(100, hexBuf->Mid(i).Length());
        TPtrC8 pduChunk(hexBuf->Mid(i).Left(len).Ptr(), len);
		OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_DUMPPDU1_2, "PDU Chunk: %s:", pduChunk);
		}

	if (0 == aPduInAscii)
	  delete hexBuf;
    }

#if (OST_TRACE_CATEGORY & OST_TRACE_CATEGORY_DEBUG)

/**
Appends Type of number and Numbering plan identification to TBuf8 buffer.

@param aBuffer Name of aTon will be appended to this buffer.
@param aTon TMobileTON whose name will be appended to aBuffer.
*/
void CSimSmsMessaging::AppendTonToBuffer(TDes8& aBuffer,const RMobilePhone::TMobileTON& aTon)
	{
	switch(aTon)
		{
	case (RMobilePhone::EUnknownNumber):
		{
		aBuffer.Append(_L("EUnknownNumber"));
		break;
		}
	case (RMobilePhone::EInternationalNumber):
		{
		aBuffer.Append(_L("EInternationalNumber"));
		break;
		}
	case (RMobilePhone::ENationalNumber):
		{
		aBuffer.Append(_L("ENationalNumber"));
		break;
		}
	case (RMobilePhone::ENetworkSpecificNumber):
		{
		aBuffer.Append(_L("ENetworkSpecificNumber"));
		break;
		}
	case (RMobilePhone::ESubscriberNumber):
		{
		aBuffer.Append(_L("ESubscriberNumber"));
		break;
		}
	case (RMobilePhone::EAlphanumericNumber):
		{
		aBuffer.Append(_L("EAlphanumericNumber"));
		break;
		}
	case (RMobilePhone::EAbbreviatedNumber):
		{
		aBuffer.Append(_L("EAbbreviatedNumber"));
		break;
		}
	default:
		aBuffer.Append(_L("Wrong Ton Type"));
		}
	}

void CSimSmsMessaging::AppendNpiToBuffer(TDes8& aBuffer,const RMobilePhone::TMobileNPI& aNpi)
/**
 *	Appends Numbering plan identification to TBuf8 buffer.
 */
	{
	switch(aNpi)
		{
	case (RMobilePhone::EUnknownNumberingPlan):
		{
		aBuffer.Append(_L("EUnknownNumberingPlan"));
		break;
		}
	case (RMobilePhone::EIsdnNumberPlan):
		{
		aBuffer.Append(_L("EIsdnNumberPlan"));
		break;
		}
	case (RMobilePhone::EDataNumberPlan):
		{
		aBuffer.Append(_L("EDataNumberPlan"));
		break;
		}
	case (RMobilePhone::ETelexNumberPlan):
		{
		aBuffer.Append(_L("ETelexNumberPlan"));
		break;
		}
	case (RMobilePhone::EServiceCentreSpecificPlan1):
		{
		aBuffer.Append(_L("EServiceCentreSpecificPlan1"));
		break;
		}
	case (RMobilePhone::EServiceCentreSpecificPlan2):
		{
		aBuffer.Append(_L("EServiceCentreSpecificPlan2"));
		break;
		}
	case (RMobilePhone::ENationalNumberPlan):
		{
		aBuffer.Append(_L("ENationalNumberPlan"));
		break;
		}
	case (RMobilePhone::EPrivateNumberPlan):
		{
		aBuffer.Append(_L("EPrivateNumberPlan"));
		break;
		}
	case (RMobilePhone::EERMESNumberPlan):
		{
		aBuffer.Append(_L("EERMESNumberPlan"));
		break;
		}
	default:
		aBuffer.Append(_L("Wrong Npi Type"));
		}
	}

void CSimSmsMessaging::LogTMobileSmsAttributesV1(const RMobileSmsMessaging::TMobileSmsAttributesV1& aSmsAttributesV1)
	{
	const TInt KTextWidth = 100;
	_LIT8(KFLAGS,		"   iFlags:      ");
	_LIT8(KSCADDR,		"   SC Address:  ");
	_LIT8(KSCADDRTON,	"   SC Addr TON: ");
	_LIT8(KSCADDRNPI,	"   SC Addr NPI: ");
	_LIT8(KDATAFORMAT,	"   iDataFormat: ");
	_LIT8(KDEADDR,		"   DE Address:  ");
	_LIT8(KDEADDRTON,	"   DE Addr TON: ");
	_LIT8(KDEADDRNPI,	"   DE Addr NPI: ");
	_LIT8(KMORETOSEND,	"   iMore:       ");
	_LIT8(KMSGREF,		"   iMsgRef:     ");
	_LIT8(KSUBMITREP,	"   iSubmitRep:  ");

	TBuf8<KTextWidth> buffer;

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_LOGTMOBILESMSATTRIBUTESV1_1, "Send Sms Attributes:");
	
	buffer.Zero();
	buffer.Copy(KFLAGS);
	buffer.Append(_L8("0x"));
	buffer.AppendFormat(_L8("%08X") , ((TInt)(aSmsAttributesV1.iFlags)));
	OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_LOGTMOBILESMSATTRIBUTESV1_2, "Buffer: %s", buffer);

	if(aSmsAttributesV1.iFlags & RMobileSmsMessaging::KGsmServiceCentre)
		{
		buffer.Zero();
		buffer.Copy(KSCADDR);
		buffer.Append(aSmsAttributesV1.iGsmServiceCentre.iTelNumber);
		OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_LOGTMOBILESMSATTRIBUTESV1_3, "Buffer: %s", buffer);

		buffer.Zero();
		buffer.Copy(KSCADDRTON);
		AppendTonToBuffer(buffer,(aSmsAttributesV1.iGsmServiceCentre.iTypeOfNumber));
		OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_LOGTMOBILESMSATTRIBUTESV1_4, "Buffer: %s", buffer);;

		buffer.Zero();
		buffer.Copy(KSCADDRNPI);
		AppendNpiToBuffer(buffer,(aSmsAttributesV1.iGsmServiceCentre.iNumberPlan));		   
		OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_LOGTMOBILESMSATTRIBUTESV1_5, "Buffer: %s", buffer);
		}
	if(aSmsAttributesV1.iFlags & RMobileSmsMessaging::KSmsDataFormat)
		{
		buffer.Zero();
		buffer.Copy(KDATAFORMAT);
		if(aSmsAttributesV1.iDataFormat == RMobileSmsMessaging::EFormatUnspecified)
			{
			buffer.Append(_L("EFormatUnspecified"));
			}
		else if(aSmsAttributesV1.iDataFormat == RMobileSmsMessaging::EFormatGsmTpdu)
			{
			buffer.Append(_L("EFormatGsmTpdu"));
			}		
		OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_LOGTMOBILESMSATTRIBUTESV1_6, "Buffer: %s", buffer);
		}
	if(aSmsAttributesV1.iFlags & RMobileSmsMessaging::KRemotePartyInfo)
		{
		const RMobileSmsMessaging::TMobileSmsSendAttributesV1& smsSendAttributesV1 = static_cast<const RMobileSmsMessaging::TMobileSmsSendAttributesV1&> (aSmsAttributesV1);

		buffer.Zero();
		buffer.Copy(KDEADDR);
		buffer.Append(smsSendAttributesV1.iDestination.iTelNumber);
		OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_LOGTMOBILESMSATTRIBUTESV1_7, "Buffer: %s", buffer);

		buffer.Zero();
		buffer.Copy(KDEADDRTON);
		AppendTonToBuffer(buffer,(smsSendAttributesV1.iDestination.iTypeOfNumber));
		OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_LOGTMOBILESMSATTRIBUTESV1_8, "Buffer: %s", buffer);

		buffer.Zero();
		buffer.Copy(KDEADDRNPI);
		AppendNpiToBuffer(buffer,(smsSendAttributesV1.iDestination.iNumberPlan)); 
		OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_LOGTMOBILESMSATTRIBUTESV1_9, "Buffer: %s", buffer);
		}
	if(aSmsAttributesV1.iFlags & RMobileSmsMessaging::KMoreToSend)
		{
		const RMobileSmsMessaging::TMobileSmsSendAttributesV1& smsSendAttributesV1 = static_cast<const RMobileSmsMessaging::TMobileSmsSendAttributesV1&> (aSmsAttributesV1);

		buffer.Zero();
		buffer.Copy(KMORETOSEND);

		if(smsSendAttributesV1.iMore)
			{
			buffer.Append(_L8("ETrue"));
			}
		else
			{
			buffer.Append(_L8("EFalse"));
			}
		OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_LOGTMOBILESMSATTRIBUTESV1_10, "Buffer: %s", buffer);
		}
	if(aSmsAttributesV1.iFlags & RMobileSmsMessaging::KMessageReference)
		{
		const RMobileSmsMessaging::TMobileSmsSendAttributesV1& smsSendAttributesV1 = static_cast<const RMobileSmsMessaging::TMobileSmsSendAttributesV1&> (aSmsAttributesV1);

		buffer.Zero();
		buffer.Copy(KMSGREF);
		buffer.Append(_L("0x"));
		buffer.AppendFormat(_L8("%08X") , smsSendAttributesV1.iMsgRef);
		OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_LOGTMOBILESMSATTRIBUTESV1_11, "Buffer: %s", buffer);
		}

	if(aSmsAttributesV1.iFlags & RMobileSmsMessaging::KGsmSubmitReport)
		{
		const RMobileSmsMessaging::TMobileSmsSendAttributesV1& smsSendAttributesV1 = static_cast<const RMobileSmsMessaging::TMobileSmsSendAttributesV1&> (aSmsAttributesV1);

		buffer.Zero();
		buffer.Copy(KSUBMITREP);

		TInt i = buffer.Length(); 
		TInt j(0);
		TInt k = (smsSendAttributesV1.iSubmitReport.Length() * 2) + buffer.Length();

		TInt l = 0;
		do
			{
			for(;i<=(KTextWidth-2);i+=2)
				{
				if(j==smsSendAttributesV1.iSubmitReport.Length())
					{
					break;
					}
				buffer.AppendFormat(_L8("%X") , smsSendAttributesV1.iSubmitReport[j]);
				j++;
				}
			OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSMESSAGING_LOGTMOBILESMSATTRIBUTESV1_12, "Buffer: %s", buffer);
			buffer.Zero();
			i=0;
			l+=KTextWidth;
			}
		while(l < k);
		}
	}

#endif // _DEBUG
