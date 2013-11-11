// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TE_SIM_USIMREL6_TEST__
#define __TE_SIM_USIMREL6_TEST__

#include "Te_SimTestStepBase.h"
#include "Te_SimConstants.h"
#include "SimTsy.h"
#include <f32file.h>

class CTestConfig;
class CTestConfigSection;
class CSimPhone;

class CSimVideomailTest1 : public CSimUSimRel6TestsTestStepBase
	{
public:
	CSimVideomailTest1();
 	virtual TVerdict doTestStepL();
private:
	RMobilePhoneBookStore iPhonebook;	
	};

class CSimVideomailTest2 : public CSimUSimRel6TestsTestStepBase
	{
public:
	CSimVideomailTest2();
 	virtual TVerdict doTestStepL();
private:
	RMobilePhoneBookStore iPhonebook;	
	};
	
class CSimVideomailTest3 : public CSimUSimRel6TestsTestStepBase
	{
public:
	CSimVideomailTest3();
 	virtual TVerdict doTestStepL();
private:
	RMobilePhoneBookStore iPhonebook;	
	};
	
class CSimVideomailTest4 : public CSimUSimRel6TestsTestStepBase
	{
public:
	CSimVideomailTest4();
 	virtual TVerdict doTestStepL();
private:
	RMobilePhoneBookStore iPhonebook;	
	};
	
class CSimVideomailTest5 : public CSimUSimRel6TestsTestStepBase
	{
public:
	CSimVideomailTest5();
 	virtual TVerdict doTestStepL();
private:
	RMobilePhoneBookStore iPhonebook;	
	};
	
class CSimWlanTest1 : public CSimUSimRel6TestsTestStepBase
	{
public:
	CSimWlanTest1();
 	virtual TVerdict doTestStepL();	
	};
			
class CSimWlanTest2 : public CSimUSimRel6TestsTestStepBase
	{
public:
	CSimWlanTest2();
 	virtual TVerdict doTestStepL();	
	};

class CSimWlanTest3 : public CSimUSimRel6TestsTestStepBase
	{
public:
	CSimWlanTest3();
 	virtual TVerdict doTestStepL();	
	};

class CSimWlanTest4 : public CSimUSimRel6TestsTestStepBase
	{
public:
	CSimWlanTest4();
 	virtual TVerdict doTestStepL();	
	};
	
class CSimWlanTest5 : public CSimUSimRel6TestsTestStepBase
	{
public:
	CSimWlanTest5();
 	virtual TVerdict doTestStepL();	
	};
	
class CSimWlanTest6 : public CSimUSimRel6TestsTestStepBase
	{
public:
	CSimWlanTest6();
 	virtual TVerdict doTestStepL();	
public:
	CMobilePhoneStoredNetworkList* iNetworkList;
	CMobilePhoneStoredWlanSIDList* iWlanList;
	};
	
class CSimWlanTest7 : public CSimUSimRel6TestsTestStepBase
	{
public:
	CSimWlanTest7();
 	virtual TVerdict doTestStepL();	
	};	

class CSimWlanTest8 : public CSimUSimRel6TestsTestStepBase
	{
public:
	CSimWlanTest8();
 	virtual TVerdict doTestStepL();	
	};
			
class CSimWlanTest9 : public CSimUSimRel6TestsTestStepBase
	{
public:
	CSimWlanTest9();
 	virtual TVerdict doTestStepL();	
	};

class CSimWlanTest10 : public CSimUSimRel6TestsTestStepBase
	{
public:
	CSimWlanTest10();
 	virtual TVerdict doTestStepL();	
	};

class CSimWlanTest11 : public CSimUSimRel6TestsTestStepBase
	{
public:
	CSimWlanTest11();
 	virtual TVerdict doTestStepL();	
	};
	
class CSimWlanTest12 : public CSimUSimRel6TestsTestStepBase
	{
public:
	CSimWlanTest12();
 	virtual TVerdict doTestStepL();	
	};
	
class CSimWlanTest13 : public CSimUSimRel6TestsTestStepBase
	{
public:
	CSimWlanTest13();
 	virtual TVerdict doTestStepL();	
	};
	
class CSimWlanTest14 : public CSimUSimRel6TestsTestStepBase
	{
public:
	CSimWlanTest14();
 	virtual TVerdict doTestStepL();	
	};

class CSimWlanTest15 : public CSimUSimRel6TestsTestStepBase
	{
public:
	CSimWlanTest15();
 	virtual TVerdict doTestStepL();	
	};
			
