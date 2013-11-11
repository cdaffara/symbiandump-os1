/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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

 
#ifndef T_CHWRMLIGHTS_DATA_H
#define T_CHWRMLIGHTS_DATA_H

//User Includes
#include "datawrapperbase.h"

//System includes
#include <hwrmlight.h>



/**
 * T_CHWRMLightsData class
 *
 */
class CT_CHWRMLightsData : public CDataWrapperBase
	{
public:
	~CT_CHWRMLightsData();
	static	CT_CHWRMLightsData* NewL();

	virtual TAny* GetObject();
	virtual TBool DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	CT_CHWRMLightsData();
	void ConstructL();

private:
	TInt DoCmdNewL(const TTEFSectionName& aSection);
	TInt DoCmdReserveLightL(const TTEFSectionName& aSection);
	TInt DoCmdLightBlinkL(const TTEFSectionName& aSection);
	TInt DoCmdLightOnL(const TTEFSectionName& aSection);
	TInt DoCmdLightOffL(const TTEFSectionName& aSection);
	TInt DoCmdLightStatus(const TTEFSectionName& aSection);
	TInt DoCmdDestructor();

private:
	void DestroyData();

private:

/**
 * HWRM lights instance
 */
	CHWRMLight* iLight;
	
	static const TEnumEntryTable iEnumLightStatus[];
	};

#endif // T_CHWRMLIGHTS_DATA_H
