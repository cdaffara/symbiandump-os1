// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef __ASYNCFILEPARSER_H
#define __ASYNCFILEPARSER_H

//  INCLUDES
#include <e32base.h>
#include "mp4atom.h"

// CONSTANTS

// FORWARD DECLARATIONS
class RFile64;
struct MP4HandleStruct;

// CLASS DECLARATION

/**
*  Async file parser
*/
NONSHARABLE_CLASS(CFileAsyncParser) : public CActive
    {
	public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CFileAsyncParser* NewL( MP4HandleStruct* aHandle, RFile64& aFile );

        /**
        * Destructor.
        */
        ~CFileAsyncParser();
    
	public: // New functions

        /**
        * Reads audio frames from file asynchronously
        * @since 3.1
        * @return Error code, KErrNone if no errors, otherwise one of the system wide error codes.
        */
        TInt ReadAudioFrames( mp4_u8 *buffer, mp4_i64 aPosition, mp4_u32 aBytesToRead );

        /**
        * Reads video frames from file asynchronously
        * @since 3.1
        * @return Error code, KErrNone if no errors, otherwise one of the system wide error codes.
        */
        TInt ReadVideoFrame(  mp4_u8 *buffer, mp4_i64 aPosition, mp4_u32 aBytesToRead );

	protected: // Functions from base classes
        /**
        * From CActive Cancels async request.
        */
        void DoCancel();

        /**
        * From CActive Called when async request completes.
        */
	    void RunL();

	private:
        /**
        * C++ default constructor.
        */
        CFileAsyncParser();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( MP4HandleStruct* aHandle, RFile64& aFile );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        TInt ReadDataAsync( mp4_u8 *buffer, mp4_i64 aPosition, mp4_u32 aBytesToRead );
        
        /**
        * Return audio frames to observer
        */
		void ReturnAudioFrames();

        /**
        * Return video frame to observer
        */
        void ReturnVideoFrame();

        /**
        * Calculates audio framecount in buffer
        */        
        TInt CalculateAudioFrameCount();
        
	private:
		// Handle to parsed file structure.
		MP4HandleStruct* iHandle;
		
		// Disk buffer.
		HBufC8* iDiskBuffer;
		// Pointer to iDiskBuffer data
		TPtr8 iDiskBufferPointer;
		// Disk buffer size
		TInt iReadBufferSize;		
		// Position of last read Diskbuffer in file
		TInt64 iCurrentDiskReadPosition;
		// Position of last read inside iDiskBuffer.
		TUint iCurrentBufferReadPosition;
		// Whether on last parse request all data was found on internal buffer.
		TBool iAllDataInMemory;
		
		// Audio parameters to be filled.
		// Size of audio frame returned to client.
		mp4_u32 iAudioSize;
		// Number of audio frames in buffer.
		mp4_u32 iReturnedAudioFrames;
		// Audio frame presentation time in milliseconds from the beginning of the audio sequence.
		mp4_u32 iAudioTimeStamp; 		
		// 	Audio frame presentation time in timescale from the	beginning of the audio sequence
		mp4_u32 iAudioTimeStamp2; 		
	
		// Video parameter to be filled.
		// Size of video frame returned to client.
		mp4_u32 iVideoSize;
		// Whether returned video frame is keyframe or not.
		mp4_bool iVideoKeyFrame;
		// Video frame presentation time in milliseconds from the beginning of the video sequence.
		mp4_u32 iVideoTimeStamp; 		
		// Video frame presentation time in timescale from the	beginning of the video sequence
		mp4_u32 iVideoTimeStamp2; 		
		
		// Read logic variables.
		// Amount of bytes client wants to read
		TInt iBytesToRead;
		// Amount of bytes read.
		TInt64 iBytesRead;
		// Flag whether there is async writing going.
	    TBool iAsyncReadOngoing;
	    // Flag whether current read is audio.
	    TBool iProcessingAudio;
		// Write error code.
	    TInt iError;

        // Parsed file being read.
        RFile64* iInputFile;
		// Output buffer being written to
		mp4_u8* iBuffer;
};

#endif  //__ASYNCFILEPARSER_H
// End of File
