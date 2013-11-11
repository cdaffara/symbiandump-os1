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



#ifndef CS_PORT_H
#define CS_PORT_H

#include <c32comm.h>

/** 
@file
defines the classes CPort and CSerial. This file should be included by the CSY

@publishedAll	
@released
*/

/** Delta timer interval */
const TInt KDeltaTimerInterval = 100000;     
/** The UID for the CSY */          
const TInt KUidUnicodeCommServerModuleV02 = 0x10005054;

/**
Defines the internal port access mode.

@publishedAll	
@released
*/
// Important note!: must have the same order as TCommAccess in c32comm.h
enum TInternalCommAccess
	{
    /**  Exclusive mode. Must be first */
	EIntCommExclusive          = 0, 
	/**  Shared mode. Must be second */
	EIntCommShared             = 1, 
	/**  Preemptable mode. Must be third */
	EIntCommPreemptable        = 2, 
	/**  Wait until the port is closed */
	EIntCommWaitUntilAvailable = 3  
	};

class CPortManager;



class CommTimer 
/**
Static utility class for global timer handling designed for use by Serial Server plug-ins (CSYs).

@publishedAll	
@released
*/
	{
public:
	IMPORT_C static void Queue(TTimeIntervalMicroSeconds32 aTimeInMicroSeconds, TDeltaTimerEntry& aHandle);
	IMPORT_C static void Remove(TDeltaTimerEntry& aHandle);
	static CDeltaTimer* GetTimer();
	};

class CCommSession;
class CSerial;




