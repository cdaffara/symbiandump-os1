// nitz.cpp
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
#include <cdblen.h>

using namespace IoUtils;

class CCmdNitz : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdNitz();
private:
	CCmdNitz();
	void OpenPhoneL();
	void PrintNitzData();
	void QueueNotification();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
private: // From CActive.
	virtual void RunL();
	virtual void DoCancel();
private:
	RTelServer iEtelServer;
	RMobilePhone iPhone;
	RMobilePhone::TMobilePhoneNITZ iNitzData;
	TBool iFollow;
	HBufC* iTsyName;
	TBool iFirstPrint;
	};


CCommandBase* CCmdNitz::NewLC()
	{
	CCmdNitz* self = new(ELeave) CCmdNitz();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdNitz::~CCmdNitz()
	{
	delete iTsyName;
	iPhone.Close();
	iEtelServer.Close();
	}

CCmdNitz::CCmdNitz() : iFirstPrint(ETrue)
	{
	}

const TDesC& CCmdNitz::Name() const
	{
	_LIT(KName, "nitz");	
	return KName;
	}

void CCmdNitz::DoRunL()
	{
	OpenPhoneL();

	TInt err = iPhone.GetNITZInfo(iNitzData);
	if (err == KErrNone)
		{
		PrintNitzData();
		}
	else if (!iFollow)
		{
		LeaveIfErr(err, _L("Unable to get NITZ information"));
		}
	
	if (iFollow)
		{
		QueueNotification();
		}
	}

void CCmdNitz::RunL()
	{
	if (iStatus.Int() == KErrNone)
		{
		PrintNitzData();
		QueueNotification();
		}
	else
		{
		Complete(iStatus.Int());
		}
	}

void CCmdNitz::DoCancel()
	{
	}

void CCmdNitz::OpenPhoneL()
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
	}

void CCmdNitz::PrintNitzData()
	{
	if (iFirstPrint)
		{
		iFirstPrint = EFalse;
		}
	else
		{
		Write(_L("\r\n"));
		}

	if (iNitzData.iNitzFieldsUsed & RMobilePhone::KCapsTimeAvailable)
		{
		Printf(_L("Universal time: %02d/%02d/%04d %02d:%02d:%02d.%d\r\n"), iNitzData.Day(), iNitzData.Month() + 1, iNitzData.Year(), iNitzData.Hour(), iNitzData.Minute(), iNitzData.Second(), iNitzData.MicroSecond());
		}
	if (iNitzData.iNitzFieldsUsed & RMobilePhone::KCapsTimezoneAvailable)
		{
		Printf(_L("Time zone: %d (+/- 15 minute offsets from GMT)\r\n"), iNitzData.iTimeZone);
		}
	if (iNitzData.iNitzFieldsUsed & RMobilePhone::KCapsDSTAvailable)
		{
		Printf(_L("Daylight saving adjustment: %d (hours)\r\n"), iNitzData.iDST);
		}
	if (iNitzData.iNitzFieldsUsed & RMobilePhone::KCapsShortNameAvailable)
		{
		Printf(_L("Short network name: \"%S\"\r\n"), &iNitzData.iShortNetworkId);
		}
	if (iNitzData.iNitzFieldsUsed & RMobilePhone::KCapsLongNameAvailable)
		{
		Printf(_L("Long network name: \"%S\"\r\n"), &iNitzData.iLongNetworkId);
		}
	}

void CCmdNitz::QueueNotification()
	{
	iPhone.NotifyNITZInfoChange(iStatus, iNitzData);
	SetActive();
	}

void CCmdNitz::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptFollow, "follow");
	aOptions.AppendBoolL(iFollow, KOptFollow);

	_LIT(KOptTsyName, "tsy-name");
	aOptions.AppendStringL(iTsyName, KOptTsyName);
	}


EXE_BOILER_PLATE(CCmdNitz)

