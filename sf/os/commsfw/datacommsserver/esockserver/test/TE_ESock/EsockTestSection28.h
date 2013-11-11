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
// This is the header file for ESOCK test section 28
// 
//


#if (!defined __ESOCKTEST_28_H__)
#define __ESOCKTEST_28_H__
#include "TestStepEsock.h"

const TInt KErrDndNameNotFound = -5120;
const TInt KMaxAddressTextSize = 64;		// at least 39 + 11 (size of IPv6 address + size of '%' and scope)

// entries in ini file section for Test_28.<n>

_LIT(KSection28, "Test_28");
_LIT(KKeyEthernet, "ethernet");
_LIT(KKeyIap, "iap");
_LIT(KKeyPrimary, "primary");
_LIT(KKeySecondary, "secondary");


class CEsockTest28 : public CESockTestStep_OOMCapable
	{
	protected:
		~CEsockTest28();
		void NameInit();
		TInt NameAppendL(const TDesC& aName);
		TBool NameFound(const TDesC& aName);
		void NamePrint();
		TVerdict Ipv6StaticDnsTestL(TInt aSubStep);
	private:
		RPointerArray<HBufC> iNames;
	};


class CEsockTest28_1 : public CEsockTest28
	{
	public:
		static const TDesC& GetTestName();
		virtual enum TVerdict doTestStepPreambleL( void );
		virtual enum TVerdict easyTestStepL();
	};

class CEsockTest28_2 : public CEsockTest28
	{
	public:
		static const TDesC& GetTestName();
		virtual enum TVerdict easyTestStepL();
	};

#endif

