// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Defines the CSimSmartCardEap class componenet
// 
//

/**
 @file
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "csimsmartcardeapTraces.h"
#endif

#include <testconfigfileparser.h>
#include "csimsmartcardeap.h"
#include "etelext.h"

// CSimSmartCardEapManager implementation; related to CSimSmartCardEap //

/**
Default constructor to initialise Ptrs.
*/
TEapChngResp::TEapChngResp()
: iChallenge(NULL), iResp(NULL),
  iAuthStatus(RMobileSmartCardEap::ENoAuthStarted)
	{
	}

/**
Default constructor to initialise Ptrs.
*/
TEapProcedureData::TEapProcedureData()
: iEapKey(NULL), iEapExtKey(NULL), iEapId(NULL), iEapPsId(NULL)
	{
	}

/**
Factory constructor.
*/
CSimSmartCardEapManager* CSimSmartCardEapManager::NewL(CSimPhone *aPhone)
	{
	CSimSmartCardEapManager* phone = new(ELeave) CSimSmartCardEapManager(aPhone);
	CleanupStack::PushL(phone);
	phone->ConstructL();
	CleanupStack::Pop();
	return phone;
	}

/**
Default constructor, initialises a pointer to the owner phone object.
*/
CSimSmartCardEapManager::CSimSmartCardEapManager(CSimPhone *aPhone)
: iPhone(aPhone)
	{
	}

/**
Second-phase constructor.
*/
void CSimSmartCardEapManager::ConstructL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_CONSTRUCTL_1, "CSimSmartCardEapManager second phase construction created");

	ParseEapInfoL();

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_CONSTRUCTL_2, "CSimSmartCardEapManager second phase construction completed");
	}

/**
If this is destroyed then so should all EAP sub-session objects.
*/
CSimSmartCardEapManager::~CSimSmartCardEapManager()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_DTOR_1, "CSimSmartCardEapManager destructing");

	for (TInt jj = iSubSessionObjs.Count()-1; jj >= 0; jj--)
		{
		delete iSubSessionObjs[jj];
		}

	iSubSessionObjs.Close();

	ClearParsedData();

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_DTOR_2, "CSimSmartCardEapManager destructed");
	}

void CSimSmartCardEapManager::ClearParsedData()
	{
	// Cleanup remaining unused parsed EAP procedure data
	for (TInt jjj = 0; jjj < iEapProcData.Count(); jjj++)
		{
		for (TInt aa = 0; aa < iEapProcData[jjj].iChResp.Count(); aa++)
			{
			TEapChngResp& temp = iEapProcData[jjj].iChResp[aa];
			delete (temp.iChallenge);
			delete (temp.iResp);
			temp.iChallenge = NULL;
			temp.iResp = NULL;
			}
		iEapProcData[jjj].iChResp.Close();

		delete iEapProcData[jjj].iEapKey;
		delete iEapProcData[jjj].iEapExtKey;
		delete iEapProcData[jjj].iEapId;
		delete iEapProcData[jjj].iEapPsId;
		iEapProcData[jjj].iEapKey = NULL;
		iEapProcData[jjj].iEapExtKey = NULL;
		iEapProcData[jjj].iEapId = NULL;
		iEapProcData[jjj].iEapPsId = NULL;
		}

	iEapProcData.Close();
	iDiscardedProcedure.Close();
	}

/**
Returns a pointer to the selected config.txt file section.
*/
const CTestConfigSection* CSimSmartCardEapManager::CfgFile()
	{
	return iPhone->CfgFile();
	}

