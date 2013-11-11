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


#ifndef DATA_DEALER_H
#define DATA_DEALER_H

#include <e32property.h>
#include <e32base.h>


class CDataDealer : public CBase
	{
public:
	static CDataDealer* NewL( const TBool aProducer, const TUint aKey );
	virtual ~CDataDealer();

protected:
	void ConstructL();
	
private:
	CDataDealer( const TBool aProducer, const TUint aKey );
	
public:
	void ProduceDataL( const TDesC8& aData );
	void ConsumeDataL( HBufC8*& aData );
	void ConsumeDataL( HBufC8*& aData, const TInt aMaxSize, TBool& aMore );

private:
	void WaitForConsumerL();
	void WaitForPropertyToBeCreatedL();

	/** The property to produce to or consume from */
	RProperty iProperty;

	TInt iPropertySize;
	TBool iProducer;
	TUint iKey;

	TUid iDtorID;
	};


#endif // DATA_DEALER_H
	
