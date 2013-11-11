// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example CTestStep derived implementation
//



/**
 @file cbtpsypropertywatch.cpp
 @internalTechnology
*/

#include "cbtpsypropertywatch.h"

/**
Constructor
 */
CBTPSYPropertyWatchArray::CBTPSYPropertyWatchArray()
	{
	}

/**
Adds the active object to the active scheduler and creates a handle to the integer property
@param aConsole The console interface
 */
void CBTPSYPropertyWatchArray::ConstructL()
	{
	iPropertyWatchArray = new (ELeave) CArrayPtrFlat<CBTPSYPropertyWatch>(KPropertyWatcherNumber);
	iPropertyWatchArray->AppendL(CBtGpsPsyBatteryStateWatch::NewL());
	iPropertyWatchArray->AppendL(CBtGpsPsyBatteryLevelWatch::NewL());
	iPropertyWatchArray->AppendL(CBtGpsPsyExtAntennaWatch::NewL());
	iPropertyWatchArray->AppendL(CBtGpsPsyExtPowerWatch::NewL());
	iPropertyWatchArray->AppendL(CBtGpsPsyStateWatch::NewL());
	}

/**
Constructs a CBTPSYPropertyWatchArray object
@param aConsole The console interface
@return A CBTPSYPropertyWatchArray object
 */
