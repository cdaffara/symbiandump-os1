// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef TE_NET_C32MT_SUITE
#define TE_NET_C32MT_SUITE
#include "TE_c32base.h"
 
#define STATE (41)
#define NO_OF_SESSION (7)
#define PORT_NUMBER (3)

const TInt KDummy12ReadWaitContingency = 2000000; //time in micro sec, which is to check for the loopback csy to read/write operation completion
  /*
	KMaxBlockingTestWrites = 4;            
		attempt to perform four read/write operation in loopback while waiting for blocking dummy12, 
		4 being enough to simulate quite a bit of "work".

	KMicroSecWaitBetnLoopbackWrites = 1000000;    
		1 sec is reasonable time to wait between loopback writes 
		to ensure processing interference is miniscule

	KMaxExpectedLoopbackWriteTime = 
		KMicroSecWaitBetnLoopbackWrites  * KMaxBlockingTestWrites;
		1000000 * 4 = 4000000
		
	KDummy12ReadWaitContingency = 2 * KMicroSecWaitBetnLoopbackWrites;
	    allow 2 extra wait times for loopback writes to complete before releasing dummy12 to compensate for any processing time
        2 * 1000000 = 2000000
                 
	//KDummy12ReadWait = KMaxExpectedLoopbackWriteTime  + KDummy12ReadWaitContingency;
	//  dummy12 waits about 6 seconds
	//	4000000 + 2000000 = 6000000
	const TInt KDummyCompleteDelayFast = 600000L;
	const TInt KDummyCompleteDelaySlow = 1200000L;	
	
	KDummy12ReadWait=(KDummyCompleteDelayFast  * KMaxBlockingTestWrites) 
					+ (KDummy12ReadWaitContingency * 2);	
					
					600000 * 4 + (200000 * 2) = 2800000
 */

const TInt KNoOfCharsLoopbackExpectsToBeWrittern = 20; // Number characters to be read and write in the Loopback port 	
const TInt KNoOfCharsDummyExpectsToBeWrittern  = 82;   // Number characters to be read and write in the Loopback port 	
const TInt KMaxBlockingTestWrites      = 4 ;           // attempt perform four read/write ops on loopback while waiting for blocking dummy12, 
										 	           // 4 being enough to simulate quite a bit of "work". 

// Test Case No NET-C32-I-001-HP
// Loading a CSY in Dealer thread
class CTestLoadDealerCSY : public CC32TestStep
	{
public:
	CTestLoadDealerCSY();
	~CTestLoadDealerCSY(){};
	virtual TVerdict doTestStepL();
    };

// Test Case No NET-C32-I-002-HP
// Load CSY in new player thread(OnDemand=1)
class CTestLoadPlayerCSY : public CC32TestStep
	{
public:
	CTestLoadPlayerCSY();
	~CTestLoadPlayerCSY(){};
	virtual TVerdict doTestStepL();
    };

// Test Case No NET-C32-I-003-HP
// Load CSY in the existing player thread(OnDemand=0)
class CTestLoadCSYthread : public CC32TestStep
	{
public:
	CTestLoadCSYthread();
	~CTestLoadCSYthread(){};
	virtual TVerdict doTestStepL();
    };

// Test Case No NET-C32-I-004-HP
// Loading more than one CSYs
class CTestLoadMoreCSY : public CC32TestStep
	{
public:
	CTestLoadMoreCSY();
	~CTestLoadMoreCSY(){};
	virtual TVerdict doTestStepL();
    };

// Test Case No NET-C32-I-005-HP
// Unload a CSY and make a call to other CSY in the same player thread
class CTestApiCallCSY : public CC32TestStep
	{
public:
	CTestApiCallCSY();
	~CTestApiCallCSY(){};
	virtual TVerdict doTestStepL();
    };

// Test case id NET-C32-I-006-HP
// loading  Unknown CSY
class CTestLoadUnknownCSY : public CC32TestStep
	{
public:
	CTestLoadUnknownCSY();
	~CTestLoadUnknownCSY(){};
	virtual TVerdict doTestStepL();
    };

// Test case id NET-C32-I-007-HP
// loading a CSY with corrupted CMI file.
class CTestMissingRoleParmCMI : public CC32TestStep
	{
public:
	CTestMissingRoleParmCMI();
	~CTestMissingRoleParmCMI(){};
	virtual TVerdict doTestStepL();
	TVerdict doTestStepPreambleL( void );
    TVerdict doTestStepPostambleL( void );
    };

