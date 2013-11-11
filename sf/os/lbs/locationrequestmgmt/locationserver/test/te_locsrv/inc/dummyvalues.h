// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// TestDummyValues.h
// 
//

#ifndef __TESTDUMMYVALUES_H__
#define __TESTDUMMYVALUES_H__

/**
 * @file TestDummyValues.h
 *
 * This file includes all the dummy data used by the test server to
 * complete client requests. The file should be included by both the
 * test server and the unit test code to ensure that the API implementation
 * works.
 */

/**
    See MLFW_TC2_vX.X.doc, Test Environment configuration
    for constants defined for use with real server.

    The RealServer project is defined to work within TC2
*/

/** THE TEST SERVER USES VALUES OF PSY2 */

// message to say server number of current test case
#define ETestCurrentTCIDServerMessage       12345
#define ETestCurrentTCIDPositionerMessage   12321
// the stuff is used when test ids are sent to server side
#define KMaxTestCaseIdStringLength          100


#define ASYNC_REQUEST_COMPLETION_TIME_MILLISECS 200

#define DUMMY_NUM_MODULES     (TUint(3))

#define DUMMY_MODULE_BAD_ID   0xFFFF // not a recognised module ID

#define DEF_SIMULATION_PSY_ID  0x101F7A81

#ifdef _TEST_SERVER_
    // general info
    #define DUMMY_MODULE_ID1                             0x123
    #define DUMMY_MODULE_ID2                             0x456
    #define DUMMY_MODULE_ID3                             0x789
    #define DUMMY_MODULE_ID_DEF                          (DUMMY_MODULE_ID2)

    // registry info
    #define DUMMY_MODULE_NAME1                          _L("A-GPS")
    #define DUMMY_MODULE_INFO_IS_AVAILABLE1                                 (ETrue)
    #define DUMMY_MODULE_INFO_TECHNOLOGY_TYPE1           (TPositionModuleInfo::TTechnologyType(TPositionModuleInfo::ETechnologyUnknown))
    #define DUMMY_MODULE_INFO_DEVICE_LOCATION1           (TPositionModuleInfo::TDeviceLocation(TPositionModuleInfo::EDeviceInternal))
    #define DUMMY_MODULE_INFO_CAPABILITIES1                                 (TPositionModuleInfo::TCapabilities(TPositionModuleInfo::ECapabilityHorizontal))
    #define DUMMY_MODULE_INFO_CLASSES_SUPPORTED1         \
                    (static_cast<TPositionInfoClassType>(EPositionInfoClass|EPositionGenericInfoClass | EPositionCourseInfoClass | EPositionSatelliteInfoClass))
    #define DUMMY_MODULE_INFO_VERSION1                   (TVersion(1,0,58))

    #define DUMMY_MODULE_NAME2                          _L("TA")
    #define DUMMY_MODULE_INFO_IS_AVAILABLE2                                 (EFalse)
    #define DUMMY_MODULE_INFO_TECHNOLOGY_TYPE2           (TPositionModuleInfo::TTechnologyType(TPositionModuleInfo::ETechnologyTerminal))
    #define DUMMY_MODULE_INFO_DEVICE_LOCATION2           (TPositionModuleInfo::TDeviceLocation(TPositionModuleInfo::EDeviceExternal))
    #define DUMMY_MODULE_INFO_CAPABILITIES2                                 (TPositionModuleInfo::TCapabilities(TPositionModuleInfo::ECapabilitySpeed))
    #define DUMMY_MODULE_INFO_CLASSES_SUPPORTED2         \
                    (static_cast<TPositionInfoClassType>(EPositionInfoClass|EPositionGenericInfoClass))
    #define DUMMY_MODULE_INFO_VERSION2                   (TVersion(1,3,1))

    #define DUMMY_MODULE_NAME3                          _L("EO-TD")
    #define DUMMY_MODULE_INFO_IS_AVAILABLE3                                 (ETrue)
    #define DUMMY_MODULE_INFO_TECHNOLOGY_TYPE3           (TPositionModuleInfo::TTechnologyType(TPositionModuleInfo::ETechnologyNetwork))
    #define DUMMY_MODULE_INFO_DEVICE_LOCATION3           (TPositionModuleInfo::TDeviceLocation(TPositionModuleInfo::EDeviceUnknown))
    #define DUMMY_MODULE_INFO_CAPABILITIES3                                 (TPositionModuleInfo::TCapabilities(TPositionModuleInfo::ECapabilityCompass|TPositionModuleInfo::ECapabilityBuilding))
    #define DUMMY_MODULE_INFO_CLASSES_SUPPORTED3         \
                    (static_cast<TPositionInfoClassType>(EPositionInfoClass|EPositionGenericInfoClass | EPositionCourseInfoClass | EPositionSatelliteInfoClass))
    #define DUMMY_MODULE_INFO_VERSION3                   (TVersion(2,1,12))