/**
Parses the UICC App EAP specific settings from the config.txt.
*/
void CSimSmartCardEapManager::ParseEapInfoL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_1, "CSimSmartCardEapManager::ParseEapInfoL called");

	CTestConfigItem* item = NULL;

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_2, "Starting to Parse Smart Card EAP Info");
	TInt count = CfgFile()->ItemCount(KScEapProcedures);

	// Used in parsing to keep track of the nested items
	TInt nestedKeyTag = 0;
	TInt nestedExtKeyTag = 0;
	TInt nestedIdTag = 0;
	TInt nestedPsIdTag = 0;
	TInt nestedChlTag = 0;

	// Counts of the total number of tags (considered as nested tags)
	TInt countKey = CfgFile()->ItemCount(KScEapKeyMSK);
	TInt countExtKey = CfgFile()->ItemCount(KScEapKeyEMSK);
	TInt countId = CfgFile()->ItemCount(KScEapIdentity);
	TInt countPsId = CfgFile()->ItemCount(KScEapPsIdentity);
	TInt countChl = CfgFile()->ItemCount(KScEapChallenge);


	for (TInt index = 0; index < count; index++)
		{
		item = const_cast<CTestConfigItem*>(CfgFile()->Item(KScEapProcedures, index));
		if(item == NULL)
			{
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_3, "WARNING CONFIGURATION FILE PARSING: SC EAP PROC INFO tag not read [%d]", index);
			continue;
			}

		TInt dataFrmt = 0;
		TInt numChallenges = 0;
		TInt ret = KErrNone;
		TPtrC8 appId, eapType;

		// To be populated and appended to iEapProcData later
		TEapProcedureData procInfo;

		// Get AID; convert to bin if required
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 0, appId);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_4, "WARNING - CONFIGURATION FILE PARSING - Reading element APPID returned %d (element no. %d) from tag %s.", ret,0,KScEapProcedures);
			continue;
			}
		else
			{
			procInfo.iAID = appId;

			// AID is always in binary format (because of the RMobileSmartCardEap::Open construction)
			ParseMixedBinaryAsciiDataL(procInfo.iAID);
			}

		// Get eap type
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 1, eapType);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_5, "WARNING - CONFIGURATION FILE PARSING - Reading element EAPTYPE returned %d (element no. %d) from tag %s.", ret,1,KScEapProcedures);
			continue;
			}
		else
			{
			// EAP type is always in ASCII
			procInfo.iEapType = eapType;
			}

		// Find number of challenges
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 2, numChallenges);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_6, "WARNING - CONFIGURATION FILE PARSING - Reading element NUMCHALLENGES returned %d (element no. %d) from tag %s.", ret,2,KScEapProcedures);
			continue;
			}
		else if (numChallenges > (countChl - nestedChlTag))
			{
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_7, "ERROR CONFIGURATION FILE PARSING: error SC EAP PROC INFO specifies more challenges than available [%d]", index);
			continue;
			}
		else if (numChallenges < 0)
			{
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_8, "ERROR CONFIGURATION FILE PARSING: error SC EAP PROC INFO specifies -ve challenge number [%d]", index);
			continue;
			}

		// Get optional data format; this format is used for all data of this parsed procedure
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 3, dataFrmt);
		if(ret != KErrNone)
			{
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_9, "CONFIGURATION FILE PARSING: SC EAP PROC INFO tag with no data format [%d]", index);
			}
		else if (dataFrmt >= EMaxConfigDataFormat)
			{
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_10, "WARNING IN CONFIGURATION FILE PARSING - error wrong data format value SC EAP PROC INFO tag [%d] (ASCII format will be used)", index);
			dataFrmt = EConfigDataFormatAscii;
			}

		TPtrC8 ptr;
		TPtr8 tempPtr(NULL,0);
		HBufC8* startData = NULL;

		// Get MSK
		if (nestedKeyTag < countKey)
			{
			item = const_cast<CTestConfigItem*>(CfgFile()->Item(KScEapKeyMSK, nestedKeyTag++));

			// parse delay and key
			if (item == NULL)
				{
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_11, "WARNING CONFIGURATION FILE PARSING: could not read EAP Key tag [%d]", nestedKeyTag-1);
				}
			else
				{
				ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 0, ptr);
				if (ret != KErrNone)
					{
					OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_12, "WARNING CONFIGURATION FILE PARSING: could not read EAP Key tag's data [%d]", nestedKeyTag-1);
					}
				else
					{
					TRAPD(kAllocErr, startData = ptr.AllocL());
					if (kAllocErr != KErrNone)
						{
						OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_13, "WARNING CONFIGURATION FILE PARSING: could not allocate mem for EAP Key data [%d]", nestedKeyTag-1);
						}
					else
						{
						tempPtr.Set(startData->Des());
						switch (dataFrmt)
							{
						case EConfigDataFormatMixedBinaryAndAscii:
							ParseMixedBinaryAsciiDataL(tempPtr);
							break;
						//case EConfigDataFormatAscii: // Do nothing
						//default:
							}
						// need to re-copy because converting to binary changes size
						TRAP(kAllocErr, procInfo.iEapKey = tempPtr.AllocL());
						if (kAllocErr != KErrNone)
							{
							OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_14, "WARNING CONFIGURATION FILE PARSING: could not allocate mem for EAP Key data copy [%d]", nestedKeyTag-1);
							}
						delete startData;
						startData = NULL;
						}
					}
				}
			}
		else
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_15, "ERROR CONFIGURATION FILE PARSING: NO SC EAP KEY INFO TAG");
			}

		// Get EMSK
		if (nestedExtKeyTag < countExtKey)
			{
			item = const_cast<CTestConfigItem*>(CfgFile()->Item(KScEapKeyEMSK, nestedExtKeyTag++));

			// parse key
			if (item == NULL)
				{
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_16, "WARNING CONFIGURATION FILE PARSING: could not read EAP Ext Key tag [%d]", nestedExtKeyTag-1);
				}
			else
				{
				ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 0, ptr);
				if (ret != KErrNone)
					{
					OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_17, "WARNING CONFIGURATION FILE PARSING: could not read EAP Ext Key tag's data [%d]", nestedExtKeyTag-1);
					}
				else
					{
					TRAPD(kAllocErr, startData = ptr.AllocL());
					if (kAllocErr != KErrNone)
						{
						OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_18, "WARNING CONFIGURATION FILE PARSING: could not allocate mem for EAP Ext Key data [%d]", nestedExtKeyTag-1);
						}
					else
						{
						tempPtr.Set(startData->Des());
						switch (dataFrmt)
							{
						case EConfigDataFormatMixedBinaryAndAscii:
							ParseMixedBinaryAsciiDataL(tempPtr);
							break;
						//case EConfigDataFormatAscii: // Do nothing
						//default:
							}
						// need to re-copy because converting to binary changes size
						TRAP(kAllocErr, procInfo.iEapExtKey = tempPtr.AllocL());
						if (kAllocErr != KErrNone)
							{
							OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_19, "WARNING CONFIGURATION FILE PARSING: could not allocate mem for EAP Ext Key data copy [%d]", nestedExtKeyTag-1);
							}
						delete startData;
						startData = NULL;
						}
					}
				}
			}
		else
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_20, "ERROR CONFIGURATION FILE PARSING: NO SC EAP EXT KEY INFO TAG");
			}

		// Get Permanent Identity
		if (nestedIdTag < countId)
			{
			item = const_cast<CTestConfigItem*>(CfgFile()->Item(KScEapIdentity, nestedIdTag++));

			// parse id
			if (item == NULL)
				{
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_21, "WARNING CONFIGURATION FILE PARSING: could not read EAP id tag [%d]", nestedIdTag-1);
				}
			else
				{
				ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 0, ptr);
				if (ret != KErrNone)
					{
					OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_22, "WARNING CONFIGURATION FILE PARSING: could not read EAP Id tag's data [%d]", nestedIdTag-1);
					}
				else
					{
					TRAPD(idAllocErr, startData = ptr.AllocL());
					if (idAllocErr != KErrNone)
						{
						OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_23, "WARNING CONFIGURATION FILE PARSING: could not allocate mem for EAP Id data [%d]", nestedIdTag-1);
						}
					else
						{
						TPtr8 tempPtr(NULL,0);
						tempPtr.Set(startData->Des());
						switch (dataFrmt)
							{
						case EConfigDataFormatMixedBinaryAndAscii:
							ParseMixedBinaryAsciiDataL(tempPtr);
							break;
						//case EConfigDataFormatAscii: // Do nothing
						//default:
							}
						// need to re-copy because converting to binary changes size
						TRAP(idAllocErr, procInfo.iEapId = tempPtr.AllocL());
						if (idAllocErr != KErrNone)
							{
							OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_24, "WARNING CONFIGURATION FILE PARSING: could not allocate mem for EAP Id data copy [%d]", nestedIdTag-1);
							}
						delete startData;
						startData = NULL;
						}
					}
				}
			}
		else
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_25, "WARNING CONFIGURATION FILE PARSING: NO SC EAP ID INFO TAG");
			}

		// Get Pseudonym Identity
		if (nestedPsIdTag < countPsId)
			{
			item = const_cast<CTestConfigItem*>(CfgFile()->Item(KScEapPsIdentity, nestedPsIdTag++));

			// parse id
			if (item == NULL)
				{
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_26, "WARNING CONFIGURATION FILE PARSING: could not read EAP Pseudonym id tag [%d]", nestedPsIdTag-1);
				}
			else
				{
				ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 0, ptr);
				if (ret != KErrNone)
					{
					OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_27, "WARNING CONFIGURATION FILE PARSING: could not read EAP Pseudonym Id tag's data [%d]", nestedPsIdTag-1);
					}
				else
					{
					TRAPD(idAllocErr, startData = ptr.AllocL());
					if (idAllocErr != KErrNone)
						{
						OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_28, "WARNING CONFIGURATION FILE PARSING: could not allocate mem for EAP Pseudonym Id data [%d]", nestedPsIdTag-1);
						}
					else
						{
						TPtr8 tempPtr(NULL,0);
						tempPtr.Set(startData->Des());
						switch (dataFrmt)
							{
						case EConfigDataFormatMixedBinaryAndAscii:
							ParseMixedBinaryAsciiDataL(tempPtr);
							break;
						//case EConfigDataFormatAscii: // Do nothing
						//default:
							}
						// need to re-copy because converting to binary changes size
						TRAP(idAllocErr, procInfo.iEapPsId = tempPtr.AllocL());
						if (idAllocErr != KErrNone)
							{
							OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_29, "WARNING CONFIGURATION FILE PARSING: could not allocate mem for EAP Pseudonym Id data copy [%d]", nestedPsIdTag-1);
							}
						delete startData;
						startData = NULL;
						}
					}
				}
			}
		else
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_30, "WARNING CONFIGURATION FILE PARSING: NO SC EAP PS ID INFO TAG");
			}

		// Get challenges
		// numChallenges is what is parsed from config.txt and will be
		//    decremented in loop till zero is reached.
		// nestedChlTag keeps track of the current KScEapChallenge tag
		//    being read of the total; i.e. not just nested.
		// countChl is the total number of KScEapChallenge found.
		while (numChallenges != 0)
			{
			if (nestedChlTag >= countChl)
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_31, "WARNING CONFIGURATION FILE PARSING: NO MORE SC EAP Challenge INFO TAG");
				break;
				}

			item = const_cast<CTestConfigItem*>(CfgFile()->Item(KScEapChallenge, nestedChlTag++));
			numChallenges--;

			// parse delay and challenge/response and auth status
			if (item == NULL)
				{
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_32, "WARNING CONFIGURATION FILE PARSING: could not read EAP challenge tag [%d]", nestedChlTag-1);
				continue;
				}

			// Parse challenge
			ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 0, ptr);
			if (ret != KErrNone)
				{
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_33, "WARNING CONFIGURATION FILE PARSING: could not read EAP challenge data [%d]", nestedChlTag-1);
				continue;
				}

			TRAPD(leaveErr, startData = ptr.AllocL());
			if (leaveErr != KErrNone)
				{
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_34, "WARNING CONFIGURATION FILE PARSING: could not allocate mem for EAP challenge data [%d]", nestedChlTag-1);
				continue;
				}
			tempPtr.Set(startData->Des());
			switch (dataFrmt)
				{
			case EConfigDataFormatMixedBinaryAndAscii:
				ParseMixedBinaryAsciiDataL(tempPtr);
				break;
			//case EConfigDataFormatAscii: // Do nothing
			//default:
				}
			TEapChngResp newChRespData;
			TRAP(leaveErr, newChRespData.iChallenge = tempPtr.AllocL());
			if (leaveErr != KErrNone)
				{
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_35, "WARNING CONFIGURATION FILE PARSING: could not allocate mem for EAP challenge data copy [%d]", nestedKeyTag-1);
				}
			delete startData;
			startData = NULL;

			// Parse response
			ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 1, ptr);
			if (ret != KErrNone)
				{
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_36, "WARNING CONFIGURATION FILE PARSING: could not read EAP response data [%d]", nestedChlTag-1);
				continue;
				}

			TRAP(leaveErr, startData = ptr.AllocL());
			if (leaveErr != KErrNone)
				{
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_37, "WARNING CONFIGURATION FILE PARSING: could not allocate mem for EAP response data [%d]", nestedChlTag-1);
				continue;
				}
			tempPtr.Set(startData->Des());
			switch (dataFrmt)
				{
			case EConfigDataFormatMixedBinaryAndAscii:
				ParseMixedBinaryAsciiDataL(tempPtr);
				break;
			//case EConfigDataFormatAscii: // Do nothing
			//default:
				}
			TRAP(leaveErr, newChRespData.iResp = tempPtr.AllocL());
			if (leaveErr != KErrNone)
				{
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_38, "WARNING CONFIGURATION FILE PARSING: could not allocate mem for EAP response data copy [%d]", nestedKeyTag-1);
				}
			delete startData;
			startData = NULL;

			// Parse status
			TInt stat;
			ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 2, stat);
			if (ret != KErrNone)
				{
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_39, "WARNING CONFIGURATION FILE PARSING: could not read EAP auth status [%d]", nestedChlTag-1);
				continue;
				}

			newChRespData.iAuthStatus = static_cast<RMobileSmartCardEap::TEapAuthStatus>(stat);

			leaveErr = procInfo.iChResp.Append(newChRespData);
			if (leaveErr != KErrNone)
				{
				OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_40, "WARNING CONFIGURATION FILE PARSING: could not allocate mem for EAP challenge/resp data [%d] [err=%d]", nestedChlTag-1, leaveErr);
				}
			} // end while

		TInt errAppend = iEapProcData.Append(procInfo);
		if (errAppend != KErrNone)
			{
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_41, "ERROR CONFIGURATION FILE PARSING: Could not store parsed EAP procedure data [err=%d]", errAppend);
			}
		else
			{
			iDiscardedProcedure.Append(EFalse);
			}
		} // end for; parsing EAP procedures from config.txt

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_PARSEEAPINFOL_42, "CSimSmartCardEapManager::ParseEapInfoL completed");
	}

