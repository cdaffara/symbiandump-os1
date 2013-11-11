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
// Implements the Phonebook Store manipulation code.
// 
//

/**
 @file
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSimPhBkUsimStoreTraces.h"
#endif

#include "CSimPhBkUsimStore.h"
#include "CSimPhone.h"
#include <testconfigfileparser.h>

const TUint16 KNpiTonInternational=145;		// < The Number Plan Identifier and Type of Number for an international telephone number.
const TUint16 KNpiTonNational=129;			// < The Number Plan Identifier and Type of Number for a national telephone number.
const TInt KMaxGroups = 10;					// < The maximum allowed number of groups associated with a phonebook entry
const TInt KMaxSecondNames = 1;				// < The maximum allowed number of Second Names associated with a phonebook entry
const TInt KPhonebookErrorGranularity=3;	// < Granularity of phonebook error list array.
const TUint DELETED=0xdededede; 
//
// CSimPhBkUSimStore
//
void CSimPhBkUSimStore::ClosePhone(TAny* aObj)
/**
 * A utility function for cleaning up the stack.
 */
	{
	((CObject*)aObj)->Close();
	}

CSimPhBkUSimStore* CSimPhBkUSimStore::NewL(CSimPhone* aPhone, const TDesC8& aStore, const TDesC8& aName, TInt aMaxNumSlots, TInt aMaxNumLen, TInt aMaxTextLen, TInt aAdditional, TInt aMaxEmail, TInt aMaxAdditionalTelNumLen, TInt aMaxAdditionalTextLen)
/**
 * Standard two phase constructor.
 * @param aPhone			The phone object from which this Phonebook was opened.
 * @param aName				The name of the created Phonebook.
 * @param aMaxNumSlots		The maximum number of slots in the Phonebook.
 * @param aMaxNumLen		The maximum length of the telephone number (<100)
 * @param aMaxTextLen		The maximum length of the alpha text strings (<100)
 * @param aAdditional		The maximum number of additional numbers associated with an entry
 * @param aMaxEmail			The maximum number of email addresses associated with an entry
 * @param aMaxAdditionalTelNumLen	The maximum length of the an additional telephone number.
 * @param aMaxAdditionalTextLen		The maximum length of the alpha tag in an additional telephone number.
 * @return CSimPhBkUSimStore*			The newly created object.
 */
	{
	CSimPhBkUSimStore* store=new(ELeave) CSimPhBkUSimStore(aPhone);
	TCleanupItem newObjClose(ClosePhone,store);
	CleanupStack::PushL(newObjClose);
	store->ConstructL(aStore,aName,aMaxNumSlots,aMaxNumLen,aMaxTextLen,aAdditional,aMaxEmail, aMaxAdditionalTelNumLen, aMaxAdditionalTextLen);	
	CleanupStack::Pop();
	return store;
	}

CSimPhBkUSimStore::CSimPhBkUSimStore(CSimPhone* aPhone)
		: CSimPhone(iDummyPhoneBaseRef), iPhone(aPhone),iIpcCnt(0),iEvOutstandingReq(EFalse)
/**
 * Trivial first phase constructor.
 * @param aPhone	The phone object from which this phonebook was opened.
 */
	{}

