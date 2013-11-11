/*
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
*
*/
// This contains namespace USB

#ifndef USBCONSTANTS_H
#define USBCONSTANTS_H

/**
 * Namespace for constant variables and data types, common to USBerian 
 */
namespace USB
	{
	/** 
	 * USB logical device driver name 
	 * */
	#ifdef __WINS__
		_LIT(KLDDName, "testusbc");
	#else
		_LIT(KLDDName, "eusbc");
	#endif
	
	/** 
	 * Endpoint type constants - Short descriptions of types 
	(see USB 2.0 specification for details) */
	
	/** 
	Control
		"Control transfers are intended to support configuration/command/status 
		type communication flows between client software and its function. Each 
		USB device is required to implement the Default Control Pipe as a message pipe."
	*/
	_LIT(KEPControl, "Control");
	
	/**
	Bulk
		"The bulk transfer type is designed to support devices that need to communicate 
		relatively large amounts of data at highly variable times where the transfer can 
		use any available bandwidth."
	*/
	_LIT(KEPBulk, "Bulk");
	
	/**
	Interrupt
		"The interrupt transfer type is designed to support those devices that need to 
		send or receive data infrequently but with bounded service periods."
	*/
	_LIT(KEPInterrupt, "Interrupt");
	/*
	Isochronous 
		"Isochronous transfer type provides the requester:
		 - guaranteed access to USB bandwidth with bounded latency
		 - guaranteed constant data rate through the pipe as long as data is provided to the pipe
		 - in the case of a delivery failure due to error, no retrying of the attempt to deliver the data "
	*/
	_LIT(KEPIsochronous, "Isochronous");
	
	/**
	 *  Endpoint direction constants 
	 * */

	_LIT(KEPIn, "In"); 		// "IN refers to transfers to the host"
	_LIT(KEPOut, "Out"); 	// "OUT refers to transfers from the host."
	_LIT(KEPBidirectional, "Bidirectional"); // only for control transfers
	
	/** 
	 * The following are default offsets data in device descriptor 
	Offsets are used to set */
	const TInt KSpecOffset = 2;
	const TInt KVendorOffset = 8;
	const TInt KProductIDOffset = 10;
	const TInt KReleaseOffset = 12;
	
	/* Default number of channels to open. Used only to initialize variable with proper value */
	const TInt MinNumberOfChannels = 1;
	
	/* Value which is not used by USB client driver, "For future compatibility, should always be zero." */
	const TInt KUSBChannelOpenParameter = 0;
	
	/* Value for how long to wait before test case ends after re-enumeration */
	const TInt KSecond = 1000000; // 1 secs = 1000000 microseconds
	
	enum TUSBEndPointMode {
		ENotChosen = -1,
		EBulk,
		EInterrupt,
		EIsochronous
	};
	
	/** 
	* USB Actions indicate which action is done in async call
	* The flag is set before async calls to RDevUsbcClient
	* It is used by USBReader and USBWriter classes
	*/
	
	enum USBAction
		{
		
		// Read Actions - used by USBReader
		ENone, // ENone Used by both USBReader and USBWriter
		ERead,		
		EReadWithLength, 
		EReadUntilShort,
		EReadUntilShortWithLength,
		EReadOneOrMore,
		EReadOneOrMoreWithLength,
		EReadError, 
		
		// Write Actions - used by USBWriter
		EWrite,
		EWriteFile,
		EWriteError

		};
	
	// Read and write buffers 0.5MB by default
	const TInt KDefaultReadBufferSize = 1024 * 512; 
	const TInt KDefaultWriteBufferSize = 1024 * 512; 
	
	const TInt KDelayBetweenDataAvailabilityQueries = 1000000;
	
	const TInt KTransferDataSize5MB = 5242880;
	const TInt KMicroSecondsToSecondsMultiplier = 1000000;
	}



#endif