/**
Function leaves if <aAID,aEapType> is not in config file.

@leave KErrNotFound if <aAID,aEapType> is not found.
*/
void CSimSmartCardEapManager::AID_EapType_ExistsInConfigL(const RMobilePhone::TAID& aAID, const RMobileSmartCardEap::TEapType& aEapType)
	{
	for (TInt ii = 0; ii < iEapProcData.Count(); ii++)
		{
		TEapProcedureData& temp = iEapProcData[ii];
		if (temp.iAID == aAID)
			{
			// two ifs rather than && to help debug
			if (temp.iEapType == aEapType)
				{
				if (!iDiscardedProcedure[ii])
					{
					return;
					}
				}
			}
		}
	User::Leave(KErrNotFound);
	}

CTelObject* CSimSmartCardEapManager::CreateScEapSubSessionL(RMobilePhone::TAID& aAID, RMobileSmartCardEap::TEapType& aEapType)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_CREATESCEAPSUBSESSIONL_1, "CSimSmartCardEapManager::CreateScEapSubSessionL called");
	// If no config exists for this eapAID,eapType pair, then this will leave
	AID_EapType_ExistsInConfigL(aAID, aEapType);

	// If exists then phoneScEap guaranteed to get data in
	// InitialiseEapMethod, since only one object can be created
	// using the unique <iAID,iEapType> pair.

	CSimSmartCardEap* phoneScEap = NULL;
	TRAPD(err, phoneScEap = CSimSmartCardEap::NewL(iPhone, this, aAID, aEapType));

	if (err != KErrNone)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAPMANAGER_CREATESCEAPSUBSESSIONL_2, "ERROR could not create CSimSmartCardEap object [err=%d]", err);
		User::Leave(err);
		}

	return phoneScEap;
	}

