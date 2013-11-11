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
#include "siftrace.h"
#include "condevalstubsecond.h"
#include "sensrvchanneldatatypes.h"
#include "sensrvinternalpskeys.h"


//
// ----------------------------------------------------------------------------------
// CCondEvalStubSecond::CCondEvalStubSecond()
// ----------------------------------------------------------------------------------
//  
CCondEvalStubSecond::CCondEvalStubSecond(TSensrvChannelInfo* aChannelInfo)
    : iChannelInfo(*aChannelInfo)
	{	
	COMPONENT_TRACE( ( _L( "SSYStub - CCondEvalStubSecond::CCondEvalStubSecond() - Enter" ) ) );
		
	COMPONENT_TRACE( ( _L( "SSYStub - CCondEvalStubSecond::CCondEvalStubSecond() - Return" ) ) );
	}

// -----------------------------------------------------------------------------
// CCondEvalStubSecond::ConstructL(
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCondEvalStubSecond::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "SSYStub - CCondEvalStubSecond::ConstructL() - Enter" ) ) );
    
    TInt leave( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvLeaveSecondStubConstruction, leave );
    if( leave != KErrNone )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CCondEvalStubSecond::ConstructL() - Leave %d" ), leave ) );
        User::Leave( leave );
        }
        
    TInt jam( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvStubConstructionJam, jam );
    if( jam )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CCondEvalStubSecond::ConstructL() - Construction JAM" ) ) );
        TRequestStatus status; 
        User::WaitForRequest( status );
        }
        
    TInt delay( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvStubConstructionDelay, delay );
    
    if( delay > 0 )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubSecond::ConstructL() - delay %d second" ), delay ) );
        User::After( delay * 1000000 ); // microseconds
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubSecond::ConstructL() - continue construction" ) ) );
        }
    
    COMPONENT_TRACE( ( _L( "SSYStub - CCondEvalStubSecond::ConstructL() - Return" ) ) );
    }
    
//
// ----------------------------------------------------------------------------------
// CCondEvalStubSecond::NewL() 
// ----------------------------------------------------------------------------------
//  
CCondEvalStubSecond* CCondEvalStubSecond::NewL(TSensrvChannelInfo* aChannelInfo)
	{
	CCondEvalStubSecond* self = new ( ELeave ) CCondEvalStubSecond(aChannelInfo);
	
	CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
	}
	
//
// ----------------------------------------------------------------------------------
// CCondEvalStubSecond::~CCondEvalStubSecond()
// ----------------------------------------------------------------------------------
// 
CCondEvalStubSecond::~CCondEvalStubSecond()
	{
    
	}


//
// ----------------------------------------------------------------------------------
// CCondEvalStubSecond::EvaluateConditionL()
// ----------------------------------------------------------------------------------
// 
TBool CCondEvalStubSecond::EvaluateConditionL(const CSensrvChannelCondition* aCondition1, 
                                    const CSensrvChannelCondition* aCondition2,  
                                    const TDesC8& aData)
    {
    TBool result(EFalse);
    
    switch ( iChannelInfo.iChannelType )
        {
        case KSensrvTestEventChannelType:
            {
            const TSensrvTestEventData* dataPtr = 
                reinterpret_cast<const TSensrvTestEventData*>(aData.Ptr());
                
            // Example of having custom value instead of regular channel data item type
            const TTime* condition1ValuePtr = 
                reinterpret_cast<const TTime*>(
                    aCondition1->ConditionValue().Ptr());

            TInt index(aCondition1->ConditionItemIndex());
            TTime dataValue;
            TimestampValue(index, *dataPtr, dataValue);
            TInt operator1(aCondition1->ConditionOperator());
            
            RDebug::Print( _L(" # CCondEvalStubSecond::EvaluateConditionL(): dataValue: %ld, condition1ValuePtr: %ld"), dataValue.Int64(), condition1ValuePtr->Int64() );

            if (aCondition1->ConditionType() == ESensrvSingleLimitCondition)
                {
                result = Compare(dataValue,
                                 *condition1ValuePtr,
                                 operator1);
                }
            else 
                {
                // Conditions are range conditions
                const TTime* condition2ValuePtr = 
                    reinterpret_cast<const TTime*>(
                        aCondition2->ConditionValue().Ptr());

                TInt partialResult1(Compare(dataValue,
                                            *condition1ValuePtr,
                                            operator1));
                TInt partialResult2(Compare(dataValue,
                                            *condition2ValuePtr,
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
        default:
            break;
        }

    return result;
    }

//
// ----------------------------------------------------------------------------------
// CCondEvalStubSecond::CheckConditionValidityL()
// ----------------------------------------------------------------------------------
// 
TBool CCondEvalStubSecond::CheckConditionValidityL(const CSensrvChannelCondition* aCondition1, 
                                                 const CSensrvChannelCondition* aCondition2 )
    {
    TBool result(EFalse);
    
    switch ( iChannelInfo.iChannelType )
        {
        case KSensrvTestEventChannelType:
            {
            // Check that condition value can be converted to timestamp
            if ( aCondition1->ConditionValue().Length() == 8 )
                {
                result = ETrue;
                
                if (aCondition2 && aCondition2->ConditionValue().Length() != 8)
                    {
                    result = EFalse;
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
// CCondEvalStubSecond::AccelerationValue
// ----------------------------------------------------------------------------------
//     
void CCondEvalStubSecond::TimestampValue(TInt aIndex,
                                         const TSensrvTestEventData& aData,
                                         TTime& aReturnValue)
    {
    switch (aIndex)
        {
        case TSensrvTestEventData::Index::iTimeStampFromEventPlugin:
            aReturnValue = aData.iTimeStampFromEventPlugin;
            break;
        case TSensrvTestEventData::Index::iTimeStampFromAccelerometerPlugin:
            aReturnValue = aData.iTimeStampFromAccelerometerPlugin;
            break;
        default:
            // Never gets here
            break;
        }
        
    return;
    }
    
//
// ----------------------------------------------------------------------------------
// CCondEvalStubSecond::Compare
// ----------------------------------------------------------------------------------
//     
TBool CCondEvalStubSecond::Compare(const TTime& aValue1, const TTime& aValue2, TInt aOperator)
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
