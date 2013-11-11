/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Stub imlementation
*
*/


#include <e32property.h>
#include <sensrvchannelcondition.h>
#include <sensrvsensorchannels.h>
#include "siftrace.h"
#include "condevalstubfirst.h"
#include "sensrvinternalpskeys.h"

//
// ----------------------------------------------------------------------------------
// CCondEvalStubFirst::CCondEvalStubFirst()
// ----------------------------------------------------------------------------------
//  
CCondEvalStubFirst::CCondEvalStubFirst(TSensrvChannelInfo* aChannelInfo)
    : iChannelInfo(*aChannelInfo)
	{	
	COMPONENT_TRACE( ( _L( "SSYStub - CCondEvalStubFirst::CCondEvalStubFirst() - Enter" ) ) );
		
	COMPONENT_TRACE( ( _L( "SSYStub - CCondEvalStubFirst::CCondEvalStubFirst() - Return" ) ) );
	}

// -----------------------------------------------------------------------------
// CCondEvalStubFirst::ConstructL(
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCondEvalStubFirst::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "SSYStub - CCondEvalStubFirst::ConstructL() - Enter" ) ) );
    
    TInt leave( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvLeaveFirstStubConstruction, leave );
    if( leave != KErrNone )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CCondEvalStubFirst::ConstructL() - Leave %d" ), leave ) );
        User::Leave( leave );
        }
    
    COMPONENT_TRACE( ( _L( "SSYStub - CCondEvalStubFirst::ConstructL() - Return" ) ) );
    }
    
//
// ----------------------------------------------------------------------------------
// CCondEvalStubFirst::NewL() 
// ----------------------------------------------------------------------------------
//  
// Two-phased constructor.
CCondEvalStubFirst* CCondEvalStubFirst::NewL(TSensrvChannelInfo* aChannelInfo)
	{
	CCondEvalStubFirst* self = new ( ELeave ) CCondEvalStubFirst(aChannelInfo);
	
	CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
	}
	
//
// ----------------------------------------------------------------------------------
// CCondEvalStubFirst::~CCondEvalStubFirst()
// ----------------------------------------------------------------------------------
// 
CCondEvalStubFirst::~CCondEvalStubFirst()
	{
    
	}


