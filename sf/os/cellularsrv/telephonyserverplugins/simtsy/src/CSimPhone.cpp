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
// This file contains the implementation for the Simulator Phone class 'CSimPhone'
// 
//

/**
 @file
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSimPhoneTraces.h"
#endif

#include <testconfigfileparser.h>
#include "CSimPhone.h"
#include "CSimCall.h"
#include "csimsmsmess.h"
#include "CSimPhBkStore.h"
#include "CSimPhBkUsimStore.h"
#include "CSimONStore.h"
#include "CSimSignalStrength.h"
#include "CSimNetworkStatus.h"
#include "CSimDtmf.h"
#include "CSimIndicator.h"
#include "utils.h"
#include "CSimPacketService.h"
#include "CSimSat.h"
#include "CSimBatteryCharger.h"
#include "CSimCallBarring.h"
#include "CSimCallForwarding.h"
#include "CSimCallWaiting.h"
#include "CSimPhoneInitialise.h"
#include "CSimPhoneIMSAuth.h"
#include "CSimTsyMode.h"
#include "csimsmartcardauth.h"
#include "csimsmartcardeap.h"

#include "simtsyglobalproperties.h"
#include "et_struct.h"


#define PDD_NAME _L("ECDRV")				// < Name of the WINS Serial Physical Device Driver
#define LDD_NAME _L("ECOMM")				// < Name of the WINS Serial Logical Device Driver

const TInt KPhBkUSimStoreGranularity=5;		// < The granularity of the phonebook store list array.
const TInt KPhonebookStoreGranularity=2;	// < The granularity of the phonebook store list array.

GLDEF_C void SimPanic(TSimPanic aPanicNumber, TInt aLineNumber)
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, SIMPANIC_1, "SMS.TSY Panic %d Line %d", aPanicNumber, aLineNumber);
	(void) aLineNumber;
	_LIT(panicText,"SIM.TSY");
	User::Panic(panicText,aPanicNumber);
	}

//
// CSimPhone
//
void CSimPhone::CloseObj(TAny* aObj)
/**
*	A utility function for cleaning up the stack.
*
* @param aObj a pointer to the CObject to close
*/
	{
	((CObject*)aObj)->Close();
	}

CSimPhone* CSimPhone::NewL(CPhoneBase*& aPhoneBaseRef)
/**
* Standard two phase constructor.
*
* @return CSimPhone  pointer to the phone object created
* @leave Leaves if no memory or object is not created for any reason
*/
	{
	CSimPhone* phone=new(ELeave) CSimPhone(aPhoneBaseRef);
	TCleanupItem newPhoneClose(CloseObj,phone);
	CleanupStack::PushL(newPhoneClose);
	phone->ConstructL();
	CleanupStack::Pop();
	
	aPhoneBaseRef = phone;
	
	return phone;
	}

CSimPhone::CSimPhone(CPhoneBase*& aPhoneBaseRef)
			: iPhoneBaseRef(aPhoneBaseRef),
			  iAPNServiceOffset(4),
			  iUSIMServiceTable(NULL),
			  iUSIMEnabledServiceTable(NULL),
			  iSIMServiceTable(NULL),
			  iModemDetection(RPhone::EDetectedPresent),
			  iMode(RPhone::EModeIdle), 
			  iNtwkMode(RMobilePhone::ENetworkModeUnknown)
	{
	iPhoneId.iError = KErrNotSupported;
	}
	
/**
* A utility function for reading the service tables and setting them up
*
* @param aMaxSupported size of table supported by etelmm
* @param aLookup config file reference to the table to be looked up
* @param pTable pointer to the table to be constructed for the config file table
* @return void does not return anything
*/	
void CSimPhone::PopulateServiceTableL(	TInt aMaxSupported, 
										const TDesC8& aLookup, 
										RMobilePhone::TMobilePhoneServiceTableV1** aTable)
	{

	RMobilePhone::TMobilePhoneServiceTableV1* pTable = NULL;
	pTable = new(ELeave) RMobilePhone::TMobilePhoneServiceTableV1();
	
	// USIM Supported entry's
	const CTestConfigItem* item=CfgFile()->Item(aLookup);
	if (item)
		{	
		TInt count = CTestConfig::CountElements(item->Value(),KStdDelimiter);
		for (TInt i= 0; i< count; i++)
			{
			TInt entry = 0;			
			TUint8* pTableEntry = &(pTable->iServices1To8);		
				
			TInt ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,entry);
			if(ret == KErrNone && entry < (aMaxSupported+1)) // This is the maximum supported services by etelmm.h
				{
					TUint8 basic = 0x01;
					pTableEntry += ((entry-1)/8);
					*pTableEntry |= (basic << ((entry-1)%8));
				}
			}
			*aTable = pTable;	
			OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_POPULATESERVICETABLEL_1, "Successfully populated %s table in simtsy", aLookup);
		}
	else	
		{
		delete(pTable);
		pTable = NULL;
		OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_POPULATESERVICETABLEL_2, "Failed to populat %s table in simtsy", aLookup);
		}
	}

/**
 * A utility function for reading the USIM V8 service tables and setting them up
 *
 * @param aTableV8 pointer to the table to be constructed for the config file table
 * @param aLookup config file reference to the table to be looked up
 * @return void does not return anything
 */	
 void CSimPhone::PopulateServiceTableV8L(const TDesC8& aLookup,RMobilePhone::TMobilePhoneServiceTableV8** aTableV8)
 	{

  	RMobilePhone::TMobilePhoneServiceTableV8* pTable = NULL;
 	pTable = new(ELeave) RMobilePhone::TMobilePhoneServiceTableV8();
 	
 	TInt aMaxSupportedV1 = 56;
 	TInt aMaxSupportedV8 = 72;
 	// USIM Supported entry's for V8
 	
 	const CTestConfigItem* item=CfgFile()->Item(aLookup);
 	if (item)
 		{	
 		TInt count = CTestConfig::CountElements(item->Value(),KStdDelimiter);
 		for (TInt i= 0; i< count; i++)
 			{
 			TInt entry = 0;			
 			TUint8* pTableEntry = &(pTable->iServices57To64);		
 				
 			TInt ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,entry);
 			if(ret == KErrNone &&  entry > (aMaxSupportedV1) && entry < (aMaxSupportedV8 +1)) // This is the maximum supported services by etelmm.h
 				{
 					entry -= aMaxSupportedV1;
 					TUint8 basic = 0x01;
 					pTableEntry += ((entry-1)/8);
 					*pTableEntry |= (basic << ((entry-1)%8));
 				}
 			}
 			*aTableV8 = pTable;	
 			OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_POPULATESERVICETABLEV8L_1, "Successfully populated %s table in simtsy", aLookup);
 		}
 	else	
 		{
 		delete(pTable);
 		pTable = NULL;
 		OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_POPULATESERVICETABLEV8L_2, "Failed to populate %s table in simtsy", aLookup);
 		}
 	}

 
 
 void CSimPhone::SetTestNumberAndReadConfigurationFileL()
     {
     TInt testNumber;
     User::LeaveIfError(GetTestNumber(testNumber));
     iSectionName.Format(KSectionNameFormat,testNumber);
     
     delete iConfigFile;
     iConfigFile = NULL;
         
     TRAPD(err, iConfigFile = CTestConfig::NewLC(iFs, KConfigFileDir, KConfigFilename); CleanupStack::Pop(iConfigFile));
     if( err == KErrNone && iConfigFile->Section(iSectionName) != NULL )
         {
         iConfigSection = iConfigFile->Section(iSectionName);
         }
    else
        {
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_SETTESTNUMBERANDREADCONFIGURATIONFILE_1, "Section for requested test number (%d) not found in the Config File",testNumber);
        delete iConfigFile;
        iConfigFile = NULL;

        User::Leave(KErrNotFound);
        }
     
     User::LeaveIfError(SetTestNumberInUse(testNumber));
     }
 
void CSimPhone::ConstructL()
/**
* 2 Phase Construction (Second phase)
* Loads the WINS Physical and Logical Serial Device Drivers.  C32 loads the non-WINS
* serial drivers, so we don't have to worry about these.
* Creates Voice (CSimVoiceLine) and Data lines (CSimDataLine), CConfigReader objects,
* CSimSysAgent objects, CSimSmsMessaging object, CSimPhoneStore object,
* CSimPhoneBook object, and CSimPacketService object.
*/
	{
#if defined (__WINS__)
	TInt r=User::LoadPhysicalDevice(PDD_NAME);
	r=User::LoadLogicalDevice(LDD_NAME);
#endif

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_CONSTRUCTL_1, "Starting to Load and Parse the Config File");
	
	User::LeaveIfError(iFs.Connect());
	
	SetTestNumberAndReadConfigurationFileL();
	
	CSimTsyMode::InitL(this);

	iReduceTimers = CSimReduceTimers::NewL();

	InitPhoneStatus();
	iSimPhoneInitialise = CSimPhoneInitialise::NewL(this);

	if (CSimTsyMode::GetMode() != CSimTsyMode::ECdmaV1)
		{
		iVoiceLine = CSimVoiceLine::NewL(this,KVoiceLineName);
		iDataLine = CSimDataLine::NewL(this,KDataLineName);
		iSmsMessaging = CSimSmsMessaging::NewL(this);
		iPhoneSecurity = CSimPhoneSecurity::NewL(this);
		iPhoneUSimApp = CSimPhoneUSimApp::NewL(this);
		iPhoneSmartCardApp = CSimPhoneSmartCardApp::NewL(this);
		iPhoneScAuth = CSimSmartCardAuth::NewL(this);

		iPhBkStores=new(ELeave) CArrayFixFlat<CSimPhBkStore*>(KPhonebookStoreGranularity);
		iPhBkUSimStores=new(ELeave) CArrayFixFlat<CSimPhBkUSimStore*>(KPhBkUSimStoreGranularity);

		FindAndCreatePhBkStoresL();
		CreateONStoreL();

		iSignalStrength=CSimSignalStrength::NewL(this);
		iNetworkStatus=CSimNetworkStatus::NewL(this);
		iDtmf=CSimDtmf::NewL(this);

		}
		
	const TInt capacityServiceTable =  56;
	PopulateServiceTableL(capacityServiceTable,KUSIMServiceTableSupportedEntrys(),	&iUSIMServiceTable);
	PopulateServiceTableL(capacityServiceTable,USIMServiceTableEnabledEntrys(), &iUSIMEnabledServiceTable);
	PopulateServiceTableL(capacityServiceTable,KSIMServiceTable(),	&iSIMServiceTable);
		
	/* Updating the V8 features */
	PopulateServiceTableV8L(KUSIMServiceTableSupportedEntrys(),&iUSIMServiceTableV8);
	
	if (ServiceEnabled(iUSIMServiceTable,iAPNServiceOffset,RMobilePhone::KUstACL))
		{
		TInt entries = 0;
		const CTestConfigItem* aPNListItem=CfgFile()->Item(KAccessPointNameList);
		if (aPNListItem)
			{
			TInt ret=CTestConfig::GetElement(aPNListItem->Value(),KStdDelimiter,0,entries);
			if(ret == KErrNone && entries > 0)
				{
				for (TInt i= 0;i< entries;i++)
					{
					RMobilePhone::TAPNEntryV3 entry;
					entry.iApn.Zero();
					TPtrC8 startupAPNList;
					ret=CTestConfig::GetElement(aPNListItem->Value(),KStdDelimiter,i+1,startupAPNList);
					entry.iApn.Copy(startupAPNList);
					iAPNList.Append(entry);	
					}
				}
			}			
		}

	TPtrC8 IMSI;
	iSubscriberId.iError = KErrNone;
	const CTestConfigItem* itemSubscriberId=CfgFile()->Item(KSubscriberId);
    if( itemSubscriberId != NULL )
        {
        TInt ret=CTestConfig::GetElement(itemSubscriberId->Value(),KStdDelimiter,0,IMSI);
        if( ret != KErrNone )
			{
            OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_CONSTRUCTL_2, "WARNING - CONFIGURATION FILE PARSING - Reading element IMSI returned %d (element no. %d) from tag %s.",ret,0,KSubscriberId);
			}
        // coverity[check_return]
        CTestConfig::GetElement(itemSubscriberId->Value(),KStdDelimiter,1,iSubscriberId.iError);
        iSubscriberId.iIMSI.Copy(IMSI);
        }
	else
		{
		iSubscriberId.iIMSI.Copy(KSubscriberIdDefault);
		}

	iNtwkMode=(RMobilePhone::TMobilePhoneNetworkMode)CfgFile()->ItemValue(KNetworkMode,KNetworkModeDefault);
	//get phone id from config file
	GetPhoneIdAndCaps();

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_CONSTRUCTL_3, "Loading the version configuration");
	iSimTsyVersion = CfgFile()->ItemValue(KTsyVersionNumber,KSimTsyDefaultVersion);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_CONSTRUCTL_4, "Simulated SIMTSY version: %d", iSimTsyVersion);
	
	iPacketService = CSimPacketService::NewL(this);
	iBatteryCharger = CSimBatteryCharger::NewL(this);
	iSat = CSimSat::NewL(this);
	iIndicator = CSimIndicator::NewL(this);
	iPhoneIMSAuth = CSimPhoneIMSAuth::NewL(this);

	iCallBarring=CSimCallBarring::NewL(this);
	iCallForwarding=CSimCallForwarding::NewL(this);
	iCallWaiting=CSimCallWaiting::NewL(this);
	iUsimRelease6=CSimUsimR6::NewL(this);

	iNetworkModeArray = new (ELeave) CArrayFixFlat<TNetworkModeBundle>(KGranularity);

	iPhoneScEapMan = CSimSmartCardEapManager::NewL(this);

	iNetworkModeTimer = CSimTimer::NewL(this);
	//< Read in all network mode data
	TInt count = CfgFile()->ItemCount(KNetworkMode);
	const CTestConfigItem* item = NULL;
	for (TInt i = 0; i < count; ++i)
		{
		item = CfgFile()->Item(KNetworkMode,i);
		if (!item)
			break;
		
		TInt duration(0); 
		TInt networkMode(0);
		TInt ret = CTestConfig::GetElement(item->Value(),KStdDelimiter,0,duration);
		if (ret != KErrNone)
			break;
		ret = CTestConfig::GetElement(item->Value(),KStdDelimiter,1,networkMode);
		if (ret != KErrNone)
			break;
		RMobilePhone::TMobilePhoneNetworkMode mode;
		if(networkMode == 1)
			{
			mode = RMobilePhone::ENetworkModeUnregistered;
			}
		else if(networkMode == 2)
			{
			mode = RMobilePhone::ENetworkModeGsm;
			}
		else if(networkMode == 3)
			{
			mode = RMobilePhone::ENetworkModeAmps;
			}
		else if(networkMode == 6)
			{
			mode = RMobilePhone::ENetworkModeWcdma;
			}
		else if(networkMode == 7)
			{
			mode = RMobilePhone::ENetworkModeTdcdma;
			}
		else
			{
			mode = RMobilePhone::ENetworkModeUnknown;
			}
		TNetworkModeBundle networkModeData;
		networkModeData.iDuration = duration;
		networkModeData.iNetworkMode = mode;
		
		TRAP_IGNORE(iNetworkModeArray->AppendL(networkModeData));
		}
	
	if (iNetworkModeArray->Count() != 0)
		{
		iNetworkModeIndex = 0;
		TNetworkModeBundle initialNetworkMode = iNetworkModeArray->At(0);
		iNtwkMode = initialNetworkMode.iNetworkMode;		// set to new value if NetworkMode present in config.txt file
		iTimerCallBackNetworkMode.SetHandle(this);
		iNetworkModeTimer->Start(initialNetworkMode.iDuration, &iTimerCallBackNetworkMode);
		}
	// end of network mode simulation setup
	iTestNumberObserver = CSimTestNumberObserver::NewL(*this);
	
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_CONSTRUCTL_5, "Completed Loading and Parsing the Config File");
	}

CSimReduceTimers* CSimPhone::GetReduceTimersSubject()
	{
	return iReduceTimers;
	}

