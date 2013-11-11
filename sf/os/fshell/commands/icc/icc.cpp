// icc.cpp
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <fshell/ioutils.h>
#include <etelmm.h>
#include <commsdattypesv1_1.h>
#include <fshell/ltkutils.h>
#include <cdblen.h>


#define SERVICE_ENTRY(XXX) { XXX, #XXX }

struct SEnumFlag
	{
	TInt iValue;
	const char* iIndentifer;
	};

SEnumFlag KSstServices1to8[] =
	{
	SERVICE_ENTRY(RMobilePhone::KSstPin1Disable),
	SERVICE_ENTRY(RMobilePhone::KSstADN),
	SERVICE_ENTRY(RMobilePhone::KSstFDN),
	SERVICE_ENTRY(RMobilePhone::KSstSMS),
	SERVICE_ENTRY(RMobilePhone::KSstAoC),
	SERVICE_ENTRY(RMobilePhone::KSstCCP),
	SERVICE_ENTRY(RMobilePhone::KSstPLMNSelector)
	};

SEnumFlag KSstServices9to16[] =
	{
	SERVICE_ENTRY(RMobilePhone::KSstMSISDN),
	SERVICE_ENTRY(RMobilePhone::KSstExt1),
	SERVICE_ENTRY(RMobilePhone::KSstExt2),
	SERVICE_ENTRY(RMobilePhone::KSstSMSP),
	SERVICE_ENTRY(RMobilePhone::KSstLND),
	SERVICE_ENTRY(RMobilePhone::KSstCBMI),
	SERVICE_ENTRY(RMobilePhone::KSstGID1),
	SERVICE_ENTRY(RMobilePhone::KSstGID2)
	};

SEnumFlag KSstServices17to24[] =
	{
	SERVICE_ENTRY(RMobilePhone::KSstSPName),
	SERVICE_ENTRY(RMobilePhone::KSstSDN),
	SERVICE_ENTRY(RMobilePhone::KSstExt3),
	SERVICE_ENTRY(RMobilePhone::KSstVGCSList),
	SERVICE_ENTRY(RMobilePhone::KSstVBSList),
	SERVICE_ENTRY(RMobilePhone::KSsteMLPP),
	SERVICE_ENTRY(RMobilePhone::KSstAnswereMLPP)
	};

SEnumFlag KSstServices25to32[] =
	{
	SERVICE_ENTRY(RMobilePhone::KSstSmsCbDataDownload),
	SERVICE_ENTRY(RMobilePhone::KSstSmsPpDataDownload),
	SERVICE_ENTRY(RMobilePhone::KSstMenuSelection),
	SERVICE_ENTRY(RMobilePhone::KSstCallControl),
	SERVICE_ENTRY(RMobilePhone::KSstProactiveSim),
	SERVICE_ENTRY(RMobilePhone::KSstCBMIRanges),
	SERVICE_ENTRY(RMobilePhone::KSstBDN),
	SERVICE_ENTRY(RMobilePhone::KSstExt4)
	};

SEnumFlag KSstServices33to40[] =
	{
	SERVICE_ENTRY(RMobilePhone::KSstDepersonalisationKeys),
	SERVICE_ENTRY(RMobilePhone::KSstCooperativeNetworks),
	SERVICE_ENTRY(RMobilePhone::KSstSMStatusReports),
	SERVICE_ENTRY(RMobilePhone::KSstNetworkIndAlerting),
	SERVICE_ENTRY(RMobilePhone::KSstMoSmControlBySim),
	SERVICE_ENTRY(RMobilePhone::KSstGprs),
	SERVICE_ENTRY(RMobilePhone::KSstImage),
	SERVICE_ENTRY(RMobilePhone::KSstSoLSA)
	};

