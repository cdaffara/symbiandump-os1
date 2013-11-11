// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the implementation of the SMS buffer
// 
//

/**
 @file
*/

#include "gsmubuf.h"
#include "Gsmumain.h"

#include <txtetext.h>


/**
 *  Internalises the object.
 *  
 *  @param aStream Stream to read from
 *  @capability None
 */
EXPORT_C void CSmsBufferBase::InternalizeL(RReadStream& aStream)
	{
	Reset();
	TInt length=aStream.ReadInt32L();
	TBuf<EMaxBufLength> buf;
	for (TInt pos=0; pos<length; pos+=EMaxBufLength)
		{
		TInt buflength=(pos+EMaxBufLength)<=length? EMaxBufLength: length-pos;
		aStream.ReadL(buf,buflength);
		InsertL(pos,buf);
		}
	} // CSmsBufferBase::InternalizeL


/**
 *  Externalises the object.
 *  
 *  @param aStream Stream to write to
 *  @capability None
 */
EXPORT_C void CSmsBufferBase::ExternalizeL(RWriteStream& aStream) const
	{
	TInt length=Length();
	aStream.WriteInt32L(length);
	for (TInt pos=0; pos<length; pos+=EMaxBufLength)
		{
		TBuf<EMaxBufLength> buf;
		TInt buflength=(pos+EMaxBufLength)<=length? EMaxBufLength: length-pos;
		Extract(buf,pos,buflength);
		aStream.WriteL(buf,buflength);
		}
	} // CSmsBufferBase::ExternalizeL


/**
 *  Allocates and constructs the buffer.
 *  
 *  @return New CSmsBuffer object
 *  @capability None
 */
EXPORT_C CSmsBuffer* CSmsBuffer::NewL()
	{
	CSmsBuffer* smsbuffer=new(ELeave) CSmsBuffer();
	CleanupStack::PushL(smsbuffer);
	smsbuffer->iBuffer=new(ELeave) CArrayFixSeg<TText>(EMaxBufLength);
	CleanupStack::Pop();
	return smsbuffer;
	} // CSmsBuffer::NewL


/**
 *  Destructor, frees resource.
 *  @capability None
 */
EXPORT_C CSmsBuffer::~CSmsBuffer()
	{
    delete iBuffer;
    } // CSmsBuffer::NewL


/**
 *  Gets the amount of space currently used in the buffer.
 *  
 *  @return Amount of space currently used in the buffer
 *  @capability None
 */
EXPORT_C TInt CSmsBuffer::Length() const
	{
	return iBuffer->Count();
	} // CSmsBuffer::Length


/**
 *  Extracts buffer data to a descriptor.
 *  
 *  @param aBuf On return, buffer data
 *  @param aPos Position within buffer to begin reading
 *  @param aLength The amount of data to read from the buffer
 *  @capability None
 */
EXPORT_C void CSmsBuffer::Extract(TDes& aBuf,TInt aPos,TInt aLength) const
	{
	aBuf.SetLength(aLength);
	for (TInt i=0; i<aLength; i++)
		aBuf[i]=iBuffer->At(aPos+i);
	} // CSmsBuffer::Extract


/**
 *  Inserts data into the buffer.
 *  
 *  @param aPos Position in the buffer to insert the data
 *  @param aBuf The data to insert into the buffer
 *  @capability None
 */
EXPORT_C void CSmsBuffer::InsertL(TInt aPos,const TDesC& aBuf)
	{
	iBuffer->InsertL(aPos,aBuf.Ptr(),aBuf.Length());
	} // CSmsBuffer::InsertL


/**
 *  Deletes data from the buffer.
 *  
 *  @param aPos Position in the buffer to delete the data
 *  @param aLength The amount of data to delete from the buffer
 *  @capability None
 */
EXPORT_C void CSmsBuffer::DeleteL(TInt aPos,TInt aLength)
	{
	iBuffer->Delete(aPos,aLength);
	} // CSmsBuffer::DeleteL


/**
 *  Resets the buffer.
 *  @capability None
 */
EXPORT_C void CSmsBuffer::Reset()
	{
	iBuffer->Reset();
	} // CSmsBuffer::Reset


CSmsBuffer::CSmsBuffer()
	{
	// NOP
	} // CSmsBuffer::CSmsBuffer


/**
 *  Allocates and constructs the buffer.
 *  
 *  @param aText Text object to use as buffer
 *  @return New CSmsEditorBuffer object
 *  @capability None
 */
EXPORT_C CSmsEditorBuffer* CSmsEditorBuffer::NewL(CEditableText& aText)
	{
	CSmsEditorBuffer* smseditorbuffer=new(ELeave) CSmsEditorBuffer(aText);
	CleanupStack::PushL(smseditorbuffer);
	CleanupStack::Pop();
	return smseditorbuffer;
	} // CSmsEditorBuffer::NewL


/**
 *  Destructor.
 *  @capability None
 */
EXPORT_C CSmsEditorBuffer::~CSmsEditorBuffer()
	{
	// NOP
    } // CSmsEditorBuffer::NewL


/**
 *  Gets the number of characters in the buffer.
 *  
 *  @return The number of characters in the buffer.
 *  @capability None
 */
EXPORT_C TInt CSmsEditorBuffer::Length() const
	{
	return iText.DocumentLength();
	} // CSmsEditorBuffer::Length


/**
 *  Extracts buffer data to a descriptor.
 *  
 *  @param aBuf On return, buffer data
 *  @param aPos Position within buffer to begin reading
 *  @param aLength The amount of data to read from the buffer
 *  @capability None
 */
EXPORT_C void CSmsEditorBuffer::Extract(TDes& aBuf,TInt aPos,TInt aLength) const
	{
	iText.Extract(aBuf,aPos,aLength);
	} // CSmsEditorBuffer::Extract


/**
 *  Inserts data into the buffer.
 *  
 *  @param aPos Position in the buffer to insert the data
 *  @param aBuf The data to insert into the buffer
 *  @capability None
 */
EXPORT_C void CSmsEditorBuffer::InsertL(TInt aPos,const TDesC& aBuf)
	{
	iText.InsertL(aPos,aBuf);
	} // CSmsEditorBuffer::InsertL


/**
 *  Deletes data from the buffer.
 *  
 *  @param aPos Position in the buffer to delete the data
 *  @param aLength The amount of data to delete from the buffer
 *  @capability None
 */
EXPORT_C void CSmsEditorBuffer::DeleteL(TInt aPos,TInt aLength)
	{
	iText.DeleteL(aPos,aLength);
	} // CSmsEditorBuffer::DeleteL


/**
 *  Reset the buffer.
 *  @capability None
 */
EXPORT_C void CSmsEditorBuffer::Reset()
	{
	iText.Reset();
	} // CSmsEditorBuffer::Reset


CSmsEditorBuffer::CSmsEditorBuffer(CEditableText& aText):
    iText(aText)
    {
    // NOP
    } // CSmsEditorBuffer::CSmsEditorBuffer
