/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ECom interface for effect data getter plugin.
*
*/

#ifndef C_HWRMHAPTICSEFFECTDATAGETTER_H
#define C_HWRMHAPTICSEFFECTDATAGETTER_H

#include <e32base.h>
#include <ecom/ecom.h>
#include <hwrmhaptics.h>

/**
 *  Haptics effect data getter ECom plugin interface.
 *
 *  @lib hwrmhapticseffectdatagetterfw.lib
 *  @since S60 5.1
 */
class CHWRMHapticsEffectDataGetter : public CBase
    {
public:

    /**
     * Instantiates an object of this type. If there are no plugins found,
     * the method leaves with KErrNotSupported. If there are more than
     * one plugin found, the method leaves with KErrNotSupported 
     * (current implementation relies on one plugin).
     *
     * @since S60 5.1
     *
     * @return Instance of haptics effect data getter plugin.
     */
    IMPORT_C static CHWRMHapticsEffectDataGetter* NewL();

    /**
     * Destructor.
     *
     * @since S60 5.1
     */
    IMPORT_C virtual ~CHWRMHapticsEffectDataGetter();

public:

    /**
     * Returns the amount of effects in the given data.
     * 
     * @param aData The data from which the effect count is read.
     * @param aCount Returned effect count.
     *
     * @since S60 5.1
     *
     * @return KErrNone if success, otherwise one of Symbian error codes.
     */ 
    virtual TInt GetEffectCount( const TDesC8& aData, TInt& aCount ) = 0;

    /**
     * Returns the name of the effect in the given index.
     * 
     * @param aData        The data from which the effect name is read.
     * @param aEffectIndex Index of the effect.
     * @param aEffectName  Returned effect name.
     *
     * @since S60 5.1
     *
     * @return KErrNone if success, otherwise one of Symbian error codes.
     */
    virtual TInt GetEffectName( const TDesC8& aData,
                                TInt aEffectIndex, 
                                TDes8& aEffectName ) = 0;

    /**
     * Returns the effect index in the given data.
     * 
     * @param aData        The data from which the effect index is read.
     * @param aEffectName  Name of the effect.
     * @param aEffectIndex Returned effect index.
     *
     * @since S60 5.1
     *
     * @return KErrNone if success, otherwise one of Symbian error codes.
     */ 
    virtual TInt GetEffectIndexFromName( const TDesC8& aData,
                                         const TDesC8& aEffectName,
                                         TInt& aEffectIndex ) = 0;

    /**
     * Returns the type of the effect.
     * 
     * @param aData        The data from which the effect type is read.
     * @param aEffectIndex Index of the effect.
     * @param aEffectType  Returned effect type.
     *
     * @since S60 5.1
     *
     * @return KErrNone if success, otherwise one of Symbian error codes.
     */
    virtual TInt GetEffectType( const TDesC8& aData, 
                                TInt aEffectIndex, 
                                TInt& aEffectType ) = 0;

    /**
     * Returns mag sweep effect definition in the given index.
     * 
     * @param aData        The data from which the effect definition is read.
     * @param aEffectIndex Index of the effect.
     * @param aEffect      Returned effect data.
     *
     * @since S60 5.1
     *
     * @return KErrNone if success, otherwise one of Symbian error codes.
     */ 
    virtual TInt GetMagSweepEffectDefinition( 
                      const TDesC8& aData, 
                      TInt aEffectIndex, 
                      CHWRMHaptics::THWRMHapticsMagSweepEffect& aEffect ) = 0;
                                                 
    /**
     * Returns periodic effect definition in the given index.
     * 
     * @param aData        The data from which the effect definition is read.
     * @param aEffectIndex Index of the effect.
     * @param aEffect      Returned effect data.
     *
     * @since S60 5.1
     *
     * @return KErrNone if success, otherwise one of Symbian error codes.
     */
    virtual TInt GetPeriodicEffectDefinition( 
                      const TDesC8& aData, 
                      TInt aEffectIndex, 
                      CHWRMHaptics::THWRMHapticsPeriodicEffect& aEffect ) = 0;
                
    /**
     * Returns the duration of the given effect.
     * 
     * @param aData           The data from which the effect duration is read.
     * @param aEffectIndex    Index of the effect.
     * @param aEffectDuration Returned effect duration.
     *
     * @since S60 5.1
     *
     * @return KErrNone if success, otherwise one of Symbian error codes.
     */
    virtual TInt GetEffectDuration( const TDesC8& aData, 
                                    TInt aEffectIndex, 
                                    TInt& aEffectDuration ) = 0;


private:
    
    // Unique instance identifier key.  
    TUid iDtor_ID_Key;
    
    };
    
#endif // C_HWRMHAPTICSEFFECTDATAGETTER_H
