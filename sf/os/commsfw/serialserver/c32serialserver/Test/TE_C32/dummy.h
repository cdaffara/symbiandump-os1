// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef DUMMY_H__
#define DUMMY_H__

/** @file
 *
 * Dummy CSY for testing.
 */

#include <cs_port.h>
#include <comms-infras/commsdebugutility.h>

const TInt KDummyCompleteDelayFast = 600000L;
const TInt KDummyCompleteDelaySlow = 1200000L;

const TInt KDummy12ReadWait=28000000;

class CDummyPort;
class CDummyCompleter;

_LIT8(KC32LoggingName,"C32SerComms");

/*
* Macros are defined in respective mmp files for different csys
*/

#if defined(DUMMYCSY1)
	#define SERIAL_DESCRIPTION _L("DummyCsy1 3 wire raw") //< CSY description
	#define SERIAL_NAME _S("DummyCsy1")                   //< name of prefix for each com port
	_LIT8(KCSYLoggingName,"DummyCsy1");
#elif defined(DUMMYCSY2)
	#define SERIAL_DESCRIPTION _L("DummyCsy2 3 wire raw") 
	#define SERIAL_NAME _S("DummyCsy2")                
	_LIT8(KCSYLoggingName,"DummyCsy2");   
#elif defined(DUMMYCSY3)
	#define SERIAL_DESCRIPTION _L("DummyCsy3 3 wire raw") 
	#define SERIAL_NAME _S("DummyCsy3")                   
	_LIT8(KCSYLoggingName,"DummyCsy3");
#elif defined(DUMMYCSY4)
	#define SERIAL_DESCRIPTION _L("DummyCsy4 3 wire raw") 
	#define SERIAL_NAME _S("DummyCsy4")                   
	_LIT8(KCSYLoggingName,"DummyCsy4");
#elif defined(DUMMYCSY5)
	#define SERIAL_DESCRIPTION _L("DummyCsy5 3 wire raw") 
	#define SERIAL_NAME _S("DummyCsy5")
	_LIT8(KCSYLoggingName,"DummyCsy5");                   
#elif defined(DUMMYCSY6)
	#define SERIAL_DESCRIPTION _L("DummyCsy6 3 wire raw") 
	#define SERIAL_NAME _S("DummyCsy6")                   
	_LIT8(KCSYLoggingName,"DummyCsy6");
#elif defined(DUMMYCSY7)
	#define SERIAL_DESCRIPTION _L("DummyCsy7 3 wire raw") 
	#define SERIAL_NAME _S("DummyCsy7")                   
	_LIT8(KCSYLoggingName,"DummyCsy7");
#elif defined(DUMMYCSY8)
	#define SERIAL_DESCRIPTION _L("DummyCsy8 3 wire raw") 
	#define SERIAL_NAME _S("DummyCsy8")                   
	_LIT8(KCSYLoggingName,"DummyCsy8");
#elif defined(DUMMYCSY9)
	#define SERIAL_DESCRIPTION _L("DummyCsy9 3 wire raw") 
	#define SERIAL_NAME _S("DummyCsy9")                   
	_LIT8(KCSYLoggingName,"DummyCsy9");
#elif defined(DUMMYCSY10)
	#define SERIAL_DESCRIPTION _L("DummyCsy10 3 wire raw") 
	#define SERIAL_NAME _S("DummyCsy10")                   
	_LIT8(KCSYLoggingName,"DummyCsy10");
#elif defined(DUMMYCSY12)
	#define SERIAL_DESCRIPTION _L("DummyCsy12 3 wire raw") 
	#define SERIAL_NAME _S("DummyCsy12")  
	_LIT8(KCSYLoggingName,"DummyCsy12");
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
	                     
#elif defined(DUMMYCSY15)
	#define SERIAL_DESCRIPTION _L("DummyCsy15 3 wire raw") 
	#define SERIAL_NAME _S("DummyCsy15")                   
	_LIT8(KCSYLoggingName,"DummyCsy15");
