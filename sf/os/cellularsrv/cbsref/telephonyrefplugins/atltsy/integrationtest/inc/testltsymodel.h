//  TestTelephonyModel.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestTelephonyModel
//
#ifndef TESTTELEPHONYMODEL_H_
#define TESTTELEPHONYMODEL_H_

// system include
#include <e32cmn.h>
#include <e32des8.h>
#include <etelmm.h>
#include <etel.h>
#include <etel3rdparty.h> 

// forward class
class CTestLtsySharedEnv;
class CTestLtsyCallSharedData;
class CTestLtsyPktSharedData;
class CTestLtsySmsSharedData;
class CTestLtsyPhoneStoreSharedData;

// define the module name length
const TUint8 KModuleNameLen = 200;


/**
 * This class acts as a proxy to provide the functions to return the shared data
 * accross the steps.
 * 
 */
class CTestLtsyModel : public CBase
/**
 * @internalAll
 * @test
 */
	{
public:
	static CTestLtsyModel* NewL();
	~CTestLtsyModel();
	inline CTestLtsySharedEnv& SharedEnv();
	inline CTestLtsyCallSharedData& CallSharedData();
	inline CTestLtsyPktSharedData&  PktSharedData();
	inline CTestLtsySmsSharedData&  SmsSharedData();
	inline CTestLtsyPhoneStoreSharedData& PhoneStoreSharedData();
private:
	void ConstructL();
private:
	CTestLtsySharedEnv* iSharedPhoneEnv;
	CTestLtsyCallSharedData* iCallSharedData;
	CTestLtsyPktSharedData*  iPktSharedData;
	CTestLtsySmsSharedData* iSmsSharedData;
	CTestLtsyPhoneStoreSharedData* iPhoneStoreSharedData;
	};
/**
 * This calss define a shared phone environment
 * 
 */
class CTestLtsySharedEnv : public CBase
/**
 * @internalAll
 * @test
 */
    { 
public:
    ~CTestLtsySharedEnv();
	static CTestLtsySharedEnv* NewL();
    inline RMobilePhone& Phone();
    inline RTelServer& ETelServer();
    inline TBool PhoneOpened();
    inline void IncreasePhoneReference();
    inline void DecreasePhoneReference();
    inline TInt PhoneRefCount();
	void LoadModuleL(const TDesC& aModuleName);
	void OpenPhoneL(const TDesC& aModuleName);
	// must be called by client thread if it call OpenPhoneL()
	void ClosePhone();
	void CreateTelephonyL();
	void DestroyTelephony();
	CTelephony* Telephony();
private:
    CTestLtsySharedEnv();
    void ConstructL();	
    TBool FindPhoneIndex();
private:
	TBuf<KModuleNameLen> iModuleName;
	RMobilePhone* iSharedPhone;
	TBool iPhoneInitialized;
	RTelServer iSharedTelServer;
	TInt iPhoneRef;
	TInt iPhoneIndex;
	CTelephony *iTelephony;
//	RMmCustomAPI     *iRmmCustomAPI;
	CActiveScheduler*  iActiveSchaduler;
//    TBuf<KCommsDbSvrMaxFieldLength> iTsyNameFromCommdb;
	};	

#include "TestLtsyModel.inl"

#endif /*TESTTELEPHONYMODEL_H_*/
