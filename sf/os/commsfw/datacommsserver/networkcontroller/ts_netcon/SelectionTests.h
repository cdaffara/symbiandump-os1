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

#ifndef __SELECTIONTESTS_H__
#define __SELECTIONTESTS_H__

#include "TestBases.h"

//
//  Test case NC-2-1
//
class CNetConTest0201 : public CNetConSelectionRequestTestBase
{
public:
	virtual ~CNetConTest0201();
	static CNetConTest0201* NewLC();

	const TDesC& Name() const;
	
	// override of functions from CNetConSelectionRequestTestBase
	virtual TInt FindExistingAgentForSelection(CNifAgentBase*& aAgent, CCommsDbAccess*) const;
	virtual void RequestComplete(const CSelectionRequest* aRequest, TInt aError);

private:
	CNetConTest0201();
	void ConstructL();
};


//
//  Test case NC-2-2
//
class CNetConTest0202 : public CNetConSelectionRequestTestBase
{
public:
	static CNetConTest0202* NewLC();

	const TDesC& Name() const;

private:
	CNetConTest0202();
};


//
//  Test case NC-2-3
//
class CNetConTest0203 : public CNetConSelectionRequestTestBase
{
public:
	virtual ~CNetConTest0203();
	static CNetConTest0203* NewLC();

	const TDesC& Name() const;

	// override of function from CNetConTestBase
	virtual const HBufC* ImplicitConnectionAgentName() const;
	const TConnPref& ImplicitConnectionPrefs() const;

	// override of functions from CNetConSelectionRequestTestBase
	virtual TInt FindExistingAgentForSelection(CNifAgentBase*& aAgent, CCommsDbAccess*) const;
	virtual void RequestComplete(const CSelectionRequest* aRequest, TInt aError);

private:
	CNetConTest0203();
	void ConstructL();

private:
	HBufC* iImplicitAgentName;
};


//
//  Test case NC-2-4
//
class CNetConTest0204 : public CNetConSelectionRequestTestBase
{
public:
	virtual ~CNetConTest0204();
	static CNetConTest0204* NewLC();

	const TDesC& Name() const;
	
	// override of function from CNetConSelectionRequestTestBase
	virtual void RequestComplete(const CSelectionRequest* aRequest, TInt aError);

private:
	CNetConTest0204();
};


//
//  Test case NC-2-5
//
class CNetConTest0205 : public CNetConSelectionRequestTestBase
{
public:
	virtual ~CNetConTest0205();
	static CNetConTest0205* NewLC();

	const TDesC& Name() const;
	
	// override of functions from CNetConSelectionRequestTestBase
	virtual void RequestComplete(const CSelectionRequest* aRequest, TInt aError);
	virtual void GetCurrentSettingsL(TConnectionSettings& aSettings, TCommDbConnectionDirection aDirection, TUint32 aRank);

private:
	CNetConTest0205();
	void ConstructL();
	};


//
//  Test case NC-2-6
//
class CNetConTest0206 : public CNetConSelectionRequestTestBase
{
public:
	virtual ~CNetConTest0206();
	static CNetConTest0206* NewLC();

	const TDesC& Name() const;

	// override of function from CNetConSelectionRequestTestBase
	virtual void RequestComplete(const CSelectionRequest* aRequest, TInt aError);

private:
	CNetConTest0206();
	};

//
//  Test case NC-2-8
//
class CNetConTest0208 : public CNetConSelectionRequestTestBase
{
public:
	virtual ~CNetConTest0208();
	static CNetConTest0208* NewLC();

	const TDesC& Name() const;

	// override of function from CNetConSelectionRequestTestBase
	virtual void RequestComplete(const CSelectionRequest* aRequest, TInt aError);

private:
	CNetConTest0208();
	void DialogCbComplete();
	};


//
//  Test case NC-2-9
//
class CNetConTest0209 : public CNetConSelectionRequestTestBase
{
public:
	virtual ~CNetConTest0209();
	static CNetConTest0209* NewLC();

	const TDesC& Name() const;

	// override of function from CNetConSelectionRequestTestBase
	virtual void RequestComplete(const CSelectionRequest* aRequest, TInt aError);

private:
	CNetConTest0209();
	void DialogCbComplete();
	};


