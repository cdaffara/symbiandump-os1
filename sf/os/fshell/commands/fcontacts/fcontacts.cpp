// fcontacts.cpp
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

#include <fshell/ioutils.h>
#include <CNTDB.H>
#include <CNTITEM.H>
#include <CNTFLDST.H>

// Some things that v9.1 doesn't have
#include "fcontacts_support.h"

using namespace IoUtils;

class CCmdContacts : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdContacts();
private:
	CCmdContacts();
	
	TInt CheckDuplicateAndUpdate(CContactItemFieldSet &aFieldSet, CContactItemField &aField);	
	void FillContactFieldSetL(CContactItemFieldSet &aFieldSet, TUid aType);
	void PrintContact(CContactItem* aContact);
	
	void PrintGroupContentsL(CContactItem* aContactGroup, TInt aVerboseLevel);
	void PrintGroupsJoined(CContactItem* aContact, TInt aVerboseLevel);	
	void PrintTime(const TTime& aTime, TBool aNewline=EFalse);
	
	void PrintAcceptedOptions();	
	static const TDesC* UidName(TUid aUid);
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
	
	void DoListL();
	void DoAddL();
	void DoDeleteL();
	void DoChangeL();
	void DoDeleteAllL();
	void DoCreateGroupL();
	void DoAddToGroupL();		
	void DoCompactL();
private:
	
	CContactDatabase* iDb;
	RArray<TBool> iVerbose;
	TInt iOptContactId;
	TInt iOptDestGrpId;
	HBufC* iMatch;
	TBool iShowFirstPhoneOnly;
	
	HBufC* iOptFirstName;
	HBufC* iOptLastName;
	HBufC* iOptTelephone;
	HBufC* iOptMobile;
	TFileName2 iOptPhotoFileName;
	HBufC* iOptGroupName;
	
	HBufC* iOptStr;
	enum
		{
		EList, EAdd, EDelete, EChange, EDeleteAll, ECreateGroup, EAddToGroup, ECompact
		} iCommand;
	};


CCommandBase* CCmdContacts::NewLC()
	{
	CCmdContacts* self = new(ELeave) CCmdContacts();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdContacts::~CCmdContacts()
	{
	delete iOptFirstName;
	delete iOptLastName;
	delete iOptTelephone;
	delete iOptMobile;
	delete iOptGroupName;
	delete iOptStr;
	delete iMatch;
	delete iDb;
	iVerbose.Close();
	}

CCmdContacts::CCmdContacts()
	{
	}

const TDesC& CCmdContacts::Name() const
	{
	_LIT(KName, "fcontacts");
	return KName;
	}

void CCmdContacts::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendEnumL((TInt&)iCommand, _L("command"));
	}

void CCmdContacts::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendBoolL(iVerbose, _L("verbose"));
	aOptions.AppendUintL((TUint&)iOptContactId, _L("contact-id"));
	aOptions.AppendUintL((TUint&)iOptDestGrpId, _L("group-id"));
	aOptions.AppendStringL(iMatch, _L("match"));
	aOptions.AppendBoolL(iShowFirstPhoneOnly, _L("first-phone-only"));
	aOptions.AppendStringL(iOptFirstName, _L("first-name"));
	aOptions.AppendStringL(iOptLastName, _L("last-name"));
	aOptions.AppendStringL(iOptTelephone, _L("telephone"));
	aOptions.AppendStringL(iOptMobile, _L("mobile"));
	aOptions.AppendFileNameL(iOptPhotoFileName, _L("photo"));
	aOptions.AppendStringL(iOptGroupName, _L("group"));
	}

void CCmdContacts::DoRunL()
	{
	TRAPD(err, iDb = CContactDatabase::OpenL());
	LeaveIfErr(err, _L("Couldn't open contacts database"));
	
	switch (iCommand)
		{
	case EList:
		DoListL();
		break;
	case EAdd:
		DoAddL();
		break;
	case EDelete:
		DoDeleteL();
		break;
	case EChange:
		DoChangeL();
		break;
	case EDeleteAll:
		DoDeleteAllL();
		break;
	case ECreateGroup:
		DoCreateGroupL();
		break;
	case EAddToGroup:
		DoAddToGroupL();	
		break;
	case ECompact:
		DoCompactL();
		break;
		}
	}


