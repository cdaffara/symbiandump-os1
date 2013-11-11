/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  API definition file for tv-out configuration.
*
*/

#ifndef __TVOUTCONFIGDEF_H__
#define __TVOUTCONFIGDEF_H__

//- Include Files  ----------------------------------------------------------

#include <e32base.h>

//- Class and Enumeration Definitions ---------------------------------------

/**
CEA specified fixed modes.
E<Columns>x<Total Lines><p=progressive/i=interlaced><Refresh rate separated with d, _ means .>Hz<Picture aspect ratio>
Enumeration value matching to specified mode number.
*/
enum TFixedModeCea
    {
    ECeaFixedModeNone           = 0,

    //CEA fixed modes (CEA-861-B.pdf)
    E640x480p59_94d60Hz4d3      = 1,
    E720x480p59_94d60Hz4d3      = 2,
    E720x480p59_94d60Hz16d9     = 3,
    E1280x720p59_94d60Hz16d9    = 4,
    E1920x1080i59_94d60Hz16d9   = 5,
    E720x480i59_94d60Hz4d3      = 6,
    E720x480i59_94d60Hz16d9     = 7,
    E720x240p59_94d60Hz4d3      = 8,
    E720x240p59_94d60Hz16d9     = 9,
    E2880x480i59_94d60Hz4d3     = 10,
    E2880x480i59_94d60Hz16d9    = 11,
    E2880x240p59_94d60Hz4d3     = 12,
    E2880x240p59_94d60Hz16d9    = 13,
    E1440x480p59_94d60Hz4d3     = 14,
    E1440x480p59_94d60Hz16d9    = 15,
    E1920x1080p59_94d60Hz16d9   = 16,
    E720x576p50Hz4d3            = 17,
    E720x576p50Hz16d9           = 18,
    E1280x720p50Hz16d9          = 19,
    E1920x1080i50Hz16d9_1125    = 20, // 1125 vertical lines
    E720x1080i50Hz16d9          = 20, // Wrong value. Will be deprecated.
    E720x576i50Hz4d3            = 21,
    E720x576i50Hz16d9           = 22,
    E720x288p50Hz4d3            = 23,
    E720x288p50Hz16d9           = 24,
    E2880x576i50Hz4d3           = 25,
    E2880x576i50Hz16d9          = 26,
    E2880x288p50Hz4d3           = 27,
    E2880x288p50Hz16d9          = 28,
    E1440x576p50Hz4d3           = 29,
    E1440x576p50Hz16d9          = 30,
    E1920x1080p50Hz16d9         = 31,
    E1920x1080p23_97d24Hz16d9   = 32,
    E1920x1080p25Hz16d9         = 33,
    E1920x1080p29_97d30Hz16d9   = 34,

    //CEA fixed modes (CEA-861-D.pdf)
    E2880x480p59_94d60Hz4d3     = 35,
    E2880x480p59_94d60Hz16d9    = 36,
    E2880x576p50Hz4d3           = 37,
    E2880x576p50Hz16d9          = 38,
    E1920x1080i50Hz16d9_1250    = 39, // 1250 vertical lines
    E1920x1080i50Hz16d9         = 39, // Confusing name. Will be deprecated.
    E1920x1080i100Hz16d9        = 40,
    E1280x720p100Hz16d9         = 41,
    E720x576p100Hz4d3           = 42,
    E720x576p100Hz16d9          = 43,
    E720x576i100Hz4d3           = 44,
    E720x576i100Hz16d9          = 45,
    E1920x1080i119_88d120Hz16d9 = 46,
    E1280x720p119_88d120Hz16d9  = 47,
    E720x480p119_88d120Hz4d3    = 48,
    E720x480p119_88d120Hz16d9   = 49,
    E720x480i119_88d120Hz4d3    = 50,
    E720x480i119_88d120Hz16d9   = 51,
    E720x576p200Hz4d3           = 52,
    E720x576p200Hz16d9          = 53,
    E720x576i200Hz4d3           = 54,
    E720x576i200Hz16d9          = 55,
    E720x480p239_76d240Hz4d3    = 56,
    E720x480p239_76d240Hz16d9   = 57,
    E720x480i239_76d240Hz4d3    = 58,
    E720x480i239_76d240Hz16d9   = 59,

    //CEA fixed modes (CEA-861-E.pdf)
    E1280x720p23_97d24Hz16d9    = 60,
    E1280x720p25Hz16d9          = 61,
    E1280x720p29_97d30Hz16d9    = 62,
    E1920x1080p119_88d120Hz16d9 = 63,
    E1920x1080p100Hz16d9        = 64,

    ELastCeaFixedModeValue,	//this must be last.
    };