SEnumFlag KSstServices41to48[] =
	{
	SERVICE_ENTRY(RMobilePhone::KSstUssdStringInCallControl),
	SERVICE_ENTRY(RMobilePhone::KSstRunATCommand),
	SERVICE_ENTRY(RMobilePhone::KSstPlmnSelectorListWithAccessTechnology),
	SERVICE_ENTRY(RMobilePhone::KSstOplmnSelectorListWithAccessTechnology),
	SERVICE_ENTRY(RMobilePhone::KSstHplmnAccessTechnology),
	SERVICE_ENTRY(RMobilePhone::KSstCpbcchInformation),
	SERVICE_ENTRY(RMobilePhone::KSstInvestigationScan),
	SERVICE_ENTRY(RMobilePhone::KSstExtendedCcp)
	};

SEnumFlag KSstServices49to56[] =
	{
	SERVICE_ENTRY(RMobilePhone::KSstMExE),
	SERVICE_ENTRY(RMobilePhone::KSstRplmnLastUsedAccessTechnology)
	};

SEnumFlag KUstServices1to8[] =
	{
	SERVICE_ENTRY(RMobilePhone::KUstLocalPhBk),
	SERVICE_ENTRY(RMobilePhone::KUstFDN),
	SERVICE_ENTRY(RMobilePhone::KUstExt2),
	SERVICE_ENTRY(RMobilePhone::KUstSDN),
	SERVICE_ENTRY(RMobilePhone::KUstExt3),
	SERVICE_ENTRY(RMobilePhone::KUstBDN),
	SERVICE_ENTRY(RMobilePhone::KUstExt4),
	SERVICE_ENTRY(RMobilePhone::KUstOugoingCallInfo)
	};

SEnumFlag KUstServices9to16[] =
	{
	SERVICE_ENTRY(RMobilePhone::KUstIncomingCallInfo),
	SERVICE_ENTRY(RMobilePhone::KUstSMS),
	SERVICE_ENTRY(RMobilePhone::KUstSMSR),
	SERVICE_ENTRY(RMobilePhone::KUstSMSP),
	SERVICE_ENTRY(RMobilePhone::KUstAoC),
	SERVICE_ENTRY(RMobilePhone::KUstCCP),
	SERVICE_ENTRY(RMobilePhone::KUstCBMI),
	SERVICE_ENTRY(RMobilePhone::KUstCBMIRanges)
	};

SEnumFlag KUstServices17to24[] =
	{
	SERVICE_ENTRY(RMobilePhone::KUstGID1),
	SERVICE_ENTRY(RMobilePhone::KUstGID2),
	SERVICE_ENTRY(RMobilePhone::KUstSPN),
	SERVICE_ENTRY(RMobilePhone::KUstPLMNSelAccessTech),
	SERVICE_ENTRY(RMobilePhone::KUstMSISDN),
	SERVICE_ENTRY(RMobilePhone::KUstIMG),
	SERVICE_ENTRY(RMobilePhone::KUsteMLPP)
	};

SEnumFlag KUstServices25to32[] =
	{
	SERVICE_ENTRY(RMobilePhone::KUstAnswereMLPP),
	SERVICE_ENTRY(RMobilePhone::KUstGSMAccess),
	SERVICE_ENTRY(RMobilePhone::KUstSMSPpDataDownload),
	SERVICE_ENTRY(RMobilePhone::KUstSMSCbDataDownload),
	SERVICE_ENTRY(RMobilePhone::KUstCallControl),
	SERVICE_ENTRY(RMobilePhone::KUstMoSmControl),
	SERVICE_ENTRY(RMobilePhone::KUstPCmdRunAtCommand)
	};

SEnumFlag KUstServices33to40[] =
	{
	SERVICE_ENTRY(RMobilePhone::KUstAlways),
	SERVICE_ENTRY(RMobilePhone::KUstEST),
	SERVICE_ENTRY(RMobilePhone::KUstACL),
	SERVICE_ENTRY(RMobilePhone::KIstDepersonalisationKeys),
	SERVICE_ENTRY(RMobilePhone::KUstCooperativeNetwork),
	SERVICE_ENTRY(RMobilePhone::KUstGSMSecurityContext),
	SERVICE_ENTRY(RMobilePhone::KUstCPBCCHInfo),
	SERVICE_ENTRY(RMobilePhone::KUstInvestigationScan)
	};