/**
Returns the first procedure data section, as parsed from the
config.txt, with aAID and aEapType.

@param aAID
@param aEapType
@return The first procedure data with aAID and aEapType from the
        config.txt.  NULL if no such procedure data is found.
*/
TEapProcedureData* CSimSmartCardEapManager::ProcData(const RMobilePhone::TAID& aAID, const RMobileSmartCardEap::TEapType& aEapType)
	{
	for (TInt ii = 0; ii < iEapProcData.Count(); ii++)
		{
		if (iEapProcData[ii].iAID == aAID)
			{
			// two ifs rather than && to help debug
			if (iEapProcData[ii].iEapType == aEapType)
				{
				if (!iDiscardedProcedure[ii])
					{
					return &iEapProcData[ii];
					}
				}
			}
		}
	return NULL;
	}

void CSimSmartCardEapManager::ProcDataUseCompleted(const TEapProcedureData* aProcData)
	{
	if (aProcData == NULL)
		{
		return;
		}

	TInt pos = KErrNotFound;

	for (TInt ii = 0; ii < iEapProcData.Count(); ii++)
		{
		if ((&iEapProcData[ii]) == aProcData)
			{
			pos = ii;
			break;
			}
		}

	if (pos != KErrNotFound)
		{
		iDiscardedProcedure[pos] = ETrue;
		}
	}

/**
Register a sub-session object.  At the present time, list is maintained
for clean-up only; i.e. this object stores pointers to all EAP sub-
session objects.
*/
void CSimSmartCardEapManager::RegisterSubSessionL(CSimSmartCardEap* aToRegister)
	{
	iSubSessionObjs.AppendL(aToRegister);
	}

/**
Remove a sub-session object from list of active sub-sessions.
*/
TInt CSimSmartCardEapManager::DeRegisterSubSession(const CSimSmartCardEap* aToDeRegister)
	{
	TInt index = iSubSessionObjs.Find(aToDeRegister);

	if (index < 0)
		{
		return index;
		}

	iSubSessionObjs.Remove(index);
	iSubSessionObjs.Compress();

	return KErrNone;
	}

// CSimSmartCardEap implementation //

CSimSmartCardEap* CSimSmartCardEap::NewL(CSimPhone *aPhone, CSimSmartCardEapManager* aEapMan, RMobilePhone::TAID& aAID, RMobileSmartCardEap::TEapType& aEapType)
	{
	CSimSmartCardEap* phone = new(ELeave) CSimSmartCardEap(aPhone, aAID, aEapType);
	CleanupStack::PushL(phone);
	phone->ConstructL(aEapMan);
	CleanupStack::Pop();
	return phone;
	}

CSimSmartCardEap::CSimSmartCardEap(CSimPhone *aPhone, RMobilePhone::TAID& aAID, RMobileSmartCardEap::TEapType& aEapType)
: iPhone(aPhone), iProcedureData(NULL), iSSInitialised(EFalse),
  iAccessStatus(RMobileSmartCardEap::EEapMethodAvailable), iAuthStatus(RMobileSmartCardEap::ENoAuthStarted),
  iCliTerminationListener(NULL), iCurrentChallenge(0)
	{
	iAID = aAID;
	iEapType = aEapType;
	}

void CSimSmartCardEap::ConstructL(CSimSmartCardEapManager* aEapMan)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAP_CONSTRUCTL_1, "CSimSmartCardEap: starting second phase construction");

	iSemaphr.CreateGlobal(KNullDesC, EOwnerThread);
	aEapMan->RegisterSubSessionL(this);
	iEapMan = aEapMan;

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAP_CONSTRUCTL_2, "CSimSmartCardEap created");
	}

