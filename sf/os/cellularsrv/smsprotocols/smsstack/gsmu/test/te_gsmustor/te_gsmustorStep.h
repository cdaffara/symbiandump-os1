/**
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file
*/

#ifndef __TE_GSMUSTORSTEP_H__
#define __TE_GSMUSTORSTEP_H__

#include <e32math.h>
#include <es_sock.h>
#include "gsmustor.h"
#include "smspstor.h"
#include "smsuaddr.H"
#include "es_wsms.h"

#include <test/testexecutestepbase.h>
#include "te_gsmustor.h"

// __EDIT_ME__ - Create your own test step definitions

class CTgsmustorStepBase : public CTestStep
{
public:
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

protected:
	void ExpectFileToBeL(TBool aOpen, TBool aInTransaction, CSARStore* aStore);
	void BeginTransactionL(CSARStore& aStore);

#if 0
	void TestDumpSARStoreEntriesL(void);
	void DumpSAREntriesL(const TDesC& aName, TUid aUid);
#endif

	RFs iFs;
};


/**
 *  implements a dummy test version of the CSARStore
 *  other classes which also derives from CSARStore
 *  in the SmsStack is:
 *  
 *  r:\smsstack\smsprot\Inc\smspstor.h(41):class CSmsReassemblyStore : public CSARStore
 *  r:\smsstack\smsprot\Inc\smspstor.h(75):class CSmsSegmentationStore : public CSARStore
 *  r:\smsstack\wapprot\Inc\wappstor.h(21):class CWapReassemblyStore : public CSARStore
 */
class CTestSARStore : public CSARStore
	{
public:
	static CTestSARStore* NewL(RFs& aFs);
	~CTestSARStore();
	//
	void AddEntryL(const TSAREntry& aEntry);
	void ChangeEntryL(TInt aIndex,const TSAREntry& aNewEntry);
	void SetExtraStreamIdL(TStreamId aExtraStreamId);

private:
	CTestSARStore(RFs& aFs);
	};


class CTestGsmuStorFileOpenClose : public CTgsmustorStepBase
	{
public:
	TVerdict doTestStepL();
	};


class CTestGsmuStorFileOpenAfterAccess : public CTgsmustorStepBase
	{
public:
	TVerdict doTestStepL();
	};


class CTestGsmuStorTSAREntry : public CTgsmustorStepBase
	{
public:
	TVerdict doTestStepL();

private:
	void TestSAREntryL(TSAREntry& aEntry);
	void TestSmsReassemblyEntryL(TSmsReassemblyEntry& aEntry);
	void TestSmsSegmentationEntryL(TSmsSegmentationEntry& aEntry);
	void TestWapReassemblyEntryL(TWapReassemblyEntry& aEntry);
	};


class CTestGsmuStorCSARStoreClass : public CTgsmustorStepBase
	{
	TVerdict doTestStepL();
	};

#endif // __TE_GSMUSTORSTEP_H__
