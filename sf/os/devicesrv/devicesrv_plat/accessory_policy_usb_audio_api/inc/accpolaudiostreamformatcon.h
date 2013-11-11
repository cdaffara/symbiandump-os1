/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Object container for audio stream format objects.
*
*/

#ifndef ACCPOLAUDIOSTREAMFORMATCON_H
#define ACCPOLAUDIOSTREAMFORMATCON_H

#include <accpolobjectcon.h>
#include <accpolaudiostreamformat.h>


/**
 *  Stores an audio stream format objects.
 *
 *  @accpolaudiodevicetopology.lib
 */
NONSHARABLE_CLASS( CAccPolAudioStreamFormatCon ) : public CAccPolObjectCon
    {

public:

    /**
     * Destructor.
     */
    virtual ~CAccPolAudioStreamFormatCon();

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolAudioStreamFormatCon* NewL( CAccPolObjectCon* aObject = NULL );

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolAudioStreamFormatCon* NewLC( CAccPolObjectCon* aObject = NULL );

    /**
     * Gets a input stream audio format objects.
     *
     * @param aInputStreamFormats, Reference to pointer array where the objects are stored.
     * @return void.
     */
    IMPORT_C void GetInputStreamFormatsL( RPointerArray<CAccPolAudioStreamFormat>& aInputStreamFormats ) const;

    /**
     * Gets a output stream audio format objects.
     *
     * @param aOutputStreamFormat, Reference to pointer array where the objects are stored.
     * @return void.
     */
    IMPORT_C void GetOutputStreamFormatsL( RPointerArray<CAccPolAudioStreamFormat>& aOutputStreamFormat ) const;

    /**
     * Duplicates the object data.
     *
     * @return CAccPolObjectBase*, Pointer to duplicated object.
     */
    CAccPolObjectBase* DuplicateLC( CAccPolObjectCon* aObject = NULL );

    /**
     * Externalize object.
     *
     * @param aStream, Write stream object.
     * @return void.
     */
    void InternalizeL( RReadStream& aStream );

    /**
     * Internalize object.
     *
     * @param aStream, Read stream object.
     * @return void.
     */
    void ExternalizeL( RWriteStream& aStream ) const;

private:

    /**
     * C++ default constructor.
     */
    CAccPolAudioStreamFormatCon( CAccPolObjectCon* aObject = NULL );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

  };

#endif // ACCPOLAUDIOSTREAMFORMATCON_H
