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

#include <e32std.h>
#include <c32comm.h>
#include <e32panic.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <c32comm_internal.h>
#endif

#include "CS_STD.H"
#include "C32LOG.H"

/** @file
 *
 * Implements the client side code of C32, including RCommServ and RComm
 */


const TUint KDefaultMessageSlots = 16;    //< number of slots in the Client/Server message buffer


//
// implementation of RCommServ
//


EXPORT_C RCommServ::RCommServ()
/** Default constructor. 
@capability None
*/
	{
	}


EXPORT_C TInt RCommServ::Connect()
/** Connects a client process to the comms server.

Use this function to open a session to the Comms server.
Default number of message slots is defined in KDefaultMessageSlots.

Notes:

1. RHandleBase 
   provides the necessary Close() function, which should be called when the server 
   session is no longer required. Note that the kernel completes the Close() function immediately
   and so RCommServ resources still used by this client may not yet have been released at the time
   the Close() returns. A consequence of this is that an attempt to open a port previously opened
   for exclusive use in a session that is still closing might fail.
2. Older versions of Symbian OS specified that a call to "StartC32" was required before a call to
   this Connect(). However, C32 Serial server is now started automatically during device boot
   so this is not necessary unless the application is to run in a non-standard GUI environment.


@return System-wide error code 
@capability None
*/
    {
    TInt r = CreateSession(KCommServerName(), Version(), KDefaultMessageSlots);
	if (r==KErrNotFound)
		{
		// attempt to start the comms system. This is normally done on device boot but to support
		// any old test environments we still try here as well. This is a deprecated behaviour that
		// results in warnings in the log file
		r = StartC32();
		if (r==KErrNone)
			r=CreateSession(KCommServerName(), Version(), KDefaultMessageSlots);
		}
    return(r); 
    }


EXPORT_C TVersion RCommServ::Version() const
/** Returns the client side version number.

Use this function to get the version number. The version number may be
incremented in future releases of the comms server. If extra features are
added in such releases, the version number may be used by application programs
as a basis for assessing the capabilities of the comms server. Version-specific
functions will be marked as such in the SDK documentation.

@return Version number. 
@capability None
*/
    {
    return(TVersion(KEC32MajorVersionNumber, KEC32MinorVersionNumber, KEC32BuildVersionNumber));
    }

EXPORT_C TInt RCommServ::LoadCommModule(const TDesC& aFileName)
/** Loads a comms module.

Use this function to load a CSY comms module. These are protocol modules that C32 Serial Server uses to
understand how to handle a particular port.

Notes:

1. There is no need for an extension, as ".CSY" is automatically appended by 
   this function.

2. This function only loads CSYs that have been installed correctly into one of the system's /sys/bin directories
   
3. If the module has already been loaded by another client, this function will increment
   the reference to the module.
   
4. It is not an error to load the same module twice or more, although there is no practical
   application for this ability. On each subsequent load for the same module the reference count
   is simply incremented. When the session is closed, all module references are removed including
   references from repeated loading of the same module.
   
5. There is no limit to the number of modules that can be loaded from within a single session.

With the exception of some hardware configurations, four modules are normally available as standard:

  ECUART is used to address appropriate serial ports in RS232 mode. It is normally customised for a device.

  IRCOMM drives the infrared port in infrared mode.
 
  BTCOMM drives the Bluetooth emulated serial port.
  
  ECACM drives the USB port(s).

@param aFileName Name of the file to load as a module, optionally containing the ".CSY" extension.
The filename may include the full path including drive letter for the CSY.
@return KErrNone if the load operation is successful; KErrServerTerminated if the server no longer present;
 KErrServerBusy if there are no message slots available; KErrNoMemory if there is insufficient memory available;
 KErrBadLibraryEntryPoint if the CSY fails the consistency check; KErrNotFound if the CSY or other component
 or service required to load it is not found; other return codes are possible from the CSY itself.
@capability None
*/
    {
    // No means to insert a 16-bit string into an 8-bit string, so all is 16-bit
	C32_STATIC_LOG2(KC32Client,_L("RCommServ::LoadCommModule() : %S"), &aFileName);

	TIpcArgs args(&aFileName);
	return SendReceive(ECommLoadCommModule, args);
    }


EXPORT_C TInt RCommServ::UnloadCommModule(const TDesC& aName)
/** Remove this client's reference to the named communications module. If this is the
last client to reference the communications module, the module is unloaded.

Note:

There is no need to unload a comms module when the connection to the
server is closed, as this is done automatically by the C32 Server. For example, the following code will not result in a leak:
@code
		_LIT(KCSYName,"ECUART");
		RCommServ commSession;
		if (commSession.Connect() == KErrNone)
			{
			TInt ret = commSession.LoadCommModule(KCSYName);
			commSession.Close();
			}
@endcode

@param aName Name of the module to unload described in Port Prefix format. This may not necessarily be
the same name as that used to load the module.
Examples:
The ECUART module, where supplied, is loaded via "ECUART" or "ECUART.CSY" but unloaded via "COMM".
The infrared module, where supplied, is loaded via "IRCOMM.CSY" or "IRCOMM" but unloaded only via "IRCOMM".
@return System wide error code.

*/
    {
	C32_STATIC_LOG2(KC32Client,_L("RCommServ::UnloadCommModule() : %S"), &aName);
	
	TIpcArgs args(&aName);
	return SendReceive(ECommCloseCommModule, args);
    }


EXPORT_C TInt RCommServ::NumPorts(TInt& aCount)
/** Get the number of unique comms modules (CSYs) loaded into C32.

This number includes modules loaded by other sessions.

@param aCount On return, holds the number of loaded CSYs
@return System wide error code. 
@capability None
*/
    {
	TPckg<TInt> num(aCount);
	TIpcArgs args(&num);
	return SendReceive(ECommNumPorts,args);
    }


EXPORT_C TInt RCommServ::GetPortInfo(const TDesC& aName, TSerialInfo& aInfo)
/** Gets static information about the available serial ports for a particular comms module.

This variant enables the comms module to be specified in Port Prefix format or by filename. 

There is no wildcard support.

Notes:

1. This API only returns static information about the available ports as they were when the module loaded, so
ports listed as available may actually be in use exclusively by another client at the time of this call.

2. The information returned does not account for any security restrictions on ports, and so does not check
whether the client has the capabilities to open all the advertised ports.

@param aName The name of the comms module in Port Prefix format (eg "COMM" for RS232), 
or the filename of the CSY as supplied to LoadCommModule() excluding
 the ".CSY" extension (eg "ECUART" for RS232). The supplied name will be truncated to be KMaxPortName (16) characters long.
@param aInfo On return, contains information about the ports available, including 
the high and low unit numbers and their name in Port Prefix format.
@return KErrNone, if the send operation is successful; 
KErrServerTerminated, if the server is no longer present; 
KErrServerBusy, if there are no message slots available; 
KErrNoMemory, if there is insufficient memory available.
@capability None
*/
    {
	C32_STATIC_LOG2(KC32Client,_L("RCommServ::GetPortInfo() for comms module : %S"), &aName);
	TPckg<TSerialInfo> portDesc(aInfo);
	TIpcArgs args(&portDesc,&aName);
	return SendReceive(ECommPortInfoByName, args);
    }


