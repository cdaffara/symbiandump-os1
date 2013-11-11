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



#include <centralrepository.h>

#include "epos_modulesinternal.h"
#include <lbs/epos_cposmoduleidlist.h>
#include "epos_cposmodulessettings.h"

// ============================ LOCAL FUNCTIONS ==============================

// =========================== MEMBER FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CPosModulesSettings::CPosModulesSettings()
	{
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CPosModulesSettings* CPosModulesSettings::NewL( 
	const TDesC& aPriorityKey,
	const TDesC& aInvisibleKey,
	const TDesC& aVersionKey )
    {
    CPosModulesSettings* self = new (ELeave) CPosModulesSettings();
    CleanupStack::PushL( self );
    self->ConstructL( aPriorityKey, aInvisibleKey, aVersionKey );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesSettings::ConstructL(
	const TDesC& aPriorityKey,
	const TDesC& aInvisibleKey,
	const TDesC& aVersionKey )
    {
    ParseAttributedKeyL( aPriorityKey, iPriorityList );
    ParseAttributedKeyL( aInvisibleKey, iInvisibleList );
    SetVersionL( aVersionKey );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CPosModulesSettings* CPosModulesSettings::NewL( const CPosModulesSettings& aSettings )
    {
    CPosModulesSettings* self = new (ELeave) CPosModulesSettings();
    CleanupStack::PushL( self );
    self->ConstructL( aSettings );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesSettings::ConstructL( const CPosModulesSettings& aSettings )
    {
    for ( TInt i = 0; i < aSettings.iPriorityList.Count(); i++ )
        {
        iPriorityList.AppendL( aSettings.iPriorityList[i] );
        }
    for ( TInt i = 0; i < aSettings.iInvisibleList.Count(); i++ )
        {
        iInvisibleList.AppendL( aSettings.iInvisibleList[i] );
        }
    SetVersionL( aSettings.Version() );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CPosModulesSettings::~CPosModulesSettings()
	{
	iPriorityList.Close();
	iInvisibleList.Close();
    delete iVersion;
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CPosModulesSettings::GetAttributes( 
	TPositionModuleId aModuleId, 
	TPosModuleAttributes& aAttributes ) const
	{
    TInt index = FindVisible( aModuleId );
    if ( index != KErrNotFound )
    	{
    	aAttributes = iPriorityList[index];
    	aAttributes.iVisible = ETrue;
    	}
	else
		{
		index = FindInvisible( aModuleId );
	    if ( index != KErrNotFound )
	    	{
	    	aAttributes = iInvisibleList[index];
	    	aAttributes.iVisible = EFalse;
	    	}
		else
			{
			return KErrNotFound;
			}
		}
	return KErrNone;
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesSettings::SetAttributesL( 
	TPositionModuleId aModuleId, 
	const TPosModuleAttributes& aAttributes )
	{
	TPosModuleListItem item;
	item.iUid = aModuleId;	
	item.iCost = aAttributes.iCost;	
	item.iAvailable = aAttributes.iAvailable;	
		
	TInt index = FindVisible( aModuleId );
	if ( index != KErrNotFound )
		{
		if ( aAttributes.iVisible )
			{
			iPriorityList[index] = item;
			}
		else
			{
			// item needs to be moved to invisible list
			iInvisibleList.AppendL( item );
			iPriorityList.Remove( index );
			}	
		}
	else if ( ( index = FindInvisible( aModuleId ) ) != KErrNotFound )	
		{
		if ( !aAttributes.iVisible )
			{
			iInvisibleList[index] = item;
			}
		else
			{
			// item needs to be moved to visible list
			iPriorityList.AppendL( item );
			iInvisibleList.Remove( index );
			}	
		}
	else
		{
		User::Leave( KErrNotFound );
		}		
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CPosModuleIdList* CPosModulesSettings::GetPriorityListLC() const
	{
	CPosModuleIdList* list = CPosModuleIdList::NewL();
	CleanupStack::PushL( list );
	
	for ( TInt i = 0; i < iPriorityList.Count(); i++ )
		{
		list->AppendL( iPriorityList[i].iUid );
		}
	
	return list;
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CPosModuleIdList* CPosModulesSettings::GetInstalledListLC() const
	{
	CPosModuleIdList* list = GetPriorityListLC();
	
	for ( TInt i = 0; i < iInvisibleList.Count(); i++ )
		{
		list->AppendL( iInvisibleList[i].iUid );
		}
	
	return list;
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TBool CPosModulesSettings::IsVisible( TPositionModuleId aModuleId ) const
	{
	return ( FindVisible( aModuleId ) != KErrNotFound );
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesSettings::CompareL( 
	const CPosModulesSettings& aSettings, 
	RArray<TPosModulesEvent>& aEvents ) const
	{
    SearchNewOldOrChangedL( aSettings, aEvents );

	if ( !aEvents.Count() ) // no other events found
		{
	    SearchPriorityChangeL( aSettings, aEvents );
		}
	}	

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TBool CPosModulesSettings::ModuleExists( TPositionModuleId aModuleId ) const
	{
	return ( FindVisible( aModuleId ) != KErrNotFound || 
			 FindInvisible( aModuleId ) != KErrNotFound );
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesSettings::SetPrioritiesL( const CPosModuleIdList& aList )
	{
    // check that the lists are equal in number of items
    if ( aList.Count() != iPriorityList.Count() )
        {
        User::Leave( KErrArgument );
        }

    TIdentityRelation<TPosModuleListItem> byUid( CompareListItemsByUid );
	TPosModuleListItem item;

    // First, verify that IDs mentioned in the input list are valid
    for ( TInt i = 0; i < aList.Count(); i++ )
        {
		item.iUid = aList[i];
        
        // Does this UID exist in current settings?
        TInt index = iPriorityList.Find( item, byUid );
        if ( index == KErrNotFound ) 
			{
			User::Leave( KErrArgument );
			}

        // Isn't this UID duplicated?
        for ( TInt j = 0; j < i; j++ )
			{
			if ( aList[j] == aList[i] )
				{
				User::Leave( KErrArgument );
				}
			}
			
		// Can't move items to new positions
		// until input list is fully validated
        }
        
    // UIDs are valid, just rearrange them now
    for ( TInt k = 0; k < aList.Count(); k++ )
    	{
		item.iUid = aList[k];
        TInt oldIndex = iPriorityList.Find( item, byUid );

		// move item from old position to k
		TPosModuleListItem swap = iPriorityList[k];
		iPriorityList[k] = iPriorityList[oldIndex];
		iPriorityList[oldIndex] = swap;
    	}
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesSettings::AddL( 
    TPositionModuleId aModuleId,
    const TPosModuleAttributes& aAttributes )
	{
	__ASSERT_DEBUG( !ModuleExists( aModuleId ), Panic( KErrAlreadyExists ) );
	
	TPosModuleListItem item( aAttributes, aModuleId );
	if ( aAttributes.iVisible )
		{
		User::LeaveIfError( iPriorityList.Append( item ) );
		} 
	else
		{
		User::LeaveIfError( iInvisibleList.Append( item ) );
		}
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesSettings::Remove( TPositionModuleId aModuleId )
	{
    TInt index = FindVisible( aModuleId );
    if ( index != KErrNotFound )
    	{
    	iPriorityList.Remove( index );
    	}
	
	if ( ( index = FindInvisible( aModuleId ) ) != KErrNotFound )
    	{
    	iInvisibleList.Remove( index );
    	}
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesSettings::FormatL( TPtr& aPriorityKey, TPtr& aInvisibleKey ) const
	{
	HBufC* priorityKey = FormatAttributedKeyLC( iPriorityList );
	aPriorityKey.Copy( *priorityKey );
	CleanupStack::PopAndDestroy( priorityKey );

	HBufC* invisibleKey = FormatAttributedKeyLC( iInvisibleList );
	aInvisibleKey.Copy( *invisibleKey );
	CleanupStack::PopAndDestroy( invisibleKey );
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const TPtrC CPosModulesSettings::Version() const
	{
	return *iVersion;
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesSettings::SetVersionL( const TDesC& aVersion )
	{
	HBufC* version = aVersion.AllocLC();
	delete iVersion;
	iVersion = version;
	CleanupStack::Pop( version );
	}

// ========================= INTERNAL FUNCTIONS ==============================

// ---------------------------------------------------------------------------
//  Fails only on OOM case
// ---------------------------------------------------------------------------
//
void CPosModulesSettings::ParseAttributedKeyL( 
	const TDesC& aString, 
	RModuleList& aList ) const
	{
    // The sequence in the buffer is "H{8},D+,D+[,...]" 
    // which is "UID,status,cost"
    // H - hex digit, D - decimal digit, delimiter - comma
    // UID check is strict and fails with KErrCorrupt if something's wrong
    // Status and Cost checks a lazy, use default value if error
    // Status - inactive, cost - unknown
    
    // TLex uses whitespace as delimiter
    // Replace all commas with whitespace
    HBufC* string = aString.AllocLC();
    ReplaceCommasWithWhitespace( string->Des() );
    
    TLex lex( *string );
    while ( !lex.Eos() )
        {
        TPosModuleListItem item;
    
        // parse UID
        TPtrC token( lex.NextToken() );
        if ( !token.Length() )
            {
            break;  // no more items found
            }
        
        item.iUid.iUid = 0;
        TInt err = ParseModuleUid( token, (TUint32&) item.iUid.iUid );

        // parse status field
        token.Set( lex.NextToken() );
        err |= TLex( token ).Val( (TUint&) item.iAvailable, EDecimal );

        if ( item.iAvailable != EModuleAvailable && 
        	 item.iAvailable != EModuleNotAvailable )
        	 {
        	 err |= KErrCorrupt;
        	 }

        // parse cost field
        token.Set( lex.NextToken() );
        err |= TLex( token ).Val( (TUint&) item.iCost, EDecimal );

        // Avoid duplicate UIDs ( in both lists )
        TPosModuleAttributes otherItem;
        if ( !err && !ModuleExists( item.iUid ) )
            {
	        // Add module list item to the list
	        aList.AppendL( item );
            }
        }
        
	CleanupStack::PopAndDestroy( string );
	}
	
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
HBufC* CPosModulesSettings::FormatAttributedKeyLC( const RModuleList& aList ) const
	{
    HBufC* keyBuf = HBufC::NewLC( NCentralRepositoryConstants::KMaxUnicodeStringLength );
    TPtr key = keyBuf->Des();

    TBuf<KAttributedKeyMaxItemLength> item;

    for ( TInt i = 0; i < aList.Count(); i++ )
        {
        TPosModuleListItem module = aList[i];
        
        item.Format( KAttributedKeyItemFormat,
            module.iUid,
            module.iAvailable ? 1 : 0,
            module.iCost );
            
        if ( key.Length() + item.Length() + KDelimiterLength < key.MaxLength() ) 
            {
            if ( i > 0 )
                {
                key.Append( KFieldDelimiter ); // no comma before first item
                }
            key.Append( item );
            }
        else
            {
            User::Leave( KErrOverflow );
            }
        }
	return keyBuf;
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesSettings::SearchNewOldOrChangedL( 
	const CPosModulesSettings& aOldSettings, 
	RArray<TPosModulesEvent>& aEvents ) const
	{
	CPosModuleIdList* currentList = GetInstalledListLC();
	CPosModuleIdList* oldList = aOldSettings.GetInstalledListLC();

	// Search removed
	for ( TInt i = 0; i < oldList->Count(); i++ )
		{
		TPositionModuleId id = (*oldList)[i];

        if ( currentList->Find( id ) == KErrNotFound )
        	{
        	AddEventL( id, EPosModulesEventModuleRemoved, aEvents );
        	}
		}

	// Search installed or changed
	for ( TInt i = 0; i < currentList->Count(); i++ )
		{
		TPositionModuleId id = (*currentList)[i];

		TPosModuleAttributes current; 
		GetAttributes( id, current );
		
		TPosModuleAttributes old;
        if ( aOldSettings.GetAttributes( id, old ) == KErrNotFound )
        	{
        	AddEventL( id, EPosModulesEventModuleInstalled, aEvents );
        	}
		else
			{
			ComparePropertiesL( id, current, old, aEvents );
			}        	
		}
		
	CleanupStack::PopAndDestroy( oldList );
	CleanupStack::PopAndDestroy( currentList );
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesSettings::SearchPriorityChangeL( 
	const CPosModulesSettings& aOldSettings, 
	RArray<TPosModulesEvent>& aEvents ) const
	{
	if ( iPriorityList.Count() != aOldSettings.iPriorityList.Count() )
		{
		return;
		}

	for ( TInt i = 0; i < iPriorityList.Count(); i++ )
		{
		// do UIDs match?
		if ( iPriorityList[i].iUid != aOldSettings.iPriorityList[i].iUid )
			{
			AddEventL( KNullUid, EPosModulesEventPrioritiesChanged, aEvents );
			return;
			}
		}
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CPosModulesSettings::FindVisible( const TPositionModuleId& aModuleId ) const
	{
	TPosModuleListItem item( aModuleId );

    TIdentityRelation<TPosModuleListItem> byUid( CompareListItemsByUid );
    return iPriorityList.Find( item, byUid );
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CPosModulesSettings::FindInvisible( const TPositionModuleId& aModuleId ) const
	{
	TPosModuleListItem item( aModuleId );

    TIdentityRelation<TPosModuleListItem> byUid( CompareListItemsByUid );
    return iInvisibleList.Find( item, byUid );
	}
	
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TBool CPosModulesSettings::CompareListItemsByUid(
    const TPosModuleListItem& aOne, 
    const TPosModuleListItem& aAnother )
    {
    return ( aOne.iUid == aAnother.iUid );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesSettings::AddEventL( 
	TPositionModuleId aModuleId, 
	TPosModulesEventType aEventType,
	RArray<TPosModulesEvent>& aEvents ) const
	{	
	TPosModulesEvent event;
	event.iModuleId = aModuleId;
	event.iType = aEventType;
	aEvents.AppendL( event );
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesSettings::ComparePropertiesL( 
	TPositionModuleId aModuleId, 
	TPosModuleAttributes aOne, 
	TPosModuleAttributes aAnother, 
	RArray<TPosModulesEvent>& aEvents ) const
	{
	if ( aOne.iVisible != aAnother.iVisible ) 
		{
    	AddEventL( aModuleId, EPosModulesEventVisibilityChanged, aEvents );
		}
	if ( aOne.iCost != aAnother.iCost ) 
		{
    	AddEventL( aModuleId, EPosModulesEventCostChanged, aEvents );
		}
	if ( aOne.iAvailable != aAnother.iAvailable ) 
		{
    	AddEventL( aModuleId, EPosModulesEventAvailabilityChanged, aEvents );
		}
	}				

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesSettings::CopyAttributesL( const CPosModulesSettings& aSource )
	{
	CPosModuleIdList* list = aSource.GetInstalledListLC();
	for (int i = 0; i < list->Count(); ++i) 
		{
		TUid moduleId = (*list)[i];
		TPosModuleAttributes attrSource, attrTarget;
		aSource.GetAttributes( moduleId, attrSource );
		TInt err = GetAttributes( moduleId, attrTarget );
		if ( !err )
			{
			attrTarget.iAvailable = attrSource.iAvailable;
			attrTarget.iCost = attrSource.iCost;
			// visibility not copied
			SetAttributesL( moduleId, attrTarget );
			}
		}
	CleanupStack::PopAndDestroy( list );
	}
