// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test step declarations for the PhoneBookStore functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTPHONEBOOKSTORE_H__
#define __CCTSYINTEGRATIONTESTPHONEBOOKSTORE_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include <mpbutil.h>
#include "tsimtsytesthelper.h"
#include "tphonetsytesthelper.h"
#include "tnetworktsytesthelper.h"

#define KBufSize 100

class CEtelSessionMgr;


class CCTSYIntegrationTestPhoneBookStoreBase : public CCTSYIntegrationTestSuiteStepBase
/**
 * The base class for all phone book store tests.
 */
	{
public:

	CCTSYIntegrationTestPhoneBookStoreBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestPhoneBookStoreBase();

protected:
	TSimTsyTestHelper iSimTsyTestHelper;
	TPhoneTsyTestHelper iPhoneTsyTestHelper;
	TNetworkTsyTestHelper iNetworkTsyTestHelper;
	// Declare a TSY test helper here if required
	TBool EncodeTLVFormat(CPhoneBookBuffer* aPbBuffer, TBuf<KBufSize>& aNumber,
			TUint8 aNumberType, TBuf<KBufSize>& aName);
	TBool DecodeTLVFormat(CPhoneBookBuffer* aPbBuffer, TInt aBufLen, TBuf<KBufSize>& aNumber
			, TBuf<KBufSize>& aName, TInt aIndex);

	}; // class CCTSYIntegrationTestPhoneBookStoreBase