/**
VESA specified fixed modes.
E<Columns>x<Total Lines><p=progressive/i=interlaced><Refresh rate>Hz<RB=Reduced Blanking>
Enumeration value matching to specified mode number.
*/
enum TFixedModeDmt
    {
    EDmtFixedModeNone = 0,

    //VESA (DMTv1r11.pdf)
    E640x350p85HzRB     = 1,
    E640x400p85HzRB     = 2,
    E720x400p85HzRB     = 3,
    E640x480p60HzRB     = 4,
    E640x480p72HzRB     = 5,
    E640x480p75HzRB     = 6,
    E640x480p85HzRB     = 7,
    E800x600p56HzRB     = 8,
    E800x600p60HzRB     = 9,
    E800x600p72HzRB     = 10,
    E800x600p75HzRB     = 11,
    E800x600p85HzRB     = 12,
    E800x600p120HzRB    = 13,
    E848x480p60HzRB     = 14,
    E1024x768i87HzRB    = 15,
    E1024x768p60HzRB    = 16,
    E1024x768p70HzRB    = 17,
    E1024x768p75HzRB    = 18,
    E1024x768p85HzRB    = 19,
    E1024x768p120HzRB   = 20,
    E1152x864p75HzRB    = 21,
    E1280x768p60HzRB    = 22,
    E1280x768p60Hz      = 23,
    E1280x768p75HzRB    = 24,
    E1280x768p85HzRB    = 25,
    E1280x768p120HzRB   = 26,
    E1280x800p60HzRB    = 27,
    E1280x800p60Hz      = 28,
    E1280x800p75HzRB    = 29,
    E1280x800p85HzRB    = 30,
    E1280x800p120HzRB   = 31,
    E1280x960p60HzRB    = 32,
    E1280x960p85HzRB    = 33,
    E1280x960p120HzRB   = 34,
    E1280x1024p60HzRB   = 35,
    E1280x1024p75HzRB   = 36,
    E1280x1024p85HzRB   = 37,
    E1280x1024p120HzRB  = 38,
    E1360x768p60HzRB    = 39,
    E1360x768p120HzRB   = 40,
    E1400x1050p60HzRB   = 41,
    E1400x1050p60Hz     = 42,
    E1400x1050p75HzRB   = 43,
    E1400x1050p85HzRB   = 44,
    E1400x1050p120HzRB  = 45,
    E1440x900p60HzRB    = 46,
    E1440x900p60Hz      = 47,
    E1440x900p75HzRB    = 48,
    E1440x900p85HzRB    = 49,
    E1440x900p120HzRB   = 50,
    E1600x1200p60HzRB   = 51,
    E1600x1200p65HzRB   = 52,
    E1600x1200p70HzRB   = 53,
    E1600x1200p75HzRB   = 54,
    E1600x1200p85HzRB   = 55,
    E1600x1200p120HzRB  = 56,
    E1680x1050p60HzRB   = 57,
    E1680x1050p60Hz     = 58,
    E1680x1050p75HzRB   = 59,
    E1680x1050p85HzRB   = 60,
    E1680x1050p120HzRB  = 61,
    E1792x1344p60HzRB   = 62,
    E1792x1344p75HzRB   = 63,
    E1792x1344p120HzRB  = 64,
    E1856x1392p60HzRB   = 65,
    E1856x1392p75HzRB   = 66,
    E1856x1392p120HzRB  = 67,
    E1920x1200p60HzRB   = 68,
    E1920x1200p60Hz     = 69,
    E1920x1200p75HzRB   = 70,
    E1920x1200p85HzRB   = 71,
    E1920x1200p120HzRB  = 72,
    E1920x1440p60HzRB   = 73,
    E1920x1440p75HzRB   = 74,
    E1920x1440p120HzRB  = 75,
    E2560x1600p60HzRB   = 76,
    E2560x1600p60Hz     = 77,
    E2560x1600p75HzRB   = 78,
    E2560x1600p85HzRB   = 79,
    E2560x1600p120HzRB  = 80,
    E1366x768p60HzRB    = 81,
    E1920x1080p60HzRB   = 82,

    ELastDmtFixedModeValue, //this must be last.
    };

