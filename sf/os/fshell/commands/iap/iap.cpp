// iap.cpp
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <commsdattypesv1_1.h>
#include <fshell/ioutils.h>

using namespace CommsDat;
using namespace IoUtils;

class CCmdIap : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdIap();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	CCmdIap();
	void ListIapsL();
	void AddWinsockprtIapL();
	void DoAddWinsockprtIapL(CMDBSession& aDbSession);
private:
	TBool iAddWinsockprtIap;
	};

	
CCommandBase* CCmdIap::NewLC()
	{
	CCmdIap* self = new (ELeave) CCmdIap();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdIap::~CCmdIap()
	{
	}
	
CCmdIap::CCmdIap()
	{
	}

const TDesC& CCmdIap::Name() const
	{
	_LIT(KName, "iap");
	return KName;
	}

void CCmdIap::DoRunL()
	{
	if (iAddWinsockprtIap)
		{
		AddWinsockprtIapL();
		}
	else
		{
		ListIapsL();
		}
	}

void CCmdIap::ListIapsL()
	{
	CMDBSession* session = CMDBSession::NewL(CMDBSession::LatestVersion());
	CleanupStack::PushL(session);
	CMDBRecordSet<CCDIAPRecord>* iapRecordSet = new (ELeave) CMDBRecordSet<CCDIAPRecord>(KCDTIdIAPRecord);
	CleanupStack::PushL(iapRecordSet);
	iapRecordSet->LoadL(*session);
	const TInt maxCount = iapRecordSet->iRecords.Count();
	if (maxCount == 0)
		{
		Printf(_L("No IAP records present.\r\n"));
		}
	else
		{
		IoUtils::CTextFormatter* formatter = CTextFormatter::NewLC(Stdout());
		IoUtils::CTextBuffer* buf = IoUtils::CTextBuffer::NewLC(0x100);
		buf->AppendFormatL(_L("Name\tType\tIdentifier\r\n"));
		for (TInt count = 0 ; count < maxCount ; count++)
			{
			// print to stdout details on this particular IAP record
			CCDIAPRecord& rec = *((*iapRecordSet)[count]);
			TPtrC ptr(static_cast<const TDesC&>(rec.iRecordName));
			TPtrC iapServiceType(rec.iServiceType);
			buf->AppendFormatL(_L("%S\t%S\t%d\r\n"), &ptr, &iapServiceType, rec.RecordId());
			}
		formatter->TabulateL(0, 2, buf->Descriptor());
		Write(formatter->Descriptor());
		CleanupStack::PopAndDestroy(2, formatter);
		}
	CleanupStack::PopAndDestroy(2); // iaprecordSet, session
	}

void CCmdIap::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptWinsockprt, "winsockprt");
	aOptions.AppendBoolL(iAddWinsockprtIap, KOptWinsockprt);
	}

void CCmdIap::AddWinsockprtIapL()
	{
	CMDBSession* session = CMDBSession::NewL(CMDBSession::LatestVersion());
	CleanupStack::PushL(session);
	session->OpenTransactionL();

	TRAPD(err, DoAddWinsockprtIapL(*session));

	if (err == KErrNone)
		{
		session->CommitTransactionL();
		}
	else
		{
		PrintError(err, _L("Unable to add WinSockPrt IAP, rolling back..."));
		session->RollbackTransactionL();
		}

	CleanupStack::PopAndDestroy(session);
	}

void CCmdIap::DoAddWinsockprtIapL(CMDBSession& aDbSession)
	{
	_LIT(KIapName, "WinSockPrt");
	_LIT(KIspType, "DialOutISP");
	_LIT(KNifName, "nullnif");
	_LIT(KAgentName, "nullagt.agt");
	_LIT(KBearerType, "ModemBearer");
	_LIT(KDescription, "A dummy IAP for use with WinSockPrt.");
	_LIT(KNetwork, "ip");

	CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(iapRecord);
	iapRecord->iRecordName.SetL(KIapName);
	if (iapRecord->FindL(aDbSession))
		{
		iapRecord->DeleteL(aDbSession);
		}

	CCDDialOutISPRecord* ispRecord = static_cast<CCDDialOutISPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdDialOutISPRecord));
	CleanupStack::PushL(ispRecord);
	ispRecord->iRecordName.SetL(KIapName);
	if (ispRecord->FindL(aDbSession))
		{
		ispRecord->DeleteL(aDbSession);
		}
	ispRecord->SetRecordId(KCDNewRecordRequest);
	ispRecord->iDescription.SetL(KDescription);
	ispRecord->iUseLoginScript = EFalse;
	ispRecord->iPromptForLogin = EFalse;
	ispRecord->iIfNetworks.SetL(KNetwork);
	ispRecord->StoreL(aDbSession);

	CCDModemBearerRecord* bearerRecord = static_cast<CCDModemBearerRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdModemBearerRecord));
	CleanupStack::PushL(bearerRecord);
	bearerRecord->iRecordName.SetL(KIapName);
	if (bearerRecord->FindL(aDbSession))
		{
		bearerRecord->DeleteL(aDbSession);
		}
	bearerRecord->SetRecordId(KCDNewRecordRequest);
	bearerRecord->iBearerAgent.SetL(KAgentName);
	bearerRecord->iNifName.SetL(KNifName);
	bearerRecord->StoreL(aDbSession);

	CCDNetworkRecord* networkRecord = static_cast<CCDNetworkRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdNetworkRecord));
	CleanupStack::PushL(networkRecord);
	networkRecord->iRecordName.SetL(KIapName);
	if (networkRecord->FindL(aDbSession))
		{
		networkRecord->DeleteL(aDbSession);
		}
	networkRecord->SetRecordId(KCDNewRecordRequest);
	networkRecord->StoreL(aDbSession);

	iapRecord->SetRecordId(KCDNewRecordRequest);
	iapRecord->iRecordName.SetL(KIapName);
	iapRecord->iServiceType.SetL(KIspType);
	iapRecord->iService.SetL(ispRecord->RecordId());
	iapRecord->iBearerType.SetL(KBearerType);
	iapRecord->iBearer.SetL(bearerRecord->RecordId());
	iapRecord->iNetwork.SetL(networkRecord->RecordId());
	iapRecord->StoreL(aDbSession);

	CleanupStack::PopAndDestroy(4, iapRecord);
	}

EXE_BOILER_PLATE(CCmdIap)
