/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This class defines capability storage for Generid IDs.
*
*/


#ifndef ACCSRVCAPABILITYSTORAGE_H
#define ACCSRVCAPABILITYSTORAGE_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <s32mem.h>
#include <AccPolGenericID.h> 
#include <AccPolGIDHeader.h> 
#include <AccPolValueRecord.h>
#include <AccPolSubblockNameArray.h>

class CCapabilityStorage;

/**
*  Data structure for an individual capability. For accessory server internal use.
*
*  @lib AccServer.lib
*  @since S60 3.1
*/
class CCapValue : public CBase
{
	public:
		/**
        * constructor.
        */
        CCapValue();			
		
		/**
        * Destructor.
        */
		~CCapValue();		
	public:
		 /**
        * Return name of the capability
        *
        * @since S60 3.1
        * @return Name of the capability
        */	
		IMPORT_C TUint32 Name() const;

		/**
        * Set int value of the capability
        *
        * @since S60 3.1
        * @return void
        */	
		IMPORT_C void SetValueL( const TInt32& aNewValue);

		/**
        * Set string value of the capability
        *
        * @since S60 3.1
        * @return void
        */	
		IMPORT_C void SetValueL( const TDesC8& aNewValue);

		/**
        * Return integer or bool value of the capability
        *
        * @since S60 3.1
        * @param aValue Value of the capability
        * @return error code
        */	
		IMPORT_C TInt ValueInt( TInt32& aValue) const;

		/**
        * Return descriptor value of the capability
        *
        * @since S60 3.1
        * @param aValue Value of the capability
        * @return error code
        */	
		IMPORT_C TInt ValueDesc( TPtrC8& aValue) const;

		/**
        * Return value type of the capability
        *
        * @since S60 3.1
        * @return Value type of the capability
        */	
		IMPORT_C TAccPolValueType Type() const;
		

		/**
        * Return locatiom of the capability
        *
        * @since S60 3.1
        * @return location of the capability
        */	
		IMPORT_C TAccPolValueLocation Location() const;
		
	private:	//data
		// 0-5 bits for subblock, 6-10 bits for namespace, 11-15 bits for name
		// bit 16 for location, 17-21 bits for type
		TUint32 iName;		
	
		// capability value
		union
		{
			TInt32 iIntValue;
			TBool iBoolValue;
			HBufC8* iHBufValue;	//owned	            
		};
	private:
		 friend class CCapabilityStorage;
};

/**
*  A capability storage for accessory server internal use.
*
*  @lib AccServer.lib
*  @since S60 3.1
*/

class CCapabilityStorage : public CBase
{
	private:
		typedef RPointerArray<CCapValue> RCapabilityArray;

		/**
		*  Data structure for an capability list. 
		*
		*  @lib AccServer.lib
		*  @since S60 3.1
		*/
		class CListContainer : public CBase
		{
			public:
				CListContainer();
				~CListContainer();
			public: //data
				RCapabilityArray iCapabilityArray;
				TAccPolGenericID iGenericId;		
			private:
		};

		typedef RPointerArray<CListContainer> RCapabilityArrays;

	public:
		/**
        * Two-phased constructor.
        */
        static IMPORT_C CCapabilityStorage* NewL();			
		
		/**
        * Destructor.
        */
		IMPORT_C ~CCapabilityStorage();
	public:
	
    	/**
    	* Generate unique id for specific generic id
  		*
   	 	* @since S60 3.1
	    * @param aGenericID Generic ID 
	    * @return void
	    */
	    IMPORT_C void GenerateUniqueId( TAccPolGenericID& aGenericId );	
	
		 /**
        * Create new capability list for specific Generid ID
        *
        * @since S60 3.1
        * @param aGenericID Generic ID of the list to be created
        * @param aReadStrm Capability stream
        * @return ETrue if connection already exist (so this is update for connection)
        */
		IMPORT_C TBool NewCapabilityListL( const TAccPolGenericID& aGenericId, 
                                           RBufReadStream& aReadStrm );

		 /**
        * Create new capability list for specific Generid ID
        *
        * @since S60 3.1
        * @param aGenericID Generic ID of the list to be created
        * @return void
        */
		IMPORT_C void NewCapabilityListL( const TAccPolGenericID& aGenericId );


		 /**
        * Externalize capability name list
        *
        * @since S60 3.1
        * @param aGenericID Generic ID of the list to be created
        * @param aWriteStrm Capability stream
        * @return void
        */
		IMPORT_C void ExternalizeCapabilityNameListL( const TAccPolGenericID& aGenericId,
                                                      RBufWriteStream& aWriteStrm );
		

    	/**
    	* Remove capability list accordingg to specific Generid ID
  		*
   	 	* @since S60 3.1
	    * @param aGenericID Generic ID of the list to be removed
	    * @return error code
	    */
	    IMPORT_C TInt RemoveCapabilityList( const TAccPolGenericID& aGenericId );


    	/**
    	* Fill generic id header
  		*
   	 	* @since S60 3.1
	    * @param aGenericID Generic ID to be filled (unique id already set)
	    * @return error code
	    */
	    IMPORT_C TInt GetGenericID( TAccPolGenericID& aGenericId );

    	/**
    	* Set updated generic id to be used
  		*
   	 	* @since S60 3.1
	    * @param aGenericID Generic ID to be get and updated
	    * @return void
	    */
	    IMPORT_C void UseUpdatedGenericIDL( const TAccPolGenericID& aGenericId );

