// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalAll 
*/

#ifndef MISDISPATCHINTERFACESUPPORTED_H_
#define MISDISPATCHINTERFACESUPPORTED_H_

#include <ctsy/ltsy/mltsydispatchfactory.h>

class MIsDispatchInterfaceSupported
/**
 * Interface implemented by the functional unit handlers to return whether a 
 * specific interface is supported. 
 */
	{
public:
	
	virtual TBool IsInterfaceSupported(TLtsyDispatchInterfaceApiId aDispatchApiId) = 0;
	
	}; // class MIsDispatchInterfaceSupported

#endif // MISDISPATCHINTERFACESUPPORTED_H_
