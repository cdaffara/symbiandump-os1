/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of CTFAStifTestLog class
*
*/


#ifndef __TFASTIFTESTLOG_H__
#define __TFASTIFTESTLOG_H__

#include "ctfatestlog.h"

class CStifLogger;
class TTFAOverflowHandler;
const TInt KMaxLogLength = 254; // From STIF

/**
* Implements the abstract logger interface to delegate logging to STIF-TF logger.
*/
NONSHARABLE_CLASS( CTFAStifTestLog ): public CTFATestLog
    {
    public:
        /**
        * Creates a new logger
        */
        static CTFAStifTestLog* NewLC( void );

        /**
        * Destructor
        */
        virtual ~CTFAStifTestLog( void );
    
    private:
        /**
        * Constructor
        */
        CTFAStifTestLog( void );

        /**
        * 2nd phase constructor
        */
        void ConstructL( void );

        /**
        * Copy constructor is hidden
        */
        CTFAStifTestLog( const CTFAStifTestLog& aLog );

        /**
        * Assignment operator is hidden
        */
        CTFAStifTestLog& operator=( const CTFAStifTestLog& aLog );

    public:
        /**
        * Writes data to log
        */
        void Write( const TDesC& aDes );

        /**
        * Writes data to log
        */
        void Write( const TDesC8& aDes );

        /**
        * Writes formatted to log
        */
        void Write( TRefByValue<const TDesC> aDes, ... );

        /**
        * Writes formatted to log
        */
        void Write( TRefByValue<const TDesC8> aDes, ... );

        /**
        * Writes a block of data to log. If logging HTML, this wraps the block into <pre><blockquote>
        */
        void WriteBlock( const TDesC& aDes );

        /**
        * Writes a block of data to log. If logging HTML, this wraps the block into <pre><blockquote>
        */
        void WriteBlock( const TDesC8& aDes );

        /**
        * Writes an end-of-line character to log
        */
        void EndLine( void );

        /**
        * Starts a new chapter
        */
        void StartChapter( void );

        /**
        * Starts a bulleted list
        */
        void StartList( void );

        /**
        * Writes formatted data into bulleted list at given depth
        */
        void WriteList( TInt aDepth, TRefByValue<const TDesC> aDes, ... );

        /**
        * Writes formatted data into bulleted list at given depth
        */
        void WriteList( TInt aDepth, TRefByValue<const TDesC8> aDes, ... );

        /**
        * Ends a bulleted list
        */
        void EndList( void );

        /**
        * Sets logging style
        */
        void SetStyle( TTFLogStyle aStyle );

        /**
        * Sets logging color
        */
        void SetColor( TUint32 aColor );

    private:
        /**
        * Writes start of list entry
        */
        void WriteListEntryStart( TInt aDepth );
    
    private:
        CStifLogger* iLogger;
        TBuf<KMaxLogLength> iLogBuffer;
        TBuf8<KMaxLogLength> iLogBuffer8;
        TTFAOverflowHandler* iOverflowHandler;
        TInt iCurrentDepth;
        TTFLogStyle iStyle;
        TInt iColor;
    };

#endif
