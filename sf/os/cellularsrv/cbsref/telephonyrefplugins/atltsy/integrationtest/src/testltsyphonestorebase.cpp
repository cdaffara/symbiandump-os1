// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// 

// user include
#include "testltsyphonestorebase.h"
#include "testltsyphonestoreshareddata.h"

//const define
_LIT16(KPhoneTSY, "phonetsy");

// member functions

/**
* Constructor
* 
*/
CTestLtsyPhoneStoreBase::CTestLtsyPhoneStoreBase(CTestLtsyModel& aTestModel)
    : iModel(aTestModel)
	{
	}

/**
* Destructor
*
*/	
CTestLtsyPhoneStoreBase::~CTestLtsyPhoneStoreBase()
	{
	
	}

TVerdict CTestLtsyPhoneStoreBase::doTestStepPreambleL()
	{
	OpenPhoneL();
	if(TestStepResult() == EPass)
		{		
		
		}
	
	return TestStepResult();
	}

TVerdict CTestLtsyPhoneStoreBase::doTestStepPostambleL()
	{

	return TestStepResult();
	}

/**
 * Get the PhoneON object
 * 
 */
RMobileONStore& CTestLtsyPhoneStoreBase::PhoneONStore()
	{
	return iModel.PhoneStoreSharedData().PhoneONStore();	
	}

/**
 * Get the Enstore object
 * 
 */
RMobileENStore& CTestLtsyPhoneStoreBase::PhoneENStore()
	{
    return iModel.PhoneStoreSharedData().PhoneENStore();	
	}

/**
 * Get the PhoneBook store object
 * 
 */
RMobilePhoneBookStore& CTestLtsyPhoneStoreBase::PhoneBookStore()
	{
	 return iModel.PhoneStoreSharedData().PhoneBookStore();	
	}

void CTestLtsyPhoneStoreBase::OpenPhoneL()
	{
	INFO_PRINTF1(_L("starting opne phone"));
		
	if (!iModel.SharedEnv().PhoneOpened())
		{
		iModel.SharedEnv().OpenPhoneL(KPhoneTSY);
		}
	}

// end of file


