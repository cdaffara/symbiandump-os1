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

#ifndef __BEARERTESTS_H__
#define __BEARERTESTS_H__

#include "TestBases.h"

//
//  Test case NC-4-1
//
class CTelBearer;
class CNetConTest0401 : public CNetConBearerTestBase
	{
public:
	~CNetConTest0401();
	static CNetConTest0401* NewLC();

	// implementation of MNetConTest interface
	const TDesC& Name() const;
	virtual void StartTestL();

private:
	CNetConTest0401();
	void ConstructL();

	};


//
//  Test case NC-4-2
//
class CNetConTest0402 : public CNetConBearerTestBase
	{
public:
	~CNetConTest0402();
	static CNetConTest0402* NewLC();

	// implementation of MNetConTest interface
	const TDesC& Name() const;
	virtual void StartTestL();

private:
	CNetConTest0402();
	void ConstructL();

	};


//
//  Test case NC-4-3
//
class CNetConTest0403 : public CNetConBearerTestBase
	{
public:
	~CNetConTest0403();
	static CNetConTest0403* NewLC();

	// implementation of MNetConTest interface
	const TDesC& Name() const;
	virtual void StartTestL();

private:
	CNetConTest0403();
	void ConstructL();

	};


//
//  Test case NC-4-4
//
class CNetConTest0404 : public CNetConBearerTestBase
	{
public:
	~CNetConTest0404();
	static CNetConTest0404* NewLC();

	// implementation of MNetConTest interface
	const TDesC& Name() const;
	virtual void StartTestL();

private:
	CNetConTest0404();
	void ConstructL();

	};


//
//  Test case NC-4-5
//
class CNetConTest0405 : public CNetConBearerTestBase
	{
public:
	~CNetConTest0405();
	static CNetConTest0405* NewLC();

	// implementation of MNetConTest interface
	const TDesC& Name() const;
	virtual void StartTestL();

private:
	CNetConTest0405();
	void ConstructL();

	};


//
//  Test cases NC-4-6, NC-4-8, NC-4-10,
//  NC-4-12, NC-4-14, NC-4-16
//
class CNetConTest0406 : public CNetConBearerTestBase
	{
public:
	~CNetConTest0406();
	static CNetConTest0406* NewLC();

	// implementation of MNetConTest interface
	const TDesC& Name() const;
	virtual void StartTestL();

	// implementation of MBearerObserver interface
	virtual void BearerStateChange(CBearerBase* aBearer);

	// override function from CNetConTestBase
	virtual void GetTsyNameL(TDes& aName);
	virtual void SetNetworkMode(RMobilePhone::TMobilePhoneNetworkMode aNetworkMode);

private:
	CNetConTest0406();
	void ConstructL();

	};




//
//  Test case NC-4-9
//
class CNetConTest0409 : public CNetConBearerTestBase
	{
public:
	~CNetConTest0409();
	static CNetConTest0409* NewLC();

	// implementation of MNetConTest interface
	const TDesC& Name() const;
	virtual void StartTestL();

	// implementation of MBearerObserver interface
	virtual void BearerStateChange(CBearerBase* aBearer);

	// override function from CNetConTestBase
	virtual void GetTsyNameL(TDes& aName);

private:
	CNetConTest0409();
	void ConstructL();

	};


//
//  Test case NC-4-11
//
class CNetConTest0411 : public CNetConBearerTestBase
	{
public:
	~CNetConTest0411();
	static CNetConTest0411* NewLC();

	// implementation of MNetConTest interface
	const TDesC& Name() const;
	virtual void StartTestL();

	// implementation of MBearerObserver interface
	virtual void BearerStateChange(CBearerBase* aBearer);

	// override function from CNetConTestBase
	virtual void GetTsyNameL(TDes& aName);

private:
	CNetConTest0411();
	void ConstructL();

	};


