// TestTelephonyAsyncOperation.h
//
// Copyright (c) 2004-2008 Symbian Software Ltd.  All rights reserved.
//
// Active object derived class for asynchronous operation
//


#if (!defined __TEST_TELEPHONY_ASYNC_OPERATION_H__)
#define  __TEST_TELEPHONY_ASYNC_OPERATION_H__

// EPOC include
#include <e32base.h>
#include <etel3rdparty.h>

/*@{*/
///constants
enum TTestAsyncOperation
	{
	KNotifyLineStatusChange,
	};
/*@}*/

class CTelephonyAsyncOperation : public CActive
/**
@internalAll
@test
@class CTelephonyAsyncOperation

Active object for executing the asynchronous requests. 
*/
	{
public:
	CTelephonyAsyncOperation(CTestStep& aTestStep);
	~CTelephonyAsyncOperation();

	virtual void	DoCancel();
	virtual void	RunL();

	TInt	SetOperation();
	
private:
	CTestStep&	iTestStep;
	};

#endif /* __TEST_TELEPHONY_ASYNC_OPERATION_H__ */
