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
* Description:  Channel condition object
*
*/


#ifndef SENSRVCHANNELCONDITION_H
#define SENSRVCHANNELCONDITION_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS

class CSensrvChannelCondition;

// DATA TYPES

/**
* RPointerArray based channel condition list
*/
typedef RPointerArray<CSensrvChannelCondition> RSensrvChannelConditionList;

/**
* Defines condition type.
* 
* Conditions can be standalone conditions or a part of a two-part condition.
* 
* - ESensrvLimitCondition is a standalone condition which has a limit. This kind of condition
*   is met when the observed value exceeds or equals the condition value, depending on the
*   operator used in the condition.
* - ESensrvRangeConditionLowerLimit is the first part of a two-part range condition. This kind
*   of condition is met when the observed value is between specified lower and upper limits or
*   beyond specified range, depending on the operators used in the two conditions that make up
*   the range.
*   A condition set that contains ESensrvRangeConditionLowerLimit condition must also contain
*   ESensrvRangeConditionUpperLimit condition using the same index. Both lower and upper limit
*   operators must have different direction, i.e. if lower limit has "less than" operator, then
*   upper limit must have "greater than" operator, and vice versa. ESensrvOperatorEquals operator
*   is not valid for range condition.
* - ESensrvRangeConditionUpperLimit is the second part of a two-part range condition.
* - ESensrvBinaryCondition is used for channels that provide bitmask values (typically event
*   channels). ESensrvOperatorBinaryXxx operations can only be used with this condition type.
* 
* @see CSensrvChannelCondition
* @see TSensrvConditionOperator
*/
enum TSensrvConditionType
    {
    /** Standalone condition which has a limit */
    ESensrvSingleLimitCondition = 0,
    /** 1st part of a two-part range condition. 2nd part is ESensrvRangeConditionUpperLimit */
    ESensrvRangeConditionLowerLimit,
    /** 2nd part of a two-part range condition. 1st part is ESensrvRangeConditionLowerLimit */
    ESensrvRangeConditionUpperLimit,
    /** Standalone condition for binary operations */
    ESensrvBinaryCondition
    };

/*
* Defines condition operator
* 
* For a range condition a ConditionSet must contain 2 Conditions each with an operator in a
* different direction. i.e. There must be matching greater than and less than pair. Also the pair
* of conditions must have the same index in the ConditionSet.
* 
* - ESensrvOperatorEquals checks if the value got from the sensor is equal to the set value. This is
*   not a valid operator for a range condition.
* - ESensrvOperatorGreaterThan checks if the value got from the sensor is greater than the set limit. 
*   This is not a valid operator for a binary condition.
* - ESensrvOperatorGreaterThanOrEquals checks if the value got from the sensor is greater than or equal
*   to the set limit. This is not a valid operator for a binary condition.
* - ESensrvOperatorLessThan checks if the value got from the sensor is less than the set limit. This is
*   not a valid operator for a binary condition.
* - ESensrvOperatorLessThanOrEquals checks if the value got from the sensor is less than or equal to the
*   set limit. This is not a valid operator for a binary condition.
* - ESensrvOperatorBinaryAnd checks if a bitmask data value got from the sensor has set at least one of
*   the bits set in the condition value. In other words, if result of datavalue & conditionvalue != 0,
*   there is a match. This is not a valid operator for a single limit or range conditions.
* 
*   Example: 
*   Condition value: 01101101
*   Data value:      10001010
*   Resulting value: 00001000 -> Non-zero, which indicates a match.
* - ESensrvOperatorBinaryAll checks if a bitmask data value got from the sensor has set all of the bits
*   set in the condition value. The rest of the data value bits are ignored. In other words, if
*   datavalue & conditionvalue == conditionvalue, there is a match. Not a valid operator for a single limit
*   or range conditions.
*   Example: 
*   Condition value: 01101101
*   Data value:      10001010
*   Resulting value: 00001000 -> Not equal to condition value -> Not a match.
* 
* @see CSensrvChannelCondition
* @see TSensrvConditionType
*/
enum TSensrvConditionOperator
    {
    ESensrvOperatorEquals = 0,
    ESensrvOperatorGreaterThan,
    ESensrvOperatorGreaterThanOrEquals,
    ESensrvOperatorLessThan,
    ESensrvOperatorLessThanOrEquals,
    ESensrvOperatorBinaryAnd,
    ESensrvOperatorBinaryAll    
    };

