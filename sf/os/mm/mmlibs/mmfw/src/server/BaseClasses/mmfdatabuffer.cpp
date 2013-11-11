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
// source\mmf\server\baseclasses\mmfdatabuffer.cpp
// 
//

#include <mmf/server/mmfdatabuffer.h>




/**
Method to instantiate a CMMFDataBuffer defaults to a CMMFDescriptorBuffer
to maintain buffer compatiblity with MFAD ie. instantiating a CMMFDataBuffer defaults to creating 
a CMMFDescriptorBuffer. This NewL creates a CMMFDescriptorBuffer with a default size of 32 bytes.

@return	A pointer to a new CMMFDescriptorBuffer.
*/
EXPORT_C CMMFDataBuffer* CMMFDataBuffer::NewL()
	{
	return CMMFDescriptorBuffer::NewL();
	}

/**
Method to instantiate a CMMFDataBuffer defaults to a CMMFDescriptorBuffer by default
to maintain buffer compatiblity with MFAD ie. instantiating a CMMFDataBuffer defaults to
creating a CMMFDescriptorBuffer. This NewL creates a CMMFDescriptorBuffer with a size of 
aMaxBufferSize bytes.

@param  aMaxBufferSize
        The size in bytes of the descriptor buffer to be created.

@return A pointer to a new CMMFDescriptorBuffer.
*/
EXPORT_C CMMFDataBuffer* CMMFDataBuffer::NewL(TInt aMaxBufferSize)
	{
	return CMMFDescriptorBuffer::NewL(aMaxBufferSize);
	}


/**
Method to instantiate a CMMFDescriptorBuffer.

Defaults to a CMMFDescriptorBuffer automatically. This NewL creates a CMMFDescriptorBuffer with a 
default size of 32 bytes.

@return A pointer to a new CMMFDescriptorBuffer.
*/
EXPORT_C CMMFDescriptorBuffer* CMMFDescriptorBuffer::NewL()
	{
	CMMFDescriptorBuffer* self = new(ELeave) CMMFDescriptorBuffer;
	CleanupStack::PushL(self);
	self->ConstructL(KMMFDataBufferDefaultBufferSize);
	CleanupStack::Pop(); // self
	return self;
	}


/**
Method to instantiate a CMMFDescriptorBuffer.
This NewL creates a CMMFDescriptorBuffer with a size of aMaxBufferSize bytes.

@param  aMaxBufferSize
        The size in bytes of the descriptor buffer to be created.

@return A pointer to a new CMMFDescriptorBuffer.
*/
EXPORT_C CMMFDescriptorBuffer* CMMFDescriptorBuffer::NewL(TInt aMaxBufferSize)
	{
	CMMFDescriptorBuffer* self = new(ELeave) CMMFDescriptorBuffer;
	CleanupStack::PushL(self);
	self->ConstructL(aMaxBufferSize);
	CleanupStack::Pop(); // self
	return self;
	}

/**
@internalTechnology

Internal.

@param  aMaxBufferSize
        The size in bytes of the descriptor buffer to be created.
*/
void CMMFDescriptorBuffer::ConstructL(TInt aMaxBufferSize)
	{
	iData = new(ELeave) TUint8[aMaxBufferSize];
	iPtr.Set(iData, 0, aMaxBufferSize);
	}

/**
Destructor.

Destructor also deletes the buffer contained in the CMMFDescriptorBuffer.
*/
EXPORT_C CMMFDescriptorBuffer::~CMMFDescriptorBuffer()
	{
	delete[] iData;
	}

/**
Reallocates the max size in bytes of a CMMFDescriptorBuffer.

@param  aMaxBufferSize
        The new size in bytes of the descriptor buffer.
*/
EXPORT_C void CMMFDescriptorBuffer::ReAllocBufferL(TInt aMaxBufferSize)
	{
	TUint8* tmp = new(ELeave) TUint8[aMaxBufferSize];
	delete[] iData;
	iData = tmp;
	iPtr.Set(iData, 0, aMaxBufferSize);
	}

/**
Returns a descriptor to the data contained in the CMMFDescriptorBuffer.

@return A reference to a TPtr containing the CMMFDescriptorBuffer data.
*/
TDes8& CMMFDescriptorBuffer::Data()
	{
	return iPtr;
	}