void CCmdContacts::DoListL()
	{
	if (iOptContactId)
		{
		CContactItem* contact = NULL;
		TRAPD(err, contact = iDb->ReadContactL(iOptContactId));
		LeaveIfErr(err, _L("Couldn't read contact %d"), iOptContactId);
		PrintContact(contact);
		delete contact;
		}
	else
		{
		// List all
		TContactIter iter(*iDb);
		TContactItemId id;
		TBool foundMatch = EFalse;
		while ((id = iter.NextL()) != KNullContactId)
			{
			CContactItem* contact = iDb->ReadContactLC(id);
			TBool show = ETrue;
			if (iMatch)
				{
				show = EFalse;
				CContactItemFieldSet& fields = contact->CardFields();
				TInt num = fields.Count();
				for (TInt i = 0; i < num; i++)
					{
					CContactItemField& field = fields[i];
					switch (field.StorageType())
						{
						case KStorageTypeText:
							if (field.TextStorage()->Text().MatchF(*iMatch) != KErrNotFound)
								{
								show = ETrue;
								foundMatch = ETrue;
								}
							break;
						default:
							break;
						}
					if (show) break;
					}
				}
			if (show)
				{
				PrintContact(contact);
				}
			CleanupStack::PopAndDestroy(contact);
			if (show && iShowFirstPhoneOnly)
				{
				break;
				}
			}
		if (iMatch && !foundMatch)
			{
			User::Leave(KErrNotFound);
			}
		}
	}

void CCmdContacts::DoAddL()
	{
	TContactItemId newId = 0;
	//create a "contact card" contact item
	CContactItem* contact = CContactCard::NewLC();
	CContactItemFieldSet& fieldSet = contact->CardFields();
	FillContactFieldSetL(fieldSet, KUidContactCard);
			
	TRAPD(err, newId = iDb->AddNewContactL(*contact));
	LeaveIfErr(err, _L("Could not add this contact"));
	
	Printf(_L("Contact Added\r\nContact ID:%d\r\n"), newId);
	CleanupStack::PopAndDestroy(contact);
	}


void CCmdContacts::DoDeleteL()
	{
	if (iOptContactId)
		{
		TRAPD(err, iDb->DeleteContactL(iOptContactId));
		if (err == KErrNone)
			{
			Printf(_L("Contact %d deleted\r\n"), iOptContactId);
			}
		else
			{
			LeaveIfErr(err, _L("Could not detelet contact item %d"), iOptContactId);
			}
		}
	else
		{
		LeaveIfErr(KErrArgument, _L("No contact id specified. Use -i to specify a contact ID"));						
		}	
	}


void CCmdContacts::DoChangeL()
	{
	if (iOptContactId) 
		{
		PrintAcceptedOptions();		
		CContactItem* contact = NULL;
		TRAPD(err, contact = iDb->OpenContactL(iOptContactId));
		LeaveIfErr(err, _L("could not open contact item %d"), iOptContactId);
				
		CleanupStack::PushL(contact);
		
		CContactItemFieldSet& FieldSet = contact->CardFields();
		FillContactFieldSetL(FieldSet, KUidContactCard);
		
		TRAP(err, iDb->CommitContactL(*contact));
		LeaveIfErr(err, _L("Could not commit the change for item %d"), iOptContactId);
						
		CleanupStack::PopAndDestroy(contact);
		
		}
	else
		{
		LeaveIfErr(KErrArgument, _L("No contact id specified. Use -i to specify a contact ID"));
		}		
	}

void CCmdContacts::DoDeleteAllL()
	{
	TInt TotalErrors = 0;	//how many items are not deleted due to error 
	TInt ContactItemCnt = iDb->CountL();
	Printf(_L("Database contains %d items \r\n"), ContactItemCnt);
	// List all
	TContactIter iter(*iDb);
	TContactItemId id;
	while ((id = iter.NextL()) != KNullContactId)
		{
		Printf(_L("Deleting contact item %d ..."), id);
		TRAPD(err, iDb->DeleteContactL(id));
		
		if (err==KErrNone)
			Printf(_L("done \r\n"), id);
		else 
			{
			TotalErrors++;
			Printf(_L("fail, error code=%d \r\n"), err);
			}		
		}
	
	if (TotalErrors > 0)
		{
		LeaveIfErr(KErrGeneral, _L("%d items are not deleted due to error"), TotalErrors);
		}	
	}

