// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef REMCONCONVERTERPLUGIN_H
#define REMCONCONVERTERPLUGIN_H

#include <e32base.h>
#include <remcon/messagetype.h>

/** 
The UID of the RemCon converter interface. Implementors of new converters are 
implementing this interface. The converter interface is simply a means of 
supplying implementations of the 'converter API' to RemCon. The converter API 
may change over time, so this extension pattern is used.
*/
const TInt KRemConConverterInterfaceUid = 0x10205C43;

/**
Base class for RemCon converter plugins.
*/
class CRemConConverterPlugin : public CBase
	{
public:
	/** 
	Constructor.
	@param aImplementationUid The implemention UID.
	@return Ownership of a new converter plugin.
	*/
	IMPORT_C static CRemConConverterPlugin* NewL(TUid aImplementationId);

	/** Destructor. */
	IMPORT_C ~CRemConConverterPlugin();

public:
	/**
	Called by RemCon server to get a pointer to an object which implements the 
	converter API with UID aUid. This is a mechanism for allowing future 
	change to the converter API without breaking BC in existing (non-updated) 
	converter plugins.
	*/
	virtual TAny* GetInterface(TUid aUid) = 0;

protected:
	/** Constructor
	@param aObserver To be notified of the completion of requests.
	@param aImplementationId Identifier of the concrete implementation.
	*/
	IMPORT_C CRemConConverterPlugin();
	
private: // owned
	/**
	UID set by ECOM when the instance is created. Used when the instance is 
	destroyed.
	*/
	TUid iInstanceId;

	/** 
	Pad for BC-friendly future change.
	*/
	TAny* iPad;
	};

#endif // REMCONCONVERTERPLUGIN_H