#else
    // general info
    #define DUMMY_MODULE_ID1                            0x03133002    // LCFPSY1
    #define DUMMY_MODULE_ID2                            0x03133004    // LCFPSY2
    #define DUMMY_MODULE_ID3                            0x03133006    // LCFPSY3
    #define DUMMY_MODULE_NAME1                          _L("LCF PSY Dummy1")
    #define DUMMY_MODULE_NAME2                          _L("LCF PSY Dummy2")
    #define DUMMY_MODULE_NAME3                          _L("LCF PSY Dummy3")

    // registry info
    // 1
    #define DUMMY_MODULE_INFO_IS_AVAILABLE1                                (ETrue)
    #define DUMMY_MODULE_INFO_TECHNOLOGY_TYPE1          (TPositionModuleInfo::TTechnologyType(TPositionModuleInfo::ETechnologyTerminal))
    #define DUMMY_MODULE_INFO_DEVICE_LOCATION1          (TPositionModuleInfo::TDeviceLocation(TPositionModuleInfo::EDeviceInternal))
    #define DUMMY_MODULE_INFO_CAPABILITIES1                                (TPositionModuleInfo::TCapabilities( \
                                                            TPositionModuleInfo::ECapabilityHorizontal | \
                                                            TPositionModuleInfo::ECapabilityVertical | \
                                                            TPositionModuleInfo::ECapabilityAddress))
    #define DUMMY_MODULE_INFO_CLASSES_SUPPORTED1        (static_cast<TPositionInfoClassType>( \
                                                            EPositionInfoClass|EPositionGenericInfoClass))
    #define DUMMY_MODULE_INFO_VERSION1                  (TVersion(1,0,0))

    // 2
    #define DUMMY_MODULE_INFO_IS_AVAILABLE2                                (ETrue)
    #define DUMMY_MODULE_INFO_TECHNOLOGY_TYPE2          (TPositionModuleInfo::TTechnologyType(TPositionModuleInfo::ETechnologyTerminal))
    #define DUMMY_MODULE_INFO_DEVICE_LOCATION2          (TPositionModuleInfo::TDeviceLocation(TPositionModuleInfo::EDeviceInternal))
    #define DUMMY_MODULE_INFO_CAPABILITIES2                                (TPositionModuleInfo::TCapabilities( \
                                                            TPositionModuleInfo::ECapabilityHorizontal | \
                                                            TPositionModuleInfo::ECapabilityVertical | \
                                                            TPositionModuleInfo::ECapabilityAddress))
    #define KPositionClassLcfInfo                       0x0010
    #define DUMMY_MODULE_INFO_CLASSES_SUPPORTED2        (static_cast<TPositionInfoClassType>( \
                                                            EPositionInfoClass|EPositionGenericInfoClass| \
                                                            KPositionClassLcfInfo))
    #define DUMMY_MODULE_INFO_VERSION2                  (TVersion(1,0,0))

    // 3
    #define DUMMY_MODULE_INFO_IS_AVAILABLE3                                (ETrue)
    #define DUMMY_MODULE_INFO_TECHNOLOGY_TYPE3          (TPositionModuleInfo::TTechnologyType(TPositionModuleInfo::ETechnologyTerminal))
    #define DUMMY_MODULE_INFO_DEVICE_LOCATION3          (TPositionModuleInfo::TDeviceLocation(TPositionModuleInfo::EDeviceInternal))
    #define DUMMY_MODULE_INFO_CAPABILITIES3                                (TPositionModuleInfo::TCapabilities( \
                                                            TPositionModuleInfo::ECapabilityHorizontal | \
                                                            TPositionModuleInfo::ECapabilityVertical | \
                                                            TPositionModuleInfo::ECapabilityAddress | \
                                                            TPositionModuleInfo::ECapabilityDirection | \
                                                            TPositionModuleInfo::ECapabilitySatellite))
    #define DUMMY_MODULE_INFO_CLASSES_SUPPORTED3        (static_cast<TPositionInfoClassType>( \
                                                            EPositionInfoClass|EPositionGenericInfoClass| \
                                                            EPositionCourseInfoClass | EPositionSatelliteInfoClass))
    #define DUMMY_MODULE_INFO_VERSION3                  (TVersion(1,0,0))
