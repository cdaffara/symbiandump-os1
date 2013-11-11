/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Channel condition set object
*
*/


#ifndef SENSRVCHANNELCONDITIONSET_H
#define SENSRVCHANNELCONDITIONSET_H

#include <e32base.h>
#include <sensrvchannelcondition.h>

// FORWARD DECLARATIONS

class CSensrvChannelConditionSet;

// DATA TYPES

/**
* RPointerArray based channel condition set list
*/
typedef RPointerArray<CSensrvChannelConditionSet> RSensrvChannelConditionSetList;

/**
* Logical operator to be used in a condition set.
* 
* @see CSensrvChannelConditionSet
*/
enum TSensrvConditionSetType
{
    /** OR-operator for a channel condition set */
    ESensrvOrConditionSet = 0,
    /** AND-operator for a channel condtion set */
    ESensrvAndConditionSet 
};


/**
* CSensrvChannelConditionSet represents a set of conditions. A condition set is a container for one
* or more conditions.
* 
* The data type of the value contained in each Condition, in a Condition Set, must have the same data type
* as the data for the channel to which it is added. By default this should be a packaged data object of
* the channel. See the channel specific headers in \epoc32\include\sensors\channels. If the channel type
* requires a different type of value, that must be indicated clearly in the channel specific header
* defining the channel.
* 
* The channel condition set combines channel conditions with either an AND-operator or an OR-operator. In
* an AND-set, all conditions need to be met by single data item before a condition set is met. In an
* OR-set, a single condition in the set needs to be met before a condition set is met. Certain condition
* types (range conditions) require two condition objects (upper and lower limit) in a condition set for
* the set to be valid.
* 
* The pair of conditions must both have the same index in the condition set and they are considered a
* single channel condition.
*
* @see CSensrvChannelCondition
* @lib sensrvutil.lib
* @since S60 5.0
*/
NONSHARABLE_CLASS( CSensrvChannelConditionSet ): public CBase
    {
public:
    /**
    * Two-phase constructor
    * 
    * @since S60 5.0
    * @param  aConditionSetType Defines logical operator to be used for the condition set.
    * @return Pointer to created object
    * @leave  KErrNoMemory
    * @leave  One of the system-wide error codes  
    */  
    IMPORT_C static CSensrvChannelConditionSet* NewL
                ( TSensrvConditionSetType aConditionSetType );

    /**
    * Two-phase constructor
    * 
    * @since S60 5.0
    * @param  aConditionSetType Defines logical operator to be used for the condition set.
    * @return Pointer to created object
    * @leave  KErrNoMemory
    * @leave  One of the system-wide error codes
    */  
    IMPORT_C static CSensrvChannelConditionSet* NewLC
                ( TSensrvConditionSetType aConditionSetType );
    
public:
  
    /**
    * Get condition set type.
    * 
    * @return Type of the condition set
    */    
    virtual TSensrvConditionSetType ConditionSetType() const = 0;

    /**
    * Adds a channel condition to the condition set. Ownership of the CSensrvChannelCondition
    * is transferred to the CSensrvChannelConditionSet. The channel condition is deleted when
    * this condition set is destroyed.
    * 
    * Single conditions added to the Condition Set using this operation must each have a
    * different item index. Range conditions must have the same item index and they must also
    * have complimentary parts. When the second part of a range condition is added, i.e. the
    * condition has the same item index as a condition already in the set, it is checked to
    * ensure it is the complimentary partner of the already added part. If it is not
    * complimentary then the operation will leave with KErrArgument. The conditions at each
    * different index whether single or range is considered an individua condition.  
    * 
    * Individual conditions do not need to have contiguous index values.  
    * 
    * @since S60 5.0
    * @param  aChannelCondition Channel condition to be added
    * @leave  KErrArgument If the condition is NULL, there is already a condition for the same index
    *         in this set and the new condition is not a complementary part of range condition for that
    *         existing condition.
    * @leave  One of the system-wide error codes
    * @see CSensrvChannelCondition    
    */  
    virtual void AddChannelConditionL
            ( CSensrvChannelCondition* aChannelCondition ) = 0;

    /**
    * Get all channel conditions in this condition set. Conditions with the same index (range conditions)
    * are always in sequential slots in the returned array. Otherwise, conditions are in the order they
    * were added.
    * 
    * @since S60 5.0
    * @return Reference to a list of condition pointers
    */
    virtual const RSensrvChannelConditionList& AllConditions() const = 0;

public:
    /**
    * Default constructor. 
    */
    CSensrvChannelConditionSet();
    };


#endif //SENSRVCHANNELCONDITIONSET_H

// End of File