void CCmdContacts::DoCreateGroupL()
	{
	Printf(_L("Creating a new group... \r\n"));
	CContactItem* contact = NULL;
	TRAPD(err, contact = iDb->CreateContactGroupL());
	LeaveIfErr(err, _L("Could not create contact group"));
	
	CleanupStack::PushL(contact);
	TContactItemId GroupId = contact->Id();
	CleanupStack::PopAndDestroy(contact);
	contact = NULL;
	
	///////////////////////////////////////////////////
	TRAP(err, contact = iDb->OpenContactL(GroupId));
	LeaveIfErr(err, _L("Could not open contact group for editing"));
	CleanupStack::PushL(contact);
	
	Printf(_L("Changing group name for contact item %d... \r\n"), GroupId);
	
	CContactItemFieldSet& FieldSet = contact->CardFields();
	FillContactFieldSetL(FieldSet, KUidContactGroup);
	TRAP(err, iDb->CommitContactL(*contact));
	LeaveIfErr(err, _L("Could not sumbit the change"));

	CleanupStack::PopAndDestroy(contact);

	}

void CCmdContacts::DoCompactL()
	{
	TRAPD(err, iDb->CompactL());
	LeaveIfErr(err, _L("Could not compact the database"));
	}

void CCmdContacts::DoAddToGroupL()
	{
	if (iOptContactId && iOptDestGrpId)
		{
		Printf(_L("Adding contact item %d to destination group item %d \r\n"), 
				iOptContactId, iOptDestGrpId);
		TRAPD(err, iDb->AddContactToGroupL( (TContactItemId)iOptContactId, (TContactItemId)iOptDestGrpId) ) ;
		
		//this is most common mistake, need to point out especially to remind the user.
		if (err==KErrNotSupported)
			PrintError(KErrNotSupported, _L("Destination is not a group"));
		
		User::LeaveIfError(err);
		
		}
	else 
		{
		LeaveIfErr(KErrArgument, _L("No contact id specified. Use -i to specify a contact ID"));						
		}	
	}


// FOR TEST PURPOSE
//to check if a field of the same type has already existed in a FieldSet.
//if yes, delete that field first and then add it
TInt CCmdContacts::CheckDuplicateAndUpdate(CContactItemFieldSet &aFieldSet, CContactItemField &aField) 
	{
	const CContentType &newContentType=aField.ContentType();
	TInt fieldPos=0;
	
	TInt FieldCnt = aFieldSet.Count();
	for(; fieldPos<FieldCnt ; fieldPos++ )
		{
		CContactItemField& TestedField = aFieldSet[fieldPos];
		if (newContentType == TestedField.ContentType())
			{
				aFieldSet.Remove(fieldPos);
				break;
			}
		}
	
	if (aField.Storage()->IsFull())
		{
		CContactItemField* field=CContactItemField::NewLC(aField);
		aFieldSet.InsertL(fieldPos,*field);
		CleanupStack::Pop();	// field
		}	
	
	return 0;
	}

