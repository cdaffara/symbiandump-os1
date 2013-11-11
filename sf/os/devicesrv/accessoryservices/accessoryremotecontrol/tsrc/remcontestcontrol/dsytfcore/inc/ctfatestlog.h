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
* Description:  Declaration of CTFATestLog class
*
*/


#ifndef __CTFATESTLOG_H__
#define __CTFATESTLOG_H__

#include <e32base.h>

/**
* Logger styles
*/
enum TTFLogStyle
    {
    ETFLogStyleNormal       = 0,
    ETFLogStyleBold         = 1,
    ETFLogStyleItalic       = 2
    };

/**
* Abstraction for test logger. The implementation is in the adapter DLL
*/
class CTFATestLog : public CBase
    {
    protected:
        /**
        * Constructor
        */
        IMPORT_C CTFATestLog( void );
    
    public:
        /**
        * Destructor
        */
        IMPORT_C ~CTFATestLog( void );
    
    private:
        /**
        * Copy constructor is hidden
        */
        CTFATestLog( const CTFATestLog& aLog );

        /**
        * Assignment operator is hidden
        */
        CTFATestLog& operator=( const CTFATestLog& aLog );
    
    public:
        /**
        * Writes data to log
        */
        virtual void Write( const TDesC& aDes ) = 0;

        /**
        * Writes data to log
        */
        virtual void Write( const TDesC8& aDes ) = 0;

        /**
        * Writes formatted data to log
        */
        virtual void Write( TRefByValue<const TDesC> aDes, ... ) = 0;

        /**
        * Writes formatted data to log
        */
        virtual void Write( TRefByValue<const TDesC8> aDes, ... ) = 0;

        /**
        * Writes a block of data to log. If HTML logging is used, this
        * wraps the data into <pre><blockquote>
        */
        virtual void WriteBlock( const TDesC& aDes ) = 0;

        /**
        * Writes a block of data to log. If HTML logging is used, this
        * wraps the data into <pre><blockquote>
        */
        virtual void WriteBlock( const TDesC8& aDes ) = 0;

        /**
        * Writes an end-of-line character into log.
        */
        virtual void EndLine( void ) = 0;

        /**
        * Starts a new chapter
        */
        virtual void StartChapter( void ) = 0;

        /**
        * Starts a list
        */
        virtual void StartList( void ) = 0;

        /**
        * Writes formatted data into a list at given depth
        */
        virtual void WriteList( TInt aDepth, TRefByValue<const TDesC> aDes, ... ) = 0;

        /**
        * Writes formatted data into a list at given depth
        */
        virtual void WriteList( TInt aDepth, TRefByValue<const TDesC8> aDes, ... ) = 0;

        /**
        * Ends a list
        */
        virtual void EndList( void ) = 0;

        /**
        * Sets current logging style
        */
        virtual void SetStyle( TTFLogStyle aStyle ) = 0;

        /**
        * Sets current logging color
        */
        virtual void SetColor( TUint32 aColor ) = 0;
    };
    
#endif
