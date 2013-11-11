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

#include <ssm/ssmstatetransition.h>

EXPORT_C TSsmStateTransition::TSsmStateTransition()
	: iState(0,0), iReason(0)
	{
	}

/**
@param aState	The target system state.
@param aReason	A parameter interpreted by the System State Policy DLL implementation.
*/
EXPORT_C TSsmStateTransition::TSsmStateTransition(TSsmState aState, TInt aReason)
	: iState(aState), iReason(aReason)
	{
	} //lint !e1746 Suppress parameter 'aState' could be made const reference

/**
@param aMainState The target major state.
@param aSubState The target sub state.
@param aReason	A parameter interpreted by the System State Policy DLL implemtation.
*/
EXPORT_C TSsmStateTransition::TSsmStateTransition(TUint16 aMainState, TUint16 aSubState, TInt aReason)
	: iState(aMainState,aSubState), iReason(aReason)
	{
	}

/**
 Copy constructor
 */
EXPORT_C TSsmStateTransition::TSsmStateTransition(const TSsmStateTransition& aRequest)
	: iState(aRequest.iState), iReason(aRequest.iReason)
	{
	}

/**
 Accessor for the System State attribute
 @return A copy of the iState attribute in this object
 */
EXPORT_C TSsmState TSsmStateTransition::State() const
	{
	return iState;
	}

/**
 Accessor for the Reason attribute
 @return A copy of the iReason attribute in this object
 */
EXPORT_C TInt TSsmStateTransition::Reason() const
	{
	return iReason;
	}

/**
@param aRequest The value to be assigned to this object.
@return A reference to this object.
*/
EXPORT_C TSsmStateTransition& TSsmStateTransition::operator=(const TSsmStateTransition& aRequest)
	{
	// protect against self assignment
	if( &aRequest == this)
		{
		return(*this);
		}
		
	// do assigment
	iState = aRequest.iState;
	iReason = aRequest.iReason;	
	
	return(*this);
	}

/**
@param aRequest The value to be compared.
@return True, if the values are equal; false otherwise.
*/
EXPORT_C TBool TSsmStateTransition::operator==(const TSsmStateTransition& aRequest) const
	{
	return ((iState==aRequest.iState) && (iReason==aRequest.iReason));	
	}

/**
@param aRequest The value to be compared.
@return True, if the values are not equal; false otherwise.
*/
EXPORT_C TBool TSsmStateTransition::operator!=(const TSsmStateTransition& aRequest) const
	{
		return !(*this == aRequest);
	}