//this function will retrieve infomation from command line arguments and fill them into 
//a field set, FieldSet doesn't have to be empty
//aType: either KUidContactCard or KUidContactGroup
void CCmdContacts::FillContactFieldSetL(CContactItemFieldSet &aFieldSet, TUid aType)
	{	
	if (aType == KUidContactGroup)
		{
		//group name
		if (iOptGroupName && iOptGroupName->Length())
			{		
			CContactItemField* field = NULL;
			field = CContactItemField::NewLC(KStorageTypeText);
			field->SetLabelL(_L("Group Label"));
			field->SetMapping(KUidContactFieldNone);
			field->AddFieldTypeL(KUidContactFieldTemplateLabel);	
			CContactTextField* pFieldText = field->TextStorage();
			pFieldText->SetTextL(*iOptGroupName);
			aFieldSet.UpdateFieldL(*field, 1);
			CleanupStack::PopAndDestroy(field);	
			}
		}
	
	//Synchronization, for both Group and normal contacts
		{
		CContactItemField* field = NULL;		
		field = CContactItemField::NewLC(KStorageTypeText);
		field->SetLabelL(_L("Synchronization"));
		field->SetMapping(KUidContactFieldVCardMapClass);
		field->AddFieldTypeL(KUidContactFieldClass);	
		CContactTextField* pFieldText = field->TextStorage();
		pFieldText->SetTextL(_L("private"));
		aFieldSet.UpdateFieldL(*field, 1);
		CleanupStack::PopAndDestroy(field);		
		}
		
	if (aType == KUidContactCard)
		{
		//last name
		if (iOptLastName && iOptLastName->Length())
			{		
			CContactItemField* field = NULL;
			field = CContactItemField::NewLC(KStorageTypeText);
			field->SetLabelL(_L("Last name"));
			field->SetMapping(KUidContactFieldVCardMapUnusedN);
			field->AddFieldTypeL(KUidContactFieldFamilyName);	
			CContactTextField* pFieldText = field->TextStorage();
			pFieldText->SetTextL(*iOptLastName);
			aFieldSet.UpdateFieldL(*field, 1);
			CleanupStack::PopAndDestroy(field);	
			}
		
		//first name
		if (iOptFirstName && iOptFirstName->Length())
			{	
			CContactItemField* field = NULL;
			field = CContactItemField::NewLC(KStorageTypeText);
			field->SetLabelL(_L("First name"));
			field->SetMapping(KUidContactFieldVCardMapUnusedN);
			field->AddFieldTypeL(KUidContactFieldGivenName);	
			CContactTextField* pFieldText = field->TextStorage();
			pFieldText->SetTextL(*iOptFirstName);
			aFieldSet.UpdateFieldL(*field, 1);
			CleanupStack::PopAndDestroy(field);	
			}			
		//general telephone
		if (iOptTelephone && iOptTelephone->Length())
			{	
			CContactItemField* field = NULL;
			field = CContactItemField::NewLC(KStorageTypeText);
			field->SetLabelL(_L("Telephone"));
			field->SetMapping(KUidContactFieldVCardMapTEL);
			field->AddFieldTypeL(KUidContactFieldPhoneNumber);	
			field->AddFieldTypeL(KUidContactFieldVCardMapVOICE);		
			CContactTextField* pFieldText = field->TextStorage();
			pFieldText->SetTextL(*iOptTelephone);
			aFieldSet.UpdateFieldL(*field, 1);
			CleanupStack::PopAndDestroy(field);	
			}			
		//mobile phone
		if (iOptMobile && iOptMobile->Length())
			{	
			CContactItemField* field = NULL;
			field = CContactItemField::NewLC(KStorageTypeText);
			field->SetLabelL(_L("Mobile"));
			field->SetMapping(KUidContactFieldVCardMapTEL);
			field->AddFieldTypeL(KUidContactFieldPhoneNumber);
			field->AddFieldTypeL(KUidContactFieldVCardMapCELL);
			CContactTextField* pFieldText = field->TextStorage();
			pFieldText->SetTextL(*iOptMobile);
			aFieldSet.UpdateFieldL(*field, 1);
			CleanupStack::PopAndDestroy(field);	
			}
		
		//photo
		if (iOptPhotoFileName.Length())
			{
			RFile file;
			LeaveIfErr(file.Open(FsL(), iOptPhotoFileName, EFileRead), _L("Couldn't open file \"%S\""), &iOptPhotoFileName);
			TInt fileSize;
			LeaveIfErr(file.Size(fileSize), _L("Couldn't read the size of file \"%S\""), &iOptPhotoFileName);
			RBuf8 buf;
			buf.Create(fileSize);
			buf.CleanupClosePushL();
			LeaveIfErr(file.Read(buf), _L("Couldn't read the contents of file \"%S\""), &iOptPhotoFileName);

			CContactItemField* field = CContactItemField::NewLC(KStorageTypeStore);
			field->SetLabelL(_L("Photo"));
			field->SetMapping(KUidContactFieldVCardMapPHOTO);
			field->AddFieldTypeL(KUidContactFieldPicture);
			CContactStoreField* storeField = field->StoreStorage();
			storeField->SetThingL(buf);
			aFieldSet.UpdateFieldL(*field, 1);

			CleanupStack::PopAndDestroy(2, &buf);	
			}
		}
	}