EXPORT_C TInt RCommServ::GetPortInfo(TInt aIndex, TDes& aModuleName, TSerialInfo& aInfo)
/** Gets static information about the available serial ports for a particular comms module.

This variant enables the comms module to be specified by an index number 
from 0, enabling iteration through the loaded comms modules. Call NumPorts() to obtain
the current number of loaded comms modules.

Notes:

1. This API only returns static information about the available ports as they were when the module loaded, so
ports listed as available may actually be in use exclusively by another client at the time of this call.

2. The information returned does not account for any security restrictions on ports, and so does not check
whether the client has the capabilities to open all the advertised ports.

3. If another C32 client unloads a CSY module, the index of each of 
   the remaining modules may change. If
   a client is attempting to iterate through all the open modules when this occurs, the iteration may miss
   a module or receive KErrNotFound unexpectedly. This behaviour can be detected by 
   observing that the value returned by NumPorts() changes.



@param aIndex Index number of the comms module, starting at 0. 
@param aModuleName On return, contains the name of the comms module referred 
to by aIndex. This name is expressed as the filename without the path or extension, and the case is
not specified since it is determined by the first request to LoadCommModule.

@param aInfo On return, contains information about the ports available, including 
the high and low unit numbers and their name. 
@return KErrNone, if the send operation is successful; 
KErrServerTerminated, if the server is no longer present; 
KErrServerBusy, if there are no message slots available; 
KErrNoMemory, if there is insufficient memory available.
@capability None
*/
    {
	C32_STATIC_LOG2(KC32Client,_L8("RCommServ::GetPortInfo() for comms module of index number : %d"), aIndex);
	TPckg<TSerialInfo> portDesc(aInfo);
	TIpcArgs args(&portDesc,&aModuleName,aIndex);
	return SendReceive(ECommPortInfoByNumber, args);
    }
	

EXPORT_C TInt RCommServ::__DbgMarkHeap()
/** Sets a heap mark in the comm server

Only valid for Debug builds.

@return System wide error code. 
@capability None
*/
    {
#ifdef _DEBUG
	TIpcArgs args(TIpcArgs::ENothing);
	return SendReceive(ECommDbgMarkHeap, args);
#else
    return KErrNone;
#endif
    }


EXPORT_C TInt RCommServ::__DbgCheckHeap(TInt aCount)
/** Checks the heap mark in the comm server

Only valid for Debug builds. 
@capability None
*/
    {
#ifdef _DEBUG
	TIpcArgs args(aCount);
	return SendReceive(ECommDbgCheckHeap, args);
#else
	(void)aCount; // not used for Release builds
    return KErrNone;
#endif
    }


EXPORT_C TInt RCommServ::__DbgMarkEnd(TInt aCount)
/** Sets the heap mark end in the comm server

Only valid for Debug builds. 
@capability None
*/
    {
#ifdef _DEBUG
	TIpcArgs args(aCount);
	return SendReceive(ECommDbgMarkEnd, args);
#else
	(void)aCount; // not used for Release builds
    return KErrNone;
#endif
    }


EXPORT_C TInt RCommServ::__DbgFailNext(TInt aCount)
/** Emulates a single fail next heap allocation in the comm server.
If C32 is running multiple threads the same request is passed to each thread. Each C32 thread
will only act on this request if it has not already been configured for heap failures via CMI files.

@param aCount Specifies that after aCount-1 successful allocations the failure will occur on the next
allocation - ie the failure will occur on the aCount allocation attempt.
 A value less than zero cancels any outstanding failure request. A value of zero is equivalent to a value
 of one - ie, fail on the next attempt.
 
Only valid for Debug builds. 
@capability None
*/
    {
#ifdef _DEBUG
	TIpcArgs args(aCount);
	return SendReceive(ECommDbgFailNext,args);
#else
	(void)aCount; // not used for Release builds
    return KErrNone;
#endif
    }


EXPORT_C TInt RCommServ::__DbgSetTraceMask(TC32Trace aMask)
/** The functionality of this API has been removed.

Where tracing is required, please refer to the Comms Infrastructure documentation
on logging.

@param aMask This value is not used.
@return Always KErrNone.
@capability None
*/
    {
#ifdef _DEBUG
	(void)aMask; // aMask is of no use
	// At one time this function would send ECommDbgSetDebugPrintMask to the server, but
	// the server hasn't supported this function since at least the last release of EPOC5.
	return KErrNone;
#else
	(void)aMask; // not used for Release builds
    return KErrNone;
#endif
    }

EXPORT_C TInt RCommServ::CreateThreadInCommProc(const TDesC&, const TDesC&, TThreadFunction, TInt aStackSize, TInt aHeapMinSize, TInt aHeapMaxSize)
/** This functionality has been withdrawn since the introduction of Platform Security.

This API has been replaced with a more secure and controlled model for loading threads
 into the Comms process, known as the Configurator. The Configurator requires a Comms Provider Module
 (DLL) to be available that provides the code for the thread to execute, and a configuration file
 that describes to the Configurator how to manage the Comms Provider Module.
 Further information on this is only available in documentation libraries that include the Configurator documentation.

@param aLibraryName This value is not used.
@param aThreadName  This value is not used.
@param aFunction    This value is not used.
@param aStackSize   A negative value will result in a panic.
@param aHeapMinSize A value less than KMinHeapSize will result in a panic.
@param aHeapMaxSize A value less than aHeapMinSize will result in a panic.
@return Always KErrNotSupported. 
@capability CommDD (Deprecate)
*/

/*  Please refer to the Rootserver component documentation for more information.
*/

	{
	C32_STATIC_LOG(KC32Warning,_L8("WARNING: Call to deprecated function CreateThreadInCommProc."));	
	
	_LIT(KCommClientPanic,"Comm client");	// panic the client here rather than panicking the C32 server
        									// thread
	__ASSERT_ALWAYS(aStackSize>=0,User::Panic(KCommClientPanic,EThrdStackSizeNegative));
	__ASSERT_ALWAYS(aHeapMinSize>=KMinHeapSize,User::Panic(KCommClientPanic,EThreadHeapMinTooSmall));
	__ASSERT_ALWAYS(aHeapMaxSize>=aHeapMinSize,User::Panic(KCommClientPanic,EThreadHeapMaxLessThanMin));

	return KErrNotSupported;
	}

