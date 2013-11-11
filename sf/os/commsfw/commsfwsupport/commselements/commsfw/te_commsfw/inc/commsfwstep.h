/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file CommsFWStep.h
*/
#if (!defined __COMMSFWSTEP_H__)
#define __COMMSFWSTEP_H__

#include <test/testexecutestepbase.h>
#include <test/testexecuteserverbase.h>
#include <cflog.h>
#include <cfthread.h>
#include "CommsFWServer.h"
#include "TestUtilities.h"

using namespace CommsFW;

#define TESTEQ(LHS, RHS, MSG, MSGVAL)	\
	if((LHS) != (RHS))					\
		{								\
		this->ERR_PRINTF2(_L(MSG), MSGVAL);	\
		User::Leave(KErrAbort);			\
		}
#define TESTTRUE(COND, MSG)	\
	if(!(COND))							\
		{								\
		this->ERR_PRINTF1(_L(MSG));			\
		User::Leave(KErrAbort);			\
		}


class CEasyTestStep : public CTestStep
	{
public:
	virtual TVerdict doTestStepPreambleL()
		{
		MergeVerdict(easyTestStepPreambleL());
		return TestStepResult(); 
		}
	virtual TVerdict doTestStepL()
		{
		MergeVerdict(easyTestStepL());
		return TestStepResult(); 
		}
	virtual TVerdict doTestStepPostambleL()
		{
		MergeVerdict(easyTestStepPostambleL());
		return TestStepResult(); 
		}

	virtual TVerdict easyTestStepPreambleL()
		{ return EPass; }
	virtual TVerdict easyTestStepL() = 0;
	virtual TVerdict easyTestStepPostambleL()
		{ return EPass; }

	void MergeVerdict(TVerdict aVerdict)
		{
		if(TestStepResult() == EPass)
			SetTestStepResult(aVerdict);
		}
	};



class CCFTestBase : public CEasyTestStep
	{
public:
	// CTestStep virtuals
	virtual TVerdict easyTestStepPreambleL();
	virtual TVerdict easyTestStepPostambleL();

	void LeaveUnlessPassedL(TVerdict aVerdict);
	~CCFTestBase();
private:
	CActiveScheduler* iScheduler;
	};


// Open/close base provider
template <class RChanClass>
class CCFTestCommsChannel : public CCFTestBase
	{
public:

protected:
	RChanClass* iUpper;
	RChanClass* iLower;
	};

// Open/close helper; avoids multiple inheritance
template <class RChanClass>
class TUniDirectionalOpenClose
	{
public:
	TUniDirectionalOpenClose(CTestExecuteLogger& aLogger)
		: iLogger(aLogger)
		{ }
	TVerdict CreateEndpointsL(RChanClass*& aUpper, RChanClass*& aLower);
	TVerdict CloseEndpointsL(RChanClass*& aUpper, RChanClass*& aLower);
private:
	inline CTestExecuteLogger& Logger() {return iLogger;}

	CTestExecuteLogger& iLogger;
	};




// Single priority base provider
template <class RChanClass>
class CTestSP : public CCFTestCommsChannel<RChanClass>
	{
public:
	virtual TVerdict easyTestStepL();
private:
	// Might need to be re-implemented for bidirectional case
	virtual TVerdict RxTxWithoutNotificationsL();
	virtual TVerdict RxTxWithNotificationsL();
	virtual TVerdict RxTxCancelNotificationsL();
	};

// Unidirectional single priority tests
class CTestUDSP : public CTestSP<RChanUDSP>
	{
public:
	// CTestStep virtuals
	virtual TVerdict easyTestStepPreambleL();
	virtual TVerdict easyTestStepPostambleL();
	};

// Unidirectional triple priority tests
class CTestUDTP : public CCFTestCommsChannel<RChanUDTP>
	{
public:
	virtual TVerdict easyTestStepL();

	// CTestStep virtuals
	virtual TVerdict easyTestStepPreambleL();
	virtual TVerdict easyTestStepPostambleL();
private:
	TVerdict RxTxWithoutNotificationsL();
	TVerdict RxTxWithNotificationsL();
	TVerdict RxTxCancelNotificationsL();
	TVerdict OOMCreationAndCloseL();
	void DoOOMCreationL();
	void DoOOMClose();
	};

// Bidirectional single priority tests
class CTestBDSP : public CTestSP<RChanBDSP>
	{
public:
	virtual TVerdict easyTestStepPreambleL()
		{
		LeaveUnlessPassedL(CTestSP<RChanBDSP>::easyTestStepPreambleL());
		return CreateEndpointsL();
		}
	virtual TVerdict easyTestStepL();

private:
	TVerdict CreateEndpointsL();
	TVerdict GracefulCloseWithoutTimeoutL();
	TVerdict NonFullQueueGracefulCloseWithTimeoutL();
	TVerdict FullQueueGracefulCloseWithTimeoutL();
	TVerdict FullQueueGracefulCloseWithZeroLengthTimeoutL();
	TVerdict OOMNonFullQueueGracefulCloseWithTimeoutL();
	TVerdict SimultaneousGracefulCloseL();

	TVerdict GracefulCloseCoreL(TBool aFullQueue, TInt aTimeout, TBool aOOMLoop);
	};


class CTestPanic : public CCFTestBase
	{
public:
	virtual TVerdict easyTestStepL();
private:
	TVerdict DuplicateCreationAsUpperL();
	TVerdict DuplicateCreationAsLowerL();
	TVerdict CreationAsSenderAndReceiverL();
	TVerdict DestructionOfOpenRecvEndpointL();
	TVerdict DestructionOfOpenSendEndpointL();
	TVerdict SendAboveUpperPriorityBoundL();
	TVerdict NotifySpaceBelowLowerPriorityBoundL();
	TVerdict UDTPNestedNotifyDataAvailableL();
	TVerdict UDSPNestedNotifyDataAvailableL();
	TVerdict UDSPNestedNotifySpaceAvailableL();
	};

class CTestRCFThread : public CCFTestBase
	{
public:
	virtual TVerdict easyTestStepL();
private:
	static TInt TestEntrypoint(TAny*);
	};

class CTestTCFSubModuleAddress : public CEasyTestStep
	{
public:
	TVerdict easyTestStepL();
	};



#include "commsfwstep.inl"

#endif	// __COMMSFWSTEP_H__

