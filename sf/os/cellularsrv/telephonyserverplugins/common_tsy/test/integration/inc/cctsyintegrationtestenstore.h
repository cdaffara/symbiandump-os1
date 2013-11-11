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
// Test step declarations for the ENStore functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTENSTORE_H__
#define __CCTSYINTEGRATIONTESTENSTORE_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include "tsimtsytesthelper.h"
#include "tphonetsytesthelper.h"

class CEtelSessionMgr;


class CCTSYIntegrationTestENStoreBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestENStoreBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestENStoreBase();

protected:

	// Declare a TSY test helper here if required
	TSimTsyTestHelper iSimTsyTestHelper;
	TPhoneTsyTestHelper iPhoneTsyTestHelper;
	}; // class CCTSYIntegrationTestENStoreBase


class CCTSYIntegrationTestENStore0001 : public CCTSYIntegrationTestENStoreBase
	{
public:

	CCTSYIntegrationTestENStore0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestENStore0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestENStore0001


class CCTSYIntegrationTestENStore0002 : public CCTSYIntegrationTestENStoreBase
	{
public:

	CCTSYIntegrationTestENStore0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestENStore0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestENStore0002


class CCTSYIntegrationTestENStore0003 : public CCTSYIntegrationTestENStoreBase
	{
public:

	CCTSYIntegrationTestENStore0003(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestENStore0003();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestENStore0003


class CCTSYIntegrationTestENStore0004 : public CCTSYIntegrationTestENStoreBase
	{
public:

	CCTSYIntegrationTestENStore0004(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestENStore0004();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestENStore0004


class CCTSYIntegrationTestENStore0005 : public CCTSYIntegrationTestENStoreBase
	{
public:

	CCTSYIntegrationTestENStore0005(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestENStore0005();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestENStore0005


class CCTSYIntegrationTestENStore0006 : public CCTSYIntegrationTestENStoreBase
	{
public:

	CCTSYIntegrationTestENStore0006(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestENStore0006();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestENStore0006


class CCTSYIntegrationTestENStore0007 : public CCTSYIntegrationTestENStoreBase
	{
public:

	CCTSYIntegrationTestENStore0007(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestENStore0007();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestENStore0007


class CCTSYIntegrationTestENStore0008 : public CCTSYIntegrationTestENStoreBase
	{
public:

	CCTSYIntegrationTestENStore0008(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestENStore0008();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestENStore0008


class CCTSYIntegrationTestENStore0009 : public CCTSYIntegrationTestENStoreBase
	{
public:

	CCTSYIntegrationTestENStore0009(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestENStore0009();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestENStore0009


class CCTSYIntegrationTestENStore0010 : public CCTSYIntegrationTestENStoreBase
	{
public:

	CCTSYIntegrationTestENStore0010(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestENStore0010();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestENStore0010


class CCTSYIntegrationTestENStore0011 : public CCTSYIntegrationTestENStoreBase
	{
public:

	CCTSYIntegrationTestENStore0011(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestENStore0011();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestENStore0011


class CCTSYIntegrationTestENStore0012 : public CCTSYIntegrationTestENStoreBase
	{
public:

	CCTSYIntegrationTestENStore0012(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestENStore0012();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestENStore0012


class CCTSYIntegrationTestENStore0013 : public CCTSYIntegrationTestENStoreBase
	{
public:

	CCTSYIntegrationTestENStore0013(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestENStore0013();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestENStore0013



#endif // __CCTSYINTEGRATIONTESTENSTORE_H__