TBool RCommServ::IsServerThreadL()
/**
 * Return TRUE if this thread is the C32 main thread
 *
 * @note This function is not as water tight as it could be.  This is due to a limitation
 *       in our ability to get the thread name.  Under non-WINS builds the thread name is
 *       (even after we rename it) a combination of process name, OS data and thread name.
 *       Therefore we search for a thread name ending in "CommServer" and belonging to
 *       the C32 process.  If another process started a thread in C32 via the
 *       CreateThreadInCommProc() call with a name ending in "CommServer" then this
 *       function may start to produce incorrect results.
 *
 * @return ETrue if this is the C32 main server thread
 */
	{
	TInt res = KErrNone;
	TFullName name;

	//
	// Find the Process ID of the C32 process.  Note that WINS builds run the C32 'process'
	// as a thread and not a separate process.
	//
#ifndef __WINS__
	_LIT(KCommProcessName, "C32exe*");

	RProcess cpProcess;
	TFindProcess findCp(KCommProcessName);
	res = findCp.Next(name);
	__ASSERT_ALWAYS(res==KErrNone,User::Invariant());
	res = cpProcess.Open(findCp); 
	(void) User::LeaveIfError(res);

	TProcessId cpid = cpProcess.Id();
	cpProcess.Close();

	//
	// A quick short cut.  Is this process C32?  If not then this thread cannot be the
	// server thread.
	//
	if (RProcess().Id() != cpid)
		{
		return (EFalse);
		}
#endif

	//
	// Search for the the C32 comm server thread.  For non-WINS builds get the Process
	// ID of the thread and confirm it matches the C32 Process ID.
	//
	RThread ctThread;
	//thread name comes from C32.cmi file.
	_LIT(KCommThreadName, "CCommServer");
	TFindThread findCt(KCommThreadName);
	while((res=findCt.Next(name))==KErrNone)
		{
		(void)User::LeaveIfError(ctThread.Open(KCommThreadName)); // ignoring return value
#ifdef __WINS__
		break;
#else
		if((res=ctThread.Process(cpProcess))!=KErrNone)
			{
			ctThread.Close();
			User::Leave(res);
			}
		TProcessId pid=cpProcess.Id();
		cpProcess.Close();

		//
		// We already know that this process is the C32 process.  Does the found thread
		// belong to C32?  If so then ctThread is the C32 main thread.
		//
		if(pid==cpid)
			{
			break;
			}
		ctThread.Close();
#endif
		}
	if (res==KErrNotFound)
		return EFalse;	// if the C32 thread is not running, then this thread cannot be it!
	(void)User::LeaveIfError(res); // ignoring return value

	//
	// Is this thread the C32 Comm Server thread that we just found?
	//
	TBool match=(ctThread.Id()==RThread().Id());
	ctThread.Close();
	
	return match;	// Return TRUE if this thread is the C32 main thread
	}

//
// implementation of RComm
//

EXPORT_C RComm::RComm()
/** Constructor */
	: iSignalsNotification(NULL,0,0)
	,iFlowNotification(NULL,0,0)
	{
	}


EXPORT_C TInt RComm::Open(RCommServ& aServer, const TDesC& aName, TCommAccess aMode)
/** Opens a serial port, for one of the three modes indicated by the TCommAccess 
argument.

The TCommAccess argument may dictate exclusive use of the RComm,
shared use (thereby giving permission for the port to be shared by other RComm
objects) or pre-emptable use (allowing another client to pre-empt this session
with an open request in one of the other two modes). The request will fail if
the port does not exist, or if it has been opened in exclusive mode elsewhere.
If the port has been opened in shared mode elsewhere, the request will fail if
a subsequent attempt is made to open it in exclusive mode.
A client opening a port in pre-emptive mode must be prepared to have its
outstanding requests errored with KErrCancel if another client opens the port
in either ECommShared or ECommExclusive mode.  In this case the port handle
will effectively be transferred from the pre-emptable client to the new client.

The first variant (this) will open the port in DTE role, that is, the lines
are configured as for a port of a computer. For instance, the signal lines
DTR and RTS are outputs while the other signals (DCD,CTS,DSR,RING) are inputs.

The second variant allows the port to be opened in either DTE role or DCE
role. This latter role means that the lines are configured as for a port of
a modem. DTR and RTS are inputs while the other signals are outputs.

The request to open in either role will fail with KErrLocked if the port has
already been opened in the opposite role elsewhere.

@code
Signal constant    DTE role     DCE role
----------------------------------------
KSignalCTS         input        output
KSignalDSR         input        output
KSignalDCD         input        output
KSignalRNG         input        output
KSignalRTS         output       input
KSignalDTR         output       input
@endcode

Note:

1. On some earlier versions of C32 the port number was limited to one digit,
i.e. from 0 to 9. This restriction is now removed, and the number
of ports is now only limited by the CSY and/or the Device Drivers up to a maximum
of KMaxTUint ports.

2. This API checks that the client has the correct capabilities to open the port, while
the RCommServ APIs do not. So a CSY that allowed the client to interrogate it via RCommServ::GetPortInfo()
and load it via RCommServ::LoadCommModule() may still reject the same client's call to this API.

3. There are a number of distinct steps that must be performed
before being able to issue an Open() request for a serial port.
For convenience these are listed here:

a. Load the physical device driver, which interacts directly with the hardware at the
lowest level (usually via User::LoadPhysicalDevice()).

b. Load the logical device driver, which provides the comms server with a consistent
interface to the hardware (usually via User::LoadLogicalDevice()).

c. Connect to the C32 Serial Server using RCommServ

d. Load a specific Comms Module (.CSY) by filename via RCommServ::LoadCommModule.

In some environments the physical and logical device drivers are loaded as part of device boot.



@param aServer The comms server session.
@param aName   The name of the port (e.g. "COMM::0")
@param aMode   The mode in which the port is opened. There is no default.
@return System-wide error code: KErrNone if successful; KErrPermissionDenied if 
the port given in aName is wrong or if the request fails the CSY's own security check;
 KErrNotSupported if this port is not 
supported by the CSY or the hardware; KErrLocked if the port has already been 
opened; KErrAccessDenied if the device driver encounteres a problem opening the hardware port.

@capability Dependent
*/
    {
	C32_STATIC_LOG5(KC32Client,_L("RComm::Open(), aServer 0x%x, (port) aName = %S, (port open) aMode = %d (%S)"), &aServer, &aName, aMode, &TC32Log::CommAccessStr16(aMode));
	iSignalsNotification.Set(NULL,0,0);
	iFlowNotification.Set(NULL,0,0);

	TCommRole role = ECommRoleDTE;
	TIpcArgs args(&aName,aMode,role);
	RSessionBase& s = aServer;
    return CreateSubSession(s,ECommOpen, args);
    }

//
// Version 02 extension to the RComm API
//

EXPORT_C TInt RComm::Open(RCommServ& aServer, const TDesC& aName, TCommAccess aMode, TCommRole aRole)
/** Opens a serial port, as described for the other Open() variant. 

This variant allows the port to be opened in either DTE role or DCE role. DCE 
role means that the lines are configured as for a port of a modem. DTR and RTS 
are inputs while the other signals are outputs.

@param aServer The comms server session 
@param aName The name of the port 
@param aMode The mode in which the port is opened. There is no default. 
@param aRole DTE/DCE role 
@return System-wide error code: KErrNone if successful; KErrPermissionDenied if 
the port given in aName is wrong; KErrNotSupported if this port is not supported 
by the CSY or the hardware; KErrLocked if the port has already been opened 
in the opposite role elsewhere. 
@capability Dependent
*/
    {
	C32_STATIC_LOG7(KC32Client,_L("RComm::Open(), aServer 0x%x, (port) aName %S, (port open) aMode = %d (%S), (port) aRole = %d (%S)"), &aServer, &aName, aMode, &TC32Log::CommAccessStr16(aMode), aRole, &TC32Log::CommRoleStr16(aRole));
	TIpcArgs args(&aName,aMode,aRole);
	RSessionBase &s = aServer;
	return CreateSubSession(s,ECommOpen, args);
    }


//
// Version 02 extension to the RComm API
//