TInt CSimPhone::GetTestNumber(TInt& aTestNumber)
/**
 * Retrieve the test number.
 */
	{
	TInt  ret = RProperty::Get(KUidPSSimTsyCategory, KPSSimTsyTestNumber, aTestNumber);

	if (ret == KErrNone  &&  aTestNumber >= 0)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_GETTESTNUMBER_1, "Got system property KUidPSSimTsyCategory/KPSSimTsyTestNumber. testNumber=%d", aTestNumber);
		}
	else
		{
		aTestNumber = KDefaultTestNumber;
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_GETTESTNUMBER_2, "Using the default test number. testNumber=%d", aTestNumber);
		}

	return KErrNone;
	}


TInt CSimPhone::SetTestNumberInUse(TInt aTestNumber)
/**
 * Set the test number that is actually used by the TSY
 */
	{
	TInt  ret = RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyTestNumberInUse, aTestNumber);

	if (ret == KErrNone)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_SETTESTNUMBERINUSE_1, "Set system property KUidPSSimTsyCategory/KPSSimTsyTestNumberInUse. testNumber=%d", aTestNumber);
		}

	return ret;
	}


CSimPhone::~CSimPhone()
/**
* Standard destructor.
* Any objects created by the ::ConstructL() function should be destroyed here.
* The objects to be destroyed are:
* CSimVoiceLine, CSimDataLine, CConfigReader, CSimSysAgent, CSimSmsMessaging
* CSimPhoneStore, CSimPhoneBook, CSimPacketService
*/
	{
	iPhoneBaseRef = NULL;

	delete iNetworkModeTimer;

	delete iDtmf;
	delete iNetworkStatus;
	delete iSignalStrength;
	delete iIndicator;
	delete iBatteryCharger;
	delete iPhoneIMSAuth;
	delete iPhoneScEapMan;
	delete iPhoneScAuth;
	delete iUsimRelease6;
	
	iFs.Close();
	if(iDataLine)
		iDataLine->Close();
	if(iVoiceLine)
		iVoiceLine->Close();
	if(iSmsMessaging)
		iSmsMessaging->Close();
	if(iPhoneSecurity)
		iPhoneSecurity->Close();
	if(iPhoneUSimApp)
		iPhoneUSimApp->Close();
	if(iPhoneSmartCardApp)
		iPhoneSmartCardApp->Close();
			
	if (iUSIMServiceTable != NULL)		
		delete(iUSIMServiceTable);
	if (iUSIMEnabledServiceTable != NULL)		
		delete(iUSIMEnabledServiceTable);	
	if (iSIMServiceTable != NULL)		
		delete(iSIMServiceTable);
	if (iUSIMServiceTableV8 != NULL)		
		delete(iUSIMServiceTableV8);
	
	if( iPhBkStores != NULL )
		{
		TInt storeCount=iPhBkStores->Count();
		for(TInt i=0;i<storeCount;i++)
			{
			iPhBkStores->At(i)->Close();
			}
		delete iPhBkStores;
		}
	if( iPhBkUSimStores != NULL )
	    {
		TInt storeCount=iPhBkUSimStores->Count();
		for(TInt i=0;i<storeCount;i++)
			{
			iPhBkUSimStores->At(i)->Close();
			}
		delete iPhBkUSimStores;
		}
	
	if(iONStore)
		{
		iONStore->Close();
		iONStore = NULL;
		}
	if( iNetworkModeArray != NULL )
		{
		iNetworkModeArray->Delete(0,iNetworkModeArray->Count());
		delete iNetworkModeArray;
		}
	if( iPacketService!= NULL )
	    {
		iPacketService->Close();
	    }
	if( iSat != NULL )
	    {
		iSat->Close();
	    }
	if( iConfigFile == NULL)
	    {
	    // In this case if iConfigSection exists then it is not owned
	    // by iConfigFile - this is cos there was no config file for
	    // this SIM TSY.
	    delete iConfigSection;
	    }
	delete iConfigFile;
    delete iCallBarring;
    delete iCallForwarding;
    delete iCallWaiting;

	if (iSetCallProcessingSuspendStateTimer)
		{
		iSetCallProcessingSuspendStateTimer->Cancel();
		delete iSetCallProcessingSuspendStateTimer;		
		}
	delete iSimPhoneInitialise;
	
	delete iReduceTimers;
	delete iTestNumberObserver;
	
	CSimTsyMode::FreeMode();
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_DTOR_1, "CSimPhone Destroyed");
	}

void CSimPhone::InitPhoneStatus()
	{
	const CTestConfigItem* item = NULL;
	TInt ret = KErrNone;

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_INITPHONESTATUS_1, "Starting to Load and Parse Phone Config parameters");

	item = CfgFile()->Item(KPhoneStatus);
	if(!item)
		return;

	TInt modemDetection, mode;
	ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 0, modemDetection);
	if (ret == KErrNone)
		{
		iModemDetection = (RPhone::TModemDetection)modemDetection;
		}
	ret=CTestConfig::GetElement(item->Value(), KStdDelimiter, 1, mode);
	if (ret != KErrNone)
		{
		iMode = (RPhone::TMode)mode;
		}
	}

void CSimPhone::FindAndCreatePhBkStoresL()
	{
	TInt count=CfgFile()->ItemCount(KPhBkStore);
	const CTestConfigItem* item=NULL;
	TInt ret=KErrNone;

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_FINDANDCREATEPHBKSTORESL_1, "Starting to Load and Parse PhoneBookStore Config parameters");
	TInt i;
	for(i=0;i<count;i++)
		{
		item=CfgFile()->Item(KPhBkStore,i);
		if(!item)
			break;

		TPtrC8 phonebookName;
		TInt maxNumSlots, telNumMaxLen, alphaTagMaxLen;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,phonebookName);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_FINDANDCREATEPHBKSTORESL_2, "WARNING - CONFIGURATION FILE PARSING - Reading element PHONEBOOKNAME returned %d (element no. %d) from tag %s.",ret,0,KPhBkStore);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,maxNumSlots);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_FINDANDCREATEPHBKSTORESL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element MAXNUMSLOTS returned %d (element no. %d) from tag %s.",ret,1,KPhBkStore);
			continue;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,telNumMaxLen);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_FINDANDCREATEPHBKSTORESL_4, "WARNING - CONFIGURATION FILE PARSING - Reading element TELNUMMAXLEN returned %d (element no. %d) from tag %s.",ret,2,KPhBkStore);
			continue;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,alphaTagMaxLen);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_FINDANDCREATEPHBKSTORESL_5, "WARNING - CONFIGURATION FILE PARSING - Reading element ALPHATAGMAXLEN returned %d (element no. %d) from tag %s.",ret,3,KPhBkStore);
			continue;
			}


		CSimPhBkStore* phBkStore=CSimPhBkStore::NewL(this,phonebookName,maxNumSlots,telNumMaxLen,alphaTagMaxLen);
		TCleanupItem newObjClose(CloseObj,phBkStore);
		CleanupStack::PushL(newObjClose);
		iPhBkStores->AppendL(phBkStore);
		CleanupStack::Pop();
		}


	count=CfgFile()->ItemCount(KPhBkUSimStore);

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_FINDANDCREATEPHBKSTORESL_6, "Starting to Load and Parse USim PhoneBookStore Config parameters");

	for(i=0;i<count;i++)
		{
		item=CfgFile()->Item(KPhBkUSimStore,i);
		if(!item)
			break;

		TPtrC8 phonebookName, phonebookStore;
		TInt maxNumSlots, telNumMaxLen, alphaTagMaxLen;
		TInt additional, maxEmail, maxAdditionalTelNumLen, maxAdditionalTextLen;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,phonebookStore);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_FINDANDCREATEPHBKSTORESL_7, "WARNING - CONFIGURATION FILE PARSING - Reading element PHONEBOOKSTORE returned %d (element no. %d) from tag %s.",ret,0,KPhBkUSimStore);
			continue;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,phonebookName);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_FINDANDCREATEPHBKSTORESL_8, "WARNING - CONFIGURATION FILE PARSING - Reading element PHONEBOOKNAME returned %d (element no. %d) from tag %s.",ret,1,KPhBkUSimStore);
			continue;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,maxNumSlots);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_FINDANDCREATEPHBKSTORESL_9, "WARNING - CONFIGURATION FILE PARSING - Reading element MAXNUMSLOTS returned %d (element no. %d) from tag %s.",ret,2,KPhBkUSimStore);
			continue;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,telNumMaxLen);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_FINDANDCREATEPHBKSTORESL_10, "WARNING - CONFIGURATION FILE PARSING - Reading element TELNUMMAXLEN returned %d (element no. %d) from tag %s.",ret,3,KPhBkUSimStore);
			continue;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,alphaTagMaxLen);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_FINDANDCREATEPHBKSTORESL_11, "WARNING - CONFIGURATION FILE PARSING - Reading element ALPHATAGMAXLEN returned %d (element no. %d) from tag %s.",ret,4,KPhBkUSimStore);
			continue;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,5,additional);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_FINDANDCREATEPHBKSTORESL_12, "WARNING - CONFIGURATION FILE PARSING - Reading element ADDITIONAL returned %d (element no. %d) from tag %s.",ret,5,KPhBkUSimStore);
			continue;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,6,maxEmail);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_FINDANDCREATEPHBKSTORESL_13, "WARNING - CONFIGURATION FILE PARSING - Reading element MAXEMAIL returned %d (element no. %d) from tag %s.",ret,6,KPhBkUSimStore);
			continue;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,7,maxAdditionalTelNumLen);
		if(ret!=KErrNone)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_FINDANDCREATEPHBKSTORESL_14, "WARNING maxAdditionalTelNumLen missing, defaulting to telNumMaxLen");
			maxAdditionalTelNumLen = telNumMaxLen;
			}
	
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,8,maxAdditionalTextLen);
		if(ret!=KErrNone)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_FINDANDCREATEPHBKSTORESL_15, "WARNING maxAdditionalTextLen missing, defaulting to alphaTagMaxLen");
			maxAdditionalTextLen = alphaTagMaxLen;
			}

		TInt countStores = iPhBkUSimStores->Count();
		TBool found=EFalse;

		if(phonebookName.Compare(KGsmPhoneBook) == KErrNone)
		{
		TInt countShort = iPhBkStores->Count();
		TBool foundShort=EFalse;

			for(TInt j=0; i < countShort; j++)
			{
				CSimPhBkStore* phBkStore = iPhBkStores->At(j);
				if((phBkStore->Name().Compare(phonebookName) == KErrNone))
					{
					foundShort=ETrue;
					break;
					}
			}
			if(foundShort) //warn but not fail!
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_FINDANDCREATEPHBKSTORESL_16, "WARNING! PhBkUSimStore and PhBkStore duplicate phonebook configuration");
				}
		}
		for(TInt j=0; j < countStores; j++)
			{
				CSimPhBkUSimStore* phBkStore = iPhBkUSimStores->At(j);
				if((phBkStore->Name().Compare(phonebookName) == KErrNone)&&
					(phBkStore->PhBkStore().Compare(phonebookStore) == KErrNone))
					{
					found=ETrue;
					break;
					}
			}

		if(found)    //This store already exists!!!
			continue;
		
		CSimPhBkUSimStore* phBkStore=CSimPhBkUSimStore::NewL(this,phonebookStore,phonebookName,maxNumSlots,telNumMaxLen,alphaTagMaxLen,additional,maxEmail,maxAdditionalTelNumLen,maxAdditionalTextLen);
		TCleanupItem newObjClose(CloseObj,phBkStore);
		CleanupStack::PushL(newObjClose);
		iPhBkUSimStores->AppendL(phBkStore);
		CleanupStack::Pop();
		}


// Populate the Phonebook Stores after construction, so as not to disturb the config file's
// search pointer
	for(i=0;i<iPhBkStores->Count();i++)
		{
		iPhBkStores->At(i)->PopulateStoreFromConfigFileL();
		}
	for(i=0;i<iPhBkUSimStores->Count();i++)
		{
		iPhBkUSimStores->At(i)->PopulateStoreFromConfigFileL();
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_FINDANDCREATEPHBKSTORESL_17, "Finished parsing PhBkStores config parameters");
	}

void CSimPhone::CreateONStoreL()
	{
	const CTestConfigItem* item=NULL;
	TInt ret=KErrNone;

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_CREATEONSTOREL_1, "Starting to Load and Parse Own Number Config parameters");

	TInt i=0;
	item=CfgFile()->Item(KONStore,i);
	if(!item)
		return;

	TInt maxNumSlots, telNumMaxLen, alphaTagMaxLen;
	ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,maxNumSlots);
	if(ret!=KErrNone)
		{
		OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_CREATEONSTOREL_2, "WARNING - CONFIGURATION FILE PARSING - Reading element MAXNUMSLOTS returned %d (element no. %d) from tag %s.",ret,1,KONStore);
		return;
		}

	ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,telNumMaxLen);
	if(ret!=KErrNone)
		{
		OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_CREATEONSTOREL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element TELNUMMAXLEN returned %d (element no. %d) from tag %s.",ret,2,KONStore);
		return;
		}

	ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,alphaTagMaxLen);
	if(ret!=KErrNone)
		{
		OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_CREATEONSTOREL_4, "WARNING - CONFIGURATION FILE PARSING - Reading element ALPHATAGMAXLEN returned %d (element no. %d) from tag %s.",ret,3,KONStore);
		return;
		}

	iONStore=CSimONStore::NewL(this,maxNumSlots,telNumMaxLen,alphaTagMaxLen);

// Populate the Own Number Store 
	iONStore->PopulateStoreFromConfigFileL();

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_CREATEONSTOREL_5, "Finished parsing Own Number Store config parameters");
	}

