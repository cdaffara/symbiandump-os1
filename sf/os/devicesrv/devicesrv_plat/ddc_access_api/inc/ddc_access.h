/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: API for DDC (E-EDID and displayID) read from DVI/HDMI display
*
*/
/*







            ddc_access.dll
            ----------------

            SW Include Document - C++








Module name:    ddc_access.dll

Filename:       ddc_access.h

Working copy:   ddc_access

*  Continuus version  :
*   %version: 2 %,
    %date_modified: Thu May 06 12:35:27 2010 %

---------------------------------------------------------------------------*/
/**

  @mainpage
  @section intro_sec Introduction

  <p>This is adaptation component which hides hw-drivers from users.
     Because this is adaptation API, and user is also adaptation, this
     API is not binary compatible, only source compatible.</p>

  <p>API for DDC (E-EDID and displayID) read from DVI/HDMI display.</p>

  <p>@see EDDCv1_2.pdf/DisplayIDv1.pdf/EEDIDrAr2.pdf (or newer) for more details</p>

*/
/** @file

*///=========================================================================


#ifndef __DDC_ACCESS_H__
#define __DDC_ACCESS_H__


//- Include Files  ----------------------------------------------------------

#ifndef __KERNEL_MODE__
 #include <e32base.h>
#else
 #include <e32cmn.h>
#endif //__KERNEL_MODE__

//- Enumerations and consts -------------------------------------------------

/** API version number:
 2:  __KERNEL_MODE__ definitions, capability ECapabilityReadDeviceData
 1:  First version */
const TUint KDdcAccessVersion = 1;

///One DDC segment size. See that EDID block size is just 128 bytes, so base-EDID read might contain one EDID extension
const TUint KSizeOfSegment = 256;

/** One data block array */
typedef TUint8 TDataBlock[ KSizeOfSegment ];

///DDC port enumerations
enum TDdcPort
    {
    EUndefinedPort = 0x00, //!<Initialisation value, don't use
    EMonitorPort   = 0xA0, //!<0xA0/0xA1, DDC monitor port. Normally for EDID, but if EDID is not supported here is DisplayID
    EDisplayIdPort = 0xA4, //!<0xA4/0xA5, if EDID is supported, here may exists DisplayID
    };

//- Namespace ---------------------------------------------------------------


//- Using -------------------------------------------------------------------


//- Data Types --------------------------------------------------------------


//- Constants ---------------------------------------------------------------


//- Macros ------------------------------------------------------------------


//- External Data -----------------------------------------------------------


//- Variables ---------------------------------------------------------------


//- Forward Declarations ----------------------------------------------------


//- Class Definitions -------------------------------------------------------
#ifndef __KERNEL_MODE__

/**
Adaptation API for VESA's E-DDC (Enhanced Display Data Channel) raw read
*/
class CDdcPortAccess : public CBase
    {
    public:
        /** Gets API version number. Compare this to KDdcAccessVersion, must match.
            @return  KDdcAccessVersion which is used for implementation */
        IMPORT_C static TUint ApiVersion();

        /** Creates object.
            Method requires ECapabilityReadDeviceData capability.
            @return  new CDdcPortAccess, NULL if there is error, otherwise CDdcPortAccess */
        IMPORT_C static CDdcPortAccess* NewL();
        ///Destructor
        virtual ~CDdcPortAccess(){};

        /** Starts DDC read from wanted port.

            User should first read EMonitorPort, if there is EDID found, then read also EDisplayIdPort.
            If DisplayID is found from EMonitorPort, then it is not needed to read EDisplayIdPort.

            DisplayID is "second generation" of EDID, and it is proposed to be used.

            The method reads one TDataBlock, and its size is double of one EDID block,
            so the first EDID read might get base-EDID and first extension, if such exists.
            With variable size DisplayID, method reads the whole size of TDataBlock anyway,
            even if DisplayID section is smaller.

            @param  aDdcPort            DDC port which needs to be read
            @param  aBlockNumber        Datablock which needs to be read (parameter maps to segment-register 0x60)
            @param  aDataBlock          Method fills this with read values. Ensure that this data exists all the time, be careful if local variable.
            @param  aCompletedWhenRead  Will be completed when ready, if no errors, then aDataBlock contains read values
            @return KErrNone if success */
        virtual TInt Read(TDdcPort aDdcPort, TUint aBlockNumber, TDataBlock& aDataBlock, TRequestStatus& aCompletedWhenRead) = 0;

        /** Cancels all TRequestStatuses. */
        virtual void CancelAll() = 0;

    protected:
        ///Constructor
        CDdcPortAccess(){};
        ///Second phase constructor
        virtual void ConstructL() = 0;
    };

#endif //__KERNEL_MODE__
//- Global Function Prototypes ----------------------------------------------


//- Inline Functions --------------------------------------------------------


#endif //__DDC_ACCESS_H__

// End of File