void CSimPhBkUSimStore::ConstructL(const TDesC8& aStore, const TDesC8& aName, TInt aMaxNumSlots, TInt aMaxNumLen, TInt aMaxTextLen, TInt aAdditional, TInt aMaxEmail, TInt aMaxAdditionalTelNumLen, TInt aMaxAdditionalTextLen)
/**
 * Second phase constructor that allocates memory for the phonebook, batch read buffer and
 * a delayed completion timer.  The constructor also reads the individual and batch read
 * delays from the configuration file.
 *
 * @param aName			The name of the created phonebook.
 * @param aMaxNumLen	The maximum length of a telephone number.
 * @param aMaxTextLen	The maximum length of an alpha tag.
 * @param aMaxNumLen	The maximum length of the telephone number (<100).
 * @param aMaxTextLen	The maximum length of the alpha text strings (<100).
 * @param aAdditional	The maximum number of additional numbers associated with an entry.
 * @param aMaxEmail		The maximum number of email addresses associated with an entry.
 * @param aMaxAdditionalTelNumLen	The maximum length of the an additional telephone number.
 * @param aMaxAdditionalTextLen		The maximum length of the alpha tag in an additional telephone number.
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_CONSTRUCTL_1, "Starting to parse Phonebook store additional config parameters...");
	__ASSERT_ALWAYS(aMaxNumLen<=KPhBkMaxTelNumSize,SimPanic(EPhonebookNameOrNumberTooLarge));
	__ASSERT_ALWAYS(aMaxTextLen<=KPhBkMaxAlphaTagSize,SimPanic(EPhonebookNameOrNumberTooLarge));

	iPhBkMaxNumSlots=aMaxNumSlots;
	iPhBkMaxTelNumLen=aMaxNumLen;
	iPhBkMaxTextLen=aMaxTextLen;
	iPhBkStoreName.Copy(aName);
	iPhBkStore.Copy(aStore);
	iPhBkAdditional=aAdditional;
	iPhBkMaxEmail=aMaxEmail;
	iPhBkMaxAdditionalTelNumLen = aMaxAdditionalTelNumLen;
	iPhBkMaxAdditionalTextLen = aMaxAdditionalTextLen;	
	
	iPhBkAid.Zero();
	iPhBkAidUSim1.Zero();
	iPhBkAidUSim2.Zero();

	if(iPhBkStore.Compare(KUSimPhoneBook)==KErrNone)
		{
		iPhBkUSim1StoreEntries=new(ELeave) TPhBkUSimStoreEntry[aMaxNumSlots+1]; //slot 0 is unused
		iPhBkUSim2StoreEntries=new(ELeave) TPhBkUSimStoreEntry[aMaxNumSlots+1]; //slot 0 is unused
		}
	else
		{
		iPhBkUSim1StoreEntries=new(ELeave) TPhBkUSimStoreEntry[aMaxNumSlots+1]; //slot 0 is unused
		iPhBkUSimStoreEntries=iPhBkUSim1StoreEntries;
		}

	iReqTimer=CSimTimer::NewL(iPhone);
	iOOBWriteTimer=CSimTimer::NewL(iPhone);
	iOOBDeleteTimer=CSimTimer::NewL(iPhone);
	iPhBkRwBuffer=new(ELeave) CPhoneBookBuffer();
	iPhBkError=new(ELeave) CArrayFixFlat<TPhBkError>(KPhonebookErrorGranularity);

	const CTestConfigItem* item = NULL;
	TInt ret = KErrNone;
	
	item=CfgFile()->Item(KTriggerEventIPC,0);
	if(item)
		{
		TInt ipc, cnt, event;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,ipc);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_CONSTRUCTL_2, "WARNING - CONFIGURATION FILE PARSING - Reading element IPC returned %d (element no. %d) from tag %s.",ret,0,KTriggerEventIPC);
			}
		else
			iTriggerEventIPC.iIPC=ipc;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,cnt);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_CONSTRUCTL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element CNT returned %d (element no. %d) from tag %s.",ret,1,KTriggerEventIPC);
			}
		else
			iTriggerEventIPC.iIPCCnt=cnt;
		
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,event);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_CONSTRUCTL_4, "WARNING - CONFIGURATION FILE PARSING - Reading element EVENT returned %d (element no. %d) from tag %s.",ret,2,KTriggerEventIPC);
			}
		else
			iTriggerEventIPC.iEvent=RMobilePhone::TMobilePhoneSecurityEvent(event);

		}

	const CTestConfigItem* item0=NULL;
	TInt count=CfgFile()->ItemCount(KPhBkPhoneUSimStoreCaps);
	
	if(count==0)
		iPhBkStoreCaps=KDefaultPhBkPhoneStoreCaps;
	else
		{
		TBool capsFound=EFalse;
		for(TInt i=0;(i<count || !capsFound);i++)
		{
			item0=CfgFile()->Item(KPhBkPhoneUSimStoreCaps,i);
			
			if (item0)
				{
				TPtrC8 value0,phonebookStore, phonebookName;
				TInt ret0;

				ret0=CTestConfig::GetElement(item0->Value(),KStdDelimiter,0,phonebookStore);
				if(ret0!=KErrNone)
					{
					iPhBkStoreCaps=KDefaultPhBkPhoneStoreCaps;
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_CONSTRUCTL_5, "WARNING - CONFIGURATION FILE PARSING - Reading element CAPS:PHONEBOOKSTORE returned %d (element no. %d) from tag %s.",ret,0,KPhBkPhoneUSimStoreCaps);
					}	

				if(phonebookStore.MatchF(iPhBkStore)!=0)// Not this PhBkStore type
					continue;

				ret0=CTestConfig::GetElement(item0->Value(),KStdDelimiter,1,phonebookName);
				if(ret0!=KErrNone)
					{
					iPhBkStoreCaps=KDefaultPhBkPhoneStoreCaps;
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_CONSTRUCTL_6, "WARNING - CONFIGURATION FILE PARSING - Reading element PHONEBOOKNAME returned %d (element no. %d) from tag %s.",ret,1,KPhBkPhoneUSimStoreCaps);
					}	
				
				if(phonebookName.MatchF(iPhBkStoreName)!=0)// Not this PhBkStoreName type
					continue;
				
				ret0=CTestConfig::GetElement(item0->Value(),KStdDelimiter,2,value0);
				if(ret0!=KErrNone)
					{
					iPhBkStoreCaps=KDefaultPhBkPhoneStoreCaps;
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_CONSTRUCTL_7, "WARNING - CONFIGURATION FILE PARSING - Reading element CAPS returned %d (element no. %d) from tag %s.",ret,2,KPhBkPhoneUSimStoreCaps);
					}	
				else
					{
					TUint32 intValue;
					TInt ret = AsciiToNum(value0, intValue);
					if(ret!=KErrNone)
						iPhBkStoreCaps=KDefaultPhBkPhoneStoreCaps;
					else
						{
						iPhBkStoreCaps = intValue;
						capsFound=ETrue;
						}
					}
				
				}
			}
	}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_CONSTRUCTL_8, "...Finished parsing Phonebook store additional config parameters...");
	}

void CSimPhBkUSimStore::PopulateStoreFromConfigFileL()
/**
 * Populate the Phonebook Store from information in the configuration file.  This is performed
 * after the standard Phonebook Store construction in order to prevent reseting the configuation
 * file accessor class' pointers while possibly multiple Phonebook Stores are created.
//alex
 *
 * The store entries comply to the following format:
 * "PhBkStoreEntry = <store name>, <slot number>, <telephone number>, <alphatag>"
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_POPULATESTOREFROMCONFIGFILEL_1, "Starting to read Phonebook store entries...");
	iPhBkIndividualPause=CfgFile()->ItemValue(KPhBkStoreIndividualReqPause,KDefaultPhBkStoreIndividualReqPause);
	iPhBkBatchPause=CfgFile()->ItemValue(KPhBkStoreBatchReqPause,KDefaultPhBkStoreBatchReqPause);

	const CTestConfigItem* item=NULL;
	TInt ret=KErrNone;

	TInt i;

	// Get Phonebook entries
	TInt count=CfgFile()->ItemCount(KPhBkUSimStoreEntry);
	for(i=0;i<count;i++)
		{
		TInt parseIndex=0;

		item=CfgFile()->Item(KPhBkUSimStoreEntry,i);
		if(!item)
			break;

		TPtrC8 phonebookStore,aid, phonebookName,alphaTag,telNum,alphaTag2,emails, groups, additional;
		TInt index;
		TUint8 npiTon;
		ret=GetPhBkUSimEntry(item,parseIndex,phonebookStore,phonebookName,aid,index,telNum,alphaTag,npiTon,alphaTag2);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_POPULATESTOREFROMCONFIGFILEL_2, "WARNING - CONFIGURATION FILE PARSING - Reading element PHONEBOOK ENTRY returned %d (element no. %d) from tag %s.",ret,index,KPhBkUSimStoreEntry);
			continue;
			}

		if(phonebookStore.MatchF(iPhBkStore)!=0)// Not this PhBkStore type
			continue;
		else if(phonebookName.MatchF(iPhBkStoreName)!=0)// Not this phonebook
			continue;

		if(phonebookStore.MatchF(KUSimPhoneBook)==0 &&
			iPhBkAid.Length()==0) //starting up
			{
			iPhBkAid.Copy(aid);
			iPhBkAidUSim1.Copy(aid);
			iPhBkUSimStoreEntries=iPhBkUSim1StoreEntries;
			}

		else if(phonebookStore.MatchF(KUSimPhoneBook)==0 &&
			iPhBkAid.Length()!=0 && iPhBkAid.Compare(aid)!=KErrNone)
			{
			if(iPhBkAidUSim2.Length()==0)
				{
				iPhBkAid.Copy(aid);
				iPhBkAidUSim2.Copy(aid);
				iPhBkUSimStoreEntries=iPhBkUSim2StoreEntries;
				}
			else if(iPhBkAidUSim2.Compare(aid)==KErrNone)
				{
				iPhBkAid.Copy(aid);
				iPhBkUSimStoreEntries=iPhBkUSim2StoreEntries;	
				}
			else if(iPhBkAidUSim2.Compare(aid)!=KErrNone)
				//Currently there is support for only 2 USim phonebooks
				{continue;} //...so, a possible third one will be ignored!!!
			}

		iPhBkUSimStoreEntries[index].iAlphaTag.Copy(alphaTag);
		iPhBkUSimStoreEntries[index].iTelNum.Copy(telNum);
		iPhBkUSimStoreEntries[index].iTonNpi=npiTon;
		iPhBkUSimStoreEntries[index].iAlphaTag2.Copy(alphaTag2);
	
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,parseIndex++,additional);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_POPULATESTOREFROMCONFIGFILEL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element ADDITIONAL returned %d (element no. %d) from tag %s.",ret,parseIndex,KPhBkUSimStoreEntry);
			}
		else
			{	
			TPtrC8 myName,myNum;
			TUint8 myTon;
			TInt myindex=0,pIndex=0;
			iPhBkUSimStoreEntries[index].iAdditional=new(ELeave) CArrayFixFlat<TPhBkStoreEntry>(iPhBkAdditional+1);
	
			while((GetPhBkAdditionalEntry(additional,pIndex,myNum,myName,myTon)==KErrNone)
					&& (myindex < iPhBkAdditional))
				{
				TPhBkStoreEntry entry;
			
				entry.iAlphaTag.Copy(myName);
				entry.iTelNum.Copy(myNum);
				entry.iTonNpi=myTon;
				TRAP_IGNORE(iPhBkUSimStoreEntries[index].iAdditional->AppendL(entry));

				myindex++;
				}

			}
	
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,parseIndex++,emails);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_POPULATESTOREFROMCONFIGFILEL_4, "WARNING - CONFIGURATION FILE PARSING - Reading element EMAILS returned %d (element no. %d) from tag %s.",ret,parseIndex,KPhBkUSimStoreEntry);
			}
		else
		{	
			TPtrC8 myemail;
			TInt myindex=0;
			iPhBkUSimStoreEntries[index].iEmails=new(ELeave) CArrayFixFlat<TPhBkUSimEmail>(iPhBkMaxEmail+1);
			
			while((CTestConfig::GetElement(emails,';',myindex,myemail)==KErrNone) &&
					(myindex < iPhBkMaxEmail))
			{
			TPhBkUSimEmail entry;
			entry.Copy(myemail);
			TRAP_IGNORE(iPhBkUSimStoreEntries[index].iEmails->AppendL(entry));
			myindex++;
			}
		}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,parseIndex++,groups);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_POPULATESTOREFROMCONFIGFILEL_5, "WARNING - CONFIGURATION FILE PARSING - Reading element GROUPS returned %d (element no. %d) from tag %s.",ret,parseIndex,KPhBkUSimStoreEntry);
			}
		else
		{	

			TPtrC8 mygroup;
			TInt myindex=0;
			iPhBkUSimStoreEntries[index].iGroups=new(ELeave) CArrayFixFlat<TPhBkUSimGroup>(KMaxGroups+1);

		
			while((CTestConfig::GetElement(groups,';',myindex,mygroup)==KErrNone) &&
					(myindex < KMaxGroups))
			{
			TPhBkUSimGroup entry;
			entry.Copy(mygroup);
			TRAP_IGNORE(iPhBkUSimStoreEntries[index].iGroups->AppendL(entry));
			myindex++;
			}
		}

		TBool hidden;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,parseIndex++,hidden);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_POPULATESTOREFROMCONFIGFILEL_6, "WARNING - CONFIGURATION FILE PARSING - Reading element EMAILS returned %d (element no. %d) from tag %s.",ret,parseIndex,KPhBkUSimStoreEntry);
			}
		else
			iPhBkUSimStoreEntries[index].iHiddenEntry=hidden;

		}

	count=CfgFile()->ItemCount(KPhBkError);
	item=NULL;

	for(i=0;i<count;i++)
		{
		item=CfgFile()->Item(KPhBkError,i);
		if(!item)
			break;

		TInt count,error;
		TPtrC8 phonebookName, phonebookStore;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,count);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_POPULATESTOREFROMCONFIGFILEL_7, "WARNING - CONFIGURATION FILE PARSING - Reading element COUNT returned %d (element no. %d) from tag %s.",ret,0,KPhBkError);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,error);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_POPULATESTOREFROMCONFIGFILEL_8, "WARNING - CONFIGURATION FILE PARSING - Reading element ERROR returned %d (element no. %d) from tag %s.",ret,1,KPhBkError);
			continue;
			}
		
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,phonebookName);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_POPULATESTOREFROMCONFIGFILEL_9, "WARNING - CONFIGURATION FILE PARSING - Reading element PHONEBOOKNAME returned %d (element no. %d) from tag %s.",ret,2,KPhBkError);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,phonebookStore);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_POPULATESTOREFROMCONFIGFILEL_10, "WARNING - CONFIGURATION FILE PARSING - Reading element PHONEBOOKSTORE returned %d (element no. %d) from tag %s.",ret,3,KPhBkError);
			//global phonebook error
			continue;
			}
		
		if(phonebookName.MatchF(iPhBkStoreName)!=0)// Not this phonebook
			continue;
		else if(phonebookStore.MatchF(iPhBkStore)!=0)// Not this phonebook
			continue;

		TPhBkError entry;
		entry.iCount=count;
		entry.iError=error;
		TRAP_IGNORE(iPhBkError->AppendL(entry));
		}
	PopulateOOBWrite();
	PopulateOOBDelete();
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_POPULATESTOREFROMCONFIGFILEL_11, "...Finished reading Phonebook store entries...");

	if(iPhBkOOBWriteDuration!=-1)
		iOOBWriteTimer->Start(iPhBkOOBWriteDuration,this,ETimerIdPhBkUSimStorOOBWrite);
	if(iPhBkOOBDeleteDuration!=-1)
		iOOBDeleteTimer->Start(iPhBkOOBDeleteDuration,this,ETimerIdPhBkUSimStorOOBDelete);
	}

void CSimPhBkUSimStore::PopulateOOBWrite()
/**
 * Populate the member variables required to operate the OOB Store functionality from
 * the configuration file.
 * The OOBPhBkStore configuration file tag should have the following format:
 * "OOBPhBkStore= <duration>, <phonebook name>, <index>, <telephone number>, <alpha tag>"
 */
	{
	TInt parseIndex=0;
	iPhBkOOBWriteDuration=KErrNotFound;
	const CTestConfigItem* item=CfgFile()->Item(KOOBPhBkUSimWrite);
	if(!item)
		return;

	TInt count;
	TInt ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,parseIndex++,count);
	if(ret!=KErrNone)
		{
		OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_POPULATEOOBWRITE_1, "WARNING - CONFIGURATION FILE PARSING - Reading element COUNT returned %d (element no. %d) from tag %s.",ret,parseIndex,KOOBPhBkUSimWrite);
		return;
		}

	TPtrC8 phonebookStore,aid, phonebookName,alphaTag,telNum,alphaTag2,emails, groups, additional;
	TInt index;
	TUint8 npiTon;
	ret=GetPhBkUSimEntry(item,parseIndex,phonebookStore,phonebookName,aid,index,telNum,alphaTag,npiTon,alphaTag2);
	if(ret!=KErrNone)
		{
		OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_POPULATEOOBWRITE_2, "WARNING - CONFIGURATION FILE PARSING - Reading element PHONEBOOK ENTRY returned %d (element no. %d) from tag %s.",ret,parseIndex,KOOBPhBkUSimWrite);
		return;
		}

	if(phonebookStore.MatchF(iPhBkStore)!=0)// Not this PhBkStore type
		return;
	else if(phonebookName.MatchF(iPhBkStoreName)!=0)// Not this phonebook
		return;

	iPhBkOOBWriteDuration=count;
	iPhBkOOBWriteIndex=index;
	iPhBkOOBWrite.iAlphaTag.Copy(alphaTag);
	iPhBkOOBWrite.iTelNum.Copy(telNum);
	iPhBkOOBWrite.iTonNpi=npiTon;
	iPhBkOOBWrite.iAlphaTag2.Copy(alphaTag2);

	ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,parseIndex++,additional);
	if(ret!=KErrNone)
		{
		OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_POPULATEOOBWRITE_3, "WARNING - CONFIGURATION FILE PARSING - Reading element ADDITIONAL returned %d (element no. %d) from tag %s.",ret,parseIndex,KOOBPhBkUSimWrite);
		}
	else
	{	
		TPtrC8 myName,myNum;
		TUint8 myTon;
		TInt myindex=0,pIndex=0;
		
		TRAPD(err,iPhBkOOBWrite.iAdditional=new(ELeave) CArrayFixFlat<TPhBkStoreEntry>(iPhBkAdditional+1));
		if (err!=KErrNone)
			return;
		while((GetPhBkAdditionalEntry(additional,pIndex,myNum,myName,myTon)==KErrNone)
				&& (myindex < iPhBkAdditional))
		{
		TPhBkStoreEntry entry; 

		entry.iAlphaTag.Copy(myName);
		entry.iTelNum.Copy(myNum);
		entry.iTonNpi=myTon;
		TRAP_IGNORE(iPhBkOOBWrite.iAdditional->AppendL(entry));
		myindex++;
		}
	}

	ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,parseIndex++,groups);
	if(ret!=KErrNone)
		{
		OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_POPULATEOOBWRITE_4, "WARNING - CONFIGURATION FILE PARSING - Reading element GROUPS returned %d (element no. %d) from tag %s.",ret,parseIndex,KOOBPhBkUSimWrite);
		}
	else
	{	
		TPtrC8 mygroup;
		TInt myindex=0;
		
		TRAPD(err,iPhBkOOBWrite.iGroups=new(ELeave) CArrayFixFlat<TPhBkUSimGroup>(KMaxGroups+1));
		if (err!=KErrNone)
			return;
		while((CTestConfig::GetElement(groups,';',myindex,mygroup)==KErrNone) &&
				(myindex < KMaxGroups))
		{
		TPhBkUSimGroup entry;
		entry.Copy(mygroup);
		TRAP_IGNORE(iPhBkOOBWrite.iGroups->AppendL(entry));
		myindex++;
		}
	}

	ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,parseIndex++,emails);
	if(ret!=KErrNone)
		{
		OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_POPULATEOOBWRITE_5, "WARNING - CONFIGURATION FILE PARSING - Reading element EMAILS returned %d (element no. %d) from tag %s.",ret,parseIndex,KOOBPhBkUSimWrite);
		}
	else
	{	
		TPtrC8 myemail;
		TInt myindex=0;
		
		TRAPD(err,iPhBkOOBWrite.iEmails=new(ELeave) CArrayFixFlat<TPhBkUSimEmail>(iPhBkMaxEmail+1));
		if (err!=KErrNone)
			return;
	
		while((CTestConfig::GetElement(emails,';',myindex,myemail)==KErrNone) &&
				(myindex < iPhBkMaxEmail))
		{
		TPhBkUSimEmail entry;
		entry.Copy(myemail);
		TRAP_IGNORE(iPhBkOOBWrite.iEmails->AppendL(entry));
		myindex++;
		}
	}

	}

