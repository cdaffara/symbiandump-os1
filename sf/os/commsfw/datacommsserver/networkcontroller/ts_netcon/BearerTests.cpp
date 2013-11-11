// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "BearerTests.h"
#include "CTelBearer.h"
#include "NetConError.h"

//
//  Test case NC-4-1
//

CNetConTest0401* CNetConTest0401::NewLC()
	{
	CNetConTest0401* self = new(ELeave) CNetConTest0401();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0401::CNetConTest0401()
	{
	}

void CNetConTest0401::ConstructL()
	{

	CNetConBearerTestBase::ConstructL(); // base class construct
	}

CNetConTest0401::~CNetConTest0401()
	{
	}

const TDesC& CNetConTest0401::Name() const
	{

	_LIT(KTestName, "Test Case NC-4-1");
	return KTestName();
	}

void CNetConTest0401::StartTestL()
	{

	iTelBearer->StartChecking(EFalse);
	iTelBearer->StopChecking();
	CompleteTest(MNetConTest::EPassed);
	}


//
//  Test case NC-4-2
//

CNetConTest0402* CNetConTest0402::NewLC()
	{
	CNetConTest0402* self = new(ELeave) CNetConTest0402();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0402::CNetConTest0402()
	{
	}

void CNetConTest0402::ConstructL()
	{

	CNetConBearerTestBase::ConstructL(); // base class construct
	}

CNetConTest0402::~CNetConTest0402()
	{
	}

const TDesC& CNetConTest0402::Name() const
	{

	_LIT(KTestName, "Test Case NC-4-2");
	return KTestName();
	}

void CNetConTest0402::StartTestL()
	{
	
	if(!iTelBearer->StopChecking())
		{
		CompleteTest(MNetConTest::EPassed);
		}
	else
		{
		CompleteTest(MNetConTest::EFailed);
		}
	}



//
//  Test case NC-4-3
//

CNetConTest0403* CNetConTest0403::NewLC()
	{
	CNetConTest0403* self = new(ELeave) CNetConTest0403();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0403::CNetConTest0403()
	{
	}

void CNetConTest0403::ConstructL()
	{

	CNetConBearerTestBase::ConstructL(); // base class construct
	}

CNetConTest0403::~CNetConTest0403()
	{
	}

const TDesC& CNetConTest0403::Name() const
	{

	_LIT(KTestName, "Test Case NC-4-3");
	return KTestName();
	}

void CNetConTest0403::StartTestL()
	{

	const TInt err = iTelBearer->SecondPhaseAvailability();
	if (err == KErrNone)
		{
		CompleteTest(MNetConTest::EPassed);
		}
	else
		{
		CompleteTest(MNetConTest::EFailed);
		}
	}


//
//  Test case NC-4-4
//

CNetConTest0404* CNetConTest0404::NewLC()
	{
	CNetConTest0404* self = new(ELeave) CNetConTest0404();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0404::CNetConTest0404()
	{
	}

void CNetConTest0404::ConstructL()
	{

	CNetConBearerTestBase::ConstructL(); // base class construct
	}

CNetConTest0404::~CNetConTest0404()
	{
	}

const TDesC& CNetConTest0404::Name() const
	{

	_LIT(KTestName, "Test Case NC-4-4");
	return KTestName();
	}

void CNetConTest0404::StartTestL()
	{

	MNetConTest::TResult outcome = MNetConTest::EFailed;
	
	if(!iTelBearer->StartChecking())
		{
		outcome = MNetConTest::EFailed;
		}
	else
		{
		const TInt err = iTelBearer->SecondPhaseAvailability();
		if (err == KErrNone)
			{
			outcome = MNetConTest::EPassed;
			}
		else
			{
			outcome = MNetConTest::EFailed;
			}
		}

	CompleteTest(outcome);
	}


//
//  Test case NC-4-5
//

CNetConTest0405* CNetConTest0405::NewLC()
	{
	CNetConTest0405* self = new(ELeave) CNetConTest0405();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0405::CNetConTest0405()
	{
	}

void CNetConTest0405::ConstructL()
	{

	CNetConBearerTestBase::ConstructL(); // base class construct
	}

CNetConTest0405::~CNetConTest0405()
	{

	}

const TDesC& CNetConTest0405::Name() const
	{

	_LIT(KTestName, "Test Case NC-4-5");
	return KTestName();
	}

void CNetConTest0405::StartTestL()
	{

	MNetConTest::TResult outcome = MNetConTest::EFailed;
	
	if(!iTelBearer->StartChecking())
		{
		outcome = MNetConTest::EFailed;
		}
	else if (!iTelBearer->StopChecking())
		{
		outcome = MNetConTest::EFailed;
		}
	else
		{
		const TInt err = iTelBearer->SecondPhaseAvailability();
		if (err == KErrNone)
			{
			outcome = MNetConTest::EPassed;
			}
		else
			{
			outcome = MNetConTest::EFailed;
			}
		}

	CompleteTest(outcome);
	}


//
//  Test case NC-4-6
//

CNetConTest0406* CNetConTest0406::NewLC()
	{
	CNetConTest0406* self = new(ELeave) CNetConTest0406();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0406::CNetConTest0406()
	{
	}

void CNetConTest0406::ConstructL()
	{
	CNetConBearerTestBase::ConstructL(); // base class construct
	}

CNetConTest0406::~CNetConTest0406()
	{

	}

const TDesC& CNetConTest0406::Name() const
	{

	_LIT(KTestName, "Test Case NC-4-6");
	return KTestName();
	}

void CNetConTest0406::StartTestL()
	{

	iTelBearer->StartChecking(EFalse);
	}

void CNetConTest0406::GetTsyNameL(TDes& aName)
	{

	_LIT(KTsyName, "mm.tsy");
	aName = KTsyName();
	}

void CNetConTest0406::SetNetworkMode(RMobilePhone::TMobilePhoneNetworkMode)
	{ }

void CNetConTest0406::BearerStateChange(CBearerBase* aBearer)
	{
	
	ASSERT(aBearer == iTelBearer);
	(void)aBearer;

	CompleteTest(MNetConTest::EPassed);
	}



//
//  Test case NC-4-9
//

CNetConTest0409* CNetConTest0409::NewLC()
	{
	CNetConTest0409* self = new(ELeave) CNetConTest0409();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0409::CNetConTest0409()
	{
	}

void CNetConTest0409::ConstructL()
	{
	CNetConBearerTestBase::ConstructL(); // base class construct
	}

CNetConTest0409::~CNetConTest0409()
	{

	}

const TDesC& CNetConTest0409::Name() const
	{

	_LIT(KTestName, "Test Case NC-4-9");
	return KTestName();
	}

void CNetConTest0409::StartTestL()
	{

	iTelBearer->StartChecking(EFalse);
	}

void CNetConTest0409::GetTsyNameL(TDes& aName)
	{

	aName = Name();
	}

void CNetConTest0409::BearerStateChange(CBearerBase* aBearer)
	{

	ASSERT(aBearer == iTelBearer);

	if (aBearer->AvailableBearerSet() == KCommDbBearerUnknown)
		{
		CompleteTest(MNetConTest::EPassed);
		}
	else
		{
		CompleteTest(MNetConTest::EFailed);
		}
	}


//
//  Test case NC-4-11
//

CNetConTest0411* CNetConTest0411::NewLC()
	{
	CNetConTest0411* self = new(ELeave) CNetConTest0411();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0411::CNetConTest0411()
	{
	}

void CNetConTest0411::ConstructL()
	{
	CNetConBearerTestBase::ConstructL(); // base class construct
	}

CNetConTest0411::~CNetConTest0411()
	{

	}

const TDesC& CNetConTest0411::Name() const
	{

	_LIT(KTestName, "Test Case NC-4-11");
	return KTestName();
	}

void CNetConTest0411::StartTestL()
	{

	iTelBearer->StartChecking(EFalse);
	}

void CNetConTest0411::GetTsyNameL(TDes& aName)
	{

	aName = Name();
	}

void CNetConTest0411::BearerStateChange(CBearerBase* aBearer)
	{

	ASSERT(aBearer == iTelBearer);

	if (aBearer->AvailableBearerSet() == KCommDbBearerUnknown)
		{
		CompleteTest(MNetConTest::EPassed);
		}
	else
		{
		CompleteTest(MNetConTest::EFailed);
		}
	}


//
//  Test case NC-4-13
//

CNetConTest0413* CNetConTest0413::NewLC()
	{
	CNetConTest0413* self = new(ELeave) CNetConTest0413();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0413::CNetConTest0413()
	{
	}

void CNetConTest0413::ConstructL()
	{

	CNetConBearerTestBase::ConstructL(); // base class construct
	}

CNetConTest0413::~CNetConTest0413()
	{
	}

const TDesC& CNetConTest0413::Name() const
	{

	_LIT(KTestName, "Test Case NC-4-13");
	return KTestName();
	}

void CNetConTest0413::StartTestL()
	{

	iTelBearer->StartChecking(EFalse);
	}

void CNetConTest0413::GetTsyNameL(TDes& aName)
	{

	aName = Name();
	}

void CNetConTest0413::BearerStateChange(CBearerBase* aBearer)
	{

	ASSERT(aBearer == iTelBearer);

	if (aBearer->AvailableBearerSet() == KCommDbBearerUnknown)
		{
		CompleteTest(MNetConTest::EPassed);
		}
	else
		{
		CompleteTest(MNetConTest::EFailed);
		}
	}


//
//  Test case NC-4-15
//

CNetConTest0415* CNetConTest0415::NewLC()
	{
	CNetConTest0415* self = new(ELeave) CNetConTest0415();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0415::CNetConTest0415()
	{
	}

void CNetConTest0415::ConstructL()
	{

	CNetConBearerTestBase::ConstructL(); // base class construct
	}

CNetConTest0415::~CNetConTest0415()
	{
	}

const TDesC& CNetConTest0415::Name() const
	{

	_LIT(KTestName, "Test Case NC-4-15");
	return KTestName();
	}

void CNetConTest0415::StartTestL()
	{

	iTelBearer->StartChecking(EFalse);
	}

void CNetConTest0415::GetTsyNameL(TDes& aName)
	{

	aName = Name();
	}

void CNetConTest0415::SetNetworkMode(RMobilePhone::TMobilePhoneNetworkMode)
	{ }

void CNetConTest0415::BearerStateChange(CBearerBase* aBearer)
	{

	ASSERT(aBearer == iTelBearer);

	if (aBearer->AvailableBearerSet() == KCommDbBearerUnknown)
		{
		CompleteTest(MNetConTest::EPassed);
		}
	else
		{
		CompleteTest(MNetConTest::EFailed);
		}
	}


//
//  Test case NC-4-17
//

CNetConTest0417* CNetConTest0417::NewLC()
	{
	CNetConTest0417* self = new(ELeave) CNetConTest0417();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0417::CNetConTest0417()
	{
	}

void CNetConTest0417::ConstructL()
	{

	CNetConBearerTestBase::ConstructL(); // base class construct
	}

CNetConTest0417::~CNetConTest0417()
	{
	}

const TDesC& CNetConTest0417::Name() const
	{

	_LIT(KTestName, "Test Case NC-4-17");
	return KTestName();
	}

void CNetConTest0417::StartTestL()
	{

	iTelBearer->StartChecking(EFalse);
	}

void CNetConTest0417::GetTsyNameL(TDes& aName)
	{

	aName = Name();
	}

void CNetConTest0417::SetNetworkMode(RMobilePhone::TMobilePhoneNetworkMode)
	{ }

void CNetConTest0417::BearerStateChange(CBearerBase* aBearer)
	{

	ASSERT(aBearer == iTelBearer);

	if (aBearer->AvailableBearerSet() == KCommDbBearerUnknown)
		{
		CompleteTest(MNetConTest::EPassed);
		}
	else
		{
		CompleteTest(MNetConTest::EFailed);
		}
	}


//
//  Test case NC-4-18
//

CNetConTest0418* CNetConTest0418::NewLC()
	{
	CNetConTest0418* self = new(ELeave) CNetConTest0418();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0418::CNetConTest0418()
	{
	}

void CNetConTest0418::ConstructL()
	{

	CNetConBearerTestBase::ConstructL(); // base class construct
	}

CNetConTest0418::~CNetConTest0418()
	{
	}

const TDesC& CNetConTest0418::Name() const
	{

	_LIT(KTestName, "Test Case NC-4-18");
	return KTestName();
	}

void CNetConTest0418::StartTestL()
	{

	iTelBearer->StartChecking(EFalse);
	}

void CNetConTest0418::GetTsyNameL(TDes& aName)
	{

	_LIT(KTsyName, "mm.tsy");
	aName = KTsyName();
	}

void CNetConTest0418::SetNetworkMode(RMobilePhone::TMobilePhoneNetworkMode)
	{ }

void CNetConTest0418::GetMinSigStrengthL(TUint32& aMinAcceptedSignalStrength)
	{

	aMinAcceptedSignalStrength = 10500;
	}

void CNetConTest0418::BearerStateChange(CBearerBase* aBearer)
	{

	ASSERT(aBearer == iTelBearer);
	(void)aBearer;

	const TInt ret = iTelBearer->SecondPhaseAvailability();

	if(ret == KErrNone)
		{
		CompleteTest(MNetConTest::EPassed);
		}
	else
		{
		CompleteTest(MNetConTest::EFailed);
		}
	}


//
//  Test case NC-4-19
//

CNetConTest0419* CNetConTest0419::NewLC()
	{
	CNetConTest0419* self = new(ELeave) CNetConTest0419();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0419::CNetConTest0419()
	{
	}

void CNetConTest0419::ConstructL()
	{

	CNetConBearerTestBase::ConstructL(); // base class construct
	}

CNetConTest0419::~CNetConTest0419()
	{
	}

const TDesC& CNetConTest0419::Name() const
	{

	_LIT(KTestName, "Test Case NC-4-19");
	return KTestName();
	}

void CNetConTest0419::StartTestL()
	{

	iTelBearer->StartChecking(EFalse);
	}

void CNetConTest0419::GetTsyNameL(TDes& aName)
	{

	_LIT(KTsyName, "mm.tsy");
	aName = KTsyName();
	}

void CNetConTest0419::SetNetworkMode(RMobilePhone::TMobilePhoneNetworkMode)
	{ }

void CNetConTest0419::GetMinSigStrengthL(TUint32& aMinAcceptedSignalStrength)
	{

	aMinAcceptedSignalStrength = 12000;
	}

void CNetConTest0419::BearerStateChange(CBearerBase* aBearer)
	{

	ASSERT(aBearer == iTelBearer);
	(void)aBearer;

	const TInt ret = iTelBearer->SecondPhaseAvailability();

	if(ret == KErrNetConInadequateSignalStrengh)
		{
		CompleteTest(MNetConTest::EPassed);
		}
	else
		{
		CompleteTest(MNetConTest::EFailed);
		}
	}


//
//  Test case NC-4-20
//

CNetConTest0420* CNetConTest0420::NewLC()
	{
	CNetConTest0420* self = new(ELeave) CNetConTest0420();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0420::CNetConTest0420()
	{
	}

void CNetConTest0420::ConstructL()
	{

	CNetConBearerTestBase::ConstructL(); // base class construct
	}

CNetConTest0420::~CNetConTest0420()
	{
	}

const TDesC& CNetConTest0420::Name() const
	{

	_LIT(KTestName, "Test Case NC-4-20");
	return KTestName();
	}

void CNetConTest0420::GetTsyNameL(TDes& aName)
	{

	aName = Name();
	}

void CNetConTest0420::StartTestL()
	{

	iTelBearer->StartChecking(EFalse);
	}

void CNetConTest0420::SetNetworkMode(RMobilePhone::TMobilePhoneNetworkMode)
	{
	}

void CNetConTest0420::BearerStateChange(CBearerBase* aBearer)
	{

	ASSERT(aBearer == iTelBearer);
	(void)aBearer;

	const TUint32 available = iTelBearer->AvailableBearerSet();
	}
	

