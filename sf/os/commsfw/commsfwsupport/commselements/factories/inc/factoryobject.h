// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @released since 9.5
*/

#ifndef SYMBIAN_FACTORYOBJECT_H
#define SYMBIAN_FACTORYOBJECT_H

#include <elements/nm_node.h>

namespace Factories
{
class AFactoryObject;

struct TFactoryObjectEntry
/**
Structure for entry in a factory's object list. 
 */
	{
	/**
	Possible flags for factory object. Only @c EBeingDeleted is recognised by the base factory.
	If a factory object has @c EBeingDeleted set, it won't be given as a response to a create factory object
	request.
	*/
    enum TFlags
        {
   		/**
		User-specific flag range start.
		*/        
        ECustomStart = 0,
   		/**
		User-specific flag range end.
		*/        
        ECustomEnd   = 0x00FF,
   		/**
		The factory is being deleted and is not available for new requests.
		*/
        EBeingDeleted,
        };

	/**
	Constructor
	@param aFactoryObject Pointer to the factory object for this entry
	@param aFlag Flags for this entry
	*/
	TFactoryObjectEntry( AFactoryObject* aFactoryObject, TUint aFlag ) :
		iFactoryObject( aFactoryObject ),
		iFlag(aFlag)
		{
		}

	/**
	Pointer to the actual factory object
	*/
	AFactoryObject* iFactoryObject;

	/**
	Flags for the factory object. These flags are generally factory specific, though @c EBeingDeleted applies to all factories.
	*/
	TUint iFlag;
	};

struct TFactoryObjectInfo
/**
Container for data to be compared against the parameters of a FindObject query.

@see CFactoryBase::FindObject
 */
	{
	/**
	Constructor
	@param aInfo Factory object entry to be compared
	*/
	TFactoryObjectInfo( const TFactoryObjectEntry& aInfo ) :
		iInfo( aInfo ),
		iSelectedFactoryObject(aInfo.iFactoryObject)
		{
		}

	/**
	Factory object entry to be used in the comparison
	*/
	const TFactoryObjectEntry& iInfo;

	/**
	If the comparison returns @c EMatch, this is the matched factory object
	*/
	AFactoryObject* iSelectedFactoryObject;
	};

class MFactoryQuery
/**	Abstract class used as a parameter for CFactoryBase::FindObject. Implements the
	@c Match method that derivees must override to implement their own @c Match method to
	compare @c AFactoryObjects

@see CFactoryBase::FindObject

*/
	{
public:
	/**
	Possible return to the match function. They specify what the object doing the search should do next.
	 */
	enum TMatchResult
		{
		/**
		Match found, stop searching
		*/
		EMatch,
		/**
		No match found, continue searching
		*/
		EContinue,
		/**
		No match found, stop searching
		*/
		ECancel
		};
public:

	/**
	Compare this factory object with another factory object.
	@param aObjectInfo Object to compare parameters against. Criteria for comparison is implementation specific.
	@return Result of the match
	@see TMatchResult
	 */
	virtual TMatchResult Match( TFactoryObjectInfo& aObjectInfo ) = 0;
	};

class CFactoryBase;

class AFactoryObject
/** Factory Object Base class providing basic managed objects functionality
	in conjunction with CFactoryBase.

*/
	{
	friend class RPointerArray<AFactoryObject>;
	friend class CFactoryBase;

public:
	/**
	Delete the factory object. Should only be called by CFactoryBase.
	*/
	IMPORT_C void DeleteMeNow();
	
	/**
	Mark this factory object for deletion. Once marked for deletion, it will not be given to clients
	as a result of a find or create factory object request.
	 */
	IMPORT_C void MarkMeForDeletion();

	/**
	Get the factory which owns this factory object
	@return The factory which owns this factory object
	*/
	CFactoryBase& Factory() const;

protected:
	/**	Constructor for AFactoryObject
	@param aFactory The factory which owns this factory object
	@see CFactoryBase */
	IMPORT_C AFactoryObject(CFactoryBase& aFactory);

protected:
	// Don't allow clients to invoke the destructor.
	// (Only the CFactoryBase should do this)
	IMPORT_C virtual ~AFactoryObject();

private:
	mutable CFactoryBase& iFactory;
	};

inline CFactoryBase& AFactoryObject::Factory() const
	{
	return iFactory;
	}

} //namespace Factories

#endif
//SYMBIAN_FACTORYOBJECT_H

