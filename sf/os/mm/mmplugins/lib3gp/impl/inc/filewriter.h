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

#ifndef __FILEWRITER_H
#define __FILEWRITER_H

//  INCLUDES
#include <e32base.h>
#include "mp4atom.h"

// CONSTANTS
const TInt KFileWriterBufferSizeSmall = 65536;
const TInt KFileWriterBufferSizeLarge = (4*65536);  
const TInt KFileWriterSoftBufLimit = 12;
const TInt KFileWriterHardBufLimit = 16;
const TInt KFileWriterMinBufferCount = 4; // shouldn't be less than 4

// FORWARD DECLARATIONS
class RFile64;

// CLASS DECLARATION

/**
*  Async buffering file writer.
*/
NONSHARABLE_CLASS(CFileWriter) : public CActive
    {
    public:
        enum TOutputBufferSize
            {   
                EBufferSizeSmall = 0,
                EBufferSizeLarge, 
                EBufferSizeCustom
            };

    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CFileWriter* NewL( RFile64& aFile, TInt aInitSetSize = 0, TInt aOutputBufferSizeSmall = KFileWriterBufferSizeSmall, TInt aOutputBufferSizeLarge = KFileWriterBufferSizeLarge );

        /**
        * Destructor.
        */
        ~CFileWriter();
    
    public: // New functions

        /**
        * Writes incoming buffer data to internal buffers for writing to disk.
        * @since 2.1
        * @param aBuf Data to be written
        * @return Error code, KErrNone if no errors, otherwise one of the system wide error codes.
        */
        TInt Write( const TDesC8& aBuf );

        /**
        * Flush internal buffers to disk.
        * @since 2.1
        * @param aBuf Additional data to be written before flush.
        * @return Error code, KErrNone if no errors, otherwise one of the system wide error codes.
        */
        TInt Flush( const TDesC8& aBuf );

        /**
        * Set file output buffer size
        * @since 2.6
        * @param aBufferSize Size of buffer.
        * @param aHandle MP4Handle.
        * @return Error code, KErrNone if no errors, otherwise one of the system wide error codes.
        */
        TInt SetOutputBufferSize( TOutputBufferSize aBufferSize, MP4Handle aHandle );
        
        /**
        * Set file output buffer count
        * @since 3.0
        * @param aHandle MP4Handle.
        */
        void SetOutputBufferCount( MP4Handle aHandle ); 

        inline TInt64 OutputFileSize() const
			{
			return iOutputFileSize;
			}    

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
        CFileWriter( TInt aInitSetSize, TInt aOutputBufferSizeSmall, TInt aOutputBufferSizeLarge );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( RFile64& aFile );

        /**
        * Writes incoming data to internal buffers and buffer queues..
        * @since 2.1
        * @param aBuf Data to be added to buffers..
        * @return Error code, KErrNone if no errors, otherwise one of the system wide error codes.
        */
        TInt AddDataToBuffer( const TDesC8& aBuf );

        /**
        * Allocates input and output buffers.
        * @since 2.6
        */
        void AllocateBuffersL();
		
		/**
		* Updates output file size and reserves extra space for following writing if iSetSize is set.
		* Takes into account if the position in the file was changed.
		*/
        void UpdateOutputFileSize();

private:
        // Whether we are flushing after async write.
    	TBool iFlush;
		// Flag whether there is async writing going.
	    TBool iAsyncWritingOngoing;
		// Flag whether we have received any data.
        TBool iWritingStarted;
        // Flag whether init has been done
        TBool iMemReadyForWriting;

        // Write error code.
        TInt iError;
        
        // Current set file size
        TInt64 iSetSize;
        // Current output file size
	    TInt64 iOutputFileSize;		

        TInt iOutputBufferSizeSmall;
        TInt iOutputBufferSizeLarge;
        
        // Output buffer size.
        TInt iOutputBufferSize;
        // Hard limit for max output buffers
        TInt iMaxOutputBufHardLimit;
        // Soft limit for max output buffers
        TInt iMaxOutputBufSoftLimit;


        // Output file for writes.
        RFile64* iOutputFile;
        // Queue of empty write buffers.
        RPointerArray<HBufC8> iEmptyBufferQueue;
        // Queue of full write buffers.
        RPointerArray<HBufC8> iFullBufferQueue;

        // Current input buffers for incoming data.
        HBufC8* iInputBuf;
};

#endif  //__FILEWRITER_H
// End of File