/**
Color space coordinates. Fixed point is 10th bit.
Example: If iRed.iY=625, it should be divided by 1024 (2^10) for getting real value 0.61035...
*/
class TTvColorCoordinates
    {
    public:
        TTvColorCoordinates():iRed(),
                              iGreen(),
                              iBlue(),
                              iWhite()
                              {}
    ///Red chromaticity coordinates (X,Y)
    TPoint iRed;
    ///Green chromaticity coordinates
    TPoint iGreen;
    ///Blue chromaticity coordinates
    TPoint iBlue;
    ///White chromaticity coordinates
    TPoint iWhite;
    };

//Pixel repetation bit masks for TSupportedHdmiDviMode::iPixelRepeatBitMask
const TUint KPixelSentOnce    = 0;         //No repetation, pixel sent once
const TUint KPixelSent2Times  = ( 1 << 0 );
const TUint KPixelSent3Times  = ( 1 << 1 );
const TUint KPixelSent4Times  = ( 1 << 2 );
const TUint KPixelSent5Times  = ( 1 << 3 );
const TUint KPixelSent6Times  = ( 1 << 4 );
const TUint KPixelSent7Times  = ( 1 << 5 );
const TUint KPixelSent8Times  = ( 1 << 6 );
const TUint KPixelSent9Times  = ( 1 << 7 );
const TUint KPixelSent10Times = ( 1 << 8 );
/**
One mode struct for making capability query of own HW (phone capabilities).
*/
class TSupportedHdmiDviMode
    {
    public:
        enum TStandardModeType
            {
            ///CEA mode, TFixedModeCea
            ECea = 0x00,
            ///Dmt mode, TFixedModeDmt
            EDmt = 0x01,
            };
    public:
        TSupportedHdmiDviMode():iStandardMode(ECeaFixedModeNone),
                                iStandardModeType(ECea),
                                iHigherRefreshRateSupported(0),
                                iLowerRefreshRateSupported(0),
                                iPixelRepeatBitMask(KPixelSentOnce)
                                {}
    public:
        ///Standard CEA (TFixedModeCea) or DMT (TFixedModeDmt) mode number. @see iStandardModeType
        TUint8 iStandardMode;
        ///Type of TStandardModeType. ECea if iStandardMode is CEA mode (TFixedModeCea), EDmt when DMT mode (TFixedModeDmt).
        TUint8 iStandardModeType;
        ///@see iLowerRefreshRateSupported. If this is True (1), then higher frequency is supported.
        TUint8 iHigherRefreshRateSupported;
        /** If mode has two refresh rates. If is True (1), then lower frequency is supported.
            If mode has only one refresh rate, then this has to be True (1).
            E.g. CEA mode 1: 640x480p @59.94/60Hz, so this defines is 59.94Hz supported (lower) or not.
            Don't care if mode has only one refresh rate (e.g. CEA 17 720x576p @50Hz)
            iLowerRefreshRateSupported=1=Lower refresrate supported (e.g. 59.94Hz).
            iHigherRefreshRateSupported=1=Higher refreshrate supported (e.g. 60Hz) */
        TUint8 iLowerRefreshRateSupported;
        ///Bit mask of supported pixel repetation. 0=no repetation=KPixelSentOnce.
        TUint16 iPixelRepeatBitMask;
    };

