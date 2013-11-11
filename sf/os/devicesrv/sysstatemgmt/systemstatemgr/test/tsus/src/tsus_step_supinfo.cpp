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

#include <s32mem.h>
#include <ssm/ssmsuscli.h>

#include "tsus_step_supinfo.h"


//
// TestStep
//


_LIT(KFileName, "abcdefgh.txt");
const TUid KId = {12345};
const TInt KOrdinal = 27;

CSsmSupInfoTest::~CSsmSupInfoTest()
	{
	}

CSsmSupInfoTest::CSsmSupInfoTest()
	{
	SetTestStepName(KTSsmSupInfoStep);
	}

/** */
TVerdict CSsmSupInfoTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

/** 
 Old Test CaseID 		APPFWK-SUS-0001
 New Test CaseID 		DEVSRVS-SSMA-SUS-0001
 */

TVerdict CSsmSupInfoTest::doTestStepL()
	{
	doTestStructL();
	doTestSerializationL();
	return TestStepResult();
	}

/** */
TVerdict CSsmSupInfoTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

/** */
void CSsmSupInfoTest::doTestStructL()
	{
	INFO_PRINTF1(_L("Testing all public methods except Internalize/Externalize"));
	TSsmSupInfo empty;
	TEST(empty.FileName() == KNullDesC);
	TEST(empty.Identity() == KNullUid);
	TEST(empty.NewLOrdinal() == 1);
	
	TSsmSupInfo nameOnly(KFileName);
	TEST(nameOnly.FileName() == KFileName);
	TEST(nameOnly.Identity() == KNullUid);
	TEST(nameOnly.NewLOrdinal() == 1);
	
	TSsmSupInfo nameAndId(KFileName, KOrdinal);
	TEST(nameAndId.FileName() == KFileName);
	TEST(nameAndId.Identity() == KNullUid);
	TEST(nameAndId.NewLOrdinal() == KOrdinal);
	
	TSsmSupInfo all(KFileName, KOrdinal, KId);
	TEST(all.FileName() == KFileName);
	TEST(all.Identity() == KId);
	TEST(all.NewLOrdinal() == KOrdinal);
	
	empty.SetL(KFileName, KOrdinal, KId);
	TEST(empty.FileName() == KFileName);
	TEST(empty.Identity() == KId);
	TEST(empty.NewLOrdinal() == KOrdinal);
	}

/** */
void CSsmSupInfoTest::doTestSerializationL()
	{
	INFO_PRINTF1(_L("Testing Internalize/Externalize"));
	
	// Create a supinfo with non default values
	TSsmSupInfo supinfo;
	supinfo.SetL(KFileName, KOrdinal, KId);
	
	// Externalize it
	CBufFlat* buf = CBufFlat::NewL( 1024 );
	CleanupStack::PushL( buf );	
	RBufWriteStream writeStream( *buf );
	CleanupClosePushL( writeStream );	
	supinfo.ExternalizeL( writeStream );
	CleanupStack::PopAndDestroy( &writeStream );
	
	// Create a new startupprop instance 
	TSsmSupInfo second_supinfo;
	
	// Internalize the data
	RBufReadStream readStream( *buf );
	CleanupClosePushL( readStream );	
	second_supinfo.InternalizeL( readStream );
	CleanupStack::PopAndDestroy( &readStream );
	
	// And TEST that all members was read back successfully
	TEST(supinfo.FileName() == second_supinfo.FileName());
	TEST(supinfo.Identity() == second_supinfo.Identity());
	TEST(supinfo.NewLOrdinal() == second_supinfo.NewLOrdinal());
	
	// Externalize the second instance
	CBufFlat* buf2 = CBufFlat::NewL( 1024 );
	CleanupStack::PushL( buf2 );	
	RBufWriteStream writeStream2( *buf2 );
	CleanupClosePushL( writeStream2 );	
	second_supinfo.ExternalizeL( writeStream2 );
	CleanupStack::PopAndDestroy( &writeStream2 );
	
	// Assert that the first and second buffer have the same contents
	TEST(buf->Ptr(0) == buf2->Ptr(0));
	
	CleanupStack::PopAndDestroy( buf2 );
	CleanupStack::PopAndDestroy( buf );
	}
