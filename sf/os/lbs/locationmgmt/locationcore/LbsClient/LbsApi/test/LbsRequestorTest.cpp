// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "unittest.h"
#include "testregistry.h"
#include "test.h"
#include <LbsRequestor.h>
#include <s32mem.h>

//
// CRequestor Tests
//


TEST(CRequestor, NewL_Construction, "RQDE", 1)
	{
	_LIT(KDummyLit, "Hello");
	const CRequestor::TRequestorType KDummyRequestorType(CRequestor::ERequestorService);
	const CRequestor::TRequestorFormat KDummyRequestorFormat(CRequestor::EFormatTelephone);
	CRequestor* requestor = CRequestor::NewL(KDummyRequestorType, KDummyRequestorFormat, KDummyLit);

	TPtrC ptr = requestor->RequestorData();
	CHECK(!ptr.Compare(KDummyLit));
	CHECK_EQUAL(KDummyRequestorType, requestor->RequestorType());
	CHECK_EQUAL(KDummyRequestorFormat, requestor->RequestorFormat());

	delete requestor;
	}

TEST(CRequestor, NewLC_Construction, "RQDE", 1)
	{
	_LIT(KDummyLit, "Hello");
	const CRequestor::TRequestorType KDummyRequestorType(CRequestor::ERequestorService);
	const CRequestor::TRequestorFormat KDummyRequestorFormat(CRequestor::EFormatTelephone);
	CRequestor* requestor = CRequestor::NewLC(KDummyRequestorType, KDummyRequestorFormat, KDummyLit);

	TPtrC ptr = requestor->RequestorData();
	CHECK(!ptr.Compare(KDummyLit));
	CHECK_EQUAL(KDummyRequestorType, requestor->RequestorType());
	CHECK_EQUAL(KDummyRequestorFormat, requestor->RequestorFormat());

	CleanupStack::PopAndDestroy(requestor);
	}

TEST(CRequestor, New_Construction, "RQDE", 1)
	{
	_LIT(KDummyLit, "Hello");
	const CRequestor::TRequestorType KDummyRequestorType(CRequestor::ERequestorService);
	const CRequestor::TRequestorFormat KDummyRequestorFormat(CRequestor::EFormatTelephone);
	CRequestor* requestor =static_cast<CRequestor*>( User::LeaveIfNull(CRequestor::New(KDummyRequestorType, KDummyRequestorFormat, KDummyLit)));

	TPtrC ptr = requestor->RequestorData();
	CHECK(!ptr.Compare(KDummyLit));
	CHECK_EQUAL(KDummyRequestorType, requestor->RequestorType());
	CHECK_EQUAL(KDummyRequestorFormat, requestor->RequestorFormat());

	delete requestor;
	}

TEST(CRequestor, SetRequestorL, "RQDE", 2)
	{
	_LIT(KDummyLit, "Hello");
	const CRequestor::TRequestorType KDummyRequestorType(CRequestor::ERequestorService);
	const CRequestor::TRequestorFormat KDummyRequestorFormat(CRequestor::EFormatTelephone);
	CRequestor* requestor = CRequestor::NewLC(KDummyRequestorType, KDummyRequestorFormat, KDummyLit);

	//Go from a short descriptor to a longer one
	_LIT(KNewDummyLit, "Goodbye");
	const CRequestor::TRequestorType KNewDummyRequestorType(CRequestor::ERequestorContact);
	const CRequestor::TRequestorFormat KNewDummyRequestorFormat(CRequestor::EFormatUrl);
	requestor->SetRequestorL(KNewDummyRequestorType, KNewDummyRequestorFormat, KNewDummyLit);

	TPtrC ptr = requestor->RequestorData();
	CHECK(!ptr.Compare(KNewDummyLit));
	CHECK_EQUAL(KNewDummyRequestorType, requestor->RequestorType());
	CHECK_EQUAL(KNewDummyRequestorFormat, requestor->RequestorFormat());

	//Go from a long descriptor to a short one.
	requestor->SetRequestorL(KDummyRequestorType, KDummyRequestorFormat, KDummyLit);
	ptr.Set(requestor->RequestorData());
	CHECK(!ptr.Compare(KDummyLit));
	CHECK_EQUAL(KDummyRequestorType, requestor->RequestorType());
	CHECK_EQUAL(KDummyRequestorFormat, requestor->RequestorFormat());

	CleanupStack::PopAndDestroy(requestor);
	}

