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

#ifndef TEST_SCRIPTS_INL_
#define TEST_SCRIPTS_INL_

inline const RPointerArray<CTestConfigSection>& CTestConfig::Sections() const
	{
	return iSections;
	}

inline RPointerArray<CTestConfigSection>& CTestConfig::Sections()
	{
	return iSections;
	}

inline const TDesC8& CTestConfigSection::SectionName() const
	{
	return *iSectionName;
	}

inline const TDesC8& CTestConfigItem::Item() const
	{
	return *iItem;
	}

inline void CTestConfigSection::SetDefaultsL(const CTestConfigSection& aDefaults)
	{
	CTestConfigSection* def = aDefaults.CopyLC();
	CleanupStack::Pop(def);
	delete iDefaults;
	iDefaults = NULL;
	iDefaults = def;
	}


inline const TDesC8& CTestConfigItem::Value() const
	{
	return *iValue;
	}


inline CTestConfigItem* CTestConfigItem::CopyLC() const
	{
	return NewLC(iParent, *iItem, *iValue);
	}



#endif
