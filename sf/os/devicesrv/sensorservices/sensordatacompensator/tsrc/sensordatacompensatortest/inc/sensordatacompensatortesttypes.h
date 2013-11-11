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
* Description: 
*
*/



#ifndef SENSORDATACOMPENSATORTESTTYPES_H
#define SENSORDATACOMPENSATORTESTTYPES_H

/**
* Test Uid for simulating states like orientations, flip etc.
*/
const TUid KTestPSUid = {0x0AA01554};

/**
* Test states
*/
enum TTestPSIntKeys
    {
    ETestKeyPluginAction = 0, // See TTestPluginAction
    ETestPluginReturnErrorOnDataUpdate, // Sets error to be returned from plugin on update
    ETestInputDataExpectedError, // Sets error to be expected as Compensate() return value
    ETestOutputDataThreshold, // Sets threshold for output data checks
    ETestIntKeyCount
    };

const TInt KTestMaxDataSize = 512;

enum TTestPSDes8Keys
    {
    ETestKeyPluginDataAppend = ETestIntKeyCount,
    ETestDes8KeyCount
    };

/**
* Plugin actions
*/
enum TTestPluginAction // Used with ETestKeyPluginAction
    {
    ETestPluginResetData = 1, // Reset all internal data
    ETestPluginFailCreation // Fail next plugin creation attempt
    };

/**
* Test data streams
*/
enum TTestDataStreams
    {
    ETestInputDataItem = 1, // Sets input data for Compensate()
    ETestOutputDataItem, // Sets expected output data for Compensate()
    ETestPluginDataItem, // Sets compensate data from plugin
    ETestInputDataItemCrap, // Sets invalid sized input data
    ETestPluginDataItemCrap // Sets invalid sized compensate data from plugin
    };

#endif // SENSORDATACOMPENSATORTESTTYPES_H

// End of File
