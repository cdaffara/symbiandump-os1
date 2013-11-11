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
* Description:  Definition for audio feature unit object.
*
*/

#ifndef ACCPOLFEATUREUNIT_H
#define ACCPOLFEATUREUNIT_H

#include <accpolaudiounitbase.h>
#include <accpolaudiocontrolbase.h>

/**
 *  Class definition for audio feature unit objects.
 *
 *  @accpolaudiodevicetopology.lib
 */
NONSHARABLE_CLASS ( CAccPolFeatureUnit ) : public CAccPolAudioUnitBase
    {

public:

    /**
     * Destructor.
     */
    virtual ~CAccPolFeatureUnit();

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolFeatureUnit* NewL( CAccPolObjectCon* aObject = NULL );

    /**
     * Two-phased constructor.
     * @param aUnitId, Audio unit Id of this audio unit.
     * @param aSourceId, Source unit id where this audio unit is linked.
     */
    IMPORT_C static CAccPolFeatureUnit* NewL( const TUint8 aUnitId,
                                              const TUint8 aSourceId );

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolFeatureUnit* NewLC( CAccPolObjectCon* aObject = NULL );

    /**
     * Two-phased constructor.
     * @param aUnitId, Audio unit Id of this audio unit.
     * @param aSourceId, Source unit id where this audio unit is linked.
     */
    IMPORT_C static CAccPolFeatureUnit* NewLC( const TUint8 aUnitId,
                                               const TUint8 aSourceId );

    /**
     * Assign operator for audio feature unit class.
     *
     * @param CAccPolFeatureUnit, Const reference to base class to be copied.
     * @return CAccPolFeatureUnit, Reference to object pointer.
     */
    IMPORT_C CAccPolFeatureUnit& operator= ( const CAccPolFeatureUnit& );

    /**
     * Set a bimap which indicates that the mentioned control(s) is/are supported in channel.
     *
     * @param aChannel, Channel number. Supported channels see accpolaudiotopologytypes.h.
     * @param aBitmap, Bitmap. Supported audio controls see accpolaudiotopologytypes.h.
     * @return void.
     */
    IMPORT_C void SetSupportedControls( TUint32 aChannel, TUint32 aBitmap );

    /**
     * Get a bimap which indicates that the mentioned control(s) is/are supported in channel.
     *
     * @param aChannel, Channel number. Supported channels see accpolaudiotopologytypes.h.
     * @return TUint32, Bitmap. Supported audio controls see accpolaudiotopologytypes.h.
     */
    IMPORT_C TUint32 SupportedControls( TUint32 aChannel ) const;

    /**
      * Tells whether the supported audio controls given as parameter are
      * suppported.
      *
      * @param aChannel, Channel number. Supported channels see accpolaudiotopologytypes.h.
      * @param aSupportedControlsBitMask, Audio control(s) bitmask definition.
      *        Can contain one or more definitions if the given parameter is
      *        a result of OR -bit operation, for instance
      *        TUint32 KParamBitMask = KAccMuteControl | KAccVolumeControl.
      *        Bitmasks are defined in accpolaudiotopologytypes.h.
      * @return TBool indicating whether channel supports an audio controls (Etrue) or not (EFalse).
      */
    IMPORT_C TBool SupportedControls( TUint32 aChannel,
                                      TUint32 aSupportedControlsBitMask ) const;

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
    void ExternalizeL( RWriteStream& aStream ) const;

    /**
     * Internalize object.
     *
     * @param aStream, Read stream object.
     * @return void.
     */
    void InternalizeL( RReadStream& aStream );

private:

    /**
     * C++ default constructor.
     */
     CAccPolFeatureUnit( CAccPolObjectCon* aObject = NULL );

    /**
     * C++ constructor.
     */
    CAccPolFeatureUnit( const TUint8 aUnitId,
                        const TUint8 aSourceId );
    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:

   /**
    * Supported controls.
    */
   TFixedArray< TUint32, KAccChannelCount > iSupportedControls;

    };

#endif // ACCPOLFEATUREUNIT_H


