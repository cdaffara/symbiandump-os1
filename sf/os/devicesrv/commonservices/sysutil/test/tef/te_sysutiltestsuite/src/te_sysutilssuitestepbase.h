/**
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
* Description:
*
*/



/**
 @file 
 @internalTechnology
*/

#if (!defined __TE_SYSUTILS_SUITE_STEP_BASE__)
#define __TE_SYSUTILS_SUITE_STEP_BASE__
#include <test/testexecutestepbase.h>
#include "sysutilfunctions.h"
#include "sysutilconstants.h"
#include <sysutil.h>

// Please add your include here if you have 
_LIT(KFilePresent, "FilePresent");

_LIT(KManufacturerName, "ManufacturerName");
_LIT(KModelName, "ModelName");
_LIT(KModelCode, "ModelCode");
_LIT(KRevisionID, "RevisionID");
_LIT(KDefaultDeviceName, "DefaultDeviceName");
_LIT(KUIPlatformName, "UIPlatformName");
_LIT(KUIPlatformVersion, "UIPlatformVersion");
_LIT(KUIPlatformVersionMajor, "UIPlatformVersionMajor");
_LIT(KUIPlatformVersionMinor, "UIPlatformVersionMinor");
_LIT(KOSVersion, "OSVersion");
_LIT(KOSVersionMajor, "OSVersionMajor");
_LIT(KOSVersionMinor, "OSVersionMinor");
_LIT(KDeviceCreatorAttribute, "DeviceCreatorAttribute");

_LIT(KManufacturerNameError, "ManufacturerNameError");
_LIT(KModelNameError, "ModelNameError");
_LIT(KModelCodeError, "ModelCodeError");
_LIT(KRevisionIDError, "RevisionIDError");
_LIT(KDefaultDeviceNameError, "DefaultDeviceNameError");
_LIT(KUIPlatformNameError, "UIPlatformNameError");
_LIT(KUIPlatformVersionStringError, "UIPlatformVersionStringError");
_LIT(KUIPlatformVersionIntError, "UIPlatformVersionIntError");
_LIT(KOSVersionStringError, "OSVersionStringError");
_LIT(KOSVersionIntError, "OSVersionIntError");
_LIT(KDeviceCreatorAttributeError, "DeviceCreatorAttributeError");


/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/
class CTeSysUtilsSuiteStepBase : public CTestStep
	{
public:
	virtual ~CTeSysUtilsSuiteStepBase();
	CTeSysUtilsSuiteStepBase();
	

//Please add your class members which will be common to all individual test steps:
protected:
	TInt FindValueL(const TUint aPCKey, const TUint aCRKey, const TInt aPatchableData);
	TBool CheckError(const TDesC& aAttributeError, const TInt aValue);
	TBool CheckFileL(const TDesC& aFilename);
	TBool CheckValue(const TDesC& aAttribute, const TDesC16& aValue);
	TBool CheckValue(const TDesC& aAttribute, const TUint16 aValue);
	
	TBool TestNamedAttributeFunctions(CDeviceTypeInformation& aAttribs);
	TBool TestUnnamedAttributeFunctions(CDeviceTypeInformation& aAttribs);
	RFs iFs; // File system	
	};

#endif