void CSimPhBkUSimStore::PopulateOOBDelete()
/**
 * Populate the member variables required to operate the OOB Delete functionality from
 * the configuration file.
 * The OOBPhBkDelete configuration file tag should have the following format:
 * "OOBPhBkDelete= <duration>, <phonebook name>, <index>
 */
	{
	iPhBkOOBDeleteDuration=KErrNotFound;
	const CTestConfigItem* item=CfgFile()->Item(KOOBPhBkUSimDelete);
	if(!item)
		return;

	TInt count;
	TInt ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,count);
	if(ret!=KErrNone)
		{
		OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_POPULATEOOBDELETE_1, "WARNING - CONFIGURATION FILE PARSING - Reading element COUNT returned %d (element no. %d) from tag %s.",ret,0,KOOBPhBkUSimDelete);
		return;
		}

	TPtrC8 phonebookName, phonebookStore;
		
	ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,phonebookStore);
	if(ret!=KErrNone)
		{
		OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_POPULATEOOBDELETE_2, "WARNING - CONFIGURATION FILE PARSING - Reading element PHONEBOOKSTORE returned %d (element no. %d) from tag %s.",ret,1,KOOBPhBkUSimDelete);
		return;
		}

	ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,phonebookName);
	if(ret!=KErrNone)
		{
		OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_POPULATEOOBDELETE_3, "WARNING - CONFIGURATION FILE PARSING - Reading element PHONEBOOKNAME returned %d (element no. %d) from tag %s.",ret,2,KOOBPhBkUSimDelete);
		return;
		}

	if(phonebookStore.MatchF(iPhBkStore)!=0)
		return;						// Not this store
	else if(phonebookName.MatchF(iPhBkStoreName)!=0)// Not this phonebook
		return;

	TInt index;
	ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,index);
	if(ret!=KErrNone)
		{
		OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_POPULATEOOBDELETE_4, "WARNING - CONFIGURATION FILE PARSING - Reading element INDEX returned %d (element no. %d) from tag %s.",ret,3,KOOBPhBkUSimDelete);
		return;
		}

	iPhBkOOBDeleteDuration=count;
	iPhBkOOBDeleteIndex=index;
	}


TInt CSimPhBkUSimStore::GetPhBkUSimEntry(const CTestConfigItem* aItem, TInt& aItemIndex,
								 TPtrC8& aPhonebookStore, 
								 TPtrC8& aPhonebookName, 
								 TPtrC8& aAid,TInt& aIndex,
								 TPtrC8& aTelNum, TPtrC8& aAlphaTag, TUint8& aNpiTon,
								 TPtrC8& aAlphaTag2)
/**
 * Retrieve a phonebook entry from the configuration file, starting at a given item index
 * value.
 * @param aItem				Pointer to the config file item from which the phonebook entry will be read.
 * @param aItemIndex		The index number within the item from which the phonebook entry will be read.
 * @param aPhonebookName	The returned phonebook name
 * @param aIndex			The returned index number
 * @param aTelNum			The returned telephone number
 * @param aAlphaTag			The returned alpha tag
 * @param aNpiTon			The returned Number Plan Identifier and Type of Number information
 * @return TInt				Standard error value.
 */
	{
	TInt ret;
	
	ret=CTestConfig::GetElement(aItem->Value(),KStdDelimiter,aItemIndex++,aPhonebookStore);
	if(ret!=KErrNone)
		return ret;

	ret=CTestConfig::GetElement(aItem->Value(),KStdDelimiter,aItemIndex++,aPhonebookName);
	if(ret!=KErrNone)
		return ret;

	ret=CTestConfig::GetElement(aItem->Value(),KStdDelimiter,aItemIndex++,aAid);
	if(ret!=KErrNone)
		return ret;

	ret=CTestConfig::GetElement(aItem->Value(),KStdDelimiter,aItemIndex++,aIndex);
	if(ret!=KErrNone)
		return ret;

	if(aIndex>iPhBkMaxNumSlots) //the max number of slot is a valid slot
		return KErrArgument;

	ret=CTestConfig::GetElement(aItem->Value(),KStdDelimiter,aItemIndex++,aTelNum);
	if(ret!=KErrNone)
		return ret;
	if(aTelNum.Length()>iPhBkMaxTelNumLen)
		return EXTENDEDERROR(KErrArgument, KErrPhonebookNumberOverflow);

	ret=CTestConfig::GetElement(aItem->Value(),KStdDelimiter,aItemIndex++,aAlphaTag);
	if(ret!=KErrNone)
		return ret;
	if(aAlphaTag.Length()>iPhBkMaxTextLen)
		return EXTENDEDERROR(KErrArgument, KErrPhonebookTextOverflow);

	if((aTelNum.Length()>0) && (aTelNum[0]=='+'))
		{
		aTelNum.Set(aTelNum.Mid(1));
		aNpiTon=KNpiTonInternational;
		}
	else
		aNpiTon=KNpiTonNational;

	ret=CTestConfig::GetElement(aItem->Value(),KStdDelimiter,aItemIndex++,aAlphaTag2);
	if(ret!=KErrNone)
		return ret;
	if(aAlphaTag.Length()>iPhBkMaxTextLen)
		return EXTENDEDERROR(KErrArgument, KErrPhonebookTextOverflow);

	return ret;
	}

TInt CSimPhBkUSimStore::GetPhBkAdditionalEntry(TPtrC8& aEntry, TInt& aItemIndex,
								 TPtrC8& aTelNum, TPtrC8& aAlphaTag, TUint8& aNpiTon)
/**
 * Retrieve a phonebook entry from the configuration file, starting at a given item index
 * value.
 * @param aItem				Pointer to the config file item from which the phonebook entry will be read.
 * @param aItemIndex		The index number within the item from which the phonebook entry will be read.
 * @param aTelNum			The returned telephone number
 * @param aAlphaTag			The returned alpha tag
 * @param aNpiTon			The returned Number Plan Identifier and Type of Number information
 * @return TInt				Standard error value.
 */
	{
	TInt ret;

	ret=CTestConfig::GetElement(aEntry,';',aItemIndex++,aAlphaTag);
	if(ret!=KErrNone)
		return ret;
	
	if(aAlphaTag.Length()>iPhBkMaxAdditionalTextLen)
		return EXTENDEDERROR(KErrArgument, KErrPhonebookAdditionalNumberAlphaTextOverflow);

	ret=CTestConfig::GetElement(aEntry,';',aItemIndex++,aTelNum);
	if(ret!=KErrNone)
		return ret;
	if(aTelNum.Length()>iPhBkMaxAdditionalTelNumLen)
		return EXTENDEDERROR(KErrArgument, KErrPhonebookAdditionalNumberNumberOverflow);

		
	if((aTelNum.Length()>0) && (aTelNum[0]=='+'))
		{
		aTelNum.Set(aTelNum.Mid(1));
		aNpiTon=KNpiTonInternational;
		}
	else
		{
		aNpiTon=KNpiTonNational;
		}
		
	return ret;
	}

CSimPhBkUSimStore::~CSimPhBkUSimStore()
/**
 * Standard destructor.  Any objects created by the ::ConstructL() function
 * will be destroyed here.
 */
	{

	for(TInt i=1;i<=iPhBkMaxNumSlots;i++)
		{
			if(iPhBkUSim1StoreEntries[i].iTelNum.Length()>0 ||
				iPhBkUSim1StoreEntries[i].iAlphaTag.Length()>0)
			{
				iPhBkUSim1StoreEntries[i].iTelNum.Zero();
				if((iPhBkUSim1StoreEntries[i].iAdditional!=NULL)
					&& ((TUint)iPhBkUSim1StoreEntries[i].iAdditional!=DELETED))
				{
					if(iPhBkUSim1StoreEntries[i].iAdditional->Count()>=0)
					{
						iPhBkUSim1StoreEntries[i].iAdditional->Delete(0,iPhBkUSim1StoreEntries[i].iAdditional->Count());
						delete iPhBkUSim1StoreEntries[i].iAdditional;
					}
				}
				if((iPhBkUSim1StoreEntries[i].iGroups!=NULL)
					&& ((TUint)iPhBkUSim1StoreEntries[i].iGroups!=DELETED))
				{
					if(iPhBkUSim1StoreEntries[i].iGroups->Count()>=0)
					{
						iPhBkUSim1StoreEntries[i].iGroups->Delete(0,iPhBkUSim1StoreEntries[i].iGroups->Count());
						delete iPhBkUSim1StoreEntries[i].iGroups;
					}
				}
				if((iPhBkUSim1StoreEntries[i].iEmails!=NULL)	
					&& ((TUint)iPhBkUSim1StoreEntries[i].iEmails!=DELETED))
				{
					if(iPhBkUSim1StoreEntries[i].iEmails->Count()>=0)
					{
						iPhBkUSim1StoreEntries[i].iEmails->Delete(0,iPhBkUSim1StoreEntries[i].iEmails->Count());
						delete iPhBkUSim1StoreEntries[i].iEmails;
					}
				}
			}
		if(iPhBkUSim2StoreEntries!=NULL &&
			(iPhBkUSim2StoreEntries[i].iTelNum.Length()>0 ||
				iPhBkUSim2StoreEntries[i].iAlphaTag.Length()>0))
		{
			iPhBkUSim2StoreEntries[i].iTelNum.Zero();
			if((iPhBkUSim2StoreEntries[i].iAdditional!=NULL)	
				&& ((TUint)iPhBkUSim2StoreEntries[i].iAdditional!=DELETED))
			{
				if(iPhBkUSim2StoreEntries[i].iAdditional->Count()>=0)
				{
					iPhBkUSim2StoreEntries[i].iAdditional->Delete(0,iPhBkUSim2StoreEntries[i].iAdditional->Count());
					delete iPhBkUSim2StoreEntries[i].iAdditional;
				}
			}
			if((iPhBkUSim2StoreEntries[i].iGroups!=NULL)
				&& ((TUint)iPhBkUSim2StoreEntries[i].iGroups!=DELETED))
			{
				if(iPhBkUSim2StoreEntries[i].iGroups->Count()>=0)
				{
					iPhBkUSim2StoreEntries[i].iGroups->Delete(0,iPhBkUSim2StoreEntries[i].iGroups->Count());
					delete iPhBkUSim2StoreEntries[i].iGroups;
				}
			}
			if((iPhBkUSim2StoreEntries[i].iEmails!=NULL)
				&& ((TUint)iPhBkUSim2StoreEntries[i].iEmails!=DELETED))
			{
				if(iPhBkUSim2StoreEntries[i].iEmails->Count()>=0)
				{
					iPhBkUSim2StoreEntries[i].iEmails->Delete(0,iPhBkUSim2StoreEntries[i].iEmails->Count());
					delete iPhBkUSim2StoreEntries[i].iEmails;	
				}
			}
		}
	}	
	
	if((iPhBkOOBWrite.iAdditional!=NULL)
		&& ((TUint)iPhBkOOBWrite.iAdditional!=DELETED))
	{
		iPhBkOOBWrite.iAdditional->Delete(0,iPhBkOOBWrite.iAdditional->Count());
		delete iPhBkOOBWrite.iAdditional;
	}
	if((iPhBkOOBWrite.iGroups!=NULL)
		&& ((TUint)iPhBkOOBWrite.iGroups!=DELETED))
	{
		iPhBkOOBWrite.iGroups->Delete(0,iPhBkOOBWrite.iGroups->Count());
		delete iPhBkOOBWrite.iGroups;
	}
	if((iPhBkOOBWrite.iEmails!=NULL)
		&& ((TUint)iPhBkOOBWrite.iEmails!=DELETED))
	{
		iPhBkOOBWrite.iEmails->Delete(0,iPhBkOOBWrite.iEmails->Count());
		delete iPhBkOOBWrite.iEmails;
	}
	delete[] iPhBkUSim1StoreEntries;
	delete[] iPhBkUSim2StoreEntries;
	delete iPhBkError;
	delete iPhBkRwBuffer;
	delete iOOBWriteTimer;
	delete iOOBDeleteTimer;
	delete iReqTimer;

	}