EXPORT_C void RComm::OpenWhenAvailable(TRequestStatus& aStatus, RCommServ& aServer, const TDesC& aName)
/** Opens a serial port when it is freed by another client.

When a client has a port open in either exclusive or shared mode, another
client may use this function to gain access to the port when the owning
client closes its handle.  Upon closure, the asynchronous OpenWhenAvailable()
function will complete.  The port role, DTE or DCE, will not be changed by
this request and so the role will be inherited from the previous session.
Furthermore, the port will automatically opened in ECommPreemptable mode.
This can be subsequently upgraded to ECommShared or ECommExclusive mode
using the SetAccessMode() function.

If client submits an OpenWhenAvailable() request and then attempts
to execute an action, for example a read or write request, before the
OpenWhenAvailable has completed, the action will fail with the KErrNotReady
error code.

The server can only support one OpenWhenAvailable request per port. For
example, if client A has a port open in exclusive mode and client B has
an OpenWhenAvailable request pending, then client C's OpenWhenAvailable
request will fail with the KErrAccessDenied error code.

The server is designed to support the OpenWhenAvailable request dispatched
from a different RCommServ session than the one actively using the port.
This is not viewed as a major disadvantage since, in practice, the two
clients will normally be running in different threads, and quite probably
different processes.

If the port is available immediately, it will be opened with DTE role.

@param aStatus The request status used to contain completion information for the function.
@param aServer The comms server session.
@param aName   The name of the port.
@return System wide error code: see return values of RComm::Open().
@capability Dependent
*/
	{
	OpenWhenAvailable(aStatus, aServer, aName, ECommRoleDTE);
	}


EXPORT_C void RComm::OpenWhenAvailable(TRequestStatus& aStatus, RCommServ& aServer, const TDesC& aName, TCommRole aRole)
/** Opens a serial port when freed.

Same as the other OpenWhenAvailable() but here you can specify the preferred
role in case the port is free when you want to open it. If the port is already
opened by another client, you will get the same role when it becomes available.

@param aStatus The request status used to contain completion information for 
			   the function.
@param aServer The comms server session.
@param aName   The name of the port.
@param aRole   Preferred role (DTE or DCE).
@return TInt   A system wide error code: see return values of RComm::Open(). 
@capability Dependent
*/
	{

	C32_STATIC_LOG5(KC32Client,_L("RComm::OpenWhenAvailable(), aServer 0x%x, (port) aName = %S, (port) aRole = %d (%S)"), &aServer, &aName, aRole, &TC32Log::CommRoleStr16(aRole));
	TIpcArgs args(&aName,TIpcArgs::ENothing,aRole);
	RSessionBase& s = aServer;
	TInt ret = CreateSubSession(s, ECommOpenWhenAvailable, args);

	if(ret != KErrNone)
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, ret);
		return;
		}

	args.Set(0,ECommPreemptable);
	SendReceive(ECommSetAccess, args, aStatus);
	}


EXPORT_C void RComm::OpenWhenAvailableCancel()
/** Cancels an OpenWhenAvailable() pending request. 
@capability None
*/
    {
    if (SendReceive(ECommOpenWhenAvailableCancel) == KErrNone)
    	{
    	Close();
    	}
    }


EXPORT_C void RComm::Read(TRequestStatus& aStatus, TDes8& aDes)
/** Reads data from a serial port. 

All reads from the serial device use 8-bit descriptors as data buffers, even 
on a Unicode system.

The length of the TDes8 is set to zero on entry, which means that buffers 
can be reused without having to be zeroed first.

The number of bytes to read is set to the maximum length of the descriptor.

If a read is issued with a data length of zero the Read() completes immediately 
with the side effect that the serial hardware is powered up.

If a read terminates with KErrTimedOut and the descriptor is not empty, 
it will contain valid data. Its length should therefore be tested.

The behaviour of this API after a call to NotifyDataAvailable() is not prescribed
and so different CSY's behave differently. IrComm will allow a successful completion of this API
after a call to NotifyDataAvailable(), while ECUART and ECACM will complete the request with KErrInUse.

@param aStatus The request status used to contain completion information for 
the function. On completion, contains a system-wide error code.
@param aDes A buffer to store the incoming data. The maximum length of the 
descriptor is the number of bytes to be read. 
@capability None
*/
    {
	C32_STATIC_LOG2(KC32Client,_L8("RComm::Read(), Request to read max number of bytes %d"), aDes.MaxLength());
	aDes.SetLength(0);
	TIpcArgs args(&aDes,aDes.MaxLength(),0);
	SendReceive(ECommRead, args, aStatus);
    }


EXPORT_C void RComm::Read(TRequestStatus& aStatus, TDes8& aDes, TInt aLength)
/** Reads a specified number of bytes from a serial port.

Overloaded version of Read() with explicit length.

All reads from the serial device use 8-bit descriptors as data buffers, even 
on a Unicode system.

The length of the TDes8 is set to zero on entry, which means that buffers 
can be reused without having to be zeroed first.

The number of bytes to read is set to aLength.

If a read is issued with a data length of zero (aLength=0) the Read() completes 
immediately with the side effect that the serial hardware is powered up.

If a read terminates with KErrTimedOut and the descriptor is not empty, 
it will contain valid data. Its length should therefore be tested.

The behaviour of this API after a call to NotifyDataAvailable() is not prescribed
and so different CSY's behave differently. IrComm will allow a successful completion of this API
after a call to NotifyDataAvailable(), while ECUART and ECACM will complete the request with KErrInUse.

@param aStatus 	The request status used to contain completion information for 
				the function. On completion, contains a system-wide error code.
@param aDes 	A buffer to store the incoming data. 
@param aLength	An explicit length of data to be read. This length must be 
				between 0 and the MaxLength of the descriptor both included. 
@capability None
*/
    {
	C32_STATIC_LOG3(KC32Client,_L8("RComm::Read(), bytes to read = %d, given buffer's max length %d"), aLength, aDes.MaxLength());
	if((aLength <= aDes.MaxLength()) && (aLength >= 0))
		{
		aDes.SetLength(0);
		TIpcArgs args(&aDes,aLength,0);
		SendReceive(ECommRead, args, aStatus);
	}
	else
		{//Complete the request due to invalid length
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrArgument);
		}
    }


EXPORT_C void RComm::Read(TRequestStatus& aStatus, TTimeIntervalMicroSeconds32 aTimeOut, TDes8& aDes)
/** Reads data from a serial port only if it arrives before a specified time-out.

Overloaded version of Read() with timeout.

All reads from the serial device use 8-bit descriptors as data buffers, even 
on a Unicode system.

The length of the TDes8 is set to zero on entry, which means that buffers 
can be reused without having to be zeroed first.

The number of bytes to read is set to the maximum length of the descriptor. 

If a read is issued with a data length of zero the Read() completes immediately 
but with the side effect that the serial hardware is powered up.

When a Read() terminates with KErrTimedOut, different protocol modules can 
show different behaviours. Some may write any data received into the aDes 
buffer, while others may return just an empty descriptor. In the case of a 
returned empty descriptor use ReadOneOrMore() to read any data left in the 
buffer.

The behaviour of this API after a call to NotifyDataAvailable() is not prescribed
and so different CSY's behave differently. IrComm will allow a successful completion of this API
after a call to NotifyDataAvailable(), while ECUART and ECACM will complete the request with KErrInUse.

@param aStatus The request status used to contain completion information for 
the function. On completion, contains a system-wide error code.
@param aTimeOut The time-out value for reads in microseconds. If data fails 
to arrive before the specified number of microseconds, a KErrTimedOut status 
is returned.
@param aDes A buffer to store the incoming data. The maximum length of the 
descriptor is the default for the number of bytes to be read. 
@capability None
*/
    {
	C32_STATIC_LOG3(KC32Client,_L8("RComm::Read(), aTimeOut = %d, Max Length = %d"), aTimeOut.Int(), aDes.MaxLength());
	aDes.SetLength(0);
	TIpcArgs args(&aDes,aDes.MaxLength(),aTimeOut.Int());
	SendReceive(ECommRead, args, aStatus);
    }