void CCmdContacts::PrintContact(CContactItem* aContact)
	{
	TContactItemId ContactId = aContact->Id();
	TUid ContactType = aContact->Type();
	CContactItemFieldSet& fields = aContact->CardFields();

	TBool verbose = (iVerbose.Count() > 0);
	TBool veryverbose = (iVerbose.Count() > 1);

	if (verbose)
		{
		Printf(_L("Contact item id %d:\r\n"), aContact->Id());
		Printf(_L("\tLast modified: "));
		PrintTime(aContact->LastModified(), ETrue);
		if (veryverbose)
			{
			Printf(_L("\tHidden=%d System=%d AccessCount=%d Deleted=%d\r\n"), aContact->IsHidden(), aContact->IsSystem(), aContact->AccessCount(), aContact->IsDeleted());
			TPtrC uid = aContact->UidStringL(iDb->MachineId()); // Don't understand the syntax of this function
			Printf(_L("\tUid: %S\r\n"), &uid);
			TUint32 type = aContact->Type().iUid;
			Printf(_L("\tType: 0x%08x "), type);
			switch (type)
				{
				case KUidContactCardValue:
					Printf(_L("(KUidContactCard)\r\n")); break;
				case KUidContactGroupValue:
					Printf(_L("(KUidContactGroup)\r\n")); break;
				case KUidContactOwnCardValue:
					Printf(_L("(KUidContactOwnCard)\r\n")); break;
				case KUidContactICCEntryValue:
					Printf(_L("(KUidContactICCEntry)\r\n")); break;
				case KUidContactTemplateValue:
					Printf(_L("(KUidContactTemplate)\r\n")); break;
				case KUidContactCardTemplateValue:
					Printf(_L("(KUidContactCardTemplate)\r\n")); break;
				default:
					Printf(_L("(Unknown)\r\n")); break;
				}
			}
		
		PrintGroupContentsL(aContact, iVerbose.Count());
		PrintGroupsJoined(aContact, iVerbose.Count());
		
		TInt num = fields.Count();
		for (TInt i = 0; i < num; i++)
			{
			CContactItemField& field = fields[i];
			TPtrC label = field.Label();
			Printf(_L("\t%d [%S]: "), field.Id(), &label);
			switch (field.StorageType())
				{
				case KStorageTypeText:
					{					
					TPtrC fieldText = field.TextStorage()->Text();
					Printf(_L("KStorageTypeText: %S\r\n"), &fieldText);
					break;
					}
				case KStorageTypeDateTime:
					{
					TTime time = field.DateTimeStorage()->Time();
					PrintTime(time, ETrue);
					break;
					}
				case KStorageTypeStore:
					{
					HBufC8* thing = field.StoreStorage()->Thing();
					if (thing)
						{
						Printf(_L("<Binary data of length %d>\r\n"), thing->Size());
						}
					else
						{
						Printf(_L("<Null binary data>\r\n"));
						}
					break;
					}
				case KStorageTypeContactItemId:
					{
					Printf(_L("<Contact item id %d>\r\n"), field.AgentStorage()->Value());
					break;
					}
				default:
					Printf(_L("<Unknown storage type>"));
					break;
				}
			if (veryverbose)
				{
				Printf(_L("\t\tDeleted=%d Disabled=%d Hidden=%d Private=%d Readonly=%d SpeedDial=%d\r\n"), field.IsDeleted(), field.IsDisabled(), field.IsHidden(), field.IsPrivate(), field.IsReadOnly(), field.IsSpeedDial());
				Printf(_L("\t\tUser flags:    0x%08x\r\n"), field.UserFlags());
				const CContentType& contentType = field.ContentType();
				Printf(_L("\t\tVCard mapping: 0x%08x (%S)\r\n"), contentType.Mapping(), UidName(contentType.Mapping()));
				TInt fieldTypeCount = contentType.FieldTypeCount();
				for (TInt i = 0; i < fieldTypeCount; i++)
					{
					TUid fieldType = contentType.FieldType(i);
					Printf(_L("\t\tField type %d:  0x%08x (%S)\r\n"), i, fieldType, UidName(fieldType));
					}
				}
			}
		}
	else
		{
		TInt found;
		if (ContactType == KUidContactGroup)
			{
			TPtrC GroupName;
			found = fields.Find(KUidContactFieldTemplateLabel, KUidContactFieldNone);
			if (found != KErrNotFound)
				GroupName.Set( fields[found].TextStorage()->Text() );
			else
				GroupName.Set(_L("[Unnamed]"));
			
			Printf(_L("%d. [Group] %S "), ContactId, &GroupName);
			
			//list containing items...
			PrintGroupContentsL(aContact, 0);
			}
		else
			{
			if (iShowFirstPhoneOnly)
				{
				found = fields.Find(KUidContactFieldPhoneNumber);
				if (found != KErrNotFound)
					{
					TPtrC phone = TPtrC(fields[found].TextStorage()->Text());
					Printf(_L("%S"), &phone);
					}
				}
			else
				{
				TPtrC first;
				found = fields.Find(KUidContactFieldGivenName);
				if (found != KErrNotFound) first.Set(fields[found].TextStorage()->Text());
				TPtrC last;
				found = fields.Find(KUidContactFieldFamilyName);
				if (found != KErrNotFound) last.Set(fields[found].TextStorage()->Text());
				TPtrC phone;
				found = fields.Find(KUidContactFieldPhoneNumber);
				if (found != KErrNotFound) phone.Set(fields[found].TextStorage()->Text());
				Printf(_L("%d. %S %S %S\r\n"), ContactId, &first, &last, &phone);			
				}
			}
		
		}
	}

