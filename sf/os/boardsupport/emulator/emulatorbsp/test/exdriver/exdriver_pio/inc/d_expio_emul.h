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
// This is a header file to define wins specific classes and info
// 
//

#ifndef __D_EXPIO_EMUL_H__
#define __D_EXPIO_EMUL_H__

// pdd include file
#include "d_expio_pdd.h"

// include files
#include <d32comm.h> // comm header file

// DFC related constants
//
_LIT(KExEmulUartDfcName,"ExEmulUartDfc"); 		// Name of the DFC thread
const TInt KExEmulUartDfcPriority=26; 		// DFC thread priority		

// UART FIFO sizes
const TInt KTxFifoSize=64; // 64bytes fifo
const TInt KRxFifoSize=64; // 64bytes fifo

// Timer constants
const TInt KRxPollTimeout=1; // 1ms
const TInt KTimerStarted=1;
const TInt KTimerExpired=0;
const TInt KTimerCancelled=-1;


/**
 * Physical device class specific for Emulator UART Physical Device Driver (PDD)
 */
class DExEmulPhysicalDevice: public DExDriverPhysicalDevice
{
public:
    DExEmulPhysicalDevice();   
    ~DExEmulPhysicalDevice();  // Destructor
    TInt Create(DBase*& aChannel, TInt aUnit, const TDesC8* aInfo, 
    										const TVersion& aVer);
};

/**
 Hardware peripheral class
 
 Target specific class, H4 uart controller. This class 
 is accessed from physical channel class. It's object 
 is created in physical channel DoCreate()and deleted 
 in it's destructor
 */ 
class DExUartPhysicalChannelEmul:public DExDriverPhysicalChannel
	{
public:
	// Public functions access to all
	DExUartPhysicalChannelEmul();					// Constructor	
	~DExUartPhysicalChannelEmul();					// Destructor	
	TInt DoCreate(TInt aUnit, const TDesC8* aInfo, 
								const TVersion& aVer); // 2nd Stage constructor
	void Caps(TDes8& aCaps);						// Get channel Capabilities
	TInt Configure(const TCommConfigV01& aConfig);  // Configure the uart	
	TInt TransmitData(const TDesC8& aData);			// Transmit data to uart	
	TInt ReceiveData(TDes8& aData,TInt aLen);					// Receive data from uart	
	TDynamicDfcQue* DfcQ();							// DFC queue creation
private:
	static void RxPollTimerCallback(TAny* aPtr);	
private:	
	TBool iConfigured;				// Channel configuration status;
	TBps iBaudRate;		
	NTimer iRxPollTimer;			// Timer to poll for Rx data
	TInt iTimerStatus;				// Rx data timer status
	// Windows handle for com port
	HANDLE hCommPort;
	DWORD iBytesRead;
	DWORD iBytesWritten;
	};
		
#endif // __D_EXPIO_EMUL_H__

//
// End of d_expio_emul.h