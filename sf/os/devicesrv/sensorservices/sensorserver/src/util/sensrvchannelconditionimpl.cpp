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
* Description:  Channel condition object implementation.
*
*/


#include "sensrvchannelconditionimpl.h"
#include "sensrvtrace.h"


// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CSensrvChannelConditionImpl::CSensrvChannelConditionImpl
        ( TInt aConditionType, 
          TInt aConditionOperator,
          TInt aItemIndex ) 
    : iType(aConditionType),
      iOperator(aConditionOperator),
      iIndex(aItemIndex)
    {
    }


// ---------------------------------------------------------------------------
// 2nd phase of construction
// ---------------------------------------------------------------------------
//
void CSensrvChannelConditionImpl::ConstructL(TDesC8& aValue)
    {
    COMPONENT_TRACE( ( _L( "Sensrv Util - CSensrvChannelConditionImpl::ConstructL()" ) ) );
    
    // Check parameters    
    // Note: Type and operator are not checked for being one of the
    //       predefined ones, as it is possible that some channels
    //       require custom condition types or operations.
    if (
        // Single limit condition cannot have binary operators        
        (   iType == ESensrvSingleLimitCondition 
         && (   iOperator == ESensrvOperatorBinaryAnd
             || iOperator == ESensrvOperatorBinaryAll ) )
        // Range conditions cannot have equals or binary operators 
        || (   (   iType == ESensrvRangeConditionLowerLimit
                || iType == ESensrvRangeConditionUpperLimit ) 
            && (   iOperator == ESensrvOperatorEquals
                || iOperator == ESensrvOperatorBinaryAnd
                || iOperator == ESensrvOperatorBinaryAll ) )
        // Binary condition cannot have >, <, >= or <= operatios
        || (   (   iType == ESensrvBinaryCondition ) 
            && (   iOperator == ESensrvOperatorGreaterThan
                || iOperator == ESensrvOperatorGreaterThanOrEquals
                || iOperator == ESensrvOperatorLessThan
                || iOperator == ESensrvOperatorLessThanOrEquals ) )
        // Value must not be zero length
        || !aValue.Length()
       )
        {
        User::Leave(KErrArgument);
        }

    iValue = aValue.AllocL();
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSensrvChannelConditionImpl::~CSensrvChannelConditionImpl()
    {
    COMPONENT_TRACE( ( _L( "Sensrv Util - CSensrvChannelConditionImpl::~CSensrvChannelConditionImpl()" ) ) );
    
    delete iValue;
    }

// ---------------------------------------------------------------------------
// Return condition type
// ---------------------------------------------------------------------------
//
TInt CSensrvChannelConditionImpl::ConditionType() const
    {
    return iType;
    }

// ---------------------------------------------------------------------------
// Return condition operator
// ---------------------------------------------------------------------------
//
TInt CSensrvChannelConditionImpl::ConditionOperator() const
    {
    return iOperator;
    }

// ---------------------------------------------------------------------------
// Return condition item index
// ---------------------------------------------------------------------------
//
TInt CSensrvChannelConditionImpl::ConditionItemIndex() const
    {
    return iIndex;
    }

// ---------------------------------------------------------------------------
// Return condition value as paramater
// ---------------------------------------------------------------------------
//
TInt CSensrvChannelConditionImpl::GetConditionValue(TDes8& aValue) const
    {
    TInt err( KErrNone );
    
    if( aValue.Length() < iValue->Length() )
        {
        // aValue is too small
        err = KErrOverflow;
        }
    else
        {
        // Set return value
        TUint8* ptr = const_cast<TUint8*>( iValue->Des().Ptr() );
        aValue.Copy( ptr, iValue->Length() );
        }
    
    return err;
    }

// ---------------------------------------------------------------------------
// Condition value as reference
// ---------------------------------------------------------------------------
//
const TDesC8& CSensrvChannelConditionImpl::ConditionValue() const
    {
    return *iValue;
    }