// Test case id NET-C32-I-008-HP
// loading a CSY with corrupted CMI file.
class CTestMissingWorkerIDParm : public CC32TestStep
	{
public:
	CTestMissingWorkerIDParm();
	~CTestMissingWorkerIDParm(){};
	virtual TVerdict doTestStepL();
	TVerdict doTestStepPreambleL( void );
    TVerdict doTestStepPostambleL( void );
	};

// Test Case No NET-C32-I-009-HP
// Loading and Unloading the CSY in Dealer and Player thread in same order
class CTestLoadUnLoadCsySameOrder : public CC32TestStep
	{
public:

	CTestLoadUnLoadCsySameOrder();
	~CTestLoadUnLoadCsySameOrder(){};
	virtual TVerdict doTestStepL();
    };

// Test Case No NET-C32-I-0010-HP
// Loading and Unloading the CSY in Dealer and Player thread in same order
class CTestLoadUnLoadCsyDiffOrder : public CC32TestStep
	{
public:

	CTestLoadUnLoadCsyDiffOrder();
	~CTestLoadUnLoadCsyDiffOrder(){};
	virtual TVerdict doTestStepL();
    };

// Test Case No NET-C32-I-0011-HP
// Loading a CSY in Dealer thread , open the port send and recevie the data
class CSendDatainDealerThr : public CC32TestStep
	{
public:
	 CSendDatainDealerThr();
	~CSendDatainDealerThr(){};
	virtual enum TVerdict doTestStepL();
	static TInt DataReadThread(TAny* aArg);
private:
	RSemaphore iSem;
	TInt iState1;		// test 1
	TInt iState2;		// test 2
	TInt iState3;		// test 3
	TInt iState4;		// test 3
	};

// Test Case No NET-C32-I-0012-HP
// Loading a CSY in Player thread , open the port send and recevie the data
class CSendDatainPlayerThr : public CC32TestStep
	{
public:
	CSendDatainPlayerThr();
	~CSendDatainPlayerThr(){};
	virtual enum TVerdict doTestStepL();
	static TInt DataReadThread(TAny* aArg);
private:
	RSemaphore iSem;
	TInt iState1;		// test 1
	TInt iState2;		// test 2
	TInt iState3;		// test 3
	TInt iState4;		// test 3
	};

// Test Case No NET-C32-I-0013-HP
// Loading and Unloading Different CSYs make GetPortInfo API call
class CTestGetPortInfo : public CC32TestStep
	{
public:
  	CTestGetPortInfo();
	~CTestGetPortInfo(){};
	virtual TVerdict doTestStepL();
    };

// Test Case No NET-C32-I-0014-HP
// Loading a 2 CSY from differrent sessions
class CLoadingCsydiffSession : public CC32TestStep
	{
public:
	 CLoadingCsydiffSession();
	~CLoadingCsydiffSession(){};
	virtual enum TVerdict doTestStepL();
	static TInt Session(TAny* aArg);
private:
	RSemaphore iSem;
	TInt iState1;		// test 1
	TInt iState2;		// test 2

	};

// Test Case No NET-C32-I-0015-HP
// Loading a same CSY from differrent sessions
class CLoadingSameCsydiffSession : public CC32TestStep
	{
public:
	 CLoadingSameCsydiffSession();
	~CLoadingSameCsydiffSession(){};
	virtual enum TVerdict doTestStepL();
	static TInt Session(TAny* aArg);

private:
	RSemaphore iSem;
	TInt iState1;		// test 1
	TInt iState2;		// test 2

	};

// Test Case No NET-C32-I-0016-HP
// Getting number of CSY Loaded from different  session
class CNumPortsdiffSession : public CC32TestStep
	{

public:
   CNumPortsdiffSession();
   ~CNumPortsdiffSession(){};
   virtual enum TVerdict doTestStepL();
   static TInt Session(TAny* aArg);

private:
	RSemaphore iSem;
	TInt iState1;		// test 1
	TInt iState2;		// test 2
	TInt iNPorts;       // test 3
	};

