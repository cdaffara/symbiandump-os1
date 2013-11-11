/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Public API class of the Generic ID, which provides means to 
*                describe, detect, connect and control an enhancement 
*                without need to use HW and/or provider specific definitions.
*
*/



#ifndef TACCPOLGENERICID_H
#define TACCPOLGENERICID_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <AccPolGIDHeader.h> 
#include <AccPolGenericIdDefinitions.h>

// CONSTANTS

// CLASS DECLARATION

/**
*  Generic ID class definition.
*  The Generic ID provides means to describe, detect, connect
*  and control an enhancement without need to use HW and/or provider specific 
*  definitions.
*
*  @lib AccPolicy.lib
*  @since S60 3.1
*/
class TAccPolGenericID
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        IMPORT_C TAccPolGenericID();

    public: // New functions

        /**
        * Tells whether the device type(s) given as parameter are suppported by 
        * the accessory in question.
        *
        * @since S60 3.1
        * @param aDeviceTypeBitMask Device type bitmask definition.
        *        Can contain one or more definitions if the given parameter is
        *        a result of OR -bit operation, for instance
        *        TUint32 KParamBitMask = KDTHeadset | KDTCarKit.        
        *        Device type bitmasks are defined in AccPolGenericID.h.        
        * @return TBool indicating whether accessory supports the device type(s)
        *         (Etrue) or not (EFalse). 
        */
        IMPORT_C TBool DeviceTypeCaps( const TUint32 aDeviceTypeBitMask ) const;

        /**
        * Returns the device type(s) of the accessory in question.
        *
        * @since S60 3.1
        * @return Device type(s) of the accessory in question.
        *         Device type bitmasks are defined in AccPolGenericID.h. 
        */
        IMPORT_C TUint32 DeviceTypeCaps() const;

        /**
        * Tells whether the physical connection type(s) given as parameter are 
        * suppported by the accessory in question.
        *
        * @since S60 3.1
        * @param aPhysicalConnectionBitMask Physical connection bitmask definition.
        *        Can contain one or more definitions if the given parameter is 
        *        a result of OR -bit operation, for instance 
        *        TUint32 KParamBitMask = KPCWired | KPCBluetooth.
        *        Physical connection bitmasks are defined in AccPolGenericID.h.
        * @return TBool indicating whether accessory supports the physical connection type(s)
        *         (Etrue) or not (EFalse). 
        */
        IMPORT_C TBool PhysicalConnectionCaps( const TUint32 aPhysicalConnectionBitMask ) const;

        /**
        * Returns the physical connection type(s) of the accessory in question.
        *
        * @since S60 3.1
        * @return Physical connection type(s) of the accessory in question.
        *         Physical connection bitmasks are defined in AccPolGenericID.h.
        */
        IMPORT_C TUint32 PhysicalConnectionCaps() const;

        /**
        * Tells whether the application protocol type(s) given as parameter are 
        * suppported by the accessory in question.
        *
        * @since S60 3.1
        * @param aApplicationProtocolBitMask Application Protocol bitmask definition.
        *        Can contain one or more definitions if the given parameter is 
        *        a result of OR -bit operation, for instance 
        *        TUint32 KParamBitMask = KAPAT | KAPOBEX.
        *        Application Protocol bitmasks are defined in AccPolGenericID.h.
        * @return TBool indicating whether accessory supports the application protocol type(s)
        *         (Etrue) or not (EFalse).
        */
        IMPORT_C TBool ApplicationProtocolCaps( const TUint32 aApplicationProtocolBitMask ) const;

        /**
        * Returns the application protocol type(s) of the accessory in question.
        *
        * @since S60 3.1
        * @return Application protocol type(s) of the accessory in question
        *         Application Protocol bitmasks are defined in AccPolGenericID.h.
        */
        IMPORT_C TUint32 ApplicationProtocolCaps() const;

        /**
        * Tells whether the capability group(s) given as parameter are 
        * suppported by the accessory in question.
        *
        * @since S60 3.1
        * @param aCapabilityGroupBitMask Capability group bitmask definition.
        *        Can contain one or more definitions if the given parameter is 
        *        a result of OR -bit operation, for instance 
        *        TUint32 KParamBitMask = KSBAudioSubblock | KSBVideoSubblock.
        *        Capability group bitmasks are defined in AccPolGenericID.h.
        * @return TBool indicating whether accessory supports the capability group(s)
        *         (Etrue) or not (EFalse).
        */
        IMPORT_C TBool SubblockCaps( const TUint64 aCapabilityGroupBitMask ) const;

        /**
        * Returns all the supported capability groups of the accessory in question.
        *
        * @since S60 3.1
        * @return Supported capability groups of the accessory in question.
        *         Capability group bitmasks are defined in AccPolGenericID.h.
        */
        IMPORT_C TUint64 SubblockCaps() const;

        /**
        * Returns the model identifier for this accessory.
        * The form of identifier, and if it exists, depends on the accessory
        * itself.
        *
        * @since S60 3.1
        * @return Pointer to HW Model Id. Max size is KHWModelIDMaxLength.
        */
        IMPORT_C TPtrC HWModelID() const; 
        
        /**
        * Returns the hardware device specific identifier for this accessory.
        * The form of identifier, and if it exists, depends on the accessory
        * itself.
        *
        * @since S60 3.1
        * @return hardware device identifier
        */
        IMPORT_C TUint64 HWDeviceID() const; 
        
        /**
        * Returns the device address for this accessory.
        * Value can be 0, if accessory doesn't have device address.
        *        
        * @since S60 3.1
        * @return device address
        */
        IMPORT_C TUint64 DeviceAddress() const;
        
        /**
        * Returns the unique identifier of this Generic ID.
        *
        * @since S60 3.1
        * @return Accessory unique identifier.
        */
        IMPORT_C TInt UniqueID() const;
        
    private:    // Data
        // Instance of Generic ID static attribute data structure
        TAccPolGIDHeader iStaticAttributes;

    private:    // Friend classes
        friend class TAccPolGenericIDAccessor;
    };

#endif      // TACCPOLGENERICID  
            
// End of File
