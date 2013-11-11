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
#include <f32file.h>

#include <hash.h> // CryptoHW (md5hash)

#include "T_DataVerify.h"



/*@{*/
//Command LIT's
_LIT(KCmdVerifyData,	"VerifyData");
/*@}*/

/*@{*/
//LIT's for reading params from ini file
_LIT( KDataVerifyType,  "VerifyType" );
_LIT( KDataVerifyFile,  "VerifyFile" );
_LIT( KChecksum,     	"Checksum" );
_LIT( KMD5Type,			"MD5" );        // Use MD5 with data verify
/*@}*/


namespace DataVerify
	{
	const TInt KMD5Size = 32;
    const TInt KMD5length  = 32;
    const TInt KBufsize    = 512;
    const TInt KCharlength = 2;
	}

typedef TBuf8<DataVerify::KMD5length> THashBuf;

CT_DataVerify::CT_DataVerify():
	iMD5(NULL)
	{
	}

CT_DataVerify* CT_DataVerify::NewL()
	{
	CT_DataVerify* ret = new (ELeave) CT_DataVerify();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_DataVerify::~CT_DataVerify()
	{
    iFs.Close();
    if(iMD5)
    	{
    	delete iMD5;
    	iMD5 = NULL;
    	}
	}

void CT_DataVerify::ConstructL()
	{
    User::LeaveIfError( iFs.Connect() );
    iMD5 = CMD5::NewL();
	}

TAny* CT_DataVerify::GetObject()
	{
	return NULL;
	}

TBool CT_DataVerify::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool retVal=ETrue;
	
	if (aCommand==KCmdVerifyData)
		{	
		DoCmdVerifyData(aSection);	
		}
	else
		{
		ERR_PRINTF1(_L("Unknown command"));
		retVal=EFalse;
		}

	return retVal;
	}


/**
 * Verifies that the downloaded file is not corrupted by using a previously calculated checksum.
 * @param aSection - The section in config file to look for the Data Verify Type, the file to verify and the checksum.
 * @return error - Error code. KErrNone if checksum is equal.
 */
void CT_DataVerify::DoCmdVerifyData(const TTEFSectionName& aSection)
    {
    INFO_PRINTF1(_L("*START* CT_TransferData::DoCmdVerifyData"));
    
    TBool dataOk =ETrue;
    TPtrC type;
    if(!GetStringFromConfig(aSection, KDataVerifyType, type ))
    	{
    	ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDataVerifyType);
    	SetBlockResult(EFail);
    	dataOk = EFalse;
    	}
    
    TPtrC filename;
    if(!GetStringFromConfig(aSection, KDataVerifyFile, filename ))
    	{
    	ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDataVerifyFile);
    	SetBlockResult(EFail);
    	dataOk = EFalse;
    	}
    
    TPtrC checksumParameter;
    if(!GetStringFromConfig(aSection, KChecksum, checksumParameter ))
    	{
    	ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KChecksum);
    	SetBlockResult(EFail);
    	dataOk = EFalse;
    	}
    if(dataOk)
    	{
    	TBuf8<DataVerify::KMD5Size> checksum;
    	checksum.Append(checksumParameter);

        if (type == KMD5Type)
            {
            INFO_PRINTF1(_L("MD5 selected"));
            TRAPD(error, VerifyChecksumL(checksum, filename));
            if (error == KErrNone)
                {
                INFO_PRINTF1(_L("Data verify succeeded"));
                }
            else
            	{
                ERR_PRINTF2(_L("VerifyData failed [%d]"), error);
                SetError(error);
            	}
            }
    	}

    
    INFO_PRINTF1(_L("*END* CT_TransferData::DoCmdVerifyData"));
    }


/**
 * Verify Data integrity
 *
 * @param aMethod			The verification method
 * @param aFileName			File to verify
 * @param aChecksum			Checksum
 *
 * @return					N/A
 *
 * @leave					System wide error
 */