CSimSmartCardEap::~CSimSmartCardEap()
	{
	if (iCliTerminationListener != NULL)
		{
		delete iCliTerminationListener;
		iCliTerminationListener = NULL;
		}
	iSemaphr.Close();

	// remove config entry from manager
	if (iProcedureData != NULL)
		{
		iEapMan->ProcDataUseCompleted(iProcedureData);
		iProcedureData = NULL;
		}

	TInt err = iEapMan->DeRegisterSubSession(this);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAP_DTOR_1, "CSimSmartCardEap destroyed, deregistering returned %d", err);

	iEapMan = NULL;
	}


void CSimSmartCardEap::Init()
	{
	// server calls this function once it has created the sub-session
	// it gives the TSY chance to do any initialisation it may need to do for
	// this sub-session
	}

CTelObject* CSimSmartCardEap::OpenNewObjectByNameL(const TDesC& /*aName*/)
	{
	// Server calls this function when a client is opening an object from the phone
	// for the first time.
	// Multiple clients opening handles to the same sub-session object will be dealt with
	// by the server - i.e. by reference counting
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject* CSimSmartCardEap::OpenNewObjectL(TDes& /*aNewName*/)
	{
	// all objects opened from the phone are opened by name, hence this method
	// is not supported
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject::TReqMode CSimSmartCardEap::ReqModeL(const TInt aIpc)
	{
	// ReqModeL is called from the server's CTelObject::ReqAnalyserL
	// in order to check the type of request it has

	// The following are example request types for this dummy TSY
	// All TSYs do not have to have these request types but they have been given
	// "sensible" values in this test code

	CTelObject::TReqMode ret = 0;

	switch (aIpc)
		{
	// Non flow-controlled requests
	case EMobileSmartCardEapInitialiseEapMethod:
	case EMobileSmartCardEapGetUserIdentity:
	case EMobileSmartCardEapGetAuthenticationStatus:
	case EMobileSmartCardEapGetEapKey:
	case EMobileSmartCardEapAuthenticationPhase1:
	case EMobileSmartCardEapAuthenticationPhase2:
	case EMobileSmartCardEapReleaseEapMethod:
	case EMobileSmartCardEapGetEapMethodAccessStatus:
		break;

	case EMobileSmartCardEapNotifyEapMethodAccessStatusChange:
		ret = KReqModeMultipleCompletionEnabled;
		break;

	default:
		User::Leave(KErrNotSupported);
		break;
		}

	return ret;
	}

TInt CSimSmartCardEap::RegisterNotification(const TInt /*aIpc*/)
	{
	// RegisterNotification is called when the server recognises that this notification
	// is being posted for the first time on this sub-session object.

	// It enables the TSY to "turn on" any regular notification messages that it may 
	// receive from the phone

	return KErrNone;
	}

TInt CSimSmartCardEap::DeregisterNotification(const TInt /*aIpc*/)
	{
	// DeregisterNotification is called when the server recognises that this notification
	// will not be posted again because the last client to have a handle on this sub-session
	// object has just closed the handle.

	// It enables the TSY to "turn off" any regular notification messages that it may
	// receive from the phone

	return KErrNone;
	}

TInt CSimSmartCardEap::NumberOfSlotsL(const TInt aIpc)
	{
	// NumberOfSlotsL is called by the server when it is registering a new notification
	// It enables the TSY to tell the server how many buffer slots to allocate for
	// "repost immediately" notifications that may trigger before clients collect them

	TInt numberOfSlots = 1;

	switch (aIpc)
		{
	case EMobileSmartCardEapNotifyEapMethodAccessStatusChange:
		numberOfSlots = 3;
		break;

	default:
		// Unknown or invalid Phone IPC
		User::Leave(KErrNotSupported);
		break;
		}

	return numberOfSlots;
	}

TInt CSimSmartCardEap::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,
                               const TDataPackage& aPackage)
	{
	// ExtFunc is called by the server when it has a "extended", i.e. non-core ETel request 
	// for the TSY to process
	// A request handle, request type and request data are passed to the TSY

	TAny* dataPtr = aPackage.Ptr1();
	TAny* dataPtr2 = aPackage.Ptr2();

	// The request data has to extracted from TDataPackage and the TAny* pointers have to
	// be "cast" to the expected request data type

	switch(aIpc)
		{
	// Non-Flow controlled requests

	case EMobileSmartCardEapInitialiseEapMethod:
		return SimInitialiseEapMethod(aTsyReqHandle,
		         reinterpret_cast<TThreadId*>(dataPtr));

	case EMobileSmartCardEapGetUserIdentity:
		return SimGetUserIdentity(aTsyReqHandle,
		         reinterpret_cast<RMobileSmartCardEap::TEapUserIdType*>(dataPtr),
		         aPackage.Des2n());

	case EMobileSmartCardEapGetAuthenticationStatus:
		return SimGetAuthenticationStatus(aTsyReqHandle, 
		         reinterpret_cast<RMobileSmartCardEap::TEapAuthStatus*>(dataPtr));

	case EMobileSmartCardEapGetEapKey:
		return SimGetEapKey(aTsyReqHandle,
		         reinterpret_cast<RMobileSmartCardEap::TEapKeyTag*>(dataPtr),
		         aPackage.Des2n());

	case EMobileSmartCardEapAuthenticationPhase1:
		return SimSetAuthenticateDataForPhase1(aTsyReqHandle,
		         aPackage.Des1n(), reinterpret_cast<TInt*>(dataPtr2));

	case EMobileSmartCardEapAuthenticationPhase2:
		return SimGetAuthenticateDataForPhase2(aTsyReqHandle,
		         aPackage.Des1n(), aPackage.Des2n());

	case EMobileSmartCardEapReleaseEapMethod:
		return SimReleaseEapMethod(aTsyReqHandle);

	case EMobileSmartCardEapGetEapMethodAccessStatus:
		return SimGetEapMethodAccessStatus(aTsyReqHandle,
		         reinterpret_cast<RMobileSmartCardEap::TEapMethodAccessStatus*>(dataPtr));

	case EMobileSmartCardEapNotifyEapMethodAccessStatusChange:
		return SimNotifyEapMethodAccessStatusChange(aTsyReqHandle,
		         reinterpret_cast<RMobileSmartCardEap::TEapMethodAccessStatus*>(dataPtr));

	default:
		return KErrNotSupported;
		}
	}

TInt CSimSmartCardEap::CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle)
	{
	// CancelService is called by the server when it is "cleaning-up" any still outstanding
	// asynchronous requests before closing a client's sub-session.
	// This will happen if a client closes its R-class handle without cancelling outstanding
	// asynchronous requests.

	switch (aIpc)
		{
	case EMobileSmartCardEapGetUserIdentity:
		return SimGetUserIdentityCancel(aTsyReqHandle);
	case EMobileSmartCardEapGetAuthenticationStatus:
		return SimGetAuthenticationStatusCancel(aTsyReqHandle);
	case EMobileSmartCardEapGetEapKey:
		return SimGetEapKeyCancel(aTsyReqHandle);
	case EMobileSmartCardEapInitialiseEapMethod:
		return SimInitialiseEapMethodCancel(aTsyReqHandle);
	case EMobileSmartCardEapAuthenticationPhase1:
	case EMobileSmartCardEapAuthenticationPhase2:
		return SimSmartCardEapAuthenticationCancel(aTsyReqHandle);
	case EMobileSmartCardEapNotifyEapMethodAccessStatusChange:
		return SimNotifyEapMethodAccessStatusChangeCancel(aTsyReqHandle);
	default:
		return KErrNotSupported;
		}
	}