TInt CSimPhBkUSimStore::ExtFunc(const TTsyReqHandle aReqHandle,const TInt aIpc, const TDataPackage& aPckg)
/**
 * Dispatch function for all Phonebook Store requests.
 * @param aReqHandle	The TSY request handle for this request.
 * @param aIpc			The IPC number of this request.
 * @param aPckg			The parameter package related to this request.
 * @return TInt			The return error condition.
 */
	{
	iIpcCnt++;
	TInt error=KErrNone;
	if(FindIpcErrorMatch(error))
		{
		ReqCompleted(aReqHandle,error);
		return KErrNone;
		}

// The following requests can be completed even if the completion of another request is pending.
	switch(aIpc)
		{
	case EMobilePhoneStoreGetInfo:
		error = GetInfo(aReqHandle,aPckg.Des1n());
		if(iTriggerEventIPC.iIPC==aIpc)
			{
			iTriggerCnt++;
			if(iTriggerEventIPC.iIPCCnt==iTriggerCnt)
				iPhone->SecurityEvent(iTriggerEventIPC.iEvent);
			}
		return error;

	case EMobilePhoneStoreNotifyStoreEvent:
		error = NotifyStoreEvent(aReqHandle,aPckg.Des1n(),aPckg.Des2n());
		if(iTriggerEventIPC.iIPC==aIpc)
			{
			iTriggerCnt++;
			if(iTriggerEventIPC.iIPCCnt==iTriggerCnt)
				iPhone->SecurityEvent(iTriggerEventIPC.iEvent);
			}
		return error;

	default:
		break;
		}


// The TSY can only process one of the following requests at a time.  If a second is received
// while processing the first, then it will be errored with KErrInUse.  This restriction will
// be removed later, by inserting a request queuing mechanism.  Note that the standard TSY
// "flow control" mechanism works phone-wide and so is not suitable.

	if(iReqTimer->IsActive())
		{
		ReqCompleted(aReqHandle,KErrInUse);
		return KErrNone;
		}

	switch(aIpc)
		{
// The standard multimode store read and write are not supported.
	case EMobilePhoneStoreRead:
	case EMobilePhoneStoreWrite:
	case EMobilePhoneStoreReadAllPhase1:
	case EMobilePhoneStoreReadAllPhase2:
		ReqCompleted(aReqHandle,KErrNotSupported);
		return KErrNone;

	case EMobilePhoneBookStoreRead:
		 error = Read(aReqHandle,aPckg.Des1n(),aPckg.Des2n());
		 if(iTriggerEventIPC.iIPC==aIpc)
			{
			iTriggerCnt++;
			if(iTriggerEventIPC.iIPCCnt==iTriggerCnt)
				iPhone->SecurityEvent(iTriggerEventIPC.iEvent);
			}
		 return error;
			
	case EMobilePhoneBookStoreWrite:
		error = Write(aReqHandle,aPckg.Des1n(),aPckg.Des2n());
		if(iTriggerEventIPC.iIPC==aIpc)
			{
			iTriggerCnt++;
			if(iTriggerEventIPC.iIPCCnt==iTriggerCnt)
				iPhone->SecurityEvent(iTriggerEventIPC.iEvent);
			}
		 return error;

	case EMobilePhoneStoreDelete:
		error =  Delete(aReqHandle,aPckg.Des1n());
		if(iTriggerEventIPC.iIPC==aIpc)
			{
			iTriggerCnt++;
			if(iTriggerEventIPC.iIPCCnt==iTriggerCnt)
				iPhone->SecurityEvent(iTriggerEventIPC.iEvent);
			}
		 return error;

	case EMobilePhoneStoreDeleteAll:
		error =  DeleteAll(aReqHandle);
		if(iTriggerEventIPC.iIPC==aIpc)
			{
			iTriggerCnt++;
			if(iTriggerEventIPC.iIPCCnt==iTriggerCnt)
				iPhone->SecurityEvent(iTriggerEventIPC.iEvent);
			}
		 return error;

	default:
		break;
		}

	return KErrNotSupported;
	}

CTelObject* CSimPhBkUSimStore::OpenNewObjectByNameL(const TDesC& /*aName*/)
/**
 * The API does not support any objects that could be opened from this one.
 */
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject* CSimPhBkUSimStore::OpenNewObjectL(TDes&)
/**
 * The API does not support any objects that could be opened from this one.
 */
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject::TReqMode CSimPhBkUSimStore::ReqModeL(const TInt aIpc)
/**
 * This function returns the Request Mode for the request with the passed IPC value.
 * @param aIpc		The IPC number of the request.
 * @return TReqMode	The request mode.
 */
	{
	CTelObject::TReqMode ret=0;	

	switch(aIpc)
		{
	case EMobilePhoneStoreGetInfo:
	case EMobilePhoneStoreDelete:
	case EMobilePhoneStoreDeleteAll:
	case EMobilePhoneBookStoreRead:
	case EMobilePhoneBookStoreWrite:
		break;

	case EMobilePhoneStoreNotifyStoreEvent:
		ret=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
		break;

	default:
		User::Leave(KErrNotSupported);
		break;
		}

	return ret;
	}

TInt CSimPhBkUSimStore::RegisterNotification(const TInt /*aIpc*/)
/**
 * The ETel Server calls this function when the first client makes a notification
 * request.  If supported by the underlying protocol controlling the
 * signalling stack, this can be used to start requesting updates for the relevant
 * service.
 */
	{
	return KErrNone;
	}

TInt CSimPhBkUSimStore::DeregisterNotification(const TInt /*aIpc*/)
/**
 * The ETel Server calls this function when the last client that had previously
 * made a notification request closes its ETel Server handle.  If supported by
 * the underlying protocol controlling the	signalling stack, this can be used
 * to stop requesting updates for the relevant service.
 */
	{
	return KErrNone;
	}

TInt CSimPhBkUSimStore::NumberOfSlotsL(const TInt /*aIpc*/)
/**
 * Return the number of slots that the ETel Server should allocate for buffering requests
 * of the given IPC number.
 */
	{
	return KDefaultNumberOfSlots;
	}

TInt CSimPhBkUSimStore::CancelService(const TInt aIpc,const TTsyReqHandle /*aTsyReqHandle*/)
/**
 * Cancel an outstanding request.
 * @param aIpc			The IPC number of the request that is to be cancelled.
 * @param aTsyReqHandle	The TSY request handle of the request that is to be cancelled.
 * @param TInt			Standard return value.
 */
	{
	switch(aIpc)
		{
	case EMobilePhoneStoreGetInfo:
	case EMobilePhoneStoreDelete:
	case EMobilePhoneStoreDeleteAll:
	case EMobilePhoneBookStoreRead:
	case EMobilePhoneBookStoreWrite:
		break;

	case EMobilePhoneStoreNotifyStoreEvent:
		NotifyStoreEventCancel();
		break;

	default:
		break;
		}
	return KErrNone;
	}

void CSimPhBkUSimStore::Init()
/**
 *	This function can be used to perform any necessary synchronous initialisation.
 */
	{
	}

TInt CSimPhBkUSimStore::GetInfo(TTsyReqHandle aReqHandle, TDes8* aPckg)
/**
 * Retrieve Phonebook Store information. This request is completed immediately, as it is assumed
 * that in a real TSY, all this data will be cached in the TSY.
 *
 * @param aReqHandle	The TSY request handle associated with this request.
 * @param aPckg			The parameter package associated with this request.
 */
	{
	if(iPhone->IsICCLocked()!=EFalse)
		{
		ReqCompleted(aReqHandle, KErrAccessDenied);
		return KErrNone;
		}

	if(CheckAndSwitchUSimApps()!=KErrNone)
		{
		ReqCompleted(aReqHandle, KErrNotFound);
		return KErrNone;
		}

	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg* getInfoPckg=(RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg*)aPckg;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1& getInfo=(*getInfoPckg)();

	PopulatePhBkStoreInfo(&getInfo);

	if(&getInfo!=NULL) // NULL will have been assigned by PopulatePhBkStoreInfo if the version checking failed.
		{
		ReqCompleted(aReqHandle,KErrNone);
		}
	else
		{
		ReqCompleted(aReqHandle,KErrNotSupported);
		}
	
	return KErrNone;
	}

