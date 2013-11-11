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

#include <ssm/ssmstate.h>
#include "cmnpanic.h"

/**
Default constructor. Initialise this object to 0.
*/
EXPORT_C TSsmState::TSsmState() 
	: iMainState(0), iSubState(0)
	{		
	}

/**
Constructor taking aMainState and aSubState as two separate arguments.
@param aMainState The main state to be assigned to this object.
@param aSubState The sub state to be assigned to this object.
*/	
EXPORT_C TSsmState::TSsmState(TSsmMainSystemStates aMainState, TUint16 aSubState)
	: iMainState(aMainState), iSubState(aSubState)
	{
	__ASSERT_DEBUG(aMainState <= ESsmMainSystemStateMax, User::Panic(KPanicSsmCmn, ECmnStateMaxValue1));
	}

/**
Constructor taking aMainState and aSubState as two separate arguments.
@param aMainState The main state to be assigned to this object.
@param aSubState The sub state to be assigned to this object.
*/	
EXPORT_C TSsmState::TSsmState(TUint16 aMainState, TUint16 aSubState) 
	: iMainState(aMainState), iSubState(aSubState)
	{
	__ASSERT_DEBUG(aMainState <= ESsmMainSystemStateMax, User::Panic(KPanicSsmCmn, ECmnStateMaxValue2));
	}

/**
Copy constructor.
@param aState The value to be assigned to this object.
*/	
EXPORT_C TSsmState::TSsmState(const TSsmState& aState)
	: iMainState(aState.iMainState), iSubState(aState.iSubState)
	{		
	}

/**
@return The state as an unsigned integer
*/	
EXPORT_C TUint16 TSsmState::MainState() const
	{
	return iMainState;
	}

/**
@return The substate as an unsigned integer
*/
EXPORT_C TUint16 TSsmState::SubState() const
	{
	return iSubState;
	}

/**
Set member data.
@param aMainState The major state to be assigned to this object.
@param aSubState The sub state to be assigned to this object.
*/	
EXPORT_C void TSsmState::Set(TUint16 aMainState, TUint16 aSubState) 
	{		
	__ASSERT_DEBUG(aMainState <= ESsmMainSystemStateMax, User::Panic(KPanicSsmCmn, ECmnStateMaxValue3));
	iMainState = aMainState;
	iSubState = aSubState;
	}

/**
@return An integer representing the value of this object. MainState is held in the upper 16 bits
		and SubState in the lower 16 bits.
*/
EXPORT_C TUint32 TSsmState::Int() const
	{
	return (iMainState<<16 | iSubState);
	}

/**
Set member data using a 32-bit unsigned integer.
@param aState The composite value to be assigned to this object.
*/	
EXPORT_C void TSsmState::SetFromInt(TUint32 aValue) 
	{		
	__ASSERT_DEBUG(((aValue & 0xFFFF0000)>>16) <= ESsmMainSystemStateMax, User::Panic(KPanicSsmCmn, ECmnStateMaxValue4));
	iMainState = ((aValue & 0xFFFF0000)>>16);
	iSubState = (aValue & 0x0000FFFF);
	}

/**
Gives a textual description of the values assigned to this object.
E.g. ESsmStartup.KSsmAnySubState is formated as 0000.FFFF
@return A descriptor describing the TSsmState on the format XXXX.XXXX 
*/
EXPORT_C TSsmStateName TSsmState::Name() const
	{
	TSsmStateName buf;
	buf.AppendNumFixedWidth(iMainState,EHex,4);
	buf.Append(TChar('.'));
	buf.AppendNumFixedWidth(iSubState,EHex,4);
	return(buf);	
	}

/**
@param aState The value to be assigned to this object.
@return A reference to this object.
*/
EXPORT_C TSsmState& TSsmState::operator=(const TSsmState& aState)
	{
	// protect against self assignment
	if( &aState == this)
		{
		return(*this);
		}
		
	// do assigment
	iMainState = aState.iMainState;
	iSubState = aState.iSubState;	
	return(*this);
	}

/**
@param aState The value to be compared.
@return True, if the values are equal; false otherwise.
*/
EXPORT_C TBool TSsmState::operator==(const TSsmState& aState) const
	{
	if(iSubState == KSsmAnySubState || aState.iSubState == KSsmAnySubState)
		{
		return (iMainState == aState.iMainState);
		}
	else
		{
		return ((iMainState == aState.iMainState) && (iSubState == aState.iSubState));
		}
	}

/**
@param aState The value to be compared.
@return True, if the values are not equal; false otherwise.
*/
EXPORT_C TBool TSsmState::operator!=(const TSsmState& aState) const
	{
	return !(*this == aState);
	}