//
//  Test case NC-2-10
//
class CNetConTest0210 : public CNetConSelectionRequestTestBase
{
public:
	virtual ~CNetConTest0210();
	static CNetConTest0210* NewLC();

	const TDesC& Name() const;

private:
	CNetConTest0210();
	void DialogCbComplete();
	};


//
//  Test case NC-2-11
//
class CNetConTest0211 : public CNetConSelectionRequestTestBase
{
public:
	virtual ~CNetConTest0211();
	static CNetConTest0211* NewLC();

	const TDesC& Name() const;

	// override of function from CNetConTestBase
	virtual RMobilePhone::TMobilePhoneNetworkMode NetworkMode() const;

	// override of functions from CNetConSelectionRequestTestBase
	virtual void GetCurrentSettingsL(TConnectionSettings& aSettings, TCommDbConnectionDirection aDirection, TUint32 aRank);

private:
	CNetConTest0211();
	void DialogCbComplete();

	};


//
//  Test case NC-2-12
//
class CNetConTest0212 : public CNetConSelectionRequestTestBase
{
public:
	virtual ~CNetConTest0212();
	static CNetConTest0212* NewLC();

	const TDesC& Name() const;

	// override of function from CNetConTestBase
	virtual RMobilePhone::TMobilePhoneNetworkMode NetworkMode() const;

	// override of functions from CNetConSelectionRequestTestBase
	virtual void GetCurrentSettingsL(TConnectionSettings& aSettings, TCommDbConnectionDirection aDirection, TUint32 aRank);

private:
	CNetConTest0212();
	void DialogCbComplete();
	};


//
//  Test case NC-2-13
//
class CNetConTest0213 : public CNetConSelectionRequestTestBase
{
public:
	virtual ~CNetConTest0213();
	static CNetConTest0213* NewLC();

	const TDesC& Name() const;

	// override of function from CNetConTestBase
	virtual RMobilePhone::TMobilePhoneNetworkMode NetworkMode() const;

	// override of functions from CNetConSelectionRequestTestBase
	virtual void GetCurrentSettingsL(TConnectionSettings& aSettings, TCommDbConnectionDirection aDirection, TUint32 aRank);

private:
	CNetConTest0213();
	void DialogCbComplete();
	};


//
//  Test case NC-2-14
//
class CNetConTest0214 : public CNetConSelectionRequestTestBase
{
public:
	virtual ~CNetConTest0214();
	static CNetConTest0214* NewLC();

	const TDesC& Name() const;

private:
	CNetConTest0214();
	};


//
//  Test case NC-2-15
//
class CNetConTest0215 : public CNetConSelectionRequestTestBase
{
public:
	virtual ~CNetConTest0215();
	static CNetConTest0215* NewLC();

	const TDesC& Name() const;
	
	// override of functions from CNetConSelectionRequestTestBase
	virtual TInt FindExistingAgentForSelection(CNifAgentBase*& aAgent, CCommsDbAccess*) const;
	virtual void RequestComplete(const CSelectionRequest* aRequest, TInt aError);

private:
	CNetConTest0215();
	void ConstructL();
};


//
//  Test case NC-2-16
//
class CNetConTest0216 : public CNetConSelectionRequestTestBase
{
public:
	virtual ~CNetConTest0216();
	static CNetConTest0216* NewLC();

	const TDesC& Name() const;
	
	// override of function from CNetConSelectionRequestTestBase
	virtual void RequestComplete(const CSelectionRequest* aRequest, TInt aError);

private:
	CNetConTest0216();
};


//
//  Test case NC-2-17
//
class CNetConTest0217 : public CNetConSelectionRequestTestBase
{
public:
	virtual ~CNetConTest0217();
	static CNetConTest0217* NewLC();

	const TDesC& Name() const;
	
	// override of function from CNetConSelectionRequestTestBase
	virtual void RequestComplete(const CSelectionRequest* aRequest, TInt aError);

private:
	CNetConTest0217();
};