/**
Returns a descriptor to the data contained in the CMMFDescriptorBuffer.

@return A const reference to a TPtr containing the CMMFDescriptorBuffer data.
*/
const TDesC8& CMMFDescriptorBuffer::Data() const
	{
	return iPtr;	
	}

/**
Returns the actual data size (ie. not the maximum length) of the
data contained in the CMMFDescriptorBuffer.

@return	The size in bytes of the data contained in the CMMFDescriptorBuffer.
*/
TUint CMMFDescriptorBuffer::BufferSize() const
	{
	return iPtr.Length();
	}

/**
Sets the position.

This method is used by components (eg codecs) which read data from a buffer
and wish to store a read position marker for further reads.
Note: The position cannot exceed the size of the actual data not the max length.

@param  aPosition
        The position.
*/
void CMMFDescriptorBuffer::SetPosition (TUint aPosition)
	{//used for repositioning
	if (aPosition <= (TUint)iPtr.Length()) 
		iPosition = aPosition;
	else 
		iPosition = (TUint)iPtr.Length();//tried to position beyond end of data
	}

/**
Sets the request size.

This function is used in cases where a component (eg a data source) may not be able
or be desirable to write to the entire max length of the buffer (eg variable bit rate codecs).
In which case the SetRequestSizeL() can be set which can be read by the RequestSize()
function in the component so that it knows to only write data upto the request size
and not fill the buffer up to its max length.

@param  aSize
        The request size.
*/
void CMMFDescriptorBuffer::SetRequestSizeL(TInt aSize)
	{
	if (aSize < 0)
		User::Leave(KErrUnderflow);
	else if (aSize > iPtr.MaxLength())
		User::Leave(KErrOverflow);
	else
		iRequestSize = aSize;
	}


/**
Overriden method to set the status and resets the data size to 0 when the buffer becomes available.

@param  aStatus
        The buffer status. See TBufferStatus for possible options.
*/
void CMMFDescriptorBuffer::SetStatus(TBufferStatus aStatus)
	{
	CMMFBuffer::SetStatus(aStatus);
	if ((iStatus == EAvailable)&&iData)
		{//need to set size to 0
		iPtr.Zero();
		}
	}


/**
This function is not supported under EKA2.

Method to instantiate a CMMFTransferBuffer. This NewL creates a CMMFTransferBuffer.

@param  aTransferWindow
        This is a valid RTransferWindow that has an RTransferBuffer mapped in.
@param  aDataLength
        This parameter sets the length of the actual data present in the transferbuffer.
        This is because the length of actual data may be less than the length of the mapped in
        transfer buffer.

@return A pointer to a new CMMFTransferBuffer.
*/

EXPORT_C CMMFTransferBuffer* CMMFTransferBuffer::NewL(RTransferWindow& aTransferWindow, TUint aDataLength)
	{
//this method is not supported under EKA2
	User::Panic(_L("Not supported!"), KErrNotSupported);
	aTransferWindow = aTransferWindow;	//suppressed unused argument warnings
	aDataLength =  aDataLength;			//suppressed unused argument warnings
	return NULL;			//can't construct anything useful
	}

/**
@internalTechnology

This method is not supported under EKA2.

Internal ConstructL.

Note this method checks if a transfer buffer has been mapped in and
will leave with KErrNotReady if the RTransferWindow does not have a mapped
in RTransferBuffer.

@param  aTransferWindow
        This is a reference to a valid RTransferWindow that has an RTransferBuffer mapped in.
@param  aDataLength
        The length of the data.
*/
void CMMFTransferBuffer::ConstructL(RTransferWindow& aTransferWindow, TUint aDataLength)
	{
//this method is not supported under EKA2
	aTransferWindow = aTransferWindow;	//suppressed unused argument warnings
	aDataLength =  aDataLength;			//suppressed unused argument warnings
	}


/**
CMMFTransferBuffer destructor

Destructor maps out RTransferBuffer and closes RTransferWindow.
*/
EXPORT_C CMMFTransferBuffer::~CMMFTransferBuffer()
	{
	}

/**
Returns a descriptor to the data contained in the CMMFTransferBuffer.

@return	A reference to a TPtr containing the CMMFTransferBuffer data.
*/
TDes8& CMMFTransferBuffer::Data()
	{
	return iPtr;
	}

/**
Returns a descriptor to the data contained in the CMMFTransferBuffer.

@return	A const reference to a TPtr containing the CMMFTransferBuffer data.
*/
const TDesC8& CMMFTransferBuffer::Data() const
	{
	return iPtr;
	}

