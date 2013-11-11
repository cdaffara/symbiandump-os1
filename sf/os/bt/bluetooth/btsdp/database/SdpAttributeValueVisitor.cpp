// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <btsdp.h>

/**
This virtual function allows the M- classes to be extended in future in a binary
compatible way by providing a method that clients can override in future to
allow extra callbacks to be made via aObject.
*/	
EXPORT_C void MSdpAttributeValueVisitor::MSAVV_ExtensionInterfaceL(TUid /*aInterface*/, void*& aObject)
	{
	aObject = NULL;
	}
	
// EOF