/**
Struct for own TV HW capabilities query.
@see GetTvHwCapabilities
*/
class THwCapabilities
    {
    public:
        THwCapabilities():iHdcpSupportedInSource(EFalse),
                          iSourceHdmiVersion(0),
                          iSourceHdmiRevision(0),
                          iOverscanToBeDrawn(0),
                          iScalingMinDenominator(1),
                          iScalingMaxNumerator(1),
                          iCentralizingOnly(EFalse)
                          {}
    public:
        ///ETrue if source (own HW, phone) supports HDCP
        TBool iHdcpSupportedInSource;

        /** HDMI version what (own) HW supports. Version combination is iSourceHdmiVersion.iSourceHdmiRevision.
            If wanted connector is not HDMI, then these can be zero */
        TUint8 iSourceHdmiVersion;
        ///@see iSourceHdmiVersion
        TUint8 iSourceHdmiRevision;

        ///ETrue when composition should draw overscan (inc. position) to destination buffer
        TBool iOverscanToBeDrawn;
        ///Minimum scaling factor. E.g. if 1/2x is min, then this is 200 (100 / iScalingMinDenominator). 1x = 100. 0xFFFF = infinity
        TUint16 iScalingMinDenominator;
        ///Maximum scaling factor. E.g. if 2.5x is max, then this is 250 (iScalingMaxNumerator / 100). 1x = 100. 0xFFFF = infinity
        TUint16 iScalingMaxNumerator;

        ///ETrue when position of Tv-out cannot be change, image is always centralized. EFalse when position can be changed. @see iRightBottomCorner
        TBool iCentralizingOnly;
    };

