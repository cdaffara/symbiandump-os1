// TestTelephonyPhoneStoreSharedData.h
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestTelephonyPhoneStoreSharedData

#ifndef TESTTELEPHONYPHONESTORESHAREDDATA_H_
#define TESTTELEPHONYPHONESTORESHAREDDATA_H_
// system include
#include <e32cmn.h>
#include <e32base.h>
#include <etelmm.h>

/**
 * This class defines the shared data of phone store test category between 
 * different test steps.
 */
class CTestLtsyPhoneStoreSharedData : public CBase
/**
 * @internalAll
 * @test
 */
	{
public:
	CTestLtsyPhoneStoreSharedData();
	virtual ~CTestLtsyPhoneStoreSharedData();
	static CTestLtsyPhoneStoreSharedData* NewL();	
	RMobileENStore& PhoneENStore();	
	RMobileONStore& PhoneONStore();	
	RMobilePhoneBookStore& PhoneBookStore();
	void   Reset();	
private:
	void ConstructL();
private:
	RMobileONStore iONStore;
	RMobileENStore iENStore;
	RMobilePhoneBookStore iPhBkStore;
	};
#endif /*TESTTELEPHONYPHONESTORESHAREDDATA_H_*/