//This function will not LEAVE for the moment
//aVerboseLevel: possible values: 0 1 2
//
void CCmdContacts::PrintGroupContentsL(CContactItem* aContactGroup, TInt /*aVerboseLevel*/)
	{
	if (aContactGroup->Type() == KUidContactGroup)
		{
		CContactGroup* pContactGroup = (CContactGroup*)aContactGroup;
		const CContactIdArray* pContactIdAry = pContactGroup->ItemsContained();	
		
		//
		TInt ElementCount = pContactIdAry->Count();
		Printf(_L("\tThis Group contains %d items."), ElementCount);
		if (ElementCount)
			{
			Printf(_L(" Member contact ID list: "));
			TInt i;
			for (i=0; i<ElementCount; i++)
				{
				TContactItemId ID = (*pContactIdAry) [i];
				Printf(_L("%d "), ID);
				}
			}
		Printf(_L("\r\n"));
		
		}	
	}

void CCmdContacts::PrintGroupsJoined(CContactItem* aContact, TInt aVerboseLevel)
	{
	if (aVerboseLevel < 1) return;
	TUid ContactType = aContact->Type();
	if (ContactType == KUidContactGroup || ContactType == KUidContactCard)
		{
		CContactItemPlusGroup * pContact = (CContactItemPlusGroup *)aContact;
		const CContactIdArray* pContactIdAry = pContact->GroupsJoined();
		
		if (!pContactIdAry) 
			return;
					
		TInt ElementCount = pContactIdAry->Count();
		Printf(_L("\tThis item belongs to %d groups."), ElementCount);
		if (ElementCount)
			{
			Printf(_L(" Group contact ID list: "));
			TInt i;
			for (i=0; i<ElementCount; i++)
				{
				TContactItemId ID = (*pContactIdAry) [i];
				Printf(_L("%d "), ID);
				}
			}
		Printf(_L("\r\n"));		
		}	
	}

void CCmdContacts::PrintTime(const TTime& aTime, TBool aNewline)
	{
	_LIT8(KDateTimeFormat, "%d-%02d-%02d %02d:%02d:%02d");
	TDateTime dt = aTime.DateTime();
	Printf(KDateTimeFormat, dt.Year(), dt.Month()+1, dt.Day()+1, dt.Hour(), dt.Minute(), dt.Second());
	if (aNewline) Printf(_L("\r\n"));
	}

//print the accepted options from the command line
void CCmdContacts::PrintAcceptedOptions()
	{
	//surname
	if (iOptLastName && iOptLastName->Length())
		Printf(_L("surname: %S \r\n"), iOptLastName );				
	//first name
	if (iOptFirstName && iOptFirstName->Length())
		Printf(_L("first name: %S \r\n"), iOptFirstName );					
	//general telephone
	if (iOptTelephone && iOptTelephone->Length())
		Printf(_L("general telephone: %S \r\n"), iOptTelephone );
	//mobile phone
	if (iOptMobile && iOptMobile->Length())
		Printf(_L("mobile: %S \r\n"), iOptMobile );
	//photo
	if (iOptPhotoFileName.Length())
		Printf(_L("photo: %S \r\n"), &iOptPhotoFileName );
	}