class CSimWlanTest16 : public CSimUSimRel6TestsTestStepBase
	{
public:
	CSimWlanTest16();
 	virtual TVerdict doTestStepL();	
	};

class CSimWlanTest17 : public CSimUSimRel6TestsTestStepBase
	{
public:
	CSimWlanTest17();
 	virtual TVerdict doTestStepL();	
	};

class CSimWlanTest18 : public CSimUSimRel6TestsTestStepBase
	{
public:
	CSimWlanTest18();
 	virtual TVerdict doTestStepL();	
	};
	
class CSimWlanTest19 : public CSimUSimRel6TestsTestStepBase
	{
public:
	CSimWlanTest19();
 	virtual TVerdict doTestStepL();	
	};
	
class CSimWlanTest20 : public CSimUSimRel6TestsTestStepBase
	{
public:
	CSimWlanTest20();
 	virtual TVerdict doTestStepL();	
	};
	
class CSimWlanTest21 : public CSimUSimRel6TestsTestStepBase
	{
public:
	CSimWlanTest21();
 	virtual TVerdict doTestStepL();	
	};

class CSimWlanTest22 : public CSimUSimRel6TestsTestStepBase
	{
public:
	CSimWlanTest22();
 	virtual TVerdict doTestStepL();	
	};

class CSim3GSecurityContextTest1 : public CSimUSimRel6TestsTestStepBase
	{
public:
	CSim3GSecurityContextTest1();
 	virtual TVerdict doTestStepL();	
	};

class CSim3GSecurityContextTest2 : public CSimUSimRel6TestsTestStepBase
	{
public:
	CSim3GSecurityContextTest2();
 	virtual TVerdict doTestStepL();	
	};

class CSim3GSecurityContextTest3 : public CSimUSimRel6TestsTestStepBase
	{
public:
	CSim3GSecurityContextTest3();
 	virtual TVerdict doTestStepL();	
	};
	
class CSim3GSecurityContextTest4 : public CSimUSimRel6TestsTestStepBase
	{
public:
	CSim3GSecurityContextTest4();
 	virtual TVerdict doTestStepL();	
	};
	
class CSim3GSecurityContextTest5 : public CSimUSimRel6TestsTestStepBase
	{
public:
	CSim3GSecurityContextTest5();
 	virtual TVerdict doTestStepL();	
	};
						
static TInt OtherCltWlan(void* aThreadData);
class CSimUsimR6TestWlanThread: public CSimUSimRel6TestsTestStepBase
	{
public:
	static CSimUsimR6TestWlanThread* getInstanceL();
	virtual TVerdict doTestStepL();
	
private:
	CSimUsimR6TestWlanThread();
	CSimUsimR6TestWlanThread(const CSimUsimR6TestWlanThread&);
	CSimUsimR6TestWlanThread& operator=(const CSimUsimR6TestWlanThread&);
	static CSimUsimR6TestWlanThread* instance;
	
	};
		
class CTestRetrieveMobilePhonePreferredWlanSIDs : public CActive
 	{
public:
  	static 	CTestRetrieveMobilePhonePreferredWlanSIDs* NewLC(RMobilePhone& aPhone);
  	~CTestRetrieveMobilePhonePreferredWlanSIDs();
  	void TestRetrieve(TBool aStartReqd);
  	void Start();
protected:
  	CTestRetrieveMobilePhonePreferredWlanSIDs(RMobilePhone& aPhone);
  	void ConstructL();
  	void RunL();
  	void DoCancel();
  	TInt CheckWLANResults();
protected:
  	CRetrieveMobilePhonePreferredWlanSIDs* iRetrieve;
  	RMobilePhone& iPhone;
public:
  	CSimWlanTest6* iTestWLAN;
   	};

class CTestGetPreferredNetworks : public CActive
	{
public:
	static CTestGetPreferredNetworks* NewLC(RMobilePhone& aPhone);
	~CTestGetPreferredNetworks();
	void Start();
protected:
	CTestGetPreferredNetworks(RMobilePhone& aPhone);
	void ConstructL();
	void RunL();
	void DoCancel();
	TInt CheckResults();
protected:
	CRetrieveMobilePhonePreferredNetworks* iRetrieve;
	RMobilePhone& iPhone;
public:
  	CSimWlanTest6* iTestPhone;
	};

class CSimUsimR6GBATest1 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6GBATest1();
 	virtual TVerdict doTestStepL();
private:
	};
	
