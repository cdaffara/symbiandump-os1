//  TestTelephonyModel.cpp
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestTelephonyModel

// CTSY include
#include <bautils.h>

#include "TestLtsyModel.h"
#include "testltsycallshareddata.h"
#include "TestLtsyPktSharedData.h"
#include "TestLtsySmsSharedData.h"
#include "TestLtsyPhoneStoreSharedData.h"

// the file used to identify if we run on real environment
//_LIT(KCTsyRealEnvFilePath,    "z:\\systemtest\\testdata\\telephony\\ctsyrealenv.txt");
//_LIT(KMmTsyModuleName, "PhoneTsy");

/**
 * Factory function
 * 
 */
CTestLtsyModel* CTestLtsyModel::NewL()
	{
	CTestLtsyModel* self = new (ELeave) CTestLtsyModel();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

/**
 * Destructor
 * 
 */
CTestLtsyModel::~CTestLtsyModel()
	{
	delete iSharedPhoneEnv;
	delete iCallSharedData;
	delete iPktSharedData;
	delete iSmsSharedData;
	delete iPhoneStoreSharedData;
	}

/**
 * 2nd Constructor 
 * 
 */
void CTestLtsyModel::ConstructL()
	{
	iSharedPhoneEnv = CTestLtsySharedEnv::NewL();
	iCallSharedData = CTestLtsyCallSharedData::NewL();
	iPktSharedData 	= CTestLtsyPktSharedData::NewL();
	iSmsSharedData = CTestLtsySmsSharedData::NewL();
	iPhoneStoreSharedData = CTestLtsyPhoneStoreSharedData::NewL();
	}
	
//=============================================================================
// class CTestTelephonyAdaptorEnv
//=============================================================================	
/**
 * Constructor
 * 
 */
CTestLtsySharedEnv::CTestLtsySharedEnv()
    {
    // reset iPhoneInitialized
    iPhoneInitialized = EFalse;
    iPhoneIndex = -1;
	}

/**
 * Destructor
 */
CTestLtsySharedEnv::~CTestLtsySharedEnv()
    {     
    ClosePhone();
    DestroyTelephony();
    }

/**
 * Factory function
 * 
 */
CTestLtsySharedEnv* CTestLtsySharedEnv::NewL()
    {
    CTestLtsySharedEnv* self = new (ELeave) CTestLtsySharedEnv();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

/**
 * 2nd constructor
 */
void CTestLtsySharedEnv::ConstructL()
    {
    }
	
/**
 * Load phone module
 * 
 * @param const TDesC& aModuleName The phone module used by ETel server for loading a specific TSY
 */
void CTestLtsySharedEnv::LoadModuleL(const TDesC& aModuleName)
    {	

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	CleanupStack::PopAndDestroy(&fs);
    
	TInt ret = iSharedTelServer.LoadPhoneModule(aModuleName);

	if (ret != KErrNone)
		{
		iSharedTelServer.Close();
		User::Leave(ret);
		}
    }

/**
 * Open the shared phone
 * 
 * @param const TDesC& aModuleName The phone module used by ETel server for loading a specific TSY
 */
void CTestLtsySharedEnv::OpenPhoneL(const TDesC& aModuleName)
    {
    THandleInfo handleInfo;
    iSharedTelServer.HandleInfo(&handleInfo);
	if(iPhoneInitialized)
	    {
	    // a panic occurs if handleInfo.iNumThreads <= 0
	    if(handleInfo.iNumThreads <= 0)
	    	{
	    	iSharedPhone = NULL;
	        iPhoneInitialized = EFalse;
	        iSharedTelServer.Close();
	    	}
		else
		    {
			// return if phone has already been opened before
			return;
			}
	    }
	
	// make sure this pointer was NULL
	ASSERT(iSharedPhone == NULL);
	
	// make a copy of module name
	iModuleName.Copy(aModuleName);
	
	// start connect to ETel server
	User::LeaveIfError(iSharedTelServer.Connect());
	User::LeaveIfError(iSharedTelServer.ShareAuto());	

	// start load module	
	LoadModuleL(aModuleName);
	
	// check if we can find corresponding phone object according to passed TSY module name
	if(!FindPhoneIndex())
		{
		// the phone index could not be found
        User::Leave(KErrNotFound);		
 		}
		
	// create a new RMobilePhone instance
	iSharedPhone = new (ELeave) RMobilePhone();
	RTelServer::TPhoneInfo phoneInfo;
	User::LeaveIfError(iSharedTelServer.GetPhoneInfo(iPhoneIndex,phoneInfo));
	
	// start openning phone
	TInt ret = iSharedPhone->Open(iSharedTelServer,phoneInfo.iName);
	if(ret != KErrNone)
	    {
		User::Leave(ret);
		}
	
	iPhoneInitialized = ETrue;
	}

/**
 * Find the phone index according to TSY name
 * 
 */
TBool CTestLtsySharedEnv::FindPhoneIndex()
	{
	TInt phoneNum = 0;
	TInt err = iSharedTelServer.EnumeratePhones(phoneNum);
	if(err != KErrNone)
		{
		return EFalse;
		}
	
	TBool found = EFalse;
	for(TInt i = 0; i < phoneNum; i++)
		{
		RTelServer::TPhoneInfo phoneInfo;
		TBuf<KModuleNameLen> moduleName;
		err = iSharedTelServer.GetTsyName(i,moduleName);
		if(err != KErrNone)
			{
			return EFalse;
			}
		else
			{
			if(moduleName.CompareF(iModuleName) == 0)
				{
				iPhoneIndex = i;
				found = ETrue;
				break;
				}
			}
		}
	
	return found;
	}

/**
 * Close the shared phone and server
 * Client must call this function, if it has called OpenPhoneL().
 * 
 */
void CTestLtsySharedEnv::ClosePhone()
	{
	// close and delete the shared phone
	iSharedPhone->Close();
	delete iSharedPhone;
	iSharedPhone = NULL;
	
	// unload the phone module
	iSharedTelServer.UnloadPhoneModule(iModuleName);
	iSharedTelServer.Close();
	
	// reset iPhoneInitialized
	iPhoneInitialized = EFalse;
	}

/**
 * Create a CTelephony object and related resource. 
 * 
 */
void CTestLtsySharedEnv::CreateTelephonyL()
	{
	iActiveSchaduler = new (ELeave) CActiveScheduler();
	CActiveScheduler::Install(iActiveSchaduler);
	
	iTelephony = CTelephony::NewL();
	}

/**
 * Delete CTelephony object related source.
 * 
 */
void CTestLtsySharedEnv::DestroyTelephony()
	{
	
	delete iActiveSchaduler;
	iActiveSchaduler = NULL;
	delete iTelephony;
	iTelephony       = NULL;

	delete iSharedPhone;
	iSharedPhone = NULL;
	
	// reset iPhoneInitialized
	iPhoneInitialized = EFalse;
	}

CTelephony* CTestLtsySharedEnv::Telephony()
	{
	return iTelephony;
	}

// end of file