RHandleBase* CSimSmartCardEap::GlobalKernelObjectHandle()
	{
	return &iSemaphr;
	}

TInt CSimSmartCardEap::SimInitialiseEapMethod(const TTsyReqHandle aTsyReqHandle, TThreadId* aThreadId)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAP_SIMINITIALISEEAPMETHOD_1, "CSimSmartCardEap::SimInitialiseEapMethod called");
	// This can only be called through RMobileSmartCardEap for one instance

	if (iSSInitialised)
		{
		// re-initialise request sent, so will be treated as a mistake
		// and nothing will happen!
		ReqCompleted(aTsyReqHandle, KErrNone);
		}
	else
		{
		delete iCliTerminationListener;
		TRAPD(err, iCliTerminationListener = CThreadTerminationListener::NewL(this, *aThreadId));
		if (err != KErrNone)
			{
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAP_SIMINITIALISEEAPMETHOD_2, "ERROR could not create a client termination listener [err=%d] (not initialised)", err);
			ReqCompleted(aTsyReqHandle, err);
			}
		else
			{
			iProcedureData = iEapMan->ProcData(iAID, iEapType);
			if (iProcedureData == NULL)
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAP_SIMINITIALISEEAPMETHOD_3, "ERROR could not find sub-session's procedure");
				ReqCompleted(aTsyReqHandle, KErrNotFound);
				return KErrNone;
				}

			iSSInitialised = ETrue;
			iCliTerminationListener->Start();
			iAccessStatus = RMobileSmartCardEap::EEapMethodInUseApplicationActive;
			SimCompleteNotifyEapMethodAccessStatusChange();
			ReqCompleted(aTsyReqHandle, KErrNone);
			}
		}

	return KErrNone;
	}

TInt CSimSmartCardEap::SimInitialiseEapMethodCancel(const TTsyReqHandle aTsyReqHandle)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAP_SIMINITIALISEEAPMETHODCANCEL_1, "CSimSmartCardEap::SimInitialiseEapMethodCancel called");
	iProcedureData = NULL;
	iSSInitialised = EFalse;
	iAccessStatus = RMobileSmartCardEap::EEapMethodAvailable;
	SimCompleteNotifyEapMethodAccessStatusChange();
	ReqCompleted(aTsyReqHandle, KErrCancel);
	return KErrNone;
	}

TInt CSimSmartCardEap::SimGetUserIdentity(const TTsyReqHandle aTsyReqHandle, RMobileSmartCardEap::TEapUserIdType* aEapIdType, TDes8* aUserId)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAP_SIMGETUSERIDENTITY_1, "CSimSmartCardEap::SimGetUserIdentity called");

	RMobileSmartCardEap::TEapUserIdentityV6Pckg *userIdPckg = reinterpret_cast<RMobileSmartCardEap::TEapUserIdentityV6Pckg*>(aUserId);
	RMobileSmartCardEap::TEapUserIdentityV6 &userId = (*userIdPckg)();
	
	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(userId);
		if(err != KErrNone)
			{
			iPhone->ReqCompleted(aTsyReqHandle, err);
			return KErrNone;
			}

	if (*aEapIdType == RMobileSmartCardEap::EPermanentIdentity)
		{
		if (iProcedureData->iEapId == NULL)
			{
			ReqCompleted(aTsyReqHandle, KErrNotFound);
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAP_SIMGETUSERIDENTITY_2, "ERROR EAP sub-session does not contain EPermanentIdentity");
			return KErrNone;
			}

		userId.iEapId = iProcedureData->iEapId->Des();
		}
	else if (*aEapIdType == RMobileSmartCardEap::EPseudonymIdentity)
		{
		if (iProcedureData->iEapPsId == NULL)
			{
			ReqCompleted(aTsyReqHandle, KErrNotFound);
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAP_SIMGETUSERIDENTITY_3, "ERROR EAP sub-session does not contain EPseudonymIdentity");
			return KErrNone;
			}

		userId.iEapId = iProcedureData->iEapPsId->Des();
		}
	else
		{
		ReqCompleted(aTsyReqHandle, KErrArgument);
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAP_SIMGETUSERIDENTITY_4, "ERROR invalid EAP id type requested [tag=%d]", *aEapIdType);
		return KErrNone;
		}

	ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}

TInt CSimSmartCardEap::SimGetUserIdentityCancel(const TTsyReqHandle aTsyReqHandle)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAP_SIMGETUSERIDENTITYCANCEL_1, "CSimSmartCardEap::SimGetUserIdentityCancel called");
	ReqCompleted(aTsyReqHandle, KErrCancel);
	return KErrNone;
	}
	
