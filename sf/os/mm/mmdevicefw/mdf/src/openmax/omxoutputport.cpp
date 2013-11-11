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

#include <omxoutputport.h>
#include <omxprocessingunit.h>
#include <mdf/mdfpuconfig.h>
#include "omxcomponentbody.h"
#include "omxoutputportbody.h"

_LIT(KOmxOutputPort, "OmxOutputPort");

/**
Constructor
*/
EXPORT_C COmxOutputPort::COmxOutputPort()
	{
	}

/**
Destructor
*/	
EXPORT_C COmxOutputPort::~COmxOutputPort()
	{
	delete iBody;
	}	
	
/**
Called by an Input Port when MipWriteData() has completed.
@param  aInputPort
		The Input Port which sent the callback.
@param  aBuffer
		The data buffer which was written.
@param  aErrorCode
		An error code indicating if the function call was successful. KErrNone on success, otherwise
	 	another of the system-wide error codes. 
*/
EXPORT_C void COmxOutputPort::MipoWriteDataComplete(const MMdfInputPort* aInputPort,
		CMMFBuffer* aBuffer, TInt aErrorCode)
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxOutputPort, EBodyNotCreated));
	iBody->MipoWriteDataComplete(aInputPort, aBuffer, aErrorCode);
	}

/**
Called by an Input Port when MipDisconnectTunnel() has completed.
@param  aInputPort
		The Input Port which sent the callback.
@param  aErrorCode
		An error code indicating if the function call was successful. KErrNone on success, otherwise
	 	another of the system-wide error codes. 
*/
EXPORT_C void COmxOutputPort::MipoDisconnectTunnelComplete(const MMdfInputPort* aInputPort,
		TInt aErrorCode)
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxOutputPort, EBodyNotCreated));
	iBody->MipoDisconnectTunnelComplete(aInputPort, aErrorCode);
	}	
		
/**
Called by an Input Port when MipRestartTunnel() has completed.
@param  aInputPort
		The Input Port which sent the callback.
@param  aErrorCode
		An error code indicating if the function call was successful. KErrNone on success, otherwise
	 	another of the system-wide error codes. 
*/
EXPORT_C void COmxOutputPort::MipoRestartTunnelComplete(const MMdfInputPort* aInputPort,
		TInt aErrorCode)
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxOutputPort, EBodyNotCreated));
	iBody->MipoRestartTunnelComplete(aInputPort, aErrorCode);
	}

/**
ConstructL. Must be used by derived class to create the component
*/
EXPORT_C void COmxOutputPort::ConstructL(TInt aIndex, COmxProcessingUnit* aComponent)
	{
	__ASSERT_ALWAYS(!iBody, User::Panic(KOmxOutputPort, EBodyNotCreated));
	iBody = CBody::NewL(aIndex, aComponent, this);
	}


/**
Synchronous function used to set the observer for the OpenMAX Output Port. 
@param  aOutputPortObserver
The observer of the Output Port.
*/
EXPORT_C void COmxOutputPort::MopSetObserver(const MMdfOutputPortObserver& aOutputPortObserver) 
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxOutputPort, EBodyNotCreated));
	iBody->MopSetObserver(aOutputPortObserver);
	}

/**
Synchronous function used to configure the OpenMAX Output Port.
@param  aConfig
		Holds the configuration parameters for the Output Port.
@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	    another of the system-wide error codes.
*/
EXPORT_C TInt COmxOutputPort::MopConfigure(const TPuConfig&  aConfig)
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxOutputPort, EBodyNotCreated));
	return iBody->MopConfigure(aConfig);	
	}
	

/**
Synchronous method which gets a configuration structure for the Output Port
@param  aConfig
		The reference to the structure that is to contain the configuration information
@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	    another of the system-wide error codes.	
*/
EXPORT_C TInt COmxOutputPort::MopGetConfig(TPuConfig& aConfig)
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxOutputPort, EBodyNotCreated));
	return iBody->MopGetConfig(aConfig);	
	}		

	
/**
Synchronous function used to initialise the OpenMAX Output Port.
*/	
EXPORT_C void COmxOutputPort::MopInitialize()	
 	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxOutputPort, EBodyNotCreated));
 	iBody->MopInitialize();
 	}
	

/**
Synchronous function used to instruct the Output Port to create a buffer. 
@param  aBufferSize
		The size of the buffer to be created.
@return A pointer to the newly created buffer.
*/		
EXPORT_C CMMFBuffer* COmxOutputPort::MopCreateBuffer(TInt aBufferSize)
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxOutputPort, EBodyNotCreated));
	return iBody->MopCreateBuffer(aBufferSize);
	}


