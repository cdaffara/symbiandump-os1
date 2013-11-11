/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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


// INCLUDE FILES
#include "AccPolCapabilityStorage.h"
#include "AccPolGenericIDAccessor.h"
#include "AccPolSubblockNameArrayAccessor.h"
#include "acc_debug.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CCapValue::CCapValue
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCapValue::CCapValue()
    {
    }

// -----------------------------------------------------------------------------
// CCapValue::CCapValue
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCapValue::~CCapValue()
    {   
	if ( Type() == EAPVDes8  ||  Type() == EAPVObject )
	    {
	    if( iHBufValue )
	        {
	    	delete iHBufValue;			
	    	}		
	    }
    }

// -----------------------------------------------------------------------------
// CCapValue::Name
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CCapValue::Name() const
    {
	return iName & 0xFFFF;		// 16 LSB
    }

// -----------------------------------------------------------------------------
// CCapValue::SetValueL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCapValue::SetValueL( const TInt32& aNewValue )
    {
	if ( !( Type() == EAPVBool || Type() == EAPVInt || Type() == EAPVUInt ) )
    	{
		User::Leave( KErrArgument );
	    }
	iIntValue = aNewValue;
    }

// -----------------------------------------------------------------------------
// CCapValue::SetValueL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCapValue::SetValueL( const TDesC8& aNewValue )
    {
	if ( !( Type() == EAPVDes8 || Type() == EAPVObject ) )
        {
		User::Leave( KErrArgument );
	    }

	delete iHBufValue;
    iHBufValue = 0;			//Assigned to NULL in case of aNewValue.AllocL() leaves.
	iHBufValue = aNewValue.AllocL(); 
    }

