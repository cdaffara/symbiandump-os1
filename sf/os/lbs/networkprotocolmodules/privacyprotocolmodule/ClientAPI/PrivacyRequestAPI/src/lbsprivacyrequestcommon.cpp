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

#include <e32base.h>
#include "lbsprivacyrequestcommon.h"
#include "lbsdevloggermacros.h"


/**
 Returns the class type.
 
 @return An integer code representing the class type.
 @see TLbsPrivacyRequestClassType
*/
EXPORT_C TUint32 TLbsPrivacyClassTypeBase::ClassType() const
	{
	return iClassType;
	}

/**
 Returns the class size.
 
 @return An integer representing the class size.
*/
EXPORT_C TUint TLbsPrivacyClassTypeBase::ClassSize() const
	{
	return iClassSize;
	}

/**
 Default constructor for TLbsPrivacyRequest
*/
EXPORT_C TLbsPrivacyRequest::TLbsPrivacyRequest()
	: TLbsPrivacyClassTypeBase(ELbsPrivacyRequestClass, sizeof(TLbsPrivacyRequest)), 
	  iAction(ERequestActionNotUsed), iAdvice(ERequestAdviceNotUsed)
	{
	}

/**
 Returns the request advice.
 Value returned is from enumeration TLbsPrivacyRequest::_TLbsRequestAdvice
 
 @return The request advice.
 @see TLbsPrivacyRequest::_TLbsRequestAdvice
*/
EXPORT_C TLbsPrivacyRequest::TLbsRequestAdvice TLbsPrivacyRequest::RequestAdvice() const
	{
	return iAdvice;
	}

/**
 Returns for the request action.
 Value returned is from enumeration TLbsPrivacyRequest::_TLbsRequestAction
 
 @return The request action.
 @see TLbsPrivacyRequest::_TLbsRequestAction
*/
EXPORT_C TLbsPrivacyRequest::TLbsRequestAction TLbsPrivacyRequest::RequestAction() const
	{
	return iAction;
	}

/**
 Sets for the request advice.
  
 @param aAdvice The request advice to be set. Must be from enum TLbsPrivacyRequest::_TLbsRequestAdvice
*/
EXPORT_C void TLbsPrivacyRequest::SetRequestAdvice(TLbsPrivacyRequest::TLbsRequestAdvice aAdvice)
	{
	iAdvice = aAdvice;
	}

/**
 Setter for the request action.
 
 @param aAdvice The request action to be set. Must be from enum TLbsPrivacyRequest::_TLbsRequestAction
*/
EXPORT_C void TLbsPrivacyRequest::SetRequestAction(TLbsPrivacyRequest::TLbsRequestAction aAction)
	{
	iAction = aAction;
	}
