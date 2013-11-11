/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Channel condition set object implementation
*
*/


#include "sensrvchannelconditionsetimpl.h"
#include "sensrvchannelconditionimpl.h"
#include "sensrvtrace.h"
#include "sensrvdefines.h"


// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CSensrvChannelConditionSetImpl::CSensrvChannelConditionSetImpl(
    TSensrvConditionSetType aConditionSetType)
    : iSetType(aConditionSetType),
      iJustAdded(ETrue)
    {
    }


// ---------------------------------------------------------------------------
// 2nd phase of construction
// ---------------------------------------------------------------------------
//
void CSensrvChannelConditionSetImpl::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "Sensrv Util - CSensrvChannelConditionSetImpl::ConstructL()" ) ) );
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSensrvChannelConditionSetImpl::~CSensrvChannelConditionSetImpl()
    {
    COMPONENT_TRACE( ( _L( "Sensrv Util - CSensrvChannelConditionSetImpl::~CSensrvChannelConditionSetImpl()" ) ) );
    
    iConditionList.ResetAndDestroy();
    delete iMetData;
    }

// ---------------------------------------------------------------------------
// Return condition set type
// ---------------------------------------------------------------------------
//
TSensrvConditionSetType CSensrvChannelConditionSetImpl::ConditionSetType() const
    {
    return iSetType;
    }

// ---------------------------------------------------------------------------
// Add channel condition to the condition set.
// ---------------------------------------------------------------------------
//
void CSensrvChannelConditionSetImpl::AddChannelConditionL(CSensrvChannelCondition* aChannelCondition)
    {
    API_TRACE( ( _L( "Sensrv Util - CSensrvChannelConditionSetImpl::AddChannelConditionL(aChannelCondition: 0x%x)" ), aChannelCondition ) );
    
    if(aChannelCondition)
        {
        TBool newOperatorGreaterThan(EFalse);        
        if (   aChannelCondition->ConditionOperator() == ESensrvOperatorGreaterThan
            || aChannelCondition->ConditionOperator() == ESensrvOperatorGreaterThanOrEquals )
            {
            newOperatorGreaterThan = ETrue;
            }
        
        // Check that this index doesn't already have a condition, or if it has, 
        // it is a matching range condition.
        TInt indexCount(0);
        TInt existingIndex(KErrNotFound);
        for(TInt i=0; i< iConditionList.Count(); i++)
            {
            TBool oldOperatorGreaterThan(EFalse);
            if (   iConditionList[i]->ConditionOperator() == ESensrvOperatorGreaterThan
                || iConditionList[i]->ConditionOperator() == ESensrvOperatorGreaterThanOrEquals )
                {
                oldOperatorGreaterThan = ETrue;
                }

            if ( iConditionList[i]->ConditionItemIndex() == aChannelCondition->ConditionItemIndex() )
                {
                indexCount++;
                existingIndex = i;
                
                if (
                    // Condition types of new and old condition for same index 
                    // must be a matched pair of range conditions 
                    // Note: Custom condition types cannot have multi-part conditions.
                    //       If custom type requires multiple checks for single condition,
                    //       it needs to define the condition value accordingly.
                    !( (   iConditionList[i]->ConditionType() == ESensrvRangeConditionLowerLimit
                        && aChannelCondition->ConditionType() == ESensrvRangeConditionUpperLimit ) 
                      || ( iConditionList[i]->ConditionType() == ESensrvRangeConditionUpperLimit
                        && aChannelCondition->ConditionType() == ESensrvRangeConditionLowerLimit ) )
                    // Operators must be different in range conditions
                    || oldOperatorGreaterThan == newOperatorGreaterThan
                    // Maximum of KSensrvMaxConditionIndexCount (=2) conditions per index    
                    || indexCount >= KSensrvMaxConditionIndexCount
                   ) 
                    {
                    ERROR_TRACE( ( _L( "Sensrv Util - CSensrvChannelConditionSetImpl::AddChannelConditionL - ERROR: Mismatch with existing condition for same index)" ) ) );
                    User::Leave(KErrArgument);
                    }
                }
            } // end for

        if (!indexCount)
            {
            // New index, just append
            iConditionList.AppendL(aChannelCondition);
            }
        else 
            {
            // Second part of range index, always insert after existing condition.
            // Always order range conditions so that lower limit is first
            if ( aChannelCondition->ConditionType() == ESensrvRangeConditionLowerLimit )
                {
                iConditionList.InsertL(aChannelCondition, existingIndex);
                }
            else if ( aChannelCondition->ConditionType() == ESensrvRangeConditionUpperLimit )
                {
                iConditionList.InsertL(aChannelCondition, existingIndex+1);
                }
            else
                {
                // Never gets here, unless there is an error
                User::Panic(KSensrvPanicCategory, ESensrvPanicUnexpectedBranch);
                }
            }
        }
    else
        {
        ERROR_TRACE( ( _L( "Sensrv Util - CSensrvChannelConditionSetImpl::AddChannelConditionL - ERROR: NULL condition)" ) ) );
        User::Leave(KErrArgument);
        }
    }