class CSimUsimR6GBATest2 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6GBATest2();
 	virtual TVerdict doTestStepL();
private:
	};

class CSimUsimR6GBATest3 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6GBATest3();
 	virtual TVerdict doTestStepL();
private:
	};

class CSimUsimR6GBATest4 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6GBATest4();
 	virtual TVerdict doTestStepL();
private:
	};

class CSimUsimR6GBATest5 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6GBATest5();
 	virtual TVerdict doTestStepL();
private:
	};

class CSimUsimR6GBATest6 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6GBATest6();
 	virtual TVerdict doTestStepL();
private:
	};

class CSimUsimR6GBATest7 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6GBATest7();
 	virtual TVerdict doTestStepL();
private:
	};

class CSimUsimR6GBATest8 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6GBATest8();
 	virtual TVerdict doTestStepL();
private:
	};

class CSimUsimR6GBATest9 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6GBATest9();
 	virtual TVerdict doTestStepL();
private:
	};

class CSimUsimR6GBATest10 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6GBATest10();
 	virtual TVerdict doTestStepL();
private:
	};

class CSimUsimR6GBATest11 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6GBATest11();
 	virtual TVerdict doTestStepL();
private:
	};

class CSimUsimR6GBATest12 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6GBATest12();
 	virtual TVerdict doTestStepL();
	};
	
static TInt OtherCltGBA(void* aThreadData);
class CSimUsimR6GBATestThread: public CSimTestsTestStepBase
	{
public:
	static CSimUsimR6GBATestThread* getInstanceL();
	virtual TVerdict doTestStepL();
	TInt doFunction();
private:
	CSimUsimR6GBATestThread();
	CSimUsimR6GBATestThread(const CSimUsimR6GBATestThread&);
	CSimUsimR6GBATestThread& operator=(const CSimUsimR6GBATestThread&);
	static CSimUsimR6GBATestThread* instance;
	};
	
class CSimUsimR6GBATest13 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6GBATest13();
 	virtual TVerdict doTestStepL();
private:
	};

class CSimUsimR6GBATest14 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6GBATest14();
 	virtual TVerdict doTestStepL();
private:
	};

class CSimUsimR6GBATest15 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6GBATest15();
 	virtual TVerdict doTestStepL();
private:
	};

class CSimUsimR6GBATest16 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6GBATest16();
 	virtual TVerdict doTestStepL();
private:
	};

class CSimUsimR6GBATest17 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6GBATest17();
 	virtual TVerdict doTestStepL();
private:
	};

class CSimUsimR6GBATest18 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6GBATest18();
 	virtual TVerdict doTestStepL();
private:
	};

class CSimUsimR6GBATest19 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6GBATest19();
 	virtual TVerdict doTestStepL();
private:
	};

class CSimUsimR6GBATest20 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6GBATest20();
 	virtual TVerdict doTestStepL();
private:
	};

class CSimUsimR6GBATest21 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6GBATest21();
 	virtual TVerdict doTestStepL();
private:
	};

class CSimUsimR6GBATest22 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6GBATest22();
 	virtual TVerdict doTestStepL();
private:
	};

class CSimUsimR6GBATest23 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6GBATest23();
 	virtual TVerdict doTestStepL();
private:
	};

/* MBMS Testcase classes - START */

	
class CSimUsimR6MBMSTest1 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6MBMSTest1();
 	virtual TVerdict doTestStepL();
private:
	};

	
class CSimUsimR6MBMSTest2 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6MBMSTest2();
 	virtual TVerdict doTestStepL();
private:
	};
		
		
class CSimUsimR6MBMSTest3 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6MBMSTest3();
 	virtual TVerdict doTestStepL();
private:
	};

class CSimUsimR6MBMSTest4 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6MBMSTest4();
	virtual TVerdict doTestStepL();
private:
	};

	
class CSimUsimR6MBMSTest5 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6MBMSTest5();
 	virtual TVerdict doTestStepL();
private:
	};
		
	
class CSimUsimR6MBMSTest6 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6MBMSTest6();
 	virtual TVerdict doTestStepL();
private:
	};

class CSimUsimR6MBMSTest7 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6MBMSTest7();
	virtual TVerdict doTestStepL();
private:
	};

	
class CSimUsimR6MBMSTest8 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6MBMSTest8();
 	virtual TVerdict doTestStepL();
private:
	};
		
		
class CSimUsimR6MBMSTest9 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6MBMSTest9();
 	virtual TVerdict doTestStepL();
private:
	};
	
