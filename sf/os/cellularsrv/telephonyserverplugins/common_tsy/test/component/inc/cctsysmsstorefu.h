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
// The TEFUnit header file which tests the SmsStore
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYSMSSTOREFU_H
#define CCTSYSMSSTOREFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsySmsStoreFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestNotifyStoreEvent0001L();
	void TestNotifyStoreEvent0002L();
	void TestNotifyStoreEvent0004L();
	void TestRead0001L();
	void TestRead0002L();
	void TestRead0003L();
	void TestRead0004L();
	void TestRead0005L();
	void TestDelete0001L();
	void TestDelete0002L();
	void TestDelete0004L();
	void TestDelete0005L();
	void TestGetInfo0001L();
	void TestGetInfo0002L();
	void TestGetInfo0003L();
	void TestGetInfo0004L();
	void TestGetInfo0005L();
	void TestDeleteAll0001L();
	void TestDeleteAll0002L();
	void TestDeleteAll0004L();
	void TestDeleteAll0005L();
	void TestWrite0001L();
	void TestWrite0002L();
	void TestWrite0003L();
	void TestWrite0004L();
	void TestWrite0005L();
	void TestStart0001L();
	void TestStart0001bL();
	void TestStart0002L();
	void TestStart0004L();
	void TestStart0005L();
	void TestStartBatch0001L();

private:

	void AuxRead1L(const TDesC& aName);
	void AuxRead2L(const TDesC& aName);
	void AuxRead3L(const TDesC& aName);
	void AuxRead4L(const TDesC& aName);
	void AuxRead5L(const TDesC& aName);
	
	void AuxWrite1L(const TDesC& aName);
	void AuxWrite2L(const TDesC& aName);
	void AuxWrite3L(const TDesC& aName);
	void AuxWrite4L(const TDesC& aName);
	void AuxWrite5L(const TDesC& aName);

	void AuxDelete1L(const TDesC& aName);
	void AuxDelete2L(const TDesC& aName);
	void AuxDelete4L(const TDesC& aName);
	void AuxDelete5L(const TDesC& aName);
	
	void AuxDeleteAll1L(const TDesC& aName);
	void AuxDeleteAll2L(const TDesC& aName);
	void AuxDeleteAll4L(const TDesC& aName);
	void AuxDeleteAll5L(const TDesC& aName);
	
	void AuxGetInfo1L(const TDesC& aName);	
	void AuxGetInfo2L(const TDesC& aName);	
	void AuxGetInfo3L(const TDesC& aName);	
	void AuxGetInfo4L(const TDesC& aName);	
	void AuxGetInfo5L(const TDesC& aName);	

	void AuxStart1L(const TDesC& aName);	
	void AuxStart1bL(const TDesC& aName);	
	void AuxStart2L(const TDesC& aName);	
	void AuxStart4L(const TDesC& aName);	
	void AuxStart5L(const TDesC& aName);
	
	void AuxStartBatch1L(const TDesC& aName);
	
	void AuxNotifyStoreEvent1L(const TDesC& aName);	
	void AuxNotifyStoreEvent2L(const TDesC& aName);	
	void AuxNotifyStoreEvent4L(const TDesC& aName);	
	}; // class CCTsySmsStoreFU

#endif // CCTSYSMSSTOREFU_H

