
/**
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Baseband Channel Adaptor(BCA) APIs.
* This file contains all the APIs required to implement a BCA interface for Symbian OS.
* 
*
*/



/**
 @file
 @publishedPartner
 @released
*/
 
#ifndef BCA_H
#define BCA_H

#include <e32std.h>
#include <e32des8.h>

/** This namespace includes the BCA component names.*/
namespace BasebandChannelAdaptation
{

/** 
* The abstract interface between NIF and Bearer Transport. 
* The client (caller) of MBca is the one conceptually living in a specific Comms Stack layer 
* and it is entirely responsible for implementing the function of that layer as imposed by the 
* respective protocol technology. On the contrary, the MBca is merely a HAI (Hardware Abstraction 
* Interface) and its implementation is protocol/technology agnostic. This means that the MBca 
* is solely responsible for passing (entire) data frames between its caller and the bearer transport 
* as well as for propagating error conditions towards the client and for propagating flow 
* control conditions in both directions. 
*
* @publishedPartner 
* @released 
*/ 
class MBca

	{
public:
	
	/** 
	* Deletes the allocated data in BCA the MBca implementation and all resources it owns. 
	* The method is the logical reverse operation to MBcaFactory::NewBcaL()
	*/
	virtual void Release()=0;

	/** 
	* Informs that the BCA is required by the client(for instance, Raw IP NIF)
	*
	* @param aStatus complete status, KErrNone if succesful, system-wide error code otherwise.
	* All other error codes reported are fatal (denoting the MBca interface has failed to open 
	* and is not good for transferring data) including KErrCancel if request cancelled.
	*
	* @param aChannelId an optional parameter, used to open the BCA. It is implementation dependent, 
	* It can be empty descriptor when it is not used. 
	*/ 
	virtual void Open(TRequestStatus& aStatus, const TDesC& aChannelId)=0;

	
	/** 
	* Close is a "emergency"/synchronous termination which closes the BCA immediately. 
	* Informs the BCA is no longer required by the client. 
	* The method is the logical reverse operation to ::Open(). 
	* Close cancels all pending asynchronous requests with KErrCancel.
	*/ 
	virtual void Close()=0;
		

	/** 
	* Shutdown is a graceful, asynchronous termination which can take some time. It is called under 
	* normal stop cases. 
	* Informs the BCA is no longer required by the client and can release its resources. 
	* The method is the logical reverse operation to ::Open().
	* Shutdown cancels all other pending asynchronous requests with KErrCancel.
	*
	* @param aStatus complete status, KErrNone if succesful KErrCancel if request cancelled.
	*/ 
	virtual void Shutdown(TRequestStatus& aStatus)=0;


	/**
	* Queues a Read, reads data from underlying baseband channel. 
	* Used for for uplink flow control, i.e. the client will refrain from calling MBca::Read if 
	* in a flow off condition.
	*
	* @param aStatus complete status, KErrNone if succesful, system-wide error code otherwise.
	* All other errors reported are fatal (the MBca became defunct and can no loger be used to 
	* send and receive data) except KErrCancel if request cancelled and KErrNoMemory, KErrCommsParity, 
	* KErrCommsOverrun, KErrCommsFrame, in which case the MBca stays functional and is only reporting 
	* failure to read the next frame. The caller may attempt to ::Read again. 
	*
	* @param aBuf buffer for data to be read. The caller owns the buffer and is responsible for keeping 
	* its reference valid at least until the MBca::Read call is completed. The caller is responsible 
	* for supplying a large enough buffer for the MBca implementation to fit the entire pending frame. 
	* If the caller fails to supply a large enough buffer, the MBca implementation is free to error the 
	* ::Read with KErrNoMemory.
	*
	* @note the client must not modify the buffer until this call is completed. 
	*/ 
	virtual void Read(TRequestStatus& aStatus, TDes8& aBuf)=0;
	

