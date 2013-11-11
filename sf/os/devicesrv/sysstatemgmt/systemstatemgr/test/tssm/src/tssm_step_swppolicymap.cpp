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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/
 

#include "ssmswppolicymap.h"
#include "tssm_step_swppolicymap.h"
#include "ssmdebug.h"

const TUint KItemNotExist = {0x666};


//--------------------------------------------------------------------

class CSsmSwpPolicyMapTestVersion : public CSsmSwpPolicyMap
	{
public:
	static CSsmSwpPolicyMapTestVersion* NewLC()
		{
		CSsmSwpPolicyMap* self = CSsmSwpPolicyMap::NewLC();
		return static_cast<CSsmSwpPolicyMapTestVersion*>(self);
		}
	static CSsmSwpPolicyMapTestVersion* NewL()
		{
		CSsmSwpPolicyMap* self = CSsmSwpPolicyMap::NewL();
		return static_cast<CSsmSwpPolicyMapTestVersion*>(self);
		}
	TUint SwpAtL(TInt aIndex) const;
	const TDesC& FilenameAtL(TInt aIndex) const;
	TInt Count() const;
	const CSsmSwpPolicyMapping* MappingAtIndexL(const TInt aIndex) const;
	};	
	
/**
Get the swp at the supplied index into the array
@param aIndex linear index into the array
@return The swp key at that index
@leave KErrNotFound if there is no item matching this index
@leave KErrArgument if the index is out of range
*/
TUint CSsmSwpPolicyMapTestVersion::SwpAtL(TInt aIndex) const
	{
	const CSsmSwpPolicyMapping* found = MappingAtIndexL(aIndex);
	return found->SwpKey();
	}

/**
Get the file name at the supplied index into the array
@param aIndex linear index into the array
@return A descriptor containing the file name associated with the swp
@leave KErrNotFound if there is no item matching this index
@leave KErrArgument if the index is out of range
*/
const TDesC& CSsmSwpPolicyMapTestVersion::FilenameAtL(TInt aIndex) const
	{
	const CSsmSwpPolicyMapping* found = MappingAtIndexL(aIndex);
	return found->Filename();
	}

/**
Get the number of objects in list

@return The number of objects in the list
*/
TInt CSsmSwpPolicyMapTestVersion::Count() const
	{
	return iItems.Count();
	}

/**
Get the mapping at the supplied index into the array
@param aIndex linear index into the array
@return The mapping object corresponding to the index
@leave KErrNotFound if there is no item matching this index
@leave KErrArgument if the index is out of range
*/
const CSsmSwpPolicyMapping* CSsmSwpPolicyMapTestVersion::MappingAtIndexL(TInt aIndex) const
	{
	if((aIndex >= iItems.Count()) || (aIndex < 0) )
		{
		//This macro will leave in release builds as well, only more verbose in debug builds
		SSMLOGLEAVE(KErrArgument);//lint !e527 Lint gets confused by the macro, no warning when the macro is expanded.
		}
	CSsmSwpPolicyMapping* const& ret = iItems[aIndex];
	return ret;
	}


//--------------------------------------------------------------------



/**
 Construct and register test class name with TEF
 */
CSsmSwpPolicyMapTest::CSsmSwpPolicyMapTest()
	{
	SetTestStepName (KTSsmSwpPolicyMapStep);
	}

/**
 Destroy common resources created at beginning of test
 */
CSsmSwpPolicyMapTest::~CSsmSwpPolicyMapTest()
	{
	}

//----------------- CTestStep implemenation --------------------

/**
 Create common resources for this test 
 Install CActiveScheduler necessary for the tests
 */
TVerdict CSsmSwpPolicyMapTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL ();
	}

/**
Old Test CaseID 		APPFWK-SSM-0010
New Test CaseID 		DEVSRVS-SSMA-SSM-0010
 */

TVerdict CSsmSwpPolicyMapTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CSsmSwpPolicyMapTest started...."));
	
	__UHEAP_MARK;
	
	doTestForMemoryLeaksL ();
	doTestHeapFailureL();
	doTestForInsertionOrderL();
	doTestForRandomAccessL();
	doTestItemMissingL();
	doTestDoubleInsertionL();

	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("....CSsmSwpPolicyMapTest completed!!"));
	return TestStepResult();
	}

/**
 Clean up if necessary
 */
TVerdict CSsmSwpPolicyMapTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL ();
	}

/**

 */