void CSimPhBkUSimStore::PopulatePhBkStoreInfo(RMobilePhoneStore::TMobilePhoneStoreInfoV1* aStoreInfo)
/**
 * Populate the passed parameter with Phonebook Store information.
 * @param aStoreInfo	Pointer to phonebook store information structure to be populated.
 */
	{
	__ASSERT_ALWAYS(aStoreInfo, SimPanic(EIllegalNullPtrParameter));
	
	aStoreInfo->iType=RMobilePhoneStore::EPhoneBookStore;
	aStoreInfo->iTotalEntries=MaxSlots();
	aStoreInfo->iCaps=iPhBkStoreCaps;
	aStoreInfo->iName.Copy(iPhBkStoreName);
	aStoreInfo->iUsedEntries=UsedEntries();
	
	if(aStoreInfo->ExtensionId()==RMobilePhoneStore::KETelMobilePhonebookStoreV1)
		{
		RMobilePhoneBookStore::TMobilePhoneBookInfoV1* getExtraInfo=(RMobilePhoneBookStore::TMobilePhoneBookInfoV1*)aStoreInfo;

		// Check that the data structure is supported by the simulated TSY version
		TInt err = iPhone->CheckSimTsyVersion(*getExtraInfo);
		if(err != KErrNone)
			{
			getExtraInfo = NULL;
			return;
			}

		getExtraInfo->iMaxNumLength=iPhBkMaxTelNumLen;
		getExtraInfo->iMaxTextLength=iPhBkMaxTextLen;
		getExtraInfo->iLocation=RMobilePhoneBookStore::ELocationIccMemory;
		getExtraInfo->iChangeCounter=0;
		getExtraInfo->iIdentity.Copy(iPhone->GetImsi());
		}
	else if(aStoreInfo->ExtensionId()==RMobilePhoneStore::KETelMobilePhonebookStoreV2)
		{
		RMobilePhoneBookStore::TMobilePhoneBookInfoV2* getExtraInfo=(RMobilePhoneBookStore::TMobilePhoneBookInfoV2*)aStoreInfo;

		// Check that the data structure is supported by the simulated TSY version
		TInt err = iPhone->CheckSimTsyVersion(*getExtraInfo);
		if(err != KErrNone)
			{
			getExtraInfo = NULL;
			return;
			}

		getExtraInfo->iMaxNumLength=iPhBkMaxTelNumLen;
		getExtraInfo->iMaxTextLength=iPhBkMaxTextLen;
		getExtraInfo->iLocation=RMobilePhoneBookStore::ELocationIccMemory;
		getExtraInfo->iChangeCounter=0;
		getExtraInfo->iIdentity.Zero();		

		getExtraInfo->iPhBkMode.Copy(PhBkStore());		
	
		if(PhBkStore().Compare(KUSimPhoneBook)!=KErrNone)
			{
			getExtraInfo->iIdentity.Copy(iPhone->GetImsi());
			}
		else
			{
			TPtrC8 USimId(iPhone->GetActiveUSim().Right(1));
			getExtraInfo->iIdentity.Copy(USimId);
			getExtraInfo->iIdentity.Append(iPhone->GetImsi());
			}
		}
	else if(aStoreInfo->ExtensionId()==RMobilePhoneStore::KETelMobilePhonebookStoreV5)
		{
		RMobilePhoneBookStore::TMobilePhoneBookInfoV5* getExtraInfo=(RMobilePhoneBookStore::TMobilePhoneBookInfoV5*)aStoreInfo;

		// Check that the data structure is supported by the simulated TSY version
		TInt err = iPhone->CheckSimTsyVersion(*getExtraInfo);
		if(err != KErrNone)
			{
			getExtraInfo = NULL;
			return;
			}

		getExtraInfo->iMaxNumLength=iPhBkMaxTelNumLen;
		getExtraInfo->iMaxTextLength=iPhBkMaxTextLen;
		getExtraInfo->iLocation=RMobilePhoneBookStore::ELocationIccMemory;
		getExtraInfo->iChangeCounter=0;
		getExtraInfo->iIdentity.Zero();		

		getExtraInfo->iPhBkMode.Copy(PhBkStore());

		getExtraInfo->iMaxSecondNames = KMaxSecondNames;
		getExtraInfo->iMaxTextLengthSecondName = iPhBkMaxTextLen;
		getExtraInfo->iMaxAdditionalNumbers = iPhBkAdditional;
		getExtraInfo->iMaxTextLengthAdditionalNumber = iPhBkMaxAdditionalTextLen;
		getExtraInfo->iMaxNumLengthAdditionalNumber = iPhBkMaxAdditionalTelNumLen;
		getExtraInfo->iMaxGroupNames = KMaxGroups;
		getExtraInfo->iMaxTextLengthGroupName = iPhBkMaxTextLen;
		getExtraInfo->iMaxEmailAddr = iPhBkMaxEmail;
		getExtraInfo->iMaxTextLengthEmailAddr = iPhBkMaxTextLen;
	
		if(PhBkStore().Compare(KUSimPhoneBook)!=KErrNone)
			{
			getExtraInfo->iIdentity.Copy(iPhone->GetImsi());			
			}
		else
			{
			TPtrC8 USimId(iPhone->GetActiveUSim().Right(1));
			getExtraInfo->iIdentity.Copy(USimId);
			getExtraInfo->iIdentity.Append(iPhone->GetImsi());
			}
		}	
	}

TInt CSimPhBkUSimStore::Read(TTsyReqHandle aReqHandle, TDes8* aPckg1,TDes8* aPckg2)
/**
 * Read Phonebook store entries.  The completion of this request is delayed in order to
 * simulate a real TSY that would have to go and get the information from a SIM card.
 *
 * @param aReqHandle	The TSY request handle associated with this request.
 * @param aPckg1		The first parameter package associated with this request.
 *						It contains the index from which the read should start and
 *						the number of entries which should be read.
 * @param aPckg2		The second parameter package associated with this request.
 *						It contains the buffer, which will be populated with the retrieved
 *						values.
 * @return TInt			Standard return value.
 */
	{
	if(CheckAndSwitchUSimApps()!=KErrNone)
		{
		ReqCompleted(aReqHandle, KErrNotFound);
		return KErrNone;
		}

	if(iPhone->IsICCLocked()!=EFalse)
		{
		ReqCompleted(aReqHandle, KErrAccessDenied);
		return KErrNone;
		}

	TPckg<RMobilePhoneBookStore::TPBIndexAndNumEntries>* indexNumPckg=(TPckg<RMobilePhoneBookStore::TPBIndexAndNumEntries>*)aPckg1;
	RMobilePhoneBookStore::TPBIndexAndNumEntries& indexNum=(*indexNumPckg)();

	if(indexNum.iNumSlots==iPhBkMaxNumSlots)
		{
		if (!(iPhBkStoreCaps & static_cast<TUint32>(RMobilePhoneStore::KCapsWholeStore) && iPhBkStoreCaps & RMobilePhoneStore::KCapsReadAccess))
			{
			ReqCompleted(aReqHandle, KErrAccessDenied);
			return KErrNone;
			}
		}
	else if((indexNum.iIndex+indexNum.iNumSlots-1)>iPhBkMaxNumSlots)
			{
			ReqCompleted(aReqHandle,KErrArgument);
			return KErrNone;
			}
	else if (!(iPhBkStoreCaps & RMobilePhoneStore::KCapsIndividualEntry && iPhBkStoreCaps & RMobilePhoneStore::KCapsReadAccess))
			{
			ReqCompleted(aReqHandle, KErrAccessDenied);
			return KErrNone;
			}

	iPhBkRwBuffer->Set(aPckg2);

	TInt cnt=0;
	TInt ret=0;
	TBool partial=EFalse;
	TBool onlyHidden=EFalse;

	for(TInt i=indexNum.iIndex;i<=iPhBkMaxNumSlots;i++)
		{
		if((iPhBkUSimStoreEntries[i].iTelNum.Length()!=0)||(iPhBkUSimStoreEntries[i].iAlphaTag.Length()!=0))
			{				
			ret=iPhBkRwBuffer->AddNewEntryTag();
			if(ret!=KErrNone)
				break;
			
			if((iPhone->IsHiddenEnabled()!=EFalse) &&
				iPhBkUSimStoreEntries[i].iHiddenEntry==1)
				{
				//This is a hidden entry, the key is required and is currently locked
				//Therefore, the client should be notified that the slot has been occupied by a hidden entry

				partial=ETrue;
				ret=iPhBkRwBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBAdnIndex,(TUint16)i);
				if(ret!=KErrNone)
					break;
				ret=iPhBkRwBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBHiddenInfo,(TUint8) iPhBkUSimStoreEntries[i].iHiddenEntry);
				if(ret!=KErrNone)
					break;
				partial=EFalse;
				onlyHidden=ETrue;
				break;
				}

			partial=ETrue;
			ret=iPhBkRwBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBAdnIndex,(TUint16)i);
			if(ret!=KErrNone)
				break;
			ret=iPhBkRwBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBText,iPhBkUSimStoreEntries[i].iAlphaTag);
			if(ret!=KErrNone)
				break;
			ret=iPhBkRwBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBTonNpi,iPhBkUSimStoreEntries[i].iTonNpi);
			if(ret!=KErrNone)
				break;
			ret=iPhBkRwBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBNumber,iPhBkUSimStoreEntries[i].iTelNum);
			if(ret!=KErrNone)
				break;
	
			ret=iPhBkRwBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBSecondName,iPhBkUSimStoreEntries[i].iAlphaTag2);
			if(ret!=KErrNone)
				break;

			TInt EmailCount = iPhBkUSimStoreEntries[i].iEmails->Count();
			TInt GroupCount = iPhBkUSimStoreEntries[i].iGroups->Count();
			TInt AnrCount = iPhBkUSimStoreEntries[i].iAdditional->Count();
			TInt j;

			TBool corrupt=EFalse;

			for(j=0; j<AnrCount; j++)
				{
				TPhBkStoreEntry entry = iPhBkUSimStoreEntries[i].iAdditional->At(j);
				
				corrupt=ETrue;

				ret=iPhBkRwBuffer->AddNewNumberTag();
				if(ret!=KErrNone)
					break;
				
				ret=iPhBkRwBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBText,entry.iAlphaTag);
				if(ret!=KErrNone)
					break;
						
				ret=iPhBkRwBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBNumber,entry.iTelNum);
				if(ret!=KErrNone)
					break;

				ret=iPhBkRwBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBTonNpi,entry.iTonNpi);
				if(ret!=KErrNone)
					break;

				corrupt=EFalse;
				}			
			if(corrupt)
				break;

			for(j=0; j<EmailCount; j++)
				{
				TPhBkUSimEmail entry = iPhBkUSimStoreEntries[i].iEmails->At(j);
				
				corrupt=ETrue;
				ret=iPhBkRwBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBEmailAddress,entry);
				if(ret!=KErrNone)
					break;
				corrupt=EFalse;
				}			
			if(corrupt)
				break;
			
			for(j=0; j<GroupCount; j++)
				{
				TPhBkUSimGroup entry = iPhBkUSimStoreEntries[i].iGroups->At(j);
				
				corrupt=ETrue;
				ret=iPhBkRwBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBGroupName,entry);
				if(ret!=KErrNone)
					break;
				corrupt=EFalse;
				}			
			if(corrupt)
				break;

			ret=iPhBkRwBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBHiddenInfo,(TUint8) iPhBkUSimStoreEntries[i].iHiddenEntry);
			if(ret!=KErrNone)
				break;

			partial=EFalse;
			onlyHidden=ETrue;
			if(++cnt>=indexNum.iNumSlots)
				break;
			}
		}
	if(partial)
		// EXPORT - but return value not relevant
		(void)iPhBkRwBuffer->RemovePartialEntry();

	indexNum.iNumSlots=cnt;
	
	if((cnt==0)&&(partial))
		ReqCompleted(aReqHandle,KErrArgument);		// An entry was found, but the buffer was too small to return it.
	else if((cnt==0)&&(!partial)&&(!onlyHidden))
		ReqCompleted(aReqHandle,KErrNotFound);		// No entries found.
	else if((cnt==0)&&(!partial)&&(onlyHidden))
		ReqCompleted(aReqHandle,KErrAccessDenied);
	else
		ReqCompleted(aReqHandle,KErrNone);

	return KErrNone;
	}

