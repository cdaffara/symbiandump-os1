// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent
*/

#include "t_wservconfigdata.h"

//
// Construction/Destruction
//

CT_WServConfigData::CT_WServConfigData()
	{
	}

CT_WServConfigData::~CT_WServConfigData()
	{
	Empty();
	}

void CT_WServConfigData::ReplaceData(const TDesC& aName, const TDesC& aData)
	{
	TInt	count=iArrayName.Count();
	TInt	index=0;
	for ( index=0; index<count; ++index )
		{
		if ( aName.Compare(iArrayName[index]) == 0 )
			{
			break;
			}
		}
	
	iArrayData.Remove(index);
	iArrayData.Insert(aData,index);
	}
	
void CT_WServConfigData::AddDataL(const TDesC& aName, const TDesC& aData)
	{
	//	Ensure name does not already exist
	TInt	count=iArrayName.Count();
	TInt	index=0;
	for ( index=0; index<count; ++index )
		{
		if ( aName.Compare(iArrayName[index]) == 0 )
			{
			User::Leave(KErrAlreadyExists);
			}
		}

	iArrayName.AppendL(aName);
	iArrayData.AppendL(aData);
	}

void CT_WServConfigData::DeleteDataL(const TDesC& aName)
	{
	//	Ensure name already exist
	TBool	found=EFalse;
	TInt	count=iArrayName.Count();
	for ( TInt index=0; (index<count) && (!found); ++index )
		{
		if ( aName.Compare(iArrayName[index]) == 0 )
			{
			found=ETrue;
			iArrayName.Remove(index);
			iArrayData.Remove(index);
			}
		}

	if ( !found )
		{
		User::Leave(KErrNotFound);
		}
	}


const TWServConfigData& CT_WServConfigData::GetData(const TInt aIndex) const
	{
	return iArrayData[aIndex];
	}

const TWServConfigData& CT_WServConfigData::GetName(const TInt aIndex) const
	{
	return iArrayName[aIndex];
	}

void CT_WServConfigData::Empty()
	{
	iArrayName.Close();
	iArrayData.Close();
	}

TInt CT_WServConfigData::NoOfEntriesL() const
	{
	TInt	dataCount=iArrayData.Count();
	TInt	nameCount=iArrayName.Count();

	if (dataCount!=nameCount)
		{
		User::Leave(KErrGeneral);
		}

	return dataCount;
	}
