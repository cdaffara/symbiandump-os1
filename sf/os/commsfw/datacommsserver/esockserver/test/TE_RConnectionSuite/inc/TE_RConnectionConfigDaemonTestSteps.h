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
// This is the header file with the declarations of the RConnection test step classes that 
// require real ethernet as the default interface specified in CommDB or simply specify the IAP directly
// This simply tests starting connections that require configuration by a configuration daemon such as DHCP
// and querying them through the Ioctl() function. In these tests it is ethernet with dhcp.
// Each class encapsulates a complete test case and is derived from TE_RConnectionStep
// 
//

/**
 @file
*/

#if (!defined __TE_RConnectionCONFIGDAEMONTESTSTEPS_H__)
#define __TE_RConnectionCONFIGDAEMONTESTSTEPS_H__

#include "TE_RConnectionStep.h"

_LIT(KTCPIPPath, "c:\\private\\101f7989\\Esock\\ip.tcpip.esk");
_LIT(KTCPIPBackPath, "c:\\private\\101f7989\\Esock\\ip.tcpip.esk.bak");
_LIT(KQosTCPIPPath, "z:\\private\\101F7989\\ESock\\ip.qostest_tcpip.esk");

//
// Class for Copying files
//
NONSHARABLE_CLASS (TE_RConnectionPreCopy) : public TE_RConnectionStep
{
public:
	~TE_RConnectionPreCopy ();
	enum TVerdict doTestStepL( void );
	void copyFileL (const TDesC& anOld,const TDesC& aNew);
};

//
// Class for deleting files
//
NONSHARABLE_CLASS (TE_RConnectionPostDelete) : public TE_RConnectionStep
{
public:
	~TE_RConnectionPostDelete ();
	enum TVerdict doTestStepL( void );
	void copyFileL (const TDesC& anOld,const TDesC& aNew);
	void deleteFileL (const TDesC& aFileName);
};

class TE_RConnectionTest600 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest600();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest601 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest601();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest602 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest602();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest603 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest603();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest604 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest604();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest605 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest605();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest606 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest606();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest607 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest607();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest608 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest608();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest609 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest609();

	virtual enum TVerdict doTestStepL(void);
};

#endif

