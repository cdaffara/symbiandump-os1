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
 @publishedAll
 @released
*/

#ifndef REMCONINTERFACEBASE_H
#define REMCONINTERFACEBASE_H

#include <e32base.h>
#include <remcon/clienttype.h>
#include <remcon/remconinterfacefeatures.h>

class CRemConInterfaceSelector;
class CRemConInterfaceBaseExtension;

/**
Base class for outer layer APIs. 
*/
class CRemConInterfaceBase : public CBase
	{
public: 
	/** Destructor */
	IMPORT_C ~CRemConInterfaceBase();

public: // called by the ultimate client
	/** 
	Cancels the currently outstanding asynchronous send request. 
	@return KErrNone.
	*/
	IMPORT_C TInt Cancel();

public: // called by the interface selector
	/**
	Called by the interface selector to get a pointer to an object which 
	implements the interface API with UID aUid. This is a mechanism for 
	allowing future change to the interface API without breaking BC in 
	existing (non-updated) interfaces.
	*/
	virtual TAny* GetInterfaceIf(TUid aUid) = 0;

	/** 
	Accessor for the interface's UID. 
	@return The interface's UID.
	*/
	TUid InterfaceUid() const;

	/** 
	Accessor for the maximum length of operation-specific data in this 
	interface. 
	@return The size in bytes of a buffer needed to hold the largest single 
	payload of operation-associated data in this interface.
	*/
	TUint MaxLength() const;

	/**
	Accessor for the type of the interface.
	@return The interface type.
	*/
	TRemConClientType Type() const;
	
	/**
	Determine whether this is a bulk or control interface.
	@return ETrue if this interface is a bulk interface
			EFalse if this interface is a control interface
	*/
	TBool Bulk() const;
	
	/**
	Determine whether this is a target or controller interface.
	@return ETrue if this interface is a target interface
			EFalse if this interface is a controller interface
	*/
	static TBool Target(TRemConClientType aType);

protected:
	/** 
	Constructor.
	@param aInterfaceUid The UID of the interface.
	@param aMaxLength The maximum length of any operation-specific data in the 
	interface being registered.
	@param aInterfaceSelector The interface selector.
	*/
	IMPORT_C CRemConInterfaceBase(TUid aInterfaceUid, 
		TUint aMaxLength, 
		CRemConInterfaceSelector& aInterfaceSelector,
		TRemConClientType aType);

	/** Must be called by concrete types at construction time. Registers the 
	interface with the interface selector. */
	IMPORT_C void BaseConstructL();

	/** Must be called by concrete types at construction time. Registers the 
	interface with the interface selector.
	@param aRemConInterfaceFeatures Features associocated with the interface
	*/
	IMPORT_C void BaseConstructL(RRemConInterfaceFeatures& aRemConInterfaceFeatures);
	
	/** Must be called by concrete types at construction time. Registers the 
		interface with the interface selector.
	@param aRemConInterfaceFeatures Features associated with the interface
	@param aBulkInterface Specifies whether this interface is a control or bulk interface
	Only available for use by Outer Layer interfaces.
	*/
	IMPORT_C void BaseConstructL(RRemConInterfaceFeatures& aRemConInterfaceFeatures, TBool aBulkInterface);
	
	/** 
	Accessor for the interface selector. 
	@return The interface selector.
	*/
	IMPORT_C CRemConInterfaceSelector& InterfaceSelector();

private: // owned
	/** The UID of the concrete (derived) interface. */
	TUid iInterfaceUid; 

	/** Max length of operation-specific data in this interface. */
	TUint iMaxLength; 

	/** The type of the interface- controller or target. */
	const TRemConClientType iType;

	/** 
	Extension instance for RemCon Interface Base.
	*/
	CRemConInterfaceBaseExtension* iExtension;

private: // unowned
	CRemConInterfaceSelector& iInterfaceSelector;
	};

#endif // REMCONINTERFACEBASE_H
