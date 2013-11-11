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

#define ASYNC_REQUEST_COMPLETION_TIME_MILLISECS 200

#define DUMMY_NUM_MODULES     (static_cast<TUint>(3))

#define DUMMY_MODULE_ID1      0x123
#define DUMMY_MODULE_ID2      0x456
#define DUMMY_MODULE_ID3      0x789
#define DUMMY_MODULE_BAD_ID   1001 // not a recognised module ID
#define DUMMY_MODULE_ID_DEF   (DUMMY_MODULE_ID2)

#define DUMMY_MODULE_STATUS1             (static_cast<TInt>(TPositionModuleStatus::EDeviceDisabled))
#define DUMMY_MODULE_DATA_QUAL_STATUS1   (static_cast<TInt>(TPositionModuleStatus::EDataQualityUnknown))
#define DUMMY_MODULE_STATUS2             (static_cast<TInt>(TPositionModuleStatus::EDeviceReady))
#define DUMMY_MODULE_DATA_QUAL_STATUS2   (static_cast<TInt>(TPositionModuleStatus::EDataQualityNormal))
#define DUMMY_MODULE_STATUS3             (static_cast<TInt>(TPositionModuleStatus::EDeviceStandBy))
#define DUMMY_MODULE_DATA_QUAL_STATUS3   (static_cast<TInt>(TPositionModuleStatus::EDataQualityPartial))

#define DUMMY_MODULE_INFO_IS_AVAILABLE1				 (ETrue)
#define DUMMY_MODULE_INFO_TECHNOLOGY_TYPE1           (TPositionModuleInfo::TTechnologyType(TPositionModuleInfo::ETechnologyUnknown))
#define DUMMY_MODULE_INFO_DEVICE_LOCATION1           (TPositionModuleInfo::TDeviceLocation(TPositionModuleInfo::EDeviceInternal))
#define DUMMY_MODULE_INFO_CAPABILITIES1				 (TPositionModuleInfo::TCapabilities(TPositionModuleInfo::ECapabilityHorizontal))
#define DUMMY_MODULE_INFO_CLASSES_SUPPORTED1         \
		(static_cast<TPositionInfoClassType>(EPositionGenericInfoClass | EPositionCourseInfoClass | EPositionSatelliteInfoClass))
#define DUMMY_MODULE_INFO_VERSION1                   (TVersion(1,0,58))
#define DUMMY_MODULE_INFO_TIME_TO_FIRST_FIX1         (TTimeIntervalMicroSeconds(0x8000000))
#define DUMMY_MODULE_INFO_TIME_TO_NEXT_FIX1          (TTimeIntervalMicroSeconds(0x1000000))
#define DUMMY_MODULE_INFO_HORIZONTAL_ACCURACY1       10.0f
#define DUMMY_MODULE_INFO_VERTICAL_ACCURACY1         10.0f
#define DUMMY_MODULE_INFO_COST_INDICATOR1            (TPositionQuality::ECostZero)
#define DUMMY_MODULE_INFO_POWER_CONSUMPTION1         (TPositionQuality::EPowerZero)
#define DUMMY_MODULE_INFO_MOD_NAME1                  _L("A-GPS")

#define DUMMY_MODULE_INFO_IS_AVAILABLE2				 (EFalse)
#define DUMMY_MODULE_INFO_TECHNOLOGY_TYPE2           (TPositionModuleInfo::TTechnologyType(TPositionModuleInfo::ETechnologyTerminal))
#define DUMMY_MODULE_INFO_DEVICE_LOCATION2           (TPositionModuleInfo::TDeviceLocation(TPositionModuleInfo::EDeviceExternal))
#define DUMMY_MODULE_INFO_CAPABILITIES2				 (TPositionModuleInfo::TCapabilities(TPositionModuleInfo::ECapabilitySpeed))
#define DUMMY_MODULE_INFO_CLASSES_SUPPORTED2         \
		(static_cast<TPositionInfoClassType>(EPositionGenericInfoClass))
