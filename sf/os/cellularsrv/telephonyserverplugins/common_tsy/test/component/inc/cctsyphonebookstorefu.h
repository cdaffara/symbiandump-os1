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
// The TEFUnit header file which tests the PhoneBookStore
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYPHONEBOOKSTOREFU_H
#define CCTSYPHONEBOOKSTOREFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>
#include <mpbutil.h>
#include <ctsy/serviceapi/cmmgsmphonestorageutility.h>
#include "cctsycomponenttestbase.h"

class CCTsyPhoneBookStoreFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestRead0001L();
    void TestRead0001bL();
    void TestRead0001cL();
	void TestRead0002L();
	void TestRead0003L();
	void TestRead0004L();
	void TestRead0004bL();
	void TestRead0005L();
	void TestRead0006L();
	void TestRead0007L();
	void TestRead0008L();
	void TestRead0009L();
	void TestRead00010L();
	void TestRead00011L();
	void TestRead00012L();
	void TestRead00013L();
	void TestRead00014L();
	void TestRead00015L();
	void TestRead00016L();
	void TestRead00017L();
	void TestRead00018L();
	void TestRead00019L();
	void TestRead00020L();
	void TestRead00021L();
	void TestRead00022L();
	void TestRead00023L();
	void TestRead00024L();
	void TestRead00025L();
	void TestRead00026L();
	void TestRead00027L();
	void TestRead00028L();
	void TestRead00029L();
	void TestRead00030L();
	void TestWrite0001L();
	void TestWrite0002L();
	void TestWrite0003L();
	void TestWrite0004L();
	void TestWrite0005L();
	void TestWrite0006L();
	void TestWrite0007L();
	void TestWrite0008L();
	void TestWrite0009L();
	void TestWrite00010L();
	void TestWrite00011L();
	void TestWrite00016L();
	void TestWrite00017L();
	void TestWrite00018L();
	void TestWrite00019L();
	void TestWrite00020L();
	void TestWrite00021L();
	void TestWrite00022L();
	void TestWrite00023L();
	void TestWrite00024L();
	void TestWrite00025L();
	void TestWrite00026L();
	void TestWrite00027L();
	void TestWrite00028L();
	void TestWrite00029L();
	void TestWrite00030L();
	void TestNotifyStoreEvent0001L();
	void TestNotifyStoreEvent0002L();
	void TestNotifyStoreEvent0004L();
	void TestNotifyStoreEvent0006L();
	void TestNotifyStoreEvent0007L();
	void TestNotifyStoreEvent0009L();
	void TestNotifyStoreEvent00011L();
	void TestNotifyStoreEvent00012L();
	void TestNotifyStoreEvent00016L();
	void TestNotifyStoreEvent00017L();
	void TestNotifyStoreEvent00019L();
	void TestNotifyStoreEvent00021L();
	void TestNotifyStoreEvent00022L();
	void TestNotifyStoreEvent00024L();
	void TestNotifyStoreEvent00026L();
	void TestNotifyStoreEvent00027L();
	void TestNotifyStoreEvent00029L();
	void TestDelete0001L();
	void TestDelete0002L();
	void TestDelete0003L();
	void TestDelete0004L();
	void TestDelete0005L();
	void TestDelete0006L();
	void TestDelete0007L();
	void TestDelete0008L();
	void TestDelete0009L();
	void TestDelete00010L();
	void TestDelete00011L();
	void TestDelete00016L();
	void TestDelete00017L();
	void TestDelete00018L();
	void TestDelete00019L();
	void TestDelete00020L();
	void TestDelete00021L();
	void TestDelete00022L();
	void TestDelete00023L();
	void TestDelete00024L();
	void TestDelete00025L();
	void TestDelete00026L();
	void TestDelete00027L();
	void TestDelete00028L();
	void TestDelete00029L();
	void TestDelete00030L();
    void TestGetInfo0001L();
    void TestGetInfo0001bL();
	//void TestGetInfo0002L();
	void TestGetInfo0003L();
	void TestGetInfo0004L();
	//void TestGetInfo0005L();
	void TestGetInfo0006L();
    void TestGetInfo0006bL();
    void TestGetInfo0006cL();
	//void TestGetInfo0007L();
	void TestGetInfo0008L();
	void TestGetInfo0009L();
	//void TestGetInfo00010L();
    void TestGetInfo00011L();
    void TestGetInfo00011bL();
    void TestGetInfo00011cL();
	void TestGetInfo00012L();
	void TestGetInfo00013L();
	void TestGetInfo00014L();
	void TestGetInfo00015L();
    void TestGetInfo00016L();
    void TestGetInfo00016bL();
	void TestGetInfo00017L();
	void TestGetInfo00018L();
	void TestGetInfo00019L();
	void TestGetInfo00020L();
    void TestGetInfo00021L();
    void TestGetInfo00021bL();
	//void TestGetInfo00022L();
	void TestGetInfo00023L();
	void TestGetInfo00024L();
	//void TestGetInfo00025L();
    void TestGetInfo00026L();
    void TestGetInfo00026bL();
	//void TestGetInfo00027L();
	void TestGetInfo00028L();
	void TestGetInfo00029L();
	//void TestGetInfo00030L();
	void TestDeleteAll0001L();
	void TestDeleteAll0002L();
	//void TestDeleteAll0003L();
	void TestDeleteAll0004L();
	void TestDeleteAll0005L();
	void TestDeleteAll0006L();
	void TestDeleteAll0007L();
	//void TestDeleteAll0008L();
	void TestDeleteAll0009L();
	void TestDeleteAll00010L();
	void TestDeleteAll00011L();
	void TestDeleteAll00016L();
	void TestDeleteAll00017L();
	//void TestDeleteAll00018L();
	void TestDeleteAll00019L();
	void TestDeleteAll00020L();
	void TestDeleteAll00021L();
	void TestDeleteAll00022L();
	//void TestDeleteAll00023L();
	void TestDeleteAll00024L();
	void TestDeleteAll00025L();
	void TestDeleteAll00026L();
	void TestDeleteAll00027L();
	//void TestDeleteAll00028L();
	void TestDeleteAll00029L();
	void TestDeleteAll00030L();
	//void TestGetInfo0002();
	//void TestGetInfo0005();
	//void TestGetInfo0007();
	//void TestGetInfo00010();
	//void TestGetInfo00022();
	//void TestGetInfo00025();
	//void TestGetInfo00027();
	//void TestGetInfo00030();
	//void TestDeleteAll0003();
	//void TestDeleteAll0008();
	//void TestDeleteAll00013();
	//void TestDeleteAll00018();
	//void TestDeleteAll00023();
	//void TestDeleteAll00028();

	void Test2Read0001L();
	void Test2Read0002L();
	void Test2Read0003L();
	void Test2Read0004L();
	void Test2Read0005L();
	void Test2Read0006L();
	
	void Test2Write0001L();
	void Test2Write0002L();
	void Test2Write0003L();
	void Test2Write0004L();
	void Test2Write0005L();
	void Test2Write0006L();

    void Test3Read0001L();
    void Test3Read0002L();
    void Test3Read0003L();
    
    void TestOpen0001L();
    