SEnumFlag KUstServices41to48[] =
	{
	SERVICE_ENTRY(RMobilePhone::KUstMExE),
	SERVICE_ENTRY(RMobilePhone::KUstOperatorPLMNSelector),
	SERVICE_ENTRY(RMobilePhone::KUstHPLMNSelAccessTech),
	SERVICE_ENTRY(RMobilePhone::KUstExt5),
	SERVICE_ENTRY(RMobilePhone::KUstPLMNNetworkName),
	SERVICE_ENTRY(RMobilePhone::KUstOperatorPLMNList),
	SERVICE_ENTRY(RMobilePhone::KUstMDN),
	SERVICE_ENTRY(RMobilePhone::KUstMWISo)
	};

SEnumFlag KUstServices49to56[] =
	{
	SERVICE_ENTRY(RMobilePhone::KUstCFIS),
	SERVICE_ENTRY(RMobilePhone::KUstRPLMN),
	SERVICE_ENTRY(RMobilePhone::KUstSPDI),
	SERVICE_ENTRY(RMobilePhone::KUstMMS),
	SERVICE_ENTRY(RMobilePhone::KUstExt8),
	SERVICE_ENTRY(RMobilePhone::KUstCConGPRS),
	SERVICE_ENTRY(RMobilePhone::KUstMMSUCP)
	};

#if defined (FSHELL_PLATFORM_S60) && FSHELL_PLATFORM_S60 >= 5
SEnumFlag KUstServices57to64[] =
	{
	SERVICE_ENTRY(RMobilePhone::KUstVGCSGroupIdList),
	SERVICE_ENTRY(RMobilePhone::KUstVBSGroupIdList),
	SERVICE_ENTRY(RMobilePhone::KUstPseudonym),
	SERVICE_ENTRY(RMobilePhone::KUstUserPLMNSelectorWLAN),
	SERVICE_ENTRY(RMobilePhone::KUstOperatorPLMNSelectorWLAN),
	SERVICE_ENTRY(RMobilePhone::KUstUserWSIDList),
	SERVICE_ENTRY(RMobilePhone::KUstOperatorWSIDList),
	SERVICE_ENTRY(RMobilePhone::KUstVGCSSecurity)
	};
#endif

SEnumFlag KEstServices1to8[] =
	{
	SERVICE_ENTRY(RMobilePhone::KEstFDN),
	SERVICE_ENTRY(RMobilePhone::KEstBDN),
	SERVICE_ENTRY(RMobilePhone::KEstACL)
	};

#if defined (FSHELL_PLATFORM_S60) && FSHELL_PLATFORM_S60 >= 5
SEnumFlag KIstServices1to8[] =
	{
	SERVICE_ENTRY(RMobilePhone::KIstPCSCF),
	SERVICE_ENTRY(RMobilePhone::KIstGBA),
	SERVICE_ENTRY(RMobilePhone::KIstHTTPDigest)
	};
#endif


using namespace IoUtils;

class CCmdIcc : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdIcc();
private:
	CCmdIcc();
	void OpenPhoneL();
	void GetPhoneBookInfoL();
	void ApiSupported(TInt aApi, TBool& aIsSupported);
	void DisplayAccessCapsL();
	void DisplayServiceTablesL();
	void DisplayFdnStatusL();
	void DisplayPhoneBookInfoL();
	void DumpPhoneBookL();
	void EditPhoneBookL();
	void PrintServices(const TDesC& iTableName, TUint aServices, SEnumFlag aServiceTableDefinitions[], TInt aNumServiceTableDefinitions);
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	enum TOperation
		{
		EAccessCaps,
		EServiceTables,
		EFdnStatus,
		EPhoneBookInfo,
		EDumpPhoneBook,
		EEditPhoneBook
		};