CBTPSYPropertyWatchArray* CBTPSYPropertyWatchArray::NewL()
	{
	CBTPSYPropertyWatchArray* self = new (ELeave) CBTPSYPropertyWatchArray;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CBTPSYPropertyWatchArray* CBTPSYPropertyWatchArray::NewLC()
	{
	CBTPSYPropertyWatchArray* self = new (ELeave) CBTPSYPropertyWatchArray;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
Destructor
 */
CBTPSYPropertyWatchArray::~CBTPSYPropertyWatchArray()
	{
	if(iPropertyWatchArray)
		{
		iPropertyWatchArray->ResetAndDestroy();
		}
	}

void CBTPSYPropertyWatchArray::WatchL()
	{
	for(TInt index = 0; index<(iPropertyWatchArray->Count()); ++index)
		{
		(iPropertyWatchArray->At(index))->WatchL();
		User::After(K1Second);
		}
	}






/**
Constructor
 */
CBTPSYPropertyWatch::CBTPSYPropertyWatch():CActive(EPriority)
    			{
    			}

/**
Adds the active object to the active scheduler and creates a handle to the integer property
@param aConsole The console interface
 */
void CBTPSYPropertyWatch::ConstructL()
	{
	User::LeaveIfError(iProperty.Attach(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBatteryLevel));
	CActiveScheduler::Add(this);
	}

/**
Constructs a CBTPSYPropertyWatch object
@param aConsole The console interface
@return A CBTPSYPropertyWatch object
 */
CBTPSYPropertyWatch* CBTPSYPropertyWatch::NewL()
	{
	CBTPSYPropertyWatch* self = new (ELeave) CBTPSYPropertyWatch;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor
 */
CBTPSYPropertyWatch::~CBTPSYPropertyWatch()
	{
	iProperty.Close();
	Cancel();
	}

/**
Cancels any outstanding request
 */
void CBTPSYPropertyWatch::DoCancel()
	{
	iProperty.Cancel();
	}

/**
Handles the subscription to the property
Subscribes to the property as long as there is no change in the value of the property
 */
void CBTPSYPropertyWatch::RunL()
	{
	// Receive a notification when a value is published
	TInt err = CheckProperty();
	if (err == KErrNotFound)
		{
		// No need to re-subscribe to the property as it has been deleted by the publisher
		CActiveScheduler::Stop();
		}
	else if (err == KErrNone)
		{
		// Print the value of the property
		PrintPropertyL();
		// Re-subscribe to the property
		IssueRequest();
		}
	else
		{
		// Leave the function in case of any other error
		CActiveScheduler::Stop();
		User::Leave(err);
		}
	}

/**
Gets the value of the property if it is already defined
It also issues an outstanding request by re-subscribing to the property
 */
void CBTPSYPropertyWatch::WatchL()
	{
	// Checks if the property exists
	TInt res = CheckProperty();
	if (res == KErrNotFound)
		{
		// Property not defined
		User::Leave(res);
		}
	// Prints the value of the property
	PrintPropertyL();
	IssueRequest();
	}

/**
Issues an outstanding request by subscribing to the property
 */
void CBTPSYPropertyWatch::IssueRequest()
	{
	iProperty.Subscribe(iStatus);
	SetActive();
	}


/**
Prints a property value
@param aValue The changed value of the property
 */
TInt CBTPSYPropertyWatch::CheckProperty()
	{
	return -KErrArgument;
	}

/**
Prints a property value
@param aValue The changed value of the property
 */
void CBTPSYPropertyWatch::PrintPropertyL()
	{
	TInt value = 0;
	_LIT(KTxtValChange,"Value of the property : % d");
	TBuf<KMaxLogBuffer> buf;
	buf.Format(KTxtValChange, value);
	PrintPropertyForThreadL(buf);
	User::InfoPrint(buf);
	}

void CBTPSYPropertyWatch::PrintPropertyForThreadL( const TDesC& aText)
	{
	PrintPropertyForThreadL(aText, KThreadLogFile);
	}

void CBTPSYPropertyWatch::PrintPropertyForThreadL( const TDesC& aText, const TDesC& aFile )
    {
    TBuf<KMaxLogBuffer> message;
    TInt err = KErrNone;
    
    // Open connection to file server
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    
    // Open log file
    RFile file;
    CleanupClosePushL(file);
    err = file.Open(fs,aFile,EFileStreamText|EFileWrite);
    
    if (err != KErrNone)
        {
        if (err == KErrNotFound)
            {
            err = file.Create(fs,aFile,EFileStreamText|EFileWrite);
            TBuf8<2> unicodeByteOrderMark(KNullDesC8);
            unicodeByteOrderMark.Append(0xFF);
            unicodeByteOrderMark.Append(0xFE);
            file.Write(unicodeByteOrderMark);
            
            if (err != KErrNone)
                User::Leave(err);
            }
        else
            {
            User::Leave(err);
            }
        }
    
    // Write all collected time intervals to log file
    
    TFileText fileText;
    fileText.Set(file);
    fileText.Write( aText );
    
    CleanupStack::PopAndDestroy(2, &fs); //fs, file
    }

/**
Constructor
 */
CBtGpsPsyBatteryStateWatch::CBtGpsPsyBatteryStateWatch()
	{
	}

/**
Adds the active object to the active scheduler and creates a handle to the integer property
@param aConsole The console interface
 */
void CBtGpsPsyBatteryStateWatch::ConstructL()
	{
	User::LeaveIfError(iProperty.Attach(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBatteryState));
	CActiveScheduler::Add(this);
	}

/**
Constructs a CBtGpsPsyBatteryStateWatch object
@param aConsole The console interface
@return A CBtGpsPsyBatteryStateWatch object
 */
CBtGpsPsyBatteryStateWatch* CBtGpsPsyBatteryStateWatch::NewL()
	{
	CBtGpsPsyBatteryStateWatch* self = new (ELeave) CBtGpsPsyBatteryStateWatch;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor
 */
CBtGpsPsyBatteryStateWatch::~CBtGpsPsyBatteryStateWatch()
	{
	iProperty.Close();
	Cancel();
	}

/**
Prints a property value
@param aValue The changed value of the property
 */
TInt CBtGpsPsyBatteryStateWatch::CheckProperty()
	{
	TInt err = KErrNone;
	TInt value = KErrNotFound;
	err = iProperty.Get(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBatteryState, value);
	if(err == KErrNone)
		{
		if((value < EBatteryStateNA) && (value > EBatteryStateFull))
			{
			err = KErrArgument;
			}
		}
	return err;
	}

/**
Prints a property value
@param aValue The changed value of the property
 */
void CBtGpsPsyBatteryStateWatch::PrintPropertyL()
	{
	TInt value = KErrNotFound;
	iProperty.Get(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBatteryState, value);

	switch(value)
		{
		case EBatteryStateNA:
			_LIT(KTxtBatteryStateNotAvailable, "Battery state not available");
			User::InfoPrint(KTxtBatteryStateNotAvailable);
			PrintPropertyForThreadL(KTxtBatteryStateNotAvailable);
			break; 
		case EBatteryStateNormal:
			_LIT(KTxtBatteryStateNormal, "Battery state normal");
			User::InfoPrint(KTxtBatteryStateNormal);
			PrintPropertyForThreadL(KTxtBatteryStateNormal);
			break;
		case EBatteryStateLow:
			_LIT(KTxtBatteryLow, "Battery low");
			User::InfoPrint(KTxtBatteryLow);
			PrintPropertyForThreadL(KTxtBatteryLow);
			break;
		case EBatteryStateFull:
			_LIT(KTxtBatteryFull, "Battery full");
			User::InfoPrint(KTxtBatteryFull);
			PrintPropertyForThreadL(KTxtBatteryFull);
			break;
		}
	}

/**
Constructor
 */
CBtGpsPsyBatteryLevelWatch::CBtGpsPsyBatteryLevelWatch()
	{
	}

/**
Adds the active object to the active scheduler and creates a handle to the integer property
@param aConsole The console interface
 */
void CBtGpsPsyBatteryLevelWatch::ConstructL()
	{
	User::LeaveIfError(iProperty.Attach(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBatteryLevel));
	CActiveScheduler::Add(this);
	}

/**
Constructs a CBtGpsPsyBatteryLevelWatch object
@param aConsole The console interface
@return A CBtGpsPsyBatteryLevelWatch object
 */
CBtGpsPsyBatteryLevelWatch* CBtGpsPsyBatteryLevelWatch::NewL()
	{
	CBtGpsPsyBatteryLevelWatch* self = new (ELeave) CBtGpsPsyBatteryLevelWatch;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor
 */
CBtGpsPsyBatteryLevelWatch::~CBtGpsPsyBatteryLevelWatch()
	{
	iProperty.Close();
	Cancel();
	}

/**
Prints a property value
@param aValue The changed value of the property
 */
TInt CBtGpsPsyBatteryLevelWatch::CheckProperty()
	{
	TInt err = KErrNone;
	TInt value = KErrNotFound;
	err = iProperty.Get(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBatteryLevel, value);
	if(err == KErrNone)
		{
		if((value < 0) && (value > KHundredPercent))
			{
			err = KErrArgument;
			}
		}
	return err;
	}

/**
Prints a property value
@param aValue The changed value of the property
 */
void CBtGpsPsyBatteryLevelWatch::PrintPropertyL()
	{
	TInt value = KErrNotFound;
	iProperty.Get(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBatteryLevel, value);
	_LIT(KTxtBatteryLevel,"Battery Level: %d");
	TBuf<KMaxLogBuffer> buf;
	buf.Format(KTxtBatteryLevel, value);
	PrintPropertyForThreadL(buf);
	User::InfoPrint(buf);
	}

/**
Constructor
 */
CBtGpsPsyExtAntennaWatch::CBtGpsPsyExtAntennaWatch()
	{
	}

/**
Adds the active object to the active scheduler and creates a handle to the integer property
@param aConsole The console interface
 */
void CBtGpsPsyExtAntennaWatch::ConstructL()
	{
	User::LeaveIfError(iProperty.Attach(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyExtAntennaState));
	CActiveScheduler::Add(this);
	}

/**
Constructs a CBtGpsPsyExtAntennaWatch object
@param aConsole The console interface
@return A CBtGpsPsyExtAntennaWatch object
 */
CBtGpsPsyExtAntennaWatch* CBtGpsPsyExtAntennaWatch::NewL()
	{
	CBtGpsPsyExtAntennaWatch* self = new (ELeave) CBtGpsPsyExtAntennaWatch;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor
 */
CBtGpsPsyExtAntennaWatch::~CBtGpsPsyExtAntennaWatch()
	{
	iProperty.Close();
	Cancel();
	}

/**
Prints a property value
@param aValue The changed value of the property
 */
TInt CBtGpsPsyExtAntennaWatch::CheckProperty()
	{
	TInt err = KErrNone;
	TInt value = KErrNotFound;
	err = iProperty.Get(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyExtAntennaState, value);
	if(err == KErrNone)
		{
		if((value < EExtAntennaNA) && (value > EExtAntennaOn))
			{
			err = KErrArgument;
			}
		}
	return err;
	}

/**
Prints a property value
@param aValue The changed value of the property
 */
void CBtGpsPsyExtAntennaWatch::PrintPropertyL()
	{
	TInt value = KErrNotFound;
	iProperty.Get(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyExtAntennaState, value);

	switch(value)
		{
		case EExtAntennaNA:
			_LIT(KTxtExternalAntennaNotApplicable, "External antenna not applicable");
			User::InfoPrint(KTxtExternalAntennaNotApplicable);
			PrintPropertyForThreadL(KTxtExternalAntennaNotApplicable);
			break; 
		case EExtAntennaOff:
			_LIT(KTxtExternalAntennaOff, "External antenna off");
			User::InfoPrint(KTxtExternalAntennaOff);
			PrintPropertyForThreadL(KTxtExternalAntennaOff);
			break;
		case EExtAntennaOn:
			_LIT(KTxtExternalAntennaOn, "External antenna on");
			User::InfoPrint(KTxtExternalAntennaOn);
			PrintPropertyForThreadL(KTxtExternalAntennaOn);
			break;
		}
	}


/**
Constructor
 */
CBtGpsPsyExtPowerWatch::CBtGpsPsyExtPowerWatch()
	{
	}

/**
Adds the active object to the active scheduler and creates a handle to the integer property
@param aConsole The console interface
 */
void CBtGpsPsyExtPowerWatch::ConstructL()
	{
	User::LeaveIfError(iProperty.Attach(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyExtPowerState));
	CActiveScheduler::Add(this);
	}

/**
Constructs a CBtGpsPsyExtPowerWatch object
@param aConsole The console interface
@return A CBtGpsPsyExtPowerWatch object
 */
CBtGpsPsyExtPowerWatch* CBtGpsPsyExtPowerWatch::NewL()
	{
	CBtGpsPsyExtPowerWatch* self = new (ELeave) CBtGpsPsyExtPowerWatch;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor
 */
CBtGpsPsyExtPowerWatch::~CBtGpsPsyExtPowerWatch()
	{
	iProperty.Close();
	Cancel();
	}

/**
Prints a property value
@param aValue The changed value of the property
 */
TInt CBtGpsPsyExtPowerWatch::CheckProperty()
	{
	TInt err = KErrNone;
	TInt value = KErrNotFound;
	err = iProperty.Get(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyExtPowerState, value);
	if(err == KErrNone)
		{
		if((value < EExtPowerNA) && (value > EExtPowerOn))
			{
			err = KErrArgument;
			}
		}
	return err;
	}

/**
Prints a property value
@param aValue The changed value of the property
 */
void CBtGpsPsyExtPowerWatch::PrintPropertyL()
	{
	TInt value = KErrNotFound;
	iProperty.Get(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyExtPowerState, value);

	switch(value)
		{
		case EExtPowerNA:
			_LIT(KTxtExternalPowerNotApplicable, "External power not applicable");
			User::InfoPrint(KTxtExternalPowerNotApplicable);
			PrintPropertyForThreadL(KTxtExternalPowerNotApplicable);
			break; 
		case EExtPowerOff:
			_LIT(KTxtExternalPowerOff, "External power off");
			User::InfoPrint(KTxtExternalPowerOff);
			PrintPropertyForThreadL(KTxtExternalPowerOff);
			break; 
		case EExtPowerOn:
			_LIT(KTxtExternalPowerOn, "External power on");
			User::InfoPrint(KTxtExternalPowerOn);
			PrintPropertyForThreadL(KTxtExternalPowerOn);
			break;
		}
	}

/**
Constructor
 */
CBtGpsPsyStateWatch::CBtGpsPsyStateWatch()
	{
	}

/**
Adds the active object to the active scheduler and creates a handle to the integer property
@param aConsole The console interface
 */
void CBtGpsPsyStateWatch::ConstructL()
	{
	User::LeaveIfError(iProperty.Attach(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyState));
	CActiveScheduler::Add(this);
	}

/**
Constructs a CBtGpsPsyStateWatch object
@param aConsole The console interface
@return A CBtGpsPsyStateWatch object
 */
CBtGpsPsyStateWatch* CBtGpsPsyStateWatch::NewL()
	{
	CBtGpsPsyStateWatch* self = new (ELeave) CBtGpsPsyStateWatch;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor
 */
CBtGpsPsyStateWatch::~CBtGpsPsyStateWatch()
	{
	iProperty.Close();
	Cancel();
	}

/**
Prints a property value
@param aValue The changed value of the property
 */
TInt CBtGpsPsyStateWatch::CheckProperty()
	{
	TInt err = KErrNone;
	TInt value = KErrNotFound;
	err = iProperty.Get(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyState, value);
	if(err == KErrNone)
		{
		if((value < EPSYNotLoaded) && (value > EPSYInStandBy))
			{
			err = KErrArgument;
			}
		}
	return err;
	}

/**
Prints a property value
@param aValue The changed value of the property
 */
void CBtGpsPsyStateWatch::PrintPropertyL()
	{
	TInt value = KErrNotFound;
	iProperty.Get(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyState, value);

	switch(value)
		{
		case EPSYNotLoaded:
			_LIT(KTxtBTGPSPSYNotLoaded, "BT GPS PSY not loaded");
			User::InfoPrint(KTxtBTGPSPSYNotLoaded);
			PrintPropertyForThreadL(KTxtBTGPSPSYNotLoaded);
			break; 
		case EPSYLoaded:
			_LIT(KTxtBTGPSPSYLoadedNotConnected, "BT GPS PSY loaded but not connected to GPS device");
			User::InfoPrint(KTxtBTGPSPSYLoadedNotConnected);
			PrintPropertyForThreadL(KTxtBTGPSPSYLoadedNotConnected);
			break;
		case EPSYLoadedAndPNOKSupportedBTGPSUsed:
			_LIT(KTxtBTPSYLoadedAndPNOKSupportedBTGPSUsed, "BT GPS PSY loaded and connected to PNOK enabled GPS device");
			User::InfoPrint(KTxtBTPSYLoadedAndPNOKSupportedBTGPSUsed);
			PrintPropertyForThreadL(KTxtBTPSYLoadedAndPNOKSupportedBTGPSUsed);
			break;
		case EPSYLoadedAndPNOKNotSupportedBTGPSUsed:
			_LIT(KTxtBTPSYLoadedAndPNOKNotSupportedBTGPSUsed, "BT GPS PSY loaded and connected to non PNOK enabled GPS device");
			User::InfoPrint(KTxtBTPSYLoadedAndPNOKNotSupportedBTGPSUsed);
			PrintPropertyForThreadL(KTxtBTPSYLoadedAndPNOKNotSupportedBTGPSUsed);
			break;
		case EPSYInStandBy:
			_LIT(KTxtBTPSYInStandBy, "BT GPS PSY in stand by");
			User::InfoPrint(KTxtBTPSYInStandBy);
			PrintPropertyForThreadL(KTxtBTPSYInStandBy);
			break;
		}
	}

