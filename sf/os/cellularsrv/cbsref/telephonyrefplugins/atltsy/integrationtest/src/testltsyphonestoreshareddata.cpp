// TestTelephonyPhoneStoreSharedData.cpp
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestTelephonyPhoneStoreSharedData

// CTSY include

// user include
#include "TestLtsyPhoneStoreSharedData.h"

/**
 * Constructor
 * 
 */
CTestLtsyPhoneStoreSharedData::CTestLtsyPhoneStoreSharedData()
	{
	}

/**
 * Factory function to create a new CTestTelephonySmsSharedData object
 * 
 */
CTestLtsyPhoneStoreSharedData*CTestLtsyPhoneStoreSharedData::NewL()
	{
	CTestLtsyPhoneStoreSharedData* self = new (ELeave) CTestLtsyPhoneStoreSharedData();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

/**
 * Destructor
 * 
 */
CTestLtsyPhoneStoreSharedData::~CTestLtsyPhoneStoreSharedData()
	{
	Reset();
	}

/**
 * ConstructL
 * 
 */
void CTestLtsyPhoneStoreSharedData::ConstructL()
	{
	}

/**
 * Delete all store objects
 * 
 */
void CTestLtsyPhoneStoreSharedData::Reset()
	{
	iENStore.Close();
	iONStore.Close();
	iPhBkStore.Close();
	}

/**
 * return ENStore 
 * 
 */
RMobileENStore& CTestLtsyPhoneStoreSharedData::PhoneENStore()
	{
	return iENStore;
	}


/**
 * return ONStore
 * 
 */
RMobileONStore& CTestLtsyPhoneStoreSharedData::PhoneONStore()
	{
	return iONStore;
	}

/**
 * return PhoneBookStore
 * 
 */
RMobilePhoneBookStore& CTestLtsyPhoneStoreSharedData::PhoneBookStore()
	{
	return iPhBkStore;
	}
// end of file