/**
Returns the actual data size (ie. not the max length) of the
data contained in the CMMFTransferBuffer.

@return	The size in bytes of the data contained in the CMMFTransferBuffer.
*/
TUint CMMFTransferBuffer::BufferSize() const
	{
	return iPtr.Length();
	}


/**
Sets the position.

This method is used by components (eg codecs) which read data from a buffer
and wish to store a read position marker for further reads.

Note: The position cannot exceed the size of the actual data not the max length.

@param  aPosition
        The position.
*/
void CMMFTransferBuffer::SetPosition (TUint aPosition)
	{//used for repositioning
	aPosition = aPosition; //suppress compiler warning
	}

/**
Sets the request size.

This function is used in cases where a component (eg. a data source) may not be able
or be desirable to write to the entire max length of the buffer (eg. variable bit rate codecs).
In this case, the SetRequestSizeL can be set which can be read by the RequestSize()
function in the component so that it knows to only write data upto the request size
and not fill the buffer up to its max length.

@param  aSize
        The request size.
*/
void CMMFTransferBuffer::SetRequestSizeL(TInt aSize)
	{
	aSize = aSize; //suppress compiler warning
	}


/**
This function is not supported under EKA2.

Returns a reference to the transfer window currently used
by the CMMFtransferBuffer.

@return	A reference to the current RTransferWindow.
*/
EXPORT_C RTransferWindow& CMMFTransferBuffer::TransferWindow()
	{
	return iTransferWindow;
	}


/**
This method is not supported under EKA2.

Modifies the CMMFTransferBuffer by updating the RTransferWindow.

This method is used if the same CMMFTransferBuffer is used throughout
eg. if a single CMMFTransferBuffer is created upfront but a different
transfer window (or the same transfer window with a different buffer mapped in
is used). That is the same CMMFTransferBuffer but the actrual buffer may be different.

Note: If the updated RTransferWindow is new, then the old buffer must
be mapped out first by a call to CMMFTransferBuffer::MapOutBuffer() and the
RtransferWindow handle closed outside the CMMFTransferBuffer.

@param  aTransferWindow
        The RTransferWindow to update - can be a new RTransferWindow
        or the same RTransferWindow with a new RTransferBuffer mapped in.
@param  aDataLength
        The length of the data.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
*/
EXPORT_C TInt CMMFTransferBuffer::UpdateTransferWindow(RTransferWindow& aTransferWindow, TUint aDataLength)
	{
//this method is not supported under EKA2
	aTransferWindow = aTransferWindow;	//suppressed unused argument warnings
	aDataLength =  aDataLength;			//suppressed unused argument warnings
	return KErrNotSupported;
	}

/**
Maps the buffer out of the transfer window.

This method should be called in preference to
calling MapOutBuffer directly on the RtransferWindow
so that the CMMFTransferBuffer knows that it is no longer
available.
*/
EXPORT_C void CMMFTransferBuffer::MapOutBuffer()
	{
	}



/**
Function to instantiate a CMMFPtrBuffer.
This NewL creates an unititalised CMMFPtrBuffer.

@return A pointer to a new CMMFPtrBuffer.
*/
EXPORT_C CMMFPtrBuffer* CMMFPtrBuffer::NewL()
	{
	CMMFPtrBuffer* self = new(ELeave) CMMFPtrBuffer;

	return self;
	}


/**
Function to instantiate a CMMFPtrBuffer.
This NewL creates a CMMFPtrBuffer which owns a TPtr8.

@param  aPtr
        A reference to a TPtr containing the CMMFPtrBuffer data.

@return A pointer to a new CMMFPtrBuffer.
*/
EXPORT_C CMMFPtrBuffer* CMMFPtrBuffer::NewL(const TPtr8& aPtr)
	{
	CMMFPtrBuffer* self = new(ELeave) CMMFPtrBuffer;
	CleanupStack::PushL(self);
	self->ConstructL(aPtr);
	CleanupStack::Pop(self); // self
	return self;
	}

/**
 * ConstructL
 *
 * Internal ConstructL
 * @internalTechnology
 * @param	"aPtr"
 *			Reference to a TPtr containing the CMMFPtrBuffer data
 */
void CMMFPtrBuffer::ConstructL(const TPtr8& aPtr)
	{
	iPtr.Set(aPtr);
	}

