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
// Operation information 
// Used by both Remote Control server side and Remote Control client side library.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef OPERATIONINFORMATION_H
#define OPERATIONINFORMATION_H

#include <e32base.h>
#include <remcon/messagetype.h>


class TOperationInformation
	{
public:
	TUint iOperationId;
	TRemConMessageSubType iMessageSubType;
	};


#endif // OPERATIONINFORMATION_H