class CCTSYIntegrationTestPhoneBookStore0001Base : public CCTSYIntegrationTestPhoneBookStoreBase
/**
 * This class is the base class for a phone book store test and contains
 * the test procedure for a phone book test which is intended to be run
 * on a number of different phone books.
 *
 * A class should be derived from this class for each phone book that this
 * test should be run on.  That class should be the actual TestExecute test
 * case and should contain the actual doTestStepL function which will call
 * DoTestProcedure in this class.
 */
	{
public:

	CCTSYIntegrationTestPhoneBookStore0001Base(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0001Base();

	void DoTestProcedureL(TPhoneBookStoreId aPhoneBookId,RMobilePhoneBookStore::TMobilePhoneBookLocation aPhoneBookLocation,
						TUint32 aExpectedCaps);

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0001Base



class CCTSYIntegrationTestPhoneBookStore0002Base : public CCTSYIntegrationTestPhoneBookStoreBase
/**
 * This class is the base class for a phone book store test and contains
 * the test procedure for a phone book test which is intended to be run
 * on a number of different phone books.
 *
 * A class should be derived from this class for each phone book that this
 * test should be run on.  That class should be the actual TestExecute test
 * case and should contain the actual doTestStepL function which will call
 * DoTestProcedure in this class.
 */
	{
public:

	CCTSYIntegrationTestPhoneBookStore0002Base(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0002Base();

	void DoTestProcedureL(TPhoneBookStoreId aPhoneBookId);

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0002Base



class CCTSYIntegrationTestPhoneBookStore0003Base : public CCTSYIntegrationTestPhoneBookStoreBase
/**
 * This class is the base class for a phone book store test and contains
 * the test procedure for a phone book test which is intended to be run
 * on a number of different phone books.
 *
 * A class should be derived from this class for each phone book that this
 * test should be run on.  That class should be the actual TestExecute test
 * case and should contain the actual doTestStepL function which will call
 * DoTestProcedure in this class.
 */
	{
public:

	CCTSYIntegrationTestPhoneBookStore0003Base(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0003Base();

	void DoTestProcedureL(TPhoneBookStoreId aPhoneBookId);

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0003Base



class CCTSYIntegrationTestPhoneBookStore0004Base : public CCTSYIntegrationTestPhoneBookStoreBase
/**
 * This class is the base class for a phone book store test and contains
 * the test procedure for a phone book test which is intended to be run
 * on a number of different phone books.
 *
 * A class should be derived from this class for each phone book that this
 * test should be run on.  That class should be the actual TestExecute test
 * case and should contain the actual doTestStepL function which will call
 * DoTestProcedure in this class.
 */
	{
public:

	CCTSYIntegrationTestPhoneBookStore0004Base(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0004Base();

	void DoTestProcedureL(TPhoneBookStoreId aPhoneBookId);

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0004Base



class CCTSYIntegrationTestPhoneBookStore0005Base : public CCTSYIntegrationTestPhoneBookStoreBase
/**
 * This class is the base class for a phone book store test and contains
 * the test procedure for a phone book test which is intended to be run
 * on a number of different phone books.
 *
 * A class should be derived from this class for each phone book that this
 * test should be run on.  That class should be the actual TestExecute test
 * case and should contain the actual doTestStepL function which will call
 * DoTestProcedure in this class.
 */
	{
public:

	CCTSYIntegrationTestPhoneBookStore0005Base(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0005Base();

	void DoTestProcedureL(TFixedArray<TPhoneBookStoreId,2>& phoneBookTypeArray);

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0005Base



class CCTSYIntegrationTestPhoneBookStore0006Base : public CCTSYIntegrationTestPhoneBookStoreBase
/**
 * This class is the base class for a phone book store test and contains
 * the test procedure for a phone book test which is intended to be run
 * on a number of different phone books.
 *
 * A class should be derived from this class for each phone book that this
 * test should be run on.  That class should be the actual TestExecute test
 * case and should contain the actual doTestStepL function which will call
 * DoTestProcedure in this class.
 */
	{
public:

	CCTSYIntegrationTestPhoneBookStore0006Base(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0006Base();

	void DoTestProcedureL(TPhoneBookStoreId aPhoneBookId);

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0006Base



class CCTSYIntegrationTestPhoneBookStore0007Base : public CCTSYIntegrationTestPhoneBookStoreBase
/**
 * This class is the base class for a phone book store test and contains
 * the test procedure for a phone book test which is intended to be run
 * on a number of different phone books.
 *
 * A class should be derived from this class for each phone book that this
 * test should be run on.  That class should be the actual TestExecute test
 * case and should contain the actual doTestStepL function which will call
 * DoTestProcedure in this class.
 */
	{
public:

	CCTSYIntegrationTestPhoneBookStore0007Base(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0007Base();

	void DoTestProcedureL(TPhoneBookStoreId aPhoneBookId);

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0007Base



class CCTSYIntegrationTestPhoneBookStore0008Base : public CCTSYIntegrationTestPhoneBookStoreBase
/**
 * This class is the base class for a phone book store test and contains
 * the test procedure for a phone book test which is intended to be run
 * on a number of different phone books.
 *
 * A class should be derived from this class for each phone book that this
 * test should be run on.  That class should be the actual TestExecute test
 * case and should contain the actual doTestStepL function which will call
 * DoTestProcedure in this class.
 */
	{
public:

	CCTSYIntegrationTestPhoneBookStore0008Base(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0008Base();

	void DoTestProcedureL(TPhoneBookStoreId aPhoneBookId,const TDesC& aStoreName);

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0008Base



class CCTSYIntegrationTestPhoneBookStore0009Base : public CCTSYIntegrationTestPhoneBookStoreBase
/**
 * This class is the base class for a phone book store test and contains
 * the test procedure for a phone book test which is intended to be run
 * on a number of different phone books.
 *
 * A class should be derived from this class for each phone book that this
 * test should be run on.  That class should be the actual TestExecute test
 * case and should contain the actual doTestStepL function which will call
 * DoTestProcedure in this class.
 */
	{
public:

	CCTSYIntegrationTestPhoneBookStore0009Base(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0009Base();

	void DoTestProcedureL(TPhoneBookStoreId aPhoneBookId);

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0009Base



class CCTSYIntegrationTestPhoneBookStore0010Base : public CCTSYIntegrationTestPhoneBookStoreBase
/**
 * This class is the base class for a phone book store test and contains
 * the test procedure for a phone book test which is intended to be run
 * on a number of different phone books.
 *
 * A class should be derived from this class for each phone book that this
 * test should be run on.  That class should be the actual TestExecute test
 * case and should contain the actual doTestStepL function which will call
 * DoTestProcedure in this class.
 */
	{
public:

	CCTSYIntegrationTestPhoneBookStore0010Base(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0010Base();

	void DoTestProcedureL(TPhoneBookStoreId aPhoneBookId);

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0010Base



class CCTSYIntegrationTestPhoneBookStore0011Base : public CCTSYIntegrationTestPhoneBookStoreBase
/**
 * This class is the base class for a phone book store test and contains
 * the test procedure for a phone book test which is intended to be run
 * on a number of different phone books.
 *
 * A class should be derived from this class for each phone book that this
 * test should be run on.  That class should be the actual TestExecute test
 * case and should contain the actual doTestStepL function which will call
 * DoTestProcedure in this class.
 */
	{
public:

	CCTSYIntegrationTestPhoneBookStore0011Base(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0011Base();

	void DoTestProcedureL(TPhoneBookStoreId aPhoneBookId,const TDesC& aStoreName);

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0011Base



class CCTSYIntegrationTestPhoneBookStore0012Base : public CCTSYIntegrationTestPhoneBookStoreBase
/**
 * This class is the base class for a phone book store test and contains
 * the test procedure for a phone book test which is intended to be run
 * on a number of different phone books.
 *
 * A class should be derived from this class for each phone book that this
 * test should be run on.  That class should be the actual TestExecute test
 * case and should contain the actual doTestStepL function which will call
 * DoTestProcedure in this class.
 */
	{
public:

	CCTSYIntegrationTestPhoneBookStore0012Base(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0012Base();

	void DoTestProcedureL(TPhoneBookStoreId aPhoneBookId, const TDesC& aStoreName);

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0012Base



class CCTSYIntegrationTestPhoneBookStore0013Base : public CCTSYIntegrationTestPhoneBookStoreBase
/**
 * This class is the base class for a phone book store test and contains
 * the test procedure for a phone book test which is intended to be run
 * on a number of different phone books.
 *
 * A class should be derived from this class for each phone book that this
 * test should be run on.  That class should be the actual TestExecute test
 * case and should contain the actual doTestStepL function which will call
 * DoTestProcedure in this class.
 */
	{
public:

	CCTSYIntegrationTestPhoneBookStore0013Base(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0013Base();

	void DoTestProcedureL(TPhoneBookStoreId aPhoneBookId, const TDesC& aStoreName);

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0013Base



class CCTSYIntegrationTestPhoneBookStore0014Base : public CCTSYIntegrationTestPhoneBookStoreBase
/**
 * This class is the base class for a phone book store test and contains
 * the test procedure for a phone book test which is intended to be run
 * on a number of different phone books.
 *
 * A class should be derived from this class for each phone book that this
 * test should be run on.  That class should be the actual TestExecute test
 * case and should contain the actual doTestStepL function which will call
 * DoTestProcedure in this class.
 */
	{
public:

	CCTSYIntegrationTestPhoneBookStore0014Base(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0014Base();

	void DoTestProcedureL(TPhoneBookStoreId aPhoneBookId, const TDesC& aStoreName);

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0014Base



class CCTSYIntegrationTestPhoneBookStore0015Base : public CCTSYIntegrationTestPhoneBookStoreBase
/**
 * This class is the base class for a phone book store test and contains
 * the test procedure for a phone book test which is intended to be run
 * on a number of different phone books.
 *
 * A class should be derived from this class for each phone book that this
 * test should be run on.  That class should be the actual TestExecute test
 * case and should contain the actual doTestStepL function which will call
 * DoTestProcedure in this class.
 */
	{
public:

	CCTSYIntegrationTestPhoneBookStore0015Base(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0015Base();

	void DoTestProcedureL(TPhoneBookStoreId aPhoneBookId, const TDesC& aStoreName);

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0015Base



class CCTSYIntegrationTestPhoneBookStore0016Base : public CCTSYIntegrationTestPhoneBookStoreBase
/**
 * This class is the base class for a phone book store test and contains
 * the test procedure for a phone book test which is intended to be run
 * on a number of different phone books.
 *
 * A class should be derived from this class for each phone book that this
 * test should be run on.  That class should be the actual TestExecute test
 * case and should contain the actual doTestStepL function which will call
 * DoTestProcedure in this class.
 */
	{
public:

	CCTSYIntegrationTestPhoneBookStore0016Base(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0016Base();

	void DoTestProcedureL(TPhoneBookStoreId aPhoneBookId, const TDesC& aStoreName);

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0016Base



class CCTSYIntegrationTestPhoneBookStore0017Base : public CCTSYIntegrationTestPhoneBookStoreBase
/**
 * This class is the base class for a phone book store test and contains
 * the test procedure for a phone book test which is intended to be run
 * on a number of different phone books.
 *
 * A class should be derived from this class for each phone book that this
 * test should be run on.  That class should be the actual TestExecute test
 * case and should contain the actual doTestStepL function which will call
 * DoTestProcedure in this class.
 */
	{
public:

	CCTSYIntegrationTestPhoneBookStore0017Base(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0017Base();

	void DoTestProcedureL(TPhoneBookStoreId aPhoneBookId, const TDesC& aStoreName);

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0017Base



class CCTSYIntegrationTestPhoneBookStore0018Base : public CCTSYIntegrationTestPhoneBookStoreBase
/**
 * This class is the base class for a phone book store test and contains
 * the test procedure for a phone book test which is intended to be run
 * on a number of different phone books.
 *
 * A class should be derived from this class for each phone book that this
 * test should be run on.  That class should be the actual TestExecute test
 * case and should contain the actual doTestStepL function which will call
 * DoTestProcedure in this class.
 */
	{
public:

	CCTSYIntegrationTestPhoneBookStore0018Base(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0018Base();

	void DoTestProcedureL(TPhoneBookStoreId aPhoneBookId, const TDesC& aStoreName);

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0018Base



class CCTSYIntegrationTestPhoneBookStore0019Base : public CCTSYIntegrationTestPhoneBookStoreBase
/**
 * This class is the base class for a phone book store test and contains
 * the test procedure for a phone book test which is intended to be run
 * on a number of different phone books.
 *
 * A class should be derived from this class for each phone book that this
 * test should be run on.  That class should be the actual TestExecute test
 * case and should contain the actual doTestStepL function which will call
 * DoTestProcedure in this class.
 */
	{
public:

	CCTSYIntegrationTestPhoneBookStore0019Base(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0019Base();

	void DoTestProcedureL(TPhoneBookStoreId aPhoneBookId, const TDesC& aStoreName);

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0019Base



class CCTSYIntegrationTestPhoneBookStore0020Base : public CCTSYIntegrationTestPhoneBookStoreBase
/**
 * This class is the base class for a phone book store test and contains
 * the test procedure for a phone book test which is intended to be run
 * on a number of different phone books.
 *
 * A class should be derived from this class for each phone book that this
 * test should be run on.  That class should be the actual TestExecute test
 * case and should contain the actual doTestStepL function which will call
 * DoTestProcedure in this class.
 */
	{
public:

	CCTSYIntegrationTestPhoneBookStore0020Base(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0020Base();

	void DoTestProcedureL(TPhoneBookStoreId aPhoneBookId, const TDesC& aStoreName);

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0020Base



class CCTSYIntegrationTestPhoneBookStore0021Base : public CCTSYIntegrationTestPhoneBookStoreBase
/**
 * This class is the base class for a phone book store test and contains
 * the test procedure for a phone book test which is intended to be run
 * on a number of different phone books.
 *
 * A class should be derived from this class for each phone book that this
 * test should be run on.  That class should be the actual TestExecute test
 * case and should contain the actual doTestStepL function which will call
 * DoTestProcedure in this class.
 */
	{
public:

	CCTSYIntegrationTestPhoneBookStore0021Base(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0021Base();

	void DoTestProcedureL(TPhoneBookStoreId aPhoneBookId, const TDesC& aStoreName);

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0021Base



class CCTSYIntegrationTestPhoneBookStore0022Base : public CCTSYIntegrationTestPhoneBookStoreBase
/**
 * This class is the base class for a phone book store test and contains
 * the test procedure for a phone book test which is intended to be run
 * on a number of different phone books.
 *
 * A class should be derived from this class for each phone book that this
 * test should be run on.  That class should be the actual TestExecute test
 * case and should contain the actual doTestStepL function which will call
 * DoTestProcedure in this class.
 */
	{
public:

	CCTSYIntegrationTestPhoneBookStore0022Base(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0022Base();

	void DoTestProcedureL(TPhoneBookStoreId aPhoneBookId, const TDesC& aStoreName);

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0022Base



class CCTSYIntegrationTestPhoneBookStore0023Base : public CCTSYIntegrationTestPhoneBookStoreBase
/**
 * This class is the base class for a phone book store test and contains
 * the test procedure for a phone book test which is intended to be run
 * on a number of different phone books.
 *
 * A class should be derived from this class for each phone book that this
 * test should be run on.  That class should be the actual TestExecute test
 * case and should contain the actual doTestStepL function which will call
 * DoTestProcedure in this class.
 */
	{
public:

	CCTSYIntegrationTestPhoneBookStore0023Base(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0023Base();

	void DoTestProcedureL(TPhoneBookStoreId aPhoneBookId);

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0023Base



class CCTSYIntegrationTestPhoneBookStore0024Base : public CCTSYIntegrationTestPhoneBookStoreBase
/**
 * This class is the base class for a phone book store test and contains
 * the test procedure for a phone book test which is intended to be run
 * on a number of different phone books.
 *
 * A class should be derived from this class for each phone book that this
 * test should be run on.  That class should be the actual TestExecute test
 * case and should contain the actual doTestStepL function which will call
 * DoTestProcedure in this class.
 */
	{
public:

	CCTSYIntegrationTestPhoneBookStore0024Base(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0024Base();

	void DoTestProcedureL(TPhoneBookStoreId aPhoneBookId);

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0024Base



class CCTSYIntegrationTestPhoneBookStore0025Base : public CCTSYIntegrationTestPhoneBookStoreBase
/**
 * This class is the base class for a phone book store test and contains
 * the test procedure for a phone book test which is intended to be run
 * on a number of different phone books.
 *
 * A class should be derived from this class for each phone book that this
 * test should be run on.  That class should be the actual TestExecute test
 * case and should contain the actual doTestStepL function which will call
 * DoTestProcedure in this class.
 */
	{
public:

	CCTSYIntegrationTestPhoneBookStore0025Base(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0025Base();

	void DoTestProcedureL(TPhoneBookStoreId aPhoneBookId);

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0025Base



class CCTSYIntegrationTestPhoneBookStore0026Base : public CCTSYIntegrationTestPhoneBookStoreBase
/**
 * This class is the base class for a phone book store test and contains
 * the test procedure for a phone book test which is intended to be run
 * on a number of different phone books.
 *
 * A class should be derived from this class for each phone book that this
 * test should be run on.  That class should be the actual TestExecute test
 * case and should contain the actual doTestStepL function which will call
 * DoTestProcedure in this class.
 */
	{
public:

	CCTSYIntegrationTestPhoneBookStore0026Base(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0026Base();

	void DoTestProcedureL(TPhoneBookStoreId aPhoneBookId);

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0026Base



class CCTSYIntegrationTestPhoneBookStore0027Base : public CCTSYIntegrationTestPhoneBookStoreBase
/**
 * This class is the base class for a phone book store test and contains
 * the test procedure for a phone book test which is intended to be run
 * on a number of different phone books.
 *
 * A class should be derived from this class for each phone book that this
 * test should be run on.  That class should be the actual TestExecute test
 * case and should contain the actual doTestStepL function which will call
 * DoTestProcedure in this class.
 */
	{
public:

	CCTSYIntegrationTestPhoneBookStore0027Base(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0027Base();

	void DoTestProcedureL(TPhoneBookStoreId aPhoneBookId);

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0027Base



class CCTSYIntegrationTestPhoneBookStore0028Base : public CCTSYIntegrationTestPhoneBookStoreBase
/**
 * This class is the base class for a phone book store test and contains
 * the test procedure for a phone book test which is intended to be run
 * on a number of different phone books.
 *
 * A class should be derived from this class for each phone book that this
 * test should be run on.  That class should be the actual TestExecute test
 * case and should contain the actual doTestStepL function which will call
 * DoTestProcedure in this class.
 */
	{
public:

	CCTSYIntegrationTestPhoneBookStore0028Base(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0028Base();

	void DoTestProcedureL(TPhoneBookStoreId aPhoneBookId);

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0028Base



class CCTSYIntegrationTestPhoneBookStore0029Base : public CCTSYIntegrationTestPhoneBookStoreBase
/**
 * This class is the base class for a phone book store test and contains
 * the test procedure for a phone book test which is intended to be run
 * on a number of different phone books.
 *
 * A class should be derived from this class for each phone book that this
 * test should be run on.  That class should be the actual TestExecute test
 * case and should contain the actual doTestStepL function which will call
 * DoTestProcedure in this class.
 */
	{
public:

	CCTSYIntegrationTestPhoneBookStore0029Base(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0029Base();

	void DoTestProcedureL(TPhoneBookStoreId aPhoneBookId);

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0029Base

	
//
// Each individual test step that derives from one of the test scenario base 
// classes is declared below.
//

	class CCTSYIntegrationTestPhoneBookStore0001ADN : public CCTSYIntegrationTestPhoneBookStore0001Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0001ADN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0001ADN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0001ADN


class CCTSYIntegrationTestPhoneBookStore0001FDN : public CCTSYIntegrationTestPhoneBookStore0001Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0001FDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0001FDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0001FDN

class CCTSYIntegrationTestPhoneBookStore0001BDN : public CCTSYIntegrationTestPhoneBookStore0001Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0001BDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0001BDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0001BDN

class CCTSYIntegrationTestPhoneBookStore0001SDN : public CCTSYIntegrationTestPhoneBookStore0001Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0001SDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0001SDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0001SDN


class CCTSYIntegrationTestPhoneBookStore0001VMBX : public CCTSYIntegrationTestPhoneBookStore0001Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0001VMBX(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0001VMBX();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0001VMBX


class CCTSYIntegrationTestPhoneBookStore0001MBDN : public CCTSYIntegrationTestPhoneBookStore0001Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0001MBDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0001MBDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0001MBDN


class CCTSYIntegrationTestPhoneBookStore0002ADN : public CCTSYIntegrationTestPhoneBookStore0002Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0002ADN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0002ADN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0002ADN


class CCTSYIntegrationTestPhoneBookStore0002FDN : public CCTSYIntegrationTestPhoneBookStore0002Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0002FDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0002FDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0002FDN


class CCTSYIntegrationTestPhoneBookStore0002SDN : public CCTSYIntegrationTestPhoneBookStore0002Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0002SDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0002SDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0002SDN


class CCTSYIntegrationTestPhoneBookStore0002VMBX : public CCTSYIntegrationTestPhoneBookStore0002Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0002VMBX(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0002VMBX();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0002VMBX


class CCTSYIntegrationTestPhoneBookStore0002MBDN : public CCTSYIntegrationTestPhoneBookStore0002Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0002MBDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0002MBDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0002MBDN


class CCTSYIntegrationTestPhoneBookStore0003ADN : public CCTSYIntegrationTestPhoneBookStore0003Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0003ADN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0003ADN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0003ADN


class CCTSYIntegrationTestPhoneBookStore0003FDN : public CCTSYIntegrationTestPhoneBookStore0003Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0003FDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0003FDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0003FDN


class CCTSYIntegrationTestPhoneBookStore0003SDN : public CCTSYIntegrationTestPhoneBookStore0003Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0003SDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0003SDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0003SDN


class CCTSYIntegrationTestPhoneBookStore0003VMBX : public CCTSYIntegrationTestPhoneBookStore0003Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0003VMBX(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0003VMBX();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0003VMBX


class CCTSYIntegrationTestPhoneBookStore0003MBDN : public CCTSYIntegrationTestPhoneBookStore0003Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0003MBDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0003MBDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0003MBDN


class CCTSYIntegrationTestPhoneBookStore0004ADN : public CCTSYIntegrationTestPhoneBookStore0004Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0004ADN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0004ADN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0004ADN


class CCTSYIntegrationTestPhoneBookStore0004FDN : public CCTSYIntegrationTestPhoneBookStore0004Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0004FDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0004FDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0004FDN


class CCTSYIntegrationTestPhoneBookStore0004SDN : public CCTSYIntegrationTestPhoneBookStore0004Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0004SDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0004SDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0004SDN


class CCTSYIntegrationTestPhoneBookStore0004VMBX : public CCTSYIntegrationTestPhoneBookStore0004Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0004VMBX(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0004VMBX();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0004VMBX


class CCTSYIntegrationTestPhoneBookStore0004MBDN : public CCTSYIntegrationTestPhoneBookStore0004Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0004MBDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0004MBDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0004MBDN


class CCTSYIntegrationTestPhoneBookStore0005ADN : public CCTSYIntegrationTestPhoneBookStore0005Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0005ADN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0005ADN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0005ADN


class CCTSYIntegrationTestPhoneBookStore0005FDN : public CCTSYIntegrationTestPhoneBookStore0005Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0005FDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0005FDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0005FDN


class CCTSYIntegrationTestPhoneBookStore0005SDN : public CCTSYIntegrationTestPhoneBookStore0005Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0005SDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0005SDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0005SDN


class CCTSYIntegrationTestPhoneBookStore0005VMBX : public CCTSYIntegrationTestPhoneBookStore0005Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0005VMBX(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0005VMBX();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0005VMBX


class CCTSYIntegrationTestPhoneBookStore0005MBDN : public CCTSYIntegrationTestPhoneBookStore0005Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0005MBDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0005MBDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0005MBDN


class CCTSYIntegrationTestPhoneBookStore0006ADN : public CCTSYIntegrationTestPhoneBookStore0006Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0006ADN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0006ADN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0006ADN


class CCTSYIntegrationTestPhoneBookStore0006FDN : public CCTSYIntegrationTestPhoneBookStore0006Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0006FDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0006FDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0006FDN


class CCTSYIntegrationTestPhoneBookStore0006SDN : public CCTSYIntegrationTestPhoneBookStore0006Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0006SDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0006SDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0006SDN


class CCTSYIntegrationTestPhoneBookStore0006VMBX : public CCTSYIntegrationTestPhoneBookStore0006Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0006VMBX(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0006VMBX();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0006VMBX


class CCTSYIntegrationTestPhoneBookStore0006MBDN : public CCTSYIntegrationTestPhoneBookStore0006Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0006MBDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0006MBDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0006MBDN


class CCTSYIntegrationTestPhoneBookStore0007ADN : public CCTSYIntegrationTestPhoneBookStore0007Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0007ADN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0007ADN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0007ADN


class CCTSYIntegrationTestPhoneBookStore0007FDN : public CCTSYIntegrationTestPhoneBookStore0007Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0007FDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0007FDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0007FDN


class CCTSYIntegrationTestPhoneBookStore0007SDN : public CCTSYIntegrationTestPhoneBookStore0007Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0007SDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0007SDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0007SDN


class CCTSYIntegrationTestPhoneBookStore0007VMBX : public CCTSYIntegrationTestPhoneBookStore0007Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0007VMBX(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0007VMBX();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0007VMBX


class CCTSYIntegrationTestPhoneBookStore0007MBDN : public CCTSYIntegrationTestPhoneBookStore0007Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0007MBDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0007MBDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0007MBDN


class CCTSYIntegrationTestPhoneBookStore0008ADN : public CCTSYIntegrationTestPhoneBookStore0008Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0008ADN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0008ADN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0008ADN


class CCTSYIntegrationTestPhoneBookStore0008FDN : public CCTSYIntegrationTestPhoneBookStore0008Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0008FDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0008FDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0008FDN


class CCTSYIntegrationTestPhoneBookStore0008SDN : public CCTSYIntegrationTestPhoneBookStore0008Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0008SDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0008SDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0008SDN


class CCTSYIntegrationTestPhoneBookStore0008VMBX : public CCTSYIntegrationTestPhoneBookStore0008Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0008VMBX(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0008VMBX();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0008VMBX


class CCTSYIntegrationTestPhoneBookStore0008MBDN : public CCTSYIntegrationTestPhoneBookStore0008Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0008MBDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0008MBDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0008MBDN


class CCTSYIntegrationTestPhoneBookStore0009ADN : public CCTSYIntegrationTestPhoneBookStore0009Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0009ADN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0009ADN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0009ADN


class CCTSYIntegrationTestPhoneBookStore0009FDN : public CCTSYIntegrationTestPhoneBookStore0009Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0009FDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0009FDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0009FDN


class CCTSYIntegrationTestPhoneBookStore0009SDN : public CCTSYIntegrationTestPhoneBookStore0009Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0009SDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0009SDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0009SDN


class CCTSYIntegrationTestPhoneBookStore0009VMBX : public CCTSYIntegrationTestPhoneBookStore0009Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0009VMBX(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0009VMBX();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0009VMBX


class CCTSYIntegrationTestPhoneBookStore0009MBDN : public CCTSYIntegrationTestPhoneBookStore0009Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0009MBDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0009MBDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0009MBDN


class CCTSYIntegrationTestPhoneBookStore0010ADN : public CCTSYIntegrationTestPhoneBookStore0010Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0010ADN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0010ADN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0010ADN


class CCTSYIntegrationTestPhoneBookStore0010FDN : public CCTSYIntegrationTestPhoneBookStore0010Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0010FDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0010FDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0010FDN


class CCTSYIntegrationTestPhoneBookStore0010SDN : public CCTSYIntegrationTestPhoneBookStore0010Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0010SDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0010SDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0010SDN


class CCTSYIntegrationTestPhoneBookStore0010VMBX : public CCTSYIntegrationTestPhoneBookStore0010Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0010VMBX(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0010VMBX();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0010VMBX


class CCTSYIntegrationTestPhoneBookStore0010MBDN : public CCTSYIntegrationTestPhoneBookStore0010Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0010MBDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0010MBDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0010MBDN


class CCTSYIntegrationTestPhoneBookStore0011ADN : public CCTSYIntegrationTestPhoneBookStore0011Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0011ADN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0011ADN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0011ADN


class CCTSYIntegrationTestPhoneBookStore0011FDN : public CCTSYIntegrationTestPhoneBookStore0011Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0011FDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0011FDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0011FDN


class CCTSYIntegrationTestPhoneBookStore0011SDN : public CCTSYIntegrationTestPhoneBookStore0011Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0011SDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0011SDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0011SDN


class CCTSYIntegrationTestPhoneBookStore0011VMBX : public CCTSYIntegrationTestPhoneBookStore0011Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0011VMBX(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0011VMBX();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0011VMBX


class CCTSYIntegrationTestPhoneBookStore0011MBDN : public CCTSYIntegrationTestPhoneBookStore0011Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0011MBDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0011MBDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0011MBDN


class CCTSYIntegrationTestPhoneBookStore0012ADN : public CCTSYIntegrationTestPhoneBookStore0012Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0012ADN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0012ADN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0012ADN


class CCTSYIntegrationTestPhoneBookStore0012FDN : public CCTSYIntegrationTestPhoneBookStore0012Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0012FDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0012FDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0012FDN


class CCTSYIntegrationTestPhoneBookStore0012SDN : public CCTSYIntegrationTestPhoneBookStore0012Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0012SDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0012SDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0012SDN


class CCTSYIntegrationTestPhoneBookStore0012VMBX : public CCTSYIntegrationTestPhoneBookStore0012Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0012VMBX(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0012VMBX();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0012VMBX


class CCTSYIntegrationTestPhoneBookStore0012MBDN : public CCTSYIntegrationTestPhoneBookStore0012Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0012MBDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0012MBDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0012MBDN


class CCTSYIntegrationTestPhoneBookStore0013ADN : public CCTSYIntegrationTestPhoneBookStore0013Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0013ADN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0013ADN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0013ADN


class CCTSYIntegrationTestPhoneBookStore0013FDN : public CCTSYIntegrationTestPhoneBookStore0013Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0013FDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0013FDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0013FDN


class CCTSYIntegrationTestPhoneBookStore0013SDN : public CCTSYIntegrationTestPhoneBookStore0013Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0013SDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0013SDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0013SDN


class CCTSYIntegrationTestPhoneBookStore0013VMBX : public CCTSYIntegrationTestPhoneBookStore0013Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0013VMBX(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0013VMBX();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0013VMBX


class CCTSYIntegrationTestPhoneBookStore0013MBDN : public CCTSYIntegrationTestPhoneBookStore0013Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0013MBDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0013MBDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0013MBDN

class CCTSYIntegrationTestPhoneBookStore0014ADN : public CCTSYIntegrationTestPhoneBookStore0014Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0014ADN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0014ADN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0014ADN


class CCTSYIntegrationTestPhoneBookStore0014FDN : public CCTSYIntegrationTestPhoneBookStore0014Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0014FDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0014FDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0014FDN


class CCTSYIntegrationTestPhoneBookStore0014SDN : public CCTSYIntegrationTestPhoneBookStore0014Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0014SDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0014SDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0014SDN


class CCTSYIntegrationTestPhoneBookStore0014VMBX : public CCTSYIntegrationTestPhoneBookStore0014Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0014VMBX(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0014VMBX();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0014VMBX


class CCTSYIntegrationTestPhoneBookStore0014MBDN : public CCTSYIntegrationTestPhoneBookStore0014Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0014MBDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0014MBDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0014MBDN


class CCTSYIntegrationTestPhoneBookStore0015ADN : public CCTSYIntegrationTestPhoneBookStore0015Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0015ADN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0015ADN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0015ADN


class CCTSYIntegrationTestPhoneBookStore0015FDN : public CCTSYIntegrationTestPhoneBookStore0015Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0015FDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0015FDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0015FDN


class CCTSYIntegrationTestPhoneBookStore0015SDN : public CCTSYIntegrationTestPhoneBookStore0015Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0015SDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0015SDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0015SDN


class CCTSYIntegrationTestPhoneBookStore0015VMBX : public CCTSYIntegrationTestPhoneBookStore0015Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0015VMBX(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0015VMBX();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0015VMBX


class CCTSYIntegrationTestPhoneBookStore0015MBDN : public CCTSYIntegrationTestPhoneBookStore0015Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0015MBDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0015MBDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0015MBDN


class CCTSYIntegrationTestPhoneBookStore0016ADN : public CCTSYIntegrationTestPhoneBookStore0016Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0016ADN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0016ADN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0016ADN


class CCTSYIntegrationTestPhoneBookStore0016FDN : public CCTSYIntegrationTestPhoneBookStore0016Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0016FDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0016FDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0016FDN


class CCTSYIntegrationTestPhoneBookStore0016SDN : public CCTSYIntegrationTestPhoneBookStore0016Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0016SDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0016SDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0016SDN


class CCTSYIntegrationTestPhoneBookStore0016VMBX : public CCTSYIntegrationTestPhoneBookStore0016Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0016VMBX(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0016VMBX();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0016VMBX


class CCTSYIntegrationTestPhoneBookStore0016MBDN : public CCTSYIntegrationTestPhoneBookStore0016Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0016MBDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0016MBDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0016MBDN


class CCTSYIntegrationTestPhoneBookStore0017ADN : public CCTSYIntegrationTestPhoneBookStore0017Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0017ADN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0017ADN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0017ADN


class CCTSYIntegrationTestPhoneBookStore0017FDN : public CCTSYIntegrationTestPhoneBookStore0017Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0017FDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0017FDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0017FDN


class CCTSYIntegrationTestPhoneBookStore0017SDN : public CCTSYIntegrationTestPhoneBookStore0017Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0017SDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0017SDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0017SDN


class CCTSYIntegrationTestPhoneBookStore0017VMBX : public CCTSYIntegrationTestPhoneBookStore0017Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0017VMBX(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0017VMBX();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0017VMBX


class CCTSYIntegrationTestPhoneBookStore0017MBDN : public CCTSYIntegrationTestPhoneBookStore0017Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0017MBDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0017MBDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0017MBDN


class CCTSYIntegrationTestPhoneBookStore0018ADN : public CCTSYIntegrationTestPhoneBookStore0018Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0018ADN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0018ADN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0018ADN


class CCTSYIntegrationTestPhoneBookStore0018FDN : public CCTSYIntegrationTestPhoneBookStore0018Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0018FDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0018FDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0018FDN


class CCTSYIntegrationTestPhoneBookStore0018SDN : public CCTSYIntegrationTestPhoneBookStore0018Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0018SDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0018SDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0018SDN


class CCTSYIntegrationTestPhoneBookStore0018VMBX : public CCTSYIntegrationTestPhoneBookStore0018Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0018VMBX(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0018VMBX();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0018VMBX


class CCTSYIntegrationTestPhoneBookStore0018MBDN : public CCTSYIntegrationTestPhoneBookStore0018Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0018MBDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0018MBDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0018MBDN


class CCTSYIntegrationTestPhoneBookStore0019ADN : public CCTSYIntegrationTestPhoneBookStore0019Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0019ADN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0019ADN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0019ADN


class CCTSYIntegrationTestPhoneBookStore0019FDN : public CCTSYIntegrationTestPhoneBookStore0019Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0019FDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0019FDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0019FDN


class CCTSYIntegrationTestPhoneBookStore0019SDN : public CCTSYIntegrationTestPhoneBookStore0019Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0019SDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0019SDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0019SDN


class CCTSYIntegrationTestPhoneBookStore0019VMBX : public CCTSYIntegrationTestPhoneBookStore0019Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0019VMBX(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0019VMBX();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0019VMBX


class CCTSYIntegrationTestPhoneBookStore0019MBDN : public CCTSYIntegrationTestPhoneBookStore0019Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0019MBDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0019MBDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0019MBDN


class CCTSYIntegrationTestPhoneBookStore0020ADN : public CCTSYIntegrationTestPhoneBookStore0020Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0020ADN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0020ADN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0020ADN


class CCTSYIntegrationTestPhoneBookStore0020FDN : public CCTSYIntegrationTestPhoneBookStore0020Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0020FDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0020FDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0020FDN


class CCTSYIntegrationTestPhoneBookStore0020SDN : public CCTSYIntegrationTestPhoneBookStore0020Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0020SDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0020SDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0020SDN


class CCTSYIntegrationTestPhoneBookStore0020VMBX : public CCTSYIntegrationTestPhoneBookStore0020Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0020VMBX(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0020VMBX();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0020VMBX


class CCTSYIntegrationTestPhoneBookStore0020MBDN : public CCTSYIntegrationTestPhoneBookStore0020Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0020MBDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0020MBDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0020MBDN


class CCTSYIntegrationTestPhoneBookStore0021ADN : public CCTSYIntegrationTestPhoneBookStore0021Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0021ADN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0021ADN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0021ADN


class CCTSYIntegrationTestPhoneBookStore0021FDN : public CCTSYIntegrationTestPhoneBookStore0021Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0021FDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0021FDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0021FDN


class CCTSYIntegrationTestPhoneBookStore0021SDN : public CCTSYIntegrationTestPhoneBookStore0021Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0021SDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0021SDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0021SDN


class CCTSYIntegrationTestPhoneBookStore0021VMBX : public CCTSYIntegrationTestPhoneBookStore0021Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0021VMBX(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0021VMBX();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0021VMBX


class CCTSYIntegrationTestPhoneBookStore0021MBDN : public CCTSYIntegrationTestPhoneBookStore0021Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0021MBDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0021MBDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0021MBDN


class CCTSYIntegrationTestPhoneBookStore0022ADN : public CCTSYIntegrationTestPhoneBookStore0022Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0022ADN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0022ADN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0022ADN


class CCTSYIntegrationTestPhoneBookStore0022FDN : public CCTSYIntegrationTestPhoneBookStore0022Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0022FDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0022FDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0022FDN


class CCTSYIntegrationTestPhoneBookStore0022SDN : public CCTSYIntegrationTestPhoneBookStore0022Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0022SDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0022SDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0022SDN


class CCTSYIntegrationTestPhoneBookStore0022VMBX : public CCTSYIntegrationTestPhoneBookStore0022Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0022VMBX(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0022VMBX();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0022VMBX


class CCTSYIntegrationTestPhoneBookStore0022MBDN : public CCTSYIntegrationTestPhoneBookStore0022Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0022MBDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0022MBDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0022MBDN


class CCTSYIntegrationTestPhoneBookStore0023ADN : public CCTSYIntegrationTestPhoneBookStore0023Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0023ADN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0023ADN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0023ADN


class CCTSYIntegrationTestPhoneBookStore0023FDN : public CCTSYIntegrationTestPhoneBookStore0023Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0023FDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0023FDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0023FDN


class CCTSYIntegrationTestPhoneBookStore0023SDN : public CCTSYIntegrationTestPhoneBookStore0023Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0023SDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0023SDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0023SDN


class CCTSYIntegrationTestPhoneBookStore0023VMBX : public CCTSYIntegrationTestPhoneBookStore0023Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0023VMBX(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0023VMBX();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0023VMBX


class CCTSYIntegrationTestPhoneBookStore0023MBDN : public CCTSYIntegrationTestPhoneBookStore0023Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0023MBDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0023MBDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0023MBDN


class CCTSYIntegrationTestPhoneBookStore0024ADN : public CCTSYIntegrationTestPhoneBookStore0024Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0024ADN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0024ADN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0024ADN


class CCTSYIntegrationTestPhoneBookStore0024FDN : public CCTSYIntegrationTestPhoneBookStore0024Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0024FDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0024FDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0024FDN


class CCTSYIntegrationTestPhoneBookStore0024SDN : public CCTSYIntegrationTestPhoneBookStore0024Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0024SDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0024SDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0024SDN


class CCTSYIntegrationTestPhoneBookStore0024VMBX : public CCTSYIntegrationTestPhoneBookStore0024Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0024VMBX(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0024VMBX();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0024VMBX


class CCTSYIntegrationTestPhoneBookStore0024MBDN : public CCTSYIntegrationTestPhoneBookStore0024Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0024MBDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0024MBDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0024MBDN


class CCTSYIntegrationTestPhoneBookStore0025ADN : public CCTSYIntegrationTestPhoneBookStore0025Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0025ADN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0025ADN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0025ADN


class CCTSYIntegrationTestPhoneBookStore0025FDN : public CCTSYIntegrationTestPhoneBookStore0025Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0025FDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0025FDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0025FDN


class CCTSYIntegrationTestPhoneBookStore0025SDN : public CCTSYIntegrationTestPhoneBookStore0025Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0025SDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0025SDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0025SDN


class CCTSYIntegrationTestPhoneBookStore0025VMBX : public CCTSYIntegrationTestPhoneBookStore0025Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0025VMBX(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0025VMBX();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0025VMBX


class CCTSYIntegrationTestPhoneBookStore0025MBDN : public CCTSYIntegrationTestPhoneBookStore0025Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0025MBDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0025MBDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0025MBDN


class CCTSYIntegrationTestPhoneBookStore0026ADN : public CCTSYIntegrationTestPhoneBookStore0026Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0026ADN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0026ADN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0026ADN


class CCTSYIntegrationTestPhoneBookStore0026FDN : public CCTSYIntegrationTestPhoneBookStore0026Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0026FDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0026FDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0026FDN


class CCTSYIntegrationTestPhoneBookStore0026SDN : public CCTSYIntegrationTestPhoneBookStore0026Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0026SDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0026SDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0026SDN


class CCTSYIntegrationTestPhoneBookStore0026VMBX : public CCTSYIntegrationTestPhoneBookStore0026Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0026VMBX(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0026VMBX();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0026VMBX


class CCTSYIntegrationTestPhoneBookStore0026MBDN : public CCTSYIntegrationTestPhoneBookStore0026Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0026MBDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0026MBDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0026MBDN


class CCTSYIntegrationTestPhoneBookStore0027ADN : public CCTSYIntegrationTestPhoneBookStore0027Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0027ADN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0027ADN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0027ADN


class CCTSYIntegrationTestPhoneBookStore0027FDN : public CCTSYIntegrationTestPhoneBookStore0027Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0027FDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0027FDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0027FDN


class CCTSYIntegrationTestPhoneBookStore0027SDN : public CCTSYIntegrationTestPhoneBookStore0027Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0027SDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0027SDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0027SDN


class CCTSYIntegrationTestPhoneBookStore0027VMBX : public CCTSYIntegrationTestPhoneBookStore0027Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0027VMBX(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0027VMBX();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0027VMBX


class CCTSYIntegrationTestPhoneBookStore0027MBDN : public CCTSYIntegrationTestPhoneBookStore0027Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0027MBDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0027MBDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0027MBDN


class CCTSYIntegrationTestPhoneBookStore0028ADN : public CCTSYIntegrationTestPhoneBookStore0028Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0028ADN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0028ADN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0028ADN


class CCTSYIntegrationTestPhoneBookStore0028FDN : public CCTSYIntegrationTestPhoneBookStore0028Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0028FDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0028FDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0028FDN


class CCTSYIntegrationTestPhoneBookStore0028SDN : public CCTSYIntegrationTestPhoneBookStore0028Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0028SDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0028SDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0028SDN


class CCTSYIntegrationTestPhoneBookStore0028VMBX : public CCTSYIntegrationTestPhoneBookStore0028Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0028VMBX(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0028VMBX();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0028VMBX


class CCTSYIntegrationTestPhoneBookStore0028MBDN : public CCTSYIntegrationTestPhoneBookStore0028Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0028MBDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0028MBDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0028MBDN


class CCTSYIntegrationTestPhoneBookStore0029ADN : public CCTSYIntegrationTestPhoneBookStore0029Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0029ADN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0029ADN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0029ADN


class CCTSYIntegrationTestPhoneBookStore0029FDN : public CCTSYIntegrationTestPhoneBookStore0029Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0029FDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0029FDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0029FDN


class CCTSYIntegrationTestPhoneBookStore0029SDN : public CCTSYIntegrationTestPhoneBookStore0029Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0029SDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0029SDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0029SDN


class CCTSYIntegrationTestPhoneBookStore0029VMBX : public CCTSYIntegrationTestPhoneBookStore0029Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0029VMBX(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0029VMBX();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0029VMBX


class CCTSYIntegrationTestPhoneBookStore0029MBDN : public CCTSYIntegrationTestPhoneBookStore0029Base
	{
public:

	CCTSYIntegrationTestPhoneBookStore0029MBDN(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneBookStore0029MBDN();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneBookStore0029MBDN

	
	
	
#endif // __CCTSYINTEGRATIONTESTPHONEBOOKSTORE_H__