TInt CSimPhone::ExtFunc(const TTsyReqHandle aReqHandle, const TInt aIpc, const TDataPackage& aPckg)
/**
* ExtFunc is called by the server when it has a "extended", i.e. non-core ETel request
* for the TSY to process.
* A request handle, request type and request data are passed to the TSY.
*/
	{
	TAny* dataPtr = aPckg.Ptr1();

	TAny* dataPtr2 = aPckg.Ptr2();

	TInt functionValue;

	switch (aIpc)
		{
	case EMobilePhoneGetMultimodeCaps:
		return GetMultimodeCaps(aReqHandle, reinterpret_cast<TUint32*>(dataPtr));

	case EMobilePhoneGetIccAccessCaps:
		return GetIccAccessCaps(aReqHandle,
			REINTERPRET_CAST(TUint32*,dataPtr));

	case EMobilePhoneGetPhoneStoreInfo:
		switch(aPckg.Type())
		{
		// Switch between the alternative implementations of GetPhoneStoreInfo
		case TDataPackage::EPackage1n2u:
			return GetPhoneStoreInfo(aReqHandle,
					aPckg.Des1n(),aPckg.Des2u());
		case TDataPackage::EPackage1n2n:
			return GetPhoneStoreInfo(aReqHandle, aPckg.Des2n(),
					REINTERPRET_CAST(RMobilePhone::TPhoneStoreNameAndIccType*, dataPtr));
		default:
			return NULL;
		}


//		return GetPhoneStoreInfo(aReqHandle,aPckg.Des1n(),aPckg.Des2u());

// Signal Strength
	case EMobilePhoneGetSignalCaps:
		return iSignalStrength->GetSignalCaps(aReqHandle,aPckg.Des1n());

	case EMobilePhoneGetSignalStrength:
		return iSignalStrength->GetSignalStrength(aReqHandle,aPckg.Des1n(),aPckg.Des2n());

	case EMobilePhoneNotifySignalStrengthChange:
		return iSignalStrength->NotifySignalStrengthChange(aReqHandle,aPckg.Des1n(),aPckg.Des2n());

// Indicators
	case EMobilePhoneGetIndicator:
		return iIndicator->GetIndicator(aReqHandle,aPckg.Des1n());

	case EMobilePhoneNotifyIndicatorChange:
		return iIndicator->NotifyIndicatorChange(aReqHandle,aPckg.Des1n());

	case EMobilePhoneGetIndicatorCaps:
		return iIndicator->GetIndicatorCaps(aReqHandle,aPckg.Des1n(), aPckg.Des2n());

// Battery Charger
	case EMobilePhoneGetBatteryInfo:
		return iBatteryCharger->GetBatteryInfo(aReqHandle,aPckg.Des1n());

	case EMobilePhoneNotifyBatteryInfoChange:
		return iBatteryCharger->NotifyBatteryInfoChange(aReqHandle,aPckg.Des1n());

	case EMobilePhoneGetBatteryCaps:
		return iBatteryCharger->GetBatteryCaps(aReqHandle,aPckg.Des1n());

// Network Information
	case EMobilePhoneGetNetworkCaps:
		return iNetworkStatus->GetNetworkCaps(aReqHandle,aPckg.Des1n());

	case EMobilePhoneGetCurrentMode:
		return GetCurrentMode(aReqHandle,aPckg.Des1n());


	case EMobilePhoneNotifyModeChange:
		return NotifyModeChange(aReqHandle,reinterpret_cast<RMobilePhone::TMobilePhoneNetworkMode*>(dataPtr)); 


	case EMobilePhoneGetHomeNetwork:
		return iNetworkStatus->GetHomeNetwork(aReqHandle,aPckg.Des1n());

	case EMobilePhoneGetCurrentNetwork:
		return iNetworkStatus->GetCurrentNetwork(aReqHandle,aPckg.Des1n(),aPckg.Des2n());

	case EMobilePhoneGetCurrentNetworkNoLocation:
		return iNetworkStatus->GetCurrentNetworkNoLocation(aReqHandle,aPckg.Des1n());

	case EMobilePhoneNotifyCurrentNetworkChange:
		return iNetworkStatus->NotifyCurrentNetworkChange(aReqHandle,aPckg.Des1n(),aPckg.Des2n());

	case EMobilePhoneNotifyCurrentNetworkNoLocationChange:
		return iNetworkStatus->NotifyCurrentNetworkNoLocationChange(aReqHandle,aPckg.Des1n());

	case EMobilePhoneGetNetworkRegistrationStatus:
		return iNetworkStatus->GetNetworkRegistrationStatus(aReqHandle,aPckg.Des1n());

	case EMobilePhoneNotifyNetworkRegistrationStatusChange:
		return iNetworkStatus->NotifyNetworkRegistrationStatusChange(aReqHandle,aPckg.Des1n());
	
	case EMobilePhoneGetServiceProviderName:
		return iNetworkStatus->GetCurrentServiceProvider(aReqHandle,aPckg.Des1n());

	case EMobilePhoneGetNetworkName:
		return iNetworkStatus->GetCurrentNetworkName(aReqHandle,aPckg.Des1n(), aPckg.Des2n());
		
//USIM Release6 Infomration
	case EMobilePhoneGetMailboxNumbers:
		return iUsimRelease6->GetMailboxNumbers(aReqHandle,aPckg.Des1n());
		
	case EMobilePhoneNotifyMailboxNumbersChange:
	 	return iUsimRelease6->NotifyMailboxNumbersChange(aReqHandle,aPckg.Des1n());
	 	
	case EMobilePhoneNotifyMessageWaiting:
		return iUsimRelease6->NotifyMessageWaiting(aReqHandle,REINTERPRET_CAST(TInt*, dataPtr));
		
	case EMobilePhoneGetIccMessageWaitingIndicators:
	 	return iUsimRelease6->GetIccMessageWaitingIndicators(aReqHandle,aPckg.Des1n());
	 	
	case EMobilePhoneSetIccMessageWaitingIndicators:
		return iUsimRelease6->SetIccMessageWaitingIndicators(aReqHandle,aPckg.Des1n());
		
	case EMobilePhoneNotifyIccMessageWaitingIndicatorsChange:
		return iUsimRelease6->NotifyIccMessageWaitingIndicatorsChange(aReqHandle,aPckg.Des1n());
	
	case EMobilePhoneNotifyWlanDataChange:
		return iUsimRelease6->NotifyWlanDataChange(aReqHandle,aPckg.Des1n());
	
	case EMobilePhoneNotifyPreferredWlanSIDListChange:
		return iUsimRelease6->NotifyPreferredWlanSIDListChange(aReqHandle);
		
	case EMobilePhoneGetWlanData:
		return iUsimRelease6->GetWlanData(aReqHandle,aPckg.Des1n());
		
	case EMobilePhoneSetWlanData:
		return iUsimRelease6->SetWlanData(aReqHandle,aPckg.Des1n());
	
	case EMobilePhoneGetPreferredWlanSIDsPhase1:
		return iUsimRelease6->GetPreferredWlanSIDsPhase1(aReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TClientId*, dataPtr), 
			REINTERPRET_CAST(TInt*, dataPtr2));
			
	case EMobilePhoneGetPreferredWlanSIDsPhase2:
		return iUsimRelease6->GetPreferredWlanSIDsPhase2(aReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TClientId*, dataPtr), aPckg.Des2n());		
						
	case EMobilePhoneStorePreferredWlanSIDList:
		return iUsimRelease6->StorePreferredWlanSIDList(aReqHandle, aPckg.Des1n());
			
	case EMobilePhoneNotifyStorePreferredNetworksListChange:
		return iUsimRelease6->NotifyStorePreferredNetworksListChange(aReqHandle);
					
	case EMobilePhoneGetPreferredNetworksPhase1:
		return iUsimRelease6->GetPreferredNetworksPhase1(aReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TClientId*, dataPtr), 
			REINTERPRET_CAST(TInt*, dataPtr2));

	case EMobilePhoneGetPreferredNetworksPhase2:
		return iUsimRelease6->GetPreferredNetworksPhase2(aReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TClientId*, dataPtr), aPckg.Des2n());		
			
	case EMobilePhoneStorePreferredNetworksList:
		return iUsimRelease6->StorePreferredNetworksList(aReqHandle, aPckg.Des1n());
		
	case EMobilePhoneSetGbaBootstrapParams:
		return iUsimRelease6->SetGbaBootstrapParams(aReqHandle, aPckg.Des1n(),
		         reinterpret_cast<RMobilePhone::TAID*>(dataPtr2));
	
	case EMobilePhoneNotifyAuthenticateDataChange:
		return iUsimRelease6->NotifyAuthenticateDataChange(aReqHandle, aPckg.Des1n());
	
	case EMobilePhoneGetAuthenticationParams:
		return iUsimRelease6->GetAuthenticationParams(aReqHandle, aPckg.Des1n(),
				aPckg.Des2n());
		
	case EMobilePhoneAuthenticationListPhase1:
		return iUsimRelease6->GetAuthenticationListPhase1(aReqHandle,
				REINTERPRET_CAST(CRetrieveMobilePhoneAuthenticationIds::TAuthRequestData*, dataPtr),
				REINTERPRET_CAST(TInt*, dataPtr2));
		
	case EMobilePhoneAuthenticationListPhase2:
		return iUsimRelease6->GetAuthenticationListPhase2(aReqHandle, 
				REINTERPRET_CAST(RMobilePhone::TClientId*, dataPtr),
				aPckg.Des2n());

// NITZ Network Information
	case EMobilePhoneGetNITZInfo:
		return iNetworkStatus->GetNITZInfo(aReqHandle,aPckg.Des1n());

	case EMobilePhoneNotifyNITZInfoChange:
		return iNetworkStatus->NotifyNITZInfoChange(aReqHandle,aPckg.Des1n());

// Cell Information
			case EMobilePhoneGetCellInfo:
				return iNetworkStatus->GetCellInfo(aReqHandle,aPckg.Des1n());

			case EMobilePhoneNotifyCellInfoChange:
				return iNetworkStatus->NotifyCellInfoChange(aReqHandle,aPckg.Des1n());

// Get Subscriber Info
	case EMobilePhoneGetSubscriberId:
		return GetSubscriberInfo(aReqHandle,aPckg.Des1n());

// DTMF Tx
	case EMobilePhoneGetDTMFCaps:
		return iDtmf->GetDtmfCaps(aReqHandle,aPckg.Des1n());

	case EMobilePhoneNotifyDTMFCapsChange:
		return iDtmf->NotifyDtmfCapsChange(aReqHandle,aPckg.Des1n());

	case EMobilePhoneSendDTMFTones:
		return iDtmf->SendDTMFTones(aReqHandle,aPckg.Des1u());

	case EMobilePhoneStartDTMFTone:
		return iDtmf->StartDTMFTone(aReqHandle,aPckg.Des1n());

	case EMobilePhoneStopDTMFTone:
		return iDtmf->StopDTMFTone(aReqHandle);

	case EMobilePhoneNotifyStopInDTMFString:
		return iDtmf->NotifyStopInDTMFString(aReqHandle);

	case EMobilePhoneContinueDTMFStringSending:
		return iDtmf->ContinueDtmfStringSending(aReqHandle,aPckg.Des1n());
// Phone ID
	case EMobilePhoneGetIdentityCaps:
		return GetIdentityCaps(aReqHandle,*REINTERPRET_CAST(TUint32*, dataPtr));

	case EMobilePhoneGetPhoneId:
		return GetPhoneId(aReqHandle,REINTERPRET_CAST(RMobilePhone::TMobilePhoneIdentityV1*, dataPtr));

// Security (ICC)

	case EMobilePhoneGetSecurityCaps:
	case EMobilePhoneNotifySecurityCapsChange:
	case EMobilePhoneGetLockInfo:
	case EMobilePhoneNotifyLockInfoChange:
	case EMobilePhoneSetLockSetting:
	case EMobilePhoneChangeSecurityCode:
	case EMobilePhoneNotifySecurityEvent:
	case EMobilePhoneVerifySecurityCode:
	case EMobilePhoneAbortSecurityCode:
	case EMobilePhoneGetSecurityCodeInfo:
	case EMobilePhoneNotifySecurityCodeInfoChange:
		return iPhoneSecurity->ExtFunc(aReqHandle, aIpc, aPckg);

//USim Application Support
	case EMobilePhoneEnumerateUSimApplications:
	case EMobilePhoneSetUSimApplicationStatus:
	case EMobilePhoneGetUSimApplicationsInfo:
	case EMobilePhoneNotifyUSimApplicationsInfoChange:
		if(iPhoneUSimApp->FoundUSimAppTags())
			{
			return iPhoneUSimApp->ExtFunc(aReqHandle, aIpc, aPckg);
			}
		else
			{
			return iPhoneSmartCardApp->ExtFunc(aReqHandle, aIpc, aPckg);
			}
			
//SmartCard Application Support
	case EMobilePhoneEnumerateSmartCardApplications:
	case EMobilePhoneGetSmartCardApplicationInfo:
	case EMobilePhoneSetSmartCardApplicationStatus:
	case EMobilePhoneNotifySmartCardApplicationInfoChange:
	case EMobilePhoneGetScFileInfo:
	case EMobilePhoneReadScFile:
	case EMobilePhoneUpdateScFile:
		if(iPhoneSmartCardApp->FoundScAppTags())
			{
			return iPhoneSmartCardApp->ExtFunc(aReqHandle, aIpc, aPckg);
			}
		else
			{
			ReqCompleted(aReqHandle, KErrNotSupported);
			return KErrNone;
			}

		
//USim/SmartCard Application Supoprt
	case EMobilePhoneGetCurrentActiveUSimApplication:
	case EMobilePhoneGetUSimAppsSelectionMode:
	case EMobilePhoneSetUSimAppsSelectionMode:
	case EMobilePhoneNotifyUSimAppsSelectionModeChange:
		if(iPhoneUSimApp->FoundUSimAppTags())
			{
			return iPhoneUSimApp->ExtFunc(aReqHandle, aIpc, aPckg);
			}
		else
			{
			return iPhoneSmartCardApp->ExtFunc(aReqHandle, aIpc, aPckg);
			}		

// CallBarring
	case EMobilePhoneSetCallBarringStatus:
	case EMobilePhoneSetCallBarringPassword:
	case EMobilePhoneNotifyCallBarringStatusChange:
	case EMobilePhoneGetBarringStatusPhase1:
	case EMobilePhoneGetBarringStatusPhase2:
		return iCallBarring->ExtFunc(aReqHandle, aIpc, aPckg);

// CallForwarding
	case EMobilePhoneSetCallForwardingStatus:
	case EMobilePhoneNotifyCallForwardingStatusChange:
	case EMobilePhoneGetIdentityServiceStatus:
	case EMobilePhoneGetCallForwardingStatusPhase1:
	case EMobilePhoneGetCallForwardingStatusPhase2:
		return iCallForwarding->ExtFunc(aReqHandle, aIpc, aPckg);

// CallWaiting
	case EMobilePhoneSetCallWaitingStatus:
	case EMobilePhoneNotifyCallWaitingStatusChange:
	case EMobilePhoneGetWaitingStatusPhase1:
	case EMobilePhoneGetWaitingStatusPhase2:
		{
		functionValue = 0;
		TRAPD(leaveValue, functionValue = iCallWaiting->ExtFuncL(aReqHandle, aIpc, aPckg));
		if (leaveValue != KErrNone)
	 		{
 			return leaveValue;
 			}
		else
 			{
 			return functionValue;
 			}		
		}

// IMS Auth
	case EMobilePhoneAuthorizationInfoPhase1:
			return iPhoneIMSAuth->GetAuthorizationInfoPhase1(aReqHandle,
			reinterpret_cast<RMobilePhone::TClientId*>(dataPtr),
			reinterpret_cast<TInt*>(dataPtr2));

	case EMobilePhoneAuthorizationInfoPhase2:
		return iPhoneIMSAuth->GetAuthorizationInfoPhase2(aReqHandle,
			reinterpret_cast<RMobilePhone::TClientId*>(dataPtr), aPckg.Des2n());
			
	case EMobilePhoneIMSAuthenticate:
		return iPhoneIMSAuth->GetAuthenticationData(aReqHandle, aPckg.Des1n());

	case EMobilePhoneNotifyImsAuthorizationInfoChanged:
		return iPhoneIMSAuth->NotifyImsAuthorizationInfoChanged(aReqHandle);

// Generic Smart Card Authentication
	case EMobilePhoneSmartCardAuthenticate:
		return iPhoneScAuth->GetScAuthenticationData(aReqHandle, aPckg.Des1n(),
		         reinterpret_cast<RMobilePhone::TAID*>(dataPtr2));

// Access Point Name (APN)
	case EMobilePhoneEnumerateAPNEntries:
		{
		TUint32* count = static_cast<TUint32*>(dataPtr);
		return EnumerateAPNEntries(aReqHandle,count);
		}
	
	case EMobilePhoneGetAPNname:
		{
		TUint32 index = *static_cast<TUint32*>(dataPtr);
		
		RMobilePhone::TAPNEntryV3Pckg* entryToReturnPckg = 
			static_cast<RMobilePhone::TAPNEntryV3Pckg*> (aPckg.Des2n());
		
		return GetAPNname(aReqHandle, index, *entryToReturnPckg);
		}

	case EMobilePhoneAppendAPNName:
		{
		RMobilePhone::TAPNEntryV3 entryToAppend = 
			*static_cast<RMobilePhone::TAPNEntryV3*>(dataPtr);
				
		RMobilePhone::TAPNEntryV3Pckg entryToAppendPckg(entryToAppend);	
		return AppendAPNName(aReqHandle,entryToAppendPckg);
		}
		
	case EMobilePhoneDeleteAPNName:
		{
		TUint32 tmp = *static_cast<TUint32*>(dataPtr);
		
		return DeleteAPNName(aReqHandle,tmp);
		}
	
	case EMobilePhoneNotifyAPNListChanged:
		return NotifyAPNListChanged(aReqHandle);
	
	case EMobilePhoneSetAPNControlListServiceStatus:
		return SetAPNControlListServiceStatus(aReqHandle,
				*static_cast<RMobilePhone::TAPNControlListServiceStatus*>(dataPtr));
	
	case EMobilePhoneGetAPNControlListServiceStatus:
		return GetAPNControlListServiceStatus(aReqHandle,
				*static_cast<RMobilePhone::TAPNControlListServiceStatus*>(dataPtr));
	
	case EMobilePhoneNotifyAPNControlListServiceStatusChange:
		return NotifyAPNControlListServiceStatusChange(aReqHandle,
				*static_cast<RMobilePhone::TAPNControlListServiceStatus*>(dataPtr));

	case EMobilePhoneGetServiceTable:
		return GetServiceTable(aReqHandle, 
			*static_cast<RMobilePhone::TMobilePhoneServiceTable*>(dataPtr), 
				(aPckg.Des2n()));	
	default:
		ReqCompleted(aReqHandle, KErrNotSupported);
		return KErrNone;		// Real error value returned in completion of clients request
		}
		
	}