class CPort : public CObject
/** Base class for implementations of serial protocol module ports. Its functions 
are called by the comms server in response to client calls to RComm. 

The class implements CObject to obtain reference counting behaviour. The reference 
count is incremented when a port is first created, and, for ports in shared 
mode, on every later opening.

CPort is an abstract class which defines many pure virtual functions which 
derived classes must implement. Most of these pure virtual functions correspond 
to the client interface of the RComm class.

Created when the first client does an Open request passing its name.
Destroyed only when it deletes itself (following a call to Destruct).

@publishedAll	
@released */
	{
	// Warning:  No CPort::ConstructL, so can't use iExtra members until after construction.
private:
	class CExtra : public CBase
		{
	public:
		CCommSession* iBreakOwner;
		CCommSession* iPreemptableOwner;
		CCommSession* iPreemptedSession;
		CCommSession* iWaitAvailableOwner;  // Also the SetAccess owner.
		RMessagePtr2   iBlockedSetAccess; 
		};

friend class CPortManager;

public:
	typedef TCommAccess TAccess;

	void CommRead(const RMessage2& aMessage,CCommSession* aClient); // Start read operation
	void CommReadCancel(TInt aHandle, CCommSession* aClient);
	void CommWrite(const RMessage2& aMessage,CCommSession* aClient); // Start Write opneration
	void CommWriteCancel(TInt aHandle, CCommSession* aClient);
	void CommBreak(const RMessage2& aMessage,CCommSession* aClient);	// Start Break operation
	void CommBreakCancel(TInt aHandle, CCommSession* aClient);
	void CommCancel(TInt aHandle, CCommSession* aClient); // Cancel all blocked operation.

	void CommConfig(const RMessage2& aMessage, CCommSession& aSession) const;
	void CommSetConfig(const RMessage2& aMessage, CCommSession& aSession);
	void CommSetServerConfig(const RMessage2& aMessage, CCommSession& aSession);
	void CommGetServerConfig(const RMessage2& aMessage, CCommSession& aSession);
	void CommCaps(const RMessage2& aMessage, CCommSession& aSession);
	void CommSignals(const RMessage2& aMessage, CCommSession& aSession);
	void CommSetSignalsToMark(const RMessage2& aMessage, CCommSession& aSession);
	void CommSetSignalsToSpace(const RMessage2& aMessage, CCommSession& aSession);
	void CommReceiveBufferLength(const RMessage2& aMessage, CCommSession& aSession) const;
	void CommSetReceiveBufferLength(const RMessage2& aMessage, CCommSession& aSession);
	void CommQueryReceiveBuffer(const RMessage2& aMessage, CCommSession& aSession) const;
	void CommResetBuffers(const RMessage2& aMessage, CCommSession& aSession);
	void CommSetAccess(const RMessage2& aMessage, CCommSession& aSession);
	TBool IsBlockedSetAccessWaiting(CCommSession& aClient);
	void CommSetAccessCancel(TInt aHandle, CCommSession* aClient);
	void CommDebugState(const RMessage2& aMessage, CCommSession& aSession);

	TBool TakeOwnershipForReading(const RMessage2& aMessage,CCommSession* aClient);		// Check if a read request is valid and take ownership of port
	TBool TakeOwnershipForWriting(const RMessage2& aMessage,CCommSession* aClient);		// Check if a Write request is valid and take ownership of port
	TBool TakeOwnershipForBreaking(const RMessage2& aMessage,CCommSession* aClient);		// Check if a Break request is valid and take ownership of port

	void InitL(TDesC8 &aName);	// Not used
	static TInt WriteTimerExpiredHandler(TAny* aPtr);
	static TInt ReadTimerExpiredHandler(TAny* aPtr);
	TBool AreAnyPending();

	void CommGetRole(const RMessage2& aMessage, CCommSession* aClient);
	void CommGetFlowControlStatus(const RMessage2& aMessage, CCommSession* aClient);

	void CommNotifySignalChange(const RMessage2& aMessage, CCommSession* aClient);
	TBool TakeOwnershipForSignals(const RMessage2& aMessage,CCommSession* aClient);
	void CommNotifyFlowControlChange(const RMessage2& aMessage, CCommSession* aClient);
	TBool TakeOwnershipForFlowControl(const RMessage2& aMessage,CCommSession* aClient);
	void CommNotifyConfigChange(const RMessage2& aMessage, CCommSession* aClient);
	TBool TakeOwnershipForConfig(const RMessage2& aMessage,CCommSession* aClient);
	void CommNotifyBreak(const RMessage2& aMessage, CCommSession* aClient);
	TBool TakeOwnershipForBreak(const RMessage2& aMessage, CCommSession* aClient);
	void CommNotifyDataAvailable(const RMessage2& aMessage, CCommSession* aClient);
	TBool TakeOwnershipForNotifyDataAvailable(const RMessage2 &aMessage,CCommSession* aClient);
	void CommNotifyOutputEmpty(const RMessage2 &aMessage, CCommSession* aClient);
	TBool TakeOwnershipForNotifyOutputEmpty(const RMessage2 &aMessage,CCommSession* aClient);

	void CommNotifySignalChangeCancel(TInt aHandle, CCommSession* aClient);
	void CommNotifyConfigChangeCancel(TInt aHandle, CCommSession* aClient);
	void CommNotifyFlowControlChangeCancel(TInt aHandle, CCommSession* aClient);
	void CommNotifyBreakCancel(TInt aHandle, CCommSession* aClient);
	void CommNotifyDataAvailableCancel(TInt aHandle, CCommSession* aClient);
	void CommNotifyOutputEmptyCancel(TInt aHandle, CCommSession* aClient);

	TBool SessionHasBeenPreempted(CCommSession* aSession);
	TBool SessionIsAwaitingOpen(CCommSession* aSession);
	void FreeSession(CCommSession* aSession);

public:
	IMPORT_C TInt IPCRead(const TAny* aPtr, TDes8& aDes, TInt aOffset=0) const;
	IMPORT_C TInt IPCWrite(const TAny* aPtr, const TDesC8& aDes, TInt aOffset=0) const;
	IMPORT_C CPort();
	IMPORT_C void ReadCompleted(TInt anError);		// Called by a CPort to complete a read.
	IMPORT_C void WriteCompleted(TInt anError);		// Called by a CPort to complete a write
	IMPORT_C void BreakCompleted(TInt anError);		// Called by a CPort to complete a break
	IMPORT_C virtual ~CPort();
	IMPORT_C void Close();
	IMPORT_C void SignalChangeCompleted(const TUint& aSignals, TInt anError);					// Called by a CPort to complete a signal notify
	IMPORT_C void ConfigChangeCompleted(const TDesC8& aNewConfig, TInt anError);				// Called by a CPort to complete a config notify.
	IMPORT_C void FlowControlChangeCompleted(const TFlowControl& aFlowControl, TInt anError);	// Called by a CPort to complete a flow control notify
	IMPORT_C void BreakNotifyCompleted(TInt anError);			// Called by a CPort to complete a break signal notify
	IMPORT_C void NotifyDataAvailableCompleted(TInt anError);	// Called by a CPort to complete a break signal notify
	IMPORT_C void NotifyOutputEmptyCompleted(TInt anError);		// Called by a CPort to complete a break signal notify

public:
	//
	// Pure virtual methods - to be implemented by the CSY
	//

	// Called by manager when access count is 0 - CSY port must call 'delete this'
	/** Specifies the protocol for port destruction. It is called by the comms server 
	when the last client-side reference to a CPort object has been closed and 
	the CPort must be deleted. The comms server will not delete a CPort other 
	than by calling Destruct().
	
	The implementation should perform any asynchronous shutdown operations on 
	its own resources and, when these operations have completed, should delete 
	this. */
	virtual void Destruct()=0;
	// Queue a read - called by CPort when client wants to read
	// Note: if the value in aLength is negative, this means
	//       ReadOneOrMore and the CSY must invert the number
	/** Specifies the protocol for reading from the port. It is called by the comms 
	server in response to a RComm::Read() or RComm::ReadOneOrMore() request from 
	the client.
	
	A negative value for aLength is used to flag that the read request was from 
	RComm::ReadOneOrMore() rather than from RComm::Read(). The maximum length 
	of data to be read is the absolute value of aLength.
	
	The implementation should use IPCWrite() to write the data to the client's 
	buffer. When all the data has been read, the function should call ReadCompleted(). 
	
	
	@param aClientBuffer Pointer into client address space to the descriptor containing 
	the client's buffer 
	@param aLength The amount of data to be read */
	virtual void StartRead(const TAny* aClientBuffer, TInt aLength)=0;
	// Cancel a pending read
	/** Specifies the protocol for cancelling reading from the port. It is called by 
	the comms server in response to a RComm::ReadCancel() request from the client or
	when the iReadTimer timer expires.
	
	The implementation should abort any processing which was taking place as a 
	result of the read request. Do not call ReadCompleted(). */
	virtual void ReadCancel()=0;
	// Get the size of the receive buffer from the real serial port
	/** Specifies a protocol for requesting the number of bytes that are currently 
	waiting in the port's receive buffer. It is called by the comms server in 
	response to a RComm::QueryReceiveBuffer() request from the client.
	
	@param aLength On return, the number of bytes currently waiting to be read 
	from the receive buffer. 
	@return A system wide error code. */
	virtual TInt QueryReceiveBuffer(TInt& aLength) const=0;
	// reset Tx and Rx buffers
	/** Specifies a protocol for resetting the receive and/or transmit buffers to zero 
	length. It is called by the comms server in response to a RComm::ResetBuffers() 
	request from the client.
	
	@param aFlags Bitmask of the following flags: KCommResetRx to reset the receive 
	buffer; KCommResetTx to reset the transmit buffer */
	virtual void ResetBuffers(TUint aFlags)=0;
	// Queue a write - called by CPort when client wants to write
	/** Specifies the protocol for writing to the port. It is called by the comms server 
	in response to a RComm::Write() request from the client.
	
	The implementation should use IPCRead() to get the data to write from the 
	client's buffer. When all the data has been written, the function should call 
	WriteCompleted(). 
	
	@param aClientBuffer Pointer into client address space to the descriptor containing 
	the client's buffer 
	@param aLength The amount of data to be written */
	virtual void StartWrite(const TAny* aClientBuffer, TInt aLength)=0;
	// Cancel a pending write
	/** Specifies the protocol for cancelling writing to the port. It is called by 
	the comms server in response to a RComm::WriteCancel() request from the client.
	
	The implementation should abort any processing which was taking place as a 
	result of the write request. Do not call WriteCompleted(). */
	virtual void WriteCancel()=0;
	// Queue a break
	/** Specifies the protocol for setting a break condition at the port. It is called 
	by the comms server in response to a RComm::Break() request from the client.
	
	When the break is complete, the function should call BreakCompleted(). 
	
	@param aTime Time period to break for in microseconds */
	virtual void Break(TInt aTime)=0;
	// Cancel a pending break
	/** Specifies the protocol for cancelling a break request. It is called by the 
	comms server in response to a RComm::BreakCancel() request from the client.
	
	The implementation should abort any processing which was taking place as a 
	result of the break request. Do not call BreakCompleted(). */
	virtual void BreakCancel()=0;
	// Pass a config request - return in descriptor
	/** Specifies a protocol for getting the current configuration of the serial port. 
	It is called by the comms server in response to a RComm::GetConfig() request 
	from the client.
	
	@param aPackage A packaged TCommConfig buffer, set on return to the current 
	configuration of the serial port 
	@return A system wide error code */
	virtual TInt GetConfig(TDes8& aPackage) const=0;
	// Set config with package in the descriptor
	/** Specifies a protocol for setting the configuration of the port. It is called 
	by the comms server in response to a RComm::SetConfig() request from the client.
	
	@param aPackage A packaged TCommConfig buffer holding the new configuration 
	values 
	@return A system error code */
	virtual TInt SetConfig(const TDesC8& aPackage)=0;
	// Set the port to use partial reads/writes
	/** Specifies a protocol for setting the buffer mode. It is called by the comms 
	server in response to a RComm::SetMode() request from the client.
	
	@param aPackage A TCommServerConfig package buffer holding the mode settings 
	@return A system-wide error code */
	virtual TInt SetServerConfig(const TDesC8& aPackage)=0;
	// Get the server configs from the CSY
	/** Specifies a protocol for getting the buffer mode. It is called by the comms 
	server in response to a RComm::Mode() request from the client.
	
	@param aPackage A TCommServerConfig package buffer that, on return, holds 
	the current buffer mode settings 
	@return A system error code */
	virtual TInt GetServerConfig(TDes8& aPackage)=0;
	// Read capabilities from the driver
	/** Specifies a protocol for getting the port capabilities. It is called by the 
	comms server in response to a RComm::Caps() request from the client.
	
	@param aPackage A TCommCaps package buffer that, on return, holds the port 
	capabilities 
	@return A system error code */
	virtual TInt GetCaps(TDes8& aPackage)=0;
	// Get the status of the signal pins
	/** Specifies a protocol for getting the status of the serial port control lines. 
	It is called by the comms server in response to a RComm::GetSignals() request 
	from the client.
	
	@param aSignals An integer with the bits set to reflect the status of the 
	handshaking lines. 
	@return A system error code */
	virtual TInt GetSignals(TUint& aSignals)=0;
	// Set selected signals to high (logical 1)
	/** Specifies a protocol for setting serial port control lines. It is called by 
	the comms server in response to a RComm::SetSignals() request from the client.
	
	@param aSignals A bitmask of the handshaking lines to set 
	@return A system error code */
	virtual TInt SetSignalsToMark(TUint aSignals)=0;
	// Set selected signals to low (logical 0)
	/** Specifies a protocol for clearing serial port control lines. It is called by 
	the comms server in response to a RComm::SetSignals() request from the client.
	
	@param aSignals A bitmask of the handshaking lines to clear 
	@return A system error code */
	virtual TInt SetSignalsToSpace(TUint aSignals)=0;
	// Get size of Tx and Rx buffer
	/** Specifies a protocol for requesting the size of the serial port buffers. It 
	is called by the comms server in response to a RComm::ReceiveBufferLength() 
	request from the client.
	
	@param aLength The current size of the serial port buffers in bytes 
	@return A system error code */
	virtual TInt GetReceiveBufferLength(TInt& aLength) const=0;
	// Set size of Tx and Rx buffer
	/** Specifies a protocol for setting the size of the serial port buffers. It is 
	called by the comms server in response to a RComm::SetReceiveBufferLength() 
	request from the client.
	
	@param aLength Requested size of the serial port buffers in bytes 
	@return A system error code */
	virtual TInt SetReceiveBufferLength(TInt aLength)=0;
	virtual void FreeMemory();	// csys have their own implementation, e.g. ECUART
	// Notify client when the signals change
	/** Specifies the protocol for setting a "signal change" notification. It is called 
	by the comms server in response to a RComm::NotifySignalChange() request from 
	the client.
	
	@param aSignalMask Signal mask passed by client */
	virtual void NotifySignalChange(TUint aSignalMask)=0;
	// Cancel an outstanding signal change notification
	/** Specifies the protocol for cancelling a "signal change" notification. It is called 
	by the comms server in response to a RComm::NotifySignalChangeCancel() request 
	from the client. */
	virtual void NotifySignalChangeCancel()=0;
	// Notify client when the configation changes
	/** Specifies the protocol for setting a "configuration change" notification. It 
	is called by the comms server in response to a RComm::NotifyConfigChange() 
	request from the client. */
	virtual void NotifyConfigChange()=0;
	// Cancel an outstanding config change notification
	/** Specifies the protocol for cancelling a "configuration change" notification. 
	It is called by the comms server in response to a RComm::NotifyConfigChangeCancel() 
	request from the client. */
	virtual void NotifyConfigChangeCancel()=0;
	// Notify client when the flow control changes
	/** Specifies the protocol for setting a "flow control change" notification. It is 
	called by the comms server in response to a RComm::NotifyFlowControlChange() 
	request from the client. */
	virtual void NotifyFlowControlChange()=0;
	// Cancel an outstanding flow control change notification
	/** Specifies the protocol for cancelling a "flow control change" notification. It 
	is called by the comms server in response to a RComm::NotifyFlowControlChangeCancel() 
	request from the client. */
	virtual void NotifyFlowControlChangeCancel()=0;
	// Notify client when a break occurs
	/** Specifies the protocol for setting a "break" notification. It is called by the 
	comms server in response to a RComm::NotifyBreak() request from the client. */
	virtual void NotifyBreak()=0;
	// Cancel an outstanding break notification
	/** Specifies the protocol for cancelling a "break" notification. It is called by 
	the comms server in response to a RComm::NotifyBreakCancel() request from 
	the client. */
	virtual void NotifyBreakCancel()=0;
	// Notify client when data is available
	/** Specifies the protocol for setting a "data available" notification. It is called 
	
	by the comms server in response to a RComm::NotifyDataAvailable() request 
	from the client. */
	virtual void NotifyDataAvailable()=0;
	// Cancel an outstanding data available notification
	/** Specifies the protocol for cancelling a "data available" notification. It is 
	called by the comms server in response to a RComm::NotifyDataAvailableCancel() 
	request from the client. */
	virtual void NotifyDataAvailableCancel()=0;
	// Notify client when output buffer is empty
	/** Specifies the protocol for setting a transmit buffer empty notification. It 
	is called by the comms server in response to a RComm::NotifyOutputEmpty() 
	request from the client. */
	virtual void NotifyOutputEmpty()=0;
	// Cancel an outstanding output empty notification
	/** Specifies the protocol for cancelling a transmit buffer empty notification. 
	It is called by the comms server in response to a RComm::NotifyOutputEmptyCancel() 
	request from the client. */
	virtual void NotifyOutputEmptyCancel()=0;
	// Get the flow control status
	/** Gets flow control status. It is called by the comms server in response to a 
	RComm::SetMode() request from the client.
	
	@param aFlowControl Flow control status to return to the client 
	@return A system-wide error code */
	virtual TInt GetFlowControlStatus(TFlowControl& aFlowControl)=0;
	// Get the role of this port unit
	/** Gets DCE/DTE role. It is called by the comms server in response to a RComm::GetRole() 
	request from the client.
	
	@param aRole On return, DCE/DTE role to return to the client 
	@return A system-wide error code */
	virtual TInt GetRole(TCommRole& aRole)=0;
	// Set the role of this port unit
	/** Sets DCE/DTE role. It is called by the comms server in response to a RComm::Open() 
	request from the client.
	
	@param aRole DCE/DTE role 
	@return A system-wide error code */
	virtual TInt SetRole(TCommRole aRole)=0;

// Accessors
#ifdef _DEBUG_DEVCOMM
	virtual void DoDumpDebugInfo(const RMessage2 &aMessage)=0;
#endif
private:
	void DoOpenL(CCommSession* aSession, TInternalCommAccess aMode, TCommRole aRole,TBool aIsNew);
	void DoPreemption();

	TDeltaTimerEntry iReadTimer;  //< delta timer entry for read timeouts
	TBool iReadTimerPending;      //< true if a read timer is pending
	TDeltaTimerEntry iWriteTimer; //< delta timer entry for write timeouts
	TBool iWriteTimerPending;     //< true if a write timer is pending
	TInternalCommAccess iMode;    //< access mode for this port
	CCommSession* iReadOwner;     //< pointer to the read session
	TInt iReadOwnerHandle;        //< handle to the read session
	CCommSession* iWriteOwner;    //< pointer to the write session
	TInt iWriteOwnerHandle;       //< handle to the write session
	// Binary compatible
	// Was: CCommSession* iBreakOwner;  Replace with:
	CExtra* iExtra;               //< pointer to the CExtra object for pre-emptable handling
#define iBreakOwner (iExtra->iBreakOwner) //< forwards iBreakOwner to iExtra->iBreakOwner

	TInt iBreakOwnerHandle;                  //< handle to the break owner
	CCommSession* iSignalOwner;              //< pointer to the signal session
	TInt iSignalOwnerHandle;                 //< handle to the signal session
	CCommSession* iFlowControlOwner;         //< pointer to the flow control session
	TInt iFlowControlOwnerHandle;            //< handle to the flow control session
	CCommSession* iConfigOwner;              //< pointer to the config session
	TInt iConfigOwnerHandle;                 //< handle to the config session
	CCommSession* iBreakNotifyOwner;         //< pointer to the break notify session
	TInt iBreakNotifyOwnerHandle;            //< handle to the break notify session
	CCommSession* iNotifyDataAvailableOwner; //< pointer to the data available session
	TInt iNotifyDataAvailableOwnerHandle;    //< handle to the data available session
	CCommSession* iNotifyOutputEmptyOwner;   //< pointer to the output empty session
	TInt iNotifyOutputEmptyOwnerHandle;      //< handle to the output empty session

	RMessagePtr2 iBlockedRead;                //< pointer to the read request message
	RMessagePtr2 iBlockedWrite;               //< pointer to the write request message
	RMessagePtr2 iBlockedBreak;               //< pointer to the break request message

	/**The encapsulated message*/
	RMessagePtr2 iBlockedSignalChange;			//< holds the notify signal change message
	/**Private padding to preserve BC with earlier versions*/
	TInt iBlockedSignalChangeDummyPadding[7];

	/**The encapsulated message*/
	RMessagePtr2 iBlockedConfigChange;			//< holds the notify config change message
	/**Private padding to preserve BC with earlier versions*/
	TInt iBlockedConfigChangeDummyPadding[7];

	/**The encapsulated message*/
	RMessagePtr2 iBlockedFlowControlChange;		//< holds the notify flow control change message
	/**Private padding to preserve BC with earlier versions*/
	TInt iBlockedFlowControlChangeDummyPadding[7];

	RMessagePtr2 iBlockedBreakNotify;         //< pointer to the notify break request message
	RMessagePtr2 iBlockedNotifyDataAvailable; //< pointer to the notify data available request message
	RMessagePtr2 iBlockedNotifyOutputEmpty;   //< pointer to the notify output empty request message

	CPortManager* iPortManager;              //< Not Used. Not to be exposed to deriving classes of CPort.
	IMPORT_C virtual void CPort_Reserved1();
	TAny* iCPort_Reserved;                   //< reserved pointer
	};