/**
Synchronous function used to instruct the OpenMAX Output Port to use the buffer 
passed in the function's argument. 
@param  aBuffer
		The buffer to be used by the OpenMAX Output Port.
@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	    another of the system-wide error codes.
*/	
EXPORT_C TInt COmxOutputPort::MopUseBuffer(CMMFBuffer& aBuffer)
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxOutputPort, EBodyNotCreated));
	return iBody->MopUseBuffer(aBuffer);	
	}
	

/**
Synchronous function used to instruct the OpenMAX Output Port to free the buffer
passed in the function's argument. 
@param  aBuffer
		The buffer to be freed
@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	    another of the system-wide error codes.
*/
EXPORT_C TInt COmxOutputPort::MopFreeBuffer(CMMFBuffer* aBuffer)
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxOutputPort, EBodyNotCreated));
	return iBody->MopFreeBuffer(aBuffer);	
	}
	 
	 
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
EXPORT_C TInt COmxOutputPort::MopTunnelRequest(const MMdfInputPort& aInputPortToBeConnectedTo,
		TTunnelFlags& aTunnelFlags, TSupplierType& aSupplierType) 
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxOutputPort, EBodyNotCreated));
	return iBody->MopTunnelRequest(aInputPortToBeConnectedTo, aTunnelFlags, aSupplierType);
	}


/**
Asynchronous function used to read data from the Output Port.
@param  aBuffer
		Reference to the buffer containing data.
*/
EXPORT_C void COmxOutputPort::MopReadData(CMMFBuffer& aBuffer)
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxOutputPort, EBodyNotCreated));
	iBody->MopReadData(aBuffer);	
	}


// used only for tunnelling
/**
Asynchronous function used to disconnect a tunnelled port, and thus stop the data processing.
*/
EXPORT_C void COmxOutputPort::MopDisconnectTunnel()
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxOutputPort, EBodyNotCreated));
	iBody->MopDisconnectTunnel();
	}

/**
Asynchronous function used to restart the data processing of a tunnelled port.
*/
EXPORT_C void COmxOutputPort::MopRestartTunnel()
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxOutputPort, EBodyNotCreated));
	iBody->MopRestartTunnel();
	}


/**
Synchronous function used to find out if an Output Port is tunnelled or not.
@return ETrue if the Input Port is tunnelled, EFalse otherwise.
*/
EXPORT_C TBool COmxOutputPort::MopIsTunnelled() const
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxOutputPort, EBodyNotCreated));
	return iBody->MopIsTunnelled();
	}


/**
Synchronous function used to get the Output Port index
@return The Input Port index.
*/
EXPORT_C TInt COmxOutputPort::MopIndex() const
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxOutputPort, EBodyNotCreated));
	return iBody->MopIndex();
	}
	 
/**
Synchronous function used to get the size of the buffer(s) used by the Output Port.
@param The Output Port's buffer size.
*/
EXPORT_C TUint32 COmxOutputPort::MopBufferSize() const
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxOutputPort, EBodyNotCreated));
	return iBody->MopBufferSize();
	}


/**
Request extension feature. This is intended to provide additional features. 
@param  aUid
	    Used to indicate which interface is required. 
@return Standard error code. KErrNotSupported is used to indicate that the particular
	    plugin is not used.
*/
EXPORT_C TInt COmxOutputPort::MopCreateCustomInterface(TUid aUid)
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxOutputPort, EBodyNotCreated));
	return iBody->MopCreateCustomInterface(aUid);
	}

/**
Return previously created extension.
This returns a custom interface. This should only be used if CreateCustomInterface() has already
been called for the same UID value. This means that any construction for that interface
has already been called, and thus this call cannot fail.
@param aUid
	   Used to indicate which interface is required. 
@return The requested interface, or NULL if not known.
@see MipCreateCustomInterface()
*/
EXPORT_C TAny* COmxOutputPort::MopCustomInterface(TUid aUid)
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxOutputPort, EBodyNotCreated));
	return iBody->MopCustomInterface(aUid);	
	}

/**
Synchronous function used to return the observer for the OpenMAx Output Port.
@return Pointer to this class obsever
*/		
EXPORT_C MMdfOutputPortObserver* COmxOutputPort::Observer() const
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxOutputPort, EBodyNotCreated));
	return iBody->Observer();
	}
	
/**
Synchronous function used to return the OpenMAX Processing Unit this Output Port
belongs to. 
@return Pointer to the OpenMAX Processing Unit this Output Port belongs to. 
*/
EXPORT_C COmxProcessingUnit* COmxOutputPort::Component() const
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxOutputPort, EBodyNotCreated));
	return iBody->Component();
	}
	
	