#define DUMMY_MODULE_INFO_VERSION2                   (TVersion(1,3,1))
#define DUMMY_MODULE_INFO_TIME_TO_FIRST_FIX2         (TTimeIntervalMicroSeconds(0x5000000))
#define DUMMY_MODULE_INFO_TIME_TO_NEXT_FIX2          (TTimeIntervalMicroSeconds(0x5000000))
#define DUMMY_MODULE_INFO_HORIZONTAL_ACCURACY2       100.0f
#define DUMMY_MODULE_INFO_VERTICAL_ACCURACY2         500.0f
#define DUMMY_MODULE_INFO_COST_INDICATOR2            (TPositionQuality::ECostPossible)
#define DUMMY_MODULE_INFO_POWER_CONSUMPTION2         (TPositionQuality::EPowerHigh)
#define DUMMY_MODULE_INFO_MOD_NAME2                  _L("TA")

#define DUMMY_MODULE_INFO_IS_AVAILABLE3				 (ETrue)
#define DUMMY_MODULE_INFO_TECHNOLOGY_TYPE3           (TPositionModuleInfo::TTechnologyType(TPositionModuleInfo::ETechnologyNetwork))
#define DUMMY_MODULE_INFO_DEVICE_LOCATION3           (TPositionModuleInfo::TDeviceLocation(TPositionModuleInfo::EDeviceUnknown))
#define DUMMY_MODULE_INFO_CAPABILITIES3				 (TPositionModuleInfo::TCapabilities(TPositionModuleInfo::ECapabilityCompass|TPositionModuleInfo::ECapabilityBuilding))
#define DUMMY_MODULE_INFO_CLASSES_SUPPORTED3         \
		(static_cast<TPositionInfoClassType>(EPositionGenericInfoClass))
#define DUMMY_MODULE_INFO_VERSION3                   (TVersion(2,1,12))
#define DUMMY_MODULE_INFO_TIME_TO_FIRST_FIX3         (TTimeIntervalMicroSeconds(0x12000000))
#define DUMMY_MODULE_INFO_TIME_TO_NEXT_FIX3          (TTimeIntervalMicroSeconds(0x10000000))
#define DUMMY_MODULE_INFO_HORIZONTAL_ACCURACY3       500.0f
#define DUMMY_MODULE_INFO_VERTICAL_ACCURACY3         500.0f
#define DUMMY_MODULE_INFO_COST_INDICATOR3            (TPositionQuality::ECostCharge)
#define DUMMY_MODULE_INFO_POWER_CONSUMPTION3         (TPositionQuality::EPowerMedium)
#define DUMMY_MODULE_INFO_MOD_NAME3                  _L("EO-TD")

#define DUMMY_STATUS_EVENT_REQ_EVENTS         (static_cast<TUint32>(TPositionModuleStatusEvent::EEventAll))
#define DUMMY_STATUS_EVENT_OCC_EVENTS         (static_cast<TUint32>(TPositionModuleStatusEvent::EEventDeviceStatus))
#define DUMMY_STATUS_EVENT_DEV_STATUS         (static_cast<TInt>(TPositionModuleStatus::EDeviceInactive))
#define DUMMY_STATUS_EVENT_DEV_QUAL_STATUS    (static_cast<TInt>(TPositionModuleStatus::EDataQualityPartial))

#define DUMMY_REQUESTOR_TYPE_ACCEPTED1        (CRequestor::ERequestorService)
#define DUMMY_REQUESTOR_FORMAT_ACCEPTED1      (CRequestor::EFormatApplication)
#define DUMMY_REQUESTOR_NAME_ACCEPTED1        (_L("Tom Tom"))

#define DUMMY_REQUESTOR_TYPE_ACCEPTED2        (CRequestor::ERequestorContact)
#define DUMMY_REQUESTOR_FORMAT_ACCEPTED2      (CRequestor::EFormatTelephone)
#define DUMMY_REQUESTOR_NAME_ACCEPTED2        (_L("01223719800"))