// CLASS DECLARATION

/**
* CSensrvChannelCondition represents a channel condition item. Channel conditions are added to a channel
* condition set. A channel condition set is met when all channel conditions are met at the same time, if it
* is AND-type set, or when single condition is met if it is OR-type set. Only one channel condition can be
* set for each data item in single channel condition set. Certain condition types (range conditions) require
* two condition objects (upper and lower limit) in a condition set for the set to be valid. The pair of
* conditions must both have the same index in the condition set and they are considered a single channel
* condition. 
* 
* Each Condition has an ItemIndex associated with it. When a Condition is added to a Condition Set using
* CSensrvChannelConditionSet::AddChannelConditionL() this index must be different to all other conditions
* added to the set with the exception of range conditions as described above. If the index supplied already
* exists and the condition is not part of a range condition then
* CSensrvChannelConditionSet::AddChannelConditionL() leaves with KErrArgument.  
* 
* @see CSensrvChannelConditionSet
* @see TSensrvConditionType
* @see TSensrvConditionOperator
* @lib sensrvutil.lib
* @since S60 5.0
*/
NONSHARABLE_CLASS( CSensrvChannelCondition ): public CBase
    {
public:
    /**
    * Two-phase constructor
    * 
    * @param   aConditionType Defines the type of the condition
    * @param   aConditionOperator Defines the operator used in the condition
    * @param   aItemIndex Item index to be used in the condition evaluation
    * @param   aValue Value to be used in condition evaluation. By default this should be a packaged data
    *         object of the channel. See the channel specific headers in \epoc32\include\sensors\channels.
    *         If the channel type requires a different type of value, that must be indicated clearly in
    *        the channel specific header defining the channel.
    * @return Pointer to created object
    * @leave  KErrNoMemory
    * @leave  KErrArgument if parameters are invalid or are less than 0
    * @leave  One of the system-wide error codes
    */  
    IMPORT_C static CSensrvChannelCondition* NewL
                ( TInt aConditionType, 
                  TInt aConditionOperator,
                  TInt aItemIndex,
                  TDesC8& aValue );
                  
    /**
    * Two-phase constructor
    * 
    * @param   aConditionType Defines the type of the condition
    * @param   aConditionOperator Defines the operator used in the condition
    * @param   aItemIndex Item index to be used in the condition evaluation
    * @param   aValue Value to be used in condition evaluation. By default this should be a packaged data
    *         object of the channel. See the channel specific headers in \epoc32\include\sensors\channels.
    *         If the channel type requires a different type of value, that must be indicated clearly in
    *         the channel specific header defining the channel.
    * @return Pointer to created object
    * @leave  KErrNoMemory
    * @leave  KErrArgument if parameters are invalid or are less than 0
    * @leave  One of the system-wide error codes
    */  
    IMPORT_C static CSensrvChannelCondition* NewLC
                ( TInt aConditionType, 
                  TInt aConditionOperator,
                  TInt aItemIndex,
                  TDesC8& aValue );
public:  
    
    /**
    * Get condition type
    *  
    * @return Condition Type
    */    
    virtual TInt ConditionType() const = 0;

    /**
    * Get condition operator
    *
    * @return TInt Condition operator
    */      
    virtual TInt ConditionOperator() const = 0;
  
    /**
    * Get condition item index
    *
    * @return TInt Condition item index
    */      
    virtual TInt ConditionItemIndex() const = 0;

    /**
    * Get condition value.
    *
    * @param  aValue Value of the condition is copied to the supplied descriptor
    * @return KErrOverflow if aData is the wrong size for data item or one of the system-wide error codes
    */      
    virtual TInt GetConditionValue( TDes8& aValue ) const = 0;        

    /**
    * Get condition value as reference.
    * 
    * @return Reference to condition value descriptor.
    */      
    virtual const TDesC8& ConditionValue() const = 0;        

public:
    /**
    * Default constructor. 
    */
    CSensrvChannelCondition();
    };

#endif //SENSRVCHANNELCONDITION_H

// End of File