// ---------------------------------------------------------------------------
// Get all channel conditions added to this set.
// ---------------------------------------------------------------------------
//
const RSensrvChannelConditionList& CSensrvChannelConditionSetImpl::AllConditions() const
    {
    API_TRACE( ( _L( "Sensrv Util - CSensrvChannelConditionSetImpl::AllConditions()" ) ) );
    
    return iConditionList;
    }

// ---------------------------------------------------------------------------
// Check if set is complete.
// ---------------------------------------------------------------------------
//
TBool CSensrvChannelConditionSetImpl::IsComplete() const
    {
    COMPONENT_TRACE( ( _L( "Sensrv Util - CSensrvChannelConditionSetImpl::IsComplete()" ) ) );
    
    TBool complete(iConditionList.Count() > 0 ? ETrue : EFalse);
    TInt unmatchedIndex(KErrNotFound); 
    
    for ( TInt i=0; complete && i < iConditionList.Count(); i++ )
        {
        // If previous index was unmatched index, compare it to current index.
        // No need to check if current condition is range condition, as it is not
        // possible to add conditions with duplicate indexes unless they are 
        // range conditions.
        if ( unmatchedIndex != KErrNotFound )
            {
            if (   iConditionList[i]->ConditionItemIndex() != unmatchedIndex
                || iConditionList[i]->ConditionType() != ESensrvRangeConditionUpperLimit )
                {
                complete = EFalse;
                }
            else
                {
                unmatchedIndex = KErrNotFound;
                }
            }
        else if ( iConditionList[i]->ConditionType() == ESensrvRangeConditionLowerLimit )
            {
            // Next one in array should also be ESensrvRangeConditionUpperLimit with same index,
            // as range conditions are always inserted next to each other, with lower limit
            // being first one.
            unmatchedIndex = iConditionList[i]->ConditionItemIndex(); 
            }
        else if ( iConditionList[i]->ConditionType() == ESensrvRangeConditionUpperLimit )
            {
            complete = EFalse;
            }
        else
            {
            // Do nothing, 
            }
        }

    // Not complete, if last condition was unmatched
    if ( unmatchedIndex != KErrNotFound )
        {
        complete = EFalse;
        }
        
    return complete;        
    }

// ---------------------------------------------------------------------------
// Serializes this set to a single buffer that can be sent to server
// ---------------------------------------------------------------------------
//
HBufC8* CSensrvChannelConditionSetImpl::SerializeLC() const
    {
    COMPONENT_TRACE( ( _L( "Sensrv Util - CSensrvChannelConditionSetImpl::SerializeLC()" ) ) );

    TInt setSize(0);
    TInt conditionCount(iConditionList.Count());
    
    // Calculate the size of serialization buffer needed

    setSize += sizeof(TSensrvConditionSetType); // iSetType
    setSize += sizeof(TInt);                    // iId
    setSize += sizeof(TInt);                    // iConditionList.Count()    
        
    for (TInt i = 0; i < conditionCount; i++ ) // iConditionList
        {
        CSensrvChannelConditionImpl* condImpl = 
            static_cast<CSensrvChannelConditionImpl*>(iConditionList[i]);
            
        setSize += sizeof(TSensrvConditionType);      // CSensrvChannelCondition::iType
        setSize += sizeof(TSensrvConditionOperator);  // CSensrvChannelCondition::iOperator
        setSize += sizeof(TInt);                      // CSensrvChannelCondition::iIndex
        setSize += sizeof(TInt);                      // CSensrvChannelCondition::iValue size
        setSize += condImpl->iValue->Size();          // CSensrvChannelCondition::iValue
        }
    
    
    // Note: CSensrvChannelConditionSetImpl::iJustAdded is used only by server, 
    // so no need to serialize it
    
    // Note2: CSensrvChannelCondition::iValue size is added in conjunction of each iValue,
    // as there is no guarantee, that iValue actually is a channel data item.
    // Some custom implementations might pass some additional data there, too.

    // Create new serialization buffer    
    HBufC8* serializedSet = HBufC8::NewLC(setSize);
    
    // Append all relevant set elements to the serialization buffer
    serializedSet->Des().Append(reinterpret_cast<const TUint8*>(&iSetType), sizeof(TSensrvConditionSetType));
    serializedSet->Des().Append(reinterpret_cast<const TUint8*>(&iId), sizeof(TInt));
    serializedSet->Des().Append(reinterpret_cast<const TUint8*>(&conditionCount), sizeof(TInt));
    
    for (TInt i = 0; i < conditionCount; i++ ) 
        {
        CSensrvChannelConditionImpl* condImpl = static_cast<CSensrvChannelConditionImpl*>(iConditionList[i]);
        TInt valueSize(condImpl->iValue->Size());
        serializedSet->Des().Append(reinterpret_cast<const TUint8*>(&(condImpl->iType)), sizeof(TSensrvConditionType));
        serializedSet->Des().Append(reinterpret_cast<const TUint8*>(&(condImpl->iOperator)), sizeof(TSensrvConditionOperator));
        serializedSet->Des().Append(reinterpret_cast<const TUint8*>(&(condImpl->iIndex)), sizeof(TInt));
        serializedSet->Des().Append(reinterpret_cast<const TUint8*>(&valueSize), sizeof(TInt));
        serializedSet->Des().Append(*(condImpl->iValue));
        }
    
    COMPONENT_TRACE( ( _L( "Sensrv Util - CSensrvChannelConditionSetImpl::SerializeLC return 0x%x" ), serializedSet ) );
    
    return serializedSet;
    }