#define CASE_LIT(x) case x: { _LIT(KName, #x); return &KName; }
const TDesC* CCmdContacts::UidName(TUid aUid)
	{
	switch (aUid.iUid)
		{
		CASE_LIT(KUidContactFieldAddressValue)
		CASE_LIT(KUidContactFieldPostOfficeValue)
		CASE_LIT(KUidContactFieldExtendedAddressValue)
		CASE_LIT(KUidContactFieldLocalityValue)
		CASE_LIT(KUidContactFieldRegionValue)
		CASE_LIT(KUidContactFieldPostCodeValue)
		CASE_LIT(KUidContactFieldCountryValue)
		CASE_LIT(KUidContactFieldCompanyNameValue)
		CASE_LIT(KUidContactFieldCompanyNamePronunciationValue)
		CASE_LIT(KUidContactFieldPhoneNumberValue)
		CASE_LIT(KUidContactFieldGivenNameValue)
		CASE_LIT(KUidContactFieldFamilyNameValue)
		CASE_LIT(KUidContactFieldGivenNamePronunciationValue)
		CASE_LIT(KUidContactFieldFamilyNamePronunciationValue)
		CASE_LIT(KUidContactFieldAdditionalNameValue)
		CASE_LIT(KUidContactFieldSuffixNameValue)
		CASE_LIT(KUidContactFieldPrefixNameValue)
		CASE_LIT(KUidContactFieldHiddenValue)
		CASE_LIT(KUidContactFieldEMailValue)
		CASE_LIT(KUidContactFieldMsgValue)
		CASE_LIT(KUidContactFieldSmsValue)
		CASE_LIT(KUidContactFieldFaxValue)
		CASE_LIT(KUidContactFieldDefinedTextValue)
		CASE_LIT(KUidContactFieldNoteValue)
		CASE_LIT(KUidContactFieldBirthdayValue)
		CASE_LIT(KUidContactFieldUrlValue)
		CASE_LIT(KUidContactFieldStorageInlineValue)
		CASE_LIT(KUidContactFieldTemplateLabelValue)
		CASE_LIT(KUidContactFieldPictureValue)
		CASE_LIT(KUidContactFieldRingToneValue)
		CASE_LIT(KUidContactFieldDTMFValue)
		CASE_LIT(KUidContactsVoiceDialFieldValue)
		CASE_LIT(KUidContactFieldNoneValue)
		CASE_LIT(KUidContactFieldJobTitleValue)
		CASE_LIT(KUidContactFieldICCSlotValue)
		CASE_LIT(KUidContactFieldICCPhonebookValue)
		CASE_LIT(KUidContactFieldICCGroupValue)
		CASE_LIT(KUidContactFieldIMAddressValue)
		CASE_LIT(KUidContactFieldSecondNameValue)
		CASE_LIT(KUidContactFieldSIPIDValue)
		CASE_LIT(KUidContactFieldAssistantValue)
		CASE_LIT(KUidContactFieldAnniversaryValue)
		CASE_LIT(KUidContactFieldSpouseValue)
		CASE_LIT(KUidContactFieldChildrenValue)
		CASE_LIT(KUidContactFieldClassValue)
		CASE_LIT(KUidContactFieldDepartmentNameValue)
		CASE_LIT(KIntContactFieldVCardMapWORK)
		CASE_LIT(KIntContactFieldVCardMapHOME)
		CASE_LIT(KIntContactFieldVCardMapMSG)
		CASE_LIT(KIntContactFieldVCardMapVOICE)
		CASE_LIT(KIntContactFieldVCardMapFAX)
		CASE_LIT(KIntContactFieldVCardMapPREF)
		CASE_LIT(KIntContactFieldVCardMapCELL)
		CASE_LIT(KIntContactFieldVCardMapPAGER)
		CASE_LIT(KIntContactFieldVCardMapBBS)
		CASE_LIT(KIntContactFieldVCardMapMODEM)
		CASE_LIT(KIntContactFieldVCardMapCAR)
		CASE_LIT(KIntContactFieldVCardMapISDN)
		CASE_LIT(KIntContactFieldVCardMapVIDEO)
		CASE_LIT(KIntContactFieldVCardMapDOM)
		CASE_LIT(KIntContactFieldVCardMapADR)
		CASE_LIT(KIntContactFieldVCardMapPOSTOFFICE)
		CASE_LIT(KIntContactFieldVCardMapEXTENDEDADR)
		CASE_LIT(KIntContactFieldVCardMapLOCALITY)
		CASE_LIT(KIntContactFieldVCardMapREGION)
		CASE_LIT(KIntContactFieldVCardMapPOSTCODE)
		CASE_LIT(KIntContactFieldVCardMapCOUNTRY)
		CASE_LIT(KIntContactFieldVCardMapAGENT)
		CASE_LIT(KIntContactFieldVCardMapBDAY)
		CASE_LIT(KIntContactFieldVCardMapEMAILINTERNET)
		CASE_LIT(KIntContactFieldVCardMapGEO)
		CASE_LIT(KIntContactFieldVCardMapLABEL)
		CASE_LIT(KIntContactFieldVCardMapLOGO)
		CASE_LIT(KIntContactFieldVCardMapMAILER)
		CASE_LIT(KIntContactFieldVCardMapNOTE)
		CASE_LIT(KIntContactFieldVCardMapORG)
		CASE_LIT(KIntContactFieldVCardMapORGPronunciation)
		CASE_LIT(KIntContactFieldVCardMapPHOTO)
		CASE_LIT(KIntContactFieldVCardMapROLE)
		CASE_LIT(KIntContactFieldVCardMapSOUND)
		CASE_LIT(KIntContactFieldVCardMapTEL)
		CASE_LIT(KIntContactFieldVCardMapTELFAX)
		CASE_LIT(KIntContactFieldVCardMapTITLE)
		CASE_LIT(KIntContactFieldVCardMapURL)
		CASE_LIT(KIntContactFieldVCardMapUnusedN)
		CASE_LIT(KIntContactFieldVCardMapUnusedFN)
		CASE_LIT(KIntContactFieldVCardMapNotRequired)
		CASE_LIT(KIntContactFieldVCardMapUnknownXDash)
		CASE_LIT(KIntContactFieldVCardMapUnknown)
		CASE_LIT(KIntContactFieldVCardMapUID)
		CASE_LIT(KIntContactFieldVCardMapINTL)
		CASE_LIT(KIntContactFieldVCardMapPOSTAL)
		CASE_LIT(KIntContactFieldVCardMapPARCEL)
		CASE_LIT(KIntContactFieldVCardMapGIF)
		CASE_LIT(KIntContactFieldVCardMapCGM)
		CASE_LIT(KIntContactFieldVCardMapWMF)
		CASE_LIT(KIntContactFieldVCardMapBMP)
		CASE_LIT(KIntContactFieldVCardMapMET)
		CASE_LIT(KIntContactFieldVCardMapPMB)
		CASE_LIT(KIntContactFieldVCardMapDIB)
		CASE_LIT(KIntContactFieldVCardMapPICT)
		CASE_LIT(KIntContactFieldVCardMapTIFF)
		CASE_LIT(KIntContactFieldVCardMapPDF)
		CASE_LIT(KIntContactFieldVCardMapPS)
		CASE_LIT(KIntContactFieldVCardMapJPEG)
		CASE_LIT(KIntContactFieldVCardMapMPEG)
		CASE_LIT(KIntContactFieldVCardMapMPEG2)
		CASE_LIT(KIntContactFieldVCardMapAVI)
		CASE_LIT(KIntContactFieldVCardMapQTIME)
		CASE_LIT(KIntContactFieldVCardMapTZ)
		CASE_LIT(KIntContactFieldVCardMapKEY)
		CASE_LIT(KIntContactFieldVCardMapX509)
		CASE_LIT(KIntContactFieldVCardMapPGP)
		CASE_LIT(KIntContactFieldVCardMapSMIME)
		CASE_LIT(KIntContactFieldVCardMapWV)
		CASE_LIT(KIntContactFieldVCardMapSECONDNAME)
		CASE_LIT(KIntContactFieldVCardMapSIPID)
		CASE_LIT(KIntContactFieldVCardMapPOC)
		CASE_LIT(KIntContactFieldVCardMapSWIS)
		CASE_LIT(KIntContactFieldVCardMapVOIP)
		CASE_LIT(KIntContactFieldVCardMapAssistant)
		CASE_LIT(KIntContactFieldVCardMapAssistantTel)
		CASE_LIT(KIntContactFieldVCardMapAnniversary)
		CASE_LIT(KIntContactFieldVCardMapSpouse)
		CASE_LIT(KIntContactFieldVCardMapChildren)
		CASE_LIT(KIntContactFieldVCardMapClass)
		CASE_LIT(KIntContactFieldVCardMapDepartment)
		default:
			{
			_LIT(KUnknown, "Unknown");
			return &KUnknown;
			}
		}
	}

EXE_BOILER_PLATE(CCmdContacts)