#endif

// quality info (accordingly to LCFPSYs)
#define DUMMY_MODULE_INFO_TIME_TO_FIRST_FIX1        (TTimeIntervalMicroSeconds(4000000)) // 4 seconds
#define DUMMY_MODULE_INFO_TIME_TO_NEXT_FIX1         (TTimeIntervalMicroSeconds(4000000)) // 4 seconds
#define DUMMY_MODULE_INFO_HORIZONTAL_ACCURACY1      50.0                                  // static accuracy
#define DUMMY_MODULE_INFO_VERTICAL_ACCURACY1        60.0
#define DUMMY_MODULE_INFO_COST_INDICATOR1           (TPositionQuality::ECostCharge)
#define DUMMY_MODULE_INFO_POWER_CONSUMPTION1        (TPositionQuality::EPowerMedium)

#define DUMMY_MODULE_INFO_TIME_TO_FIRST_FIX2        (TTimeIntervalMicroSeconds(2000000)) // 2 sec
#define DUMMY_MODULE_INFO_TIME_TO_NEXT_FIX2         (TTimeIntervalMicroSeconds(2000000))
#define DUMMY_MODULE_INFO_HORIZONTAL_ACCURACY2      60.0
#define DUMMY_MODULE_INFO_VERTICAL_ACCURACY2        50.0
#define DUMMY_MODULE_INFO_COST_INDICATOR2           (TPositionQuality::ECostPossible)
#define DUMMY_MODULE_INFO_POWER_CONSUMPTION2        (TPositionQuality::EPowerHigh)

#define DUMMY_MODULE_INFO_TIME_TO_FIRST_FIX3        (TTimeIntervalMicroSeconds(5000000)) // 5sec
#define DUMMY_MODULE_INFO_TIME_TO_NEXT_FIX3         (TTimeIntervalMicroSeconds(5000000))
#define DUMMY_MODULE_INFO_HORIZONTAL_ACCURACY3      200.0
#define DUMMY_MODULE_INFO_VERTICAL_ACCURACY3        400.0
#define DUMMY_MODULE_INFO_COST_INDICATOR3           (TPositionQuality::ECostZero)
#define DUMMY_MODULE_INFO_POWER_CONSUMPTION3        (TPositionQuality::EPowerZero)

// test data
#define DUMMY_MODULE_STATUS1             (static_cast<TInt>(TPositionModuleStatus::EDeviceDisabled))
#define DUMMY_MODULE_DATA_QUAL_STATUS1   (static_cast<TInt>(TPositionModuleStatus::EDataQualityUnknown))
#define DUMMY_MODULE_STATUS2             (static_cast<TInt>(TPositionModuleStatus::EDeviceReady))
#define DUMMY_MODULE_DATA_QUAL_STATUS2   (static_cast<TInt>(TPositionModuleStatus::EDataQualityNormal))
#define DUMMY_MODULE_STATUS3             (static_cast<TInt>(TPositionModuleStatus::EDeviceStandBy))
#define DUMMY_MODULE_DATA_QUAL_STATUS3   (static_cast<TInt>(TPositionModuleStatus::EDataQualityPartial))

#define DUMMY_STATUS_EVENT_REQ_EVENTS         (static_cast<TUint32>(TPositionModuleStatusEvent::EEventAll))
#define DUMMY_STATUS_EVENT_OCC_EVENTS         (static_cast<TUint32>(TPositionModuleStatusEvent::EEventDeviceStatus))
#define DUMMY_STATUS_EVENT_DEV_STATUS         (static_cast<TInt>(TPositionModuleStatus::EDeviceInactive))
#define DUMMY_STATUS_EVENT_DEV_QUAL_STATUS    (static_cast<TInt>(TPositionModuleStatus::EDataQualityPartial))

#define DUMMY_REQUESTOR_TYPE_ACCEPTED1        (CRequestor::ERequestorService)
#define DUMMY_REQUESTOR_FORMAT_ACCEPTED1      (CRequestor::EFormatApplication)
#define DUMMY_REQUESTOR_NAME_ACCEPTED1        (_L("TestService"))