TEST(CRequestor, GetRequestor, "RQDE", 2)
	{
	_LIT(KDummyLit, "Hello");
	const CRequestor::TRequestorType KDummyRequestorType(CRequestor::ERequestorService);
	const CRequestor::TRequestorFormat KDummyRequestorFormat(CRequestor::EFormatTelephone);
	CRequestor* requestor = CRequestor::NewL(KDummyRequestorType, KDummyRequestorFormat, KDummyLit);

	CRequestor::TRequestorType requestorType;
	CRequestor::TRequestorFormat requestorFormat;
	TPtrC ptr;

	requestor->GetRequestor(requestorType, requestorFormat, ptr);

	CHECK(!ptr.Compare(KDummyLit));
	CHECK_EQUAL(KDummyRequestorType, requestorType);
	CHECK_EQUAL(KDummyRequestorFormat, requestorFormat);
	delete requestor;
	}

TEST(CRequestor, Internalizing_Externalizing, "RQST", 1)
	{
	//This test creates 2 different CRequestors, externalises them then
	//internalises them into the other one so a should look like b and 
	//b looks like a.
	_LIT(KDummyLit1, "Hello");
	const CRequestor::TRequestorType KDummyRequestorType1(CRequestor::ERequestorService);
	const CRequestor::TRequestorFormat KDummyRequestorFormat1(CRequestor::EFormatTelephone);
	CRequestor* requestor1 = CRequestor::NewLC(KDummyRequestorType1, KDummyRequestorFormat1, KDummyLit1);

	//Go from a short descriptor to a longer one
	_LIT(KDummyLit2, "Goodbye");
	const CRequestor::TRequestorType KDummyRequestorType2(CRequestor::ERequestorContact);
	const CRequestor::TRequestorFormat KDummyRequestorFormat2(CRequestor::EFormatUrl);
	CRequestor* requestor2 = CRequestor::NewLC(KDummyRequestorType2, KDummyRequestorFormat2, KDummyLit2);

	//Make a write stream
	HBufC8* buffer = HBufC8::NewLC(0x100);	//Should be plenty big enough
	TPtr8 ptr = buffer->Des();
	RDesWriteStream writeStream(ptr);	//Opens the write stream
	CleanupClosePushL(writeStream);
	writeStream << *requestor1;
	writeStream << *requestor2;
	writeStream.CommitL();
	CleanupStack::PopAndDestroy(&writeStream);	//Close the write stream

	RDesReadStream readStream(*buffer);
	CleanupClosePushL(readStream);
	readStream >> *requestor2;
	readStream >> *requestor1;
	CleanupStack::PopAndDestroy(&readStream);	//Close the readStream

	TPtrC ptr2 = requestor1->RequestorData();
	CHECK(!ptr2.Compare(KDummyLit2));
	CHECK_EQUAL(KDummyRequestorType2, requestor1->RequestorType());
	CHECK_EQUAL(KDummyRequestorFormat2, requestor1->RequestorFormat());

	ptr2.Set(requestor2->RequestorData());
	CHECK(!ptr2.Compare(KDummyLit1));
	CHECK_EQUAL(KDummyRequestorType1, requestor2->RequestorType());
	CHECK_EQUAL(KDummyRequestorFormat1, requestor2->RequestorFormat());

	CleanupStack::PopAndDestroy(3, requestor1);
	}

TEST(CRequestor, Construction_From_Stream, "RQST", 2)
	{
	_LIT(KDummyLit, "Hello");
	const CRequestor::TRequestorType KDummyRequestorType(CRequestor::ERequestorService);
	const CRequestor::TRequestorFormat KDummyRequestorFormat(CRequestor::EFormatTelephone);
	CRequestor* requestor1 = CRequestor::NewLC(KDummyRequestorType, KDummyRequestorFormat, KDummyLit);

	//Make a write stream
	HBufC8* buffer = HBufC8::NewLC(0x100);	//Should be plenty big enough
	TPtr8 ptr = buffer->Des();
	RDesWriteStream writeStream(ptr);	//Opens the write stream
	CleanupClosePushL(writeStream);
	writeStream << *requestor1;
	writeStream.CommitL();
	CleanupStack::PopAndDestroy(&writeStream);	//Close the write stream

	//Make a read stream
	RDesReadStream readStream(*buffer);
	CleanupClosePushL(readStream);

	CRequestor* requestor2 = CRequestor::NewL(readStream);

	CHECK(!TPtrC(KDummyLit).Compare(requestor2->RequestorData()));
	CHECK_EQUAL(requestor2->RequestorType(), KDummyRequestorType);
	CHECK_EQUAL(requestor2->RequestorFormat(), KDummyRequestorFormat);

	delete requestor2;
	CleanupStack::PopAndDestroy(3, requestor1);	//readStream, buffer, requestor1

	}

//
// RRequestorStack Tests
//

void ResetAndDestroyRequestorStack(TAny* aArray)
	{
	RRequestorStack* stack = static_cast<RRequestorStack*>(aArray);
	stack->ResetAndDestroy();
	}

