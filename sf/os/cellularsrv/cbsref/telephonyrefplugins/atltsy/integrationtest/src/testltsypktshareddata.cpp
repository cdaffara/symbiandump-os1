// TestTelephonyPktSharedData.cpp
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestTelephonyPktSharedData

// user include
#include "TestLtsyPktSharedData.h"

/**
 * Factory function
 */
CTestLtsyPktSharedData* CTestLtsyPktSharedData::NewL()
	{
	CTestLtsyPktSharedData* ret = new(ELeave) CTestLtsyPktSharedData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	
	return ret;
	}

/**
 * Constructor
 * 
 * @param CTestStep& aTestStep The test step used for logging and reading info from ini
 * @param CTestTelephonyTsyAdaptor& aTestEnvAdaptor The telephony tsy adaptor
 */
CTestLtsyPktSharedData::CTestLtsyPktSharedData()
	{
	}

/**
 * Destructor
 * 
 */
CTestLtsyPktSharedData::~CTestLtsyPktSharedData()
	{
	}


/**
 * ConstructL
 * 
 */
void CTestLtsyPktSharedData::ConstructL()
	{
	}

// end of file