// -----------------------------------------------------------------------------
// CCapValue::ValueInt
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCapValue::ValueInt( TInt32& aValue ) const
    {
	if ( !( Type() == EAPVBool || Type() == EAPVInt || Type() == EAPVUInt ) )
    	{
		return KErrArgument;
	    }
	aValue = iIntValue;	
	return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCapValue::ValueDesc
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCapValue::ValueDesc( TPtrC8& aValue ) const
    {
	if ( Type() != EAPVDes8 && Type() != EAPVObject )
	    {
		return KErrArgument;	
	    }
	aValue.Set( *iHBufValue );
	return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCapValue::Type
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TAccPolValueType CCapValue::Type() const
    {
	// 17-21 bits for type
	TUint temp = ( iName & 0x3E0000 );
	temp = temp  >> 17;

	return ( TAccPolValueType )( temp );	
    }

// -----------------------------------------------------------------------------
// CCapValue::Location
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//		
EXPORT_C TAccPolValueLocation CCapValue::Location() const
    {
	if ( Type() == EAPVNone )
	    {
		//no value defined -> no location defined...
		return EAPVNotDefined;
	    }

	// bit 16 for location
	TUint temp = iName & 0x10000;

	if ( temp )
	    {
		return EAPVAccessory;
	    }

	return EAPVPolicy;		
    }
        
// -----------------------------------------------------------------------------
// CCapabilityStorage::CAccSrvSettingsHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCapabilityStorage::CCapabilityStorage() : 
    iUniqueIdCounter(1)
    {
	API_TRACE_( "[ACCPOLICY] CCapabilityStorage::CCapabilityStorage()" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubAsyComms::NewL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CCapabilityStorage* CCapabilityStorage::NewL()
    {	
	API_TRACE_( "[ACCPOLICY] CCapabilityStorage::NewL()" );
	API_TRACE_( "[ACCPOLICY] CCapabilityStorage::NewL() - return new CCapabilityStorage instance" );
	return new ( ELeave ) CCapabilityStorage();
    }

// Destructor
EXPORT_C CCapabilityStorage::~CCapabilityStorage()
    {
	API_TRACE_( "[ACCPOLICY] CCapabilityStorage::~CCapabilityStorage()" );

	iCapabilityArrays.ResetAndDestroy();
	iCapabilityArrays.Close();

	iUpdatedCapabilityArray.ResetAndDestroy();
	iUpdatedCapabilityArray.Close();		

  iCriticalCapabilityArray.ResetAndDestroy();
  iCriticalCapabilityArray.Close();

	API_TRACE_( "[ACCPOLICY] CCapabilityStorage::~CCapabilityStorage() - return void" );
    }

// -----------------------------------------------------------------------------
// CCapabilityStorage::NewCapabilityListL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CCapabilityStorage::NewCapabilityListL( 
    const TAccPolGenericID& aGenericId, RBufReadStream& aReadStrm )
    {
	API_TRACE_1( "[ACCPOLICY] CCapabilityStorage::NewCapabilityListL() - New capability list for connection (unique id %d)", aGenericId.UniqueID() );

	TBool retVal = EFalse;

	//get a container 
	CListContainer * container = FindContainer( aGenericId, iCapabilityArrays );

	if ( container )
    	{
		API_TRACE_( "[ACCPOLICY] CCapabilityStorage::NewCapabilityListL() - Connection with unique id already exist - This is update" );

		//this update for connection
		retVal = ETrue;

		//get updated container
		CListContainer * container2 = FindContainer( aGenericId, iUpdatedCapabilityArray );

		if ( !container2 )
		    {
			//create new container to update container list
			container2 = new (ELeave) CListContainer();
			CleanupStack::PushL( container2 );		

			//set unique id and internalize caps from stream
			container2->iGenericId = aGenericId;

			//insert in container list
			TLinearOrder<CListContainer> linearOrder( 
                &CCapabilityStorage::OrderFunctionListContainer );

			iUpdatedCapabilityArray.InsertInOrderL( container2, linearOrder );
			CleanupStack::Pop( container2 );
			container = container2;
		    }
		else
		    {
			//remove old updates items
			iUpdatedCapabilityArray.ResetAndDestroy();
	    	}
	    }
	else
    	{
		//create new container
		container = new (ELeave) CListContainer();
		CleanupStack::PushL( container );		

		//set unique id and internalize caps from stream
		container->iGenericId = aGenericId;

		//insert in container list
		TLinearOrder<CListContainer> linearOrder( 
            &CCapabilityStorage::OrderFunctionListContainer );

		iCapabilityArrays.InsertInOrderL( container, linearOrder );    
		CleanupStack::Pop( container );	
    	}

	InternalizeToArrayL( aReadStrm, container->iCapabilityArray );

	API_TRACE_1( "[ACCPOLICY] CCapabilityStorage::RemoveCapabilityList() - Capability container count %d",  iCapabilityArrays.Count());	
	API_TRACE_( "[ACCPOLICY] CCapabilityStorage::NewCapabilityListL() - return void");

	return retVal;
    }

// -----------------------------------------------------------------------------
// CCapabilityStorage::GetGenericID 
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCapabilityStorage::GetGenericID( TAccPolGenericID& aGenericId )
    {
	//get a container 
	CListContainer * container = FindContainer( aGenericId, iCapabilityArrays );
	//copy container
	if(!container)
        {
        //generic id is not found
        API_TRACE_( "[ACCPOLICY] CCapabilityStorage::GetGenericID() - Unique id not found" );		
        return KErrNotFound;
        }
    aGenericId = container->iGenericId;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCapabilityStorage::UseUpdatedGenericIDL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCapabilityStorage::UseUpdatedGenericIDL( const TAccPolGenericID& aGenericId )
    {
	//get a container 
	CListContainer * updatedContainer = FindContainer( aGenericId, iUpdatedCapabilityArray );

	if ( !updatedContainer )
	    {
		User::Leave( KErrNotFound );
    	}

	//find generic id list...
	TLinearOrder<CListContainer> linearOrder( &CCapabilityStorage::OrderFunctionListContainer );
	CListContainer refContainer;
	refContainer.iGenericId = aGenericId;

	TInt index = iCapabilityArrays.FindInOrder( &refContainer, linearOrder );

	if ( index == KErrNotFound )
	    {
		User::Leave( KErrNotFound );
	    }

	//delete old one
	delete iCapabilityArrays[index];

	//and replace that with updated version
	iCapabilityArrays[index] = updatedContainer;

	index = iUpdatedCapabilityArray.FindInOrder( &refContainer, linearOrder );
	iUpdatedCapabilityArray.Remove( index );
    }

// -----------------------------------------------------------------------------
// CCapabilityStorage::SetGenericIDHeaderL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCapabilityStorage::SetGenericIDHeaderL( const TAccPolGIDHeader& aGenericHeaderId )
    {
	TAccPolGenericID genericId;
	TAccPolGenericIDAccessor::SetDBID( genericId, aGenericHeaderId.iDBID );

	//get a container 
	CListContainer * container = FindContainer( genericId, iCapabilityArrays );

	if ( !container)
	    {
		User::Leave( KErrNotFound );
    	}

	TAccPolGenericIDAccessor::SetGenericIDStaticAttributes( 
        container->iGenericId, aGenericHeaderId);
    }

// -----------------------------------------------------------------------------
// CCapabilityStorage::NewCapabilityListL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCapabilityStorage::NewCapabilityListL( const TAccPolGenericID& aGenericId)
    {
	API_TRACE_1( "[ACCPOLICY] CCapabilityStorage::NewCapabilityListL() - New capability list for connection (unique id %d)", aGenericId.UniqueID() );

	//get a container 
	CListContainer * container = FindContainer( aGenericId, iCapabilityArrays );

	if ( container )
	    {
		//generic id already exist
		API_TRACE_( "[ACCPOLICY] CCapabilityStorage::NewCapabilityListL() - Connection with unique id already exist" );
		User::Leave( KErrAlreadyExists );
	    }		

	//create new container
	container = new (ELeave) CListContainer();
	CleanupStack::PushL( container );

	//set unique id and internalize caps from stream
	container->iGenericId = aGenericId;

	//insert in container list
	TLinearOrder<CListContainer> linearOrder( &CCapabilityStorage::OrderFunctionListContainer );
	iCapabilityArrays.InsertInOrderL( container, linearOrder );    

	CleanupStack::Pop( container);

	API_TRACE_1( "[ACCPOLICY] CCapabilityStorage::RemoveCapabilityList() - Capability container count %d",  iCapabilityArrays.Count());
	API_TRACE_( "[ACCPOLICY] CCapabilityStorage::NewCapabilityListL() - return void");	
    }

// -----------------------------------------------------------------------------
// CCapabilityStorage::RemoveCapabilityList
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCapabilityStorage::RemoveCapabilityList( const TAccPolGenericID& aGenericId)
    {
	API_TRACE_1( "[ACCPOLICY] CCapabilityStorage::RemoveCapabilityList() - Remove capability list (unique id %d)", aGenericId.UniqueID() );

	//find generic id list...
	TLinearOrder<CListContainer> linearOrder( &CCapabilityStorage::OrderFunctionListContainer );
	CListContainer refContainer;
	refContainer.iGenericId = aGenericId;

	TInt index = iCapabilityArrays.FindInOrder( &refContainer, linearOrder );

	if ( index == KErrNotFound )
	    {
		//generic id is not found
		API_TRACE_( "[ACCPOLICY] CCapabilityStorage::RemoveCapabilityList() - Unique id not found" );
		return KErrNone;
    	}

	//get a container 
	CListContainer * container = iCapabilityArrays[index];

	//release capability array resources
	container->iCapabilityArray.ResetAndDestroy();
	container->iCapabilityArray.Close();

	//delete container itself
	delete container;

	//remove containers index from capability array
	iCapabilityArrays.Remove( index );

	API_TRACE_1( "[ACCPOLICY] CCapabilityStorage::RemoveCapabilityList() - Capability container count %d",  iCapabilityArrays.Count());
	API_TRACE_( "[ACCPOLICY] CCapabilityStorage::RemoveCapabilityList() - return void");		
	return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCapabilityStorage::GetCapability
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CCapValue* CCapabilityStorage::GetCapability( 
    const TAccPolGenericID& aGenericId, const TUint32& aName)
    {	
	API_TRACE_2( "[ACCPOLICY] CCapabilityStorage::GetCapability() -  unique id=%d name=%x", aGenericId.UniqueID(), aName );

	//get a container 
	CListContainer * container = FindContainer( aGenericId, iCapabilityArrays );

	if ( !container )
	    {
		//generic id is not found
		API_TRACE_( "[ACCPOLICY] CCapabilityStorage::GetCapability() - Unique id not found" );		
		return NULL;
	    }

	API_TRACE_( "[ACCPOLICY] CCapabilityStorage::GetCapability() - return void");
    //find capability from array...
	return FindCapValue( container, aName );
    }

// -----------------------------------------------------------------------------
// CCapabilityStorage::SetCapabilityL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCapabilityStorage::SetCapabilityL( const TAccPolGenericID& aGenericId, 
	    							   const TUint32& aName, 
	    							   const TAccPolValueLocation& aLocation, 
	    							   const TAccPolValueType& aType, 
	    							   const TInt32& aValue )
    {
	API_TRACE_1( "[ACCPOLICY] CCapabilityStorage::SetCapabilityL() - Set capability for connection (unique id=%d) ", aGenericId.UniqueID());
	API_TRACE_1( "[ACCPOLICY] CCapabilityStorage::SetCapabilityL() - Name = %x", aName);
	API_TRACE_1( "[ACCPOLICY] CCapabilityStorage::SetCapabilityL() - Location = %d", aLocation);
	API_TRACE_1( "[ACCPOLICY] CCapabilityStorage::SetCapabilityL() - Type = %d", aType);
	API_TRACE_1( "[ACCPOLICY] CCapabilityStorage::SetCapabilityL() - Value = %d", aValue);

	//get a container 
	CListContainer * container = FindContainer( aGenericId, iCapabilityArrays );

	if ( !container )
    	{
		//generic id is not found
		API_TRACE_( "[ACCPOLICY] CCapabilityStorage::GetCapability() - Unique id not found" );		
		User::Leave( KErrArgument );
    	}	

	CCapValue* capValue = FindCapValue( container, aName );

	if (!capValue )
	    {
		API_TRACE_( "[ACCPOLICY] CCapabilityStorage::SetCapabilityL() - Capability does not exist, create new capability");

		//create new capability
	    capValue = new ( ELeave ) CCapValue();
	    CleanupStack::PushL( capValue );	

        //this is needed because InsertInOrder doesn't work without name...
        capValue->iName = aName;

    	//insert a new item into ordered list
		TLinearOrder<CCapValue> linearOrder( &CCapabilityStorage::OrderFunctionCapList );
   	 	container->iCapabilityArray.InsertInOrderL( capValue, linearOrder );    

		CleanupStack::Pop( capValue );	
    	}

    //set properties
    capValue->iName = aName;

    if ( aLocation == EAPVAccessory )
        {
    	capValue->iName |= ( 1 << 16 );
        }
    capValue->iName |= ( aType << 17 );

    //set value...
    capValue->iIntValue = aValue;

	API_TRACE_( "[ACCPOLICY] CCapabilityStorage::SetCapabilityL() - return void");    
    }

// -----------------------------------------------------------------------------
// CCapabilityStorage::SetCapabilitiesL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCapabilityStorage::SetCapabilitiesL( 
    const TAccPolGenericID& aGenericId, RBufReadStream& aReadStrm)
    {
	API_TRACE_1( "[ACCPOLICY] CCapabilityStorage::SetCapabilitiesL() - Set capabilities for connection (unique id=%d) ", aGenericId.UniqueID());

	//get a container 
	CListContainer * container = FindContainer( aGenericId, iCapabilityArrays );

	if ( !container )
	    {
		//generic id is not found
		API_TRACE_( "[ACCPOLICY] CCapabilityStorage::GetCapability() - Unique id not found" );		
		User::Leave( KErrArgument );
	    }	

	InternalizeToArrayL( aReadStrm, container->iCapabilityArray);

	API_TRACE_( "[ACCPOLICY] CCapabilityStorage::SetCapabilitiesL() - return void");    
    }

// -----------------------------------------------------------------------------
// CCapabilityStorage::SetCapabilityL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCapabilityStorage::SetCapabilityL( 
    const TAccPolGenericID& aGenericId, 
	const TUint32& aName )
    {
	API_TRACE_1( "[ACCPOLICY] CCapabilityStorage::SetCapabilityL() - Set capability for connection (unique id=%d) ", aGenericId.UniqueID());
	API_TRACE_1( "[ACCPOLICY] CCapabilityStorage::SetCapabilityL() - Name = %x", aName);
	API_TRACE_(  "[ACCPOLICY] CCapabilityStorage::SetCapabilityL() - Location = n/a");
	API_TRACE_(  "[ACCPOLICY] CCapabilityStorage::SetCapabilityL() - Type = n/a");
	API_TRACE_(  "[ACCPOLICY] CCapabilityStorage::SetCapabilityL() - Value = n/a");

	//get a container 
	CListContainer * container = FindContainer( aGenericId, iCapabilityArrays );

	if ( !container )
	    {
		//generic id is not found
		User::Leave( KErrArgument );
	    }

	CCapValue* capValue = FindCapValue( container, aName );

	if ( !capValue )
	    {
		API_TRACE_( "[ACCPOLICY] CCapabilityStorage::SetCapabilityL() - Capability does not exist, create new capability");

		//create new capability
	    capValue = new (ELeave) CCapValue();
	    CleanupStack::PushL( capValue );	
	
        //this is needed because InsertInOrder doesn't work without name...
        capValue->iName = aName;
	
    	//insert a new item into ordered list
		TLinearOrder<CCapValue> linearOrder( &CCapabilityStorage::OrderFunctionCapList );
   	 	container->iCapabilityArray.InsertInOrderL( capValue, linearOrder );    
    
		CleanupStack::Pop( capValue );	
	    }	

    //set properties
    capValue->iName = aName;
   	API_TRACE_( "[ACCPOLICY] CCapabilityStorage::SetCapabilityL() - return void");    
    }

// -----------------------------------------------------------------------------
// CCapabilityStorage::SetCapabilityL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCapabilityStorage::RemoveCapabilitiesL( const TAccPolGenericID& aGenericId )
    {
	API_TRACE_1( "[ACCPOLICY] CCapabilityStorage::RemoveCapabilitiesL() - Remove capabilities, unique id (unique id=%d) ", iUniqueIdCounter);
	//get a container 
	CListContainer * container = FindContainer( aGenericId, iCapabilityArrays);
		if(!container)
			{
			//generic id is not found
			API_TRACE_( "[ACCPOLICY] CCapabilityStorage::RemoveCapabilitiesL() - Unique id not found" );		
			User::Leave( KErrNotFound );
			}
		else
			{	
			container->iCapabilityArray.ResetAndDestroy();
			}
  	API_TRACE_( "[ACCPOLICY] CCapabilityStorage::GenerateUniqueId() - return void");    	
    }

// -----------------------------------------------------------------------------
// CCapabilityStorage::SetCriticalCapabilityL
// -----------------------------------------------------------------------------
//
EXPORT_C void CCapabilityStorage::SetCriticalCapabilityL( 
    const TUint32& aName, 
    const TAccPolValueLocation& aLocation, 
    const TAccPolValueType& aType, 
    const TInt32& aValue )
	{	
	API_TRACE_4( "[ACCPOLICY] CCapabilityStorage::SetCriticalCapabilityL(Name=0x%x Location=%d Type=%d Value=%d", aName, aLocation, aType, aValue);

	CCapValue* capValue = new (ELeave) CCapValue();
	CleanupStack::PushL( capValue);	

    capValue->iName = aName;

    if ( aLocation == EAPVAccessory )
        {
    	capValue->iName |= (1 << 16);
        }

    capValue->iName |= (aType << 17);
    capValue->iIntValue = aValue;

    iCriticalCapabilityArray.AppendL( capValue );  

    CleanupStack::Pop( capValue);

	API_TRACE_( "[ACCPOLICY] CCapabilityStorage::SetCriticalCapabilityL - return void");    	    
	}

// -----------------------------------------------------------------------------
// CCapabilityStorage::FindCriticalCapValue
//
// Same critical capability can have several values.
// e.g. cabability X is critical and accepted values can be 1 and 2.
// -----------------------------------------------------------------------------
//
TBool CCapabilityStorage::FindCriticalCapValue( TUint32 aName, TInt32 aCapabilityValue )
    {
    API_TRACE_2( "[AccFW:ACCPOLICY] CCapabilityStorage::FindCriticalCapValue(Name=0x%x Value=%d)", aName, aCapabilityValue );
    TBool ret( EFalse );
    TInt criticalCapabilityCount = iCriticalCapabilityArray.Count();
    TInt32 criticalCapabilityValue( 0 );

    for ( TInt i( 0 ); i < criticalCapabilityCount; ++i )
        {
        if ( aName == iCriticalCapabilityArray.operator[]( i )->Name() )
            {
            iCriticalCapabilityArray.operator[]( i )->ValueInt( criticalCapabilityValue );
            if( aCapabilityValue == criticalCapabilityValue )
                {
                ret = ETrue;
                }
            }            
        }

	API_TRACE_1( "[AccFW:ACCPOLICY] CCapabilityStorage::FindCriticalCapValue - return %d", ret );

	return ret;		
    }

// -----------------------------------------------------------------------------
// CCapabilityStorage::ResolveCriticalCapabilitiesL
//
// Check if critical capabilities are defined in the
// Generic ID(aGenericID), and return the set that matches(aOutputNameArr).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCapabilityStorage::ResolveCriticalCapabilitiesL(
    const TAccPolGenericID& aGenericID,
    CAccPolSubblockNameArray& aOutputNameArr )
    {
    API_TRACE_1( "[AccFW:ACCPOLICY] CCapabilityStorage::ResolveCriticalCapabilitiesL(UniqueId=%d)", aGenericID.UniqueID() );

    TInt capaCount( iCriticalCapabilityArray.Count() );

    for ( TInt i( 0 ); i < capaCount; ++i )
        {
        TUint32 name( 0 );
        name = iCriticalCapabilityArray.operator[]( i )->Name();        

        if ( GetCapability( aGenericID, name ) )
            {
            // If aGenericID have a critical capability, append it to result array.
            TAccPolSubblockNameArrayAccessor::AppendNameL( aOutputNameArr, name );
            }
        }

    API_TRACE_( "[AccFW:ACCPOLICY] CCapabilityStorage::ResolveCriticalCapabilitiesL - return void" );
    }

// -----------------------------------------------------------------------------
// CCapabilityStorage::CompareCriticalValuesL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CCapabilityStorage::CompareCriticalValuesL( const TAccPolGenericID& aGenericID )
    {
    API_TRACE_1( "[AccFW:ACCPOLICY] CCapabilityStorage::CompareCriticalValuesL(UniqueId=%d)", aGenericID.UniqueID() );
    TBool   ret( ETrue );    
    TUint32 criticalCapabilityName( 0 );
    TInt32  capabilityValue( 0 );    
    TInt    criticalCapabilityCount( 0 ); 

    CAccPolSubblockNameArray* criticalCapabilities = CAccPolSubblockNameArray::NewL();
    CleanupStack::PushL( criticalCapabilities );

    //After this query, criticalCapabilities contains all critical capabilities from aGenericID
    ResolveCriticalCapabilitiesL( aGenericID, *criticalCapabilities );    

    criticalCapabilityCount = TAccPolSubblockNameArrayAccessor::GetCount( *criticalCapabilities );

    for ( TInt i( 0 ) ; i<criticalCapabilityCount ; ++i )
        {
        //Get the value from the aGenericID
        TAccPolSubblockNameArrayAccessor:: GetName( 
            *criticalCapabilities, i, criticalCapabilityName );

        CCapValue * capability = GetCapability( aGenericID, criticalCapabilityName );

        if ( !capability )
            {
            User::Leave( KErrArgument );
            }

        capability->ValueInt( capabilityValue );

        //compare capability value
        if( FindCriticalCapValue( criticalCapabilityName, capabilityValue ) )
            {
            //This capability is OK
            API_TRACE_( "[AccFW:ACCPOLICY] CCapabilityStorage::CompareCriticalValuesL - capability OK" );
            }
        else
            {
            ret = EFalse;
            API_TRACE_( "[AccFW:ACCPOLICY] CCapabilityStorage::CompareCriticalValuesL - check failed" );
            break;//no need to continue            
            }
        }

    CleanupStack::PopAndDestroy( criticalCapabilities );

    API_TRACE_1( "[AccFW:ACCPOLICY] CCapabilityStorage::CompareCriticalValuesL - return %d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CCapabilityStorage::GenerateUniqueId
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCapabilityStorage::GenerateUniqueId( TAccPolGenericID& aGenericId )
    {
	API_TRACE_1( "[ACCPOLICY] CCapabilityStorage::GenerateUniqueId() - Create unique id (unique id=%d) ", iUniqueIdCounter);
	TAccPolGenericIDAccessor::SetDBID( aGenericId, iUniqueIdCounter++ );
  	API_TRACE_( "[ACCPOLICY] CCapabilityStorage::GenerateUniqueId() - return void");    
    }
		
// -----------------------------------------------------------------------------
// CCapabilityStorage::InternalizeToArrayL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCapabilityStorage::InternalizeToArrayL( 
    RReadStream& aStream, RCapabilityArray& aArray )
    {
   	API_TRACE_( "[ACCPOLICY] CCapabilityStorage::InternalizeToArrayL()");   

	//read count of the capabilities
	TInt16 count( aStream.ReadInt16L() );
	API_TRACE_1( "[ACCPOLICY] CCapabilityStorage::InternalizeToArrayL() - Capability count = %d", count);

    //read individual capabilities
    for ( TInt i( 0 ) ; i < count ; ++i )
        {
    	//create new capability value
    	CCapValue* capValue = new ( ELeave ) CCapValue();
    	CleanupStack::PushL( capValue );

    	//read name value
    	capValue->iName = aStream.ReadUint16L();

    	//location, type, size
    	TUint8 properties = aStream.ReadUint8L();

    	//add type information to value field
    	capValue->iName |= ( properties << 16 );

		API_TRACE_ ( "	- New capability" );
		API_TRACE_1( "	- Name = %x", capValue->Name() );
		API_TRACE_1( "	- Location = %d", capValue->Location() );
		API_TRACE_1( "	- Type = %d", capValue->Type() );

    	switch ( capValue->Type() )
    	    {
    		case EAPVInt:
    		case EAPVUInt:
    		case EAPVDes8:
		    case EAPVObject:		  
    		    {
 			   	//read size value from property field
    			TInt size = (0xC0 & properties) >> 6;
    			size++;
    			
    			capValue->iIntValue = 0;

    			//read (size) count of bytes from the stream
    			for ( TInt i(0); i < size; i++  )
    			    {
    				capValue->iIntValue <<= 8;
    				capValue->iIntValue |= aStream.ReadUint8L();
    			    }

    			if ( capValue->Type() == EAPVDes8  || capValue->Type() == EAPVObject )
    			    {
    				//read descriptor value, value defines now the the length of the descriptor
    				TInt size = capValue->iIntValue;
					if( size > 0 )
						{
	    				//create buffer for size
	    				capValue->iHBufValue = HBufC8::NewLC( size );
	    				TPtr8 ptr( capValue->iHBufValue->Des() );

	    				for ( TInt i(0); i < size; i++ )
	    				    {
	    					ptr.Append( aStream.ReadUint8L() );
	    				    }

	    				CleanupStack::Pop( capValue->iHBufValue );
						}
    			    }
    			    
#ifdef _DEBUG
    			else
    			    {

					TInt32 value( 0 );
					capValue->ValueInt( value );
					API_TRACE_1( "	- Value = %d", value );
    			    }
#endif //_DEBUG
    			break;
    		    }
    		case EAPVBool:
    		    {
    			//check is "not defined"-flag set
    			if ( 0x80 & properties )
    		    	{
    				capValue->iBoolValue = KErrUnknown;
 					API_TRACE_1( "	- Value = %d", KErrUnknown );
   			        }
                else
    			    {
    				//read boolean flag...
    				if ( 0x40 & properties )
    				    {
    					capValue->iBoolValue = ETrue;	
    				    }

#ifdef _DEBUG
					TInt32 value( 0 );
					capValue->ValueInt( value );
    			 	API_TRACE_1( "	- Value = %d", value ); 	
#endif //_DEBUG
    			    }
    		    }	
   			default:
   			    break;
    	    };

    	//insert a new item into ordered list
		TLinearOrder<CCapValue> linearOrder( &CCapabilityStorage::OrderFunctionCapList);
    	aArray.InsertInOrderL( capValue, linearOrder);

    	CleanupStack::Pop( capValue);
        }

    API_TRACE_( "[ACCPOLICY] CCapabilityStorage::InternalizeToArrayL() - return void"); 
    }

// -----------------------------------------------------------------------------
// CCapabilityStorage::ExternalizeCapabilityNameListL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCapabilityStorage::ExternalizeCapabilityNameListL( 
    const TAccPolGenericID& aGenericId, RBufWriteStream& aWriteStrm )
    {
   	API_TRACE_( "[ACCPOLICY] CCapabilityStorage::ExternalizeCapabilityNameListL()");   

	//get a container 
	CListContainer * container = FindContainer( aGenericId, iCapabilityArrays );

	if ( !container )
	    {
		//generic id is not found
		User::Leave( KErrArgument );
	    }

    TInt16 count( container->iCapabilityArray.Count() );
    aWriteStrm.WriteInt16L( count );

	//Serialize cap names to stream...
	for ( TInt i( 0 ) ; i < container->iCapabilityArray.Count() ; i++ )
	    {
		  aWriteStrm.WriteInt16L( container->iCapabilityArray[i]->iName );		
	    }
	
   	API_TRACE_( "[ACCPOLICY] CCapabilityStorage::ExternalizeCapabilityNameListL() - return void");   
    }

// -----------------------------------------------------------------------------
// CCapabilityStorage::CapabilityInUse
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CCapabilityStorage::CapabilityInUse( 
    const TAccPolGenericID& aGenericId, const TUint32& aName )
    {
   	API_TRACE_( "[ACCPOLICY] CCapabilityStorage::CapabilityInUse()");   

	TBool retVal( EFalse );

	//check all lists expect which unique id is a same as in aGenericId parameter
	for ( TInt i(0); i < iCapabilityArrays.Count(); i++ )
	    {
		CListContainer * container = iCapabilityArrays[i];

		if ( container->iGenericId.UniqueID() != aGenericId.UniqueID() )
    		{
			if ( FindCapValue( container, aName ) )
	    		{
				//capability is in use
				retVal = ETrue;		
				break;
		    	}
		    }
	    }

   	API_TRACE_1( "[ACCPOLICY] CCapabilityStorage::CapabilityInUse() - return %d", retVal);   	
	return retVal;
    }

// -----------------------------------------------------------------------------
// CCapabilityStorage::FindContainer
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCapabilityStorage::CListContainer* CCapabilityStorage::FindContainer(
    const TAccPolGenericID& aGenericId, const RCapabilityArrays& aArray )
    {
	//find generic id list...
	TLinearOrder<CListContainer> linearOrder( &CCapabilityStorage::OrderFunctionListContainer );
	CListContainer refContainer;
	refContainer.iGenericId = aGenericId;

	TInt index = aArray.FindInOrder( &refContainer, linearOrder );

	if ( index == KErrNotFound )
	    {
		//generic id is not found
		return NULL;
    	}

	//get a container
	return aArray[ index];
    }

// -----------------------------------------------------------------------------
// CCapabilityStorage::FindCapValue
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCapValue* CCapabilityStorage::FindCapValue( 
    const CListContainer* aCapabilityList, 
    const TUint32& aName)
    {
    //find capability from array...
	TLinearOrder<CCapValue> linearOrderCaps( &CCapabilityStorage::OrderFunctionCapList);	

	CCapValue refCap;
	refCap.iName = aName;

	TInt index = aCapabilityList->iCapabilityArray.FindInOrder( &refCap, linearOrderCaps);	

	if ( index == KErrNotFound )
	    {
		//capability is not found
		return NULL;
    	}

	return aCapabilityList->iCapabilityArray[index];		
    }

// -----------------------------------------------------------------------------
// CCapabilityStorage::OrderFunctionListContainer
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCapabilityStorage::OrderFunctionListContainer( 
    CListContainer const& aContainer1, 
    CListContainer const& aContainer2 )
    {
	if ( aContainer1.iGenericId.UniqueID() == aContainer2.iGenericId.UniqueID() ) return 0;
	return ( aContainer1.iGenericId.UniqueID() < aContainer2.iGenericId.UniqueID() ? -1 : 1 );
    }

// -----------------------------------------------------------------------------
// CCapabilityStorage::OrderFunctionCapList
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCapabilityStorage::OrderFunctionCapList( 
    CCapValue const& aCapValue1, 
    CCapValue const& aCapValue2 )
    {
	if ( aCapValue1.Name() == aCapValue2.Name() ) return 0;
	return ( aCapValue1.Name() < aCapValue2.Name() ? -1 : 1 );
    }		

// -----------------------------------------------------------------------------
// CCapabilityStorage::CListContainer::CListContainer
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------


CCapabilityStorage::CListContainer::CListContainer()
    {
	TAccPolGenericIDAccessor::SetDBID( iGenericId, 0 );
    }

// -----------------------------------------------------------------------------
// CCapabilityStorage::CListContainer::~CListContainer
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------

CCapabilityStorage::CListContainer::~CListContainer()
    {
	iCapabilityArray.ResetAndDestroy();
	iCapabilityArray.Close();
    }