#elif defined(DUMMYCSY16)
	#define SERIAL_DESCRIPTION _L("DummyCsy16 3 wire raw") 
	#define SERIAL_NAME_SMALL _S("DummyCsy16") 
	#define SERIAL_NAME _S("DummyCsy16LongPortNameeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee")                   
	_LIT8(KCSYLoggingName,"DummyCsy16");
#elif defined(DUMMYCSY17)
	#define SERIAL_DESCRIPTION _L("DummyCsy17 3 wire raw") 
	#define SERIAL_NAME _S("DummyCsy17")                   
	_LIT8(KCSYLoggingName,"DummyCsy17");
#else
	#define SERIAL_DESCRIPTION _L("Dummy 3 wire raw") 	// default c32 dummy csy
	#define SERIAL_NAME _S("Dummy")  
	_LIT8(KCSYLoggingName,"Dummy");                 
#endif

_LIT8(testbufRead, "0123456789abcdefghijklmnopqrstuvxyxABCDEFGHIJKLMNOPQRSTUVXYZ[]\r\n");
_LIT8(testbufWrite, "This string has to be exactly like this othervice write returns KErrCommsLineFail.");

const TInt PORT_0_NOCAPS = 0;
const TInt PORT_1_NETWORKSERVICES = 1;
const TInt PORT_2_NOTSUPPORT = 2;
const TInt PORT_3_ALLCAPS = 3;
const TInt PORT_6_NOCAPS = 6;

/**
unique numbers used for message passing between
client and C32 comms server.
@internalAll
*/
enum TCommMess
{
	ECommLoadCommModule,            //< 0 Load a CSY comms module
	ECommCloseCommModule,           //< 1 Unload a CSY comms module
	ECommPortInfo,
	ECommPortInfoByName,            //< 3 Get information on serial ports by name
	ECommPortInfoByNumber,          //< 4 Get information on serial ports by number
	ECommNumPorts,                  //< 5 Get how many CSY's are loaded
	ECommStartServerThread,         //< 6 Start another thread in the C32 process
	ECommOpen,                      //< 7 Open a comm port
	ECommRead,                      //< 8 Read from the comm port
	ECommReadCancel,                //< 9 Cancel an outstanding read
	ECommQueryReceiveBuffer,        //< 10 Get number of bytes in received buffer

	ECommResetBuffers,              //< 11 Reset the serial port buffers
	ECommWrite,                     //< 12 Write to the comm port
	ECommWriteCancel,               //< 13 Cancel an outstanding write
	ECommBreak,                     //< 14 Send a break
	ECommBreakCancel,               //< 15 Cancel sending of break
	ECommCancel,                    //< 16 Cancel any outstanding reads or writes
	ECommConfig,                    //< 17 Get comm port configuration
	ECommSetConfig,                 //< 18 Set comm port configuration
	ECommCaps,                      //< 19 Get the capabilities of the comm port
	ECommSetMode,                   //< 20 Set server buffering mode

	ECommGetMode,                   //< 21 Get server buffering mode
	ECommSignals,                   //< 22 Read serial port control lines
	ECommSetSignalsToMark,          //< 23 Set control lines high (1)
	ECommSetSignalsToSpace,         //< 24 Set control line low (0)
	ECommReceiveBufferLength,       //< 25 Get the size of the receive buffer
	ECommSetReceiveBufferLength,    //< 26 Set the size of the receive buffer
	ECommClose,                     //< 27 Close the comm port
	ECommDbgMarkHeap,               //< 28 Set a heap mark in the Comms server
	ECommDbgCheckHeap,              //< 29 Check the heap mark in the Comms server
	ECommDbgMarkEnd,                //< 30 Set the heap mark end in the Comms server

	ECommDbgFailNext,               //< 31 Emulate fail next heap allocation in the comm server
	ECommDbgSetDebugPrintMask,      //< 32 Set the debug print mask
	ECommDbgDoDumpDebugInfo,        //< 33 Dump debug info

