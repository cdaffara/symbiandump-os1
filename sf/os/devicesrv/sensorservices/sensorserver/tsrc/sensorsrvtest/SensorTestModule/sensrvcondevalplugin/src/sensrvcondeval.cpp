/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation
*
*/


#include <e32property.h>
#include <sensrvchannelcondition.h>
#include <sensrvsensorchannels.h>
#include "sensrvtrace.h"
#include "sensrvcondeval.h"

const TInt KSensrvCondEvalFirstValidXYZIndex(1);
const TInt KSensrvCondEvalLastValidXYZIndex(3);
const TInt KSensrvCondEvalValidTappingIndex(1);
const TInt KSensrvCondEvalMaxDirectionValue(0x3F);

//
// ----------------------------------------------------------------------------------
// CSensrvCondEval::CSensrvCondEval()
// ----------------------------------------------------------------------------------
//  
CSensrvCondEval::CSensrvCondEval(TSensrvChannelInfo* aChannelInfo)
    : iChannelInfo(*aChannelInfo)
	{	
	COMPONENT_TRACE( ( _L( "CSensrvCondEval::CSensrvCondEval() - Enter" ) ) );
	
	// Nothing to do but trace
#ifdef COMPONENT_TRACE_DEBUG
    TBuf<KSensrvLocationLength> location;
    location.Copy( aChannelInfo->iLocation );
    TBuf<KSensrvVendorIdLength> vendor;
    vendor.Copy( aChannelInfo->iVendorId );

    COMPONENT_TRACE(( _L("CSensrvCondEval::CSensrvCondEval - ChannelId=%d, ContextType=%d, Quantity=%d, ChannelType=%d, Location=%S, VendorId=%S, DataItemSize=%d"),
               aChannelInfo->iChannelId, 
               aChannelInfo->iContextType, 
               aChannelInfo->iQuantity,
               aChannelInfo->iChannelType, 
               &location, 
               &vendor, 
               aChannelInfo->iDataItemSize ));
#endif	
		
	COMPONENT_TRACE( ( _L( "CSensrvCondEval::CSensrvCondEval() - Return" ) ) );
	}

// -----------------------------------------------------------------------------
// CSensrvCondEval::ConstructL(
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSensrvCondEval::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "CSensrvCondEval::ConstructL() - Enter" ) ) );
    
    // Nothing to do
    
    COMPONENT_TRACE( ( _L( "CSensrvCondEval::ConstructL() - Return" ) ) );
    }
    
//
// ----------------------------------------------------------------------------------
// CSensrvCondEval::NewL() 
// ----------------------------------------------------------------------------------
//  
// Two-phased constructor.
CSensrvCondEval* CSensrvCondEval::NewL(TSensrvChannelInfo* aChannelInfo)
	{
	CSensrvCondEval* self = new ( ELeave ) CSensrvCondEval(aChannelInfo);
	
	CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
	}
	
//
// ----------------------------------------------------------------------------------
// CSensrvCondEval::~CSensrvCondEval()
// ----------------------------------------------------------------------------------
// 
CSensrvCondEval::~CSensrvCondEval()
	{
    // Nothing to do
	}