TInt CSimSmartCardEap::SimGetAuthenticationStatus(const TTsyReqHandle aTsyReqHandle, RMobileSmartCardEap::TEapAuthStatus* aAuthStatus)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAP_SIMGETAUTHENTICATIONSTATUS_1, "CSimSmartCardEap::SimGetAuthenticationStatus called");

	(*aAuthStatus) = iAuthStatus;

	ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}
	
TInt CSimSmartCardEap::SimGetAuthenticationStatusCancel(const TTsyReqHandle aTsyReqHandle)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAP_SIMGETAUTHENTICATIONSTATUSCANCEL_1, "CSimSmartCardEap::SimGetAuthenticationStatusCancel called");
	ReqCompleted(aTsyReqHandle, KErrCancel);
	return KErrNone;
	}

TInt CSimSmartCardEap::SimGetEapKey(const TTsyReqHandle aTsyReqHandle, RMobileSmartCardEap::TEapKeyTag* aEapKeyTag, TDes8* aKey)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAP_SIMGETEAPKEY_1, "CSimSmartCardEap::SimGetEapKey called");

	RMobileSmartCardEap::TEapKeyV6Pckg *keyPckg = reinterpret_cast<RMobileSmartCardEap::TEapKeyV6Pckg*>(aKey);
	RMobileSmartCardEap::TEapKeyV6 &key = (*keyPckg)();

	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(key);
		if(err != KErrNone)
			{
			iPhone->ReqCompleted(aTsyReqHandle, err);
			return KErrNone;
			}

	if (*aEapKeyTag == RMobileSmartCardEap::EEapKeyMSK)
		{
		if (iProcedureData->iEapKey == NULL)
			{
			ReqCompleted(aTsyReqHandle, KErrNotFound);
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAP_SIMGETEAPKEY_2, "ERROR EAP sub-session does not contain EEapKeyMSK");
			return KErrNone;
			}

		key.iEapKey = iProcedureData->iEapKey->Des();
		}
	else if (*aEapKeyTag == RMobileSmartCardEap::EEapKeyEMSK)
		{
		if (iProcedureData->iEapExtKey == NULL)
			{
			ReqCompleted(aTsyReqHandle, KErrNotFound);
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAP_SIMGETEAPKEY_3, "ERROR EAP sub-session does not contain EEapKeyEMSK");
			return KErrNone;
			}

		key.iEapKey = iProcedureData->iEapExtKey->Des();
		}
	else
		{
		ReqCompleted(aTsyReqHandle, KErrArgument);
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAP_SIMGETEAPKEY_4, "ERROR invalid EAP key tag requested [tag=%d]", *aEapKeyTag);
		return KErrNone;
		}

	ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}

TInt CSimSmartCardEap::SimGetEapKeyCancel(const TTsyReqHandle aTsyReqHandle)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAP_SIMGETEAPKEYCANCEL_1, "CSimSmartCardEap::SimGetEapKeyCancel called");
	ReqCompleted(aTsyReqHandle, KErrCancel);
	return KErrNone;
	}

TInt CSimSmartCardEap::SimSetAuthenticateDataForPhase1(const TTsyReqHandle aTsyReqHandle, TDes8* aEapAuthData, TInt* aPhase1Size)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAP_SIMSETAUTHENTICATEDATAFORPHASE1_1, "CSimSmartCardEap::SimSetAuthenticateDataForPhase1 called");

	if (iCurrentChallenge >= iProcedureData->iChResp.Count())
		{
		ReqCompleted(aTsyReqHandle, KErrAccessDenied);
		return KErrNone;
		}

	RMobileSmartCardEap::CEapAuthenticateRequestDataV6* authReq = NULL;
	TRAPD(err, authReq = RMobileSmartCardEap::CEapAuthenticateRequestDataV6::NewL());
	if (err != KErrNone)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAP_SIMSETAUTHENTICATEDATAFORPHASE1_2, "ERR Could not allocate memory for challenge request object [err=%d]", err);
		ReqCompleted(aTsyReqHandle, err);
		return KErrNone;
		}

	TRAP(err, authReq->InternalizeL(*aEapAuthData));
	if (err != KErrNone)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAP_SIMSETAUTHENTICATEDATAFORPHASE1_3, "ERR Could not allocate memory for challenge request [err=%d]", err);
		ReqCompleted(aTsyReqHandle, err);
		return KErrNone;
		}

	TPtr8 reqPacket = authReq->GetEapReqPacket();

	TPtr8 tempPtr(NULL, 0);
	tempPtr.Set(iProcedureData->iChResp[iCurrentChallenge].iChallenge->Des());

	if (reqPacket != tempPtr)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAP_SIMSETAUTHENTICATEDATAFORPHASE1_4, "ERR challenge request does not match config [currentChallenge=%d]", iCurrentChallenge);
		ReqCompleted(aTsyReqHandle, KErrCorrupt);
		return KErrNone;
		}

	(*aPhase1Size) = iProcedureData->iChResp[iCurrentChallenge].iResp->Length();

	ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}

TInt CSimSmartCardEap::SimGetAuthenticateDataForPhase2(const TTsyReqHandle aTsyReqHandle, TDes8* /*aEapAuthData*/, TDes8* aPhase2Resp)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAP_SIMGETAUTHENTICATEDATAFORPHASE2_1, "CSimSmartCardEap::SimSetAuthenticateDataForPhase2 called");

	if (iCurrentChallenge >= iProcedureData->iChResp.Count())
		{
		ReqCompleted(aTsyReqHandle, KErrAccessDenied);
		return KErrNone;
		}

	TPtr8 tempPtr(NULL, 0);
	tempPtr.Set(iProcedureData->iChResp[iCurrentChallenge].iResp->Des());

	aPhase2Resp->Copy(tempPtr);
 	iAuthStatus = iProcedureData->iChResp[iCurrentChallenge].iAuthStatus;
 	iCurrentChallenge++;

	ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}

