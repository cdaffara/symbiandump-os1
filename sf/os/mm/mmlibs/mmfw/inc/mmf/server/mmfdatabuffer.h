// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// include\mmf\server\mmfdatabuffer.h
// 
//


#ifndef __MMF_SERVER_DATABUFFER_H__
#define __MMF_SERVER_DATABUFFER_H__

#include <e32base.h>


#include <mmf/server/mmfbuffer.h>

/** 
@publishedAll
@released

Default buffer size if none is specified.
*/
static const TInt KMMFDataBufferDefaultBufferSize = 32;

/**
@publishedAll
@released

This class is an abstract class for databuffers that can be presented in the form of a descriptor.
A constructor is provided which instantiates a CMMFDescriptorBuffer buffer by default.

@see CMMFDescriptorBuffer
*/
class CMMFDataBuffer : public CMMFBuffer
	{
public:
	IMPORT_C static CMMFDataBuffer* NewL();
	IMPORT_C static CMMFDataBuffer* NewL(TInt aMaxBufferSize);
	/**
	Destructor.
	*/
	virtual ~CMMFDataBuffer() {};

	/**
	Returns a reference to the data contained in the buffer (non const version).

	This is a virtual function that each derived class must implement.

	@return A reference to the buffer.
	*/
	virtual TDes8& Data() = 0;

	/**
	Returns a reference to the data contained in the buffer (const version).

	This is a virtual function that each derived class must implement.

	@return A reference to the buffer.
	*/
	virtual const TDesC8& Data() const = 0;

	/**
	Returns the buffer size, in bytes.

	This is a virtual function that each derived class must implement.

	@return The buffer size in bytes.
	*/
	virtual TUint BufferSize() const = 0;

	/**
	Sets the request size. 

	Used where a component, such as a data source,
	may not be able to write to the entire maximum length of the buffer
	(variable bit rate codecs for example).

	This is a virtual function that each derived class must implement.

	@param  aSize
	        The requested size, in bytes.
	*/
	virtual void SetRequestSizeL(TInt aSize) = 0;
protected:
	/**
	Protected constructor.
	*/
	CMMFDataBuffer(TUid aType): CMMFBuffer(aType) {};
	};


/**
@publishedAll
@released

This class is a wrapper class to give a descriptor the same API as a CMMFDataBuffer.

The purpose of this class is that components such as codecs can use CMMFDataBuffers transparently
without having to be concerned with whether the buffer is a descriptor buffer
or a transfer buffer.
*/
class CMMFDescriptorBuffer : public CMMFDataBuffer
	{
public:
	IMPORT_C static CMMFDescriptorBuffer* NewL();
	IMPORT_C static CMMFDescriptorBuffer* NewL(TInt aMaxBufferSize);
	IMPORT_C ~CMMFDescriptorBuffer();
	IMPORT_C void ReAllocBufferL(TInt aMaxBufferSize);
	virtual TDes8& Data();
	virtual const TDesC8& Data() const;
	virtual void SetStatus(TBufferStatus aStatus);
	virtual void SetRequestSizeL(TInt aSize);
	virtual TUint BufferSize() const;
	virtual void SetPosition (TUint aPosition);
private:
	CMMFDescriptorBuffer() : CMMFDataBuffer(KUidMmfDescriptorBuffer), iPtr(0,0,0) {};
	void ConstructL(TInt aMaxBufferSize);
private:
	TUint8* iData;
	TPtr8 iPtr;
	};


/**
@publishedAll
@released

This class is a wrapper class to give a transfer buffer/window the same API as a CMMFDataBuffer.
The purpose of this class is so that components such as codecs can use CMMFDataBuffers transparently
without having to be concerned with whether the buffer is a descriptor buffer
or a transfer buffer.

The class assumes that a valid RTransferWindow with a RTransferBuffer mapped in already exists.

Note:
CMMFTransferBuffers are not supported in EKA2.
*/


class RTransferWindow
	{
	// dummy class
	};
	

class CMMFTransferBuffer : public CMMFDataBuffer
	{
public:

	IMPORT_C static CMMFTransferBuffer* NewL(RTransferWindow& aTransferWindow, TUint aDataLength = 0);
	IMPORT_C ~CMMFTransferBuffer();
	virtual TDes8& Data();
	virtual const TDesC8& Data() const;
	virtual TUint BufferSize() const;
	virtual void SetRequestSizeL(TInt aSize);
	virtual void SetPosition (TUint aPosition);
	IMPORT_C RTransferWindow& TransferWindow();
	IMPORT_C void MapOutBuffer();
	IMPORT_C TInt UpdateTransferWindow(RTransferWindow& aTransferWindow, TUint aDataLength = 0);

private:
	/**
	Private constructor.
	*/
	CMMFTransferBuffer() : CMMFDataBuffer(KUidMmfTransferBuffer), iPtr(0,0,0){};
	void ConstructL(RTransferWindow& aTransferWindow, TUint aDataLength);

private:
	RTransferWindow iTransferWindow;
	TBool iTransferWindowMappedInOK;
	TPtr8 iPtr;
	};


/**
@publishedAll
@released

This class is a wrapper class intended to provide support for shared I/O buffers under EKA2
This class, unlike other data buffers does not own memory - rather it contains a pointer to already allocated memory
The purpose of this class is that components such as codecs can use CMMFDataBuffers transparently
without having to be concerned with whether the buffer is a standard buffer or memory residing in shared I/O

Users of this class need to be very careful when using buffers of this type.
It cannot be assumed that a buffer of this type can be written/read by any process other than the current one - example - handing CMMFPtrBuffer to the file server may not work since the file server's process may not have access to write data buffer.
*/
class CMMFPtrBuffer : public CMMFDataBuffer
	{
public:
	IMPORT_C static CMMFPtrBuffer* NewL();		//unitialised, pointer will be set via SetPtr
	IMPORT_C static CMMFPtrBuffer* NewL(const TPtr8& aPtr);
	IMPORT_C ~CMMFPtrBuffer();
	IMPORT_C void SetPtr(const TPtr8& aPtr);
	virtual TDes8& Data();
	virtual const TDesC8& Data() const;
	virtual void SetStatus(TBufferStatus aStatus);	
	virtual void SetRequestSizeL(TInt aSize);
	virtual TUint BufferSize() const;
	virtual void SetPosition (TUint aPosition);
private:
	CMMFPtrBuffer() : CMMFDataBuffer(KUidMmfPtrBuffer), iPtr(0,0,0) {};
	void ConstructL(const TPtr8& aPtr);
private:
	TPtr8 iPtr;
	};

#endif
