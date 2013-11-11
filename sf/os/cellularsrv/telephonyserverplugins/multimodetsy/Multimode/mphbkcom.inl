// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalAll 
*/

inline CMobilePhonebookStore& CATPhoneBookInfo::PhoneBook() const
	{return *static_cast<CMobilePhonebookStore*>(iTelObject);}

inline void CATPhoneBookRead::SetPBData(TDes8* aPBData)
	{
	iPbBuffer->Set(aPBData);
	}

inline void CATPhoneBookWrite::SetPBData(TDes8* aPBData)
	{	
	iPbBuffer->Set(aPBData);
	}

inline TInt CATPhoneBookWrite::Index() const
	{return iIndex;}

inline TInt CATPhoneBookDelete::Index() const
	{return iIndex;}

inline void CATPhoneBookInfo::MapClientIndexToPhoneIndex(TInt& aIndex) const
	{aIndex+=iIndexOffset;}

inline void CATPhoneBookInfo::MapPhoneIndexToClientIndex(TInt& aIndex) const
	{aIndex-=iIndexOffset;}

inline TBool CATPhoneBookInfo::IsValidPhoneIndex(TInt aIndex) const
	{return ((aIndex>=iIndexOffset+1) && (aIndex<=(iIndexOffset+iPhbkInfo.iTotalEntries)));}

inline TInt CATPhoneBookInfo::UsedEntries() const
	{return iPhbkInfo.iUsedEntries;}

inline TInt CATPhoneBookInfo::TotalEntries() const
	{return iPhbkInfo.iTotalEntries;}

inline TBool CATPhoneBookInfo::Completed() const
	{return iCompleted;}