TInt CSimPhone::CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle)
/**
 * Cancel an outstanding request.
 * @param aIpc The IPC number of the request that must be cancelled.  Note: this is not the
 *             IPC number of the cancel request itself.
 * @param aTsyReqHandle The TSY Request Handle of the request to be cancelled.
 */
	{
	switch(aIpc)
		{
	case EMobilePhoneNotifyIndicatorChange:
		iIndicator->NotifyIndicatorChangeCancel();
		break;

	case EMobilePhoneNotifyBatteryInfoChange:
		iBatteryCharger->NotifyBatteryInfoCancel();
		break;

	case EMobilePhoneNotifySignalStrengthChange:
		iSignalStrength->NotifySignalStrengthChangeCancel();
		break;
				
	case EMobilePhoneNotifyNetworkRegistrationStatusChange:
		iNetworkStatus->NotifyNetworkRegistrationStatusChangeCancel();
		break;

	case EMobilePhoneNotifyCurrentNetworkChange:
		iNetworkStatus->NotifyCurrentNetworkChangeCancel(aTsyReqHandle);
		break;

	case EMobilePhoneNotifyCurrentNetworkNoLocationChange:
		iNetworkStatus->NotifyCurrentNetworkNoLocationChangeCancel(aTsyReqHandle);
		break;	

	case EMobilePhoneNotifyNITZInfoChange:
		iNetworkStatus->NotifyNITZInfoChangeCancel();
		break;

	case EMobilePhoneNotifyCellInfoChange:
		iNetworkStatus->NotifyCellInfoChangeCancel(aTsyReqHandle);
		break;

	case EMobilePhoneNotifyModeChange:
		NotifyModeChangeCancel(aTsyReqHandle);
		break;


// Get Subscriber Info
	case EMobilePhoneSendDTMFTones:
		iDtmf->SendDTMFTonesCancel();
		break;

	case EMobilePhoneNotifyStopInDTMFString:
		iDtmf->NotifyStopInDTMFStringCancel();
		break;

	case EMobilePhoneNotifyDTMFCapsChange:
		iDtmf->NotifyDtmfCapsChangeCancel(aTsyReqHandle);
		break;

	case EMobilePhoneNotifySecurityCapsChange:
	case EMobilePhoneGetLockInfo:
	case EMobilePhoneNotifyLockInfoChange:
	case EMobilePhoneSetLockSetting:
	case EMobilePhoneChangeSecurityCode:
	case EMobilePhoneNotifySecurityEvent:
	case EMobilePhoneVerifySecurityCode:
	case EMobilePhoneAbortSecurityCode:
	case EMobilePhoneGetSecurityCodeInfo:
	case EMobilePhoneNotifySecurityCodeInfoChange:
		return iPhoneSecurity->CancelService(aIpc,aTsyReqHandle);

//USim Application Support
	case EMobilePhoneEnumerateUSimApplications:
	case EMobilePhoneSetUSimApplicationStatus:
	case EMobilePhoneGetUSimApplicationsInfo:
	case EMobilePhoneNotifyUSimApplicationsInfoChange:
		if(iPhoneUSimApp->FoundUSimAppTags())
			{
			return iPhoneUSimApp->CancelService(aIpc,aTsyReqHandle);
			}
		else
			{
			return iPhoneSmartCardApp->CancelService(aIpc,aTsyReqHandle);
			}
			
//SmartCard Application Support
	case EMobilePhoneEnumerateSmartCardApplications:
	case EMobilePhoneGetSmartCardApplicationInfo:
	case EMobilePhoneSetSmartCardApplicationStatus:
	case EMobilePhoneNotifySmartCardApplicationInfoChange:
	case EMobilePhoneGetScFileInfo:
	case EMobilePhoneReadScFile:
	case EMobilePhoneUpdateScFile:
		if(iPhoneSmartCardApp->FoundScAppTags())
			{
			return iPhoneSmartCardApp->CancelService(aIpc,aTsyReqHandle);
			}
		else
			{
			return CPhoneBase::CancelService(aIpc,aTsyReqHandle);;
			}

		
//USim/SmartCard Application Supoprt
	case EMobilePhoneGetCurrentActiveUSimApplication:
	case EMobilePhoneGetUSimAppsSelectionMode:
	case EMobilePhoneSetUSimAppsSelectionMode:
	case EMobilePhoneNotifyUSimAppsSelectionModeChange:
		if(iPhoneUSimApp->FoundUSimAppTags())
			{
			return iPhoneUSimApp->CancelService(aIpc,aTsyReqHandle);
			}
		else
			{
			return iPhoneSmartCardApp->CancelService(aIpc,aTsyReqHandle);
			}


		// CallBarring
	case EMobilePhoneSetCallBarringStatus:
	case EMobilePhoneSetCallBarringPassword:
	case EMobilePhoneNotifyCallBarringStatusChange:
	case EMobilePhoneGetBarringStatusPhase1:
	case EMobilePhoneGetBarringStatusPhase2:
		return iCallBarring->CancelService(aIpc, aTsyReqHandle);

		// CallForwarding
	case EMobilePhoneSetCallForwardingStatus:
	case EMobilePhoneNotifyCallForwardingStatusChange:
	case EMobilePhoneGetIdentityServiceStatus:
	case EMobilePhoneGetCallForwardingStatusPhase1:
	case EMobilePhoneGetCallForwardingStatusPhase2:
		return iCallForwarding->CancelService(aIpc, aTsyReqHandle);

		// CallWaiting
	case EMobilePhoneSetCallWaitingStatus:
	case EMobilePhoneNotifyCallWaitingStatusChange:
	case EMobilePhoneGetWaitingStatusPhase1:
	case EMobilePhoneGetWaitingStatusPhase2:
		return iCallWaiting->CancelService(aIpc, aTsyReqHandle);

		//IMS Auth
	case EMobilePhoneNotifyImsAuthorizationInfoChanged:
		return iPhoneIMSAuth->NotifyImsAuthorizationInfoChangedCancel(aTsyReqHandle);

	case EMobilePhoneAuthorizationInfoPhase1:
	case EMobilePhoneAuthorizationInfoPhase2:
		return iPhoneIMSAuth->GetAuthorizationInfoCancel(aTsyReqHandle);

	case EMobilePhoneIMSAuthenticate:
		return iPhoneIMSAuth->GetAuthenticationDataCancel(aTsyReqHandle);

		// Generic Smart Card Authenticate cancel
	case EMobilePhoneSmartCardAuthenticate:
		return iPhoneScAuth->GetScAuthenticationDataCancel(aTsyReqHandle);

// Access Point Name (APN)
	case EMobilePhoneEnumerateAPNEntries:		
	case EMobilePhoneGetAPNname:
	case EMobilePhoneAppendAPNName:
	case EMobilePhoneDeleteAPNName:
		ReqCompleted(aTsyReqHandle, KErrNone);
		return KErrNone;		
	case EMobilePhoneNotifyAPNListChanged:
		if (iAPNListNotify.iNotifyPending == TRUE)
			{
			iAPNListNotify.iNotifyPending = EFalse;
			ReqCompleted(iAPNListNotify.iNotifyHandle, KErrCancel);
			}
		return KErrNone;
	case EMobilePhoneSetAPNControlListServiceStatus:
	case EMobilePhoneGetAPNControlListServiceStatus:
		ReqCompleted(aTsyReqHandle, KErrNone);
		return KErrNone;
	case EMobilePhoneNotifyAPNControlListServiceStatusChange:		
		if (iAPNStatusNotify.iNotifyPending == TRUE)
			{
			iAPNStatusNotify.iNotifyPending = EFalse;
			ReqCompleted(iAPNStatusNotify.iNotifyHandle, KErrCancel);
			}
		return KErrNone;
	case EMobilePhoneGetServiceTable:
		ReqCompleted(aTsyReqHandle, KErrNone);
		return KErrNone;	
				
	
	case EMobilePhoneNotifyMessageWaiting:
		return iUsimRelease6->NotifyMessageWaitingCancel(aTsyReqHandle);
		
	case EMobilePhoneNotifyIccMessageWaitingIndicatorsChange:
		return iUsimRelease6->NotifyIccMessageWaitingIndicatorsChangeCancel(aTsyReqHandle);
		
	case EMobilePhoneNotifyMailboxNumbersChange:
		return iUsimRelease6->NotifyMailboxNumbersChangeCancel(aTsyReqHandle);
	
	case EMobilePhoneNotifyWlanDataChange:
		return iUsimRelease6->NotifyWlanDataChangeCancel(aTsyReqHandle);

	case EMobilePhoneNotifyPreferredWlanSIDListChange:
		return iUsimRelease6->NotifyPreferredWlanSIDListChangeCancel(aTsyReqHandle);
	
	case EMobilePhoneNotifyStorePreferredNetworksListChange:
		return iUsimRelease6->NotifyStorePreferredNetworksListChangeCancel(aTsyReqHandle);
		
	case EMobilePhoneGetPreferredNetworksPhase1:
	case EMobilePhoneGetPreferredNetworksPhase2:
		return iUsimRelease6->GetPreferredNetworksCancel(aTsyReqHandle);
		
	case EMobilePhoneGetPreferredWlanSIDsPhase1:
	case EMobilePhoneGetPreferredWlanSIDsPhase2:
		return iUsimRelease6->GetPreferredWlanSIDsCancel(aTsyReqHandle);
		
	case EMobilePhoneSetGbaBootstrapParams:
		return iUsimRelease6->SetGbaBootstrapParamsCancel(aTsyReqHandle);
		
	case EMobilePhoneNotifyAuthenticateDataChange:
		return iUsimRelease6->NotifyAuthenticateDataChangeCancel(aTsyReqHandle);
		
	case EMobilePhoneGetAuthenticationParams:
		return iUsimRelease6->GetAuthenticationParamsCancel(aTsyReqHandle);
		
	case EMobilePhoneAuthenticationListPhase1:
	case EMobilePhoneAuthenticationListPhase2:
		return iUsimRelease6->GetAuthenticationListCancel(aTsyReqHandle);

	// Default
	default:
		return CPhoneBase::CancelService(aIpc,aTsyReqHandle);
		}
	return KErrNone;
	}