private:

    void OpenPhone2L(TInt aSimError = KErrNone);
    
    void ValidateReadEntryL(TInt aNumEntries, TInt aStartIndex, CPhoneBookBuffer& aPBBuffer, const TDesC& aPBName, CPhoneBookStoreEntry* aEntryPtr);
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

	void AuxNotifyStoreEvent1L(const TDesC& aName);
	void AuxNotifyStoreEvent2L(const TDesC& aName);
	void AuxNotifyStoreEvent4L(const TDesC& aName);

	void AuxDelete1L(const TDesC& aName);
	void AuxDelete2L(const TDesC& aName);
	void AuxDelete3L(const TDesC& aName);
	void AuxDelete4L(const TDesC& aName);
	void AuxDelete5L(const TDesC& aName);

	void AuxGetInfo1L(const TDesC& aName);
	void AuxGetInfo1bL(const TDesC& aName);
	void AuxGetInfo1cL();
	void AuxGetInfo2L(const TDesC& aName);
	void AuxGetInfo3L(const TDesC& aName);
	void AuxGetInfo4L(const TDesC& aName);
	void AuxGetInfo5L(const TDesC& aName);
	
	void AuxDeleteAll1L(const TDesC& aName);
	void AuxDeleteAll2L(const TDesC& aName);
	void AuxDeleteAll4L(const TDesC& aName);
	void AuxDeleteAll5L(const TDesC& aName);
	
	void Aux2ReadL(const TDesC& aName);
	void Aux2WriteL(const TDesC& aName);
	
	void SetPhonebookNumOfEntries( CStorageInfoData& aStorageData, TInt aNumOfEntries );  
	void SetPhonebookNumOfEntriesAndInitPhonebookL( TName aName, TInt aNumOfEntries );  
	
	void AuxNotifySDNStoreEvent1L();
	
	void FillPBEntry(CPhoneBookStoreEntry* aEntryPtr);
	}; // class CCTsyPhoneBookStoreFU

#endif // CCTSYPHONEBOOKSTOREFU_H