//
// forward declaration
//
class CLibUnloader;


class CSerial : public CObject
/** 
Factory for a single CPort object.

CSerial is the base class for implementations of serial port factories. It is intended to be
overridden by the CSY so that C32 can call in and ask the CSY to create serial ports. In this way
the factory is responsible for creating instances of CPort-derived classes 
for the protocol of the CSY. It also provides query functions, which return general information 
about the capabilities of the serial protocol implemented by the CSY.

Owned by the CPort object it creates.

Writing derived classes:

The pure virtual NewPortL() and Info() functions must be implemented in derived 
classes. Serial protocol modules which can take differing action based on 
the version of the comms server should also override QueryVersionSupported(). 

@publishedAll
@released
*/
	{
public:
	IMPORT_C CSerial();
	IMPORT_C ~CSerial();
	IMPORT_C virtual TBool QueryVersionSupported(const TVersion& aVersion) const;
	void ConstructL(RLibrary& aLib);
public:
	//
	// pure virtuals to be implemented by the CSY
	//

	// Create a new port for the supplied unit number
	/** Specifies the protocol for creating a new serial port for the protocol. The 
	comms server calls this function in response to a RComm:Open() call.
	
	Typically, the implementation would call NewL() on the protocol's CPort-based 
	class. Any resources required by the new port object should be done at this 
	stage. If the serial port object cannot be created for any reason, NewPortL() 
	should leave with an appropriate error code.
	
	The interpretation of unit numbers is specific to the particular serial protocol 
	module. However, unit numbers on Symbian OS should be zero-based. For the 
	default built-in serial ports implemented by the serial protocol module ecuart.csy, 
	the unit numbers refer to the hardware ports, with 0 being the first port 
	on the machine. If a second request is made to create a port with a unit number 
	that has already been created and not destroyed, NewPortL() should leave with 
	KErrAlreadyExists.
	
	@param aUnit The unit number to be created. */
	virtual CPort* NewPortL(const TUint aUnit)=0;
	// Get info about this CSY, fill in the supplied structure.
	/** Specifies the protocol for getting the information about the serial protocol.
	
	Implementations should fill in the TSerialInfo structure to reflect the protocol's 
	capabilities.
	
	@param aSerialInfo On return, set to indicate the capabilities of the serial 
	protocol. */
	virtual void Info(TSerialInfo &aSerialInfo)=0;

	void ModuleName(TDes& aName);

	IMPORT_C virtual TSecurityPolicy PortPlatSecCapability(TUint aPort) const;

protected:
	/** Module version number. The class should initialise this member with its version 
	number from its constructor. */
	TVersion iVersion;                         //< holds the version of the CSY
private:
	CLibUnloader* iLibUnloader;                //< pointer to the library unloader
	IMPORT_C virtual void CSerial_Reserved1();
	TAny* iCSerial_Reserved;                   //< reserved pointer for future BC
	};

/** This typedef defines the form of the ordinal-1 entry point function to a serial 
protocol module. The function should create and return a concrete CSerial-derived 
class, which will then be used by the comms server.

Each serial protocol module should only ever create a single serial protocol 
factory object. If the entry point is called twice without the first factory 
object being destroyed, this function should leave with KErrGeneral. */
typedef CSerial *(*TSerialNewL)(); //< function type of CSY module entry point

#endif // CS_PORT_H