//
// ----------------------------------------------------------------------------------
// CCondEvalStubFirst::EvaluateConditionL()
// ----------------------------------------------------------------------------------
// 
TBool CCondEvalStubFirst::EvaluateConditionL(const CSensrvChannelCondition* aCondition1, 
                                    const CSensrvChannelCondition* aCondition2,  
                                    const TDesC8& aData)
    {
    TBool result(EFalse);
    
    switch ( iChannelInfo.iChannelType )
        {
        case KSensrvChannelTypeIdAccelerometerXYZAxisData:
            {
            // Note: It can be assumed that aCondition1 is not NULL
            // and that the aData contains correct data object for
            // channel type.
            // Server has also requested check for each condition validity
            // before, so index can be assumed to be one of the proper ones
            // without further checking.

            const TSensrvAccelerometerAxisData* dataPtr = 
                reinterpret_cast<const TSensrvAccelerometerAxisData*>(aData.Ptr());
                
            const TSensrvAccelerometerAxisData* condition1ValuePtr = 
                reinterpret_cast<const TSensrvAccelerometerAxisData*>(
                    aCondition1->ConditionValue().Ptr());

            TInt index(aCondition1->ConditionItemIndex());
            TInt dataValue(AccelerationValue(index, *dataPtr));
            TInt compareValue1(AccelerationValue(index, *condition1ValuePtr));
            TInt operator1(aCondition1->ConditionOperator());


            if (aCondition1->ConditionType() == ESensrvSingleLimitCondition)
                {
                RDebug::Print( _L(" # CCondEvalStubFirst::EvaluateConditionL(): dataValue: %d, compareValue1: %d"), dataValue, compareValue1 );
                result = Compare(dataValue,
                                 compareValue1,
                                 operator1);
                }
            else 
                {
                // Conditions are range conditions
                const TSensrvAccelerometerAxisData* condition2ValuePtr = 
                    reinterpret_cast<const TSensrvAccelerometerAxisData*>(
                        aCondition2->ConditionValue().Ptr());

                TInt compareValue2(AccelerationValue(index, *condition2ValuePtr));
                
                RDebug::Print( _L(" # CCondEvalStubFirst::EvaluateConditionL(): dataValue: %d, compareValue1: %d, compareValue2: %d"), dataValue, compareValue1, compareValue2 );

                TInt partialResult1(Compare(dataValue,
                                            compareValue1,
                                            operator1));
                TInt partialResult2(Compare(dataValue,
                                            compareValue2,
                                            aCondition2->ConditionOperator()));
                                            
                // Inclusive range requires both conditions to be true,
                // while exclusive range requires only one.
                // Type of range can be determined by lower limit condition operator.                                        
                if (   operator1 == ESensrvOperatorGreaterThan
                    || operator1 == ESensrvOperatorGreaterThanOrEquals )
                    {
                    // Inclusive range
                    result = (partialResult1 && partialResult2);
                    }
                else
                    {
                    // Exclusive range
                    result = (partialResult1 || partialResult2);
                    }
                }

            break;
            }
        
        case KSensrvChannelTypeIdAccelerometerWakeupData:
            {
            // Use platform default Condition Evaluator plugin instead
            break;
            }
        
        case KSensrvChannelTypeIdAccelerometerDoubleTappingData:
            {
            // Use platform default Condition Evaluator plugin insteads
            break;
            }
        default:
            break;
        }

    return result;
    }

//
// ----------------------------------------------------------------------------------
// CCondEvalStubFirst::CheckConditionValidityL()
// ----------------------------------------------------------------------------------
// 
TBool CCondEvalStubFirst::CheckConditionValidityL(const CSensrvChannelCondition* aCondition1, 
                                                 const CSensrvChannelCondition* aCondition2 )
    {
    TBool result(EFalse);
    
    switch ( iChannelInfo.iChannelType )
        {
        case KSensrvChannelTypeIdAccelerometerXYZAxisData:
            {
            // Should check here that lower and upper values of ranged conditions are
            // ok and index points to valid items. 
            // For Sensor Server module testing purposes it is ok to just check the index,
            // of the first item.
            if (   aCondition1 
                && aCondition1->ConditionItemIndex() >= 1
                && aCondition1->ConditionItemIndex() <= 3  )
                {
                result = ETrue;
                }
            break;
            }
        
        case KSensrvChannelTypeIdAccelerometerWakeupData:
            {
            // Use platform default Condition Evaluator plugin instead
            break;
            }
        
        case KSensrvChannelTypeIdAccelerometerDoubleTappingData:
            {
            // Use platform default Condition Evaluator plugin instead
            break;
            }
        default:
            break;
        }
    
    return result;
    }

//
// ----------------------------------------------------------------------------------
// CCondEvalStubFirst::AccelerationValue
// ----------------------------------------------------------------------------------
//     
TInt CCondEvalStubFirst::AccelerationValue(TInt aIndex, 
                                           const TSensrvAccelerometerAxisData& aData)
    {
    TInt retval(0);
    
    switch (aIndex)
        {
        case TSensrvAccelerometerAxisData::Index::iAxisX:
            retval = aData.iAxisX;
            break;
        case TSensrvAccelerometerAxisData::Index::iAxisY:
            retval = aData.iAxisY;
            break;
        case TSensrvAccelerometerAxisData::Index::iAxisZ:
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
// CCondEvalStubFirst::Compare
// ----------------------------------------------------------------------------------
//     
TBool CCondEvalStubFirst::Compare(TInt aValue1, 
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