    	/**
    	* Get a capability 
  		*
   	 	* @since S60 3.1
	    * @param aGenericID Generic ID of the list to be used
	    * @param aName Name of the capability
	    * @return CCapValue Capability value container
	    */
	    IMPORT_C CCapValue* GetCapability( const TAccPolGenericID& aGenericId, 
                                           const TUint32& aName );

    	/**
    	* Check is capability used in other connections
  		*
   	 	* @since S60 3.1
	    * @param aGenericID Generic ID of the connection
	    * @param aName Name of the capability
	    * @return TBool True if capability used in other connections
	    */
	    IMPORT_C TBool CapabilityInUse( const TAccPolGenericID& aGenericId, 
                                        const TUint32& aName );

    	/**
    	* Set capability for a generic id.
  		*
   	 	* @since S60 3.1
	    * @param aGenericID Generic ID of the connection
	    * @param aName Name of the capability
	    * @param aLocation Location of the capability
	    * @param aValue value of the capability
	    * @param aType type of the capability (EAPVBool, EAPVInt, EAPVUInt)
	    * @return void
	    */
	    IMPORT_C void SetCapabilityL( const TAccPolGenericID& aGenericId, 
	    							   const TUint32& aName, 
	    							   const TAccPolValueLocation& aLocation, 
	    							   const TAccPolValueType& aType, 
	    							   const TInt32& aValue );

    	/**
    	* Set capability for a generic id.
  		*
   	 	* @since S60 3.1
	    * @param aGenericID Generic ID of the connection
	    * @param aName Name of the capability
	    * @return void
	    */
	    IMPORT_C void SetCapabilityL( const TAccPolGenericID& aGenericId, 
	    							   const TUint32& aName );

		 /**
        * Set capabilities for a generic id.
        *
        * @since S60 3.1
        * @param aGenericID Generic ID of the connection
        * @param aReadStrm Capability stream
        * @return void
        */
		void SetCapabilitiesL( const TAccPolGenericID& aGenericId, RBufReadStream& aReadStrm );

    	/**
    	* Remove capabilites 
  		*
   	 	* @since S60 3.1
	    * @param aGenericID Generic ID of the connection
	    * @return void
	    */
	    void RemoveCapabilitiesL( const TAccPolGenericID& aGenericId );

    	/**
    	* Set generic ID
  		*
   	 	* @since S60 3.1
	    * @param aGenericID New generic ID of the connection
	    * @return void
	    */
	    IMPORT_C void SetGenericIDHeaderL( const TAccPolGIDHeader& aGenericHeaderId );

		/**
    	* Set critical capability.
    	* A new object is allways added to the array.
  		*
   	 	* @since S60 3.1
	    * @param aName     Name of the capability
	    * @param aLocation Location of the capability
	    * @param aValue    Value of the capability
	    * @param aType     Type of the capability (EAPVBool, EAPVInt, EAPVUInt)
	    * @return void
	    */
	    IMPORT_C void SetCriticalCapabilityL( const TUint32& aName, 
        	const TAccPolValueLocation& aLocation, 
            const TAccPolValueType& aType, 
            const TInt32& aValue );
                                              
        /**
        * Check if Generic ID instance has critical capabilities defined.
        * The matching cababilities are returned in the given reference.
        *
        * @since S60 3.1
        * @param aGenericID     Generic ID instance to be checked
        * @param aOutputNameArr Matching name(s) are written here
        */
        IMPORT_C void ResolveCriticalCapabilitiesL( const TAccPolGenericID& aGenericID,  
                                                    CAccPolSubblockNameArray& aOutputNameArr );
                      
        /**
        * Check that GenericIDs critical capabilities have correct value.
        * Single connection rule can contain several different values for a single name.
        *
        * @since S60 3.1
        * @param aGenericID GenericID which critical capabilities are checked.
        * @return ETrue if all critical capabilities have correct value, EFalse otherwise.
        */                              
        IMPORT_C TBool CompareCriticalValuesL( const TAccPolGenericID& aGenericID );
                                               
	private:
		/**
        * Constructor.
        */
		CCapabilityStorage();

	private:
	
    	/**
    	* Internalize capability stream to capability array
  		*
   	 	* @since S60 3.1
	    * @param aStream Stream to be internalized
	    * @param aArray Destination array
	    * @param void 
	    */	
		void InternalizeToArrayL( RReadStream& aStream, RCapabilityArray& aArray );	
		

		static TInt OrderFunctionListContainer( CListContainer const& aContainer1, 
                                                CListContainer const& aContainer2 );

		static TInt OrderFunctionCapList( CCapValue const& aCapValue1, 
                                          CCapValue const& aCapValue2 );

		CListContainer* FindContainer( const TAccPolGenericID& aGenericId, 
                                       const RCapabilityArrays& aArray );

		CCapValue* FindCapValue( const CListContainer* aCapabilityList, const TUint32& aName );

		TBool FindCriticalCapValue( TUint32 aName, TInt32 aCapabilityValue );

	private:
		
		//capability array container for generid ids
		RCapabilityArrays iCapabilityArrays;
		
		//capability array container for generid ids
		RCapabilityArrays iUpdatedCapabilityArray;		
		
		//unique id counter
		TInt iUniqueIdCounter;		
		
		//All critical capabilities from AccessoryPolicy DB.
		RCapabilityArray iCriticalCapabilityArray;
					
};

#endif // ACCSRVCAPABILITYSTORAGE_H

// End of file