TInt CSimSmartCardEap::SimSmartCardEapAuthenticationCancel(const TTsyReqHandle aTsyReqHandle)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAP_SIMSMARTCARDEAPAUTHENTICATIONCANCEL_1, "CSimSmartCardEap::SimSmartCardEapAuthenticationCancel called");
	ReqCompleted(aTsyReqHandle, KErrCancel);
	return KErrNone;
	}

TInt CSimSmartCardEap::SimReleaseEapMethod(const TTsyReqHandle aTsyReqHandle)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAP_SIMRELEASEEAPMETHOD_1, "CSimSmartCardEap::SimReleaseEapMethod called");
	iSSInitialised = EFalse;
	iAccessStatus = RMobileSmartCardEap::EEapMethodAvailable;
	ReqCompleted(aTsyReqHandle, KErrNone);

	SimCompleteNotifyEapMethodAccessStatusChange();

	// remove config entry from manager
	iEapMan->ProcDataUseCompleted(iProcedureData);
	iProcedureData = NULL;

	return KErrNone;
	}

TInt CSimSmartCardEap::SimGetEapMethodAccessStatus(const TTsyReqHandle aTsyReqHandle, RMobileSmartCardEap::TEapMethodAccessStatus* aEapState)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAP_SIMGETEAPMETHODACCESSSTATUS_1, "CSimSmartCardEap::SimGetEapMethodAccessStatus called");
	*aEapState = iAccessStatus;
	ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}

TInt CSimSmartCardEap::SimNotifyEapMethodAccessStatusChange(const TTsyReqHandle aTsyReqHandle, RMobileSmartCardEap::TEapMethodAccessStatus* aEapState)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAP_SIMNOTIFYEAPMETHODACCESSSTATUSCHANGE_1, "CSimSmartCardEap::SimNotifyEapMethodAccessStatusChange called");
	__ASSERT_ALWAYS(!iEapAccessNotifyData.iNotifyPending, PanicClient(EEtelPanicRequestAsyncTwice));

	iEapAccessNotifyData.iNotifyPending = ETrue;
	iEapAccessNotifyData.iNotifyHandle = aTsyReqHandle;
	iEapAccessNotifyData.iNotifyData = aEapState;

	return KErrNone;
	}

TInt CSimSmartCardEap::SimNotifyEapMethodAccessStatusChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAP_SIMNOTIFYEAPMETHODACCESSSTATUSCHANGECANCEL_1, "CSimSmartCardEap::SimNotifyEapMethodAccessStatusChangeCancel called");
	if(iEapAccessNotifyData.iNotifyPending)
		{
		iEapAccessNotifyData.iNotifyPending = EFalse;
		ReqCompleted(aTsyReqHandle, KErrCancel);
		return KErrNone;
		}	

//	iPhone->ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

void CSimSmartCardEap::SimCompleteNotifyEapMethodAccessStatusChange()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDEAP_SIMCOMPLETENOTIFYEAPMETHODACCESSSTATUSCHANGE_1, "CSimSmartCardEap::SimCompleteNotifyEapMethodAccessStatusChange called");

	if(iEapAccessNotifyData.iNotifyPending)
		{
		iEapAccessNotifyData.iNotifyPending = EFalse;
		*(reinterpret_cast<RMobileSmartCardEap::TEapMethodAccessStatus*>(iEapAccessNotifyData.iNotifyData)) = iAccessStatus;
		ReqCompleted(iEapAccessNotifyData.iNotifyHandle, KErrNone);
		}
	}

void CSimSmartCardEap::ClientHasTerminated(TInt /*aExitReason*/)
	{
	// Can TSY do anything with the thread's exit reason?
	// Exit code can be a zero (e.g. for KERN-EXEC 0) a positive value
	// (e.g. for KERN-EXEC 3) or a negative error.

	switch (iAccessStatus)
		{
	case RMobileSmartCardEap::EEapMethodInUseApplicationActive:
		iSSInitialised = EFalse;
		iAccessStatus = RMobileSmartCardEap::EEapMethodAvailable;
		SimCompleteNotifyEapMethodAccessStatusChange();
		iSemaphr.Signal();

		// remove config entry from manager
		iEapMan->ProcDataUseCompleted(iProcedureData);
		iProcedureData = NULL;

		break;

	case RMobileSmartCardEap::EEapMethodInUseApplicationInactive:
		iAccessStatus = RMobileSmartCardEap::EEapMethodAvailable;
		SimCompleteNotifyEapMethodAccessStatusChange();
		iSemaphr.Signal();
		break;

	default:
		;
		}
	}

//
// Class definition for monitoring thread termination
//

CSimSmartCardEap::CThreadTerminationListener* CSimSmartCardEap::CThreadTerminationListener::NewL(CSimSmartCardEap* aSubSess, const TThreadId& aId)
	{
	CThreadTerminationListener* self = new(ELeave) CThreadTerminationListener(aSubSess);
	CleanupStack::PushL(self);
	self->ConstructL(aId);
	CleanupStack::Pop(self);
	return self;
	}

CSimSmartCardEap::CThreadTerminationListener::CThreadTerminationListener(CSimSmartCardEap* aSubSess)
: CActive(EPriorityStandard), iSubSess(aSubSess)
	{
	}

void CSimSmartCardEap::CThreadTerminationListener::ConstructL(const TThreadId& aId)
	{
	TInt openTh = iCliThread.Open(aId);
	User::LeaveIfError(openTh);
    CActiveScheduler::Add(this);
	}

void CSimSmartCardEap::CThreadTerminationListener::Start()
	{
	iCliThread.Logon(iStatus);
	SetActive();
	}

void CSimSmartCardEap::CThreadTerminationListener::RunL()
	{
	iSubSess->ClientHasTerminated(iStatus.Int());
	}

void CSimSmartCardEap::CThreadTerminationListener::DoCancel()
	{
	iCliThread.LogonCancel(iStatus);
	}

CSimSmartCardEap::CThreadTerminationListener::~CThreadTerminationListener()
	{
	Cancel();
	iCliThread.Close();
	}
