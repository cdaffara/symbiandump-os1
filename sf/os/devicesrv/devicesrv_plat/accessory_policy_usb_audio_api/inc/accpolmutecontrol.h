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
* Description:  Definition for mute control object.
*
*/

#ifndef ACCPOLMUTECONTROL_H
#define ACCPOLMUTECONTROL_H

#include <accpolaudiocontrolbase.h>


/**
 *  Class definition for mute control objects.
 *
 *  @accpolaudiodevicetopology.lib
 */
NONSHARABLE_CLASS ( CAccPolMuteControl ) : public CAccPolAudioControlBase
    {

public:

    /**
     * Destructor.
     */
    virtual ~CAccPolMuteControl();

    /**
      * Two-phased constructor.
      */
    IMPORT_C static CAccPolMuteControl* NewL( CAccPolObjectCon* aObject = NULL );

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolMuteControl* NewLC( CAccPolObjectCon* aObject = NULL );

    /**
     * Two-phased constructor.
     * @param aChannel, Channel number where this control is targeted.
     * @param aMute, Mute value to be set.
     */
    IMPORT_C static CAccPolMuteControl* NewL( TUint32 aChannel,
                                              const TBool aMute );

    /**
     * Two-phased constructor.
     * @param aChannel, Channel number where this control is targeted.
     * @param aMute, Mute value to be set.
     */
    IMPORT_C static CAccPolMuteControl* NewLC( TUint32 aChannel,
                                               const TBool aMute );

    /**
     * Assign operator for mute control class.
     *
     * @param CAccPolMuteControl, Const reference to base class to be copied.
     * @return CAccPolMuteControl, Reference to object pointer.
     */
    IMPORT_C CAccPolMuteControl& operator= ( const CAccPolMuteControl& );

    /**
     * Sets the mute on/off.
     *
     * @param aMute, Value of mute: mute on = ETrue, mute off = EFalse.
     * @return void.
     */
    IMPORT_C void SetMute( TBool aMute );

    /**
     * Returns the mute value.
     *
     * @return TBool, Mute value: mute on = ETrue, mute off = EFalse.
     */
    IMPORT_C TBool Mute() const;

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
    CAccPolMuteControl( CAccPolObjectCon* aObject = NULL );

    /**
     * C++ constructor.
     */
    CAccPolMuteControl( TUint32 aChannel,
                        const TBool aMute );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:

    /**
     * Value of mute.
     */
    TBool iMute;

    };

#endif // ACCPOLMUTECONTROL_H
