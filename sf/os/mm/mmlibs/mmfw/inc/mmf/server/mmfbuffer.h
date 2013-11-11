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
// include\mmf\server\mmfbuffer.h
// 
//


#ifndef __MMF_SERVER_BUFFER_H__
#define __MMF_SERVER_BUFFER_H__

#include <e32base.h>
#include <mmf/server/mmfbuffer.hrh>


/**
@publishedAll
@released

Flag reflecting the current buffer status.
*/
enum TBufferStatus
	{
	/** The buffer is available for use */
	EAvailable,

	/** The buffer is currently being filled */
	EBeingFilled,

	/** The buffer is full */
	EFull,

	/** The buffer is currently being emptied */
	EBeingEmptied,

	/** The buffer is currently unavailable */
	EUnAvailable
	};


/**
@publishedAll
@released

Abstract representation of a buffer to contain multimedia data.
*/
class CMMFBuffer : public CBase
	{
public:
	IMPORT_C static TBool IsSupportedDataBuffer(TUid aUid);
	IMPORT_C static TBool IsFileServerSafe(TUid aUid);
	/**
	Returns the buffer type.

	@return The buffer type.
	*/
	TUid Type() const {return iType;};
	/**
	Sets the buffer's status.

	@param  aStatus
	        The buffer's status.
	*/
	virtual void SetStatus(TBufferStatus aStatus) 
			{iStatus = aStatus; if (iStatus == EAvailable) iPosition = 0;}
	/**
	Returns the buffer's status.

	@return The buffer's status.
	*/
	TBufferStatus Status() {return iStatus;};

	/**
	Returns the size of the data in the buffer.

	This is a virtual function that each derived class must implement.

	@return The buffer size.
	*/
	virtual TUint BufferSize() const = 0;

	/**
	Destructor.
	*/
	virtual ~CMMFBuffer() {};
	inline TTimeIntervalMicroSeconds TimeToPlay() const;
	inline void SetTimeToPlay (TTimeIntervalMicroSeconds aTime);
	inline TUint FrameNumber() const;
	inline void SetFrameNumber(TUint aFrame);
	inline void NextFrame();
	inline virtual void SetPosition (TUint aPosition);
	inline virtual TUint Position() const;
	inline virtual TInt RequestSize() const;
	inline void SetLastBuffer(TBool aLastBuffer);
	inline TBool LastBuffer() const;
protected:
	CMMFBuffer(TUid aType): iType(aType), iStatus(EAvailable) {};	

protected:
	/** 
	The buffer type. 
	*/
	TUid iType;

	/** 
	The current buffer status.

	@see enum TBufferStatus
	*/
	TBufferStatus iStatus;

	/**
	The buffer timestamp, in microseconds. 
	*/
	TTimeIntervalMicroSeconds iTimeToPlay;

	/** 
	Used to store the current read/write position. Required when the codec and data path
	may have to read and write the buffer in more than one pass.
	*/
	TUint iPosition;

	/** 
	The frame number.
	*/
	TUint iFrameNumber;

	/**
	Stores Request size, needed for dynamic buffer length.
	*/
	TInt iRequestSize;

	/** 
	Indicates if this is the last buffer. ETrue if it is.
	*/
	TBool iLastBuffer; 
	};

/**
Returns the buffer timestamp, in microseconds.

@return	The buffer timestamp, in microseconds.
*/
inline TTimeIntervalMicroSeconds CMMFBuffer::TimeToPlay() const
	{
	return iTimeToPlay;
	}

/**
Sets the buffer timestamp, in microseconds.

@param  aTime
        The buffer timestamp, in microseconds.
*/
inline void CMMFBuffer::SetTimeToPlay (TTimeIntervalMicroSeconds aTime)
	{
	iTimeToPlay = aTime;
	}

/**
Returns the frame number.

@return The frame number.
*/
inline TUint CMMFBuffer::FrameNumber() const
	{
	return iFrameNumber;
	}

/**
Sets the frame number. This is used for repositioning.

@param  aFrameNumber
        The frame number.
*/
inline void CMMFBuffer::SetFrameNumber (TUint aFrameNumber)
	{
	iFrameNumber = aFrameNumber;
	}

/**
Sets the buffer as the next frame.
*/
inline void CMMFBuffer::NextFrame()
	{
	iFrameNumber++;
	}


/**
Sets the current buffer read/write position.
Used as read/write position on a buffer where a codec may need
several passes on the same buffer.

@param  aPosition
        The buffer's number.
*/
inline void CMMFBuffer::SetPosition (TUint aPosition)
	{
	iPosition = aPosition;
	}

/**
Returns the current buffer read/write position.
Used as read/write position on a buffer where a codec may need
several passes on the same buffer.

@return The buffer's number.
*/
inline TUint CMMFBuffer::Position() const
	{
	return iPosition;
	}

/**
Tests whether the buffer is the last buffer.

@return A boolean indicating if the buffer is the last one. ETrue if it is the last buffer, 
        EFalse otherwise.
*/
inline TBool CMMFBuffer::LastBuffer() const
	{
	return iLastBuffer;
	}

/**
Sets the buffer as the last buffer.

@param  aLastBuffer
        A boolean indicating if the buffer is the last buffer. ETrue if it is the last buffer, 
        EFalse otherwise.
*/
inline void CMMFBuffer::SetLastBuffer(TBool aLastBuffer)
	{
	iLastBuffer = aLastBuffer;
	}

/** 
Returns the size of data processed by sink or data needed by source.

@return The size of data
*/
inline TInt CMMFBuffer::RequestSize() const
	{
	return iRequestSize;
	}

#endif