// ---------------------------------------------------------------------------
// Makes this set a clone of serialized set given in parameter
// ---------------------------------------------------------------------------
//
TInt CSensrvChannelConditionSetImpl::Deserialize( const TDesC8& aSerializedSet )
    {
    COMPONENT_TRACE( ( _L( "Sensrv Util - CSensrvChannelConditionSetImpl::Deserialize(<input>)" ) ) );
    
    TInt err(KErrNone);
    
    const TUint8* ptr(aSerializedSet.Ptr());
    
    // Get iSetType
    iSetType = *reinterpret_cast<const TSensrvConditionSetType*>(ptr);
    ptr += (sizeof(TSensrvConditionSetType)/sizeof(TUint8));
    
    // Get iId 
    iId = *reinterpret_cast<const TInt*>(ptr);
    ptr += (sizeof(TInt)/sizeof(TUint8));

    // Get condition count
    TInt conditionCount = *reinterpret_cast<const TInt*>(ptr);
    ptr += (sizeof(TInt)/sizeof(TUint8));
    
    for (TInt i = 0; err == KErrNone && i < conditionCount; i++ ) 
        {
        // Get CSensrvChannelCondition::iType
        TSensrvConditionType type = *reinterpret_cast<const TSensrvConditionType*>(ptr);
        ptr += (sizeof(TSensrvConditionType)/sizeof(TUint8));
        
        // Get CSensrvChannelCondition::iOperator
        TSensrvConditionOperator oper = *reinterpret_cast<const TSensrvConditionOperator*>(ptr);
        ptr += (sizeof(TSensrvConditionOperator)/sizeof(TUint8));
        
        // Get CSensrvChannelCondition::iIndex
        TInt index = *reinterpret_cast<const TInt*>(ptr);
        ptr += (sizeof(TInt)/sizeof(TUint8));
        
        // Get CSensrvChannelCondition::iValue size
        TInt valueSize = *reinterpret_cast<const TInt*>(ptr);
        ptr += (sizeof(TInt)/sizeof(TUint8));

        // Get CSensrvChannelCondition::iValue
        TPtrC8 value(ptr, valueSize);
        ptr += (valueSize/sizeof(TUint8));
        
        // Construct new condition and add it to this set
        CSensrvChannelCondition* condition = NULL;
        TRAP( err, condition = CSensrvChannelCondition::NewL(type, oper, index, value ) );
        
        if( KErrNone == err )
            {
            TRAP(err, AddChannelConditionL(condition));
            }
        }
    
    
    COMPONENT_TRACE( ( _L( "Sensrv Util - CSensrvChannelConditionSetImpl::Deserialize - return %d" ), err ) );
    
    return err;
    }

// ---------------------------------------------------------------------------
// Set met data.
// ---------------------------------------------------------------------------
//
TInt CSensrvChannelConditionSetImpl::SetMetData( const TDesC8& aMetData )
    {
    COMPONENT_TRACE( ( _L( "Sensrv Util - CSensrvChannelConditionSetImpl::SetMetData(<input>)" ) ) );
    
    TInt err(KErrNone);
    
    if (iMetData)
        {
        ERROR_TRACE( ( _L( "Sensrv Util - CSensrvChannelConditionSetImpl::SetMetData - ERROR: Met data already exists" ) ) );
        err = KErrAlreadyExists;
        }
    else
        {
        iMetData = aMetData.Alloc();
        
        if (!iMetData)
            {
            ERROR_TRACE( ( _L( "Sensrv Util - CSensrvChannelConditionSetImpl::SetMetData - ERROR: Met data allocation failure" ) ) );
            err = KErrNoMemory;
            }
        }
    
    COMPONENT_TRACE( ( _L( "Sensrv Util - CSensrvChannelConditionSetImpl::SetMetData - return %d" ), err ) );
    
    return err;
    }

