// TestTelephonySharedData.h
//
// Copyright (c) 2003-2008 Symbian Software Ltd.  All rights reserved.
//
// This contains TestTelephonySharedData
#if (!defined __TEST_TELEPHONY_SHARED_DATA_H__)
#define __TEST_TELEPHONY_SHARED_DATA_H__

// CDMA specific Conditional compilation
//#include "../../group/configuration.cfg"

// EPOC include
#include <etelmm.h>
#include <etel.h>
#if (defined CDMA_API_ENABLED)
#include <etelcdma.h>
#endif
#include <testexecutestepbase.h>

#include "TestLtsyAsyncOperation.h"

class CTestLtsySharedData : public CBase
/**
@internalAll
@test
@class CTestLtsySharedData

For creating the shared data object and to access the object across
the test steps.. 
*/
	{
public:
	static CTestLtsySharedData*	NewL();

	~CTestLtsySharedData();
	TInt		OpenPhone(const TName& aTsyName, CTestStep& aTestStep);
	void		ClosePhone();
	TInt		ReopenPhone();
	TBool		PhoneOpened() const { return iPhoneOpened; }
	CTelephony&	Telephony() const { return *iTelephony; }

protected:
	CTestLtsySharedData();
	virtual void	ConstructL();

public:
	/** Holding Calld Id of line1 */
	CTelephony::TCallId			iCallId;
	/** Holding Calld Id of line2 */
	CTelephony::TCallId			iCallId1;
	/** Pointer to the RMobileCall object */
	RMobileCall					iMobileCall;
	/** Pointer to the RMobileLine object */
	RMobileLine					iMobileLine;
	/** Holds the TST name to be loaded */
	TName						iTSYName;
	/** For RTelServer connection */
	TBool						iTelServerConnected;
	/** For Phone module loading */
	TBool						iPhoneModuleLoaded;
	/** For RMobilePhone connection */
	TBool						iPhoneOpened;
	/** For RMobilePhone initialization */
	TBool						iPhoneInitialised;
	/** For RMobileLine connection */
	TBool						iLineOpened;
	/** For RMobileCall connection */
	TBool						iCallOpened;
	/** Holds RTelServer */
	RTelServer					iServer;
	RMobilePhone				iMobilePhone;
private:
	/** Pointer shared CTelephony object */
	CTelephony*					iTelephony;
	/** Holds pointer to the ActiveScheduler */
	CActiveScheduler*			iActiveScheduler;
};

#endif //__TEST_TELEPHONY_SHARED_DATA_H__