TInt CSimPhBkUSimStore::Write(TTsyReqHandle aReqHandle,TDes8* aPckg1,TDes8* aPckg2)
/**
 * Write a phonebook store entries.  The completion of this request is delayed in order to
 * simulate a real TSY that would have to write the information to a SIM card. A store
 * event may be triggered by this request.
 *
 * @param aReqHandle	The TSY request handle associated with this request.
 * @param aPckg1		The first parameter package associated with this request.
 *						This contains the TLV phonebook entry to be written.
 * @param aPckg2		The second parameter package associated with this request.
 *						This contains the slot index number to which the entry must be written.
 */
	{
	if(CheckAndSwitchUSimApps()!=KErrNone)
		{
		ReqCompleted(aReqHandle, KErrNotFound);
		return KErrNone;
		}

	if(iPhone->IsICCLocked()!=EFalse)
		{
		ReqCompleted(aReqHandle, KErrAccessDenied);
		return KErrNone;
		}
	
	if (!(iPhBkStoreCaps & RMobilePhoneStore::KCapsIndividualEntry && iPhBkStoreCaps & RMobilePhoneStore::KCapsWriteAccess))
		{
		ReqCompleted(aReqHandle, KErrAccessDenied);
		return KErrNone;
		}

	TPckg<TInt>* indexPckg=(TPckg<TInt>*)aPckg2;
	TInt& index=(*indexPckg)();

// If the index is -1, then use the first available free slot.
	if(index==-1)
		{
		for(TInt i=1;i<=iPhBkMaxNumSlots;i++)
			{
			if((iPhBkUSimStoreEntries[i].iTelNum.Length()==0)&&(iPhBkUSimStoreEntries[i].iAlphaTag.Length()==0))
				{
				index=i;
				break;
				}
			}
		}

	if(index==-1)
		{
		ReqCompleted(aReqHandle, EXTENDEDERROR(KErrNoMemory, KErrPhonebookNoMemory));
		return KErrNone;
		}

	if((index<1)||(index>iPhBkMaxNumSlots))
		{
		ReqCompleted(aReqHandle,KErrArgument);
		return KErrNone;
		}

	TBool isSlotAlreadyUsed=EFalse;
	if(iPhBkUSimStoreEntries[index].iTelNum.Length()!=0)
		{
		if((iPhone->IsHiddenEnabled()!=EFalse) &&
			iPhBkUSimStoreEntries[index].iHiddenEntry==1)
			{
			ReqCompleted(aReqHandle, KErrAccessDenied);
			return KErrNone;
			}

		isSlotAlreadyUsed=ETrue;
		TInt idx,count;
		
		count=iPhBkUSimStoreEntries[index].iAdditional->Count();
		for(idx=0;idx<count;idx++)
			{
			TPhBkStoreEntry entry=
				iPhBkUSimStoreEntries[index].iAdditional->At(idx);
			entry.iTelNum.Zero();
			entry.iAlphaTag.Zero();	
			entry.iTonNpi=0;
			}
		iPhBkUSimStoreEntries[index].iAdditional->Delete(0,count);
		delete iPhBkUSimStoreEntries[index].iAdditional;
		count=iPhBkUSimStoreEntries[index].iGroups->Count();
		iPhBkUSimStoreEntries[index].iGroups->Delete(0,count);
		delete iPhBkUSimStoreEntries[index].iGroups;
		count=iPhBkUSimStoreEntries[index].iEmails->Count();
		iPhBkUSimStoreEntries[index].iEmails->Delete(0,count);
		delete iPhBkUSimStoreEntries[index].iEmails;

		TRAPD(err,iPhBkUSimStoreEntries[index].iAdditional=
				new(ELeave) CArrayFixFlat<TPhBkStoreEntry>(iPhBkAdditional+1));
		if (err!=KErrNone)
			return (err);

		TRAP(err,iPhBkUSimStoreEntries[index].iGroups=
				new(ELeave) CArrayFixFlat<TPhBkUSimGroup>(KMaxGroups));
		if (err!=KErrNone)
			return (err);

		TRAP(err,iPhBkUSimStoreEntries[index].iEmails=
				new(ELeave) CArrayFixFlat<TPhBkUSimEmail>(iPhBkMaxEmail+1));
		if (err!=KErrNone)
			return (err);
	}
	else 
		{
		TRAPD(err,iPhBkUSimStoreEntries[index].iAdditional=
				new(ELeave) CArrayFixFlat<TPhBkStoreEntry>(iPhBkAdditional+1));
		if (err!=KErrNone)
			return (err);

		TRAP(err,iPhBkUSimStoreEntries[index].iGroups=
				new(ELeave) CArrayFixFlat<TPhBkUSimGroup>(KMaxGroups));
		if (err!=KErrNone)
			return (err);

		TRAP(err,iPhBkUSimStoreEntries[index].iEmails=
				new(ELeave) CArrayFixFlat<TPhBkUSimEmail>(iPhBkMaxEmail+1));
		if (err!=KErrNone)
			return (err);
	}
// Unpick the phonebook entry.
	iPhBkRwBuffer->Set(aPckg1);
	iPhBkRwBuffer->StartRead();

	TUint8 tagVal;
	TUint8 npiTon=0;
	TUint8 hidden;
	TPtrC alphaTag,alphaTag2;
	TPtrC telNum;
	TPtrC bufPtr;
	TBool additional=EFalse;
	TInt additionalCount=0;
	
	CPhoneBookBuffer::TPhBkTagType tagType;
	TInt ret=KErrNone;

	TPhBkStoreEntry additionalEntry;

	while(ret==KErrNone)
		{
		ret=iPhBkRwBuffer->GetTagAndType(tagVal,tagType);
		if(ret==KErrNotFound)
			{
			ret=KErrNone;
			break;
			}
		else if(ret!=KErrNone)
			break;
		switch(tagVal)
			{
		case RMobilePhoneBookStore::ETagPBNewEntry:
			break;

		case RMobilePhoneBookStore::ETagPBText:
			ret=iPhBkRwBuffer->GetValue(alphaTag);

			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_WRITE_1, "alphaTag (ETagPBText) Length = (%d)",alphaTag.Length());
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_WRITE_2, "Phonebook Max Text Length = (%d)",iPhBkMaxTextLen);

			if(!additional && ret==KErrNone && (alphaTag.Length()<=iPhBkMaxTextLen))
				iPhBkUSimStoreEntries[index].iAlphaTag.Copy(alphaTag);
			else if(additional && ret==KErrNone && (alphaTag.Length()<=iPhBkMaxAdditionalTextLen)
					&& (additionalCount<iPhBkAdditional))
				additionalEntry.iAlphaTag.Copy(alphaTag);
			else if(additional)
				ret=EXTENDEDERROR(KErrArgument, KErrPhonebookAdditionalNumberAlphaTextOverflow);
			else
				ret=EXTENDEDERROR(KErrOverflow, KErrPhonebookTextOverflow);
			break;

		case RMobilePhoneBookStore::ETagPBSecondName:
			ret=iPhBkRwBuffer->GetValue(alphaTag2);

			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_WRITE_3, "alphaTag2 (ETagPBSecondName) Length = (%d)",alphaTag2.Length());
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_WRITE_4, "Phonebook Max Text Length = (%d)",iPhBkMaxTextLen);

			if(ret==KErrNone && (alphaTag2.Length()<=iPhBkMaxTextLen))
				iPhBkUSimStoreEntries[index].iAlphaTag2.Copy(alphaTag2);
			else
				ret=EXTENDEDERROR(KErrOverflow, KErrPhonebookSecondNameTextOverflow);
			break;

		case RMobilePhoneBookStore::ETagPBNumber:
			ret=iPhBkRwBuffer->GetValue(telNum);

			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_WRITE_5, "telNum (ETagPBNumber) Length = (%d)",telNum.Length());
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_WRITE_6, "Phonebook Max Text Length = (%d)",iPhBkMaxTelNumLen);

			if(!additional && ret==KErrNone && (telNum.Length()<=iPhBkMaxTelNumLen))
				iPhBkUSimStoreEntries[index].iTelNum.Copy(telNum);
			else if(additional && ret==KErrNone && (telNum.Length()<=iPhBkMaxAdditionalTelNumLen)
					&& (additionalCount<iPhBkAdditional))
				additionalEntry.iTelNum.Copy(telNum);
			else if(additional)
				ret=EXTENDEDERROR(KErrArgument, KErrPhonebookAdditionalNumberNumberOverflow);
			else 
				ret=EXTENDEDERROR(KErrOverflow, KErrPhonebookNumberOverflow);
			break;

		case RMobilePhoneBookStore::ETagPBTonNpi:
			ret=iPhBkRwBuffer->GetValue(npiTon);
			if(!additional && ret==KErrNone)
				iPhBkUSimStoreEntries[index].iTonNpi=npiTon;
			else if (additional && ret==KErrNone)
				{
				additionalEntry.iTonNpi=npiTon;
				TRAP_IGNORE(iPhBkUSimStoreEntries[index].iAdditional->AppendL(additionalEntry));
				additional=EFalse;
				additionalCount++;
				additionalEntry.iAlphaTag.Zero();
				additionalEntry.iTelNum.Zero();
				additionalEntry.iTonNpi=0;
				}
			break;

		case RMobilePhoneBookStore::ETagPBAnrStart:
			if(!additional)
				additional=ETrue;
			else
				ret=KErrArgument;
			break;

		case RMobilePhoneBookStore::ETagPBGroupName:
			ret=iPhBkRwBuffer->GetValue(bufPtr);

			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_WRITE_7, "bufPtr (ETagPBGroupName) Length = (%d)",bufPtr.Length());
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_WRITE_8, "Phonebook Max Text Length = (%d)",iPhBkMaxTextLen);

			if(ret==KErrNone && (bufPtr.Length()<=iPhBkMaxTextLen)
				&& (iPhBkUSimStoreEntries[index].iGroups->Count() < KMaxGroups))
			{
				TRAP_IGNORE(iPhBkUSimStoreEntries[index].iGroups->AppendL(bufPtr));
			}
			else 
				ret=EXTENDEDERROR(KErrOverflow, KErrPhonebookGroupAlphaTextOverflow);
			break;

		case RMobilePhoneBookStore::ETagPBEmailAddress:
			ret=iPhBkRwBuffer->GetValue(bufPtr);

			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_WRITE_9, "bufPtr (ETagPBEmailAddress) Length = (%d)",bufPtr.Length());
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_WRITE_10, "Phonebook Max Text Length = (%d)",iPhBkMaxTextLen);

			if(ret==KErrNone && (bufPtr.Length()<=iPhBkMaxTextLen)
				&& (iPhBkUSimStoreEntries[index].iEmails->Count() < iPhBkMaxEmail))
			{
				TRAP_IGNORE(iPhBkUSimStoreEntries[index].iEmails->AppendL(bufPtr));
			}
			else
				ret=EXTENDEDERROR(KErrOverflow, KErrPhonebookEmailTextOverflow);		
			break;

		case RMobilePhoneBookStore::ETagPBHiddenInfo:
			ret=iPhBkRwBuffer->GetValue(hidden);	
			if(ret==KErrNone)
				iPhBkUSimStoreEntries[index].iHiddenEntry=hidden;
			break;

		default:
			ret=KErrNotSupported;
			}
		}

	if(ret!=KErrNone)
		{
		//Cleanup!
		iPhBkUSimStoreEntries[index].iTelNum.Zero();
		iPhBkUSimStoreEntries[index].iAlphaTag.Zero();
		iPhBkUSimStoreEntries[index].iAlphaTag2.Zero();
		iPhBkUSimStoreEntries[index].iHiddenEntry=0;
		iPhBkUSimStoreEntries[index].iTonNpi=npiTon;

		if(iPhBkUSimStoreEntries[index].iGroups!=NULL) 
		{
			iPhBkUSimStoreEntries[index].iGroups->Delete(0,iPhBkUSimStoreEntries[index].iGroups->Count());
			delete iPhBkUSimStoreEntries[index].iGroups;
		}
		if(iPhBkUSimStoreEntries[index].iEmails!=NULL)
		{
			iPhBkUSimStoreEntries[index].iEmails->Delete(0,iPhBkUSimStoreEntries[index].iEmails->Count());
			delete iPhBkUSimStoreEntries[index].iEmails;
		}
		if(iPhBkUSimStoreEntries[index].iAdditional!=NULL)
		{
			iPhBkUSimStoreEntries[index].iAdditional->Delete(0,iPhBkUSimStoreEntries[index].iAdditional->Count());
			delete iPhBkUSimStoreEntries[index].iAdditional;
		}
		ReqCompleted(aReqHandle,ret);
		return KErrNone;
		}

//	if(alphaTag.Length()>iPhBkMaxTextLen)
//		{
//		ReqCompleted(aReqHandle, EXTENDEDERROR(KErrOverflow, KErrPhonebookTextOverflow));
//		return KErrNone;
//		}
//	else if(telNum.Length()>iPhBkMaxTelNumLen)
//		{
//		ReqCompleted(aReqHandle, EXTENDEDERROR(KErrOverflow, KErrPhonebookNumberOverflow));
//		return KErrNone;
//		}
	
	if(isSlotAlreadyUsed)
		DelayCompletion(iPhBkIndividualPause,aReqHandle,EStoreEventChanged,index);
	else
		DelayCompletion(iPhBkIndividualPause,aReqHandle,EStoreEventAdded,index);
	return KErrNone;
	}