#define DUMMY_REQUESTOR_TYPE_REJECTED1        (CRequestor::ERequestorContact)
#define DUMMY_REQUESTOR_FORMAT_REJECTED1      (CRequestor::EFormatUnknown)
#define DUMMY_REQUESTOR_NAME_REJECTED1        (_L("Mr. Stalker"))

#define DUMMY_LAST_KNOWN_LATITUDE            52.4
#define DUMMY_LAST_KNOWN_LONGITUDE            0.2
#define DUMMY_LAST_KNOWN_ALTITUDE            80.0
#define DUMMY_LAST_KNOWN_HORIZONTAL_ACCURACY  2.0
#define DUMMY_LAST_KNOWN_VERTICAL_ACCURACY    3.0

#define DUMMY_NOTIFY_POS_UPDATE_LATITUDE            49.2
#define DUMMY_NOTIFY_POS_UPDATE_LONGITUDE            3.5
#define DUMMY_NOTIFY_POS_UPDATE_ALTITUDE            50.0
#define DUMMY_NOTIFY_POS_UPDATE_HORIZONTAL_ACCURACY  2.0
#define DUMMY_NOTIFY_POS_UPDATE_VERTICAL_ACCURACY    3.0

#define DUMMY_NOTIFY_POS_UPDATE_SPEED             26.66f
#define DUMMY_NOTIFY_POS_UPDATE_HEADING           25.0
#define DUMMY_NOTIFY_POS_UPDATE_SPEED_ACCURACY    2.0
#define DUMMY_NOTIFY_POS_UPDATE_HEADING_ACCURACY  10.0

#define DUMMY_NOTIFY_POS_UPDATE_SATELLITE_ID1      10
#define DUMMY_NOTIFY_POS_UPDATE_AZIMUTH1           50.0
#define DUMMY_NOTIFY_POS_UPDATE_ELEVATION1         27.0
#define DUMMY_NOTIFY_POS_UPDATE_IS_USED1           ETrue
#define DUMMY_NOTIFY_POS_UPDATE_SIGNAL_STRENGTH1   4

#define DUMMY_NOTIFY_POS_UPDATE_SATELLITE_ID2      11
#define DUMMY_NOTIFY_POS_UPDATE_AZIMUTH2           47.0
#define DUMMY_NOTIFY_POS_UPDATE_ELEVATION2         30.0
#define DUMMY_NOTIFY_POS_UPDATE_IS_USED2           EFalse
#define DUMMY_NOTIFY_POS_UPDATE_SIGNAL_STRENGTH2   3

#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID1        1
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA1      TInt8(112)
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID2        2
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA2      TInt16(16020)
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID3        3
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA3      TInt32(1195771223)
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID4        4
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA4      TInt64(0x002000000223345)
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID5        5
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA5      TUint8(182)
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID6        6
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA6      TUint16(17233)
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID7        7
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA7      TUint32(0xbe74e297)
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID8        8
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA8      TReal32(3.14159)
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID9        9
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA9      TReal64(12.5663706)
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID10       10
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA10     _L8("Hello world")
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID11       11
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA11     _L16("Unicode string")
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID12       12
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA12     TTime(_L("20030114:133956.102030"))
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID13       13
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA13     TTimeIntervalMicroSeconds(TInt64(0x0123321899212007))
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID14       14
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA14     _L16("Not requested")
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID15       15
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA15     _L16("Not requested")

#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID16       16
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA16     TInt8(33)
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID17       17
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA17     TInt8(44)
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID18       18
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA18     TInt8(55)
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_ID19       19
#define DUMMY_NOTIFY_POS_UPDATE_FIELD_DATA19     TInt8(66)

#endif // __TESTDUMMYVALUES_H__