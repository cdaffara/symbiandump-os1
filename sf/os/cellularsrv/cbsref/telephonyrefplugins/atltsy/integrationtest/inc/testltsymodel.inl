//  TestTelephonyModel.inl
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  The inline function implementation of CTestTelephonyModel

#ifndef TESTTELEPHONYMODEL_INL_
#define TESTTELEPHONYMODEL_INL_

// user include
#include "TestLtsyModel.h"

/**
 * Get the shared phone environment
 * 
 */
CTestLtsySharedEnv& CTestLtsyModel::SharedEnv()
	{
	return *iSharedPhoneEnv;
	}

/**
 * Get the packet shared data
 *
 */
CTestLtsyPktSharedData& CTestLtsyModel::PktSharedData()
	{
	return *iPktSharedData;
	}

/**
 * Get the call shared data
 * 
 */
CTestLtsyCallSharedData& CTestLtsyModel::CallSharedData()
	{
	return *iCallSharedData;
	}	

/**
 * Get the sms shared data
 * 
 */
CTestLtsySmsSharedData& CTestLtsyModel::SmsSharedData()
	{
	return *iSmsSharedData;
	}
	
/**
 * Get the phone store shared data
 * 
 */


CTestLtsyPhoneStoreSharedData& CTestLtsyModel::PhoneStoreSharedData()
	{
	return *iPhoneStoreSharedData; 
	}

///**
// * Get the Tsy Adaptor
// * 
// */
//CTestTelephonyTsyAdaptor& CTestLtsySharedEnv::TsyAdaptor()
//	{
//	return *iTsyAdaptor;
//	}

/**
 * Get the shared phone
 * 
 */
RMobilePhone& CTestLtsySharedEnv::Phone() 
	{
	return *iSharedPhone;
	}

/**
 * Get the shared ETel server
 * 
 */
RTelServer& CTestLtsySharedEnv::ETelServer() 
	{
	return iSharedTelServer;
	}

/**
 * Check if the shared phone has arleady been opened before
 * 
 */
TBool CTestLtsySharedEnv::PhoneOpened()
	{
	return iPhoneInitialized;
	}

/**
 * Increase the shared phone reference number when a client used the shared phone
 * 
 */
void CTestLtsySharedEnv::IncreasePhoneReference()
	{
	iPhoneRef++;
	}

/**
 * Decrease the shared phone reference number
 * 
 */
void CTestLtsySharedEnv::DecreasePhoneReference()
	{
	iPhoneRef--;
	}

/**
 * Get the phone reference count
 * 
 */
TInt CTestLtsySharedEnv::PhoneRefCount()
	{
	return iPhoneRef;
	}

#endif /*TESTTELEPHONYMODEL_INL_*/

// end of file
