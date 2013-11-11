// PresetList.cpp

// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "PresetList.h"


CPresetList* CPresetList::NewL()
	{
	CPresetList* self = new (ELeave) CPresetList;
	return self;
	}

CPresetList::CPresetList()
	{
	}
	
CPresetList::~CPresetList()
	{
	Reset();
	}
	
void CPresetList::AppendL(TUid aUid, const TDesC& aName)
	{
	HBufC* tempName = aName.AllocLC();
	TPresetEle ele;
	ele.iUid = aUid;
	ele.iName = tempName;
	User::LeaveIfError(iArray.Append(ele));
	CleanupStack::Pop(tempName);
	}
	
TInt CPresetList::MdcaCount() const
	{
	return iArray.Count();
	}
	
TPtrC16 CPresetList::MdcaPoint(TInt aIndex) const
	{
	const TPresetEle& ele = iArray[aIndex];
	return TPtrC16(*ele.iName);	
	}
	
TUid CPresetList::GAPUidPoint(TInt aIndex) const
	{
	const TPresetEle& ele = iArray[aIndex];
	return ele.iUid;	
	}
	
void CPresetList::Reset()
	{
	TInt count = iArray.Count();
	
	for (TInt index=0; index<count; index++)
		{
		TPresetEle& ele = iArray[index];
		delete ele.iName; 
		ele.iName = NULL;
		}
	iArray.Reset();
	}
