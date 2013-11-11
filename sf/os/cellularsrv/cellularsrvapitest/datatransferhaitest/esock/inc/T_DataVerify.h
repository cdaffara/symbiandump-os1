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


#ifndef DATAVERIFY_H
#define DATAVERIFY_H

#include <e32base.h>
#include <f32file.h>
#include <e32cmn.h>
#include "DataWrapperBase.h"

class CMD5;

/**
 * Implementation of DataVerify interface. 
 */
class CT_DataVerify : public CDataWrapperBase
	{
public:

	enum TDataVerifyMethod
		{
		EDataVerifyMethodUnknown = 0,
		EDataVerifyMethodMD5,
		};

private:
	CT_DataVerify();

protected:
	void ConstructL();


public:
	
	/**
	 * 2 phase constructor.
	 */
	static CT_DataVerify* NewL();	
	virtual TAny*	GetObject();	
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	virtual ~CT_DataVerify();
	virtual void DoCmdVerifyData( const TTEFSectionName& aSection );
	virtual void VerifyData( const TDataVerifyMethod aMethod, const TFileName& aFileName, const TDesC& aChecksum );
	virtual void VerifyChecksumL( const TDesC8& aReferenceMD5Checksum, const TFileName& aDataFileName );
	virtual void VerifyChecksumL( const TDesC8& aReferenceMD5Checksum, const TDesC8& aData );
	virtual void VerifyFileL( const TFileName& aReferenceFileName, const TFileName& aDataFileName );
	
private:
	void VerifyBufferL( const TFileName& aReferenceFileName,const TDesC8& aData );
	void VerifyBufferL( const TDesC8& aReferenceData, const TDesC8& aData );
	TBool CompareBuffers( const TDesC8& aReferenceBuffer, const TDesC8& aDataBuffer );
	void ReadFileL(const TFileName& aFileName, TDes8& aBuf);
	void HashFileL( const TFileName& aFileName, TDes8& aDes );
	void WriteFileL( const TFileName& aFileName, const TDesC8& aData );
	void ConvertString2HexL( const TDesC8& aData, TDes8& aDes );

private:
	/** A handle to a file server session. */
	RFs iFs;
	/** An MD5 message digest.*/
	CMD5* iMD5;
	};

#endif	// DATAVERIFY_H
