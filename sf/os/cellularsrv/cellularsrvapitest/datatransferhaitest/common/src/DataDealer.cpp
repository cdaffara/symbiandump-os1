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


#include <e32base.h>
#include <ecom/ecomresolverparams.h>
#include <ecom/ecom.h>

#include "DataDealer.h"
#include "DataDealerParams.h"

// We cannot use UIDs that are reserved from Symbian (neither protected
// nor unprotected) since Publish&Subscribe properties require that
// the running process they are defined in has the secure id that matches
// the id of the P&S property (secure id check is not made for legacy UIDs
// which are < 0x10273357).
// So we use a UID from development range: 0x00000000 - 0x0FFFFFFF
const TUid KPropertyCategory = { 0x077A5355 };
//const TUid KPropertyCategory = { 0xA00023E0 };

// Time to wait before re-checking property values
const TInt KWaitForPropertyTime( 100000 );


const TUid KDataDealerImplUID1 = {0xA00023E2};

/**
 * @param aProducer Set to ETrue if this is a producer instance.
 * @param aKey The Publish&Subscribe key that is used for data passing.
 */
CDataDealer* CDataDealer::NewL( const TBool aProducer, const TUint aKey )
	{
	TDataDealerParams params( aProducer, aKey );
	
	TAny* implementation =
		REComSession::CreateImplementationL( KDataDealerImplUID1,
		                                    _FOFF( CDataDealer, iDtorID ),
											&params );
	

	CDataDealer* self = new (ELeave) CDataDealer( params.iProducer, params.iKey );
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	
	return self;
	}

CDataDealer::~CDataDealer()
	{
	// wait that the property is empty (= data consumed)
	TRAPD( err, WaitForConsumerL() );
	if( err != KErrNone )
		{
		// WaitForConsumerL error!
		}

	// The producer is responsible of creating and destroying the property
	if( iProducer )
		{
		// CDataDealer: This is a producer instance, delete the P&S property
		TInt err = iProperty.Delete( KPropertyCategory, iKey );
		if( err != KErrNone )
			{
			// CDataDealer: Could not delete property
			}
		iProperty.Close();
		}
	
	
	// Notify the ECom server that we released one implementation of some
	// DLL. When the counter hits zero ECom server may unload the DLL.
	REComSession::DestroyedImplementation( iDtorID );
	}

void CDataDealer::ConstructL()
	{
	// The producer is responsible of creating and destroying the property
	if( iProducer )
		{
		// CDataDealer: This is a producer instance, create the P&S property
		TInt err = iProperty.Define( KPropertyCategory,
											  iKey,
											  RProperty::EByteArray,
											  iPropertySize );
		if( err == KErrAlreadyExists )
			{
			// Property already exists. Clear contents.
			User::LeaveIfError( iProperty.Set( KPropertyCategory, iKey, KNullDesC8 ) );
			}
		else if( err != KErrNone )
			{
			// Could not create pub&sub property
			User::Leave( err );
			}
		}
	// Consumer waits that the property is created by producer
	else
		{
		WaitForPropertyToBeCreatedL();
		}
	}


/**
 * Consumer uses this to wait for the property to be created by producer
 * when DataDealer is constructed.
 */
void CDataDealer::WaitForPropertyToBeCreatedL()
	{
	const TInt KTempPropertyValueLength(2);
	TBuf8<KTempPropertyValueLength> tempValue;

	TInt err = iProperty.Get( KPropertyCategory, iKey, tempValue );
	while( err == KErrNotFound )
		{
		// Property did not created yet, wait
		const TInt KWaitForProperty( 500000 );
		// User::After here is not a critical error. It is used here to wait for the RProperty instance
		// to be created. The best solution would be to use RProperty notifications to do that but it is
		// currently too heavy considering how DataDealer is used: in testing context without any UI
		User::After( KWaitForProperty );
		err = iProperty.Get( KPropertyCategory, iKey, tempValue );
		}
	if( err != KErrNone && err != KErrOverflow )
		{
		// Unexpected error!
		User::Leave( err );
		}
	}

CDataDealer::CDataDealer( const TBool aProducer, const TUint aKey )
: 	CBase(),	
	// Use KMaxPropertySize as default size to ensure "real time" setting of a value
	iPropertySize( RProperty::KMaxPropertySize ),
	iProducer( aProducer ),
	iKey( aKey ),
	iDtorID()
	{
	}

/**
 * Produce data to the data storage defined in the constructor.
 */
void CDataDealer::ProduceDataL( const TDesC8& aData )
	{
	// "CDataDealer::ProduceDataL: %S", &aData

	// Check for misuse
	if( !iProducer ) User::Leave( KErrNotSupported );

	for( TInt i(0); i < aData.Length(); i += iPropertySize )
		{
		// wait that the property is empty (= data consumed)
		WaitForConsumerL();

		// Then write data block-by-block
		TPtrC8 dataBlock( KNullDesC8 );
		if( ( i + iPropertySize ) < aData.Length() )
			dataBlock.Set( aData.Mid( i, iPropertySize ) );
		else
			dataBlock.Set( aData.Mid( i, aData.Length() - i ) );
		// Write single block
		User::LeaveIfError( iProperty.Set( KPropertyCategory, iKey, dataBlock ) );
		}
	}

/**
 * Wait synchronously until the consumer has deleted the data we have produced.
 * Used by data dealer that is producing data.
 */
