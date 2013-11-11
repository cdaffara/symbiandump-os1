// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent 
*/

#ifndef __SMS_PDU_DB_INL__
#define __SMS_PDU_DB_INL__

#include <testconfigfileparser.h>

inline CSmsPduDatabase::TSmsPduDbId::TSmsPduDbId(const CTestConfigItem* aItem, TInt aId)
: iItem(aItem), iId(aId)
	{
	}

inline CSmsPduDatabase::RSmsPduDbIdArray::RSmsPduDbIdArray(const TDesC8& aType, TBool aAllowDuplicates)
: RArray<TSmsPduDbId>(), iType(aType), iAllowDuplicates(aAllowDuplicates)
	{
	}

inline const TDesC8& CSmsPduDatabase::RSmsPduDbIdArray::Type() const
	{
	return iType;
	}

inline CTestConfig& CSmsPduDatabase::ConfigFile()
	{
	return *iConfigFile;
	}

inline const CTestConfig& CSmsPduDatabase::ConfigFile() const
	{
	return *iConfigFile;
	}

inline void CSmsPduDatabase::SetSectionL(const TDesC8& aSection)
/**
 *  Sets the section to be used for the database (from iConfigFile)
 */
	{
	__ASSERT_DEBUG(iConfigFile != NULL, PduDbPanic(EPduDbPanicConfigFileNotConstructed));
	iSection = iConfigFile->Section(aSection);
	if (iSection == NULL)
		{
		User::Leave(KErrNotFound);
		}
	}

inline void CSmsPduDatabase::SetSection(const CTestConfigSection& aSection)
/**
 *  Sets the section to be used for the database. aSection does not necessary need to belong to iConfigFile
 */
	{
	iSection = &aSection;
	}

inline void PduDbPanic(TSmsPduDbPanic aPanic)
	{
	User::Panic(KSmsPduDbPanic, aPanic);
	}

inline TBool CSmsPduDatabase::IsMessage(const TDesC8& aTag) const
	{
	return (aTag.CompareF(KSmsPduDbMessage) == KErrNone);
	}


#endif