CTelObject* CSimPhone::OpenNewObjectByNameL(const TDesC& aName)
/**
* This function will open a new LINE by name.
* In reality the lines should be pre-constructed in the phone's two phase constructor,
* so this will just return a pointer to the relevant line.
*
* @param aName the name of the line object to be opened
* @return CTelObject pointer to the phone object created
* @leave Leaves if incorrect phone name
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_OPENNEWOBJECTBYNAMEL_1, ">>CSimPhone::OpenNewObjectByNameL");

// Is it a voice line?
	if (aName.CompareF(KVoiceLineName) == 0)
		{
		__ASSERT_ALWAYS(iVoiceLine!=NULL,SimPanic(ELineNotPreInstantiated));
		// Base class open.
		(void)iVoiceLine->Open();
		return iVoiceLine;
		}

// Is it a data line?
	else if (aName.CompareF(KDataLineName) == 0)
		{
		__ASSERT_ALWAYS(iDataLine!=NULL,SimPanic(ESubsessionNotPreInstantiated));
		// Base class open.
		(void)iDataLine->Open();
		return iDataLine;
		}

// Is it an SMS messaging class?
	else if (aName.CompareF(KETelSmsMessaging) == 0)
		{
		__ASSERT_ALWAYS(iSmsMessaging!=NULL,SimPanic(ESubsessionNotPreInstantiated));
		// Base class open.
		(void)iSmsMessaging->Open();
		return iSmsMessaging;
		}

	else if (aName.CompareF(KPacketName) == 0)
		{
		__ASSERT_ALWAYS(iPacketService!=NULL,SimPanic(ESubsessionNotPreInstantiated));
		// Base class open.
		(void)iPacketService->Open();
		return iPacketService;
		}
	else if (aName.CompareF(KSatName) == 0)
		{
		__ASSERT_ALWAYS(iSat!=NULL,SimPanic(ESubsessionNotPreInstantiated));
		// Base class open.
		(void)iSat->Open();
		return iSat;
		}
	else if (aName.Left(SCEAP_SSN_LENGTH).Compare(KETelSmartCardEapSession) == 0)
		{
		// "num" is used for both lengths and error codes!  saves on stack! :)

		// Extracting AID
		RMobilePhone::TAID tempAID;
		// get number of AID bytes (each byte uses two chars)
		TInt num = CharToSeptNumL(aName[SCEAP_SSN_LENGTH]) * 2;
		// get the segment containing the AID
		TPtrC ptrAid = aName.Mid(SCEAP_SSN_LENGTH + 1, num);
		// convert AID to its correct binary value
		ConvertTextToBinary(ptrAid, tempAID);

		// Extracting EapType
		RMobileSmartCardEap::TEapType eapType;
		// get position of EAP type's length
		TInt posTypeLength = SCEAP_SSN_LENGTH + num + 1;
		// get the value of the length of EAP type
		num = CharToSeptNumL(aName[posTypeLength]);
		// get the segment containing the EAP type
		TPtrC ptrEapT = aName.Mid(posTypeLength + 1, num);
		eapType.Copy(ptrEapT);

		CTelObject* phoneScEap = iPhoneScEapMan->CreateScEapSubSessionL(tempAID, eapType);

		if (phoneScEap == NULL)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_OPENNEWOBJECTBYNAMEL_2, "ERROR CSimSmartCardEap object not created, returning KErrGeneral");
			User::Leave(KErrGeneral);
			}

		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_OPENNEWOBJECTBYNAMEL_3, "CSimPhone::OpenNewObjectByNameL CSimSmartCardEap object created [0x%08x]", phoneScEap);
		return phoneScEap;
		} // End of opening Smart Card EAP sub-session
	else if (aName.CompareF(KETelOwnNumberStore) == 0)
		{
		if(iONStore==NULL)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_OPENNEWOBJECTBYNAMEL_4, "ERROR CSimONStore object not found. Please check config file.");
			User::Leave(KErrNotFound);
			}
		else
			{
			(void)iONStore->Open();
			return iONStore;
			}
		}
// Is it a phonebook store?
	else
		{
		TInt i = 0;
		TBuf8<KMaxName> name;
		TInt tLength;
		TPtrC remain(aName);
		name.Copy(aName);		// Do simple 16 bit to 8 bit conversion

		if ((tLength=remain.Find(PHBOOK_DELIMITER))==KErrNotFound)
			{
			for(i=0;i<iPhBkStores->Count();i++)
				{
				if(name.MatchF(iPhBkStores->At(i)->Name())==0)
					{
					// Base class open.
					(void)iPhBkStores->At(i)->Open();
					return iPhBkStores->At(i);
					}
				}
			for(i=0;i<iPhBkUSimStores->Count();i++)
				{
				if((name.MatchF(iPhBkUSimStores->At(i)->Name())==0) &&
					(iPhBkUSimStores->At(i)->PhBkStore().Compare(KGsmPhoneBook)==0))

					{
					(void)iPhBkUSimStores->At(i)->Open();
					return iPhBkUSimStores->At(i);
					}

				}
			}
		else
			{
			TBuf8<KMaxName> PhBkName;
			TBuf8<KMaxName> PhBkStore;
			PhBkName.Copy(aName.Left(tLength));
			PhBkStore.Copy(aName.Right(aName.Length()-tLength-2));

			for(TInt i=0;i<iPhBkUSimStores->Count();i++)
				{
				if((PhBkName.MatchF(iPhBkUSimStores->At(i)->Name())==0) &&
					(PhBkStore.MatchF(iPhBkUSimStores->At(i)->PhBkStore())==0))

					{
					(void)iPhBkUSimStores->At(i)->Open();
					return iPhBkUSimStores->At(i);
					}

				}
			}
		}
// Not matched...
	User::Leave(KErrNotFound);
	return NULL;
	}


CTelObject* CSimPhone::OpenNewObjectL(TDes& /*aNewName*/)
/**
* This function would open a new unnamed object,
* but this functionality is not supported at the phone level.
* All lines must be opened by name.
*
* @param aNewName a reference to the name of the phone object created
* @return CTelObject a pointer to the phone object created
* @leave Leaves if no memory is available
*/
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject::TReqMode CSimPhone::ReqModeL(const TInt aIpc)
/**
* This function returns the Request Mode for the request with the passed IPC value.
* The ETel Server provides a function for returning the standard request modes for
* the Core API requests.
*/
	{
	TReqMode reqMode=0;
	switch(aIpc)
		{
	case EMobilePhoneGetPhoneStoreInfo:
	case EMobilePhoneGetSignalCaps:
	case EMobilePhoneGetSignalStrength:
	case EMobilePhoneGetNetworkCaps:
	case EMobilePhoneGetCurrentMode:

	case EMobilePhoneNotifyModeChange:

	case EMobilePhoneGetHomeNetwork:
	case EMobilePhoneGetCurrentNetwork:
	case EMobilePhoneGetCurrentNetworkNoLocation:
	case EMobilePhoneGetNetworkRegistrationStatus:
	case EMobilePhoneGetNITZInfo:
	case EMobilePhoneGetCellInfo:
	case EMobilePhoneGetSubscriberId:
	case EMobilePhoneGetDTMFCaps:
	case EMobilePhoneSendDTMFTones:
	case EMobilePhoneStartDTMFTone:
	case EMobilePhoneStopDTMFTone:
	case EMobilePhoneContinueDTMFStringSending:
	case EMobilePhoneGetMultimodeCaps:
	case EMobilePhoneGetIndicator:
	case EMobilePhoneGetIndicatorCaps:
	case EMobilePhoneGetBatteryInfo:
	case EMobilePhoneGetBatteryCaps:
	case EMobilePhoneGetIccAccessCaps:
	case EMobilePhoneGetSecurityCaps:
	case EMobilePhoneGetLockInfo:
	case EMobilePhoneSetLockSetting:
	case EMobilePhoneGetSecurityCodeInfo:
	case EMobilePhoneChangeSecurityCode:
	case EMobilePhoneVerifySecurityCode:
	case EMobilePhoneAbortSecurityCode:
	case EMobilePhoneEnumerateUSimApplications:
	case EMobilePhoneGetCurrentActiveUSimApplication:
	case EMobilePhoneSetUSimApplicationStatus:
	case EMobilePhoneGetUSimApplicationsInfo:
	case EMobilePhoneGetUSimAppsSelectionMode:
	case EMobilePhoneSetUSimAppsSelectionMode:
	case EMobilePhoneEnumerateSmartCardApplications:
	case EMobilePhoneGetSmartCardApplicationInfo:
	case EMobilePhoneSetSmartCardApplicationStatus:
	case EMobilePhoneGetScFileInfo:
	case EMobilePhoneReadScFile:
	case EMobilePhoneUpdateScFile:
	case EMobilePhoneSetCallBarringStatus:
	case EMobilePhoneSetCallBarringPassword:
	case EMobilePhoneGetBarringStatusPhase1:
	case EMobilePhoneGetBarringStatusPhase2:
	case EMobilePhoneSetCallForwardingStatus:
	case EMobilePhoneGetIdentityServiceStatus:
	case EMobilePhoneGetCallForwardingStatusPhase1:
	case EMobilePhoneGetCallForwardingStatusPhase2:
	case EMobilePhoneGetIdentityCaps:
	case EMobilePhoneGetPhoneId:
	case EMobilePhoneGetServiceProviderName:
	case EMobilePhoneGetNetworkName:
	case EMobilePhoneSetCallWaitingStatus:
	case EMobilePhoneGetWaitingStatusPhase1:
	case EMobilePhoneGetWaitingStatusPhase2:
	case EMobilePhoneAuthorizationInfoPhase1:
	case EMobilePhoneAuthorizationInfoPhase2:
	case EMobilePhoneIMSAuthenticate:
	case EMobilePhoneSmartCardAuthenticate:
	case EMobilePhoneGetMailboxNumbers:
	case EMobilePhoneGetIccMessageWaitingIndicators:
	case EMobilePhoneSetIccMessageWaitingIndicators:
	case EMobilePhoneGetWlanData:
	case EMobilePhoneSetWlanData:
	case EMobilePhoneGetPreferredWlanSIDsPhase1:
	case EMobilePhoneGetPreferredWlanSIDsPhase2:
	case EMobilePhoneStorePreferredWlanSIDList:	
	case EMobilePhoneGetPreferredNetworksPhase1:
	case EMobilePhoneGetPreferredNetworksPhase2:
	case EMobilePhoneStorePreferredNetworksList:
	case EMobilePhoneSetGbaBootstrapParams:
	case EMobilePhoneGetAuthenticationParams:
	case EMobilePhoneNotifyAuthenticateDataChange:
	case EMobilePhoneAuthenticationListPhase1:
	case EMobilePhoneAuthenticationListPhase2:
		break;

// Service Tables
	case EMobilePhoneEnumerateAPNEntries:
	case EMobilePhoneGetAPNname:
	case EMobilePhoneAppendAPNName:
	case EMobilePhoneDeleteAPNName:
	case EMobilePhoneNotifyAPNListChanged:
	case EMobilePhoneSetAPNControlListServiceStatus:
	case EMobilePhoneGetAPNControlListServiceStatus:
	case EMobilePhoneNotifyAPNControlListServiceStatusChange:
	case EMobilePhoneGetServiceTable:
		break;

	case EMobilePhoneNotifyIndicatorChange:
	case EMobilePhoneNotifyBatteryInfoChange:
	case EMobilePhoneNotifySignalStrengthChange:
	case EMobilePhoneNotifyNetworkRegistrationStatusChange:
	case EMobilePhoneNotifyCurrentNetworkChange:
	case EMobilePhoneNotifyCurrentNetworkNoLocationChange:
	case EMobilePhoneNotifyNITZInfoChange:
	case EMobilePhoneNotifyCellInfoChange:
	case EMobilePhoneNotifyDTMFCapsChange:
	case EMobilePhoneNotifyStopInDTMFString:
	case EMobilePhoneNotifySecurityEvent:
	case EMobilePhoneNotifySecurityCapsChange:
	case EMobilePhoneNotifyUSimApplicationsInfoChange:
	case EMobilePhoneNotifySmartCardApplicationInfoChange:
	case EMobilePhoneNotifyUSimAppsSelectionModeChange:
	case EMobilePhoneNotifyCallBarringStatusChange:
	case EMobilePhoneNotifyCallWaitingStatusChange:
	case EMobilePhoneNotifyCallForwardingStatusChange:
	case EMobilePhoneNotifyImsAuthorizationInfoChanged:
	case EMobilePhoneNotifyMailboxNumbersChange:
	case EMobilePhoneNotifyIccMessageWaitingIndicatorsChange:
	case EMobilePhoneNotifyMessageWaiting:
	case EMobilePhoneNotifyWlanDataChange:
	case EMobilePhoneNotifyPreferredWlanSIDListChange:
	case EMobilePhoneNotifyStorePreferredNetworksListChange:
		reqMode=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
		break;
	case EMobilePhoneNotifyLockInfoChange:
	case EMobilePhoneNotifySecurityCodeInfoChange:
		reqMode=KReqModeMultipleCompletionEnabled;
		break;
	default:
		reqMode=CPhoneBase::ReqModeL(aIpc);
		break;
		}
	return reqMode;
	}

TInt CSimPhone::RegisterNotification(const TInt /*aIpc*/)
/**
* The ETel Server calls this function when the first client makes a notification request.
* If supported by the underlying protocol controlling the signalling stack,
* this can be used to start requesting updates for the relevant	service.
* This function does nothing in this TSY.
*
* @param aIpc The Ipc representing the client notification request
* @return KErrNone
*/
	{
	return KErrNone;
	}

TInt CSimPhone::DeregisterNotification(const TInt /*aIpc*/)
/**
* The ETel Server calls this function when the last client that had previously
* made a notification request closes its ETel Server handle.  If supported by
* the underlying protocol controlling the	signalling stack, this can be used
* to stop requesting updates for the relevant service.
*
* @param aIpc The Ipc representing the client notification request
* @return KErrNone
*/
	{
	return KErrNone;
	}

TInt CSimPhone::NumberOfSlotsL(const TInt aIpc)
	{
	switch(aIpc)
		{
	case EMobilePhoneNotifyIndicatorChange:
	case EMobilePhoneNotifyBatteryInfoChange:
	case EMobilePhoneNotifySignalStrengthChange:
	case EMobilePhoneNotifyCurrentNetworkChange:
	case EMobilePhoneNotifyCurrentNetworkNoLocationChange:
	case EMobilePhoneNotifyNetworkRegistrationStatusChange:
	case EMobilePhoneNotifyNITZInfoChange:
	case EMobilePhoneNotifyCellInfoChange:
	case EMobilePhoneNotifyStopInDTMFString:
	case EMobilePhoneNotifySecurityEvent:
	case EMobilePhoneNotifyUSimApplicationsInfoChange:
	case EMobilePhoneNotifySmartCardApplicationInfoChange:
	case EMobilePhoneNotifyUSimAppsSelectionModeChange:
	case EMobilePhoneNotifySecurityCapsChange:
	case EMobilePhoneNotifyCallBarringStatusChange:
	case EMobilePhoneNotifyCallForwardingStatusChange:
	case EMobilePhoneNotifyCallWaitingStatusChange:
	case EMobilePhoneNotifyImsAuthorizationInfoChanged:
	case EMobilePhoneNotifyMailboxNumbersChange:
	case EMobilePhoneNotifyIccMessageWaitingIndicatorsChange:
	case EMobilePhoneNotifyMessageWaiting:
	case EMobilePhoneNotifyWlanDataChange:
	case EMobilePhoneNotifyPreferredWlanSIDListChange:
	case EMobilePhoneNotifyStorePreferredNetworksListChange:
		return KDefaultNumberOfSlots;
	default:
		break;
		}
	return CPhoneBase::NumberOfSlotsL(aIpc);
	}

void CSimPhone::Init()
/**
*	This function can be used to perform any necessary synchronous initialisation.
*/
	{
	}

TInt CSimPhone::ControlledInitialisation(const TTsyReqHandle aTsyReqHandle)
/**
* This function can be used to start any necessary asynchronous initialisation.
* In this TSY, there are no asynchronous initialisation procedures, so its empty.
*/
	{
	iSimPhoneInitialise->Start(aTsyReqHandle);
	return KErrNone;
	}

TInt CSimPhone::ControlledInitialisationCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
* Request to cancel the ::ControlledInitialisation function.
* Since this TSY has no asynchronous initialisation procedures,
* this function should never be called.
*/
	{
	iSimPhoneInitialise->Cancel();
	return KErrNone;
	}

TInt CSimPhone::NotifyCapsChange(const TTsyReqHandle aTsyReqHandle, RPhone::TCaps* /*aCaps*/)
/**
* The phone caps will not change dynamically in this TSY as it is hardcoded,
* so there is no chance of completing this request.
*
* @param aTsyReqHandle
* @param aCaps a pointer to the phone capability
* @return KErrNone
*/
	{
	ReqCompleted(aTsyReqHandle, KErrNotSupported);
	return KErrNone;
	}

TInt CSimPhone::NotifyCapsChangeCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
*	Cancel the NotifyCapsChange request.
*
* @param aTsyReqHandle
* @return KErrNone
*/
	{
	return KErrNone;
	}



TInt CSimPhone::NotifyModemDetected(const TTsyReqHandle aTsyReqHandle, RPhone::TModemDetection* /*aDetection*/)
/**
* This request will be completed when the phone's connection status changes.
* This function is not supported in this version of the TSY
*/
	{
	ReqCompleted(aTsyReqHandle,KErrNotSupported);
	return KErrNone;
	}

TInt CSimPhone::NotifyModemDetectedCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
* Cancel outstanding modem detection notification, by TSY handle
* Returns KErrNone
*/
	{
	return KErrNone;
	}