EXPORT_C void RComm::Read(TRequestStatus& aStatus, TTimeIntervalMicroSeconds32 aTimeOut, TDes8& aDes, TInt aLength)
/** Read data from a serial port (before the time-out).

Reads a specified number of bytes from a serial port only if they arrive before 
a specified time-out. Overloaded version of Read() with timeout and explicit 
length.

All reads from the serial device use 8-bit descriptors as data buffers, even 
on a Unicode system.

The length of the TDes8 is set to zero on entry, which means that buffers 
can be reused without having to be zeroed first.

The number of bytes to read is set to aLength.

If a read is issued with a data length of zero the Read() completes immediately 
but with the side effect that the serial hardware is powered up.

When a Read() terminates with KErrTimedOut, different protocol modules can 
show different behaviours. Some may write any data received into the aDes 
buffer, while others may return just an empty descriptor. In the case of a 
returned empty descriptor use ReadOneOrMore() to read any data left in the 
buffer.

The behaviour of this API after a call to NotifyDataAvailable() is not prescribed
and so different CSY's behave differently. IrComm will allow a successful completion of this API
after a call to NotifyDataAvailable(), while ECUART and ECACM will complete the request with KErrInUse.

@param aStatus The request status used to contain completion information for 
the function. On completion, contains a system-wide error code.
@param aTimeOut The time-out value for reads in microseconds. If data fails 
to arrive before the specified number of microseconds, a KErrTimedOut status 
is returned. 
@param aDes A buffer to store the incoming data. 
@param aLength An explicit length of data to be read This length must be 
between 0 and the MaxLength of the descriptor both included.

@capability None
*/
    {
	C32_STATIC_LOG4(KC32Client,_L8("RComm::Read(), aTimeOut = %d, bytes to read = %d, Max Length of buffer = %d"), aTimeOut.Int(), aLength, aDes.MaxLength());
	if((aLength <= aDes.MaxLength()) && (aLength >= 0))
		{
		aDes.SetLength(0);
		TIpcArgs args(&aDes,aLength,aTimeOut.Int());
		SendReceive(ECommRead, args, aStatus);
		}
	else
		{//Complete the request due to invalid length
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrArgument);
		}
    }


EXPORT_C void RComm::ReadOneOrMore(TRequestStatus& aStatus, TDes8& aDes)
/** Read any bytes in the buffer or wait until one arrives.

Reads data from a serial port, but with slightly different behaviour to Read(). 
If there is data in the serial driver's buffer when ReadOneOrMore() is called, 
it will read as much data as possible (up to the maximum length of the supplied 
buffer) and return immediately. If there is no data in the buffer, the request 
will complete as soon as one or more bytes arrive at the serial hardware.

The behaviour of this API after a call to NotifyDataAvailable() is not prescribed
and so different CSY's behave differently. IrComm will allow a successful completion of this API
after a call to NotifyDataAvailable(), while ECUART and ECACM will complete the request with KErrInUse.

@param aStatus The request status used to contain completion information for 
the function. On completion, contains a system-wide error code 
@param aDes A buffer to store the incoming data. The maximum length of the descriptor
is the default for the number of bytes to be read. When the request completes this will 
hold data read from the input buffer. 
@capability None
*/
    {
	C32_STATIC_LOG2(KC32Client,_L8("RComm::ReadOneOrMore(), buffer's max length = %d"), aDes.MaxLength());
	TIpcArgs args(&aDes,-aDes.MaxLength(),0);
	// Note: specifying negative length here to indicate OneOrMore
	//       see RMaxComm::ReadOneOrMore in ECUART.CPP
	SendReceive(ECommRead, args, aStatus);
    }


EXPORT_C TInt RComm::ReadCancel()
/** Cancels any pending Read() or ReadOneOrMore() operations.

The cancelled request will still terminate with its TRequestStatus set to 
any value other than KErrPending. While this is most likely to be KErrCancel, 
it is nevertheless possible for the cancellation to have been issued after 
the asynchronous request had already terminated for some other reason. 
@capability None
*/

    {
	TIpcArgs args(TIpcArgs::ENothing);
	return SendReceive(ECommReadCancel, args);
    }


EXPORT_C TInt RComm::QueryReceiveBuffer() const
/** Gets the number of bytes currently waiting in the driver's input buffer. 
A return value of zero means the buffer is empty.

It is not possible to find out exactly how many bytes are currently in the 
driver's output buffer waiting to be transmitted. However, this is not an 
issue since it is easy to ensure that the output buffer is empty. If 
the KConfigWriteBufferedComplete bit (set via the TCommConfigV01 structure's iHandshake field) is clear,
then all write requests will delay 
completion until the data has completely cleared the driver's output buffer. 
If the KConfigWriteBufferedComplete bit is set, a write of zero bytes to a port 
which has data in the output buffer is guaranteed to delay completion until 
the buffer has been fully drained.

@return The number of bytes currently waiting to be read from the receive 
buffer. 
@capability None
*/
{
	TUint bufferSize;
	TPtr8 s((TUint8 *)&bufferSize, sizeof(bufferSize));
	TIpcArgs args(&s);
	TInt ret = SendReceive(ECommQueryReceiveBuffer,args);
	if(ret != KErrNone)
		{
		return ret;
		}
	else
		{
		return bufferSize;
		}
}


EXPORT_C TInt RComm::ResetBuffers(TUint aFlags)
/** Resets the transmit and receive serial port buffers independently.

aFlags is a bitmask, so setting KCommResetRx|KCommResetTx (the default) resets 
both buffers and all data in them is lost. This is the default value, so this 
is what happens if the function is called without a parameter.

This function should not be called when there are pending reads or writes.

@param aFlags KCommResetRx to reset the receive buffer KCommResetTx to reset 
the transmit buffer 
@capability None
*/
    {
	TIpcArgs args(aFlags);
	return SendReceive(ECommResetBuffers, args);
    }


EXPORT_C void RComm::Write(TRequestStatus& aStatus, const TDesC8& aDes)
/** Writes data to a serial port. 

All writes to the serial device use 8-bit descriptors as data buffers, even 
on a Unicode system.

The number of bytes to write is set to the maximum length of the descriptor.

When a Write() is issued with a data length of zero it cannot complete until 
the current handshaking configuration and the state of input control lines 
indicate that it is possible for data to be immediately written to the serial 
line, even though no data is to be written. This functionality is useful when 
determining when serial devices come on line, and checking that the output 
buffer is empty (if the KConfigWriteBufferedComplete bit is set).

@param aStatus The request status used to contain completion information for 
the function. On completion, contains a system-wide error code. 
@param aDes A descriptor containing the data to be written to the serial port. 
@capability None
*/
    {
	TIpcArgs args(&aDes,aDes.Length(),0);
	SendReceive(ECommWrite,args,aStatus);
    }


