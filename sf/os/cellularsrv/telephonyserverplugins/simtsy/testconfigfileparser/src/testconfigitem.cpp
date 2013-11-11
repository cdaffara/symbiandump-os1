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

CTestConfigItem::CTestConfigItem(CTestConfigSection& aParent)
: iParent(aParent)
	{
	}

void CTestConfigItem::ConstructL(const TDesC8& aItem, const TDesC8& aValue)
	{
	iItem = aItem.AllocL();
	iValue = aValue.AllocL();
	}

EXPORT_C CTestConfigItem::~CTestConfigItem()
	{
	delete iItem;
	delete iValue;
	}

EXPORT_C CTestConfigItem* CTestConfigItem::NewLC(CTestConfigSection& aParent, const TDesC8& aItem, const TDesC8& aValue)
	{
	CTestConfigItem* self = new (ELeave) CTestConfigItem(aParent);
	CleanupStack::PushL(self);
	self->ConstructL(aItem, aValue);
	return self;
	}

void CTestConfigItem::WriteL(RFile& aFile) const
	{
		//Write the section name
	HBufC8* buf = HBufC8::NewLC(iItem->Length());
	TPtr8 ptr(buf->Des());

	ptr.Copy(*iItem);
	User::LeaveIfError(aFile.Write(*buf));
	User::LeaveIfError(aFile.Write(KScriptItemEnd8));
	User::LeaveIfError(aFile.Write(KScriptSpace8));
	CleanupStack::PopAndDestroy(buf);

	HBufC8* val = CTestConfig::ReplaceLC(KScriptLF, KScriptCRLF, *iValue);

	buf = HBufC8::NewLC(val->Length());
	ptr.Set(buf->Des());
	ptr.Copy(*val);

	User::LeaveIfError(aFile.Write(*buf));
	User::LeaveIfError(aFile.Write(KScriptCRLF8));

	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PopAndDestroy(val);
	}