//
//  Test case NC-2-18
//
class CNetConTest0218 : public CNetConSelectionRequestTestBase
{
public:
	virtual ~CNetConTest0218();
	static CNetConTest0218* NewLC();

	const TDesC& Name() const;

	// override of function from CNetConSelectionRequestTestBase
	virtual void GetDesL(const TDesC& aTable, const TDesC& aField, TDes16& aValue);

private:
	CNetConTest0218();
};


//
//  Test case NC-2-19
//
class CNetConTest0219 : public CNetConSelectionRequestTestBase
{
public:
	virtual ~CNetConTest0219();
	static CNetConTest0219* NewLC();

	const TDesC& Name() const;

	// override of function from CNetConSelectionRequestTestBase
	virtual void RequestComplete(const CSelectionRequest* aRequest, TInt aError);
	
	// override of function from CControllerTestBase
	void SetOverridesL(CCommDbOverrideSettings* aOverrides);

private:
	CNetConTest0219();
	void ConstructL();
	void DialogCbComplete();
};


//
//  Test case NC-2-20
//
class CNetConTest0220 : public CNetConSelectionRequestTestBase
{
public:
	virtual ~CNetConTest0220();
	static CNetConTest0220* NewLC();

	const TDesC& Name() const;

	// override of function from CNetConSelectionRequestTestBase
	virtual void RequestComplete(const CSelectionRequest* aRequest, TInt aError);
	
	// override of function from CControllerTestBase
	void SetOverridesL(CCommDbOverrideSettings* aOverrides);

	// override of functions from CNetConRequestTestBase
	virtual void SelectConnection(MDialogProcessorObserver& aObserver, const TConnectionPrefs& aPrefs);
	virtual void SelectConnection(MDialogProcessorObserver& aObserver, const TConnectionPrefs& aPrefs, TInt aLastError);
	virtual void SelectModemAndLocation(MDialogProcessorObserver& aObserver);

private:
	CNetConTest0220();
	void ConstructL();
	void DialogCbComplete();
};

//
//  Test case NC-2-21 (was NC-1-14)
//
class CNetConTest0221 : public CNetConSelectionRequestTestBase
	{
public:
	virtual ~CNetConTest0221();
	static CNetConTest0221* NewLC();

	const TDesC& Name() const;

	// override of function from CNetConSelectionRequestTestBase
	virtual void RequestComplete(const CSelectionRequest* aRequest, TInt aError);

	// override of function from CControllerTestBase
	void SetOverridesL(CCommDbOverrideSettings* aOverrides);

private:
	CNetConTest0221();
	void ConstructL();

private:
	TCommDbMultiConnPref iMultiPref;
	};

//
//  Test case NC-2-22 (was NC-1-15)
//
class CNetConTest0222 : public CNetConSelectionRequestTestBase
	{
public:
	virtual ~CNetConTest0222();
	static CNetConTest0222* NewLC();

	const TDesC& Name() const;

	// override of function from CNetConSelectionRequestTestBase
	virtual void RequestComplete(const CSelectionRequest* aRequest, TInt aError);

	// override of function from CControllerTestBase
	void SetOverridesL(CCommDbOverrideSettings* aOverrides);

private:
	CNetConTest0222();
	void ConstructL();

private:
	TCommDbMultiConnPref iMultiPref;
	};

//
//  Test case NC-2-23 (was NC-1-16)
//
class CNetConTest0223 : public CNetConSelectionRequestTestBase
	{
public:
	virtual ~CNetConTest0223();
	static CNetConTest0223* NewLC();

	const TDesC& Name() const;

	// override of function from CNetConSelectionRequestTestBase
	virtual void RequestComplete(const CSelectionRequest* aRequest, TInt aError);

private:
	CNetConTest0223();
	void ConstructL();

private:
	TCommDbMultiConnPref iMultiPref;
	};

//
//  Test case NC-2-24
//
class CNetConTest0224 : public CNetConSelectionRequestTestBase
{
public:
	virtual ~CNetConTest0224();
	static CNetConTest0224* NewLC();

	const TDesC& Name() const;

private:
	CNetConTest0224();
	void DialogCbComplete();
	};

#endif // __SELECTIONTESTS_H__

