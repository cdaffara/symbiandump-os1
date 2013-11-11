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

#include <ssm/ssmpublishsystemstateinfo.h>

/** 
Constructor
*/
EXPORT_C TSsmPublishSystemStateInfo::TSsmPublishSystemStateInfo()
	: iState(),
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS	
	iDirection(ETraverseDefault),
#else
	iDirection(ESsmTraverseDefault),
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
	iRetries(0)
	{
	}

/** 
Overload = operator
*/
EXPORT_C TSsmPublishSystemStateInfo& TSsmPublishSystemStateInfo::operator=(const TSsmPublishSystemStateInfo &aInfo)
	{
	// protect against self assignment
	if( &aInfo != this)
		{
		Set(aInfo.State(), aInfo.Direction(), aInfo.Retries());
		}

	return (*this);
	}

/** 
@return System State
*/
//TDmDomainState TSsmPublishSystemStateInfo::State() const
EXPORT_C TSsmState TSsmPublishSystemStateInfo::State() const
	{
	return iState;
	}

/** 
@return Direction we are traversing the State tree
*/
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
EXPORT_C TDmTraverseDirection TSsmPublishSystemStateInfo::Direction() const
#else
EXPORT_C TSsmDmTraverseDirection TSsmPublishSystemStateInfo::Direction() const
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
	{
	return iDirection;
	}

/** 
@return Number of retries on failure
*/
EXPORT_C TInt16 TSsmPublishSystemStateInfo::Retries() const
	{
	return iRetries;
	}

/** 
Set System State info
@param aState					The state to transition into
@param aDirection				The direction we are traversing the State tree
@param aRetries					Number of retires on failure
@param aSeverity				The severity of the action
@param aTransitionFailurePolicy	The policy to take on failure
*/
//void TSsmPublishSystemStateInfo::SetL(TDmDomainId aDomain, TDmDomainState aState, TDmTraverseDirection aDirection, TInt aRetries, TCmdErrorSeverity aSeverity, TDmTransitionFailurePolicy aTransitionFailurePolicy)
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
EXPORT_C void TSsmPublishSystemStateInfo::Set(const TSsmState& aState, TDmTraverseDirection aDirection, TInt16 aRetries)
#else
EXPORT_C void TSsmPublishSystemStateInfo::Set(const TSsmState& aState, TSsmDmTraverseDirection aDirection, TInt16 aRetries)
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS

	{
	iState = aState;
	iDirection = aDirection;
	iRetries = aRetries;
	}

/** 
Clear System State info contents and reset to default values
*/
EXPORT_C void TSsmPublishSystemStateInfo::Clear()
	{
	iState.SetFromInt(0);
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
	iDirection = ETraverseDefault;
#else
	iDirection = ESsmTraverseDefault;
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
	iRetries = 0;
	}