TInt CSimPhBkUSimStore::Delete(TTsyReqHandle aReqHandle,TDes8* aPckg)
/**
 * Delete a single Phonebook store entry.  The completion of this request is delayed in order to
 * simulate a real TSY that would have to write the information to a SIM card. A store
 * event may be triggered by this request.
 *
 * @param aReqHandle	The TSY request handle associated with this request.
 * @param aPckg			The parameter package associated with this request.
 * @return				Standard return value.
 */
	{
	if(CheckAndSwitchUSimApps()!=KErrNone)
		{
		ReqCompleted(aReqHandle, KErrNotFound);
		return KErrNone;
		}

	if(iPhone->IsICCLocked()!=EFalse)
		{
		ReqCompleted(aReqHandle, KErrAccessDenied);
		return KErrNone;
		}
	
	if (!(iPhBkStoreCaps & RMobilePhoneStore::KCapsIndividualEntry && iPhBkStoreCaps & RMobilePhoneStore::KCapsWriteAccess))
		{
		ReqCompleted(aReqHandle, KErrAccessDenied);
		return KErrNone;
		}


	TPckg<TInt>* intPckg=(TPckg<TInt>*)aPckg;
	TInt& index=(*intPckg)();

	if((index<1)||(index>iPhBkMaxNumSlots))
		{
		ReqCompleted(aReqHandle,KErrArgument);
		return KErrNone;
		}
	
	if((iPhone->IsHiddenEnabled()!=EFalse) &&
		iPhBkUSimStoreEntries[index].iHiddenEntry==1)
		{
		ReqCompleted(aReqHandle, KErrAccessDenied);
		return KErrNone;
		}
	
	iPhBkUSimStoreEntries[index].iTelNum.Zero();
	iPhBkUSimStoreEntries[index].iAlphaTag.Zero();
	iPhBkUSimStoreEntries[index].iAlphaTag2.Zero();
	
	if((iPhBkUSimStoreEntries[index].iGroups!=NULL) &&
		((TUint)iPhBkUSimStoreEntries[index].iGroups!=DELETED))
	{
		iPhBkUSimStoreEntries[index].iGroups->Delete(0,
			iPhBkUSimStoreEntries[index].iGroups->Count());
		delete iPhBkUSimStoreEntries[index].iGroups;
	}
	if((iPhBkUSimStoreEntries[index].iEmails!=NULL) &&
		((TUint)iPhBkUSimStoreEntries[index].iEmails!=DELETED))
	{
		iPhBkUSimStoreEntries[index].iEmails->Delete(0,
			iPhBkUSimStoreEntries[index].iEmails->Count());
		delete iPhBkUSimStoreEntries[index].iEmails;
	}

	if((iPhBkUSimStoreEntries[index].iAdditional!=NULL) &&
		((TUint)iPhBkUSimStoreEntries[index].iAdditional!=DELETED))
		{
		TInt count = iPhBkUSimStoreEntries[index].iAdditional->Count();
		for(TInt i=0;i<count;i++)
			{
			TPhBkStoreEntry entry=iPhBkUSimStoreEntries[index].iAdditional->At(i);
			entry.iTelNum.Zero();
			entry.iAlphaTag.Zero();
			entry.iTonNpi=0;
			}

		iPhBkUSimStoreEntries[index].iAdditional->Delete(0,count);
		delete iPhBkUSimStoreEntries[index].iAdditional;
		}

	DelayCompletion(iPhBkIndividualPause,aReqHandle,EStoreEventDeleted,index);
	return KErrNone;
	}

TInt CSimPhBkUSimStore::DeleteAll(TTsyReqHandle aReqHandle)
/**
 * Delete all entries in the Phonebook Store.  The completion of this function is delayed in
 * order to simulate the SIM operations a real TSY would have to carry out.  This function
 * may trigger an Phonebook Store notification.
 *
 * @param aReqHandle	The TSY request handle associated with this request.
 * @return				Standard return value.
 */
	{
	if(CheckAndSwitchUSimApps()!=KErrNone)
		{
		ReqCompleted(aReqHandle, KErrNotFound);
		return KErrNone;
		}

	if(iPhone->IsICCLocked()!=EFalse)
		{
		ReqCompleted(aReqHandle, KErrAccessDenied);
		return KErrNone;
		}

	if (!(iPhBkStoreCaps & static_cast<TUint32>(RMobilePhoneStore::KCapsWholeStore) && iPhBkStoreCaps & RMobilePhoneStore::KCapsWriteAccess))
		{
		ReqCompleted(aReqHandle, KErrAccessDenied);
		return KErrNone;
		}

	for(TInt i=1;i<=iPhBkMaxNumSlots;i++)
		{
		TBool entryUsed =EFalse;

		if((iPhone->IsHiddenEnabled()!=EFalse) &&
		iPhBkUSimStoreEntries[i].iHiddenEntry==1)
			{
			//Skip me because I am a hidden entry!
			//The hidden key has to be verified to delete me!
			continue;
			}
		if(iPhBkUSimStoreEntries[i].iTelNum.Length()>0)
			entryUsed = ETrue;
			
		iPhBkUSimStoreEntries[i].iTelNum.Zero();
		iPhBkUSimStoreEntries[i].iAlphaTag.Zero();
		iPhBkUSimStoreEntries[i].iAlphaTag2.Zero();
		
		if(entryUsed)
			{
			if((iPhBkUSimStoreEntries[i].iGroups!=NULL) &&
				((TUint)iPhBkUSimStoreEntries[i].iGroups!=DELETED))
			{
				iPhBkUSimStoreEntries[i].iGroups->Delete(0,
					iPhBkUSimStoreEntries[i].iGroups->Count());
				delete iPhBkUSimStoreEntries[i].iGroups;
			}
			if((iPhBkUSimStoreEntries[i].iEmails!=NULL) &&
				((TUint)iPhBkUSimStoreEntries[i].iEmails!=DELETED))
			{
				iPhBkUSimStoreEntries[i].iEmails->Delete(0,
					iPhBkUSimStoreEntries[i].iEmails->Count());
				delete iPhBkUSimStoreEntries[i].iEmails;
			}
			if((iPhBkUSimStoreEntries[i].iAdditional!=NULL) &&
				((TUint)iPhBkUSimStoreEntries[i].iAdditional!=DELETED))
				{
				TInt count = iPhBkUSimStoreEntries[i].iAdditional->Count();
	            for(TInt index=0;index<count;index++)
					{
					TPhBkStoreEntry entry=iPhBkUSimStoreEntries[i].iAdditional->At(index);
					entry.iTelNum.Zero();
					entry.iAlphaTag.Zero();
					entry.iTonNpi=0;
					}

				iPhBkUSimStoreEntries[i].iAdditional->Delete(0,count);
				delete iPhBkUSimStoreEntries[i].iAdditional;
				}
			}
	}
	DelayCompletion(iPhBkBatchPause,aReqHandle,EStoreEventDeleted,-1);
	return KErrNone;
	}

TInt CSimPhBkUSimStore::NotifyStoreEvent(TTsyReqHandle aReqHandle,TDes8* aPckg1,TDes8* aPckg2)
/**
 * Register a client's interest in Phonebook Store events.
 *
 * @param aReqHandle	The TSY request handle associated with this request.
 * @param aPckg1		The first parameter package associated with this request.
 *						It contains the event flags that will be returned to the client.
 * @param aPckg2		The second parameter package associated with this request.
 *						It contains the index value associated with the event
 *						that will be returned to the client.
 * @return				Standard return value.
 */
	{
	TPckg<TUint32>* eventPckg=(TPckg<TUint32>*)aPckg1;
	TUint32& event=(*eventPckg)();
	TPckg<TInt>* indexPckg=(TPckg<TInt>*)aPckg2;
	TInt& index=(*indexPckg)();

	iEvOutstandingReq=ETrue;
	iEvReqHandle=aReqHandle;
	iEvEvent=&event;
	iEvIndex=&index;
	return KErrNone;
	}

void CSimPhBkUSimStore::NotifyStoreEventCancel()
/**
 * Cancel an outstanding notify store request.
 */
	{
	if(iEvOutstandingReq)
		{
		iEvOutstandingReq=EFalse;
		ReqCompleted(iEvReqHandle,KErrCancel);
		}
	}

TPtrC8 CSimPhBkUSimStore::Name()
/**
 * Accessor function fot the Phonebook Store name.
 *
 * @return TPtrC8	The name of this Phonebook Store.
 */
	{
	return iPhBkStoreName;
	}

TPtrC8 CSimPhBkUSimStore::PhBkStore()
/**
 * Accessor function fot the Phonebook Store name.
 *
 * @return TPtrC8	The name of this Phonebook Store.
 */
	{
	return iPhBkStore;
	}

TPtrC8 CSimPhBkUSimStore::AID()
/**
 * Accessor function fot the Phonebook Store name.
 *
 * @return TPtrC8	The name of this Phonebook Store.
 */
	{
	return iPhBkAid;
	}

TInt CSimPhBkUSimStore::UsedEntries()
/**
 * Count the number of used entries in the Phonebook Store.
 * @return TInt	The number of used entries in the store.
 */
	{
	TInt cnt=0;
	if(CheckAndSwitchUSimApps()!=KErrNone)
  		return KErrNotFound;
	for(TInt i=1;i<=iPhBkMaxNumSlots;i++)
		{
		if((iPhBkUSimStoreEntries[i].iTelNum.Length()!=0)||(iPhBkUSimStoreEntries[i].iAlphaTag.Length()!=0))
			cnt++;
		}
	return cnt;
	}

TInt CSimPhBkUSimStore::MaxSlots()
/**
 * Retrieve the maximum number of slots in this Phonebook Store.
 * @return TInt	The maximum number of slots in this Phonebook Store.
 */
	{
	return iPhBkMaxNumSlots;
	}

void CSimPhBkUSimStore::DelayCompletion(TInt aDelayDuration,TTsyReqHandle aReqHandle)
/**
 * A shell function for functions that wish to delay completion but do not trigger store
 * events.
 */
	{
	DelayCompletion(aDelayDuration,aReqHandle,EStoreEventNoEvent,0);
	}

void CSimPhBkUSimStore::DelayCompletion(TInt aDelayDuration,TTsyReqHandle aReqHandle,TStoreEvent aEvent,TInt aIndex)
/**
 * Delay the completion of a TSY request.  It is assumed that the member variable
 * manipulation associated with the request has already taken place, and so all that is
 * left to do is call the ETel server's request completion function when the timer expires.
 * So, just record the parameters and kick off the timer.
 *
 * @param aDelayDuration	The time (in seconds) for which the request completion is to be delayed.
 * @param aReqHandle		The TSY request handle related to the delayed completion.
 * @param aEvent			The store event related to the delayed completion.
 * @param aIndex			The index related to the event passed in aEvent.
 */
	{
	iPendingReqCompletion=aReqHandle;
	iPendingEvent=aEvent;
	iPendingIndex=aIndex;
	iReqTimer->Start(aDelayDuration,this,ETimerIdPhBkUSimStorReq);
	}

