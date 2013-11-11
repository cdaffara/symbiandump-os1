// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/


#ifndef EPOCERROR_H
#define EPOCERROR_H

#include <e32base.h>

class EpocError : CBase
/**
@internalComponent
*/
	{
	public:
		EpocError(){}
		TBool Evaluate(TInt err);
		TPtrC GetMessage() { return LastErrorMessage; }
		TInt GetCode() { return iLastError; }
		
	private:
		/** ret value of last call */
		TInt iLastError;								
		/** associated message if any */
		TPtrC LastErrorMessage;							
	};

#endif