EXPORT_C void RComm::Write(TRequestStatus& aStatus, const TDesC8& aDes, TInt aLength)
/** Writes a specified number of bytes to a serial port. 

All writes to the serial device use 8-bit descriptors as data buffers, even 
on a Unicode system.

The number of bytes to write is set to aLength.

When a Write() is issued with a data length of zero it cannot complete until 
the current handshaking configuration and the state of input control lines 
indicate that it is possible for data to be immediately written to the serial 
line, even though no data is to be written. This functionality is useful when 
determining when serial devices come on line, and checking that the output 
buffer is empty (if the KConfigWriteBufferedComplete bit is set).

@param aStatus The request status used to contain completion information for 
the function. On completion, contains a system-wide error code.
@param aDes A descriptor containing the data to be written to the serial port.
@param aLength The number of bytes to write. This length must be between 0 
and the length of the descriptor both included.
@capability None
*/
    {
	C32_STATIC_LOG2(KC32Client,_L8("RComm::Write(), (bytes to write) aLength = %d"), aLength);
	if((aLength <= aDes.Length()) && (aLength >= 0))
		{
		TIpcArgs args(&aDes,aLength,0);
		SendReceive(ECommWrite, args, aStatus);
		}
	else
		{//Complete the request due to invalid length
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrArgument);
		}
    }


EXPORT_C void RComm::Write(TRequestStatus& aStatus, TTimeIntervalMicroSeconds32 aTimeOut, const TDesC8& aDes)
/** Writes data to a serial port within a specified time-out.

All writes to the serial device use 8-bit descriptors as data buffers, even 
on a Unicode system.

The number of bytes to write is set to the maximum length of the descriptor.

When a Write() is issued with a data length of zero it cannot complete until 
the current handshaking configuration and the state of input control lines 
indicate that it is possible for data to be immediately written to the serial 
line, even though no data is to be written. This functionality is useful when 
determining when serial devices come on line, and checking that the output 
buffer is empty (if the KConfigWriteBufferedComplete bit is set).

@param aStatus The request status used to contain completion information for 
the function. On completion, contains a system-wide error code.
@param aTimeOut The write will terminate after aTimeout microseconds if the 
data requested has not been sent. A KErrTimedOut status is returned.
@param aDes A descriptor containing the data to be written to the serial port. 
@capability None
*/
    {
	C32_STATIC_LOG2(KC32Client,_L8("RComm::Write(), aTimeOut = %d (microseconds)"), aTimeOut.Int());
	TIpcArgs args(&aDes,aDes.Length(),aTimeOut.Int());
	SendReceive(ECommWrite,args, aStatus);
    }


EXPORT_C void RComm::Write(TRequestStatus& aStatus, TTimeIntervalMicroSeconds32 aTimeOut, const TDesC8& aDes, TInt aLength)
/** Writes a specified number of bytes to a serial port within a specified 
time-out.

All writes to the serial device use 8-bit descriptors as data buffers, even 
on a Unicode system.

The number of bytes to write is set to aLength.

When a Write() is issued with a data length of zero it cannot complete until 
the current handshaking configuration and the state of input control lines 
indicate that it is possible for data to be immediately written to the serial 
line. This functionality is useful when determining when serial devices come 
on line, and checking that the output buffer is empty (if 
the KConfigWriteBufferedComplete bit is set).

@param aStatus The request status used to contain completion information for 
the function. On completion, contains a system-wide error code 
@param aTimeOut The write will terminate after aTimeout microseconds if the 
data requested has not been sent. A KErrTimedOut status is returned.
@param aDes A descriptor containing the data to be written to the serial port.
@param aLength The number of bytes to write. This length must be between 0 
and the maximum length of the descriptor both included. 
@capability None
*/
    {
	C32_STATIC_LOG3(KC32Client,_L8("RComm::Write(), aTimeOut = %d (microseconds), (bytes to write) aLength = %d"), aTimeOut.Int(), aLength);
	if((aLength <= aDes.Length()) && (aLength >= 0))
		{
		TIpcArgs args(&aDes,aLength,aTimeOut.Int());
		SendReceive(ECommWrite, args, aStatus);
		}
	else
		{//Complete the request due to invalid length
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrArgument);
		}
    }


EXPORT_C TInt RComm::WriteCancel()
/** Cancels any pending Write() operations.

The cancelled request will still terminate with its TRequestStatus set, to 
any value other than KErrPending. While this is most likely to be KErrCancel, 
it is nevertheless possible for the cancellation to have been issued after 
the asynchronous request had already terminated for some other reason. 

@capability None
*/
    {
	TIpcArgs args(TIpcArgs::ENothing);
	return SendReceive(ECommWriteCancel, args);
    }

	
EXPORT_C void RComm::Break(TRequestStatus& aStatus, TTimeIntervalMicroSeconds32 aTime)
/** Sets a break condition for a specified time.

A break condition on a line is when a data line is held permanently high for
an indeterminate period which must be greater than the time normally taken
to transmit two characters. It is sometimes used as an error signal between
computers and other devices attached to them over RS232 lines.

Notes:

Setting breaks is not supported on the integral ARM serial hardware.
EPOC has no support for detecting received breaks.
There is no way to detects whether setting a break is supported using Caps().
 
@param aStatus The request status used to contain completion information for 
the function. On completion, contains a system-wide error code.
@param aTime A timed break period in microseconds. 

@capability None
*/
    {
	C32_STATIC_LOG2(KC32Client,_L8("RComm::Break(), for aTime = %d (microseconds)"), aTime.Int());
	TIpcArgs args(aTime.Int());
	SendReceive(ECommBreak, args, aStatus);
    }


EXPORT_C TInt RComm::BreakCancel()
/** Cancels any pending Break() operations. 
@capability None
*/
    {
	TIpcArgs args(TIpcArgs::ENothing);
	return SendReceive(ECommBreakCancel,args);
    }


EXPORT_C TInt RComm::Cancel()
/** Cancels any pending reads and writes.

A panic will result if an attempt is made to reconfigure a port if there are 
any pending reads or writes. Use this function before configuring a port if 
there is any doubt as to whether it is safe to do so. It isn't necessary 
to call Cancel() before closing a port as this is done automatically. 
@capability None
*/
    {
	TIpcArgs args(TIpcArgs::ENothing);
	return SendReceive(ECommCancel, args);	
    }


EXPORT_C TInt RComm::Config(TDes8& aConfig) const
/** Reads the current configuration of the serial port.

It isn't essential to read an existing configuration in order to set a new 
one. However, calling this function is a useful way of initializing a TCommConfig 
structure and thus avoiding the overhead of having to explicitly set every 
member. If a port were being shared between difference processes, it would 
also be good manners to save an existing configuration before using the port 
in order than it might be restored when you have finished with it.

@param aConfig On return, a TCommConfig package buffer holding the configuration 
settings 
@capability None
*/
    {
	TIpcArgs args(&aConfig);
	return SendReceive(ECommConfig, args);
    }


