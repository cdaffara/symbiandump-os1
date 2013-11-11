
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
 @prototype
 @released
*/
#ifndef BCA2_H
#define BCA2_H

/** This namespace includes the BCA component names.*/
namespace BasebandChannelAdaptation2
{
/**
 * The class implemented by an upper component to accept control signals from the lower component

 * @publishedPartner
 * @prototype
 */
class MUpperControl
	{
public:
	/**
	* Indicates to the layer above (link layer) that the Bca is ready to start 
	* receiving packets from the upper layer (either after MBca start 
	* completes or following congestion)
	
	* @param none
	* @return none.
	*/
	virtual void StartSending()=0;
	
	/**
	* Any fatal error which occurs during the send/receive session is reported 
	* to the client by a call to this function. Fatal errors are errors which can’t 
	* be recovered from (e.g.:  connection down) in which case MBca has become 
	* defunct and can no longer be used to send and receive data
	
	* @param aErr TThe fatal error code from the session failures propagated 
	* to the upper layer of the communications stack . Non fatal errors keep 
	* the MBca2 functional which in the context of MBca2 implementation may 
	* include KErrNoMemory, KErrCommsParity, KErrCommsOverrun, KErrCommsFrame. 
	* @return none.
	*/
	virtual void Error(TInt aErr)=0;

	};
	
/**
 * The Class implemented by an upper component to accept inbound data travelling 
 * upwards from the lower component

 * @publishedPartner
 * @prototype
 */ 	
class MUpperDataReceiver
	{
public:
	/**
	* This function is called whenever data has been received by MBca which 
	* should be processed by its client. The implementer takes ownership of the 
	* buffer and is responsible for its deletion.
	
	* @param aCommsBufChain - The list of comms buffers containing data to be processed. 
         * Destination keeps the custody of the buffer.
	* @return none.
	*/
	virtual void Process(RCommsBufChain& aCommsBufChain)=0;
	};

/**
 * The interface implemented by the lower component to accept data from the upper component

 * @publishedPartner
 * @prototype
 */
 	
class MLowerDataSender
	{
	public:
	enum TSendResult
		{
			// data accepted, send no more until MUpperControl::StartSending() is called
			ESendBlocked,
			// data accepted, can send more.
			ESendAccepted
		};
	/**
	* Sends the specified buffer data down to the base-band. The implementer takes 
	* ownership of the buffer and is responsible for its deletion.
	
	* @param aCommsBufChain the comms buffer list to be sent.The buffer ownership is passed 
	* to the BCA
	* @return TTSendResult either ESendBlocked or SendAccepted. When the Bca 
	* is congested and cannot send any data beyond the current packet (which is 
	* always accepted), the implementation returns ESendBlocked. If BCA is not
	* congested then ESendAccepted is returned to continue sending. When congestion 
	* passes, the Bca calls StartSending on the upper layer to resume sending. The
	* implementation is recommended to panic any attempts to call Send during congestion 
	*/
	virtual	TSendResult Send(RCommsBufChain& aCommsBufChain)=0;
	};

/**
 * The interface implemented by the lower component to accept control signals from the upper component
 
 * @publishedPartner
 * @prototype
 */ 
class MBca2
	{
public:
	 struct TBcaParams 
		{
		RCommsBufPond iCommsBufPond;	// Pond to be used by BCA implementation for creating, using pools and buffers
		const TDesC& iChannelId;				// port number or channel which BCA uses to communicate with its lower layers 
		TBcaParams(RCommsBufPond aCommsBufPond,const TDesC& aChannelId)
		: iCommsBufPond(aCommsBufPond),
		iChannelId(aChannelId){}
	 	};

		
	/**
	* Opens the BCA instance and commences a BCA session (::Close() terminates the session). 
	
	* @param aControl reference to a MUpperControl implementation, which will 
	* serve as the call-back interface for incoming control calls
	* @param aData - reference to a MUpperDataReceiver implementation, which
	* will serve as the call-back interface for incoming data
	* @param aBcaParams A reference to the BCA start up params
	* @return KErrNone if successful, system-wide error code otherwise. All 
	* other error codes reported are fatal denoting that MBca interface has 
	* failed to open and MBca is not good for transferring data
	*/
	virtual TInt Open(MUpperControl& aControl, 
					MUpperDataReceiver& aData, 
					const TBcaParams& aBcaParams)=0;
	
