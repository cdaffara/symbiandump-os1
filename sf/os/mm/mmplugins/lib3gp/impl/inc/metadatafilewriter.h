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

#ifndef __METADATAFILEWRITER_H
#define __METADATAFILEWRITER_H

//  INCLUDES
#include <e32base.h>
#include <3gplibrary/mp4lib.h>
#include "mp4atom.h"

// FORWARD DECLARATIONS
class RFile64;

#define NUM_MDF      10     /* Number of temporary metadata files */

// CLASS DECLARATION

NONSHARABLE_CLASS(CMetaDataWriterBuffer) : public CBase
    {
    public:  // Constructors and destructor

        inline CMetaDataWriterBuffer() : iData( NULL ), iOutputFileNum( -1 ) {}

        inline CMetaDataWriterBuffer( HBufC8& aBuffer, 
                            const TInt aOutputFileNum) 
                            : iData(&aBuffer), iOutputFileNum(aOutputFileNum) {}

        /**
        * Destructor.
        */
        ~CMetaDataWriterBuffer();

    public:
        HBufC8* iData;
        TInt iOutputFileNum;
    };

/**
*  Async buffering file writer.
*/
NONSHARABLE_CLASS(CMetaDataFileWriter) : public CActive
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMetaDataFileWriter* NewL( );

        /**
        * Destructor.
        */
        ~CMetaDataFileWriter();
    
    public: // New functions

        /**
        * Writes incoming buffer data to internal buffers for writing to disk.
        * @since 2.6
        * @param aFileNumber Index of file the data is to be written
        * @param aBuf Data to be written
        * @return Error code, KErrNone if no errors, otherwise one of the system wide error codes.
        */
        TInt Write( RFile64& aFile, const TInt aFileNumber, const TDesC8& aBuf );

        /**
        * Flush internal buffers to disk.
        * @since 2.6
        * @return Error code, KErrNone if no errors, otherwise one of the system wide error codes.
        */
        TInt Flush();

        /**
        * Reads data from internal buffers that have not yet been written to disk.
        * @since 5.0
        * @param aFileNumber Index of file
        * @param aBuf Data to be written
        * @param bytestoread Number of bytes to readData
        * @return number of bytes read
        */
        TInt ReadBuffer( const TInt aFileNumber, TDes8& aBuf, TInt bytestoread );

    protected: // Functions from base classes
        
        /**
        * From CActive Cancels async request.
        */
        void DoCancel();

        /**
        * From CActive Called when async request completes.
        */
	    void RunL();
	    
	    /**
	    * Called when errors in RunL force Leave. 
	    */
	    TInt RunError(TInt aError);

private:

        /**
        * C++ default constructor.
        */
        CMetaDataFileWriter();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( );

        /**
        * Writes incoming data to internal buffers and buffer queues..
        * @since 2.6
        * @param aFileNumber Index of file the data is to be written
        * @param aBuf Data to be added to buffers..
        * @return Error code, KErrNone if no errors, otherwise one of the system wide error codes.
        */
        TInt AddDataToBuffer(const TInt aFileNumber, const TDesC8& aBuf );

private:
		// Write error code.
	    TInt iError;
        // Whether we are flushing after async write.
    	TBool iFlush;
        // Whether the flushing is done.
    	TBool iFlushDone;
        // Array to keep file status information
        TBool iAsyncWritingOngoing;
        // Outputfiles
        RPointerArray<RFile64> iOutputFile;
        // Array of output buffers.
        RPointerArray<CMetaDataWriterBuffer> iOutputBufferQueue;
        // Array of input buffers.
        RPointerArray<CMetaDataWriterBuffer> iInputBufferArray;
        // Queue of empty write buffers.
        RPointerArray<CMetaDataWriterBuffer> iEmptyBufferQueue;
        // Array to count delivered bytes from iInputBufferArray
        TInt iInputBufferArrayDelivered[NUM_MDF];
};

#endif  //__METADATAFILEWRITER_H
// End of File