TInt CSimPhone::GetInfo(const TTsyReqHandle aTsyReqHandle, RPhone::TPhoneInfo* aPhoneInfo)
/**
* Retrieve the Phone Information
* It should always return TModemDetection::EDetectedPresent
*
* @param aTsyReqHandle
* @param aPhoneInfo pointer to the phone information to be returned to client
* @return KErrNone
*/
	{
	aPhoneInfo->iDetection=iModemDetection;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

void CSimPhone::UpdateNetworkMode(RMobilePhone::TMobilePhoneNetworkMode aNetworkMode)
	{
	iNtwkMode = aNetworkMode;
	}

RMobilePhone::TMobilePhoneNetworkMode CSimPhone::NetworkMode()
	{
	return iNtwkMode;
	}

TInt CSimPhone::GetCurrentMode(TTsyReqHandle aReqHandle,TDes8* aPckg1)
/**
 * Retrieve the Current Mode.
 * @param aReqHandle	The request handle associated with this request.
 * @param aPckg1		The first parameter package.  This will be populated with the
 *						mode setting to be returned.
 * @return TInt			Standard error value.
 */
	{
	TPckg<RMobilePhone::TMobilePhoneNetworkMode>* modePckg=(TPckg<RMobilePhone::TMobilePhoneNetworkMode>*)aPckg1;
	RMobilePhone::TMobilePhoneNetworkMode& mode=(*modePckg)();

	mode=iNtwkMode;
	ReqCompleted(aReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimPhone::GetCaps(const TTsyReqHandle aTsyReqHandle, RPhone::TCaps* aCaps)
/**
* Retrieve the phone capabilities
*
* @param aTsyReqHandle
* @param aCaps a pointer to the Phone capability
* @return KErrNone
*/
	{
	aCaps->iFlags= RPhone::KCapsVoice | RPhone::KCapsData;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimPhone::GetStatus(const TTsyReqHandle aTsyReqHandle, RPhone::TStatus* aStatus)
/**
* Retrieve the phone status.
*
* @param aTsyReqHandle
* @param aStatus pointer to the phone status info.
* @return KErrNone
*/
	{
	aStatus->iModemDetected=iModemDetection;
	aStatus->iMode=iMode;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimPhone::EnumerateLines(const TTsyReqHandle aTsyReqHandle, TInt* aParams)
/**
* Enumerate the lines
* Returns 2 (for the voice and data lines) in the integer reference passed in
* through its function argument.
*
* @param aTsyReqHandle
* @param aParams pointer to the number of lines supported
* @return KErrNone
*/
	{
	*aParams = KNumberOfLines;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimPhone::GetLineInfo(const TTsyReqHandle aTsyReqHandle, TLineInfoIndex* aParams)
/**
* Gets the line info on the particular CSimLine object required.
* Returns the current hook status, the current line status, name of last call created on
* that line and the name of the call to which a new incoming call will be directed.
* TLineInfoIndex specifies which lines' info is requested.
*
* @param aTsyReqHandle
* @param aParam pointer to the line info
* @return KErrNone
*/
	{
	if (aParams->iIndex==KVoiceLineIndex)
		{
		aParams->iInfo.iStatus = (RCall::TStatus)iVoiceLine->iState;
		aParams->iInfo.iName = iVoiceLine->iLineName;
		aParams->iInfo.iLineCapsFlags = (RLine::KCapsVoice|RLine::KCapsEventIncomingCall);
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	else if (aParams->iIndex==KDataLineIndex)
		{
		aParams->iInfo.iStatus = (RCall::TStatus)iDataLine->iState;
		aParams->iInfo.iName = iDataLine->iLineName;
		aParams->iInfo.iLineCapsFlags = (RLine::KCapsData|RLine::KCapsEventIncomingCall);
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrNotFound);
		}
	return KErrNone;
	}

TInt CSimPhone::GetPhoneStoreInfo(TTsyReqHandle aReqHandle,TDes8* aPckg1,TDes16* aPckg2)
/**
 * Retrieve information about a named Phonebook Store.
 * @param aReqHandle	The TSY request handle associated with this request.
 * @param aPckg1		The parameter package containing the index of the SMS Store for which
 *						information is going to be retrieved.
 * @param aPckg2		The parameter package in which the retrieved SMS Store information will
 *						be passed back to the client.
 * @return TInt			Standard error value.
 */
	{
	TPckg<RMobilePhoneStore::TMobilePhoneStoreInfoV1>* infoPckg=(TPckg<RMobilePhoneStore::TMobilePhoneStoreInfoV1>*)aPckg1;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1& info=(*infoPckg)();

	// Check that the data structure is supported by the simulated TSY version
	TInt err = CheckSimTsyVersion(info);
	if(err != KErrNone)
		{
		ReqCompleted(aReqHandle, err);
		return KErrNone;
		}

	TInt i;
	for(i=0;i<iPhBkStores->Count();i++)
		{
		TBuf8<KMaxName> nBuf;
		nBuf.Copy(*aPckg2);
		if(iPhBkStores->At(i)->Name().MatchF(nBuf)==0)
			break;
		}

	if(i==iPhBkStores->Count())
		{
		ReqCompleted(aReqHandle,KErrNotFound);
		return KErrNone;
		}

	iPhBkStores->At(i)->PopulatePhBkStoreInfo(&info);
	ReqCompleted(aReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimPhone::GetPhoneStoreInfo(TTsyReqHandle aReqHandle,TDes8* aPckg1,
								  RMobilePhone::TPhoneStoreNameAndIccType* aTable)
/**
 * Retrieve information about a named Phonebook Store.
 * @param aReqHandle	The TSY request handle associated with this request.
 * @param aPckg1		The parameter package containing the index of the PhoneBook Store for which
 *						information is going to be retrieved.
 * @param aTable		The parameter package in which the retrieved PhoneBook Store name and type information will
 *						be passed from the client.
 * @return TInt			Standard error value.
 */
	{
	TBool found = EFalse;
	TPckg<RMobilePhoneStore::TMobilePhoneStoreInfoV1>* infoPckg=(TPckg<RMobilePhoneStore::TMobilePhoneStoreInfoV1>*)aPckg1;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1& info=(*infoPckg)();

	// Check that the data structure is supported by the simulated TSY version
	TInt err = CheckSimTsyVersion(info);
	if(err != KErrNone)
		{
		ReqCompleted(aReqHandle, err);
		return KErrNone;
		}

	TBuf8<KMaxName> n1Buf,n2Buf;

	n1Buf.Copy(aTable->iStoreName);
	n2Buf.Copy(aTable->iMode);

	TInt i;

	for(i=0;i<iPhBkUSimStores->Count();i++)
		{
		if((iPhBkUSimStores->At(i)->Name().MatchF(n1Buf)==0) &&
			(iPhBkUSimStores->At(i)->PhBkStore().MatchF(n2Buf)==0))
			{
			found = ETrue;
			break;
			}
		}

	if(!found)
		{
			for(i=0;i<iPhBkStores->Count();i++)
				{
				if((iPhBkStores->At(i)->Name().MatchF(n1Buf)==0)  &&
					n2Buf != KUSimPhoneBook)
					{
					found = ETrue;
					break;
					}
				}
			if(!found)
				{
				ReqCompleted(aReqHandle,KErrNotFound);
				return KErrNone;
				}
			else
				{
				iPhBkStores->At(i)->PopulatePhBkStoreInfo(&info);
				ReqCompleted(aReqHandle,KErrNone);
				}
		}
	else
		{
		if(iPhBkUSimStores->At(i)->CheckAndSwitchUSimApps()!=KErrNone)
			{
			ReqCompleted(aReqHandle, KErrNotFound);
			return KErrNone;
			}

		iPhBkUSimStores->At(i)->PopulatePhBkStoreInfo(&info);
		ReqCompleted(aReqHandle,KErrNone);
		}
	return KErrNone;
	}

TInt CSimPhone::GetMultimodeCaps(const TTsyReqHandle aTsyReqHandle,TUint32* aCaps)
/**
 * Retrieve the multimode capabilities of this TSY.  This is hardcoded to support GSM, GPRS and CDMA.
 * @param aTsyReqHandle		TSY Request Handle associated with this request.
 * @param aCaps				The multimode capability structure to be populated with the result.
 * @return TInt				Standard error value.
 */
	{
	*aCaps = RMobilePhone::KCapsGsmSupported |
	         RMobilePhone::KCapsGprsSupported |
	         RMobilePhone::KCapsEapSupported | 
	         RMobilePhone::KCapsWcdmaSupported;

	ReqCompleted(aTsyReqHandle,KErrNone);

	return KErrNone;
	}

TInt CSimPhone::GetIccAccessCaps(const TTsyReqHandle aTsyReqHandle,TUint32* aCaps)
/**
 * Retrieve the access capabilities of this ICC using SimTSY.  This is hardcoded to support SIM, RUIM and USIM.
 * @param aTsyReqHandle		TSY Request Handle associated with this request.
 * @param aCaps				The multimode capability structure to be populated with the result.
 * @return TInt				Standard error value.
 */
	{
	*aCaps = RMobilePhone::KCapsSimAccessSupported | RMobilePhone::KCapsRUimAccessSupported | RMobilePhone::KCapsUSimAccessSupported;
	ReqCompleted(aTsyReqHandle,KErrNone);

	return KErrNone;
	}

TInt CSimPhone::GetSubscriberInfo(TTsyReqHandle aReqHandle,TDes8* aPckg1)
/**
 * Retrieve the Subscriber Identifier.
 * @param aReqHandle	The TSY request handle associated with this request.
 * @param aPckg1		The first parameter package.  This will be populated with the
 *						subscriber information.
 * @return TInt			Standard error value.
 */
	{
	TPckg<RMobilePhone::TMobilePhoneSubscriberId>* subscribePckg=(TPckg<RMobilePhone::TMobilePhoneSubscriberId>*)aPckg1;
	RMobilePhone::TMobilePhoneSubscriberId& subscribe=(*subscribePckg)();
	TInt ret = KErrNone;
	if (iSubscriberId.iError == KErrNone)
		{
		if(iSubscriberId.iIMSI.Length()>RMobilePhone::KIMSISize)
		    {
			subscribe.Copy(iSubscriberId.iIMSI.Left(RMobilePhone::KIMSISize));
		    }
		else
		    {
			subscribe.Copy(iSubscriberId.iIMSI);
		    }
		}
	else
	    {
		ret = iSubscriberId.iError;
	    }
	ReqCompleted(aReqHandle,ret);
	return KErrNone;
	}

const CTestConfigSection* CSimPhone::CfgFile()
/**
*	Return a pointer to the Configuration File Section
*
* @return CTestConfigSection	pointer to the configuration file section
*/
	{
	return iConfigSection;
	}

const CTestConfigSection* CSimPhone::DefaultCfgFile()
/**
*	Return a pointer to the Default Configuration File Section
*
* @return CTestConfigSection	pointer to the default configuration file section
*/
	{
	if( iConfigFile == NULL )
	    {
	    return NULL;
	    }
	return iConfigFile->Section(KScriptDefaults);
	}

TInt CSimPhone::ValidateChangeState(CSimLine* aOriginatingLine, RMobileCall::TMobileCallStatus aState)
/**
*	This function validates the proposed state change by using the iActiveLine
*	pointer member variable.  If the proposed change of state would clash with
*	an existing call, the change is	errored.
*	The function also updates the iMode state to reflect the new call state.
*
* @param aOriginatingLine pointer to the line requesting for the change of state
* @param aState the state the aOriginatingLine wants to change to
* @return Error indicates whether the change of state is successful or not
*/
	{
	__ASSERT_ALWAYS(iMode!=RPhone::EModeUnknown,SimPanic(EPhoneModeUnknownIllegal));
	__ASSERT_ALWAYS(iNtwkMode!=RMobilePhone::ENetworkModeUnknown,SimPanic(ENetworkModeUnknownIllegal));

// If there is no active line defined, then any state changes are fine.  However,
// we need to watch for a shift to an "active" status.
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_VALIDATECHANGESTATE_1, ">>CSimPhone::ValidateChangeState");
	if(!iActiveLine)
		{
		iMode=ConvertStateToMode(aState);
		if(IsStateActive(aState))
			iActiveLine=aOriginatingLine;
		// If a call is in progress, we may need to trigger NotifyIndicatorChange
		CheckIndicatorNotification();
		return KErrNone;
		}

// If this state change request is coming from the active line then any state changes
// are ok, but we must watch for a change that would "deactivate" the line.
	if(iActiveLine==aOriginatingLine)
		{
		iMode=ConvertStateToMode(aState);
		if(!IsStateActive(aState))
			iActiveLine=NULL;
		// If a call is in progress, we may need to trigger NotifyIndicatorChange
		CheckIndicatorNotification();
		return KErrNone;
		}

// If we have got this far, then there is an active line, but this state change
// request has not come from it.  If this is the case, then a change to ringing
// or Idle are the only valid state changes.

	if((aState==RMobileCall::EStatusRinging) ||
	   (aState==RMobileCall::EStatusIdle))
		return KErrNone;

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_VALIDATECHANGESTATE_2, "<<CSimPhone::ValidateChangeState");
	return KErrGeneral;
	}

TInt CSimPhone::FindActiveVoiceCall(CSimVoiceCall*& aCall)
/**
 * Find an active voice call.  Return KErrNotFound if no voice calls active.
 * @param aCall		Pointer to active voice call.
 * @return TInt		Standard return error.
 */
	{
	return iVoiceLine->FindActiveVoiceCall(aCall);
	}

TInt CSimPhone::ValidateChangeState(RPacketService::TStatus aState)
/**
*	This function validates the proposed state change by using the iActiveLine
*	pointer member variable.  If the proposed change of state would clash with
*	an existing call, the change is	errored.
*	The function also updates the iMode state to reflect the new call state.
*
* @param aOriginatingLine pointer to the line requesting for the change of state
* @param aState the state the aOriginatingLine wants to change to
* @return Error indicates whether the change of state is successful or not
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_VALIDATECHANGESTATE1_1, ">>CSimPhone::ValidateChangeState packet");
	__ASSERT_ALWAYS(iMode!=RPhone::EModeUnknown,SimPanic(EPhoneModeUnknownIllegal));
	__ASSERT_ALWAYS(iNtwkMode!=RMobilePhone::ENetworkModeUnknown,SimPanic(ENetworkModeUnknownIllegal));
	__ASSERT_ALWAYS(iPacketService->MSClass()!=RPacketService::EMSClassUnknown,SimPanic(EPacketMSClassUnKnown));

//we must know the type of class the phone supports in order to determine what action to take.
	switch(iPacketService->MSClass())
		{
	case RPacketService::EMSClassDualMode:				//< Active Simultaneous PS & CS calls supported (ClassA)
		return KErrNone;
	case RPacketService::EMSClassSuspensionRequired:	//< Active CS & Suspended PS simultaneous calls supported (ClassB)
		if(iMode == RPhone::EModeIdle)
			return KErrNone;
		else if((aState == RPacketService::EStatusUnattached) || (aState == RPacketService::EStatusSuspended))
			return KErrNone;
		else return KErrNotSupported; //Should actually force a suspend of the packet context and service
	case RPacketService::EMSClassAlternateMode:			//< Active CS or Active PS only call supported (ClassC)
		if(iMode == RPhone::EModeIdle)
			return KErrNone;
		else if(aState == RPacketService::EStatusUnattached)
			return KErrNone;
		else return KErrNotSupported;
	case RPacketService::EMSClassCircuitSwitchedOnly:	//< Active CS only call supported (ClassC)
		return KErrNotSupported; //Should actually force a detach from the network if not in unattached state already
	case RPacketService::EMSClassPacketSwitchedOnly:	//< Active PS only call supported (ClassC)
		return KErrNone; //Should disconnect all active calls if mode is not Idle.
	default:
		return KErrNotSupported;
		}
	}

RMobilePhone::TMobilePhoneBatteryStatus CSimPhone::BatteryStatus()
	{
	return iBatteryCharger->Status();
	}

RMobilePhone::TMobilePhoneRegistrationStatus CSimPhone::RegistrationStatus()
	{
	return iNetworkStatus->RegistrationStatus();
	}

void CSimPhone::CheckIndicatorNotification()
	{
	iIndicator->CheckNotification();
	}

/**
* A utility function for checking whether the first pin on the phone 
* requires authentication 
*
* @return TBool true if required and false if not 
*/
TBool CSimPhone::IsICCLocked()
	{
	return iPhoneSecurity->IsICCLocked();
	}

/**
* A utility function for checking whether the second pin on the phone 
* requires authentication 
*
* @return TBool true if required and false if not 
*/
TBool CSimPhone::IsPIN2Locked()
	{
	return iPhoneSecurity->IsPIN2Locked();
	}

TBool CSimPhone::IsHiddenEnabled()
	{
	return iPhoneSecurity->IsHiddenEnabled();
	}

void CSimPhone::SecurityEvent(RMobilePhone::TMobilePhoneSecurityEvent aEvent)
	{
	iPhoneSecurity->SecurityEvent(aEvent);
	}

RMobilePhone::TAID CSimPhone::GetActiveUSim()
	{
	if(iPhoneUSimApp->FoundUSimAppTags())
		{
		return iPhoneUSimApp->GetActiveUSim();
		}
	else
		{
		return iPhoneSmartCardApp->GetActiveUSim();
		}
	
	}

RMobilePhone::TMobilePhoneSubscriberId CSimPhone::GetImsi()
	{
	return iSubscriberId.iIMSI;
	}
void CSimPhone::GetPhoneIdAndCaps()
	{
	TPtrC8 imei, model, manufacturer, revision;
	const CTestConfigItem* item=CfgFile()->Item(KPhoneId);
	if (item)
		{
		TInt ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,manufacturer);
		if(ret!=KErrNone)
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_GETPHONEIDANDCAPS_1, "WARNING - CONFIGURATION FILE PARSING - Reading element MANUFACTURER returned %d (element no. %d) from tag %s.",ret,0,KPhoneId);
		ret = CTestConfig::GetElement(item->Value(),KStdDelimiter,1,model);
		if(ret!=KErrNone)
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_GETPHONEIDANDCAPS_2, "WARNING - CONFIGURATION FILE PARSING - Reading element MODEL returned %d (element no. %d) from tag %s.",ret,1,KPhoneId);
		ret = CTestConfig::GetElement(item->Value(),KStdDelimiter,2,revision);
		if(ret!=KErrNone)
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_GETPHONEIDANDCAPS_3, "WARNING - CONFIGURATION FILE PARSING - Reading element REVISION returned %d (element no. %d) from tag %s.",ret,2,KPhoneId);
		ret = CTestConfig::GetElement(item->Value(),KStdDelimiter,3,imei);
		if(ret!=KErrNone)
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_GETPHONEIDANDCAPS_4, "WARNING - CONFIGURATION FILE PARSING - Reading element IMEI returned %d (element no. %d) from tag %s.",ret,3,KPhoneId);
		ret = CTestConfig::GetElement(item->Value(),KStdDelimiter,4,iPhoneId.iError);

		iPhoneId.iManufacturerId.Copy(manufacturer);
		iPhoneId.iModelId.Copy(model);
		iPhoneId.iRevisionId.Copy(revision);
		iPhoneId.iSerialId.Copy(imei);
		}
	else
		{
		//set to default
		iPhoneId.iManufacturerId.Copy(KPhoneManufacturerDefault);
		iPhoneId.iModelId.Copy(KPhoneModelDefault);
		iPhoneId.iRevisionId.Copy(KPhoneRevesionDefault);
		iPhoneId.iSerialId.Copy(KPhoneSerialNumberDefault);
		iPhoneId.iError = KErrNone;
		}

	//Set Phone Identity Caps
	iPhoneIdCaps = 0;
	if(iPhoneId.iManufacturerId.Length() > 0)
		iPhoneIdCaps |= RMobilePhone::KCapsGetManufacturer;

	if(iPhoneId.iModelId.Length() > 0)
		iPhoneIdCaps |=  RMobilePhone::KCapsGetModel;

	if(iPhoneId.iRevisionId.Length() > 0)
		iPhoneIdCaps |=  RMobilePhone::KCapsGetRevision;

	if(iPhoneId.iSerialId.Length() > 0)
		iPhoneIdCaps |=  RMobilePhone:: KCapsGetSerialNumber;

	if(iSubscriberId.iIMSI.Length() > 0)
		iPhoneIdCaps |=  RMobilePhone::KCapsGetSubscriberId;
	}

TInt CSimPhone::GetIdentityCaps(TTsyReqHandle aReqHandle, TUint32& aCaps)
	{
	aCaps = iPhoneIdCaps;
	ReqCompleted(aReqHandle,KErrNone);
	return KErrNone;
	}
	
/**
* A utility function for checking whether a service is active in the service table
*
* @param aTable pointer to the table to be tested for active service
* @param aOffset to pointer in the service table
* @param aModifier which service to be tested for in the table
* @return TBool true if active and false if not enabled
*/		
TBool CSimPhone::ServiceEnabled(RMobilePhone::TMobilePhoneServiceTableV1* aTable, 
								TInt aOffset, 
								TUint8 aModifier)
	{
	if (aTable)
		{
		TUint8* pService = &(aTable->iServices1To8);
		TUint8 service = *(pService+aOffset);		
		if (service & aModifier)
			{
			return true;
			}
		}
	return false;
	}

/**
* An IPC support function for counting the number of entries in the Acces Point Name (APN) List
*
* @param aReqHandle handle to the IPC call used for completing the round trip
* @param aIndex a pointer to some memory for storing the number of entries
* @return TInt always returns KErrNone
*/
TInt CSimPhone::EnumerateAPNEntries(TTsyReqHandle aReqHandle, TUint32* aIndex)
	{
	if (IsICCLocked())
		{
		SYMBIAN_REQEXTERR(aReqHandle, KErrGsm0707SimPin1Required, KErrAccessDenied);
		return KErrNone;
		}
		
	if (ServiceEnabled(iUSIMServiceTable,iAPNServiceOffset,RMobilePhone::KUstACL))
		{
		if (ServiceEnabled(iUSIMEnabledServiceTable,iAPNServiceOffset,RMobilePhone::KUstACL))
			{
			*aIndex = iAPNList.Count();
			ReqCompleted(aReqHandle,KErrNone);
			}
		else
			{
			SYMBIAN_REQEXTERR(aReqHandle,KErrMMEtelAPNEnabledServicesTableNotFound,KErrNotFound);
			}		
		}
	else
		{
		SYMBIAN_REQEXTERR(aReqHandle,KErrMMEtelAPNNameACLNotFound,KErrNotSupported);
		}

	return KErrNone;
	}
	
/**
* An IPC support function for retrieving a name in the Acces Point Name (APN) List
*
* @param aReqHandle handle to the IPC call used for completing the round trip
* @param aIndex a index into the APN List for the name to be retrieved
* @param aRetrieved a TAPNEntryV3Pckg to store the name from the APN List
* @return TInt always returns KErrNone
*/	
TInt CSimPhone::GetAPNname(	TTsyReqHandle aReqHandle, 
							TUint32& aIndex, 
							RMobilePhone::TAPNEntryV3Pckg& aRetrieved)
	{
	if (IsICCLocked())
		{
		ReqCompleted(aReqHandle,SYMBIAN_EXTERR(	KErrGsm0707SimPin1Required,	KErrAccessDenied));		
		return KErrNone;
		}
			
	if (ServiceEnabled(iUSIMServiceTable,iAPNServiceOffset,RMobilePhone::KUstACL))
		{
		if (ServiceEnabled(iUSIMEnabledServiceTable,iAPNServiceOffset,RMobilePhone::KUstACL))
			{	
			if (aIndex < iAPNList.Count() )
				{
				RMobilePhone::TAPNEntryV3& tAPNEntryV3 = aRetrieved();
				
				tAPNEntryV3.iApn.Zero();
				tAPNEntryV3.iApn.Insert(0,iAPNList[aIndex].iApn);
				
				ReqCompleted(aReqHandle,KErrNone);
				}
			else
				{
				ReqCompleted(aReqHandle,SYMBIAN_EXTERR(KErrMMEtelAPNNameInvalidIndex,KErrNotFound));
				}
			}
		else
			{
			ReqCompleted(aReqHandle,SYMBIAN_EXTERR(	KErrMMEtelAPNEnabledServicesTableNotFound,
													KErrNotFound));
			}		
		}
	else
		{
		ReqCompleted(aReqHandle,SYMBIAN_EXTERR(	KErrMMEtelAPNNameACLNotFound,
												KErrNotSupported));
		}
	return KErrNone;		
	}
	
/**
* An IPC support function for adding a name to the Acces Point Name (APN) List
*
* @param aReqHandle handle to the IPC call used for completing the round trip
* @param aIndex a index into the APN List for the name to be retrieved
* @param aEntry a TAPNEntryV3Pckg to add to the end of the APN List
* @return TInt always returns KErrNone
*/				
TInt CSimPhone::AppendAPNName(TTsyReqHandle aReqHandle, RMobilePhone::TAPNEntryV3Pckg& aEntry)
	{
	if (IsPIN2Locked())
		{
		ReqCompleted(aReqHandle,SYMBIAN_EXTERR(	KErrGsm0707SimPin2Required,	KErrAccessDenied));		
		return KErrNone;
		}
			
	if (ServiceEnabled(iUSIMServiceTable,iAPNServiceOffset,RMobilePhone::KUstACL))
		{
		if (ServiceEnabled(iUSIMEnabledServiceTable,iAPNServiceOffset,RMobilePhone::KUstACL))
			{		
			RMobilePhone::TAPNEntryV3& entry = (aEntry)();
			iAPNList.Append(entry);
			
			ReqCompleted(aReqHandle,KErrNone);
			
			if (iAPNListNotify.iNotifyPending == TRUE)
				{
				iAPNListNotify.iNotifyPending = EFalse;
				ReqCompleted(iAPNListNotify.iNotifyHandle,KErrNone);
				}
			}
		else
			{
			ReqCompleted(aReqHandle,SYMBIAN_EXTERR(	KErrMMEtelAPNEnabledServicesTableNotFound,
													KErrNotFound));
			}		
		}
	else
		{
		ReqCompleted(aReqHandle,SYMBIAN_EXTERR(	KErrMMEtelAPNNameACLNotFound,
												KErrNotSupported));
		}
			
	return KErrNone;
	}	
	
/**
* An IPC support function for removing a name from the Acces Point Name (APN) List
*
* @param aReqHandle handle to the IPC call used for completing the round trip
* @param aIndex a index into the APN List for the name to be deleted
* @return TInt always returns KErrNone
*/		
TInt CSimPhone::DeleteAPNName(TTsyReqHandle aReqHandle, TUint32& aIndex)
	{
	if (IsPIN2Locked())
		{
		ReqCompleted(aReqHandle,SYMBIAN_EXTERR(	KErrGsm0707SimPin2Required,	KErrAccessDenied));		
		return KErrNone;
		}
			
	if (ServiceEnabled(iUSIMServiceTable,iAPNServiceOffset,RMobilePhone::KUstACL))
		{
		if (ServiceEnabled(iUSIMEnabledServiceTable,iAPNServiceOffset,RMobilePhone::KUstACL))
			{		
			if (aIndex < iAPNList.Count())
				{
				iAPNList.Remove(aIndex);
				iAPNList.Compress();
				ReqCompleted(aReqHandle,KErrNone);
				if (iAPNListNotify.iNotifyPending == TRUE)
					{
					iAPNListNotify.iNotifyPending = EFalse;
					ReqCompleted(iAPNListNotify.iNotifyHandle,KErrNone);
					}
				}
			else
				{
				ReqCompleted(aReqHandle,SYMBIAN_EXTERR(KErrMMEtelAPNNameInvalidIndex,KErrNotFound));			
				}
			}
		else
			{
			ReqCompleted(aReqHandle,SYMBIAN_EXTERR(	KErrMMEtelAPNEnabledServicesTableNotFound,
													KErrNotFound));
			}		
		}
	else
		{
		ReqCompleted(aReqHandle,SYMBIAN_EXTERR(	KErrMMEtelAPNNameACLNotFound,
												KErrNotSupported));
		}
	return KErrNone;
	}

/**
* An IPC support function for registering interest in APN List changes
*
* @param aReqHandle handle to the IPC call used for completing the round trip
* @return TInt always returns KErrNone
*/		
TInt CSimPhone::NotifyAPNListChanged(TTsyReqHandle aReqHandle)
	{
	if (ServiceEnabled(iUSIMServiceTable,iAPNServiceOffset,RMobilePhone::KUstACL))
		{
		if (ServiceEnabled(iUSIMEnabledServiceTable,iAPNServiceOffset,RMobilePhone::KUstACL))
			{
			iAPNListNotify.iNotifyPending = ETrue;
			iAPNListNotify.iNotifyHandle = aReqHandle;
			}
		else
			{
			ReqCompleted(aReqHandle,SYMBIAN_EXTERR(	KErrMMEtelAPNEnabledServicesTableNotFound,
													KErrNotFound));
			}		
		}
	else
		{
		ReqCompleted(aReqHandle,SYMBIAN_EXTERR(	KErrMMEtelAPNNameACLNotFound,
												KErrNotSupported));
		}
	return KErrNone;	
	}
	
/**
* An IPC support function for switching the Acces Point Name (APN) List to enabled or disabled
*
* @param aReqHandle handle to the IPC call used for completing the round trip
* @param aAPNControlListServiceStatus service status to be set
* @return TInt always returns KErrNone
*/		
TInt CSimPhone::SetAPNControlListServiceStatus(TTsyReqHandle aReqHandle, 
		RMobilePhone::TAPNControlListServiceStatus& aAPNControlListServiceStatus)	
	{
	if (IsPIN2Locked())
		{
		ReqCompleted(aReqHandle,SYMBIAN_EXTERR(	KErrGsm0707SimPin2Required,	KErrAccessDenied));		
		return KErrNone;
		}
			
	if (ServiceEnabled(iUSIMServiceTable,iAPNServiceOffset,RMobilePhone::KUstACL))
		{
		switch (aAPNControlListServiceStatus)
			{
			case RMobilePhone::EAPNControlListServiceDisabled:
				{
				if (iUSIMEnabledServiceTable)
					{
					iUSIMEnabledServiceTable->iServices33To40 = 
					(iUSIMEnabledServiceTable->iServices33To40 & RMobilePhone::KUstACL) 
						^ iUSIMEnabledServiceTable->iServices33To40;
						
					ReqCompleted(aReqHandle,KErrNone);
					if (iAPNStatusNotify.iNotifyPending == TRUE)
						{
						*static_cast<RMobilePhone::TAPNControlListServiceStatus*>(iAPNStatusNotify.iNotifyData) = 
								RMobilePhone::EAPNControlListServiceDisabled;
						iAPNStatusNotify.iNotifyPending = EFalse;
						ReqCompleted(iAPNStatusNotify.iNotifyHandle,KErrNone);
						}					
					}
				else
					{
					ReqCompleted(aReqHandle,SYMBIAN_EXTERR(	KErrMMEtelAPNEnabledServicesTableNotFound,
															KErrNotSupported));					
					}
				}
				break;				
			case RMobilePhone::EAPNControlListServiceEnabled:
				{
				if (iUSIMEnabledServiceTable)
					{
					iUSIMEnabledServiceTable->iServices33To40 |= RMobilePhone::KUstACL;
					ReqCompleted(aReqHandle,KErrNone);
					if (iAPNStatusNotify.iNotifyPending == TRUE)
						{
						*static_cast<RMobilePhone::TAPNControlListServiceStatus*>(iAPNStatusNotify.iNotifyData) = 
								RMobilePhone::EAPNControlListServiceEnabled;						
						iAPNStatusNotify.iNotifyPending = EFalse;
						ReqCompleted(iAPNStatusNotify.iNotifyHandle,KErrNone);
						}
					}
				else
					{
					ReqCompleted(aReqHandle,SYMBIAN_EXTERR(	KErrMMEtelAPNEnabledServicesTableNotFound,
															KErrNotSupported));	
					}						
				}
				break;	
			default:
				ReqCompleted(aReqHandle,SYMBIAN_EXTERR(	KErrMMEtelFormatNotSupported,
														KErrNotFound));				
			}
		}
	else
		{
		ReqCompleted(aReqHandle,SYMBIAN_EXTERR(	KErrMMEtelAPNEnabledServicesTableNotFound,
												KErrNotSupported));		
		}
	return KErrNone;	
	}
	
/**
* An IPC support function for retrieving the Acces Point Name (APN) List enabled/ disabled
* status
*
* @param aReqHandle handle to the IPC call used for completing the round trip
* @param aAPNControlListServiceStatus memory for storing the current status for the user
* @return TInt always returns KErrNone
*/		
TInt CSimPhone::GetAPNControlListServiceStatus(TTsyReqHandle aReqHandle, 
		RMobilePhone::TAPNControlListServiceStatus& aAPNControlListServiceStatus)
	{
	if (IsICCLocked())
		{
		ReqCompleted(aReqHandle,SYMBIAN_EXTERR(	KErrGsm0707SimPin1Required,	KErrAccessDenied));		
		return KErrNone;
		}
			
	if (ServiceEnabled(iUSIMServiceTable,iAPNServiceOffset,RMobilePhone::KUstACL))
		{
		if (ServiceEnabled(iUSIMEnabledServiceTable,iAPNServiceOffset,RMobilePhone::KUstACL))
			{
			aAPNControlListServiceStatus = RMobilePhone::EAPNControlListServiceEnabled;
			}
		else
			{
			aAPNControlListServiceStatus = RMobilePhone::EAPNControlListServiceDisabled;
			}	
		ReqCompleted(aReqHandle,KErrNone);	
		}
	else
		{
		ReqCompleted(aReqHandle,SYMBIAN_EXTERR(	KErrMMEtelAPNEnabledServicesTableNotFound,
												KErrNotSupported));				
		}
	return KErrNone;	
	}
	
/**
* An IPC support function for registering interest in the Acces Point Name (APN) List 
* enabled/ disabled status changing
*
* @param aReqHandle handle to the IPC call used for completing the round trip
* @param aAPNControlListServiceStatus memory for storing the current status for the user
* @return TInt always returns KErrNone
*/			
TInt CSimPhone::NotifyAPNControlListServiceStatusChange(TTsyReqHandle aReqHandle, 
		RMobilePhone::TAPNControlListServiceStatus& aAPNControlListServiceStatus)
	{
	iAPNStatusNotify.iNotifyPending = ETrue;
	iAPNStatusNotify.iNotifyHandle = aReqHandle;
	iAPNStatusNotify.iNotifyData = &aAPNControlListServiceStatus;
	return KErrNone;	
	}
	
/**
* A utility function for checking table availabilty and performing a copy on it
*
* @param aFrom The TMobilePhoneServiceTableV1 table to copied out of
* @param aTo The TMobilePhoneServiceTableV1 table to copied into
* @return TInt returns KErrNone for success and KErrNotSupported if the table aren't 
* valid
*/		
TInt CSimPhone::CopyServiceTable(RMobilePhone::TMobilePhoneServiceTableV1* aFrom, 
		RMobilePhone::TMobilePhoneServiceTableV1* aTo)
	{
	if (aFrom && aTo)
		{
		*aTo = *aFrom;
		return KErrNone;
		}
	else 
		{
		// This value is returned because this function tests the existance of the table 
		// not just that the args are OK.
		return SYMBIAN_EXTERR(KErrMMEtelAPNEnabledServicesTableNotFound, KErrNotSupported);
		}
	}
	
/**
 * A utility function for checking table availabilty and performing a copy on it
 *
 * @param aFrom The TMobilePhoneServiceTableV8 table to copied out of
 * @param aTo The TMobilePhoneServiceTableV8 table to copied into
 * @return TInt returns KErrNone for success and KErrNotSupported if the table aren't 
 * valid
 */		
 TInt CSimPhone::CopyServiceTableV8(RMobilePhone::TMobilePhoneServiceTableV8* aFrom, 
 		RMobilePhone::TMobilePhoneServiceTableV8* aTo)
 	{
 	if (aFrom && aTo)
 		{
 		*aTo = *aFrom;
 		return KErrNone;
 		}
 	else 
 		{
 		// This value is returned because this function tests the existance of the table 
 		// not just that the args are OK.
 		return SYMBIAN_EXTERR(KErrMMEtelAPNEnabledServicesTableNotFound, KErrNotSupported);
 		}
 	}

/**
* An IPC support function for retrieving one of the service tables
*
* @param aReqHandle handle to the IPC call used for completing the round trip
* @param aTable the type of table the user has interest in
* @param aPCmd memory to store the retrieved table
* @return TInt always returns KErrNone
*/		
TInt CSimPhone::GetServiceTable(TTsyReqHandle aReqHandle, 
		RMobilePhone::TMobilePhoneServiceTable aTable, 
		TDes8* aPCmd)	
	{
	TInt ret = KErrNone;
	RMobilePhone::TMobilePhoneServiceTableV1Pckg *pTablePckg = (RMobilePhone::TMobilePhoneServiceTableV1Pckg*) aPCmd;
	RMobilePhone::TMobilePhoneServiceTableV1 &aSst = (*pTablePckg)();
	
	switch (aTable)
		{

		/** Retrieve SIM service table from active SIM application on UICC.
		 Modes: GSM/WCDMA */
		case RMobilePhone::ESIMServiceTable:
			ret = CopyServiceTable (iSIMServiceTable, &aSst);
			break;
			/** Retrieve USIM service table from active USIM application on UICC.
			 
			 Modes: WCDMA */
		case RMobilePhone::EUSIMServiceTable:
			if ( aSst.ExtensionId ()== KEtelExtMultimodeV8)
				{
				RMobilePhone::TMobilePhoneServiceTableV8Pckg* pTableV8Pckg = (RMobilePhone::TMobilePhoneServiceTableV8Pckg*) aPCmd;
				RMobilePhone::TMobilePhoneServiceTableV8 &pTableV8 = (*pTableV8Pckg) ();
				ret = CopyServiceTableV8 (iUSIMServiceTableV8, &pTableV8);
				}
			else
				{
				ret = CopyServiceTable (iUSIMServiceTable, &aSst);
				}
			break;
			/** USIM Enabled Services Table to be used in conjunction with the USIM Service Table.
			 
			 Modes: WCDMA */
		case RMobilePhone::EUSIMEnabledServiceTable:
			ret = CopyServiceTable (iUSIMEnabledServiceTable, &aSst);
			break;
		default:
			ReqCompleted (aReqHandle, SYMBIAN_EXTERR( KErrMMEtelFormatNotSupported,
					KErrArgument));
			return KErrNone;
		}
	// Extended error codes are handled here by the CopyServiceTable() func.
	ReqCompleted(aReqHandle,ret);
	return KErrNone;
	}

TInt CSimPhone::GetPhoneId(TTsyReqHandle aReqHandle,RMobilePhone::TMobilePhoneIdentityV1* aPhoneId)
	{

	//complete with error if error specified.
	if(iPhoneId.iError != KErrNone)
		{
		ReqCompleted(aReqHandle,iPhoneId.iError);
		return KErrNone;
		}
	//fill out phone identity.
	if(iPhoneIdCaps & RMobilePhone:: KCapsGetManufacturer)
		{
		if(iPhoneId.iManufacturerId.Length() > RMobilePhone::KPhoneManufacturerIdSize)
			aPhoneId->iManufacturer.Copy(iPhoneId.iManufacturerId.Left(RMobilePhone::KPhoneManufacturerIdSize));
		else
			aPhoneId->iManufacturer.Copy(iPhoneId.iManufacturerId);
		}

	if(iPhoneIdCaps & RMobilePhone::KCapsGetModel)
		{
		if(iPhoneId.iModelId.Length() > RMobilePhone::KPhoneModelIdSize)
			aPhoneId->iModel.Copy(iPhoneId.iModelId.Left(RMobilePhone::KPhoneModelIdSize));
		else
			aPhoneId->iModel.Copy(iPhoneId.iModelId);
		}

	if(iPhoneIdCaps & RMobilePhone::KCapsGetRevision)
		{
		if(iPhoneId.iRevisionId.Length() > RMobilePhone::KPhoneRevisionIdSize)
			aPhoneId->iRevision.Copy(iPhoneId.iRevisionId.Left(RMobilePhone::KPhoneRevisionIdSize));
		else
			aPhoneId->iRevision.Copy(iPhoneId.iRevisionId);
		}

	if(iPhoneIdCaps & RMobilePhone::KCapsGetSerialNumber)
		{
		if(iPhoneId.iSerialId.Length() > RMobilePhone::KPhoneSerialNumberSize)
			aPhoneId->iSerialNumber.Copy(iPhoneId.iSerialId.Left(RMobilePhone::KPhoneSerialNumberSize));
		else
			aPhoneId->iSerialNumber.Copy(iPhoneId.iSerialId);
		}

	ReqCompleted(aReqHandle,KErrNone);
	return KErrNone;
	}

/**
 * Callback function invoked by the observer object when test number property is changed.
 * This function is supposed to reset SIMTSY. Currently only SMS messaging part of SIMTSY
 * is re-started with the new test number.
 */
void CSimPhone::HandleTestNumberChangedL()
    {
    SetTestNumberAndReadConfigurationFileL();
    iSmsMessaging->ReloadConfigurationSettingsL();
    }

TInt CSimPhone::CheckConfigFile()
    {
    TInt testNumber;
    User::LeaveIfError(GetTestNumber(testNumber));
    iSectionName.Format(KSectionNameFormat,testNumber);
    
    CTestConfig* configFile = NULL; 

    TRAPD(err, configFile = CTestConfig::NewLC(iFs, KConfigFileDir, KConfigFilename); CleanupStack::Pop(configFile));
    if( err != KErrNone || configFile->Section(iSectionName) != NULL )
        {
        err = KErrNone;
        }
    else
        {
        err = KErrNotFound;
        }
    delete configFile;
    return err;
    }
/**
	Constructor for suspend call processing timer
*/
CSimPhone::CSetCallProcessingSuspendStateTimerCallBack::CSetCallProcessingSuspendStateTimerCallBack()
	{}

/**
	Must be called before setting the timer. Simply stores the request to be completed when
	timer expires

 @param the pointer to the phone that the request will be completed on when the timer expires
 @param aReqHandle handle to request that will be completed when the timer expires
*/
void CSimPhone::CSetCallProcessingSuspendStateTimerCallBack::SetHandle(CSimPhone* aPhone, const TTsyReqHandle aHandle)
	{
	iPhone = aPhone;
	iHandle = aHandle;
	}

/**
	Implementation of MTimerCallback::TimerCallBack
	Simply completes the suspend call processing request after the delay
*/
void CSimPhone::CSetCallProcessingSuspendStateTimerCallBack::TimerCallBack(TInt /*aId*/)
	{
	iPhone->ReqCompleted(iHandle,KErrNone);
	}

/**
	Constructor for network mode processing timer
*/
CSimPhone::CNetworkModeTimerCallBack::CNetworkModeTimerCallBack()
	{
	}

void CSimPhone::CNetworkModeTimerCallBack::SetHandle(CSimPhone* aPhone)
	{
	iPhone = aPhone;
	}
/**
	Implementation of MTimerCallback::TimerCallBack
	Simply completes the suspend call processing request after the delay
*/
void CSimPhone::CNetworkModeTimerCallBack::TimerCallBack(TInt /*aId*/)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_CNETWORKMODETIMERCALLBACK_TIMERCALLBACK_1, ">>CSimPhone::CNetworkModeTimerCallBack::TimerCallBack");
	iPhone->TimerCallBackNetworkMode();
	}