#define DUMMY_REQUESTOR_TYPE_ACCEPTED2        (CRequestor::ERequestorContact)
#define DUMMY_REQUESTOR_FORMAT_ACCEPTED2      (CRequestor::EFormatTelephone)
#define DUMMY_REQUESTOR_NAME_ACCEPTED2        (_L("+123121234567"))

#define DUMMY_REQUESTOR_TYPE_REJECTED1        (CRequestor::ERequestorContact)
#define DUMMY_REQUESTOR_FORMAT_REJECTED1      (CRequestor::EFormatUnknown)
#define DUMMY_REQUESTOR_NAME_REJECTED1        (_L("Mr. Stalker"))

#define DUMMY_REQUESTOR_TYPE_REJECTED2        (CRequestor::ERequestorUnknown)
#define DUMMY_REQUESTOR_FORMAT_REJECTED2      (CRequestor::EFormatTelephone)
#define DUMMY_REQUESTOR_NAME_REJECTED2        (_L("Mrs. Stalker"))

// NotifyPositionUpdate data
// LCFPSY1 Dummy
#define DUMMY_NOTIFY_POS_UPDATE_LATITUDE1               TReal64(57.1)
#define DUMMY_NOTIFY_POS_UPDATE_LONGITUDE1              TReal64(11.3)
#define DUMMY_NOTIFY_POS_UPDATE_ALTITUDE1               TReal32(32.5)
#define DUMMY_NOTIFY_POS_UPDATE_HORIZONTAL_ACCURACY1    TReal32(40.0)
#define DUMMY_NOTIFY_POS_UPDATE_VERTICAL_ACCURACY1      TReal32(40.0)

#define DUMMY_LAST_KNOWN_LATITUDE1                      DUMMY_NOTIFY_POS_UPDATE_LATITUDE1
#define DUMMY_LAST_KNOWN_LONGITUDE1                     DUMMY_NOTIFY_POS_UPDATE_LONGITUDE1
#define DUMMY_LAST_KNOWN_ALTITUDE1                      DUMMY_NOTIFY_POS_UPDATE_ALTITUDE1
#define DUMMY_LAST_KNOWN_HORIZONTAL_ACCURACY1           DUMMY_NOTIFY_POS_UPDATE_HORIZONTAL_ACCURACY1
#define DUMMY_LAST_KNOWN_VERTICAL_ACCURACY1             DUMMY_NOTIFY_POS_UPDATE_VERTICAL_ACCURACY1

// LCFPSY2 Dummy
#define DUMMY_NOTIFY_POS_UPDATE_LATITUDE2               TReal64(61.45)
#define DUMMY_NOTIFY_POS_UPDATE_LONGITUDE2              TReal64(23.86)
#define DUMMY_NOTIFY_POS_UPDATE_ALTITUDE2               TReal32(123.0)
#define DUMMY_NOTIFY_POS_UPDATE_HORIZONTAL_ACCURACY2    TReal32(30.0)
#define DUMMY_NOTIFY_POS_UPDATE_VERTICAL_ACCURACY2      TReal32(30.0)

#define DUMMY_LAST_KNOWN_LATITUDE2                      DUMMY_NOTIFY_POS_UPDATE_LATITUDE2
#define DUMMY_LAST_KNOWN_LONGITUDE2                     DUMMY_NOTIFY_POS_UPDATE_LONGITUDE2
#define DUMMY_LAST_KNOWN_ALTITUDE2                      DUMMY_NOTIFY_POS_UPDATE_ALTITUDE2
#define DUMMY_LAST_KNOWN_HORIZONTAL_ACCURACY2           DUMMY_NOTIFY_POS_UPDATE_HORIZONTAL_ACCURACY2
#define DUMMY_LAST_KNOWN_VERTICAL_ACCURACY2             DUMMY_NOTIFY_POS_UPDATE_VERTICAL_ACCURACY2

// LCFPSY3 Dummy
#define DUMMY_NOTIFY_POS_UPDATE_LATITUDE3               TReal64(67.567)
#define DUMMY_NOTIFY_POS_UPDATE_LONGITUDE3              TReal64(-12.34)
#define DUMMY_NOTIFY_POS_UPDATE_ALTITUDE3               TReal32(45.32)
#define DUMMY_NOTIFY_POS_UPDATE_HORIZONTAL_ACCURACY3    TReal32(150.0)
#define DUMMY_NOTIFY_POS_UPDATE_VERTICAL_ACCURACY3      TReal32(500.0)