/**
Config analogue TV-Out settings
*/
class TTvSettings
    {
    public:
       ///Television broadcast standard:
        enum TSystem
            {
            EUndefSystem = 0x0, // Initial value
            ENTSC        = 0x1, // 59.94Hz 525 NTSC-M  - others than Japan
                                // also 59.94Hz 480i/480p

            ENTSCJ       = 0x2, // 59.94Hz 525 NTSC-J  - Japan
                                // also 59.94Hz 480i/480p

            EPAL         = 0x3, // 50Hz    625 PAL-B/G - others than Brazil and Argentina
                                // also 50Hz 576i/576p

            EPALM        = 0x4, // 59.94Hz 525 PAL-M   - Brazil (use not recommended)
                                // also 59.94Hz 480i/480p

            EPALNc       = 0x5, // 50Hz    625 PAL-Nc  - Argentina (use not recommended)
                                // also 50Hz 576i/576p

            ESECAM       = 0x6, // 50 Hz   625 SECAM
                                // also 50Hz 576i/576p

            EHDTV720x50  = 0x7, // 50Hz 720p. @deprecated Do not use
            EHDTV720x60  = 0x8, // 60Hz 720p. @deprecated Do not use
            EHDTV1080x50 = 0x9, // 50Hz 1080i/1080p. @deprecated Do not use
            EHDTV1080x60 = 0xA, // 60Hz 1080i/1080p. @deprecated do not use
            };

        ///Television's aspect ration:
        enum TAspectRatio
            {
            EUndefRatio = 0x0,
            E4d3        = 0x1, // 4:3 not valid for 720p/1080i/1080p
            E16d9       = 0x2, // 16:9
            };

        ///Progressive Sequence:
        enum TScanMode
            {
            EUndefScan       = 0x0,
            EProgressiveScan = 0x1, // 480p/576p/720p/1080p
            EInterlacedScan  = 0x2, // 480i/576i/1080i
            };

        ///Connection type:
        enum TOutput
            {
            EUndefOutput     = 0x0,
            ECompositeVideo  = 0x1, // NTSCJ/NTSCM/PAL/PALM/PALNc/SECAM
            ESVideo          = 0x2, // NTSCJ/NTSCM/PAL/PALM/PALNc/SECAM
            EYPbPr,                 // 480i/576i/480p/576p
            EHDTVYPbPr,             // 720p/1080i/1080p (+480i/576i/480p/576p)
            EHDMI,
            EDVI,
            };

        ///Flicker filter:
        enum TFlickerFilterType
            {
            EUndefFlicker    = 0x0,
            EFilterEnabled   = 0x1,  // not valid for EProgressiveScan
            EFilterDisabled  = 0x2,
            };

        ///Cable detection:
        enum TDetectionType
            {
            EUndefAutoDetect = 0x0,
            EAutoDetectOn    = 0x1,
            EAutoDetectOff   = 0x2,
            };
    public:
        //Inline constuctor
        TTvSettings():iTvSystem(EUndefSystem),
                      iRatio(EUndefRatio),
                      iScanMode(EUndefScan),
                      iOutput(EUndefOutput),
                      iFlickerFilter(EUndefFlicker),
                      iAutoDetection(EUndefAutoDetect),
                      iPixelAspectRatioNumerator(1),
                      iPixelAspectRatioDenominator(1),
                      iHorizontalOverScan(000),//0.00%
                      iVerticalOverScan(000),  //0.00%
                      iCornerPointsUsed(EFalse),
                      iLeftTopCorner(),
                      iRightBottomCorner()
                      {}
    ///Class members, fill these:
    public:
        ///Set Tv-system here:
        TSystem iTvSystem;
        ///Set Tv aspect ratio here:
        TAspectRatio iRatio;
        ///Set scan mode:
        TScanMode iScanMode;
        ///Output type:
        TOutput iOutput;
        ///Set flicker filter on/off:
        TFlickerFilterType iFlickerFilter;
        ///Autodetection mode:
        TDetectionType iAutoDetection;

        ///Pixel aspect ratio (PAR) numerator. PAR=iPixelAspectRatioNumerator/iPixelAspectRatioDenominator.
        TUint8 iPixelAspectRatioNumerator;
        ///@see iPixelAspectRatioNumerator
        TUint8 iPixelAspectRatioDenominator;

        ///Overscan width, percentage (0 - +15%), 2 decimals. 1 means 0.01%. Deprecated (use iRightBottomCorner&iLeftTopCorner).
        TUint16 iHorizontalOverScan;
        ///Overscan high, see iHorizontalOverScan. Deprecated (use iRightBottomCorner&iLeftTopCorner).
        TUint16 iVerticalOverScan;

        ///ETrue if iLeftTopCorner&iRightBottomCorner are used instead of iHorizontalOverScan&iVerticalOverScan
        TBool iCornerPointsUsed;
        /** Overscan in pixels. Set Left Top corner point. Zero when Tv supports underscan (0=underscan enabled)
            This should be "point" to active image area. Cannot point out of active image area.
            Cannot across with iRightBottomCorner. And this allows overscan and position tuning. */
        TPoint iLeftTopCorner;
        /** Overscan in pixels. Set Right Bottom corner point (offset from Tv's left top corner).
            @see iLeftTopCorner
            Driver returns KErrExtensionNotSupported if driver does not support this settings.
            If driver supports centralizing only, then this has to be same as iLeftTopCorner. */
        TPoint iRightBottomCorner;
    };

