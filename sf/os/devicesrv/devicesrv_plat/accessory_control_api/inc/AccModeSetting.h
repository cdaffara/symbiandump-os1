/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: ?Description
*
*/



#ifndef ACCMODESETTING_H
#define ACCMODESETTING_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS
const TInt KMaxAccModeSetting = 256; // Descriptor max length

// MACROS

// DATA TYPES

// Defines possible setting types
enum TAccModeSettingType
    {
    EAccModeSettingNotSet,
    EAccModeSettingInt,
    EAccModeSettingBool,
    EAccModeSettingDes
    };
    
// Accessory Settings API, light setting values for accessories.
// @see  SetAccessoryModeSettingL
enum TAccLightSettings
    {
    EAccLightsAutomatic,    //Phone is not forced to leave lights on.    
    EAccLightsOn            //Phone is forced to leave lights on.
    };
    
// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Used to deliver parameters in some of RAccessorySettings class
*  methods.
*
*  @lib AccControl.lib
*  @since S60 3.1
*/
class TAccModeSetting
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        IMPORT_C TAccModeSetting();

    public: // New functions

        /**
        * Sets identification number for setting.
        *
        * If any of the settings listed in AccSettingsDomainCRKeys.h are
        * stored to Accessory Server, following two things have to be taken
        * into account:
        *
        * 1) Identification number is equal to constant value in
        * AccSettingsDomainCRKeys.h. E.g. if lights are set for wired headset
        * mode, then aId = KAccServerWiredHSLights.
        *
        * 2) Value of the setting has to be of type TInt. This means that value
        * has to be set with method TAccModeSetting::SetTInt(const TInt aValue)
        *
        * If setting is read from Accessory Server, following thing has to be
        * taken into account before reading:
        *
        * 1) Set identification number of setting so that Accessory Server knows
        * what setting is wanted to be read.
        *
        * @since S60 3.1
        * @param aId identifies setting
        * @return void
        */
        IMPORT_C void SetId( const TUint32 aId );

        /**
        * Sets value for setting that has TInt as a type.
        *
        * All settings listed in AccSettingsDomainCRKeys.h are of type TInt.
        *
        * @since S60 3.1
        * @param aValue setting's value
        * @return void
        */
        IMPORT_C void SetTInt( const TInt aValue );

        /**
        * Sets value for setting that has TBool as a type.
        *
        * @since S60 3.1
        * @param aValue setting's value
        * @return void
        */
        IMPORT_C void SetTBool( const TBool aValue );

        /**
        * Sets value for setting that has descriptor as a type.
        *
        * @since S60 3.1
        * @param aValue setting's value
        * @return KErrArgument if the length of aValue is longer than maximum
        *     descriptor length ( KMaxAccModeSetting )
        */
        IMPORT_C TInt SetTDes( const TDesC& aValue );

        /**
        * Returns type of setting.
        *
        * @since S60 3.1
        * @return See definition of AccModeSettingType for possible values
        */
        IMPORT_C TAccModeSettingType Type() const;

        /**
        * Returns identification number for setting.
        *
        * @since S60 3.1
        * @return indentification number
        */
        IMPORT_C TUint32 Id() const;

        /**
        * Returns value of setting that has TInt as a type.
        *
        * All settings listed in AccSettingsDomainCRKeys.h are of type TInt.
        *
        * @since S60 3.1
        * @param aValue stores setting when method has been succesfully called
        * @return KErrNotFound if setting's type isn't TInt
        */
        IMPORT_C TInt GetTInt( TInt& aValue ) const;

        /**
        * Returns value of setting that has TBool as a type.
        *
        * @since S60 3.1
        * @param aValue stores setting when method has been succesfully called
        * @return KErrNotFound if setting's type isn't TBool
        */
        IMPORT_C TInt GetTBool( TBool& aValue ) const;

        /**
        * Returns value of setting that has descriptor as a type.
        *
        * @since S60 3.1
        * @param aDes stores setting when method has been succesfully called
        * @return KErrNotFound if setting's type isn't descriptor
        */
        IMPORT_C TInt GetTDes( TDes& aDes ) const;

    private:    // Data

        // Indentifies setting
        TUint32 iId;
        // Describes setting type
        TAccModeSettingType iType;
        // Stores setting
        TBuf<KMaxAccModeSetting> iValue;

    };

#endif      // ACCMODESETTING_H

// End of File