TInt CSimPhone::NotifyModeChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneNetworkMode* aCaps)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_NOTIFYMODECHANGE_1, "CSimPhone::NotifyModeChange called");
	__ASSERT_ALWAYS(!iNotifyNetworkModeChange.iNotifyPending,SimPanic(ENotificationAlreadyPending));
 
	if (iNetworkModeArray->Count() == 0)
		{
		ReqCompleted(aTsyReqHandle,KErrNotSupported);
		return KErrNone;
		}
	
	iNotifyNetworkModeChange.iNotifyPending = ETrue;
	iNotifyNetworkModeChange.iNotifyHandle = aTsyReqHandle;
	iNotifyNetworkModeChange.iNotifyData = aCaps;
	return KErrNone;
	}

/**
* Cancels a clients interest in the change of dynamic caps.
*
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
TInt CSimPhone::NotifyModeChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_NOTIFYMODECHANGECANCEL_1, "CSimPhone::NotifyModeChangeCancel called");
	if( (iNotifyNetworkModeChange.iNotifyPending) && (aTsyReqHandle == iNotifyNetworkModeChange.iNotifyHandle))
		{
		iNotifyNetworkModeChange.iNotifyPending=EFalse;
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	return KErrNone;
	}

/**
* Timer callback function to simulate a change in the Network Mode of the phone
*/
void CSimPhone::TimerCallBackNetworkMode()
	{
	if (++iNetworkModeIndex >= iNetworkModeArray->Count())
		{
		return;
		}
	iNtwkMode = iNetworkModeArray->At(iNetworkModeIndex).iNetworkMode;
	if (iNotifyNetworkModeChange.iNotifyPending)
		{
		iNotifyNetworkModeChange.iNotifyPending = EFalse;
		*(RMobilePhone::TMobilePhoneNetworkMode*)iNotifyNetworkModeChange.iNotifyData = (RMobilePhone::TMobilePhoneNetworkMode)iNtwkMode;
		ReqCompleted(iNotifyNetworkModeChange.iNotifyHandle,KErrNone);
		}
	
	iNetworkModeTimer->Start(iNetworkModeArray->At(iNetworkModeIndex).iDuration, &iTimerCallBackNetworkMode);
	}