EXPORT_C TInt RComm::SetConfig(const TDesC8& aConfig)
/** Sets the configuration of the serial port.

It is not possible to reconfigure the capabilities of ports while they are 
being used. In particular, if any client attempts to reconfigure a port when
there are pending reads or writes a KErrInUse error code will be returned.

@param aConfig New configuration settings packaged in a TCommConfig
@return System-wide error code: KErrInUse if there is a pending read or write on 
the port, otherwise system-wide error codes
@capability None
*/
    {
	TIpcArgs args(&aConfig);
	return SendReceive(ECommSetConfig, args);
    }


EXPORT_C TInt RComm::Caps(TDes8& aCaps) const
/** Reads the capabilities of the serial port.

As the serial driver can control a range of different physical devices with 
differing capabilities, it could be considered important from the point of 
view of the operating system to be able to read these capabilities before 
attempting to configure the port. In practice however, program development 
is likely to be targeted at a specific hardware configuration, and interrogation 
of a device to ascertain its capabilities isn't normally necessary.

@param aCaps On return this descriptor will contain either a packaged TCommCapsV01 
structure or a packaged TCommCapsV02 structure corresponding to the capabilities 
of the serial device and of the CSY module, depending on the size of the descriptor 
passed in to the function. 
@capability None
*/
    {
	TIpcArgs args(&aCaps);
	return SendReceive(ECommCaps, args);
    }


EXPORT_C TInt RComm::Mode(TCommServerConfig& aConfig) const
/** Gets server buffering mode.

@param aConfig A packaged TCommServerConfigV01 structure which will hold the 
required information on return. 
@capability None
*/
    {
	TIpcArgs args(&aConfig);
	return SendReceive(ECommGetMode, args);
    }


EXPORT_C TInt RComm::SetMode(const TCommServerConfig& aConfig)
/** Sets the server buffering mode.

This function can be used to set either buffering mode, and if partial buffering
is being used, it can also set the size of the buffer that the server will use.
These items are packaged up into the TCommServerConfig descriptor.

The default of full buffering is nearly always going to be the quicker option, 
as only a single client-server data transfer is ever needed. The mode should 
not be changed by an application without a very good reason (such as memory 
constraints) otherwise performance is liable to suffer.

@param aConfig A packaged TCommServerConfigV01 structure to be set.
@return System-wide error code 
@capability None
*/
    {
	TIpcArgs args(&aConfig);
	return SendReceive(ECommSetMode, args);
    }


EXPORT_C TUint RComm::Signals(TUint aSignalMask) const
/** Reads the serial port control lines.

Reads the status of the RS232 input and output lines (RTS, CTS, DSR, DCD, DTR). 
They are bit-masked into a single integer and one or all of them can be read 
at any time.

Not all of the input lines are guaranteed to be available on all serial devices. 

@param aSignalMask Bitmask of lines to read. It defaults to all lines (0x3f 
as only the top six bits of this integer are valid). A different value can 
be specified as aSignalMask if specific information is required: for instance, 
Signals(KSignalDSR) could be used to find out if a modem were connected to 
the serial port.
@return An integer with the bits set to reflect the status of the handshaking 
lines 
@capability None
*/
    {
	C32_STATIC_LOG2(KC32Client,_L8("RComm::Signals(), aSignalMask = %d"), aSignalMask);
    TUint signals;
    TPtr8 s((TUint8 *)&signals, sizeof(signals));
	TIpcArgs args(&s);
	SendReceive(ECommSignals, args);
	return signals & aSignalMask;
    }


EXPORT_C TInt RComm::SetSignalsToMark(TUint aSignalMask)
/** Sets the specified RS232 output lines.

Using this function, either of the RS232 output lines (DTR and RTS) can be
set manually.

For many applications, these lines will be read and set under driver
control as determined by the handshaking options selected.

@param aSignalMask Bitmask of handshaking lines 
@capability None
*/
    {
	C32_STATIC_LOG2(KC32Client,_L8("RComm::SetSignalsToMark(), aSignalMask = %d"), aSignalMask);
	TIpcArgs args(aSignalMask);
	return SendReceive(ECommSetSignalsToMark, args);
    }


EXPORT_C TInt RComm::SetSignalsToSpace(TUint aSignalMask)
/** Clears RS232 output lines.

Using this function, either of the RS232 output lines (DTR and RTS) can be
cleared manually.

For many applications, these lines will be read and set under driver
control as determined by the handshaking options selected.

@param aSignalMask Bitmask of handshaking lines 
@capability None
*/
    {
	C32_STATIC_LOG2(KC32Client,_L8("RComm::SetSignalsToSpace(), aSignalMask = %d"), aSignalMask);
	TIpcArgs args(aSignalMask);
	return SendReceive(ECommSetSignalsToSpace, args);
    }


EXPORT_C TInt RComm::ReceiveBufferLength() const
/** Gets the size of the serial port buffers. 

Despite its name, this function returns the size of both the receive and 
transmit buffers, which are (obviously) always the same size.

@return The current size of both the receive and transmit buffers in bytes. 
@capability None
*/
    {
	TInt bufferSize;
	TPtr8 s((TUint8 *)&bufferSize,sizeof(bufferSize));

	TIpcArgs args(&s);
	TInt ret = SendReceive(ECommReceiveBufferLength, args);
	if(ret != KErrNone)
		{
		return ret;
		}
	else
		{
		return bufferSize;
		}
    }


EXPORT_C TInt RComm::SetReceiveBufferLength(TInt aLength)
/** Sets the size of the serial port receive and transmit buffers.

Notes:

There is no error code returned by this function. If the buffer is set to 
too large a figure, the request is simply ignored. If you are in any doubt 
about the success of a SetReceiveBufferLength request, you should examine 
the returned value from ReceiveBufferLength. 

It isn't always essential to set a buffer size as there is a generous default 
of 1024. 

Changing the size of the receive buffer is the only way to tailor the operation 
of flow control in Symbian OS as the exact position of the high water mark 
is not configurable.

@param aLength The required size of the receive and transmit buffers in bytes. 
@capability None
*/
    {
	C32_STATIC_LOG2(KC32Client,_L8("RComm::SetReceiveBufferLength(), aLength = %d"), aLength);
	TIpcArgs args(aLength);
	return SendReceive(ECommSetReceiveBufferLength,args);
    }


EXPORT_C void RComm::Close()
/** Closes a serial port.
If the session is closed without having closed all the open ports via calls to this function, then the session close
will close all remaining ports opened by the session.
@capability None
*/
    {
    CloseSubSession(ECommClose);
    }


#ifdef _DEBUG_DEVCOMM
EXPORT_C TInt RComm::DebugInfo(TDes8& aDes)
/** Dump debug info.

 @param aDes Description of the debug information.
@capability None
*/
    {
	TIpcArgs args(&aDes);
	return SendReceive(ECommDbgDoDumpDebugInfo,args);
    }
#endif


//
// Extensions to the RComm API starts from here
//