TEST(RRequestorStack, Internalising_And_Externalising, "RSST", 1)
	{
	RRequestorStack requestorStack;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyRequestorStack, &requestorStack));

	_LIT(KDummyLit1, "Hello");
	const CRequestor::TRequestorType KDummyRequestorType1(CRequestor::ERequestorService);
	const CRequestor::TRequestorFormat KDummyRequestorFormat1(CRequestor::EFormatTelephone);

	CRequestor* requestor = CRequestor::NewLC(KDummyRequestorType1, KDummyRequestorFormat1, KDummyLit1);
	User::LeaveIfError(requestorStack.Append(requestor));	//take ownership of requestor
	CleanupStack::Pop(requestor);
	requestor = NULL;	//Just to make it clear we don't own requestor.

	_LIT(KDummyLit2, "Goodbye");
	const CRequestor::TRequestorType KDummyRequestorType2(CRequestor::ERequestorContact);
	const CRequestor::TRequestorFormat KDummyRequestorFormat2(CRequestor::EFormatUrl);
	
	requestor = CRequestor::NewLC(KDummyRequestorType2, KDummyRequestorFormat2, KDummyLit2);
	User::LeaveIfError(requestorStack.Append(requestor));	//take ownership of requestor
	CleanupStack::Pop(requestor);
	requestor = NULL;	//Just to make it clear we don't one requestor.

	//All info in requestorStack. Now stream out and back into a new requestor stack.

	CBufFlat* buffer = CBufFlat::NewL(512);	//Arbitrary size chosen
	CleanupStack::PushL(buffer);
	RBufWriteStream writeStream;
	writeStream.Open(*buffer);
	writeStream.PushL();
	writeStream << requestorStack;
	writeStream.CommitL();
	writeStream.Pop();
	//requestorStack now piped into buffer.
	//get rid of requestorStack.
	CleanupStack::Pop(buffer);
	CleanupStack::PopAndDestroy(&requestorStack);
	CleanupStack::PushL(buffer);

	RBufReadStream readStream;
	readStream.Open(*buffer);
	readStream.PushL();

	RRequestorStack requestorStack2;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyRequestorStack, &requestorStack2));
	readStream >> requestorStack2;
	//requestorStack2 should now be initialised.

	CHECK_EQUAL(2, requestorStack2.Count());

	requestor = requestorStack2[0];
	CHECK(!TPtrC(KDummyLit1).Compare(requestor->RequestorData()));
	CHECK_EQUAL(requestor->RequestorType(), KDummyRequestorType1);
	CHECK_EQUAL(requestor->RequestorFormat(), KDummyRequestorFormat1);

	requestor = requestorStack2[1];
	CHECK(!TPtrC(KDummyLit2).Compare(requestor->RequestorData()));
	CHECK_EQUAL(requestor->RequestorType(), KDummyRequestorType2);
	CHECK_EQUAL(requestor->RequestorFormat(), KDummyRequestorFormat2);
	
	CleanupStack::PopAndDestroy(3, buffer);
	}

void DoExternalizeWithNullEntryL()
	{
	RRequestorStack requestorStack;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyRequestorStack, &requestorStack));

	_LIT(KDummyLit1, "Hello");
	const CRequestor::TRequestorType KDummyRequestorType1(CRequestor::ERequestorService);
	const CRequestor::TRequestorFormat KDummyRequestorFormat1(CRequestor::EFormatTelephone);

	CRequestor* requestor = CRequestor::NewLC(KDummyRequestorType1, KDummyRequestorFormat1, KDummyLit1);
	User::LeaveIfError(requestorStack.Append(requestor));	//take ownership of requestor
	CleanupStack::Pop(requestor);
	requestor = NULL;	//Just to make it clear we don't own requestor.

	User::LeaveIfError(requestorStack.Append(requestor));	//stick a NULL entry in the stack.
	
	//Now do the streaming out.

	CBufFlat* buffer = CBufFlat::NewL(512);	//Arbitrary size chosen
	CleanupStack::PushL(buffer);
	RBufWriteStream writeStream;
	writeStream.Open(*buffer);
	writeStream.PushL();
	writeStream << requestorStack;	//should panic here.

	//This code should never run, but is put here for completeness.
	writeStream.CommitL();
	writeStream.Pop();
	CleanupStack::PopAndDestroy(buffer);
	CleanupStack::PopAndDestroy(&requestorStack);
	}

TInt ExternalizeWithNullEntry(TAny* /*aPtr*/)
	{
	CTrapCleanup* cleanupStack = CTrapCleanup::New();
	if(cleanupStack)
		{
		TRAPD(err, DoExternalizeWithNullEntryL());
		(void) err;	//ignore, we've failed if we get to this point.
		}

	delete cleanupStack;

	return KErrGeneral;
	}

TEST(RRequestorStack, Externalize_With_NULL_Entry, "RSST", 2)
	{
	DO_PANIC_TEST_L(ExternalizeWithNullEntry, KPosClientFault, EPositionNullRequestor,
	KDefaultTestTimeout)
	}