	/**
	 * The API starts the baseband connection asynchronously. The Start () request 
	 * API’s successful completion is acknowledged with MUpperControl::StartSending(). 
	 * The request failure is acknowledged with MUpperControl::Error(). Client should 
	 * take care of scenarios when MUpperControl::StartSending() gets very delayed 
	 * or not called at all
 	
	* @param none
	* @return none
	*/
	virtual void Start()=0;
	
	/**
	 * Returns a reference to the MLowerDataSender, This reference is used by 
	 * upper components to send packets to the lower components. This API must be 
	 * called only after Start() completes, otherwise the implementation should panic.
 	
	* @param none
	* @return reference to the MLowerDataSender. 
	*/
	virtual MLowerDataSender& GetSender()=0;
	
	/**
	* Synchronously closes the BCA immediately. Informs the BCA is no longer 
	* required by the client and can release its resources. Close cancels all 
	* pending asynchronous operations which can only be ::Start() or ::Control() .
	* To use the BCA instance it needs to be re-opened. Close operation must not fail.
	
	* @param none
	* @return none.
	*/
	virtual void Close()=0;
	
	/**
	* Release/deletes the BCA instance. Derived classes will typically implement 
	* this as ‘delete this’.
	
	* @param none
	* @return none.
	*/
	virtual void Release()=0;
	
	enum TBlockOption
		{
			//stop sending [block] data on interface
			EBlockFlow, 
			// start sending [unblock] data on interface
			EUnblockFlow
		};
	/**
	* Either blocks or unblocks the pushing of received data to the upper layers, 
	* depending on TBlockOption. If the upper layers can’t process any more
	* data to stop receiving packets this API is called with EBlockFlow. 
	* Later after the congestion eases, to start receiving packets again call 
	* this API with EUnblockFlow
	
	* @param aOption either block or unblock receive flow
	* @return none.
	*/
	virtual void SetFlowControl(TBlockOption aOption)=0;

	/**
	* The BCA control function to get or set the options of the BCA in an 
	* asynchronous manner.
	
	* @param aStatus  asynchronous completion status, KErrNone if successful, 
         * system-wide error code otherwise
	* @param aOptLevel option level to be used KBcaOptLevelGeneric 
	* or KBcaOptLevelExtSerial
	* @param aOptName option name to be used.
	* @param aOpt an optional parameter,holds the option value on return or the
	* option value to be set.
	* @return none.
	*/
	virtual void Control(TRequestStatus& aStatus,
							TUint aOptLevel,
							TUint aOptName,
							TDes8& aOpt)=0;

	/**
	* Cancels the Control request in a synchronous way.
	
	* @param none
	* @return none.
	*/
	virtual void CancelControl()=0;
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
* Generic Control option name
*/
const TUint KBCAMru	                = 0x12;
const TUint KBCAMtu	                = 0x13;
const TUint KBCASpeedMetric	        = 0x14;
const TUint KBCACaps	            = 0x15;
const TUint KBCASetIapId	        = 0x16;
const TUint KBCASetBcaStack	        = 0x1e;
const TUint KVersionNumber	        = 0x1c;
/** 
Purges the buffers */
const TUint KBCAResetBuffers        = 0x1f;

/**
* Serial Control option name
*/
const TUint KSerialCaps	            = 0x17;
const TUint KSerialConfig	        = 0x18;
const TUint KSerialSetConfig	    = 0x1a;
const TUint KSerialPortName	        = 0x19;
const TUint KSerialSetCsyName	    = 0x1b;
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
Structure associated with KSerialSetSignals control.
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

#endif // BCA2_H