// Test Case No NET-C32-I-0017-HP
// Reading and writing data in both Dealer and Player thread
class CDataReadDealerPlayer : public CC32TestStep
	{

public:
   CDataReadDealerPlayer();
   ~CDataReadDealerPlayer(){};
   virtual enum TVerdict doTestStepL();
   static TInt Session(TAny* aArg);

private:
	RSemaphore iSem;
	TInt iState[7];		// test 1
	RComm iPortList[NUMBER_OF_PORTS];
	};

// Test Case No NET-C32-I-0019-HP
// CSY is communicating with anoter CSY which is loaded in another player Thread
class CTestStackableCSY : public CC32TestStep
	{
public:
	CTestStackableCSY();
	~CTestStackableCSY(){};
	virtual TVerdict doTestStepL();
    };

// Test Case No NET-C32-I-0020-HP
// Loading a CSYs in old cmi file
class CLoadCsywithOldCMI : public CC32TestStep
	{
public:
	CLoadCsywithOldCMI();
	~CLoadCsywithOldCMI(){};
	virtual TVerdict doTestStepL();
    };

// Test Case No NET-C32-I-0021-HP
// Loading a CSY in SingleThreaded and Multi threaded CMI files
// checking for the backward compatability by putting old and new CMI files in privat directory
//old cmi file refers for Dealer thread and new cmi file refer for player threads
class COldandNewCMIfiles : public CC32TestStep
	{
public:
	 COldandNewCMIfiles();
	~COldandNewCMIfiles(){};
	virtual enum TVerdict doTestStepL();

private:

	};

// Test Case No NET-C32-I-0022-HP
// Loading a CSY in player thread, CMI file mentioned Role=Dealer for Dealer thread
//There should be minimum of two CMI file for Loading CSY in player thread one for dealer
//thread and other for player thread
class CLoadCsyPlayerThread : public CC32TestStep
	{
public:
	 CLoadCsyPlayerThread();
	~CLoadCsyPlayerThread(){};
	virtual enum TVerdict doTestStepL();

private:

	};

//Test Case No NET-C32-I-0023-HP
//Loading CSY in both Dealer and player thread, mentioned CMI files in tescase, should be available in private dir
//old cmi file and new cmi files mentioned Role=Dealer and Role=Player
class CLoadCsyDealerPlayerThreads : public CC32TestStep
	{
public:
	 CLoadCsyDealerPlayerThreads();
	~CLoadCsyDealerPlayerThreads(){};
	virtual enum TVerdict doTestStepL();

private:

	};

//Test Case No NET-C32-I-0024-HP
//Loading and Unloading CSY from different session which is loaded in Player thread
class CLoadandUnloadDiffSession : public CC32TestStep
	{
public:
	 CLoadandUnloadDiffSession();
	~CLoadandUnloadDiffSession(){};
	virtual enum TVerdict doTestStepL();
	static TInt Session(TAny* aArg);
private:
	TInt iState1;		// test 1
	TInt iState2;		// test 2
	};

//Test Case No NET-C32-I-0025-HP
//Loading a CSY in a thread in which CMI file has missing CSY list parameter
class CMissingCsyListinCMI : public CC32TestStep
	{
public:
	 CMissingCsyListinCMI();
	~CMissingCsyListinCMI(){};
	virtual enum TVerdict doTestStepL();
	TVerdict doTestStepPreambleL( void );
    TVerdict doTestStepPostambleL( void );
	};

//Test Case No NET-C32-I-0026-HP
//Loading a CSY in which all the CMI files available in private dir contains Role=Player
class CAllCMIFileRolePlayer : public CC32TestStep
	{
public:
	 CAllCMIFileRolePlayer();
	~CAllCMIFileRolePlayer(){};
	virtual enum TVerdict doTestStepL();
	TVerdict doTestStepPreambleL( void );
    TVerdict doTestStepPostambleL( void );
	};

//Test Case No NET-C32-I-0027-HP
//Loading a CSY in which Two CMI files set wildcard in private dir
class CTwoCMIfileSetWildcard : public CC32TestStep
	{
public:
	 CTwoCMIfileSetWildcard();
	~CTwoCMIfileSetWildcard(){};
	virtual enum TVerdict doTestStepL();
	};

//Test Case No NET-C32-I-0028-HP
//Loading a CSY in which No CMI files set wildcard in private dir
class CNoCMIfileSetWildcard : public CC32TestStep
	{
public:
	 CNoCMIfileSetWildcard();
	~CNoCMIfileSetWildcard(){};
	 virtual enum TVerdict doTestStepL();
	};

