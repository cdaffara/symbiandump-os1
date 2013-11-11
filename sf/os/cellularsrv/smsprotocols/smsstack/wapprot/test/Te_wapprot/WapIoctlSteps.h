/**
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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

/**
    @file
    @test
*/

#ifndef WAPIOCTLTESTSTEPS_H
#define WAPIOCTLTESTSTEPS_H

#include "WapProtSuiteStepBase.h"

class CIoctlStep_1 : public CWapProtSuiteStepBase
/**
 *  Sends/receives a 7_Bit business card using IOCTL
 */
	{
public:
	virtual TVerdict doTestStepL();
	};

class CIoctlStep_2 : public CWapProtSuiteStepBase
/**
 *  Send/receive a long 7-Bit calendar entry using IOCTL(two segments)
 */
	{
public:
	virtual TVerdict doTestStepL();
	};

class CIoctlStep_3 : public CWapProtSuiteStepBase
/**
 *  send/receive one 7-Bit two segments message and one 7-Bit one segment message
 */
	{
public:
	virtual TVerdict doTestStepL();
	};

class CIoctlStep_4 : public CWapProtSuiteStepBase
/**
 *  Simulating client crash conditions with 7bit message
 */
	{
public:
	virtual TVerdict doTestStepL();
	};

class CIoctlStep_5 : public CWapProtSuiteStepBase
/**
 *  Simulating client crash conditions with 8bit push message
 */
	{
public:
	virtual TVerdict doTestStepL();
	};

class CIoctlStep_6 : public CWapProtSuiteStepBase
/**
 *  Testing the old client for 7Bit message
 */
	{
public:
	virtual TVerdict doTestStepL();
	};

class CIoctlStep_7 : public CWapProtSuiteStepBase
/**
 *  Testing push messages (8-Bit)
 */
	{
public:
	virtual TVerdict doTestStepL();
	};

class CIoctlStep_8 : public CWapProtSuiteStepBase
/**
 *  Testing store of 8-Bit messages
 */
	{
public:
	virtual TVerdict doTestStepL();
	};

class CIoctlStep_9 : public CWapProtSuiteStepBase
/**
 *  Testing old clientwith 8-Bit messages
 */
	{
public:
	virtual TVerdict doTestStepL();
	};

class CIoctlStep_10 : public CWapProtSuiteStepBase
/**
 *  Testing wrong port numebr (i.e.8Bit port number) for 7Bit message
 */
	{
public:
	virtual TVerdict doTestStepL();
	};

class CIoctlStep_11 : public CWapProtSuiteStepBase
/**
 *  Testing CancelIOCTL
 */
	{
public:
	virtual TVerdict doTestStepL();
	};

class CIoctlStep_12 : public CWapProtSuiteStepBase
/**
 *  Test issue IOCTL without SetOption NewStyleClient
 */
	{
public:
	virtual TVerdict doTestStepL();
	};

class CIoctlStep_13 : public CWapProtSuiteStepBase
/**
 *  Test sending 8-Bit Calendar entry
 */
	{
public:
	virtual TVerdict doTestStepL();
	};

class CIoctlStep_14 : public CWapProtSuiteStepBase
/**
 *  Simulating client crash conditions with 8bit message
 */
	{
public:
	virtual TVerdict doTestStepL();
	};

class CIoctlStep_15 : public CWapProtSuiteStepBase
/**
 *  Receives a 7Bit v-card using IOCTL with concatenation for new style client
 */
	{
public:
	virtual TVerdict doTestStepL();
	};

class CIoctlStep_17 : public CWapProtSuiteStepBase
/**
 *  Sends a 7_Bit business card. Using IOCTL query SMS transport parameters
 */
	{
public:
	virtual TVerdict doTestStepL();
	};
	
class CIoctlStep_18 : public CWapProtSuiteStepBase
/**
 *  Sends a 7-Bit calendar entry (two segments). Using IOCTL query SMS transport parameters.
 */
	{
public:
	virtual TVerdict doTestStepL();
	};
	
class CIoctlStep_19 : public CWapProtSuiteStepBase
/**
 *  Send/receive one 7-Bit two segments message and one 7-Bit one segment message. 
 *  Using IOCTL query SMS transport parameters.
 */
	{
public:
	virtual TVerdict doTestStepL();
	};
	