//
// ----------------------------------------------------------------------------------
// Evaluates condition(s) against given data to determine if match is found.
// ----------------------------------------------------------------------------------
// 
TBool CSensrvCondEval::EvaluateConditionL(const CSensrvChannelCondition* aCondition1, 
                                          const CSensrvChannelCondition* aCondition2,  
                                          const TDesC8& aData)
    {
    TBool result(EFalse);
    
    // Note: Server guarantees that aCondition1 is not NULL
    // and that the aData contains correct data object for
    // this channel.
    // Server has also requested check for each condition validity
    // before calling this method, so it can be assumed that the index is one 
    // of the valid ones and that the conditions are of a valid type
    // without further checking.

    switch ( iChannelInfo.iChannelType )
        {
        case KSensrvChannelTypeIdAccelerometerXYZAxisData:
            {
            const TSensrvAccelerometerAxisData* dataPtr = 
                reinterpret_cast<const TSensrvAccelerometerAxisData*>(aData.Ptr());
                
            const TSensrvAccelerometerAxisData* condition1ValuePtr = 
                reinterpret_cast<const TSensrvAccelerometerAxisData*>(
                    aCondition1->ConditionValue().Ptr());

            TInt index(aCondition1->ConditionItemIndex());
            TInt dataValue(AccelerationValue(index, *dataPtr));
            TInt compareValue1(AccelerationValue(index, *condition1ValuePtr));
            TInt operator1(aCondition1->ConditionOperator());

            if ( aCondition1->ConditionType() == ESensrvSingleLimitCondition )
                {
                COMPONENT_TRACE( (_L(" # CSensrvCondEval::EvaluateCondition(): KSensrvChannelTypeIdAccelerometerXYZAxisData - dataValue: %d, compareValue1: %d"), dataValue, compareValue1 ) ) ;
                result = Compare( dataValue,
                                  compareValue1,
                                  operator1 );
                }
            else 
                {
                // Conditions are range conditions
                const TSensrvAccelerometerAxisData* condition2ValuePtr = 
                    reinterpret_cast<const TSensrvAccelerometerAxisData*>(
                        aCondition2->ConditionValue().Ptr());

                TInt compareValue2(AccelerationValue(index, *condition2ValuePtr));
                
                COMPONENT_TRACE(( _L(" # CSensrvCondEval::EvaluateCondition(): KSensrvChannelTypeIdAccelerometerXYZAxisData - dataValue: %d, compareValue1: %d, compareValue2: %d"), dataValue, compareValue1, compareValue2 ));

                TInt partialResult1( Compare( dataValue,
                                              compareValue1,
                                              operator1 ) );
                TInt partialResult2( Compare( dataValue,
                                              compareValue2,
                                              aCondition2->ConditionOperator() ) );
                                            
                // Inclusive range requires both conditions to be true,
                // while exclusive range requires only one.
                // Type of range can be determined by lower limit condition operator.                                        
                if (   operator1 == ESensrvOperatorGreaterThan
                    || operator1 == ESensrvOperatorGreaterThanOrEquals )
                    {
                    // Inclusive range
                    result = ( partialResult1 && partialResult2 );
                    }
                else
                    {
                    // Exclusive range
                    result = ( partialResult1 || partialResult2 );
                    }
                }

            break;
            }
        
        // The following channels use same data type
//        case KSensrvChannelTypeIdAccelerometerWakeupData:
        case KSensrvChannelTypeIdAccelerometerDoubleTappingData:
            {
            const TSensrvTappingData* dataPtr = 
                reinterpret_cast<const TSensrvTappingData*>(aData.Ptr());
                
            const TSensrvTappingData* condition1ValuePtr = 
                reinterpret_cast<const TSensrvTappingData*>(
                    aCondition1->ConditionValue().Ptr());
            
            COMPONENT_TRACE(( _L(" # CSensrvCondEval::EvaluateCondition(): dataValue: 0x%x, compareValue1: 0x%x"), dataPtr->iDirection, condition1ValuePtr->iDirection ));
            
            switch ( aCondition1->ConditionOperator() )
                {
                case ESensrvOperatorEquals:
                    {
                    // Exact match of all bits required, whether set or not
                    result = ( condition1ValuePtr->iDirection == dataPtr->iDirection );
                    break;
                    }
                case ESensrvOperatorBinaryAnd:
                    {
                    // At least one set bit must match data
                    result = ( condition1ValuePtr->iDirection & dataPtr->iDirection );
                    break;
                    }
                case ESensrvOperatorBinaryAll:
                    {
                    // All set bits must match data, rest are not relevant                    
                    result = ( ( condition1ValuePtr->iDirection & dataPtr->iDirection ) == condition1ValuePtr->iDirection );
                    break;
                    }
                default:
                    {
                    // Cannot actually get here unless there is an error somewhere
                    ERROR_TRACE( (_L(" # CSensrvCondEval::EvaluateCondition() - ERROR: Invalid operator: %d"), aCondition1->ConditionOperator() ) ) ;
                    break;
                    }
                }
            break;
            }
        default:
            break;
        }

    return result;
    }