//Test Case No NET-C32-I-0029-HP
//Loading a CSY in which Two CMI files have same worker ID
class CTwoCMIsameWorkerID : public CC32TestStep
	{
public:
	 CTwoCMIsameWorkerID();
	~CTwoCMIsameWorkerID(){};
	virtual enum TVerdict doTestStepL();
	TVerdict doTestStepPreambleL( void );
    TVerdict doTestStepPostambleL( void );
	};

//Test Case No NET-C32-I-0030-HP
//Loading a CSY in which Two new CMI files have Role=Dealer
class CTwoNewCMIfileRoleDealer : public CC32TestStep
	{
public:
	 CTwoNewCMIfileRoleDealer();
	~CTwoNewCMIfileRoleDealer(){};
	virtual enum TVerdict doTestStepL();
	};

//Test Case No NET-C32-I-0031-HP
//Loading a CSY in which new CMI file Role=Dealer and old CMI file.
class COldNewCMIfileRoleDealer : public CC32TestStep
	{
public:
	 COldNewCMIfileRoleDealer();
	~COldNewCMIfileRoleDealer(){};
	virtual enum TVerdict doTestStepL();
	};

// Test Case No NET-C32-I-0034-HP
// Loading a CSYs in all new player Threads (OnDemand=1 in all 8 cmi files)
class CLoadCsyAllPlayerThread : public CC32TestStep
	{
public:
	CLoadCsyAllPlayerThread();
	~CLoadCsyAllPlayerThread(){};
	virtual TVerdict doTestStepL();
    };

// Test Case No NET-C32-I-0035-HP
// Loading  CSYs make GetPortInfo API call(passing index parameter as -ve)
class CGetPortInfo : public CC32TestStep
	{
public:

	CGetPortInfo();
	~CGetPortInfo(){};
	virtual TVerdict doTestStepL();
    };

// Test Case No NET-C32-I-0032-HP
// Loading a CSY in Dealer thread
class CTestBlockCSY : public CC32TestStep
	{
public:
	CTestBlockCSY();
	~CTestBlockCSY(){};
	virtual TVerdict doTestStepL();
    };

_LIT(KSemaphoreName,"SemName");


// Test Case No NET-C32-I-0033-HP
// Loading a CSY in Dealer thread
class CTestCorruptCsy: public CC32TestStep
	{
public:
	CTestCorruptCsy();
	~CTestCorruptCsy(){};
	virtual TVerdict doTestStepL();
    };

// Test Case No NET-C32-I-0038-HP
// Loading and Unloading Different CSYs make GetPortInfo by name as parameter API call
class CGetPortInfoByName : public CC32TestStep
	{
public:
   	CGetPortInfoByName();
	~CGetPortInfoByName(){};
	virtual TVerdict doTestStepL();
    };

// Test Case No NET-C32-I-0036-HP
// Loading a CSY in player thread two cmi file set unsequential worker id
//Three cmi file set WorkerID=0. WorkerID=1, other cmi file set WorkerID=3
class CUnSequentialWorkerID: public CC32TestStep
	{
public:
	CUnSequentialWorkerID();
	~CUnSequentialWorkerID(){};
	virtual TVerdict doTestStepL();
    };
    
// Test Case No NET-C32-I-0038-HP
// Loading a CSY in player but not in c32.dll thread 
class CTestLoadUnknownCPM : public CC32TestStep
	{
public:
	 CTestLoadUnknownCPM();
	~CTestLoadUnknownCPM(){};
	virtual TVerdict doTestStepL();
    };
    
// Test Case No NET-C32-I-0037-HP
// Loading a CSY, having a long port name.
  class CTestLongPortName : public CC32TestStep
	{
public:
	 CTestLongPortName();
	~CTestLongPortName(){};
	virtual TVerdict doTestStepL();
    }; 
     
// Test Case No NET-C32-I-0039-HP
// CSY is communicating with anoter CSY which is loaded in another player Thread
class CTestStackableCSYNoClose : public CC32TestStep
	{
public:
	CTestStackableCSYNoClose();
	~CTestStackableCSYNoClose(){};
	virtual TVerdict doTestStepL();
    };
    

#endif // TE_NET_C32MT_SUITE

