// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @publishedPartner
 @released
*/

#ifndef MDFOUTPUTPORT_H
#define MDFOUTPUTPORT_H

#include <e32base.h>
#include <mdf/mdfcommon.h>

class CMdfProcessingUnit;
class CMMFBuffer;
class MMdfOutputPort;
class MMdfInputPort;
class MMdfInputPortObserver;
class TPuConfig;

/**
Codec API Output Port observer class.
*/
class MMdfOutputPortObserver
	{
public:
	/**
	Called by the Codec API Output Port when MopReadData() has completed.
	@param  aOutputPort
	        The Output Port which sent the callback.
	@param  aBuffer
		  The buffer read by the Output Port.
	@param  aErrorCode
		  An error code indicating if the function call was successful. KErrNone on success, otherwise
	 	  another of the system-wide error codes. 
	*/
	virtual void MopoReadDataComplete(const MMdfOutputPort* aOutputPort,
		CMMFBuffer* aBuffer, TInt aErrorCode) = 0;

	/**
	Called by the Codec API Output Port when MopDisconnectTunnel() has completed.
	@param  aOutputPort
		  The Output Port which sent the callback.
	@param  aErrorCode
		  An error code indicating if the function call was successful. KErrNone on success, otherwise
	 	  another of the system-wide error codes. 
	*/
	virtual void MopoDisconnectTunnelComplete(const MMdfOutputPort* aOutputPort,
		TInt aErrorCode) = 0;

	/**
	Called by the Codec API Output Port when MopRestartTunnel() has completed.
	@param  aOutputPort
		  The Ouput Port which sent the callback.
	@param  aErrorCode
		  An error code indicating if the function call was successful. KErrNone on success, otherwise
	 	  another of the system-wide error codes. 
	*/
	virtual void MopoRestartTunnelComplete(const MMdfOutputPort* aOutputPort,
		TInt aErrorCode) = 0;	
	};

/**
Codec API Output Port interface.
*/
class MMdfOutputPort
	{
public:
	/**
	Synchronous function used to configure the Output Port.
	@param  aConfiguration
		  Holds the configuration parameters for the Output Port.
	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	virtual TInt MopConfigure(const TPuConfig&  aConfiguration) = 0;


	/**
	Synchronous method which gets a configuration structure for the Output Port
	@param  aConfigurationSetup
		  The reference to the structure that is to contain the configuration information
	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.	
	*/
	virtual TInt MopGetConfig(TPuConfig& aConfigurationSetup) = 0;
	

	/**
	Synchronous function used to initialise the Output Port.
	*/
	virtual void MopInitialize() = 0;

	/**
	Synchronous function used to set the observer for the Output Port. 
	@param  aOutputPortObserver
		  The observer of the Output Port.
	*/
	virtual void MopSetObserver(const MMdfOutputPortObserver& aOutputPortObserver) = 0;

	/**
	Synchronous function used to instruct the Output Port to create a buffer. 
	@param  aBufferSize
		  The size of the buffer to be created.
	@return A pointer to the newly created buffer.
	*/
	virtual CMMFBuffer* MopCreateBuffer(TInt aBufferSize) = 0;

	/**
	Synchronous function used to instruct the Output Port to use the buffer 
	passed in the function's argument. 
	@param  aBuffer
		  The buffer to be used by the Output Port.
	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	virtual TInt MopUseBuffer(CMMFBuffer& aBuffer) = 0;

	/**
	Synchronous function used to instruct the Output Port to free the buffer
	passed in the function's argument. 
	@param  aBuffer
		  The buffer to be freed
	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	virtual TInt MopFreeBuffer(CMMFBuffer* aBuffer) = 0;

	/**
	Synchronous function used to request the set up of a tunnel between this Output Port
	and an Input Port.
	@param  aInputPortToBeConnectedTo
		  Reference to the Input Port to be connected to.
	@param  aTunnelFlags
		  Control flags for tunneling
	@param  aSupplierType
		  Specifies the supplier of the buffers for the tunnel.
	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	virtual TInt MopTunnelRequest(const MMdfInputPort& aInputPortToBeConnectedTo,
		TTunnelFlags& aTunnelFlags, TSupplierType& aSupplierType) = 0;

	/**
	Asynchronous function used to read data from the Output Port.
	@param  aBuffer
		  Reference to the buffer containing data.
	*/
	virtual void MopReadData(CMMFBuffer& aBuffer) = 0;

	/**
	Asynchronous function used to disconnect a tunnelled port, and thus stop the data processing.
	*/
	virtual void MopDisconnectTunnel() = 0;

	/**
	Asynchronous function used to restart the data processing of a tunnelled port.
	*/
	virtual void MopRestartTunnel() = 0;

	/**
	Synchronous function used to find out if an Output Port is tunnelled or not.
	@return ETrue if the Input Port is tunnelled, EFalse otherwise.
	*/
	virtual TBool MopIsTunnelled() const = 0;

	/**
	Synchronous function used to get the Output Port index
	@return The Input Port index.
	*/
	virtual TInt MopIndex() const = 0;

	/**
	Synchronous function used to get the size of the buffer(s) used by the Output Port.
	@return The Output Port's buffer size.
	*/
	virtual TUint32 MopBufferSize() const = 0 ;

	/**
	Request extension feature. This is intended to provide additional features. 
	@param  aUid
	        Used to indicate which interface is required. 
	@return Standard error code. KErrNotSupported is used to indicate that the particular
	        plugin is used.
	*/
	virtual TInt MopCreateCustomInterface(TUid aUid) = 0;

	/**
	Return previously created extension.
	This returns a custom interface whose ownership is not transferred to the caller. This
	should only be used if CreateCustomInterface() has already been called for the same UID
	value. This means that any construction for that interface has already been called, 
	and thus this call cannot fail.
	@param aUid
	       Used to indicate which interface is required. 
	@return The requested interface, or NULL if not known.
	@see MopCreateCustomInterface()
	*/
	virtual TAny* MopCustomInterface(TUid aUid) = 0;
	};

#endif // MDFOUTPUTPORT_H
