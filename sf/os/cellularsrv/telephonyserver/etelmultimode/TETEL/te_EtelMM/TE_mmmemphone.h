// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent 
*/

#ifndef _TE_MMMEMPHONE_H_
#define _TE_MMMEMPHONE_H_

#include <etelmm.h>
#include <e32base.h>
#include <mmretrieve.h>
#include "TE_mmcanceller.h"
#include "TE_mmphone.h"



class CTestMemPhone : public CTestPhone //public CEtelMMTestStepBase
	{
public:
	TInt iTestCount;
	CTestMemPhone() ;
	~CTestMemPhone(){} ;
	virtual enum TVerdict doTestStepL();

private:

	class CMemTestGetDetectedNetworks : public CTestPhone::CTestGetDetectedNetworks
		{
	public:
		static CMemTestGetDetectedNetworks* NewLC(RMobilePhone& aPhone, CTestMemPhone* aTestMemPhone);
		void Start();
		CTestMemPhone* iTestMemPhone;
	protected:
		CMemTestGetDetectedNetworks(RMobilePhone& aPhone, CTestMemPhone* aTestMemPhone);
		TInt RunError(TInt aError);
		void StartMemTest();
		virtual void RunL();
	protected:
		TInt iFailEvery;
		};
	
	class CMemTestGetCFStatus : public CTestPhone::CTestGetCFStatus
		{
	public:
		static CMemTestGetCFStatus* NewLC(RMobilePhone& aPhone, CTestMemPhone* aTestMemPhone);
		void Start();
		CTestMemPhone* iTestMemPhone;
	protected:
		CMemTestGetCFStatus(RMobilePhone& aPhone, CTestMemPhone* aTestMemPhone);
		TInt RunError(TInt aError);
		void StartMemTest();
		virtual void RunL();
	protected:
		TInt iFailEvery;
		};

	class CMemTestGetCB : public CTestPhone::CTestGetCB
		{
	public:
		static CMemTestGetCB* NewLC(RMobilePhone& aPhone, CTestMemPhone* aTestMemPhone);
		void Start();
		CTestMemPhone* iTestMemPhone;
	protected:
		CMemTestGetCB(RMobilePhone& aPhone, CTestMemPhone* aTestMemPhone);
		TInt RunError(TInt aError);
		void StartMemTest();
		virtual void RunL();
	protected:
		TInt iFailEvery;
		};

	class CMemTestGetCW : public CTestPhone::CTestGetCW
		{
	public:
		static CMemTestGetCW* NewLC(RMobilePhone& aPhone, CTestMemPhone* aTestMemPhone);
		void Start();
		CTestMemPhone* iTestMemPhone;
	protected:
		CMemTestGetCW(RMobilePhone& aPhone, CTestMemPhone* aTestMemPhone);
		TInt RunError(TInt aError);
		void StartMemTest();
		virtual void RunL();
	protected:
		TInt iFailEvery;
		};

	class CMemTestGetCcbs : public CTestPhone::CTestGetCcbs
		{
	public:
		static CMemTestGetCcbs* NewLC(RMobilePhone& aPhone, CTestMemPhone* aTestMemPhone);
		void Start();
		CTestMemPhone* iTestMemPhone;
	protected:
		CMemTestGetCcbs(RMobilePhone& aPhone, CTestMemPhone* aTestMemPhone);
		TInt RunError(TInt aError);
		void StartMemTest();
		virtual void RunL();
	protected:
		TInt iFailEvery;
		};
		
	class CMemTestGetPreferredNetworks : public CTestPhone::CTestGetPreferredNetworks
		{
	public:
		static CMemTestGetPreferredNetworks* NewLC(RMobilePhone& aPhone, CTestMemPhone* aTestMemPhone);
		void Start();
		CTestMemPhone* iTestMemPhone;
	protected:
		CMemTestGetPreferredNetworks(RMobilePhone& aPhone, CTestMemPhone* aTestMemPhone);
		TInt RunError(TInt aError);
		void StartMemTest();
		virtual void RunL();
	protected:
		TInt iFailEvery;
		};

friend class CMemTestGetDetectedNetworks;
friend class CMemTestGetCFStatus;
friend class CMemTestGetCB;
friend class CMemTestGetCW;
friend class CMemTestGetCcbs;
friend class CMemTestGetPreferredNetworks;

	};

#endif