private:
	RTelServer iEtelServer;
	RMobilePhone iPhone;
	RMobilePhoneBookStore iPhoneBookStore;
	TBool iVerbose;
	HBufC* iTsyName;
	TUint iServiceTable;
	HBufC* iStoreName;
	HBufC* iMode;
	TInt iSlot;
	HBufC* iData;
	TOperation iOperation;
	TBool iV5Tsy;
	TBool iV2Tsy;
	TBool iUsimTsy;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV5 iStoreInfo;
	};


CCommandBase* CCmdIcc::NewLC()
	{
	CCmdIcc* self = new(ELeave) CCmdIcc();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdIcc::~CCmdIcc()
	{
	iPhoneBookStore.Close();
	iPhone.Close();
	iEtelServer.Close();
	delete iTsyName;
	delete iStoreName;
	delete iMode;
	delete iData;
	}

CCmdIcc::CCmdIcc()
	{
	}

const TDesC& CCmdIcc::Name() const
	{
	_LIT(KName, "icc");	
	return KName;
	}

void CCmdIcc::DoRunL()
	{
	OpenPhoneL();

	switch (iOperation)
		{
		case EAccessCaps:
			DisplayAccessCapsL();
			break;
		case EServiceTables:
			DisplayServiceTablesL();
			break;
		case EFdnStatus:
			DisplayFdnStatusL();
			break;
		case EPhoneBookInfo:
			DisplayPhoneBookInfoL();
			break;
		case EDumpPhoneBook:
			DumpPhoneBookL();
			break;
		case EEditPhoneBook:
			EditPhoneBookL();
			break;
		default:
			User::Leave(KErrArgument);
		}
	}

void CCmdIcc::DisplayAccessCapsL()
	{
	TUint32 iccCaps;
	LeaveIfErr(iPhone.GetIccAccessCaps(iccCaps), _L("Couldn't get ICC access capabilities"));

	TBool first(ETrue);
	if (iccCaps & RMobilePhone::KCapsSimAccessSupported)
		{
		if (first)
			{
			first = EFalse;
			}
		else
			{
			Write(_L(" | "));
			}
		Write(_L("KCapsSimAccessSupported"));
		}
	if (iccCaps & RMobilePhone::KCapsRUimAccessSupported)
		{
		if (first)
			{
			first = EFalse;
			}
		else
			{
			Write(_L(" | "));
			}
		Write(_L("KCapsRUimAccessSupported"));
		}
	if (iccCaps & RMobilePhone::KCapsUSimAccessSupported)
		{
		if (first)
			{
			first = EFalse;
			}
		else
			{
			Write(_L(" | "));
			}
		Write(_L("KCapsUSimAccessSupported"));
		}
	if (!first)
		{
		Write(_L("\r\n"));
		}
	}
	
void CCmdIcc::PrintServices(const TDesC& iTableName, TUint aServices, SEnumFlag aServiceTableDefinitions[], TInt aNumServiceTableDefinitions)
	{
	Printf(_L("%S "), &iTableName);
	TBool first(ETrue);
	for (TInt i = 0; i < aNumServiceTableDefinitions; ++i)
		{
		if (aServices & aServiceTableDefinitions[i].iValue)
			{
			if (first)
				{
				first = EFalse;
				}
			else
				{
				Printf(_L(" | "));
				}
			TPtrC8 identifier((const TUint8*)aServiceTableDefinitions[i].iIndentifer);
			identifier.Set(identifier.Mid(identifier.Find(_L8("::")) + 2));
			Printf(identifier);
			}
		}
	Write(_L("\r\n"));
	}

void CCmdIcc::DisplayServiceTablesL()
	{
	RMobilePhone::TMobilePhoneServiceTableV2 serviceTable;
	TRequestStatus status;

	if (iV2Tsy)
		{
		RMobilePhone::TMobilePhoneServiceTableV2Pckg serviceTablePckg(serviceTable);
		iPhone.GetServiceTable(status, (RMobilePhone::TMobilePhoneServiceTable)iServiceTable, serviceTablePckg);
		}
	else
		{
		RMobilePhone::TMobilePhoneServiceTableV1Pckg serviceTablePckg(serviceTable);
		iPhone.GetServiceTable(status, (RMobilePhone::TMobilePhoneServiceTable)iServiceTable, serviceTablePckg);
		}
	
	User::WaitForRequest(status);
	LeaveIfErr(status.Int(), _L("Unable to get service table %d"), iServiceTable);

	switch (iServiceTable)
		{
		case RMobilePhone::ESIMServiceTable:
			PrintServices(_L("1 to 8:  "), serviceTable.iServices1To8, KSstServices1to8, sizeof(KSstServices1to8) / sizeof(SEnumFlag));
			PrintServices(_L("9 to 16: "), serviceTable.iServices9To16, KSstServices9to16, sizeof(KSstServices9to16) / sizeof(SEnumFlag));
			PrintServices(_L("17 to 24:"), serviceTable.iServices17To24, KSstServices17to24, sizeof(KSstServices17to24) / sizeof(SEnumFlag));
			PrintServices(_L("25 to 32:"), serviceTable.iServices25To32, KSstServices25to32, sizeof(KSstServices25to32) / sizeof(SEnumFlag));
			PrintServices(_L("33 to 40:"), serviceTable.iServices33To40, KSstServices33to40, sizeof(KSstServices33to40) / sizeof(SEnumFlag));
			PrintServices(_L("41 to 48:"), serviceTable.iServices41To48, KSstServices41to48, sizeof(KSstServices41to48) / sizeof(SEnumFlag));
			PrintServices(_L("49 to 56:"), serviceTable.iServices49To56, KSstServices49to56, sizeof(KSstServices49to56) / sizeof(SEnumFlag));
			break;
		case RMobilePhone::EUSIMServiceTable:
			PrintServices(_L("1 to 8:  "), serviceTable.iServices1To8, KUstServices1to8, sizeof(KUstServices1to8) / sizeof(SEnumFlag));
			PrintServices(_L("9 to 16: "), serviceTable.iServices9To16, KUstServices9to16, sizeof(KUstServices9to16) / sizeof(SEnumFlag));
			PrintServices(_L("17 to 24:"), serviceTable.iServices17To24, KUstServices17to24, sizeof(KUstServices17to24) / sizeof(SEnumFlag));
			PrintServices(_L("25 to 32:"), serviceTable.iServices25To32, KUstServices25to32, sizeof(KUstServices25to32) / sizeof(SEnumFlag));
			PrintServices(_L("33 to 40:"), serviceTable.iServices33To40, KUstServices33to40, sizeof(KUstServices33to40) / sizeof(SEnumFlag));
			PrintServices(_L("41 to 48:"), serviceTable.iServices41To48, KUstServices41to48, sizeof(KUstServices41to48) / sizeof(SEnumFlag));
			PrintServices(_L("49 to 56:"), serviceTable.iServices49To56, KUstServices49to56, sizeof(KUstServices49to56) / sizeof(SEnumFlag));
			break;
		case RMobilePhone::ECDMAServiceTable:
			break;
		case RMobilePhone::EUSIMEnabledServiceTable:
			PrintServices(_L("1 to 8:  "), serviceTable.iServices1To8, KEstServices1to8, sizeof(KEstServices1to8) / sizeof(SEnumFlag));
			break;
#if defined (FSHELL_PLATFORM_S60) && FSHELL_PLATFORM_S60 >= 5
		case RMobilePhone::EISIMServiceTable:
			PrintServices(_L("1 to 8:  "), serviceTable.iServices1To8, KIstServices1to8, sizeof(KIstServices1to8) / sizeof(SEnumFlag));
			break;
#endif
		}

	if (iV2Tsy)
		{
		Write(_L("AID:\r\n"));
		LtkUtils::HexDumpToOutput(serviceTable.iAID, Stdout());
		}
	}

void CCmdIcc::DisplayFdnStatusL()
	{
	RMobilePhone::TMobilePhoneFdnStatus fdnStatus;
	LeaveIfErr(iPhone.GetFdnStatus(fdnStatus), _L("Unable to get FDN status"));
	switch (fdnStatus)
		{
		case RMobilePhone::EFdnNotActive:
			Write(_L("EFdnNotActive\r\n"));
			break;
		case RMobilePhone::EFdnActive:
			Write(_L("EFdnActive\r\n"));
			break;
		case RMobilePhone::EFdnPermanentlyActive:
			Write(_L("EFdnPermanentlyActive\r\n"));
			break;
		case RMobilePhone::EFdnNotSupported:
			Write(_L("EFdnNotSupported\r\n"));
			break;
		case RMobilePhone::EFdnUnknown:
		default:
			Write(_L("EFdnUnknown\r\n"));
			break;
		}
	}

void CCmdIcc::DisplayPhoneBookInfoL()
	{
	if (iStoreName == NULL)
		{
		LeaveIfErr(KErrArgument, _L("The name of the phone store must be specified using -s"));
		}

	if (iVerbose)
		{
		Printf(_L("Getting phone store info for phone book \"%S\"...\r\n"), iStoreName);
		}

	GetPhoneBookInfoL();

	Printf(_L("Total number of entries:                 %d\r\n"), iStoreInfo.iTotalEntries);
	Printf(_L("Used number of entries:                  %d\r\n"), iStoreInfo.iUsedEntries);
	Printf(_L("Capabilities:                            0x%08x\r\n"), iStoreInfo.iCaps);
	Printf(_L("Phone name:                              %S\r\n"), &iStoreInfo.iName);
	Printf(_L("Maximum number length:                   %d\r\n"), iStoreInfo.iMaxNumLength);
	Printf(_L("Maximum text length:                     %d\r\n"), iStoreInfo.iMaxTextLength);
	Write( _L("Store location:                          "));
	switch (iStoreInfo.iLocation)
		{
		default:
		case RMobilePhoneBookStore::ELocationUnknown:
			Write(_L("ELocationUnknown"));
			break;
		case RMobilePhoneBookStore::ELocationIccMemory:
			Write(_L("ELocationIccMemory"));
			break;
		case RMobilePhoneBookStore::ELocationPhoneMemory:
			Write(_L("ELocationPhoneMemory"));
			break;
		case RMobilePhoneBookStore::ELocationExternalMemory:
			Write(_L("ELocationExternalMemory"));
			break;
		case RMobilePhoneBookStore::ELocationCombinedMemory:
			Write(_L("ELocationCombinedMemory"));
			break;
		}
	Write(_L("\r\n"));
	Printf(_L("Change counter:                          %d\r\n"), iStoreInfo.iChangeCounter);
	Printf(_L8("Phone book identity:                     %S\r\n"), &iStoreInfo.iIdentity);

	if (iV2Tsy)
		{
		Printf(_L("Mode:                                    %S\r\n"), &iStoreInfo.iPhBkMode);
		}

	if (iV5Tsy)
		{
		Printf(_L("Maximum second names:                    %d\r\n"), iStoreInfo.iMaxSecondNames);
		Printf(_L("Maximum text length second names:        %d\r\n"), iStoreInfo.iMaxTextLengthSecondName);
		Printf(_L("Maximum additional numbers:              %d\r\n"), iStoreInfo.iMaxAdditionalNumbers);
		Printf(_L("Maximum number length additional number: %d\r\n"), iStoreInfo.iMaxNumLengthAdditionalNumber);
		Printf(_L("Maximum text length additional number:   %d\r\n"), iStoreInfo.iMaxTextLengthAdditionalNumber);
		Printf(_L("Maximum group names:                     %d\r\n"), iStoreInfo.iMaxGroupNames);
		Printf(_L("Maximum text length group name:          %d\r\n"), iStoreInfo.iMaxTextLengthGroupName);
		Printf(_L("Maximum email address:                   %d\r\n"), iStoreInfo.iMaxEmailAddr);
		Printf(_L("Maximum text length email address:       %d\r\n"), iStoreInfo.iMaxTextLengthEmailAddr);
		}
	}

void CCmdIcc::DumpPhoneBookL()
	{
	if (iStoreName == NULL)
		{
		LeaveIfErr(KErrArgument, _L("The name of the phone store must be specified using -s"));
		}

	if (iVerbose)
		{
		Printf(_L("Dumping phone book store \"%S\"...\r\n"), iStoreName);
		}

	GetPhoneBookInfoL();

	if (iVerbose)
		{
		Printf(_L("Phone book store has %d slots\r\n"), iStoreInfo.iTotalEntries);
		}

	for (TInt slotIndex = 0; slotIndex < iStoreInfo.iTotalEntries; ++slotIndex)
		{
		if (iVerbose)
			{
			Printf(_L("Reading slot %d...\r\n"), slotIndex);
			}

		TBuf8<1024> buf;
		TRequestStatus status;
		iPhoneBookStore.Read(status, slotIndex, 1, buf);
		User::WaitForRequest(status);
		if (status.Int() != KErrNone)
			{
			Printf(_L8("%d: %d\r\n"), slotIndex, status.Int());
			}
		else
			{
			Printf(_L8("%d:\r\n"), slotIndex);
			LtkUtils::HexDumpToOutput(buf, Stdout());
			Write(_L("\r\n"));
			}
		}
	}

void CCmdIcc::EditPhoneBookL()
	{
	if (iStoreName == NULL)
		{
		LeaveIfErr(KErrArgument, _L("The name of the phone store must be specified using -s"));
		}

	if (iData == NULL)
		{
		LeaveIfErr(KErrArgument, _L("The data to be entered must be specified using -d"));
		}

	if (!iOptions.IsPresent(&iSlot))
		{
		LeaveIfErr(KErrArgument, _L("The slot to be edited must be specified using -l"));
		}

	GetPhoneBookInfoL();
	TRequestStatus status;
	TPtrC8 data((const TUint8*)iData->Ptr(), iData->Size());

	if (iVerbose)
		{
		Printf(_L("Writing the following to slot %d:\r\n"), iSlot);
		LtkUtils::HexDumpToOutput(data, Stdout());
		}

	iPhoneBookStore.Write(status, data, iSlot);
	User::WaitForRequest(status);
	LeaveIfErr(status.Int(), _L("Unable to write to phone book store \"%S\""), iStoreName);
	Printf(_L("Slot %d edited\r\n"), iSlot);
	}

void CCmdIcc::ApiSupported(TInt aApi, TBool& aIsSupported)
	{
	TInt err = iEtelServer.IsSupportedByModule(*iTsyName, aApi, aIsSupported);
	if (err != KErrNone)
		{
		aIsSupported = EFalse;
		}
	}

void CCmdIcc::GetPhoneBookInfoL()
	{
	if (iMode)
		{
		LeaveIfErr(iPhoneBookStore.Open(iPhone, *iStoreName, *iMode), _L("Unable to open phone book store \"%S\" in mode \"%S\""), iStoreName, iMode);
		}
	else
		{
		LeaveIfErr(iPhoneBookStore.Open(iPhone, *iStoreName), _L("Unable to open phone book store \"%S\""), iStoreName);
		}

	TRequestStatus status;
	if (iV5Tsy)
		{
		TPckg<RMobilePhoneBookStore::TMobilePhoneBookInfoV5> pckg(iStoreInfo);
		iPhoneBookStore.GetInfo(status, pckg);
		}
	else if (iV2Tsy)
		{
		TPckg<RMobilePhoneBookStore::TMobilePhoneBookInfoV2> pckg(iStoreInfo);
		iPhoneBookStore.GetInfo(status, pckg);
		}
	else
		{
		TPckg<RMobilePhoneBookStore::TMobilePhoneBookInfoV1> pckg(iStoreInfo);
		iPhoneBookStore.GetInfo(status, pckg);
		}
	User::WaitForRequest(status);
	LeaveIfErr(status.Int(), _L("Couldn't get phone book store info"));
	}

void CCmdIcc::OpenPhoneL()
	{
	if (iTsyName == NULL)
		{
		CommsDat::CMDBSession* db = CommsDat::CMDBSession::NewLC(KCDLatestVersion);
		CommsDat::CMDBField<TUint32>* globalSettingField = new(ELeave) CommsDat::CMDBField<TUint32>(CommsDat::KCDTIdModemPhoneServicesSMS);
		CleanupStack::PushL(globalSettingField);
		globalSettingField->SetRecordId(1);
		globalSettingField->LoadL(*db);
		TUint32 modemId = *globalSettingField;
		CommsDat::CMDBField<TDesC>* tsyField = new(ELeave) CommsDat::CMDBField<TDesC>(CommsDat::KCDTIdTsyName);
		CleanupStack::PushL(tsyField);
		tsyField->SetRecordId(modemId);
		TRAPL(tsyField->LoadL(*db), _L("Unable to read default TSY name"));
		TBuf<KCommsDbSvrMaxFieldLength> tsyName;
		tsyName = *tsyField;
		iTsyName = tsyName.AllocL();
		CleanupStack::PopAndDestroy(3, db); // db, tsyField & globalSettingField
		}
	
	LeaveIfErr(iEtelServer.Connect(), _L("Unable to connect to the ETel server"));
	LeaveIfErr(iEtelServer.LoadPhoneModule(*iTsyName), _L("Unable to load TSY module \"%S\""), iTsyName);
	TInt numPhones = 0;
	LeaveIfErr(iEtelServer.EnumeratePhones(numPhones), _L("Couldn't enumerate phones"));
	TBool matchFound(EFalse);
	TInt phoneIndex;
	for (phoneIndex = 0; phoneIndex < numPhones; ++phoneIndex)
		{
		TName thisTsyName;
		if ((iEtelServer.GetTsyName(phoneIndex, thisTsyName) == KErrNone) && (iTsyName->CompareF(thisTsyName) == KErrNone))
			{
			matchFound = ETrue;
			break;
			}
		}
	if (!matchFound)
		{
		LeaveIfErr(KErrNotFound, _L("Couldn't find phone using TSY \"%S\""), iTsyName);
		}
	RTelServer::TPhoneInfo phoneInfo;
	LeaveIfErr(iEtelServer.GetPhoneInfo(phoneIndex, phoneInfo), _L("Couldn't get phone info"));
	LeaveIfErr(iPhone.Open(iEtelServer, phoneInfo.iName), _L("Couldn't open phone"));

	ApiSupported(KEtelExtMultimodeV5, iV5Tsy);
	ApiSupported(KETelExtMultimodeV2, iV2Tsy);
	ApiSupported(KEtelFuncMobileUSIMApplications, iUsimTsy);

	if (iVerbose)
		{
		if (iUsimTsy)
			{
			Write(_L("TSY supports USIM API\r\n"));
			}
		if (iV2Tsy)
			{
			Write(_L("TSY supports V2 API\r\n"));
			}
		if (iV5Tsy)
			{
			Write(_L("TSY supports V5 API\r\n"));
			}
		}
	}

void CCmdIcc::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KOperationName, "operation");
	aArguments.AppendEnumL((TInt&)iOperation, KOperationName);
	}

void CCmdIcc::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptVerbose, "verbose");
	aOptions.AppendBoolL(iVerbose, KOptVerbose);

	_LIT(KOptTsyName, "tsy-name");
	aOptions.AppendStringL(iTsyName, KOptTsyName);

	_LIT(KArgServiceTable, "service-table");
	aOptions.AppendUintL(iServiceTable, KArgServiceTable);

	_LIT(KArgStoreName, "store-name");
	aOptions.AppendStringL(iStoreName, KArgStoreName);

	_LIT(KArgMode, "mode");
	aOptions.AppendStringL(iMode, KArgMode);

	_LIT(KArgSlot, "slot");
	aOptions.AppendIntL(iSlot, KArgSlot);

	_LIT(KArgData, "data");
	aOptions.AppendStringL(iData, KArgData);
	}


EXE_BOILER_PLATE(CCmdIcc)