void CSsmSwpPolicyMapTest::doTestForMemoryLeaksL()
	{
	INFO_PRINTF1(_L("Checking for destructor memoryleaks in CSsmSwpPolicyMap"));

	__UHEAP_MARK;
	CSsmSwpPolicyMap* policyMap = CSsmSwpPolicyMap::NewL ();
	delete policyMap;
	__UHEAP_MARKEND;
	}


void CSsmSwpPolicyMapTest::doTestHeapFailureL()
	{
	INFO_PRINTF1(_L("Checking CSsmSwpPolicyMap for memoryleaks during OOM"));


	// CSsmSwpPolicyMap
	INFO_PRINTF1(_L("CSsmSwpPolicyMap construction under OOM"));
	CSsmSwpPolicyMapTestVersion* map=NULL;
	TInt failRate = 1;
	for (;;failRate++)
	    {
	    __UHEAP_RESET;
	    __UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
	    __UHEAP_MARK;

	    TRAPD(ret, map = CSsmSwpPolicyMapTestVersion::NewL());
	    
        TEST((ret==KErrNone || ret==KErrNoMemory));
	    if (ret!=KErrNone)
	        {
	        __UHEAP_MARKEND;
	        TEST(map==NULL);
	        }
	    else
	        {
	        TEST(map!=NULL);
			delete map;
	        map = NULL;
	        __UHEAP_MARKEND;
	        break;
	        }
	    }
	__UHEAP_RESET;
	INFO_PRINTF2(_L("  #allocs for c'tion: %d."),failRate-1);
	
	
	// AddL, FileL, SwpAtL, FilenameAtL
	INFO_PRINTF1(_L("CSsmSwpPolicyMap AddL, FileL, SwpAtL, FileAtL under OOM"));
	for (failRate = 1;;failRate++)
	    {
	    __UHEAP_RESET;
	    __UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
	    __UHEAP_MARK;

	    TRAPD(ret, 
	    	{
	    	CSsmSwpPolicyMapTestVersion* map = CSsmSwpPolicyMapTestVersion::NewLC();
			map->AddL(0, _L("OOM"));
			TUint swp = map->SwpAtL(0);
			const TDesC& file = map->FilenameL(swp);
			const TDesC& fileAt = map->FilenameAtL(0);
			CleanupStack::PopAndDestroy (map);
	    	}) //lint !e529 Suppress Symbol 'file' not subsequently referenced
	    	
        __UHEAP_MARKEND;
	    
        TEST((ret==KErrNone || ret==KErrNoMemory));
	    if (ret==KErrNone)
	        {
	        break;
	        }
	    }
	__UHEAP_RESET;
	INFO_PRINTF2(_L("  #allocs for AddL, FileL, SwpAtL, FileAtL: %d."),failRate-1);
	}
	
	
void CSsmSwpPolicyMapTest::doTestForInsertionOrderL()
	{
	INFO_PRINTF1(_L("Checking CSsmSwpPolicyMap for item insertion ordering"));

	__UHEAP_MARK;
	CSsmSwpPolicyMapTestVersion* map = CSsmSwpPolicyMapTestVersion::NewLC();
	
	// insert the items out of order
	const TUint nine = {9};	map->AddL(nine, _L("0009"));
							map->AddL(0, _L("0000"));
	const TUint eight = {8};	map->AddL(eight, _L("0008"));
	const TUint one = {1};	map->AddL(one, _L("0001"));
	const TUint seven = {7};	map->AddL(seven, _L("0007"));
	const TUint two = {2};	map->AddL(two, _L("0002"));
	const TUint six = {6};	map->AddL(six, _L("0006"));
	const TUint three = {3};	map->AddL(three, _L("0003"));
	const TUint five = {5};	map->AddL(five, _L("0005"));
	const TUint four = {4};	map->AddL(four, _L("0004"));

	TInt idx = 0;
	for(;idx < map->Count(); idx++)
		{
		// make sure the numbers item
		const TUint key = {idx};
		TEST(key == map->SwpAtL(idx));

		// generate the expected file
		TBuf<16> num;
		num.Format(_L("%04d"), idx);
		const TDesC& fileAt = map->FilenameAtL(idx);
		TEST(0 == fileAt.Compare(num));		
		}

	CleanupStack::PopAndDestroy (map);

	__UHEAP_MARKEND;
	}