///The maximum length for the product name on TMDS bus
const TUint KProductNameChars = 8;
///The maximum length for the product description on TMDS bus
const TUint KProductDescriptorsChars = 16;
/**
HDMI/DVI timings and settings. See detailed values of standard formats from OutputModes_X.xls
*/
class THdmiDviTimings
    {
    public:
        enum TSourceType
            {
            ESourceTypeUnknown    = 0x00,
            ESourceTypeDigitalSTB = 0x01,
            ESourceTypeDVD        = 0x02,
            ESourceTypeDVHS       = 0x03,
            ESourceTypeHddVideo   = 0x04,
            ESourceTypeDVC        = 0x05,
            ESourceTypeDSC        = 0x06,
            ESourceTypeVideoCD    = 0x07,
            ESourceTypeGame       = 0x08,
            ESourceTypePCGeneral  = 0x09,
            };

    public:
        THdmiDviTimings():iCeaMode(ECeaFixedModeNone),
                          iDmtMode(EDmtFixedModeNone),
                          iTvPhysicalImageWidthMm(0),
                          iTvPhysicalImageHeightMm(0),
                          iTvPhysicalImageAspectRatioNumerator(0),
                          iTvPhysicalImageAspectRatioDenominator(0),
                          iPixelClockKHz(0),
                          iHorizontalActivePixels(0),
                          iHorizontalBlankingPixels(0),
                          iVerticalActiveLines(0),
                          iVerticalBlankingLines(0),
                          iHorizontalSyncOffsetPixels(0),
                          iHorizontalSyncPulseWidthPixels(0),
                          iHorizontalBorderPixels(0),
                          iVerticalSyncOffsetLinesField1(00),//0.0
                          iVerticalSyncPulseWidthLinesField1(0),
                          iVerticalBorderLinesField1(0),
                          iVerticalSyncOffsetLinesField2(00),//0.0
                          iVerticalSyncPulseWidthLinesField2(0),
                          iVerticalBorderLinesField2(0),
                          iLeftBorderPixels(0),
                          iRightBorderPixels(0),
                          iInterlaced(EFalse),
                          iHorizontalSyncPolarity(EFalse),
                          iVerticalSyncPolarity(EFalse),
                          iPixelRepeat(0),//No repeat
                          iUnderscanEnabled(EFalse),
                          iLeftTopCorner(),
                          iRightBottomCorner(),
                          iImageAspectRatio(TTvSettings::EUndefRatio),
                          iPixelAspectRatioNumerator(1),
                          iPixelAspectRatioDenominator(1),
                          iConnector(TTvSettings::EUndefOutput),
                          iTvColorCoordinates(),
                          iTvHdmiVersion(0),
                          iTvHdmiRevision(0),
                          iProductName(),
                          iProductDescription(),
                          iSourceType(ESourceTypeUnknown)
                          {}
    public:
        /** Set CEA-mode. If none or DMT-mode is selected this should be ECeaFixedModeNone.
            Even modes are given to driver, all other datamembers must be valid.
            With CEA mode 1 this is E640x480p59_94d60Hz4d3 */
        TFixedModeCea iCeaMode;
        ///Set DMT-mode. If none or CEA-mode is selected this should be EDmtFixedModeNone. With CEA mode 1 this is EDmtFixedModeNone
        TFixedModeDmt iDmtMode;

        ///Tv physical size in millimetres. Can be 0 if iTvPhysicalImageAspectRatioNumerator is used.
        TUint iTvPhysicalImageWidthMm;
        ///@see iTvPhysicalImageWidthMm
        TUint iTvPhysicalImageHeightMm;
        /** Tv physical aspect ratio. Can be NULL if iTvPhysicalImageWidthMm is defined. Read from EDID.
            AR=iTvPhysicalImageAspectRatioNumerator/iTvPhysicalImageAspectRatioDenominator */
        TUint16 iTvPhysicalImageAspectRatioNumerator;
        ///@see iTvPhysicalImageAspectRatioNumerator
        TUint16 iTvPhysicalImageAspectRatioDenominator;

        ///Dot clock. With CEA mode 1 and 59.94Hz this is 25175KHz
        TUint iPixelClockKHz;

        ///Horizontal image width (resolution, columns). With CEA mode 1 this is 640
        TUint16 iHorizontalActivePixels;
        /** Horizontal blanking time in pixels. Total HTime=iHorizontalActivePixels+iHorizontalBlankingPixels. With CEA mode 1 this is 160
            Presented always as in CEA-model. */
        TUint16 iHorizontalBlankingPixels;
        ///Vertical image height (resolution, lines). With CEA mode 1 this is 480
        TUint16 iVerticalActiveLines;
        ///Vertical blanking time in lines. Total VTime=iVerticalActiveLines+iVerticalBlankingLines. With CEA mode 1 this is 45
        TUint16 iVerticalBlankingLines;

        ///Horizontal pulse frontporch in pixels. With CEA mode 1 this is 16
        TUint16 iHorizontalSyncOffsetPixels;
        ///Horizontal pulse duration in pixels. With CEA mode 1 this is 96
        TUint16 iHorizontalSyncPulseWidthPixels;
        ///Horizontal border (decreases addressable area). With CEA mode 1 this is 0.
        TUint16 iHorizontalBorderPixels;
        /** Vertical pulse frontporch in lines. Fixed point with 10x, 1=10. 1.5=15.
            With CEA mode 1 this is 100 (actual value = 10).
            Presented always as CEA-model handle porches. For example E1024x768i87HzRB this is 00 */
        TUint16 iVerticalSyncOffsetLinesField1;
        ///Vertical pulse duration in lines. With CEA mode 1 this is 2. With E1024x768i87HzRB this is 4.
        TUint16 iVerticalSyncPulseWidthLinesField1;
        ///Vertical border (decreases addressable area). With CEA mode 1 this is 0.
        TUint16 iVerticalBorderLinesField1;
        ///@see iVerticalSyncOffsetLinesField1. With progressive mode, this is unused. With E1024x768i87HzRB this is 10 (value 1).
        TUint16 iVerticalSyncOffsetLinesField2;
        ///@see iVerticalSyncPulseWidthLinesField1. With progressive mode, this is unused. With E1024x768i87HzRB this is 4.
        TUint16 iVerticalSyncPulseWidthLinesField2;
        ///@see iVerticalBorderLinesField1. With progressive mode, this is unused.
        TUint16 iVerticalBorderLinesField2;

        ///Left border (decreases addressable area). With CEA mode 1 this is 0.
        TUint16 iLeftBorderPixels;
        ///Right border (decreases addressable area). With CEA mode 1 this is 0.
        TUint16 iRightBorderPixels;

        ///ETrue=Interlaced scanning, EFalse=Progressive scanning. With CEA mode 1 this is EFalse.
        TBool iInterlaced;
        ///Horizontal sync polarity, EFalse=Negative, ETrue=Positive. With CEA mode 1 this is EFalse.
        TBool iHorizontalSyncPolarity;
        ///Vertical sync polarity, EFalse=Negative, ETrue=Positive. With CEA mode 1 this is EFalse.
        TBool iVerticalSyncPolarity;
        /** Wanted pixel repeat, can be used if mode supports. Otherwise horisontal resolution can be increased also.
            Value minus one. 0 -> sent once. 9 -> pixel sent 10 times.
            0 when repeat is disabled. With CEA mode 1 this is 0. */
        TUint8 iPixelRepeat;

        /** ETrue if Tv supports underscan, and it is enabled (to be always enabled when supported/possible),
            EFalse otherwise */
        TBool iUnderscanEnabled;
        /** Overscan in pixels. Set Left Top corner point. Zero when Tv supports underscan (0=underscan enabled)
            This should be "point" to active image area. Cannot point out of active image area.
            Cannot across with iRightBottomCorner. And this allows overscan and position tuning. */
        TPoint iLeftTopCorner;
        /** Overscan in pixels. Set Right Bottom corner point (offset from Tv's left top corner).
            @see iLeftTopCorner. Has not affect if iCentralizingOnly==ETrue. */
        TPoint iRightBottomCorner;

        ///Image aspect ratio (IAR). With CEA mode 1 this is E4d3.
        TTvSettings::TAspectRatio iImageAspectRatio;
        ///Pixel aspect ratio (PAR) numerator. PAR=iPixelAspectRatioNumerator/iPixelAspectRatioDenominator. With CEA mode 1 PAR=1/1.
        TUint8 iPixelAspectRatioNumerator;
        ///@see iPixelAspectRatioNumerator
        TUint8 iPixelAspectRatioDenominator;

        ///Tv connector type. E.g. if connected to Tv's HDMI connector, this is EHDMI.
        TTvSettings::TOutput iConnector;

        ///Tv color coordinates (read from EDID)
        TTvColorCoordinates iTvColorCoordinates;

        ///HDMI version what Tv supports. Version combination is iTvHdmiVersion.iTvHdmiRevision. Zeroes if DVI.
        TUint8 iTvHdmiVersion;
        ///@see iTvHdmiVersion
        TUint8 iTvHdmiRevision;

        /** Product name which to be sent to Tv in "Source Product Infoframe".
            Cell 0 contains first letter. All unused characters should be Null. */
        TChar iProductName[ KProductNameChars ];
        /** Product Description which to be sent to Tv in "Source Product Infoframe".
            Cell 0 contains first letter. All unused characters should be Null. */
        TChar iProductDescription[ KProductDescriptorsChars ];
        ///Source type which to be sent to Tv in "Source Product Infoframe". All unused characters should be Null.
        TSourceType iSourceType;
    };

