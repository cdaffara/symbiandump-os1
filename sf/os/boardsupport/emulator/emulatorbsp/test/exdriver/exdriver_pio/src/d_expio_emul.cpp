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
// This file implements the DExUartPhysicalChannelEmul class functions. 
// This is the implementation os serila port driver for emulator
// target, i.e serial port of windows PC. This pdd is actually an
// application for windows serial driver and therefore uses windows
// API to access windows serial driver functionality
// 
//

// include h4 specific header file
#include "d_expio_emul.h"

/** 
 PDD entry point
 This function is called when the PDD is loaded. This creates a factory
 object for PDD. DECLARE_STANDARD_PDD macro defines the first export to
 represent the DLL factory function, that will be called first by loader
 after loading the PDD. 
 */
DECLARE_STANDARD_PDD()
	{	
	// Create a PDD factory object, i.e instance of DPhysicalDevice derived 
	// class. This is the first step that is done after loading the driver.
	// Physical device constructor inturn creates a Physical Channel.
	//		
	DExEmulPhysicalDevice* pD = new DExEmulPhysicalDevice;
	if(pD)
		{
		// TDynamicDfcQue is used for creating and destroying a DFC as needed
		// This extends TDfcQue with destroy method. This ensures there is
		// no memory leak done while using DFC queue 
		//
		TDynamicDfcQue* pDfcQ;
		// Create a new DFC queue using kernel API, Kern::DynamicDfcQCreate(). It 
		// allocates a TDynamicDfcQue object on the heap and initializes it with 
		// provided parameters like thread priority for the queue. Thread priority
		// has to be chosen carefully, as priority higher than the kernel threads 
		// used for timers may adversely effect the system performance, nano 
		// kernel ticks.
		//
		TInt r = Kern::DynamicDfcQCreate(pDfcQ,KExEmulUartDfcPriority,
														KExEmulUartDfcName);
		if (KErrNone==r)
			{
			// Store the DFC thread pointer to return when required
			pD->iDfcQueue = pDfcQ;
			// Success return point
			return pD;
			}
		// if DFCQ creation failed, then fail the PDD loading, hence asynchronously close	
		// the LDD factory object created.
		//
		pD->AsyncClose();
		}
	// Failure exit point	
	return NULL;
	}

/**
 PDD factory constructor. This is called while creating the PDD factory
 object as a part of the driver (PDD) loading. This is called after the
 base class constructor, i.e DExUartPhysicalDevice()
 */
DExEmulPhysicalDevice::DExEmulPhysicalDevice()	
	{	
	// if multiple units are supported, then iUnitsMask is set here
	// to indicate the units being supported by the driver.	
	}

/**
 Physical device destructor. This is called whicle unloading the PDD
 */
DExEmulPhysicalDevice::~DExEmulPhysicalDevice()	
	{	
	// If a Dynamic Dfc Queue is created, delete it to ensure there is no
	// memory leak.
	//
	if (iDfcQueue)
		{
		// Destroys the DFC queue.The function destroys the DFC queue, killing
		// the DFC thread and deleting the TDynamicDfcQue object itself
		//
		iDfcQueue->Destroy();
		}
	}

/** 
 PDD factory object (physical device) create. This is called by framework
 to create a physical channel. It is called in the context of the client 
 user-side thread that requested the creation of the logical channel. 
 This is a result of a user-side call to RBusLogicalChannel::DoCreate().
 
 @param 	aChannel
 			reference to the physical channel object created
 
 @return	KErrNone for success or KErrNoMemory for failure
 */ 
TInt DExEmulPhysicalDevice::Create(DBase*& aChannel,TInt aUnit, const TDesC8* aInfo, 
								const TVersion& aVer)
	{		
	// Create the Physical channel		
	DExUartPhysicalChannelEmul *device = new DExUartPhysicalChannelEmul;
	if (!device)
		return KErrNoMemory;
	
	aChannel = device;
	
	// Call the second stage contructor of physical channel
	return device->DoCreate(aUnit,aInfo,aVer);	
	}		
		