void CSsmSwpPolicyMapTest::doTestForRandomAccessL()
	{
	INFO_PRINTF1(_L("Checking CSsmSwpPolicyMap for item access"));

	__UHEAP_MARK;
	CSsmSwpPolicyMapTestVersion* map = CSsmSwpPolicyMapTestVersion::NewLC();
	
	// insert the items out of order
	const TUint nine = {9};	map->AddL(nine, _L("0009"));
							map->AddL(0, _L("0000"));
	const TUint eight = {8};	map->AddL(eight, _L("0008"));
	const TUint one = {1};	map->AddL(one, _L("0001"));
	const TUint seven = {7};	map->AddL(seven, _L("0007"));
	const TUint two = {2};	map->AddL(two, _L("0002"));
	const TUint six = {6};	map->AddL(six, _L("0006"));
	const TUint three = {3};	map->AddL(three, _L("0003"));
	const TUint five = {5};	map->AddL(five, _L("0005"));
	const TUint four = {4};	map->AddL(four, _L("0004"));

	for(TInt swp = 0; swp < 10; swp++)
		{
		// generate the expected file
		TBuf<16> num;
		num.Format(_L("%04d"), swp);
		// get the file name and check it corresponds to the correct swp
		const TUint key = {swp};
		const TDesC& file = map->FilenameL(key);
		TEST(0 == file.Compare(num));
		}

	CleanupStack::PopAndDestroy (map);

	__UHEAP_MARKEND;
	}

void CSsmSwpPolicyMapTest::doTestItemMissingL()
	{
	INFO_PRINTF1(_L("Checking CSsmSwpPolicyMap for missing item"));

	__UHEAP_MARK;
	CSsmSwpPolicyMapTestVersion* map = CSsmSwpPolicyMapTestVersion::NewLC();
	
	// insert a few items
	const TUint nine = {9};	map->AddL(nine, _L("0009"));
							map->AddL(0, _L("000"));
	const TUint eight = {8};	map->AddL(eight, _L("0008"));
	const TUint one = {1};	map->AddL(one, _L("0001"));
	const TUint seven = {7};	map->AddL(seven, _L("0007"));
	const TUint two = {2};	map->AddL(two, _L("0002"));
	const TUint six = {6};	map->AddL(six, _L("0006"));
	const TUint three = {3};	map->AddL(three, _L("0003"));
	const TUint five = {5};	map->AddL(five, _L("0005"));
	const TUint four = {4};	map->AddL(four, _L("0004"));

	TRAPD(err, doItemMissingL(map));
	TEST(err == KErrNotFound);

	CleanupStack::PopAndDestroy (map);

	__UHEAP_MARKEND;
	}

void CSsmSwpPolicyMapTest::doItemMissingL(CSsmSwpPolicyMap* aSwpPolicyMap)
	{
	const TDesC& desc = aSwpPolicyMap->FilenameL(KItemNotExist);
	} //lint !e529 Symbol 'desc' not subsequently referenced

void CSsmSwpPolicyMapTest::doTestDoubleInsertionL()
	{
	INFO_PRINTF1(_L("Checking CSsmSwpPolicyMap for double insertion"));

	__UHEAP_MARK;
	CSsmSwpPolicyMapTestVersion* map = CSsmSwpPolicyMapTestVersion::NewLC();

	INFO_PRINTF1(_L("--same TSsmSwp item twice"));
	TRAPD(err, doDoubleInsertionL(map))
	TEST(err == KErrAlreadyExists);
	
	INFO_PRINTF1(_L("--different TSsmSwp items with same file name"));
	const TUint key1 = {0x56};
	const TUint key2 = {0x57};
	map->AddL(key1, _L("same"));
	map->AddL(key2, _L("same"));

	// Check for consistency
	TUint swp = {0x55};
	const TDesC& file1 = map->FilenameL(swp);
	swp++;
	TEST(0 == file1.Compare(_L("first")));
	const TDesC& file2 = map->FilenameL(swp);
	swp++;
	TEST(0 == file2.Compare(_L("same")));
	const TDesC& file3 = map->FilenameL(swp);
	TEST(0 == file3.Compare(_L("same")));
	
	CleanupStack::PopAndDestroy (map);

	__UHEAP_MARKEND;
	}

void CSsmSwpPolicyMapTest::doDoubleInsertionL(CSsmSwpPolicyMap* aSwpPolicyMap)
	{
	const TUint key = {0x55};
	aSwpPolicyMap->AddL(key, _L("first"));
	aSwpPolicyMap->AddL(key, _L("second"));
	}
