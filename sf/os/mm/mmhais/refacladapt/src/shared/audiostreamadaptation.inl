/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/



// Interface's (abstract base class's) destructor
inline CAudioStreamAdaptation::~CAudioStreamAdaptation()
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

// Interface's (abstract base class's) static factory method implementation.
// Asks ECOM plugin framework to instantiate appropriate concret plugin
// implementation.

inline CAudioStreamAdaptation* CAudioStreamAdaptation::NewL(TUid aType)
	{
	TAny* ptr = REComSession::CreateImplementationL( aType, _FOFF(CAudioStreamAdaptation,iDtor_ID_Key) ) ;

	// The type of TAny* ptr should be CAudioStreamAdaptation
	return REINTERPRET_CAST(CAudioStreamAdaptation*, ptr);
	}

