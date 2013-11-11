// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the header file of the CSmsBuffers.
// 
//

/**
 @file
*/

#ifndef __GSMUBUF_H__
#define __GSMUBUF_H__

#include <e32std.h>
#include <s32strm.h>

class CEditableText;
class RReadStream;
class RWriteStream;


/**
 *  The base class for all SMS buffers.
 *  @publishedAll
 *  @released
 */
class CSmsBufferBase : public CBase
	{
public:
	enum
		{
		EMaxBufLength=0x100
		};
public:
	/**
	 *  Gets the number of characters in the buffer.
	 *  
	 *  	@return The number of characters in the buffer. 
	 */
	virtual TInt Length() const=0;
	/**
	 *  Extracts buffer data to a descriptor.
	 *  
	 *  @param aBuf On return, buffer data
	 *  @param aPos Position within buffer to begin reading
	 *  	@param aLength The number of bytes to read from the buffer 
	 */
	virtual void Extract(TDes& aBuf,TInt aPos,TInt aLength) const=0;
	/**
	 *  Inserts data into the buffer.
	 *  
	 *  @param aPos Position in the buffer to insert the data
	 *  	@param aBuf The data to insert into the buffer 
	 */
	virtual void InsertL(TInt aPos,const TDesC& aBuf)=0;
	/**
	 *  Deletes data from the buffer.
	 *  
	 *  @param aPos Position in the buffer to delete the data
	 *  	@param aLength The number of bytes to delete from the buffer 
	 */
	virtual void DeleteL(TInt aPos,TInt aLength)=0;
	/** Resets the buffer. */
	virtual void Reset()=0;
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	};


/**
 *  This is the simplest implementation of CSmsBufferBase.
 *  
 *  It stores the buffer in an array of TTexts.
 *  @publishedAll
 *  @released
 */
class CSmsBuffer : public CSmsBufferBase
	{
public:
	IMPORT_C static CSmsBuffer* NewL();
	IMPORT_C ~CSmsBuffer();
	IMPORT_C TInt Length() const;
	IMPORT_C void Extract(TDes& aBuf,TInt aPos,TInt aLength) const;
	IMPORT_C void InsertL(TInt aPos,const TDesC& aBuf);
	IMPORT_C void DeleteL(TInt aPos,TInt aLength);
	IMPORT_C void Reset();
private:
	CSmsBuffer();
private:

	CArrayFix<TText>* iBuffer;
	};


/**
 *  SMS buffer, implemented as a thin wrapper over CEditableText.
 *  
 *  This class is designed to be used by the Message Server, which stores SMS
 *  text as CRichText, which is derived from CEditableText.
 *  @publishedAll
 *  @released
 */
class CSmsEditorBuffer : public CSmsBufferBase
	{
public:
	IMPORT_C static CSmsEditorBuffer* NewL(CEditableText& aText);
	IMPORT_C ~CSmsEditorBuffer();
	IMPORT_C TInt Length() const;
	IMPORT_C void Extract(TDes& aBuf,TInt aPos,TInt aLength) const;
	IMPORT_C void InsertL(TInt aPos,const TDesC& aBuf);
	IMPORT_C void DeleteL(TInt aPos,TInt aLength);
	IMPORT_C void Reset();
private:
	CSmsEditorBuffer(CEditableText& aText);
private:
	CEditableText& iText;
	};

#endif // !defined __GSMUBUF_H__