TInt CSimPhone::CheckSimTsyVersion(RMobilePhone::TMultimodeType& aDataStruct) // overload this for other types of structures
/**
* Checks the version of a data structure against the (simulated) version of SIMTSY, in order
* to determine if the version of the API used is compatible with the version of the TSY used.
*
* @param aDataStruct A versioned data-holding structure, derived from RMobilePhone::TMultimodeType
*
* @return KErrNone if the version of aDataStruct is supported by the user-defined simulated TSY version. KErrNotSupported otherwise
*/
	{
	TInt ret = KErrNone;
	
	//retrieval of data version
	TInt dataVersion;
	switch (aDataStruct.ExtensionId())
		{
		case KETelExtMultimodeV1:
		case RMobilePhoneStore::KETelMobilePhonebookStoreV1:
		case RMobileSmsMessaging::KETelMobileSmsSendAttributesV1:
		case KETelExt3rdPartyV1:
			dataVersion = 1;
			break;
		case KETelExtMultimodeV2:
		//case KETelMobilePhonebookStoreV2: - omitted because it is equal to KETelExtMultimodeV2
			dataVersion = 2;
			break;
		case KETelExtMultimodeV3:
			dataVersion = 3;
			break;
		case KETelExtMultimodeV4:
			dataVersion = 4;
			break;
		case KEtelExtMultimodeV5:
		//case KETelMobilePhonebookStoreV5: - omitted because it is equal to KETelExtMultimodeV5
			dataVersion = 5;
			break;
		case KEtelExtMultimodeV6:
			dataVersion = 6;
			break;
		case KEtelExtMultimodeV7:
			dataVersion = 7;
			break;
		case KEtelExtMultimodeV8:
			dataVersion = 8;
			break;
		case KEtelExtMultimodeV9:
			dataVersion = 9;
			break;
		default:
			dataVersion = KSimTsyDefaultVersion; 
			break;
		}
		
	if(iSimTsyVersion < dataVersion)
		{
		ret = KErrNotSupported;
		}
		
	return ret;	
	}

TInt CSimPhone::CheckSimTsyVersion(TPacketBase& aDataStruct)
/**
* Checks the version of a data structure against the (simulated) version of SIMTSY, in order
* to determine if the version of the API used is compatible with the version of the TSY used.
*
* @param aDataStruct A versioned data-holding structure, derived from TPacketBase
*
* @return KErrNone if the version of aDataStruct is supported by the user-defined simulated TSY version. KErrNotSupported otherwise
*/
	{
	TInt ret = KErrNone;
	
	//retrieval of data version
	TInt dataVersion;
	switch (aDataStruct.ExtensionId())
		{
		case KETelExtPcktV1:
			dataVersion = 1;
			break;
		case KETelExtPcktV2:
			dataVersion = 2;
			break;
		case KETelExtPcktV3:
			dataVersion = 3;
			break;
		default:
			dataVersion = KSimTsyDefaultVersion; 
			break;
		}
		
	if(iSimTsyVersion < dataVersion)
		{
		ret = KErrNotSupported;
		}
		
	return ret;	
	}


TInt CSimPhone::CheckSimTsyVersion(RCall::TCallParams& aDataStruct)
/**
* Checks the version of a data structure against the (simulated) version of SIMTSY, in order
* to determine if the version of the API used is compatible with the version of the TSY used.
*
* @param aDataStruct A versioned data-holding structure, derived from TCallParams
*
* @return KErrNone if the version of aDataStruct is supported by the user-defined simulated TSY version. KErrNotSupported otherwise
*/
	{
	TInt ret = KErrNone;

	//retrieval of data version
	TInt dataVersion;
	switch (aDataStruct.ExtensionId())
		{
		case RMobileCall::KETelMobileCallParamsV1:
		case RMobileCall::KETelMobileDataCallParamsV1:
		case RMobileCall::KETelMobileHscsdCallParamsV1:
		case RMobileCall::KETel3rdPartyCallParamsV1:
			dataVersion = 1;
			break;
		case RMobileCall::KETelMobileCallParamsV2:
		case RMobileCall::KETelMobileDataCallParamsV2:
		case RMobileCall::KETelMobileHscsdCallParamsV2:
			dataVersion = 2;
			break;
		default:
			dataVersion = KSimTsyDefaultVersion; 
			break;
		}
		
	if(iSimTsyVersion < dataVersion)
		{
		ret = KErrNotSupported;
		}
		
	return ret;	
	}


TInt CSimPhone::CheckSimTsyVersion(RMobilePhone::CImsAuthorizationInfoV5& /*aDataStruct*/) // overload this for other types of structures
/**
* Checks the version of a data structure against the (simulated) version of SIMTSY, in order
* to determine if the version of the API used is compatible with the version of the TSY used.
*
* @param aDataStruct A versioned data-holding structure, of type RMobilePhone::CImsAuthorizationInfoV5 or a class derived from it.
*
* @return KErrNone if the version of aDataStruct is supported by the user-defined simulated TSY version. KErrNotSupported otherwise
*/
	{

	TInt ret = KErrNone;
	
	//retrieval of data version
	TInt dataVersion = 5; // (does not derive from the TMultimodeType data structure as usual...)
		
	if(iSimTsyVersion < dataVersion)
		{
		ret = KErrNotSupported;
		}
		
	return ret;	
	}
	

TInt CSimPhone::CheckSimTsyVersion(RPacketContext::CTFTMediaAuthorizationV3& /*aDataStruct*/) // overload this for other types of structures
/**
* Checks the version of a data structure against the (simulated) version of SIMTSY, in order
* to determine if the version of the API used is compatible with the version of the TSY used.
*
* @param aDataStruct A versioned data-holding structure, of type RPacketContext::CTFTMediaAuthorizationV3 or a class derived from it.
*
* @return KErrNone if the version of aDataStruct is supported by the user-defined simulated TSY version. KErrNotSupported otherwise
*/
	{

	TInt ret = KErrNone;
	
	//retrieval of data version
	TInt dataVersion = 3; // (does not derive from the TMultimodeType data structure as usual...)
		
	if(iSimTsyVersion < dataVersion)
		{
		ret = KErrNotSupported;
		}
		
	return ret;	
	}
////////////////////
// CSimTestNumberObserver
////////////////////

CSimPhone::CSimTestNumberObserver::CSimTestNumberObserver(CSimPhone& aSimPhone)
: CActive(CActive::EPriorityStandard),
  iSimPhone(aSimPhone)
    {
    CActiveScheduler::Add(this);
    }

CSimPhone::CSimTestNumberObserver::~CSimTestNumberObserver()
    {
    Cancel();
    iProperty.Close();
    }

CSimPhone::CSimTestNumberObserver* CSimPhone::CSimTestNumberObserver::NewL(CSimPhone& aSimPhone)
    {
    CSimPhone::CSimTestNumberObserver* self = new(ELeave)CSimPhone::CSimTestNumberObserver(aSimPhone);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

void CSimPhone::CSimTestNumberObserver::ConstructL()
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_CSIMTESTNUMBEROBSERVER_CONSTRUCTL_1, "CSimPhone::CSimPhoneObserver::ConstructL");
    User::LeaveIfError(iProperty.Attach(KUidPSSimTsyCategory, KPSSimTsyTestNumber));
    Start();
    }

void CSimPhone::CSimTestNumberObserver::Start()
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_CSIMTESTNUMBEROBSERVER_START_1, "CSimPhone::CSimPhoneObserver::Start");
    iProperty.Subscribe(iStatus);
    SetActive();
    }

void CSimPhone::CSimTestNumberObserver::RunL()
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_CSIMTESTNUMBEROBSERVER_RUNL_1, "CSimPhone::CSimPhoneObserver::RunL [iStatus=%d]", iStatus.Int());
    TInt err = iSimPhone.CheckConfigFile();
    if( err == KErrNone )
        {
        iSimPhone.HandleTestNumberChangedL();
        }

    Start();
    }

void CSimPhone::CSimTestNumberObserver::DoCancel()
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONE_CSIMTESTNUMBEROBSERVER_DOCANCEL_1, "CSimPhone::CSimPhoneObserver::DoCancel");
    iProperty.Cancel();
    }