/**
 Constructor for physical channel. Called after the base class constructor
 */
DExUartPhysicalChannelEmul::DExUartPhysicalChannelEmul()
:iConfigured(EFalse),
iBytesRead(0),
 iRxPollTimer(RxPollTimerCallback,this)	// Timer to poll Rx data
	{	
	}

/**
 Hardware peripheral class (uart) Destructor
 */
DExUartPhysicalChannelEmul::~DExUartPhysicalChannelEmul()
	{
	// Cancel the Rx poll timer. NTimer.Cancel() Cancel a nanokernel 
	// timer. Removes this timer from the nanokernel timer queue. 
	// Does nothing if the timer is inactive or has already expired. 
	// If the timer was queued and DFC callback requested it is possible 
	// for the expiry handler to run even after Cancel() has been called.
	//
	if (iTimerStatus==KTimerStarted)
		iRxPollTimer.Cancel();

	// close the port if the handle is invalid
	if (hCommPort!=INVALID_HANDLE_VALUE)
	{
		// Close the Serial Port after reading data
		CloseHandle(hCommPort);
		KEXDEBUG(Kern::Printf("Emulator::Serial Port Closed"));
	}
	}

/**
 Physical channel second stage constructor. This is called from the DPhysicalDevice::Create()
 after creating the physical channel. Any further initializations as a part of the physical
 channel creation are done here.
 
 @param	aUnit
 		device unit number
 @param	aInfo
 		device related information
 @param	aVer
 		version number
 @return
 */
TInt DExUartPhysicalChannelEmul::DoCreate(TInt aUnit, const TDesC8* aInfo, const TVersion& aVer)
    { 
    TInt r;
     
    // We are not using these parameters here, and therefore making them void to avoid any
    // warning messages during build. If multiple units are supported, creating the channel
    // w.r.t unit can be controlled here
    //
    (void)aUnit;     
    (void)aInfo;
    (void)aVer;
    
    // TCommConfigV01 structure defined in d32comm.h is used
 	// to hold the configuration details like baudrate, parity,
 	// databits etc for serial
 	//
    TCommConfigV01 cfg;
    // Configure the channel by default, (9600 baud rate)
    cfg.iRate = EBps9600;
    r = Configure(cfg);
    if (r!=KErrNone)
    	{
    	// Configuration failed, still continue by updating device state
    	iConfigured=EFalse;
    	}
    else
    	{
    	// Device configured successfully
        iConfigured=ETrue;		    
    	}
    
    // Physical channel creation is successful
    return KErrNone;    
    }
    
/**   
 DfcQ - Creates a DFC queue dedicated for the tutorial driver. By default
 logical channel derived from DLogicalChannel has a DFCQ, and DFC thread 0
 is generally used. However, driver can choose o create it's own DFC and
 use it for queuing it's messages to this queue. In this case, PDD can 
 implement this function, DfcQ(), which is called by LDD to initialize it's
 messgage queue with this DFC.
 
 @return	refernce to the created TDynamicDfcQue object
 */
TDynamicDfcQue* DExUartPhysicalChannelEmul::DfcQ()    
	{
	// return the dfc thread created for this driver. one per uint/device
	return ((DExEmulPhysicalDevice*)iLdd->iPhysicalDevice)->iDfcQueue;		
	}

/** 
 Get the capabilities of the channel. This can be used by the user to get
 the capabilities for the channel from PSL.
 
 @param	aCaps
 		descriptor returned after filling with capabilities 
 */ 