class CSimUsimR6MBMSTest10 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6MBMSTest10();
	virtual TVerdict doTestStepL();
private:
	};

	
class CSimUsimR6MBMSTest11 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6MBMSTest11();
 	virtual TVerdict doTestStepL();
private:
	};
		
		
class CSimUsimR6MBMSTest12 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6MBMSTest12();
 	virtual TVerdict doTestStepL();
private:
	};
	
class CSimUsimR6MBMSTest13 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6MBMSTest13();
	virtual TVerdict doTestStepL();
private:
	};

	
class CSimUsimR6MBMSTest14 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6MBMSTest14();
 	virtual TVerdict doTestStepL();
private:
	};
		
		
class CSimUsimR6MBMSTest15 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6MBMSTest15();
 	virtual TVerdict doTestStepL();
private:
	};
	
class CSimUsimR6MBMSTest16 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6MBMSTest16();
	virtual TVerdict doTestStepL();
private:
	};

	
class CSimUsimR6MBMSTest17 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6MBMSTest17();
 	virtual TVerdict doTestStepL();
private:
	};
		
		
class CSimUsimR6MBMSTest18 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6MBMSTest18();
 	virtual TVerdict doTestStepL();
private:
	};
	
class CSimUsimR6MBMSTest19 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6MBMSTest19();
	virtual TVerdict doTestStepL();
private:
	};

class CSimUsimR6MBMSTest20 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6MBMSTest20();
 	virtual TVerdict doTestStepL();
private:
	};
		
		
class CSimUsimR6MBMSTest21 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6MBMSTest21();
 	virtual TVerdict doTestStepL();
private:
	};
	
class CSimUsimR6MBMSTest22 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6MBMSTest22();
 	virtual TVerdict doTestStepL();
private:
	};
	
class CSimUsimR6MBMSTest23 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6MBMSTest23();
 	virtual TVerdict doTestStepL();
private:
	};
	
class CSimUsimR6MBMSTest24 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6MBMSTest24();
 	virtual TVerdict doTestStepL();
private:
	};

class CSimUsimR6MBMSTest25 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6MBMSTest25();
 	virtual TVerdict doTestStepL();
private:
	};
	
class CSimUsimR6MBMSTest26: public CSimTestsTestStepBase
	{
public:
	CSimUsimR6MBMSTest26();
 	virtual TVerdict doTestStepL();
private:
	};
	
class CSimUsimR6MBMSTest27 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6MBMSTest27();
 	virtual TVerdict doTestStepL();
private:
	};
	
class CSimUsimR6MBMSTest28 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6MBMSTest28();
 	virtual TVerdict doTestStepL();
private:
	};
	
class CSimUsimR6MBMSTest29 : public CSimTestsTestStepBase
	{
public:
	CSimUsimR6MBMSTest29();
 	virtual TVerdict doTestStepL();
private:
	};
	
class CTestCanceller : public CAsyncOneShot
	{
public:
	CTestCanceller(CActive*);
	~CTestCanceller();
private:
	void ConstructL();
	void RunL();
private:
	CActive* iRetrieve;// not owned by this
	};
	
class CTestRetrieveMobilePhoneAuthenticationIds : public CActive
	{
public:
	enum TTypeOfAuth
		{
		EGba,
		EMbms
		};
	static CTestRetrieveMobilePhoneAuthenticationIds* NewLC(RMobilePhone& aPhone,const TTypeOfAuth aType);
	~CTestRetrieveMobilePhoneAuthenticationIds();
	void SetAuthType(TTypeOfAuth aVersion);
	void TestRetrieve(RMobilePhone::TAuthData aAuthData);
	void TestCancel(RMobilePhone::TAuthData aAuthData);
protected:
	CTestRetrieveMobilePhoneAuthenticationIds(RMobilePhone& aPhone,const TTypeOfAuth aType);
	void ConstructL();
	void RunL();
	void DoCancel();
	void Start(RMobilePhone::TAuthData aAuthData);
	CTestCanceller* RetrieveCancelPtr();
	TInt CheckGBAResults();
	TInt CheckMBMSResults();
protected:
	CRetrieveMobilePhoneAuthenticationIds* iRetrieveAuthId;
	CTestCanceller* iCanceller;
	RMobilePhone& iPhone;
	CSimUsimR6GBATest14* iGBATest;
private:
	TTypeOfAuth iAuthType;		//Used to determine whether to run the test for Gba or Mbms,
	};
				
#endif // __TE_SIM_USIMREL6_TEST__
