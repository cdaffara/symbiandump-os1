// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains declaration of test 04 that:
// - establishes client server/connection between two threads;
// - tranfers short message.
// 
//

#if (!defined __TEST04IPCMULTITHREADEDDATATRANSFER_H__)
#define __TEST04IPCMULTITHREADEDDATATRANSFER_H__

#include "TestStepIPC.h"

class CTest04IPCMultithreadedDataTransfer : public CIPCTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		CTest04IPCMultithreadedDataTransfer();
		~CTest04IPCMultithreadedDataTransfer();
		virtual enum TVerdict InternalDoTestStepL( void );
	private:
		static TInt fReceiverThread(TAny*);   //This is the thread that will receive data
		static TInt fTransmitterThread(TAny*);//This is the thread that will send data
		/* This thread ensures that if either of the other threads hang (due to 
		 * uncompleted asynchronous call), this test is not timed out by TEF. */
		static TInt fInactivityTimeoutThread(TAny*);
		
		TInt iThreadStackSize;         //Stack size of the threads
		TBufC<8> iSemName;		       //Will contain semaphore name: "IPC sem"
		TBufC8<0x20> iTestString;      //Will contain test string
		TBuf<200> iReceiveThreadResult;
		TBuf<200> iTransmitThreadResult;
		RThread iThreadRec;
		RThread iThreadTr;
		TRequestStatus iRecStatus;
		TRequestStatus iTrStatus;
		
	};	
#endif //(__TEST04IPCMULTITHREADEDDATATRANSFER_H__)