void DExUartPhysicalChannelEmul:: Caps(TDes8& aCaps)
	{
	// Package buffer of TCommCapsV03. This creates a descriptor
	// for the commcaps structure, and provide compatibility
	// to use with API using descriptors
	//
	TCommCaps3 capsBuf;    
	  
	// Retrieves the data structure from the package buffer. TCommCapsV03
	// holds the uart capabilities information.
    //
	TCommCapsV03 &caps=capsBuf();
	
	caps.iRate=KCapsBps9600;	// baudrate
	caps.iDataBits=KCapsData8;	// data size
	caps.iFifo=KCapsHasFifo;	// fifo enabled
	caps.iBreakSupported=EFalse;// no braek support	

	// [TDes8::MaxLength()] - Get the descriptor's length.
	TInt len = aCaps.MaxLength();
	
	// [TDes8::FillZ(len)] -Fill the descriptor's data area with binary 
	// zeroes, replacing any existing data and change its length. 
	aCaps.FillZ(len);
	
    TInt size = sizeof(caps);
    if (size>len)
    	size=len;
    
    // [TDes8::Copy()] - Copy the data of length (size) into aDes descriptor
    //  replacing any existing data in the descriptor.
    aCaps.Copy((TUint8*)&caps, size);
        	
	aCaps=capsBuf.Left(Min(capsBuf.Length(),aCaps.MaxLength()));
	}
		
/**
 Configure the hardware device (Uart). This is device specific API, that 
 provides functionality to configure the uart. Uart configuration settings are 
 passed to this function by user. User calls this by using 
 RBusLogicalChannel::DoControl() to LDD and ldd inturn calls this PDD function
 to do the actual operation on the device.
  
 @param	aConfig
 		configuration settings for the device
 
 @return	KErrNone or standard error code
 */
