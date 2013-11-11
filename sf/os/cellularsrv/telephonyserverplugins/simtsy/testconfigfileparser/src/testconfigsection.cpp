/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "testconfigfileparser.h"
#include <f32file.h>


//
// CTestConfigSection
//

CTestConfigSection::CTestConfigSection()
	: CBase()
	{
	}

EXPORT_C CTestConfigSection* CTestConfigSection::NewLC(const TDesC8& aSectionName, CTestConfigSection& aDefaults)
	{
	CTestConfigSection* self = new (ELeave) CTestConfigSection();
	CleanupStack::PushL(self);
	self->iDefaults = aDefaults.CopyLC();
	CleanupStack::Pop(self->iDefaults);
	self->ConstructL(aSectionName);
	return self;
	}

EXPORT_C CTestConfigSection* CTestConfigSection::NewLC(const TDesC8& aSectionName)
	{
	CTestConfigSection* self = new (ELeave) CTestConfigSection();
	CleanupStack::PushL(self);
	self->ConstructL(aSectionName);
	return self;
	}

void CTestConfigSection::ConstructL(const TDesC8& aSectionName)
	{
	iSectionName = aSectionName.AllocL();
	}

EXPORT_C const TDesC8& CTestConfigSection::ItemValue(const TDesC8& aItem, const TDesC8& aDefault) const
	{
	const CTestConfigItem* sectionItem = Item(aItem);

	if (sectionItem != NULL)
		{
		const TPtrC8 value(sectionItem->Value());

		if (iDefaults != NULL && (value.CompareF(KScriptDefault1) == 0 || value.CompareF(KScriptDefault2) == 0))
			{
			return iDefaults->ItemValue(aItem, aDefault);
			}
		else
			{
			return sectionItem->Value();
			}
		}
	else if (iDefaults != NULL)
		{
		return iDefaults->ItemValue(aItem, aDefault);
		}
	else
		{
		return aDefault;
		}
	}


EXPORT_C TInt CTestConfigSection::ItemValue(const TDesC8& aItem, const TInt aDefault) const
	{
	const TDesC8& value = ItemValue(aItem, KNullDesC8);

	if (value.Length() == 0)
		{
		if (iDefaults != NULL)
			return iDefaults->ItemValue(aItem, aDefault);
		else
			return aDefault;
		}

	TLex8 input(value);
	TInt ret = aDefault;
	TInt err = input.Val(ret);
	
	if (err)
		{
		if (iDefaults != NULL)
			return iDefaults->ItemValue(aItem, aDefault);
		else
			return aDefault;
		}

	return ret;
	}

EXPORT_C CTestConfigItem& CTestConfigSection::AddItemL(const TDesC8& aItem, const TDesC8& aValue)
	{
	CTestConfigItem* sectionItem = CTestConfigItem::NewLC(*this, aItem, aValue);
	User::LeaveIfError(iItems.Append(sectionItem));
	CleanupStack::Pop(sectionItem);
	return *sectionItem;
	}

EXPORT_C void CTestConfigSection::DeleteItemsL(const TDesC8& aItem)
	{
	TInt count = iItems.Count();

	//Remove existing CTestConfigItems that have the same item name
	while (count--)
		{
		CTestConfigItem* sectionItem = iItems[count];

		if (sectionItem->Item().CompareF(aItem) == 0)
			{
			delete sectionItem;
			iItems.Remove(count);
			}
		}	
	}


EXPORT_C CTestConfigSection::~CTestConfigSection()
	{
	iItems.ResetAndDestroy();
	iItems.Close();

	delete iSectionName;
	delete iDefaults;
	}

EXPORT_C CTestConfigSection* CTestConfigSection::CopyLC() const
	{
	TInt count = iItems.Count();
	CTestConfigSection* copy = NULL;

	if (iDefaults != NULL)
		copy = CTestConfigSection::NewLC(SectionName(), *iDefaults);
	else
		copy = CTestConfigSection::NewLC(SectionName());
		
	while (count--)
		{
		CTestConfigItem* item = iItems[count]->CopyLC();
		User::LeaveIfError(copy->iItems.Append(item));
		CleanupStack::Pop(item);
		}

	return copy;
	}