void CDataDealer::WaitForConsumerL()
	{
	const TInt KTempPropertyValueLength(2);
	TBuf8<KTempPropertyValueLength> tempValue;

	TInt err = iProperty.Get( KPropertyCategory, iKey, tempValue );

	if( err != KErrOverflow && err != KErrNone )
		{
		// un-expected error
		// CDataDealer: Could not get property value.
		User::Leave( err );
		}

	while( err == KErrOverflow || tempValue != KNullDesC8 )
		{
		// Not empty yet, wait

		// User::After here is not a critical error. It is used here to wait for the RProperty instance
		// to be created. The best solution would be to use RProperty notifications to do that but it is
		// currently too heavy considering how DataDealer is used: in testing context without any UI
		User::After( KWaitForPropertyTime ); // CSI: 92 #
		err = iProperty.Get( KPropertyCategory, iKey, tempValue );
		}

	}

/**
 * @param aData On return contains a pointer to heap descriptor containing the received data.
 */
void CDataDealer::ConsumeDataL( HBufC8*& aData )
	{
	// Check for misuse
	if( iProducer ) User::Leave( KErrNotSupported );

	TInt err( KErrNone );
	HBufC8* data = HBufC8::NewLC( iPropertySize );
	TPtr8 dataPtr = data->Des();
	const TInt KMinHBufCLength(1);
	HBufC8* wholeData = HBufC8::NewLC( KMinHBufCLength );

	// Loop until the property is destroyed by the producer
	for( TInt i(0); err != KErrNotFound; )
		{
		// check if the property exists and has data
		err = iProperty.Get( KPropertyCategory, iKey, dataPtr );
		TBool empty = ( dataPtr == KNullDesC8 );
		if( err == KErrNone )
			{
			if( empty )
				{
				// Property empty, cannot consume, wait
				// If the property exists but is empty, wait for data
				// User::After here is not a critical error. It is used here to wait for the RProperty instance
				// to be created. The best solution would be to use RProperty notifications to do that but it is
				// currently too heavy considering how DataDealer is used: in testing context without any UI
				User::After( KWaitForPropertyTime ); // CSI: 92 #
				}
			else
				{
				// Clear the property...
				User::LeaveIfError( iProperty.Set( KPropertyCategory, iKey, KNullDesC8 ) );

				// Append the retrieved data to the buffer that will be returned
				// CDataDealer: Data ready, consumed
				HBufC8* newWholeData = wholeData->ReAllocL( wholeData->Length() + iPropertySize );
				CleanupStack::Pop( wholeData );
				wholeData = newWholeData;
				CleanupStack::PushL( wholeData );
				wholeData->Des().Append( *data );
				// Clear the buffer
				dataPtr.Zero();

				// ..and set new storage index
				i += iPropertySize;
				}
			}
		// If the property is not found, it means that there is (no more) data to be consumed available
		else if( err != KErrNotFound )
			{
			// CDataDealer: Could not open the property.
			User::Leave( err );
			}
		}

	// "CDataDealer::ConsumeDataL: %S", wholeData
	// Ownership transferred
	CleanupStack::Pop( wholeData );
	aData = wholeData;
	CleanupStack::PopAndDestroy( data );
	}

/**
 * Consume data that is produced to the data storage defined in the constructor.
 * @param aData On return contains a pointer to heap descriptor containing the received data.
 * @param aMaxSize The maximum size of the data block.
 * @param aMore ETrue if there is more data to be consumed.
 */
void CDataDealer::ConsumeDataL( HBufC8*& aData, const TInt aMaxSize, TBool& aMore )
	{
	// Check for misuse
	if( iProducer ) User::Leave( KErrNotSupported );

	HBufC8* data = HBufC8::NewLC( iPropertySize );
	TPtr8 dataPtr = data->Des();

	// Loop until data is produced to the property
	TInt err( KErrNone );
	while( err != KErrNotFound && dataPtr.Length() == 0 )
		{
		// check if the property exists and has data
		err = iProperty.Get( KPropertyCategory, iKey, dataPtr );
		TBool empty = ( dataPtr == KNullDesC8 );

		if( err == KErrNone )
			{
			if( empty )
				{
				// If the property exists but is empty, wait for data
				// User::After here is not a critical error. It is used here to wait for the RProperty instance
				// to be created. The best solution would be to use RProperty notifications to do that but it is
				// currently too heavy considering how DataDealer is used: in testing context without any UI
				User::After( KWaitForPropertyTime ); // CSI: 92 #
				}
			else if( dataPtr.Length() > aMaxSize )
				{
				// Too long data (%i, max: %i): consuming only the first block", dataPtr.Length(), aMaxSize
				// Consuming only the first aMaxSize characters; save the rest of the data to the property
				iProperty.Set( KPropertyCategory, iKey, dataPtr.Right( dataPtr.Length() - aMaxSize ) );
				// Take the first aMaxSize characters and ignore the rest
				data->Des().Copy( dataPtr.Left( aMaxSize ) );
				}
			else
				{
				// Clear the property...
				User::LeaveIfError( iProperty.Set( KPropertyCategory, iKey, KNullDesC8 ) );
				}
			}
		// If the property is not found, it means that there is (no more) data to be consumed available
		else if( err == KErrNotFound )
			{
			// CDataDealer: No more data in the property (KErrNotFound)
			aMore = EFalse;
			}
		else
			{
			// CDataDealer: Could not open the property.
			aMore = EFalse;
			User::Leave( err );
			}
		}

	CleanupStack::Pop( data );
	aData = data;
	}