/**
Standby Figure Format. Fully deprecated since TB10.2.
Note for TB9.2 and TB10.1: 
- iLeftTopColumn & iLeftTopRow are deprecated and those can be ignored.
- the icon of iColumns x iRows is scaled by (iScaleNumeratorWidth/iScaleDenominatorWidth) and (iScaleNumeratorHeight/iScaleDenominatorHeight)
- the resulting icon is centered to the layer's extent which contained the protected surface
- the scaled and centered icon is clipped against layer extent and screen size.
- pixels inside the layer's extent not covered by the scaled and centered icon are filled with iBackGroundColor.
*/
class TStandByFigure
    {
    public:
        enum TFigureType
            {
            ///Figure instead of UI
            EStandByFigure,
            ///Figure when copy protected material
            EProtectedContentFigure,//Keep latest
            };

    public:
        TStandByFigure():iTable(NULL),
                         iFigureType(EStandByFigure),
                         iColumns(0),
                         iRows(0),
                         iScaleNumeratorWidth(1),//1x
                         iScaleDenominatorWidth(1),
                         iScaleNumeratorHeight(1),//1x
                         iScaleDenominatorHeight(1),
                         iLeftTopColumn(0),
                         iLeftTopRow(0),
                         iBackGroundColor(0x0000)//Black
                         {}
    public:
        /** Input RGB565 RRRR RGGG GGGB BBBB LE.
            Set pointer to zero when TFigureType-figure is wanted to disable.
            Table's max size is StandByFigureMaxSizeInBytes() */
        TUint16* iTable;
        ///StandBy figure type
        TFigureType iFigureType;
        ///Describes how many columns figure has.
        TUint16 iColumns;
        ///Describes how many rows figure has.
        TUint16 iRows;
        ///Width scaler numerator (iScaleNumeratorWidth/iScaleDenominatorWidth)
        TUint16 iScaleNumeratorWidth;
        ///Width scaler denominator
        TUint16 iScaleDenominatorWidth;
        ///Height scaler numerator (iScaleNumeratorHeight/iScaleDenominatorHeight)
        TUint16 iScaleNumeratorHeight;
        ///Height scaler denominator
        TUint16 iScaleDenominatorHeight;
        ///Start column. Deprecated!
        TUint16 iLeftTopColumn;
        ///Start row. Deprecated!
        TUint16 iLeftTopRow;
        ///Back ground color. Input RGB565 RRRR RGGG GGGB BBBB.
        TUint16 iBackGroundColor;
    };

#endif //__TVOUTCONFIGDEF_H__

// End of File


