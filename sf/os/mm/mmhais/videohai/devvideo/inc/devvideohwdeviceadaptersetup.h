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

#ifndef __DEVVIDEOHWDEVICEADAPTERSETUP_H__
#define __DEVVIDEOHWDEVICEADAPTERSETUP_H__

/**
Custom interface for setting up the DevVideo hw device adapter
@publishedPartner
@released
*/	
class MDevVideoHwDeviceAdapterSetup 
	{
public:
	/**
	Set the Uid of the processing unit into the hw device adapter
	*/
	virtual void LoadProcessingUnitL(const CImplementationInformation& aImplInfo) = 0;
	};
	
#endif

