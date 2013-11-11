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

#include <omxprocessingunit.h>
#include <omxinputport.h>
#include <mdf/mdfpuconfig.h>
#include "omxcomponentbody.h"
#include "omxinputportbody.h"

_LIT(KOmxInputPort, "OmxInputPort");

/**
Constructor.
*/
EXPORT_C COmxInputPort::COmxInputPort()
	{
	}

/**
Destructor.
*/
EXPORT_C COmxInputPort::~COmxInputPort()
	{
	delete iBody;
	}
	
/**
Class constructor.
*/
EXPORT_C void COmxInputPort::ConstructL(TInt aIndex, COmxProcessingUnit* aComponent)
	{
	__ASSERT_ALWAYS(!iBody, User::Panic(KOmxInputPort, EBodyNotCreated));
	iBody = CBody::NewL(aIndex, aComponent, this);
	}

	
/**
Synchronous function used to configure the OpenMAX Input Port.
@param  aConfiguration
		Holds the configuration parameters for the OpenMAX Input Port.
@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	    nother of the system-wide error codes.
*/
EXPORT_C TInt COmxInputPort::MipConfigure(const TPuConfig& aConfig)
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxInputPort, EBodyNotCreated));
	return iBody->MipConfigure(aConfig);	
	}	
	

/**
Synchronous method which gets a configuration structure for the OpenMAX Input Port
@param  aConfig
		The reference to the structure that is to contain the configuration information.
@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	    another of the system-wide error codes.	
*/
EXPORT_C TInt COmxInputPort::MipGetConfig(TPuConfig& aConfig)
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxInputPort, EBodyNotCreated));
	return iBody->MipGetConfig(aConfig);	
	}	


/**
Synchronous function used to initialise the OpenMAX Input Port.
*/
EXPORT_C void COmxInputPort::MipInitialize()
	{	
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxInputPort, EBodyNotCreated));
	iBody->MipInitialize();
	}


/**
Synchronous function used to instruct the OpenMAX Input Port to create a buffer. 
@param  aBufferSize
		The size of the buffer to be created.
@return A pointer to the new created buffer.
*/
EXPORT_C CMMFBuffer* COmxInputPort::MipCreateBuffer(TInt aBufferSize)
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxInputPort, EBodyNotCreated));
	return iBody->MipCreateBuffer(aBufferSize);
	}


/**
Synchronous function used to instruct the OpenMAX Input Port to use the buffer 
passed in the function's argument. 
@param  aBuffer
		The buffer to be used by the Input Port.
@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	    another of the system-wide error codes.
*/
EXPORT_C TInt COmxInputPort::MipUseBuffer(CMMFBuffer& aBuffer)
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxInputPort, EBodyNotCreated));
	return iBody->MipUseBuffer(aBuffer);		
	}


/**
Synchronous function used to instruct the OpenMAX Input Port to free the buffer given
passed in the function's argument. 
@param  aBuffer
		The buffer to be freed
@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	    another of the system-wide error codes.
*/	
EXPORT_C TInt COmxInputPort::MipFreeBuffer(CMMFBuffer* aBuffer)
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxInputPort, EBodyNotCreated));
	return iBody->MipFreeBuffer(aBuffer);		
	}


/**
Synchronous function used to request the set up of a tunnel between this OpenMAX Input Port
and an OpenMAX Output Port.
@param  aOutputPortToBeConnectedTo
		Reference to the Output Port to be connected to.
@param  aTunnelFlags
		Control flags for tunneling
@param  aSupplierType
		Specifies the supplier of the buffers for the tunnel.
@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	    another of the system-wide error codes.
*/
EXPORT_C TInt COmxInputPort::MipTunnelRequest(const MMdfOutputPort& aOutputPortToBeConnectedTo,
		TTunnelFlags& aTunnelFlags, TSupplierType& aSupplierType) 
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxInputPort, EBodyNotCreated));
	return iBody->MipTunnelRequest(aOutputPortToBeConnectedTo, aTunnelFlags, aSupplierType);
	}
	

/**
Asynchronous function used to write data to the OpenMAX Input Port.
@param  aBuffer
		Reference to the buffer containing data.
*/		 
EXPORT_C void COmxInputPort::MipWriteData(CMMFBuffer& aInputBuffer)
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxInputPort, EBodyNotCreated));
	iBody->MipWriteData(aInputBuffer);	
	}


/**
Asynchronous function used to disconnect a tunnelled port, and thus stop the data processing.
*/
EXPORT_C void COmxInputPort::MipDisconnectTunnel()
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxInputPort, EBodyNotCreated));
	iBody->MipDisconnectTunnel();
	}

/**
Asynchronous function used to restart the data processing of a tunnelled port.
*/		 
EXPORT_C void COmxInputPort::MipRestartTunnel()
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxInputPort, EBodyNotCreated));
	iBody->MipRestartTunnel();
	}		

	
/**
Synchronous function used to find out if an OpenMAX Input Port is tunnelled or not.
@return ETrue if the Input Port is tunnelled, EFalse otherwise.
*/		
EXPORT_C TBool COmxInputPort::MipIsTunnelled() const
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxInputPort, EBodyNotCreated));
	return iBody->MipIsTunnelled();
	}


/**
Synchronous function used to get the OpenMAX Input Port index
@return The OpenMAX Input Port index.
*/
EXPORT_C TInt COmxInputPort::MipIndex() const
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxInputPort, EBodyNotCreated));
	return iBody->MipIndex();
	}
	
	 
/**
Synchronous function used to get the size of the buffer(s) used by the OpenMAX Input Port.
@param The OpenMAX Input Port's buffer size.
*/		 
EXPORT_C TUint32 COmxInputPort::MipBufferSize() const
	{
	return iBody->MipBufferSize();
	}


/**
Synchronous function used to set the observer for the OpenMAX Input Port. 
@param  aOutputPortObserver
		The observer of the OpenMAX Input Port.
*/
EXPORT_C void COmxInputPort::MipSetObserver(const MMdfInputPortObserver& aOutputPortObserver)
	{
	iBody->MipSetObserver(aOutputPortObserver);
	}

/**
Request extension feature. This is intended to provide additional features. 
@param  aUid
	    Used to indicate which interface is required. 
@return Standard error code. KErrNotSupported is used to indicate that the particular
	    plugin is not used.
*/
EXPORT_C TInt COmxInputPort::MipCreateCustomInterface(TUid aUid)
	{
	return iBody->MipCreateCustomInterface(aUid);	
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
EXPORT_C TAny* COmxInputPort::MipCustomInterface(TUid aUid)
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxInputPort, EBodyNotCreated));
	return MipCustomInterface(aUid);
	}
	
/**
Synchronous function used to return the observer for the OpenMAx Input Port.
@return Pointer to this class obsever
*/	
EXPORT_C MMdfInputPortObserver* COmxInputPort::Observer() const
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxInputPort, EBodyNotCreated));
	return iBody->Observer();
	}

/**
Synchronous function used to return the OpenMAX Processing Unit this Input Port
belongs to. 
@return Pointer to the OpenMAX Processing Unit this Input Port belongs to. 
*/	
EXPORT_C COmxProcessingUnit* COmxInputPort::Component() const
	{
	__ASSERT_ALWAYS(iBody, User::Panic(KOmxInputPort, EBodyNotCreated));
	return iBody->Component();
	}