#define DUMMY_LAST_KNOWN_LATITUDE3                      DUMMY_NOTIFY_POS_UPDATE_LATITUDE3
#define DUMMY_LAST_KNOWN_LONGITUDE3                     DUMMY_NOTIFY_POS_UPDATE_LONGITUDE3
#define DUMMY_LAST_KNOWN_ALTITUDE3                      DUMMY_NOTIFY_POS_UPDATE_ALTITUDE3
#define DUMMY_LAST_KNOWN_HORIZONTAL_ACCURACY3           DUMMY_NOTIFY_POS_UPDATE_HORIZONTAL_ACCURACY3
#define DUMMY_LAST_KNOWN_VERTICAL_ACCURACY3             DUMMY_NOTIFY_POS_UPDATE_VERTICAL_ACCURACY3

// course info (LCFPSY3)
#define DUMMY_NOTIFY_POS_UPDATE_SPEED                   TReal32(26.66)
#define DUMMY_NOTIFY_POS_UPDATE_HEADING                 TReal32(25.0)
#define DUMMY_NOTIFY_POS_UPDATE_SPEED_ACCURACY          TReal32(2.0)
#define DUMMY_NOTIFY_POS_UPDATE_HEADING_ACCURACY        TReal32(10.0)

// satellite info (LCFPSY3)
#define DUMMY_PSY3_SATELLITE_NUM                        2

#define DUMMY_NOTIFY_POS_UPDATE_SATELLITE_ID1           10
#define DUMMY_NOTIFY_POS_UPDATE_AZIMUTH1                TReal32(50.0)
#define DUMMY_NOTIFY_POS_UPDATE_ELEVATION1              TReal32(27.0)
#define DUMMY_NOTIFY_POS_UPDATE_IS_USED1                ETrue
#define DUMMY_NOTIFY_POS_UPDATE_SIGNAL_STRENGTH1        4

#define DUMMY_NOTIFY_POS_UPDATE_SATELLITE_ID2           11
#define DUMMY_NOTIFY_POS_UPDATE_AZIMUTH2                TReal32(47.0)
#define DUMMY_NOTIFY_POS_UPDATE_ELEVATION2              TReal32(30.0)
#define DUMMY_NOTIFY_POS_UPDATE_IS_USED2                EFalse
#define DUMMY_NOTIFY_POS_UPDATE_SIGNAL_STRENGTH2        3

#define DUMMY_NOTIFY_POS_UPDATE_SAT_HDOP                TReal32(0.1)
#define DUMMY_NOTIFY_POS_UPDATE_SAT_VDOP                TReal32(0.2)
#define DUMMY_NOTIFY_POS_UPDATE_SAT_TDOP                TReal32(0.3)
#define DUMMY_NOTIFY_POS_UPDATE_SAT_TIME                TTime(_L("20030315:115959.112233"))

// generic info fields
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID1               1
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA1             TInt8(112)
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID2               2
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA2             TInt16(16020)
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID3               3
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA3             TInt32(1195771223)
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID4               4
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA4             TInt64(0x0020000, 0x00223345)
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID5               5
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA5             TUint8(182)
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID6               6
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA6             TUint16(17233)
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID7               7
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA7             TUint32(0xbe74e297)
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID8               8
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA8             TReal32(3.14159)
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID9               9
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA9             TReal64(12.5663706)
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID10              10
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA10            _L8("Hello world")
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID11              11
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA11            _L16("Unicode string")
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID12              12
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA12            TTime(_L("20030114:133956.102030"))
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID13              13
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA13            TTimeIntervalMicroSeconds(TInt64(0x01233218, 0x99212007))
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID14              14
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA14            _L16("Not requested")
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID15              15
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA15            _L16("Not requested")

#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID16              16
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA16            TInt8(33)
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID17              17
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA17            TInt8(44)
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID18              18
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA18            TInt8(55)
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID19              19
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA19            TInt8(66)

// for LCFPSY1
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_STREET_ID         EPositionFieldStreet
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_STREET_DATA       _L16("Tieteenkatu 1")//KTieteenkatu1
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_CITY_ID           EPositionFieldCity
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_CITY_DATA         _L16("Tampere")
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_COUNTRY_ID        EPositionFieldCountry
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_COUNTRY_DATA      _L16("Finland")

#endif // __TESTDUMMYVALUES_H__
