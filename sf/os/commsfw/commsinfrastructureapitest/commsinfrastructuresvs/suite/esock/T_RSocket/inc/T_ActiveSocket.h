/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef __T_ACTIVE_SOCKET__
#define __T_ACTIVE_SOCKET__

#include "DataWrapperBase.h"

//	EPOC includes
#include <es_sock.h>

const	TInt	KMaxBufferLength	=2048;
const	TInt	KMaxIPAddressLength	=50;

class CActiveSocket : public CActiveCallback
	{
public:
	virtual void	ValidateCompletionL(CDataWrapperBase& aTestStep);

	virtual ~CActiveSocket();
	
protected:
	CActiveSocket( const TDesC& aName, RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority );
	
	void	DestroyData();
	void	CreateDataL(const TDesC* aExpectedDataIn, TInt aBufferSize);
	void	CreateDataL(HBufC8* aExpectedDataIn, TInt aBufferSize);
	
private:

protected:
	RSocket&		iSocket;
	TPtr8*			iDataIn;

private:
	const TDesC&	iName;
	HBufC8*			iDataReturned;
	HBufC8*			iExpectedDataIn;
	};

#endif /* __T_ACTIVE_SOCKET__ */