class CIoctlStep_20 : public CWapProtSuiteStepBase
/**
 *  Simulating client crash conditions with 7bit message.
 *  Using IOCTL query SMS transport parameters.
 */
	{
public:
	virtual TVerdict doTestStepL();
	};
	
class CIoctlStep_21 : public CWapProtSuiteStepBase
/**
 *  Simulating client crash conditions with 8bit push message.
 *  Using IOCTL query SMS transport parameters.
 */
	{
public:
	virtual TVerdict doTestStepL();
	};

class CIoctlStep_22 : public CWapProtSuiteStepBase
/**
 *  Push Message (8-Bit). Using IOCTL query SMS transport parameters
 */
	{
public:
	virtual TVerdict doTestStepL();
	};
	
class CIoctlStep_23 : public CWapProtSuiteStepBase
/**
 *  Two Push Messages (8-Bit). Using IOCTL query SMS transport parameters
 */
	{
public:
	virtual TVerdict doTestStepL();
	};

class CIoctlStep_24 : public CWapProtSuiteStepBase
/**
 *  Test Cancel IOCTL
 */
	{
public:
	virtual TVerdict doTestStepL();
	};
	
class CIoctlStep_25 : public CWapProtSuiteStepBase
/**
 *  Test issue IOCTL without SetOption NewStyleClient
 */
	{
public:
	virtual TVerdict doTestStepL();
	};
	
class CIoctlStep_26 : public CWapProtSuiteStepBase
/**
 *  Sends a 8-Bit calendar entry using IOCTL(two segments)
 */
	{
public:
	virtual TVerdict doTestStepL();
	};

class CIoctlStep_27 : public CWapProtSuiteStepBase
/**
 *  Sends a 7_Bit business card. Issue both a get length and get message perameters to IOCTL at sametime
 */
	{
public:
	TInt DoPanicTestL(TPtrC* aTestData1, TPtrC16* aTelNumber, TInt aPort);
	virtual TVerdict doTestStepL();
	};

class CIoctlStep_28 : public CWapProtSuiteStepBase
/**
 *  Sends a 7_Bit business card. Issue a get message parameters to IOCTL without correct platform security
 */
	{
public:

    virtual TVerdict doTestStepPreambleL();
	TInt DoPanicTestL(TPtrC* aTestData1, TPtrC16* aTelNumber, TInt aPort);
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	};


class CSetDiskMonitorLimits : public CWapProtSuiteStepBase
/**
 *  Setting the Disk space monitor space limits
 */
	{
public:
    CSetDiskMonitorLimits();
	virtual TVerdict doTestStepL();
	};

class CSetDiskSpace : public CWapProtSuiteStepBase
	{
public:
    CSetDiskSpace();
	virtual TVerdict doTestStepL();

	};
	
class CFreeDiskSpace : public CWapProtSuiteStepBase
/**
 *  Free the disk space
 */
	{
public:
	virtual TVerdict doTestStepL();

	};

class CReceiveWapMessage : public CWapProtSuiteStepBase
	{
public:
    CReceiveWapMessage();
	virtual TVerdict doTestStepL();
	};

/**
Tests enumerating of a VCard which needs to go via the observer and
not returned to the client.
*/
class CTestEnumeratingVCard : public CWapProtSuiteStepBase
	{
public:
	virtual TVerdict doTestStepL();
	};

/**
 *  Sends a 7-bit VCard with an email address in it, to ensure that '@'
 *  characters are sent correctly.
 */
class CTest7BitBusinessCardWithEmail : public CWapProtSuiteStepBase
	{
public:
	virtual TVerdict doTestStepL();
	};

/**
 *  Attempts to send datagrams bigger than the maximum message and bigger
 *  than the maximum size of an MBuf.
 */
class CTestOversizedDatagram : public CWapProtSuiteStepBase
	{
public:
	virtual TVerdict doTestStepL();
	};

class CTestWapDatagramSegmentContainingNoData : public CWapProtSuiteStepBase
 /**
  *  Receive 3 WAP datagrams. Each contains a datagram with at least 1
  *  segment which has a wap header but an empty data field.
  */
     {
public:
     virtual TVerdict doTestStepL();
     };

#endif // WAPIOCTLTESTSTEPS_H