	/** 
	* Queues a Write, write data to underlying baseband channel. 
	* Used for downlink flow control. The MBca implementation will not complete a pending ::Write if 
	* in flow off condition. 
	*	
	* @param aStatus the complete status, KErrNone if succesful, system-wide error code otherwise. 
	* All errors reported are fatal (the MBca became defunct and can no loger be used to 
	* send and receive data) except KErrCancel if request cancelled and KErrNoMemory and KErrNotReady,
	* in which case the MBca implementation indicates the failure to send the current frame. The caller 
	* may drop or resend the current packet depending on its logic. 
	*
	* @param aBuf the buffer to sent. The caller owns the buffer and is responsible for keeping 
	* its reference valid at least until the MBca::Write call is completed.
	*
	* @note the client must not modify the buffer until this call is completed.
	*/
	virtual void Write(TRequestStatus& aStatus, const TDesC8& aBuf)=0;
	

	/** 
	* Cancels the pending read request with KErrCancel.
	*/ 
	virtual void CancelRead()=0;


	/** 
	* Cancels the pending write request with KErrCancel.
	*/
	virtual void CancelWrite()=0;
	

	/** 
	* The BCA control function,gets or sets the information of the BCA in an asynchronous manner.
	*
	* @param aStatus complete status, KErrNone if succesful, system-wide error code otherwise.
	* All errors reported are fatal (the MBca became defunct and can no loger be used to 
	* send and receive data) except KErrNotSupported, no functionality provided and KErrAlreadyExists.
	*
	* @param aOptLevel option level to be used. 
	*
	* @param aOptName option name to be used. 
	*
	* @param aOpt an optional parameter,holds the option value on return or the option value to be set. 
	*
	* @return none. 
	*/ 
	virtual void Ioctl(TRequestStatus& aStatus, TUint aOptLevel, TUint aOptName, TDes8& aOpt)=0;

	
	/**
	* Cancels the pending Ioctl request with KErrCancel.
	*/
	virtual void CancelIoctl()=0;
	};
	

/**
* Control option level.
*/
const TUint KBcaOptLevelGeneric 		= 0x0194;
const TUint KBcaOptLevelExtSerial 		= 0x0195;

/**
* C32 BCA capability
*/
const TUint KBcaCapSerial			= 0x01; //Serial port capability supported

/**
* Control option name
*/
const TUint KBCAMru	                = 0x12;
const TUint KBCAMtu	                = 0x13;
const TUint KBCASpeedMetric	        = 0x14;
const TUint KBCACaps	            = 0x15;
const TUint KBCASetIapId	        = 0x16;
const TUint KBCASetBcaStack	        = 0x1e;

/** 
Purges the buffers */
const TUint KBCAResetBuffers        = 0x1f;

const TUint KSerialCaps	            = 0x17;
const TUint KSerialConfig	        = 0x18;
const TUint KSerialPortName	        = 0x19;
const TUint KSerialSetConfig	    = 0x1a;
const TUint KSerialSetCsyName	    = 0x1b;
const TUint KVersionNumber	        = 0x1c;
const TUint KSerialSetCommRole	    = 0x1d;

/**
Set the size of receive & transmit buffers.
Provided for compatibility with C32 RComm */
const TUint KSerialSetTxRxBufferSize  = 0x2a;

/** Retrieves the size of the receive & transmit buffers */
const TUint KSerialTxRxBufferSize     = 0x2b;

/** Set and/or clear signal lines */
const TUint KSerialSetControlLines	  = 0x2c;

struct TSerialSetControlLines {
/**
Structure associated with KSerialSetSignals Ioctl.
Indicates which control lines to set/clear. 
*/
	TUint	iSetMask;
	TUint	iClearMask;
};

//
// Bitmasks specifying which buffer to reset. Used with KBCaResetBuffers
//

/** Reset Rx buffer only */
const TUint KResetRxBuf = 0x01;

/** Reset Tx buffer only */
const TUint KResetTxBuf = 0x02;


/** Monitor EIA-232 control lines and take action specified. */
const TUint KSerialMonitorControlLines = 0x30;

/** Turn control line monitoring off - do not fail on line change */
const TUint KMonitorOff = 0x0;

/** Fail when BCA-specific lines go from high to low. 
This is the "Default" option, where the BCA itself selects what lines to monitor 
This mask must not be specified in conjunction with other masks, as the BCA will monitor
& fail on its specific lines ONLY. The other lines will be ignored.*/
const TUint KFailBcaSpecificOnly = 0xFFFFFFFF; 
}

#endif // BCA_H
