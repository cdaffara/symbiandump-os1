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
// This file defines the common data structures used by the
// Privacy Request interface. 
// 
//

/**
 @file
 @publishedPartner
 @prototype
*/

#include <lbs/lbsprivacyrequestclasstypes.h>

#ifndef LBSPRVREQUESTCOMMON_H_
#define LBSPRVREQUESTCOMMON_H_

/**
The generic base class for classes that store LBS privacy related information.
@publishedPartner
@prototype
@see TLbsPrivacyRequest
 */
class TLbsPrivacyClassTypeBase
	{
public:
	IMPORT_C TUint32 ClassType() const;
	IMPORT_C TUint ClassSize() const;

protected:
	/**
	 Constructor that takes a class type and class size as parameters
	 @param aClassType the type of the class
	 @param aClassSize the size of the class
	 */
	inline TLbsPrivacyClassTypeBase(TUint32 aClassType, TUint aClassSize)
		: iClassType(aClassType), iClassSize(aClassSize)
	{}

protected:
	/** The type of the derived class.
	 @see TLbsPrivacyRequestClassType
	 */
	TUint32 iClassType;
	/** The size of the derived class */
	TUint iClassSize;
	};


/**
A class that wraps privacy request parameters.

An instance of the class TLbsPrivacyRequest is passed to the LBS subsystem when a local or 
remote privacy request is sent via the Privacy Request API. 
The class is used to indicate what advice should be given to the user. For example, this may advise
that the user is required to verify the request or is simply notified about it.

The class also specifies whether the request should be accepted or rejected. In the situation
where the user must verify the request, the action is the default behaviour when the user does not respond.

For example, if the advice is set to ERequestAdviceVerify and the action is ERequestActionAllow,
this indicates that the user should be asked if they wish to accept or reject the request. 
However, if the user does not respond, the LBS subsystem should automatically accept the request.

Alternatively, the user may be notified that a request has been rejected. In this situation the advice 
is set to ERequestAdviceNotify with an action of ERequestActionReject.
	
@publishedPartner
@prototype
*/
class TLbsPrivacyRequest : public TLbsPrivacyClassTypeBase
	{
public:
	/** Defined type for TLbsRequestAdvice */
	typedef TUint32 TLbsRequestAdvice;


 	/** Note that the TLbsRequestAdvice enum may be extended in the future by adding
 	more enumerated values. To maintain compatibility any unrecognized values 
 	must be handled as ERequestAdviceNotUsed. */
	enum _TLbsRequestAdvice
		{
				/** Not Used */
		ERequestAdviceNotUsed = 0,
				/** The LBS subsystem should notify the user that their
				location has been requested. */
		ERequestAdviceNotify,
				/** The LBS subsystem should ask the user if they wish
				to accept or reject the request. */
		ERequestAdviceVerify,
				/** The LBS subsystem should neither notify or query the user if they wish
				to accept or reject a request. Details of the request should be logged. */
		ERequestAdviceSilent,
				/** No indication should be given to a user. The user should neither be 
				notified nor queried. In addition, no information about the request should be logged.
				This option is defined by some network protocols (e.g. SUPL) but is not implemented
				by the LBS subsystem. */
		ERequestAdviceStealth
		};
		

	/** Defined type for TLbsRequestAction.*/
	typedef TUint32 TLbsRequestAction;

 	/** Note that the TLbsRequestAction enum may be extended in the future by adding
 	more enumerated values. To maintain compatibility any unrecognized values 
 	must be handled as ERequestActionNotUsed. */
	enum _TLbsRequestAction 
		{
				/** Not Used */
		ERequestActionNotUsed = 0,
				/** The default action for LBS subsystem should be to accept the request.
				If this value is passed in combination with ERequestAdviceVerify, it
				indicates that the user should be asked if they wish to accept or reject
				the request. However, if the user does not respond, the request should
				be allowed. 
				
				In most situations the privacy response of the user should be used to decide
				whether to accept a location request. Therefore in most cases a privacy advice
				of ERequestAdviceNotify and a privacy action of ERequestActionAllow should
				not be used together, although it is not an error to do so.							
				*/
		ERequestActionAllow,
				/** The default action for LBS subsystem should be to reject the request.
				If this value is passed in combination with ERequestAdviceVerify, it
				indicates that the user should asked if they wish to accept or reject
				the request. However, if the user does not respond, the request should
				be rejected.
		
				If ERequestActionReject is passed in combination with ERequestAdviceNotify,
				it indicates that the user should be informed that a request has been made
				but the terminal should	automatically reject it.
				*/
		ERequestActionReject,
		};
	
	IMPORT_C TLbsPrivacyRequest();

	IMPORT_C TLbsRequestAdvice RequestAdvice() const;
	
	IMPORT_C TLbsRequestAction RequestAction() const;

	IMPORT_C void SetRequestAdvice(TLbsRequestAdvice aAdvice);
	
	IMPORT_C void SetRequestAction(TLbsRequestAction aAction);
	
protected:
	TLbsRequestAction iAction;
	TLbsRequestAdvice iAdvice;
	TUint8  iReserved[16];
	};


#endif // LBSPRVREQUESTCOMMON_H_