EXPORT_C void RComm::NotifySignalChange(TRequestStatus& aStatus, TUint& aSignals, TUint aSignalMask)
/** Notifies the client when one of the signals change.

@param aStatus Asynchronous completion status 
@param aSignals On return, the new set of signals. The bottom five bits contain 
a mask of the signals that are set (using the Signal line constants). The 
top four bits store which signal has changed (using the Signal line changed 
constants).
@param aSignalMask Bitmaks of the signals to monitor. The default is all signals. 
See also KSignalDTEOutputs and KSignalDTEInputs. 
@capability None
*/
    {
	C32_STATIC_LOG3(KC32Client,_L8("RComm::NotifySignalChange(), aSignals = %d, aSignalMask = %d"), aSignals, aSignalMask);
	iSignalsNotification.Set((TUint8*)&aSignals, sizeof(TUint), sizeof(TUint));
	TIpcArgs args(&iSignalsNotification,aSignalMask);
	SendReceive(ECommNotifySignals, args, aStatus);
    }


EXPORT_C TInt RComm::NotifySignalChangeCancel() const
/** Cancels a NotifySignalChange() request. 
@capability None
*/
    {
	TIpcArgs args(TIpcArgs::ENothing);
	return SendReceive(ECommNotifySignalsCancel, args);
    }


EXPORT_C void RComm::NotifyConfigChange(TRequestStatus& aStatus, TDes8& aNewConfig) const
/** Notifies the client when the port configuration (data rate, character width, 
stop bits, handshaking, or parity) of the remote end changes.

@param aStatus On return, system wide error code for asynchronous function.
@param aNewConfig On completion, a packaged TCommNotificationV01. 
@capability None
*/
    {
	TIpcArgs args(&aNewConfig);
	SendReceive(ECommNotifyConfigChange, args, aStatus);
    }


EXPORT_C TInt RComm::NotifyConfigChangeCancel() const
/** Cancels a NotifyConfigChange() request. 
@capability None
*/
    {
	TIpcArgs args(TIpcArgs::ENothing);
	return SendReceive(ECommNotifyConfigChangeCancel, args);
    }


EXPORT_C void RComm::NotifyFlowControlChange(TRequestStatus& aStatus, TFlowControl& aFlowControl)
/** Notifies the client when the flow control between the CSY and the external 
device changes.

@param aStatus On return, system wide error code for asynchronous function.
@param aFlowControl On completion, new flow control status. 
@capability None
*/
    {
	iFlowNotification.Set((TUint8*)&aFlowControl, sizeof(TFlowControl), sizeof(TFlowControl));
	TIpcArgs args(&iFlowNotification);
	SendReceive(ECommNotifyFlowControl, args, aStatus);
    }


EXPORT_C TInt RComm::NotifyFlowControlChangeCancel() const
/** Cancels a NotifyFlowControlChange() request. 
@capability None
*/
    {
	TIpcArgs args(TIpcArgs::ENothing);
	return SendReceive(ECommNotifyFlowControlCancel, args);
    }


EXPORT_C TInt RComm::GetFlowControlStatus(TFlowControl& aFlowControl) const
/** Gets the current status of flow control between the port and the third party 
(external PC or internal signalling stack).

@param aFlowControl On return, flow control status 
@return System-wide error code 
@capability None
*/
    {
	TPckg<TFlowControl> len(aFlowControl);
	TIpcArgs args(&len);
	return SendReceive(ECommGetFlowControl, args);
    }




EXPORT_C void RComm::NotifyBreak(TRequestStatus& aStatus) const
/** Notifies the client when a break occurs: the remote end has held the communication 
line high for a certain number of bits to indicate a break condition.

@param aStatus On return, system wide error code for asynchronous function. 
@capability None
*/
	{
	TIpcArgs args(TIpcArgs::ENothing);
	SendReceive(ECommNotifyBreak,args,aStatus);
    }


EXPORT_C TInt RComm::NotifyBreakCancel() const
/** Cancels a NotifyBreak() request. 
@capability None
*/
    {
	TIpcArgs args(TIpcArgs::ENothing);
	return SendReceive(ECommNotifyBreakCancel, args);
    }


EXPORT_C TInt RComm::GetRole(TCommRole& aRole) const
/** Gets current DCE/DTE role.

@param aRole On return, current DCE/DTE role 
@return System-wide error code 
@capability None
*/
    {
	TPckg<TCommRole> len(aRole);
	TIpcArgs args(&len);
	return SendReceive(ECommGetRole, args);
    }


EXPORT_C void RComm::NotifyDataAvailable(TRequestStatus& aStatus) const
/** Notifies the client when data is available to be read from the input buffer.

This API allows the client to delay creation of the buffer receiving the data until the data
has arrived. This allows the client to query the size of the data waiting before creating the buffer,
and simplifies the client's buffer management since it is only needed during the subsequent Read() calls.

The behaviour of this API after a call to Read() or ReadOneOrMore() is not prescribed
and so different CSY's behave differently. IrComm will allow a successful completion of this API
after a call to Read() or readOneOrMore(), while ECUART and ECACM will complete the request with KErrInUse.

@param aStatus On return, a system wide error code.
@capability None
*/
	{
	TIpcArgs args(TIpcArgs::ENothing);
	SendReceive(ECommNotifyDataAvailable,args, aStatus);
    }


EXPORT_C TInt RComm::NotifyDataAvailableCancel() const
/** Cancels a NotifyDataAvailable() request. 
@capability None
*/
    {
	TIpcArgs args(TIpcArgs::ENothing);
	return SendReceive(ECommNotifyDataAvailableCancel,args);
    }


EXPORT_C void RComm::NotifyOutputEmpty(TRequestStatus& aStatus) const
/** Notifies the client when the transmit buffer is emptied.

@param aStatus On return, system wide error code for asynchronous function. 
@capability None
*/
	{
	TIpcArgs args(TIpcArgs::ENothing);
	SendReceive(ECommNotifyOutputEmpty,args,aStatus);
    }


EXPORT_C TInt RComm::NotifyOutputEmptyCancel() const
/** Cancels a NotifyOutputEmpty() request. 
@capability None
*/
    {
	TIpcArgs args(TIpcArgs::ENothing);
	return SendReceive(ECommNotifyOutputEmptyCancel,args);
    }


EXPORT_C TInt RComm::SetAccessMode(TCommAccess aNewMode)
/** Upgrades the mode of a port handle from pre-emptive mode to shared or exclusive 
mode.

Notes:

If a client attempts to change the mode of a port that is already in either 
shared or exclusive modes, the request will fail with the KErrNotSupported 
error code.

Clients should not attempt to change the access mode to pre-emptive, the behaviour 
in this case is unpredictable.

@param aNewMode The value of the new port mode, either ECommShared or ECommExclusive.
@return System-wide error code. 
@capability None
*/
	{
	C32_STATIC_LOG3(KC32Client,_L8("RComm::SetAccessMode(), aNewMode = %d (%S)"), aNewMode, &TC32Log::CommAccessStr(aNewMode));
	TIpcArgs args(aNewMode);
	return SendReceive(ECommSetAccess, args);
	}

#ifdef _DEBUG
EXPORT_C TInt RComm::DebugState( TCommDebugInfo& aInfo )
/** Returns the debug state.

@return aInfo debug info to be returned to. 
@capability None
*/
	{
	TPckg<TCommDebugInfo> x(aInfo);
	TIpcArgs args(&x);
	return SendReceive(ECommDebugState, args);
	}
#else
EXPORT_C TInt RComm::DebugState( TCommDebugInfo& /*aInfo*/ )
/** Debug state information. Only implemented for DEBUG builds. */
	{
	return 0;
	}
#endif


// EOF - CC_CLI.CPP