	// Here starts version 02 extensions
	ECommGetRole,                   //< 34 Get the current role
	ECommNotifySignals,             //< 35 Notify when signals change
	ECommNotifySignalsCancel,       //< 36 Cancel an outstanding signals notify
	ECommNotifyFlowControl,         //< 37 Notify when flow control changes
	ECommNotifyFlowControlCancel,   //< 38 Cancel an outstanding flow control notify
	ECommGetFlowControl,            //< 39 Get the current status of flow control
	ECommNotifyConfigChange,        //< 40 Notify when config change

	ECommNotifyConfigChangeCancel,  //< 41 Cancel an outstanding config notify
	ECommNotifyBreak,               //< 42 Send break
	ECommNotifyBreakCancel,	        //< 43 Cancel an outstanding sending of break
	ECommNotifyDataAvailable,       //< 44 Notify when data available in Rx buffer
	ECommNotifyDataAvailableCancel, //< 45 Cancel an outstanding notify data available
	ECommNotifyOutputEmpty,         //< 46 Notify when Tx buffer is empty
	ECommNotifyOutputEmptyCancel,   //< 47 Cancel an outstanding output empty notify
	// Here ends version 02 extensions

	ECommSetAccess,                 //< 48 Set the access mode defined by TCommAccess
	ECommDebugState,                //< 49 Get the debug state
	ECommOpenWhenAvailable          //< 50 Open port when it is available
	};


/**
 * a dummy class for completing asynchronour requests
 */
class CDummyCompleter : public CActive 
    {
	public:
		// construction
		CDummyCompleter(CDummyPort& aPort);
		
		// destruction
		~CDummyCompleter();

		// Completes async call
		void Complete(TCommMess aMess, TTimeIntervalMicroSeconds32 anInterval, TInt aReason, TUint aValue = 0);
		void Complete(TCommMess aMess, TTimeIntervalMicroSeconds32 anInterval, TInt aReason, const TDesC8& aDescriptor);
		void CancelComplete();
				
	private: 
		void DoCancel();				/* stop object activity */
		void RunL();					/* active object callback */
		
	private:
		TCommMess iMess;
		RTimer iTimer;
		CDummyPort& iPort;
		TInt iReason;
		TUint iValue;
		TCommNotificationV01 iPeerConfig;
						
    };


/**
 * "Entry point object" makes the objects which do the work
 */
class CDummyPortFactory : public CSerial
	{
	enum {KMajorVersion=1,KMinorVersion=0,KBuild=92};
public:
	static CDummyPortFactory* NewL();
	virtual CPort * NewPortL(const TUint aUnit);
	virtual void Info(TSerialInfo &aSerialInfo);
#ifdef DUMMYCSY10
	void LoadAnotherCSY();
#endif
#ifdef DUMMYCSY17
	void LoadAnotherCSYNoClose();
#endif

public: //CSerial
	TSecurityPolicy PortPlatSecCapability(TUint aPort) const;
private:
	void ConstructL();
	CDummyPortFactory();
	};


/**
 * CPort is the object which interfaces to the commserver
 */
