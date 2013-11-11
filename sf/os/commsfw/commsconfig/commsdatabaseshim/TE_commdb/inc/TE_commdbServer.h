/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if (!defined __TE_commdbSUITE_H__)
#define __TE_commdbSUITE_H__

#include <test/TestExecuteServerBase.h>

class CTE_commdbServer : public CTestServer
	{
public:
	static CTE_commdbServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	
	
	// this should be a pure virtual so every test ddl
	// has to provide a version but for now defaults to ?.?
	virtual TPtrC GetVersion( void );
		
	};

	
// these values have been copied from Testenv.h
const TInt KSmallBufSize = 256;
const TInt KLargeBufSize = 2048;
const TInt KHugeBufSize = 16384;

// CommDb test macros copied from Testenv.h
typedef TBuf8<KLargeBufSize> TLargeBuf;
#define IPADDR(a,b,c,d) (TUint32)(((a)<<24)|((b)<<16)|((c)<<8)|(d))

//Test suite parameters
const TInt KHeapFailure	= 500;

#endif