void CT_DataVerify::VerifyData( const TDataVerifyMethod aMethod, const TFileName& aFileName, const TDesC& aChecksum )
	{
	TBool dataOk = ETrue;
	
	if( aFileName.Length() > KMaxFileName)
		{
		ERR_PRINTF2(_L("Illegal filename: %S"), &aFileName);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if(dataOk)
		{
		THashBuf checksum;
		checksum.Append( aChecksum );
		
		// Use MD5 to verify file
		if( aMethod == EDataVerifyMethodMD5 )
			{
			// MD5 selected
			TRAPD(error, VerifyChecksumL( checksum, aFileName ));
			if(error == KErrNone)
				{
				INFO_PRINTF1(_L("Data verify succeeded."));
				}
			else
				{
				ERR_PRINTF2(_L("Checksum verification left with error %d"), error);
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF1(_L("Verification method not supported"));
			SetBlockResult(EFail);
			}
		}
	}

/**
 * Verify File integrity
 *
 * @param aReferenceFileName	Reference file against to verify integrity
 * @param aDataFileName			File to verify
 *
 * @return						N/A
 *
 * @leave						System wide error
 */
void CT_DataVerify::VerifyFileL( const TFileName& aReferenceFileName, const TFileName& aDataFileName )
	{
	THashBuf refHash;

	// hash the first file
	HashFileL( aReferenceFileName, refHash );

	THashBuf dataHash;
	// hash the second file
	HashFileL( aDataFileName, dataHash );

	// compare hashes together
	if( !CompareBuffers( refHash, dataHash ) )
		{
		// Data was corrupted
		User::Leave( KErrCorrupt );
		}
	}

/**
 * Verify Buffer integrity
 *
 * @param aReferenceFileName	Reference file against to verify integrity
 * @param aData					Buffer to verify
 *
 * @return						N/A
 *
 * @leave						System wide error
 */
void CT_DataVerify::VerifyBufferL( const TFileName& aReferenceFileName, const TDesC8& aData )
    {
	HBufC8* refdata = 0;
	TPtr8 ptr = refdata->Des();
	// read file into buffer, file must fit into HBufC8
	ReadFileL( aReferenceFileName, ptr );

	// compare read file with given buffer
	if( !CompareBuffers( refdata->Des(), aData ) )
		{
		// Data was corrupted
		User::Leave( KErrCorrupt );
		}
    }

/**
 * Verify Buffer integrity
 *
 * @param aReferenceBuffer		Reference buffer against to verify integrity
 * @param aData					Buffer to verify
 *
 * @return						N/A
 *
 * @leave						System wide error
 */
void CT_DataVerify::VerifyBufferL( const TDesC8& aReferenceData, const TDesC8& aData )
	{
	// Compare buffers
	if ( !CompareBuffers( aReferenceData, aData ) )
		{
		// Data was corrupted
		User::Leave( KErrCorrupt );
		}
    }

/**
 * Verify Checksum integrity
 *
 * @param aReferenceMD5Checksum		Reference checksum
 * @param aDataFileName				File to verify
 *
 * @return							N/A
 *
 * @leave							System wide error
 */
void CT_DataVerify::VerifyChecksumL( const TDesC8& aReferenceMD5Checksum, const TFileName& aDataFileName )
	{
	// MD5 checksum is always 128 bit (32 char) long
	if( aReferenceMD5Checksum.Length() != DataVerify::KMD5length )
		{
		User::Leave( KErrBadDescriptor );
		}

	THashBuf hash;
	// hash file
	HashFileL( aDataFileName, hash );

	// converting MD5 checksum to binary is necessary because Hash() function
	// returns hash in binary format and reference checksum given in parameter
	// is a 32 character long string
	THashBuf conversion;
	ConvertString2HexL( aReferenceMD5Checksum, conversion );

	if( !CompareBuffers( conversion, hash ) )
		{
		// Data was corrupted
		User::Leave( KErrCorrupt );
		}
	}

/**
 * Verify Checksum integrity
 *
 * @param aReferenceMD5Checksum		Reference checksum
 * @param aData						Data to verify
 *
 * @return							N/A
 *
 * @leave							System wide error
 */
void CT_DataVerify::VerifyChecksumL( const TDesC8& aReferenceMD5Checksum, const TDesC8& aData )
    {
	// MD5 checksum is always 128 bit (32 char) long
	if( aReferenceMD5Checksum.Length() != DataVerify::KMD5length )
		{
		// MD5 Checksum was not correct length (32 characters)
		User::Leave( KErrBadDescriptor );
		}

	// hash the given buffer
	TPtrC8 hash = iMD5->Hash(aData);

	// converting MD5 checksum to binary is necessary because Hash() function
	// returns hash in binary format and reference checksum given in parameter
	// is a 32 character long string
	THashBuf conversion;
	ConvertString2HexL( aReferenceMD5Checksum, conversion );

	// Compare hashs together
	if( !CompareBuffers( conversion, hash ) )
		{
		// Data was corrupted
		User::Leave( KErrCorrupt );
		}
	}

/**
 * Compare Buffers
 *
 * @param aReferenceBuffer		Reference buffer
 * @param aDataBuffer			Buffer to compare
 *
 * @return						ETrue if buffer contents match
 */
TBool CT_DataVerify::CompareBuffers( const TDesC8& aReferenceBuffer, const TDesC8& aDataBuffer )
	{
	TBool res = EFalse;
	TInt result = aReferenceBuffer.Compare( aDataBuffer );
	
	if( result == 0 )
		{
		// Buffer contents match
		res = ETrue;
		}
	return res;
	}

/**
 * Read File
 *
 * @param aFileName			File name to read
 * @param aBuf				Buffer to read file content
 *
 * @return					N/A
 *
 * @leave					System wide error
 */
void CT_DataVerify::ReadFileL(const TFileName& aFileName, TDes8& aBuf)
    {
    RFile file;
    User::LeaveIfError( file.Open( iFs, aFileName, EFileRead ) );
    CleanupClosePushL( file );

    // Get the file size
    TInt size;
    User::LeaveIfError( file.Size( size ) );

    // allocate memory from heap for file content
    HBufC8* buf = HBufC8::NewLC( size );
    aBuf = buf->Des();

    // read file content into buffer, file may be too big for HBufC8
    User::LeaveIfError( file.Read( aBuf ) );

    CleanupStack::Pop( buf );
    CleanupStack::PopAndDestroy( &file ); // file.Close
    }

/**
 * Hash file in pieces
 *
 * @param aFileName			File name to be hashed
 * @param aDes				Hash value
 *
 * @return					N/A
 *
 * @leave					System wide error
 */
void CT_DataVerify::HashFileL( const TFileName& aFileName, TDes8& aDes )
	{
	RFile file;
	User::LeaveIfError( file.Open( iFs, aFileName, EFileRead ) );
	CleanupClosePushL( file );

	TBuf8<DataVerify::KBufsize> buf;

	TInt error = KErrNone;
	iMD5->Reset();
	// File can be hashed in pieces
	while( error == KErrNone ) // loop until something goes wrong
		{
		error = file.Read( buf, DataVerify::KBufsize );
		if( buf.Length() != 0 )
			{
			// MD5 checksum is calculated in pieces
			iMD5->Update(buf);
			}
		else
			{
			// file ended, end loop
			break;
			}
		}
	// Return calculated MD5 checksum
	aDes = iMD5->Final();

	CleanupStack::PopAndDestroy(&file); // file.Close
	}

/**
 * Write File
 *
 * @param aFileName			File name
 * @param aData				Data to be written into file name.
 *
 * @return					N/A
 *
 * @leave					System wide error
 */
void CT_DataVerify::WriteFileL( const TFileName& aFileName, const TDesC8& aData )
    {
    RFile file;
    // Create file, replace if exists
    User::LeaveIfError( file.Replace( iFs, aFileName, EFileWrite|EFileStream ) );
    CleanupClosePushL( file );
    // Write data into file
    User::LeaveIfError( file.Write( aData ) );
    // Commit write
    User::LeaveIfError( file.Flush() );

    CleanupStack::PopAndDestroy( &file ); // file.Close
    }

/**
 * Convert a string to Hexadecimal
 *
 * @param aData			String descriptor
 * @param aDes			Descriptor where Hex value is stored
 *
 * @return				N/A
 *
 * @leave				System wide error
 */
void CT_DataVerify::ConvertString2HexL( const TDesC8& aData, TDes8& aDes )
    {
	TBuf8<DataVerify::KCharlength> charBuf;

	// Check that buffer is even number
	if( ( aData.Length() % DataVerify::KCharlength ) != 0 )
		{
		User::Leave( KErrBadDescriptor );
		}

	// Go through the data and convert it two characters at a time
	// buffer overflow does not occur because buffer is checked to be even number first
	for( TInt i = 0, limit = aData.Length()-DataVerify::KCharlength; i <= limit; i+=DataVerify::KCharlength )
		{
		// Clean char buffer first
		charBuf.Delete( 0, charBuf.Length() );
		
		// Add KCharlength characters into buffer
		for ( TInt j = 0; j < DataVerify::KCharlength; j++ )
			{
			charBuf.Append( aData[i+j] );
			}

		TUint number;
		TLex8 converter = TLex8( charBuf );
		// Two characters together represent a hex number in MD5 checksum
		User::LeaveIfError( converter.Val( number, EHex ) );

		aDes.Append( number );
		}
	}