class CDummyPort : public CPort
	{
public:
	static CDummyPort * NewL(TUint aUint);
public:
	virtual void StartRead(const TAny *aClientBuffer,TInt aLength);
	virtual void ReadCancel();
	virtual TInt QueryReceiveBuffer(TInt& aLength) const;
	virtual void ResetBuffers(TUint aFlags);
	virtual void StartWrite(const TAny *aClientBuffer,TInt aLength);
	virtual void WriteCancel();
	virtual void Break(TInt aTime);
	virtual void BreakCancel();
	virtual TInt GetConfig(TDes8& aPackage) const;
	virtual TInt SetConfig(const TDesC8& aPackage);
	virtual TInt SetServerConfig(const TDesC8& aPackage);
	virtual TInt GetServerConfig(TDes8& aPackage);
	virtual TInt GetCaps(TDes8& aPackage);
	virtual TInt GetSignals(TUint& aSignals);
	virtual TInt SetSignalsToMark(TUint aSignals);
	virtual TInt SetSignalsToSpace(TUint aSignals);
	virtual TInt GetReceiveBufferLength(TInt& aLength) const;
	virtual TInt SetReceiveBufferLength(TInt aLength);
	virtual ~CDummyPort();

	// Version 02 additions to IrCOMM starts from here
	virtual void NotifySignalChange(TUint aSignalMask);
	virtual void NotifySignalChangeCancel();
	virtual void NotifyConfigChange();
	virtual void NotifyConfigChangeCancel();
	virtual void NotifyFlowControlChange();
	virtual void NotifyFlowControlChangeCancel();
	virtual void NotifyBreak();
	virtual void NotifyBreakCancel();
	virtual void NotifyDataAvailable();
	virtual void NotifyDataAvailableCancel();
	virtual void NotifyOutputEmpty();
	virtual void NotifyOutputEmptyCancel();
	virtual TInt GetFlowControlStatus(TFlowControl& aFlowControl);
	virtual TInt GetRole(TCommRole& aRole);
	virtual TInt SetRole(TCommRole aRole);
	// Version 02 additions to IrCOMM ends to here

	void Destruct();

private:
    CDummyPort();
	void ConstructL(TUint aUint);
	
private:

	TUint iBufFlags;   //< buffer flags
	TInt iBufSize;     //< buffer size

	TInt iBreakDelay;  //< time of break delay

	TUint8* iDataIn;   //< pointer to data in buffer
	TPtr8 iPtrDataIn;   //< descriptor pointer to data in buffer

	TUint8* iDataOut;  //< pointer to data out buffer
	TPtr8 iPtrDataOut;  //< descriptor pointer to data out buffer

	TUint8* iCompData; //< pointer to compare data
	TPtr8 iPtrCompData; //< descriptor pointer to compare data

	TAny* iClientPtr;             //< pointer to client buffer
    TInt iClientLength;           //< length of client buffer
	TInt iLength;                 //< length count of client buffer
	TUint iSignals;               //< RS232 signals
    TCommConfigV01 iConfig;       //< port configuration
    TDeltaTimerEntry iBreakTimer; //< break timer entry
	TCommRole iRole;                            //< current role, DTE or DCE
	TUint iNotifySignalMask;                    //< signals notification mask
	TFlowControl iFlowControl;                  //< flow control settings
	TPckg<TCommCapsV02> *iCapsPtr;              //< pointer to capabilities package
	TCommNotificationV01 iPeerConfig;           //< peer configuration
	CDummyCompleter *iCompleteSigPtr;           //< pointer to signal completer
	CDummyCompleter *iCompleteFlowPtr;          //< pointer to flow control completer
	CDummyCompleter *iCompleteConfPtr;          //< pointer to configuration completer
	CDummyCompleter *iCompleteCapsPtr;          //< pointer to capabilities completer
	CDummyCompleter *iCompleteReadPtr;          //< pointer to data read completer
	CDummyCompleter *iCompleteWritePtr;         //< pointer to data write completer
	CDummyCompleter *iCompleteSetBreakPtr;      //< pointer to set break completer
	CDummyCompleter *iCompleteBreakPtr;         //< pointer to break completer
	CDummyCompleter *iCompleteDataAvailablePtr; //< pointer to data available completer
	CDummyCompleter *iCompleteOutputEmptyPtr;   //< pointer to output empty completer
	CDummyCompleter *iCompleteClosePtr;         //< pointer to close completer

	TTimeIntervalMicroSeconds32 iDummyCompleteDelayFast; //< fast complete delay
	TTimeIntervalMicroSeconds32 iDummyCompleteDelaySlow; //< slow complete delay
	};

/**
 * faults in the CSY
 */
enum TCSYFaults
	{
		E_ParametersCorrupted,    //< wrong parameter
		E_UnknownAsynchronousCall //< not used
	};


#endif // DUMMY_H__

