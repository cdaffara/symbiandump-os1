// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedAll
 @released
*/

#ifndef REMCONCOREAPICONTROLLEROBSERVER_H
#define REMCONCOREAPICONTROLLEROBSERVER_H

#include <e32base.h>
#include <remconcoreapi.h>

/**
Clients must implement this interface in order to instantiate objects of type 
CRemConCoreApiController. This interface passes incoming responses from RemCon 
to the client. 
In order to minimise the number of virtual functions the client has to 
implement, there's just one non-pure function which delivers all responses.
*/
class MRemConCoreApiControllerObserver
	{
public:
	/** 
	A response has been received. 
	@param aOperationId The operation ID. The response is to a previous 
	command of this type.
	@param The response error.
	*/
	IMPORT_C virtual void MrccacoResponse(TRemConCoreApiOperationId aOperationId, TInt aError);
	};

#endif // REMCONCOREAPICONTROLLEROBSERVER_H
