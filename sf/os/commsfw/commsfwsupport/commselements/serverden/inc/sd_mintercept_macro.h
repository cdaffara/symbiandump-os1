// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file 
 @internalTechnology
*/

#ifndef SYMBIAN_ELEMENTS_MINTERCEPT_MACRO_H
#define SYMBIAN_ELEMENTS_MINTERCEPT_MACRO_H

#include <e32base.h>
#include <elements/nm_signals.h>

// Based upon build options enable/disable support for the message intercept
#ifndef __MESSAGE_INTERCEPT_NO_UDEB
	#ifdef _DEBUG
		// Message interceptor was moved from esock to elements.
		// However previous Interceptor headers were not tagged. Which means that the headers could
		// be introduced in a publishedAll context. But this is unlikely because the purpose of
		// message intercept is debug.
		// In order to minimise any impact the change may have we define here a new macro for
		// elements/messageintercept, so that changes could be reverted if necessary, but this
		// version can be kept. Main motivation is to make Message Interceptor reusable.
		#ifndef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
			#define __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
		#endif	// !__ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	#endif // _DEBUG
#endif //__MESSAGE_INTERCEPT_NO_UDEB

#ifdef __MESSAGE_INTERCEPT_UREL
	#ifndef _DEBUG
		#ifndef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
			#define __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
		#endif //__ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	#endif //_DEBUG
#endif //__MESSAGE_INTERCEPT_UREL


#if defined (__ELEMENTS_MESSAGE_INTERCEPT_ACTIVE)
	// Make sure users of the message intercept register are warned that its enabled in a release build
	#if (!defined __MESSAGE_INTERCEPT_SUPPRESS_REL_WARNING) && (!defined _DEBUG)
	#pragma message ("WARNING: Use of esock debug message intercept register in a release build!")
	#endif
#endif	// __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE


// Macros
//-------
#if defined (__ELEMENTS_MESSAGE_INTERCEPT_ACTIVE)
#define ELEMENTS_DEBUG_MESSAGE_INTERCEPT(SENDER, MESSAGE, RECEIVER) \
	{ \
	NetInterfaces::TInterfaceControl& mi = Den::CMessageInterceptRegister::GetInterfaceControl(); \
	Den::ANodeIntercept* intercepter = static_cast<Den::ANodeIntercept*>(mi.FetchInterfaceL(Den::ANodeIntercept::KInterfaceId)); \
	Den::TEventSummaryTriple nodeEvent(address_cast<TNodeId>(SENDER), MESSAGE, RECEIVER); \
	intercepter->QueryMessageInterceptL(nodeEvent); \
	}

#define ELEMENTS_DEBUG_REGISTER_GENERAL_NODE(UID, NODE) \
	{ \
	NetInterfaces::TInterfaceControl& mi = Den::CMessageInterceptRegister::GetInterfaceControl(); \
	Den::ANodeIntercept* intercepter = static_cast<Den::ANodeIntercept*>(mi.FetchInterfaceL(Den::ANodeIntercept::KInterfaceId)); \
	Den::TNodeSpecifier nodeToRegister(TUid::Uid(UID), NODE->Id()); \
	intercepter->RegisterNewNode(nodeToRegister); \
	}

#else

// Define-out our macros
#define ELEMENTS_DEBUG_MESSAGE_INTERCEPT(SENDER, MESSAGE, RECEIVER)
#define ELEMENTS_DEBUG_REGISTER_GENERAL_NODE(UID, COMMSID)

#endif	// __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE

#endif	//SYMBIAN_ELEMENTS_MINTERCEPT_MACRO_H


