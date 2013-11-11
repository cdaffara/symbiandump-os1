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

#ifndef MDFINPUTPORT_H
#define MDFINPUTPORT_H

#include <e32base.h>
#include <mdf/mdfcommon.h>

class CMdfProcessingUnit;
class CMMFBuffer;
class MMdfInputPort;
class MMdfOutputPort;
class TPuConfig;

/**
Codec API Input Port observer class.
*/
class MMdfInputPortObserver
	{
public:
	/**
	Called by the Codec API Input Port when MipWriteData() has completed.
	@param  aInputPort
		  The Input Port which sent the callback.
	@param  aBuffer
		  The data buffer which was written.
	@param  aErrorCode
		  An error code indicating if the function call was successful. KErrNone on success, otherwise
	 	  another of the system-wide error codes. 
	*/
	virtual void MipoWriteDataComplete(const MMdfInputPort* aInputPort,
		CMMFBuffer* aBuffer, TInt aErrorCode) = 0;

	/**
	Called by the Codec API Input Port when MipDisconnectTunnel() has completed.
	@param  aInputPort
		  The Input Port which sent the callback.
	@param  aErrorCode
		  An error code indicating if the function call was successful. KErrNone on success, otherwise
	 	  another of the system-wide error codes. 
	*/
	virtual void MipoDisconnectTunnelComplete(const MMdfInputPort* aInputPort,
		TInt aErrorCode) = 0;

	/**
	Called by the Codec API Input Port when MipRestartTunnel() has completed.
	@param  aInputPort
		  The Input Port which sent the callback.
	@param  aErrorCode
		  An error code indicating if the function call was successful. KErrNone on success, otherwise
	 	  another of the system-wide error codes. 
	*/
	virtual void MipoRestartTunnelComplete(const MMdfInputPort* aInputPort,
		TInt aErrorCode) = 0;
	};

/**
Codec API Input Port interface.
*/
class MMdfInputPort  
	{
public:
	/**
	Synchronous function used to configure the Input Port.
	@param  aConfiguration
		  Holds the configuration parameters for the Input Port.
	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	virtual TInt MipConfigure(const TPuConfig&  aConfiguration) = 0;

	/**
	Synchronous method which gets a configuration structure for the Input Port
	@param  aConfigurationSetup
		  The reference to the structure that is to contain the configuration information
	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.	
	*/
	virtual TInt MipGetConfig(TPuConfig& aConfigurationSetup) = 0;

	/**
	Synchronous function used to initialise the Input Port.
	*/
	virtual void MipInitialize() = 0;

	/**
	Synchronous function used to set the observer for the Input Port. 
	@param  aInputPortObserver
		  The observer of the Input Port.
	*/
	virtual void MipSetObserver(const MMdfInputPortObserver& aInputPortObserver) = 0;

	/**
	Synchronous function used to instruct the Input Port to create a buffer. 
	@param  aBufferSize
		  The size of the buffer to be created.
	@return A pointer to the new created buffer.
	*/
	virtual CMMFBuffer* MipCreateBuffer(TInt aBufferSize) = 0;

	/**
	Synchronous function used to instruct the Input Port to use the buffer 
	passed in the function's argument. 
	@param  aBuffer
		  The buffer to be used by the Input Port.
	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	virtual TInt MipUseBuffer(CMMFBuffer& aBuffer) = 0;

	/**
	Synchronous function used to instruct the Input Port to free the buffer given
	passed in the function's argument. 
	@param  aBuffer
		  The buffer to be freed
	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	virtual TInt MipFreeBuffer(CMMFBuffer* aBuffer) = 0;

	/**
	Synchronous function used to request the set up of a tunnel between this Input Port
	and an Output Port.
	@param  aOutputPortToBeConnectedTo
		  Reference to the Output Port to be connected to.
	@param  aTunnelFlags
		  Control flags for tunneling
	@param  aSupplierType
		  Specifies the supplier of the buffers for the tunnel.
	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	virtual TInt MipTunnelRequest(const MMdfOutputPort& aOutputPortToBeConnectedTo,
		TTunnelFlags& aTunnelFlags, TSupplierType& aSupplierType) = 0;

	/**
	Asynchronous function used to write data to the Input Port.
	@param  aBuffer
		  Reference to the buffer containing data.
	*/
	virtual void MipWriteData(CMMFBuffer& aBuffer) = 0;

	/**
	Asynchronous function used to disconnect a tunnelled port, and thus stop the data processing.
	*/
	virtual void MipDisconnectTunnel() = 0;
	
	/**
	Asynchronous function used to restart the data processing of a tunnelled port.
	*/
	virtual void MipRestartTunnel() = 0;

	/**
	Synchronous function used to find out if an Input Port is tunnelled or not.
	@return ETrue if the Input Port is tunnelled, EFalse otherwise.
	*/
	virtual TBool MipIsTunnelled() const = 0;

	/**
	Synchronous function used to get the Input Port index
	@return The Input Port index.
	*/
	virtual TInt MipIndex() const = 0;

	/**
	Synchronous function used to get the size of the buffer(s) used by the Input Port.
	@return The Input Port's buffer size.
	*/
	virtual TUint32 MipBufferSize() const = 0 ;

	/**
	Request extension feature. This is intended to provide additional features. 
	@param  aUid
	        Used to indicate which interface is required. 
	@return Standard error code. KErrNotSupported is used to indicate that the particular
	        plugin is used.
	*/
	virtual TInt MipCreateCustomInterface(TUid aUid) = 0;

	/**
	Return previously created extension.
	This returns a custom interface whose ownership is not transferred to the caller. This
	should only be used if CreateCustomInterface() has already been called for the same UID
	value. This means that any construction for that interface	has already been called, 
	and thus this call cannot fail.
	@param aUid
	       Used to indicate which interface is required. 
	@return The requested interface, or NULL if not known.
	@see MipCreateCustomInterface()
	*/
	virtual TAny* MipCustomInterface(TUid aUid) = 0;
	};

#endif // MDFINPUTPORT_H