/**
Destructor.

Destructor does no deletion, as this buffer class does not own the memory.
*/
EXPORT_C CMMFPtrBuffer::~CMMFPtrBuffer()
	{

	}

/**
Returns a descriptor to the data contained in the CMMFPtrBuffer.

@return	A reference to a TPtr containing the CMMFPtrBuffer data.
*/
TDes8& CMMFPtrBuffer::Data()
	{
	return iPtr;	
	}

/**
Returns a descriptor to the data contained in the CMMFPtrBuffer.

@return	A const reference to a TPtr containing the CMMFPtrBuffer data.
*/
const TDesC8& CMMFPtrBuffer::Data() const
	{
	return iPtr;	
	}

/**
Returns the actual data size (ie. not the max length) of the
data contained in the CMMFPtrBuffer.

@return	The size in bytes of the data contained in the CMMFPtrBuffer.
*/
TUint CMMFPtrBuffer::BufferSize() const
	{
	return iPtr.Length();
	}

/**
Sets the position.

This function is used by components (eg. codecs) which read data from a buffer
and wish to store a read position marker for further reads.

Note: The position cannot exceed the size of the actual data not the maximum length.

@param  aPosition
        The position.
*/
void CMMFPtrBuffer::SetPosition (TUint aPosition)
	{//used for repositioning
	if (aPosition <= (TUint)iPtr.Length()) 
		iPosition = aPosition;
	else 
		iPosition = (TUint)iPtr.Length();//tried to position beyond end of data
	}

/**
Sets the request size.

This method is used in cases where a component (eg. a data source) may not be able
or be desirable to write to the entire max length of the buffer (eg. variable bit rate codecs).
In this case, the SetRequestSizeL() can be set which can be read by the RequestSize()
function in the component so that it knows to only write data upto the requested size
and not fill the buffer up to its maximum length.

@param  aSize
        The request size.
*/
void CMMFPtrBuffer::SetRequestSizeL(TInt aSize)
	{
	if (aSize < 0)
		User::Leave(KErrUnderflow);
	else if (aSize > iPtr.MaxLength())
		User::Leave(KErrOverflow);
	else
		iRequestSize = aSize;
	}


/**
Overriden method to set the status.
Resets the data size to 0 when the buffer becomes available.

@param  aStatus
        The buffer status. See enum TBufferStatus.
*/
void CMMFPtrBuffer::SetStatus(TBufferStatus aStatus)
	{
	CMMFBuffer::SetStatus(aStatus);
	if (iStatus == EAvailable)
		{//need to set size to 0
		iPtr.Zero();
		}
	}


/** 
Takes a TPtr8 to pre-allocated memory.

@param  aPtr
		The pointer refernce.
*/
EXPORT_C void CMMFPtrBuffer::SetPtr(const TPtr8& aPtr)
	{
	iPtr.Set(aPtr);
	}


//This functions needs updating
//should more CMMFDataBuffers be supported in future
/**
Static method which returns ETrue if the buffer UID is a supported
CMMFDataBuffer type.

Note:
If the buffer is not a CMMFDataBuffer this method should
return EFalse.

@param  aUid
        The UID of the CMMFBuffer to be checked for support.
@return The buffer size.
*/
EXPORT_C TBool CMMFBuffer::IsSupportedDataBuffer(TUid aUid)
	{ 
	return((aUid == KUidMmfDescriptorBuffer)
		|| (aUid == KUidMmfTransferBuffer)
		|| (aUid == KUidMmfPtrBuffer));
	}


/**
Static method which returns ETrue if the buffer UID is a buffer
that is safe to be used with the file server.  If the buffer type
is not safe to be used with the file server, then the client would
need to copy the contents of the buffer, prior to passing it onto
the file server.

This implementation assumes the CMMFPtrBuffer is safe for file server copy. If this is not the case 
then remove the PtrBuffer set to ETrue.

@param  aUid
        The UID of the CMMFBuffer to be checked for support.

@return The buffer size.
*/
EXPORT_C TBool CMMFBuffer::IsFileServerSafe(TUid aUid)
	{
	TBool isFileServerSafe = EFalse;

	if (aUid == KUidMmfDescriptorBuffer)
		isFileServerSafe = ETrue;
	if (aUid == KUidMmfPtrBuffer) //remove this if target CMMFPtrBuffers
		isFileServerSafe = ETrue; //are not safe for file server copy
	
	return isFileServerSafe;
	}