//
//  Test case NC-4-13
//
class CNetConTest0413 : public CNetConBearerTestBase
	{
public:
	~CNetConTest0413();
	static CNetConTest0413* NewLC();

	// implementation of MNetConTest interface
	const TDesC& Name() const;
	virtual void StartTestL();

	// implementation of MBearerObserver interface
	virtual void BearerStateChange(CBearerBase* aBearer);

	// override function from CNetConTestBase
	virtual void GetTsyNameL(TDes& aName);

private:
	CNetConTest0413();
	void ConstructL();

	};


//
//  Test case NC-4-15
//
class CNetConTest0415 : public CNetConBearerTestBase
	{
public:
	~CNetConTest0415();
	static CNetConTest0415* NewLC();

	// implementation of MNetConTest interface
	const TDesC& Name() const;
	virtual void StartTestL();

	// implementation of MBearerObserver interface
	virtual void BearerStateChange(CBearerBase* aBearer);

	// override function from CNetConTestBase
	virtual void GetTsyNameL(TDes& aName);
	virtual void SetNetworkMode(RMobilePhone::TMobilePhoneNetworkMode aNetworkMode);

private:
	CNetConTest0415();
	void ConstructL();

	};


//
//  Test case NC-4-17
//
class CNetConTest0417 : public CNetConBearerTestBase
	{
public:
	~CNetConTest0417();
	static CNetConTest0417* NewLC();

	// implementation of MNetConTest interface
	const TDesC& Name() const;
	virtual void StartTestL();

	// implementation of MBearerObserver interface
	virtual void BearerStateChange(CBearerBase* aBearer);

	// override function from CNetConTestBase
	virtual void GetTsyNameL(TDes& aName);
	virtual void SetNetworkMode(RMobilePhone::TMobilePhoneNetworkMode aNetworkMode);

private:
	CNetConTest0417();
	void ConstructL();

	};


//
//  Test case NC-4-18
//
class CNetConTest0418 : public CNetConBearerTestBase
	{
public:
	~CNetConTest0418();
	static CNetConTest0418* NewLC();

	// implementation of MNetConTest interface
	const TDesC& Name() const;
	virtual void StartTestL();

	// implementation of MBearerObserver interface
	virtual void BearerStateChange(CBearerBase* aBearer);

	// override function from CNetConTestBase
	virtual void GetTsyNameL(TDes& aName);
	virtual void SetNetworkMode(RMobilePhone::TMobilePhoneNetworkMode aNetworkMode);

	virtual void GetMinSigStrengthL(TUint32& aMinAcceptedSignalStrength);

private:
	CNetConTest0418();
	void ConstructL();

	};


//
//  Test case NC-4-19
//
class CNetConTest0419 : public CNetConBearerTestBase
	{
public:
	~CNetConTest0419();
	static CNetConTest0419* NewLC();

	// implementation of MNetConTest interface
	const TDesC& Name() const;
	virtual void StartTestL();

	// implementation of MBearerObserver interface
	virtual void BearerStateChange(CBearerBase* aBearer);

	// override function from CNetConTestBase
	virtual void GetTsyNameL(TDes& aName);
	virtual void SetNetworkMode(RMobilePhone::TMobilePhoneNetworkMode aNetworkMode);
	virtual void GetMinSigStrengthL(TUint32& aMinAcceptedSignalStrength);

private:
	CNetConTest0419();
	void ConstructL();

	};


//
//  Test case NC-4-20
//
class CNetConTest0420 : public CNetConBearerTestBase
	{
public:
	~CNetConTest0420();
	static CNetConTest0420* NewLC();

	// implementation of MNetConTest interface
	const TDesC& Name() const;
	virtual void StartTestL();

	// implementation of MBearerObserver interface
	virtual void BearerStateChange(CBearerBase* aBearer);

	// override function from CNetConTestBase
	virtual void GetTsyNameL(TDes& aName);
	virtual void SetNetworkMode(RMobilePhone::TMobilePhoneNetworkMode aNetworkMode);

private:
	CNetConTest0420();
	void ConstructL();

	};

#endif // __BEARERTESTS_H__