void CTestConfigSection::WriteL(RFile& aFile) const
	{
	//Write the section name
	HBufC8* buf = HBufC8::NewLC(iSectionName->Length());
	buf->Des().Copy(*iSectionName);

	User::LeaveIfError(aFile.Write(KScriptSectionStart));
	User::LeaveIfError(aFile.Write(*buf));
	User::LeaveIfError(aFile.Write(KScriptSectionEnd));
	User::LeaveIfError(aFile.Write(KScriptCRLF8));

	CleanupStack::PopAndDestroy(buf);
	const TInt count = iItems.Count();

	//Write the items
	for (TInt i = 0; i < count; i++)
		iItems[i]->WriteL(aFile);

	User::LeaveIfError(aFile.Write(KScriptCRLF8));
	}

TBool CTestConfigSection::operator==(const CTestConfigSection& aSection) const
	{
	TInt count = iItems.Count();

	if (count != aSection.Items().Count())
		return EFalse;

	TBool retVal = (*iSectionName == aSection.SectionName());

	while (count-- && retVal)
		{
		retVal = retVal && (*iItems[count] == aSection[count]);
		}

	return retVal;
	}

EXPORT_C const CTestConfigItem* CTestConfigSection::Item(const TDesC8& aItem) const
	{
	const CTestConfigItem* sectionItem = NULL;
	const TInt count = iItems.Count();
	for (TInt i = 0; i < count; i++)
		{
		if (iItems[i]->Item().CompareF(aItem) == 0)
			{
			sectionItem = iItems[i];
			break;
			}
		}
	return sectionItem;
	}

EXPORT_C CTestConfigItem* CTestConfigSection::Item(const TDesC8& aItem)
	{
	CTestConfigItem* sectionItem = NULL;
	const TInt count = iItems.Count();
	for (TInt i = 0; i < count; i++)
		{
		if (iItems[i]->Item().CompareF(aItem) == 0)
			{
			sectionItem = iItems[i];
			break;
			}
		}
	return sectionItem;
	}

EXPORT_C const CTestConfigItem* CTestConfigSection::Item(const TDesC8& aItem,TInt aIndex) const
	{
	const CTestConfigItem* sectionItem = NULL;
	const TInt count = iItems.Count();
	TInt foundItemCnt=0;
	for (TInt i = 0; i < count; i++)
		{
		if (iItems[i]->Item().CompareF(aItem) == 0)
			{
			if(foundItemCnt++==aIndex)
				{
				sectionItem = iItems[i];
				break;
				}
			}
		}
	return sectionItem;
	}

EXPORT_C CTestConfigItem* CTestConfigSection::Item(const TDesC8& aItem,TInt aIndex)
	{
	CTestConfigItem* sectionItem = NULL;
	const TInt count = iItems.Count();
	TInt foundItemCnt=0;
	for (TInt i = 0; i < count; i++)
		{
		if (iItems[i]->Item().CompareF(aItem) == 0)
			{
			if(foundItemCnt++==aIndex)
				{
				sectionItem = iItems[i];
				break;
				}
			}
		}
	return sectionItem;
	}

EXPORT_C void CTestConfigSection::ItemsL(RPointerArray<CTestConfigItem>& aArray, const TDesC8& aItemTag)
	{
	aArray.Reset();
	const TInt count = iItems.Count();

	for (TInt i = 0; i < count; i++) //< Order important
		{
		CTestConfigItem& item = *iItems[i];

		if (item.Item().CompareF(aItemTag) == 0)
			{
			User::LeaveIfError(aArray.Append(&item));
			}
		}
	}

EXPORT_C void CTestConfigSection::ItemsL(RPointerArray<const CTestConfigItem>& aArray, const TDesC8& aItemTag) const
	{
	aArray.Reset();
	const TInt count = iItems.Count();

	for (TInt i = 0; i < count; i++) //< Order important
		{
		const CTestConfigItem& item = *iItems[i];

		if (item.Item().CompareF(aItemTag) == 0)
			{
			User::LeaveIfError(aArray.Append(&item));
			}
		}
	}


EXPORT_C TInt CTestConfigSection::ItemCount(const TDesC8& aItem) const
	{
	const TInt count = iItems.Count();
	TInt foundItemCnt=0;
	for (TInt i = 0; i < count; i++)
		{
		if (iItems[i]->Item().CompareF(aItem) == 0)
			foundItemCnt++;
		}
	return foundItemCnt;
	}