void CSimPhBkUSimStore::TimerCallBack(TInt aId)
/**
 * Process a timer call back event.  There are three timers associated with this class
 * and this callback will be used for all of them.  The timers can be identified from the
 * aId parameter passed with the callback.
 *
 * The "Request" timer is used to kick requests which have had their completions delayed.
 * The "Out of Band Write" timer is used to schedule a non-client phonebook write.
 * The "Out of Band Delete" timer is used to schedule a non-client phonebook delete.
 *
 * @param aId	The Id of the timer to which this callback relates.
 */
	{
	TInt idx=0,count;
	TBool isSlotAlreadyUsed=EFalse;

	switch(aId)
		{
	case ETimerIdPhBkUSimStorReq:
		StoreEvent(iPendingEvent,iPendingIndex);
		ReqCompleted(iPendingReqCompletion,KErrNone);
		break;

	case ETimerIdPhBkUSimStorOOBWrite:

		if(CheckAndSwitchUSimApps()!=KErrNone)
			if(iEvOutstandingReq)
				{
				ReqCompleted(iEvReqHandle,KErrNotFound);
				return;
				}
			else
				{
				return;
				}

		if(iPhBkUSimStoreEntries[iPhBkOOBWriteIndex].iTelNum.Length()!=0)
			isSlotAlreadyUsed=ETrue;
		
		iPhBkUSimStoreEntries[iPhBkOOBWriteIndex].iAlphaTag.Copy(iPhBkOOBWrite.iAlphaTag);
		iPhBkUSimStoreEntries[iPhBkOOBWriteIndex].iTelNum.Copy(iPhBkOOBWrite.iTelNum);
		iPhBkUSimStoreEntries[iPhBkOOBWriteIndex].iTonNpi=iPhBkOOBWrite.iTonNpi;
		iPhBkUSimStoreEntries[iPhBkOOBWriteIndex].iAlphaTag2.Copy(iPhBkOOBWrite.iAlphaTag2);

		if(isSlotAlreadyUsed)
			{
			if((count=iPhBkUSimStoreEntries[iPhBkOOBWriteIndex].iAdditional->Count())>0)
				{
				for(idx=0;idx<count;idx++)
						{
						TPhBkStoreEntry entry=
							iPhBkUSimStoreEntries[iPhBkOOBWriteIndex].iAdditional->At(idx);
						entry.iTelNum.Zero();
						entry.iAlphaTag.Zero();	
						entry.iTonNpi=0;
						}
				iPhBkUSimStoreEntries[iPhBkOOBWriteIndex].iAdditional->Delete(0,count);
				delete iPhBkUSimStoreEntries[iPhBkOOBWriteIndex].iAdditional;
				}
			
			if((count = iPhBkUSimStoreEntries[iPhBkOOBWriteIndex].iGroups->Count())>0)
				{
					iPhBkUSimStoreEntries[iPhBkOOBWriteIndex].iGroups->Delete(0,count);
					delete iPhBkUSimStoreEntries[iPhBkOOBWriteIndex].iGroups;
				}
			if((count = iPhBkUSimStoreEntries[iPhBkOOBWriteIndex].iEmails->Count())>0)
				{
					iPhBkUSimStoreEntries[iPhBkOOBWriteIndex].iEmails->Delete(0,count);
					delete iPhBkUSimStoreEntries[iPhBkOOBWriteIndex].iEmails;
				}
			}
	
		count = iPhBkOOBWrite.iAdditional->Count();

		//sometimes because of a kernel bug on wins, iAdditional pointer gets filled up with
		//bad address 0xdededede (investigating)
	
		TRAPD(err,iPhBkUSimStoreEntries[iPhBkOOBWriteIndex].iAdditional=new(ELeave) CArrayFixFlat<TPhBkStoreEntry>(iPhBkAdditional+1));
		if (err!=KErrNone)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_TIMERCALLBACK_1, "CSimPhBkUSimStore::TimerCallBack - Memory Allocation Failure");
			return;
			}

		for(idx=0;idx<count;idx++)
			{
			TPhBkStoreEntry entry=iPhBkOOBWrite.iAdditional->At(idx);
			TRAP_IGNORE(iPhBkUSimStoreEntries[iPhBkOOBWriteIndex].iAdditional->AppendL(entry));
			}

		
		TRAP(err,iPhBkUSimStoreEntries[iPhBkOOBWriteIndex].iGroups=new(ELeave) CArrayFixFlat<TPhBkUSimGroup>(KMaxGroups));
		if (err!=KErrNone)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_TIMERCALLBACK_2, "CSimPhBkUSimStore::TimerCallBack - Memory Allocation Failure");
			return;
			}

		for(idx=0;idx<count;idx++)
			{
			TPhBkUSimGroup entry;
			TRAP_IGNORE(iPhBkUSimStoreEntries[iPhBkOOBWriteIndex].iGroups->AppendL(entry));
			}
		
		count = iPhBkOOBWrite.iEmails->Count();
		TRAP(err,iPhBkUSimStoreEntries[iPhBkOOBWriteIndex].iEmails=new(ELeave) CArrayFixFlat<TPhBkUSimEmail>(iPhBkMaxEmail+1));	
		if (err!=KErrNone)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_TIMERCALLBACK_3, "CSimPhBkUSimStore::TimerCallBack - Memory Allocation Failure");
			return;
			}

		for(idx=0;idx<count;idx++)
			{
			TPhBkUSimEmail entry;
			TRAP_IGNORE(iPhBkUSimStoreEntries[iPhBkOOBWriteIndex].iEmails->AppendL(entry));
			}
		
		iPhBkUSimStoreEntries[iPhBkOOBWriteIndex].iHiddenEntry=iPhBkOOBWrite.iHiddenEntry;
	
		StoreEvent(EStoreEventAdded,iPhBkOOBWriteIndex);
		break;

	case ETimerIdPhBkUSimStorOOBDelete:
		
		if(CheckAndSwitchUSimApps()!=KErrNone)
			if(iEvOutstandingReq)
				{
				ReqCompleted(iEvReqHandle,KErrNotFound);
				return;
				}
			else
				{
				return;
				}

		iPhBkUSimStoreEntries[iPhBkOOBDeleteIndex].iAlphaTag.Zero();
		iPhBkUSimStoreEntries[iPhBkOOBDeleteIndex].iTelNum.Zero();
		iPhBkUSimStoreEntries[iPhBkOOBDeleteIndex].iTonNpi=0;
		iPhBkUSimStoreEntries[iPhBkOOBDeleteIndex].iAlphaTag2.Zero();

		iPhBkUSimStoreEntries[iPhBkOOBDeleteIndex].iGroups->Delete(0,
			iPhBkUSimStoreEntries[iPhBkOOBDeleteIndex].iGroups->Count());
		delete iPhBkUSimStoreEntries[iPhBkOOBDeleteIndex].iGroups;

		iPhBkUSimStoreEntries[iPhBkOOBDeleteIndex].iEmails->Delete(0,
			iPhBkUSimStoreEntries[iPhBkOOBDeleteIndex].iEmails->Count());
		delete iPhBkUSimStoreEntries[iPhBkOOBDeleteIndex].iEmails;

		count = iPhBkUSimStoreEntries[iPhBkOOBDeleteIndex].iAdditional->Count();
		for(idx=0;idx<count;idx++)
			{
			TPhBkStoreEntry entry=iPhBkUSimStoreEntries[iPhBkOOBDeleteIndex].iAdditional->At(idx);
			entry.iTelNum.Zero();
			entry.iAlphaTag.Zero();	
			entry.iTonNpi=0;
			}

		iPhBkUSimStoreEntries[iPhBkOOBDeleteIndex].iAdditional->Delete(0,count);
		delete iPhBkUSimStoreEntries[iPhBkOOBDeleteIndex].iAdditional;
		iPhBkUSimStoreEntries[iPhBkOOBDeleteIndex].iHiddenEntry=0;

		StoreEvent(EStoreEventDeleted,iPhBkOOBDeleteIndex);
		break;

	default:
		break;
		}
	}

void CSimPhBkUSimStore::StoreEvent(TStoreEvent aEvent,TInt aIndex)
/**
 * Determine if a store event notification should be completed.
 * @param aEvent	The store event.
 * @param aIndex	The index related to the store event.
 */
	{
	if(iEvOutstandingReq)
		{
		TUint event=0;
		switch(aEvent)
			{
		case EStoreEventNoEvent:
			return;

		case EStoreEventAdded:
			event|=RMobilePhoneStore::KStoreEntryAdded;
			break;

		case EStoreEventDeleted:
			event|=RMobilePhoneStore::KStoreEntryDeleted;
			break;

		case EStoreEventChanged:
			event|=RMobilePhoneStore::KStoreEntryChanged;
			break;

		default:
			break;
			}

		TInt cnt=UsedEntries();
		if(cnt==0)
			event|=RMobilePhoneStore::KStoreEmpty;

		if(cnt==iPhBkMaxNumSlots)
			event|=RMobilePhoneStore::KStoreFull;
		else
			event|=RMobilePhoneStore::KStoreHasSpace;

		*iEvEvent=event;
		*iEvIndex=aIndex;
		iEvOutstandingReq=EFalse;
		ReqCompleted(iEvReqHandle,KErrNone);
		}
	}

TBool CSimPhBkUSimStore::FindIpcErrorMatch(TInt& aError)
/**
 * Determine whether the IPC counter has signalled that the current request should
 * be errored, rather than executed.
 *
 * @param aError	If the function returns ETrue, this parameter will pass back the
 *					number of the error to be propagated.
 * @return TBool	Returns ETrue if a match with the IPC count is found, EFalse if not.
 */
	{
	TInt i;
	for(i=0;i<iPhBkError->Count();i++)
		{
		if(iPhBkError->At(i).iCount==iIpcCnt)
			{
			aError=iPhBkError->At(i).iError;
			return ETrue;
			}
		}
	return EFalse;
	}


/**
 *  Check that the USIM App phonebook in use is the same as the current
 *  USIM App, and if not then swap the phonebook over.
 */
TInt CSimPhBkUSimStore::CheckAndSwitchUSimApps()
	{	
	TInt ret=KErrNone;

	if(iPhBkStore.Compare(KUSimPhoneBook)==KErrNone)
		{
		if(iPhone->GetActiveUSim().Compare(iPhBkAid)==KErrNone) 
			{
			//
			// The active USIM App is the same as last time, but ensure that
			// we are using the right phonebook anyway.
			//
			if(iPhBkAid.Compare(iPhBkAidUSim2)==KErrNone)
				{
				iPhBkUSimStoreEntries=iPhBkUSim2StoreEntries;
				}
			else if(iPhBkAid.Compare(iPhBkAidUSim1)==KErrNone)
				{
				iPhBkUSimStoreEntries=iPhBkUSim1StoreEntries;
				}
			else 
				{
				//messed up
				return KErrUnknown;
				}
			}
		else if((iPhone->GetActiveUSim().Compare(iPhBkAid)!=KErrNone) &&
			(iPhone->GetActiveUSim().Compare(iPhBkAidUSim2)==KErrNone)) 
			{
			//
			// The active USIM App is not the same as last time and it appears
			// to be USIM App 2, so use USIM phonebook number 2.
			//
			iPhBkAid.Copy(iPhBkAidUSim2);
			iPhBkUSimStoreEntries=iPhBkUSim2StoreEntries;
			}
		else if((iPhone->GetActiveUSim().Compare(iPhBkAid)!=KErrNone) &&
			(iPhone->GetActiveUSim().Compare(iPhBkAidUSim1)==KErrNone)) 
			{
			//
			// The active USIM App is not the same as last time and it appears
			// to be USIM App 1, so use USIM phonebook number 1.
			//
			iPhBkAid.Copy(iPhBkAidUSim1);
			iPhBkUSimStoreEntries=iPhBkUSim1StoreEntries;
			}
		else
			{
			//Unknown active application!!
			ret=KErrNotFound;
			}
		}	
	return ret;
}

const CTestConfigSection* CSimPhBkUSimStore::CfgFile()
/**
* Returns a pointer to the config file section
*
* @return CTestConfigSection a pointer to the configuration file data section
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKUSIMSTORE_CFGFILE_1, ">>CSimPhBkUSimStore::CfgFile");
	return iPhone->CfgFile();
	}