TInt DExUartPhysicalChannelEmul::Configure(const TCommConfigV01& aConfig)
	{

	// if channel is already configured to same baud rate, then no need to 
	// repeat the configuration
	//
	if (iConfigured!=EFalse)
	{
		if(iBaudRate==aConfig.iRate)
			return KErrNone;
	}
	
	iBaudRate = aConfig.iRate;

	// variables used with the com port
	BOOL     bPortReady;
	DCB      dcb;
	COMMTIMEOUTS commTimeouts;

	bPortReady = TRUE; // everything is OK so far
	
	// Open a serial port device using the CreateFile() function by specifying
	// a filename that refers to the specific port, such as COM1 or COM2.
	// 
	// When you open a serial port, it is opened automatically for exclusive access,
	// so you should pass a zero for the CreateFile()'s third parameter and OPEN_EXISTING
	// for the open mode (fifth parameter). You can add a combination of the special file
	// mode flags to indicate overlapped I/O or any special buffering requirements, as normal.
	// 
	// If the serial port opens successfully, a Win32 file object handle is returned.
	// Otherwise, INVALID_HANDLE_VALUE is returned.
	//
	hCommPort=CreateFileA((LPCSTR)"\\\\.\\com1",GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
	
	if (hCommPort==INVALID_HANDLE_VALUE)	
	{	
		Kern::Printf("Emulator::Configure: FAILED");
		
		bPortReady = FALSE;

		DWORD winErr=GetLastError();
		switch (winErr)
		{
			case ERROR_INVALID_USER_BUFFER:
			case ERROR_NOT_ENOUGH_MEMORY:
			case ERROR_INSUFFICIENT_BUFFER:
				return(KErrNoMemory);
			case ERROR_ACCESS_DENIED:
				return(KErrAccessDenied);
			case ERROR_FILE_NOT_FOUND:		//	Reflects value returned by
				return(KErrNotSupported);	//	corresponding MARM Pdd  
			case ERROR_NOT_SUPPORTED:
				return(KErrNotSupported);
			default:
				return(KErrGeneral);
		}
	}
	
	if (bPortReady)
	{
		bPortReady = SetupComm(hCommPort, 256, 256); // set buffer sizes
		if (!bPortReady)
		{
			return(KErrNoMemory);
		}
	}
	
    
    // After you open a serial port, you must set the many flags required to configure
    // the device. These flags are held in a device control block (DCB) structure.
    // You can either fill in the entire DCB structure or use one of the helper functions
    // to fill in some of the details. The GetCommState() function fills in a DCB structure
    // with the current settings from the hardware, and you can use a corresponding
    // SetCommState() function to specify the new settings from a DCB structure.
	//
	if (bPortReady)
	{
		bPortReady = GetCommState(hCommPort, &dcb);
		if (!bPortReady)
		{
			return(KErrGeneral);
		}
	}
	
	if (bPortReady)
	{
		//baud rate
		switch (aConfig.iRate)
			{
		case EBps9600:
			dcb.BaudRate=9600;
			break;
		case EBps38400:
			dcb.BaudRate=38400;
			break;
		case EBps115200:
			dcb.BaudRate=115200;
			break;
		default:
			dcb.BaudRate=9600;
			break;
			}
			
	   	//data bits
	   	switch(aConfig.iDataBits)
			{
		case EData7:
			dcb.ByteSize=7;
			break;
		case EData8:
			dcb.ByteSize=8;
			break;
		default:
			dcb.ByteSize=8;
			break;
			}

		//stop bits
		switch(aConfig.iStopBits)
			{
		case EStop1:
			dcb.StopBits=ONESTOPBIT;
			break;
		case EStop2:
			dcb.StopBits=TWOSTOPBITS;
			break;
		default:
			dcb.StopBits = ONESTOPBIT;
			break;	
			}
		
		dcb.Parity = NOPARITY;

		bPortReady = SetCommState(hCommPort, &dcb);
		if (!bPortReady)
		{
			return(KErrGeneral);
		}
	}

	if (bPortReady)
	{		
		// You can find the current timeout settings using the GetCommTimeout()
		// function, which fills a passed COMMTIMEOUTS structure.	
		//
		bPortReady = GetCommTimeouts (hCommPort, &commTimeouts);
		if (bPortReady)
		{
			// You can set ReadIntervalTimeout to MAXDWORD and set the
			// ReadTotalTimeoutMultiplier and ReadTotalTimeoutConstant members
			// to zero to indicate that the ReadFile() should return immediately.
			//
		    commTimeouts.ReadIntervalTimeout = MAXDWORD;
		    commTimeouts.ReadTotalTimeoutConstant = 0;
		    commTimeouts.ReadTotalTimeoutMultiplier = 0;
		}
		else
		{
			return(KErrGeneral);
		}
		if(!SetCommTimeouts(hCommPort, &commTimeouts))
			return(KErrGeneral);
		
	}
	return KErrNone;
	}

// After configuring the serial port, you can start transferring data via ReadFile()
// and WriteFile() functions. However, you should remember that if you haven't specified
// the FILE_FLAG_OVERLAPPED flag in the CreateFile() flags parameter,
// ReadFile() will block waiting for input. This probably is good if your program spawns
// another thread that specifically waits for incoming serial port characters, but not
// if you want to issue a ReadFile() and periodically check to see whether any characters
// have arrived.
// 

/**
 Transmit data over uart for emulator targte, i.e serial port for pc
 Transmit data buffer is filled and passed by user to the driver
 
 @param	aData
 			buffer for transmit data
 
 @return	KErrNone or standard error code
 */
TInt DExUartPhysicalChannelEmul::TransmitData(const TDesC8& aData)
	{
	TInt r;
	TInt count;
	TInt size;
	TInt offset=0;
		
	// if the device is not configured, try to configure the device again
	if (iConfigured==EFalse)
		{
		// Configure the channel by default
    	TCommConfigV01 cfg;
    	cfg.iRate = EBps9600; // 9600 baudrate
    	r = Configure(cfg);
    	if (r!=KErrNone)
    		{
    		// configure failed again, abort the request
    		iConfigured=EFalse;
    		return KErrAbort;
    		}
    	// configured successfully, continue with Tx	
    	iConfigured=ETrue;	
		}	
	
	// Size of the data to be transmitted is obtained from the descriptor. 
	// TDesC8::Size() gets the size of the data interms of number of bytes occupied 
	// by the data represented by the descriptor
	//
	count= aData.Size();
	if (count<=0)
		return KErrAbort;
	
	// Loop till all the data sent from LDD is transmitted in blocks of KTxFifoSize
 	while (count>0)
 		{
 		// Each block size can be max KTxFifoSize or less
 		size=(count<KTxFifoSize)?count:KTxFifoSize;
		
		BOOL bWriteRC;
		DWORD iBytesWritten;

		iBytesWritten = 0;
		bWriteRC = WriteFile(hCommPort, aData.Ptr(), size, &iBytesWritten, NULL);

		if (!bWriteRC || iBytesWritten == 0)
			{
			return(KErrGeneral);
			}
		
 		// calculate the offset
		offset+=iBytesWritten;			
		// calculate the remaining buffer size
		count-=iBytesWritten;			
		}
		
	return KErrNone;
	}

/**
 Receive data over uart for emulator target, i.e uart of pc
 Receive data buffer is filled and passed by driver to the user
 
 @param	aData
 			buffer for received data
 
 @return	KErrNone or standard error code
 */
TInt DExUartPhysicalChannelEmul::ReceiveData(TDes8& aData,TInt aLen)
	{	
	TInt ret;
	TInt size;
	TInt count;
	TInt bytesRcvd=0;
	
	// Size of the data to be transmitted is obtained from the descriptor. 
	// TDesC8::Size() gets the size of the data interms of number of bytes occupied 
	// by the data represented by the descriptor
	//
	size= aLen;
	if (size<=0)
		return KErrAbort; // Zero length request, exit and fail
	
	// Keep track of the requested size
	count=size;	

 	// Loop till the requested amount of data from LDD is filled or a timeout
  	while (count>0)
 		{ 		
 		BOOL bReadRC;
		char sBuffer[256];
	
		memset(sBuffer,0,sizeof(sBuffer));
 		// if no data and timer expired
 		while ((bReadRC=ReadFile(hCommPort, sBuffer, sizeof(sBuffer), &iBytesRead, NULL)) == 0
 				|| (iBytesRead==0)) 		
 			{
 			if (iTimerStatus==KTimerExpired)
 				{
 				aData.SetLength(bytesRcvd);
 				return KErrTimedOut;
 				}
 			}
 				 			
 		if (iTimerStatus==KTimerStarted)
			{
			// Cancel the Rx poll timer. NTimer.Cancel() Cancel a nanokernel 
			// timer. Removes this timer from the nanokernel timer queue. 
			// Does nothing if the timer is inactive or has already expired. 
			// If the timer was queued and DFC callback requested it is possible 
			// for the expiry handler to run even after Cancel() has been called.
			//
			iRxPollTimer.Cancel();
			// Update status as Cancelled
			iTimerStatus=KTimerCancelled;
			}
			
		if (bReadRC && (iBytesRead>0))
			{
			if (iBytesRead>=static_cast<TUint>(size))			
				iBytesRead=size;
			aData.Append(TPtrC8((const TText8*)sBuffer,iBytesRead));		
			bytesRcvd+=iBytesRead;
			iBytesRead=0;
			}
		else
			{
			iTimerStatus = KTimerStarted;			 			
			// Start a nanokernel timer in one-shot mode with ISR callback
			// Queues the timer to expire in the specified number of nanokernel
			// ticks. The actual wait time will be at least that much and 
			// may be up to one tick more. The expiry handler will be called in
			// ISR context.
			//		 			
    		ret=iRxPollTimer.OneShot(KRxPollTimeout); 			
			if(ret!=KErrNone) // timer creation failed
				return ret;
			}	

		// If we have received the requested number of bytes, return
		if (bytesRcvd>=size)
			{					
			if (iTimerStatus == KTimerStarted)
				iRxPollTimer.Cancel();			
			return KErrNone;
			}
						
		// remaining bytes to be received	
		count -= bytesRcvd;		 		 		
		}		

	return KErrNone;
	}

void DExUartPhysicalChannelEmul::RxPollTimerCallback(TAny* aPtr)
    {
    KEXDEBUG(Kern::Printf("EMUL UART::Rx Timer Expired, Data Flow Stopped"));
    ((DExUartPhysicalChannelEmul*)aPtr)->iTimerStatus=KTimerExpired;        
    }
	
//
// End of d_expio_emul.cpp