//
// ----------------------------------------------------------------------------------
// Checks that condition is valid
// ----------------------------------------------------------------------------------
// 
TBool CSensrvCondEval::CheckConditionValidityL(const CSensrvChannelCondition* aCondition1, 
                                               const CSensrvChannelCondition* aCondition2 )
    {
    TBool result(EFalse);
    
    switch ( iChannelInfo.iChannelType )
        {
        case KSensrvChannelTypeIdAccelerometerXYZAxisData:
            {
            // Sensor server guarantees that both conditions point to same index,
            // so index check needed for only first condition.
            if (   aCondition1 
                && aCondition1->ConditionItemIndex() >= KSensrvCondEvalFirstValidXYZIndex
                && aCondition1->ConditionItemIndex() <= KSensrvCondEvalLastValidXYZIndex
                && (   aCondition1->ConditionType() == ESensrvSingleLimitCondition 
                    || aCondition1->ConditionType() == ESensrvRangeConditionLowerLimit )  )
                {

                // Check that lower and upper values of ranged conditions are ok.
                // If there are two conditions, it means this is ranged condition.
                if (    aCondition2
                     && aCondition1->ConditionType() == ESensrvRangeConditionLowerLimit
                     && aCondition2->ConditionType() == ESensrvRangeConditionUpperLimit )            
                    {
                    const TSensrvAccelerometerAxisData* condition1ValuePtr = 
                        reinterpret_cast<const TSensrvAccelerometerAxisData*>(
                            aCondition1->ConditionValue().Ptr());

                    const TSensrvAccelerometerAxisData* condition2ValuePtr = 
                        reinterpret_cast<const TSensrvAccelerometerAxisData*>(
                            aCondition2->ConditionValue().Ptr());

                    TInt index(aCondition1->ConditionItemIndex());
                    TInt compareValue1(AccelerationValue(index, *condition1ValuePtr));
                    TInt compareValue2(AccelerationValue(index, *condition2ValuePtr));
                    TInt diff = compareValue2 - compareValue1;
                    TBool isInclusive(EFalse);
                    TInt equalityCount(0);
                    
                    if (   aCondition1->ConditionOperator() == ESensrvOperatorGreaterThan
                        || aCondition1->ConditionOperator() == ESensrvOperatorGreaterThanOrEquals )
                        {
                        isInclusive = ETrue;
                        
                        if ( aCondition1->ConditionOperator() == ESensrvOperatorGreaterThanOrEquals )
                            {
                            equalityCount++;
                            }
                            
                        if ( aCondition2->ConditionOperator() == ESensrvOperatorLessThanOrEquals )
                            {
                            equalityCount++;
                            }
                        }
                    
                    // Note: Magic numbers used for readability. These values cannot change
                    //       without breaking the algorithm, so no real need for constants.
                    if ( !diff )
                        {
                        // Both limits are the same
                        if ( !isInclusive || equalityCount == 2 )
                            {
                            result = ETrue;
                            }
                        else
                            {
                            // Inclusive range where both of the limits don't include equality check,
                            // so condition can never be matched -> result defaults to EFalse
                            // E.g. (x>100, x<=100) -> No possible match
                            }
                        }
                    else if ( diff == 1 )
                        {
                        // Limits are adjacent
                        if ( !isInclusive || equalityCount >= 1 )
                            {
                            result = ETrue;
                            }
                        else
                            {
                            // Inclusive range where neither of the limits don't include equality check,
                            // so condition can never be matched -> result defaults to EFalse
                            // E.g. (x>100, x<101) -> No possible match
                            }
                        }
                    else if ( compareValue1 < compareValue2 )
                        {
                        // Nonadjacent values where upper limit is bigger than lower limit 
                        // are always valid.
                        result = ETrue;
                        }
                    else
                        {
                        // Lower limit is bigger than upper limit -> result defaults to EFalse
                        }
                    }
                else
                    {
                    // Single condition only, value is always ok
                    result = ETrue;
                    }
                }
            else
                {
                // Invalid index or condition type, result defaults to EFalse
                }
            break;
            }
        
        // The following channels use same data type
//        case KSensrvChannelTypeIdAccelerometerWakeupData:
        case KSensrvChannelTypeIdAccelerometerDoubleTappingData:
            {
            // Only binary conditions for direction field are valid
            if (    aCondition1 
                 && aCondition1->ConditionItemIndex() == KSensrvCondEvalValidTappingIndex
                 && aCondition1->ConditionType() == ESensrvBinaryCondition  )
                {
                const TSensrvTappingData* condition1ValuePtr = 
                    reinterpret_cast<const TSensrvTappingData*>(
                        aCondition1->ConditionValue().Ptr());

                // Check that value contains only valid values. 
                // Condition must specify at least one direction.
                if (    condition1ValuePtr->iDirection > 0 
                     && condition1ValuePtr->iDirection <= KSensrvCondEvalMaxDirectionValue )
                    {
                    result = ETrue;
                    }
                }
            break;
            }
        default:
            ERROR_TRACE( (_L(" # CSensrvCondEval::CheckConditionValidity() - ERROR:  iChannelInfo.iChannelType: %d"),  iChannelInfo.iChannelType ) ) ;
            break;
        }
    
    return result;
    }

//
// ----------------------------------------------------------------------------------
// CSensrvCondEval::AccelerationValue
// ----------------------------------------------------------------------------------
//     
TInt CSensrvCondEval::AccelerationValue(TInt aIndex, 
                                        const TSensrvAccelerometerAxisData& aData)
    {
    TInt retval(0);
    
    switch (aIndex)
        {
        case TSensrvAccelerometerAxisData::EAxisX:
            retval = aData.iAxisX;
            break;
        case TSensrvAccelerometerAxisData::EAxisY:
            retval = aData.iAxisY;
            break;
        case TSensrvAccelerometerAxisData::EAxisZ:
            retval = aData.iAxisZ;
            break;
        default:
            // Never gets here
            break;
        }
        
    return retval;
    }
    
//
// ----------------------------------------------------------------------------------
// CSensrvCondEval::Compare
// ----------------------------------------------------------------------------------
//     
TBool CSensrvCondEval::Compare(TInt aValue1, 
                               TInt aValue2, 
                               TInt aOperator)
    {
    TBool retval(EFalse);
    
    switch (aOperator)
        {
        case ESensrvOperatorEquals:
            retval = (aValue1 == aValue2);
            break;
        case ESensrvOperatorGreaterThan:
            retval = (aValue1 > aValue2);
            break;
        case ESensrvOperatorGreaterThanOrEquals:
            retval = (aValue1 >= aValue2);
            break;
        case ESensrvOperatorLessThan:
            retval = (aValue1 < aValue2);
            break;
        case ESensrvOperatorLessThanOrEquals:
            retval = (aValue1 <= aValue2);
            break;
        default:
            break;
        }
    return retval;
    }    

