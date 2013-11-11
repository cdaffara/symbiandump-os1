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


#ifndef T_CRESTRICTEDAUDIOOUTPUTDATA_H_
#define T_CRESTRICTEDAUDIOOUTPUTDATA_H_

//User Includes
#include "DataWrapperBase.h"

//Epoc Includes
#include <restrictedaudiooutput.h>//CRestrictedAudioOutput
#include <restrictedaudiooutputmessagetypes.h>
#include <sounddevice.h>//CMMFDevSound

/** 
 * This class tests the CRestrictedAudioOutput API's 
 */

class CT_CRestrictedAudioOutputData : public CDataWrapperBase
	{
public:
	~CT_CRestrictedAudioOutputData();
	static CT_CRestrictedAudioOutputData* NewL();
	virtual TAny* GetObject();
	virtual TBool DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/);

private:
	void DestroyData();
protected:
	void DoCmdNewL(const TTEFSectionName& aSection);
	void DoCmdDestructor();
	void DoCmdAppendAllowedOutput(const TTEFSectionName& aSection);
	void DoCmdRemoveAllowedOutput(const TTEFSectionName& aSection);
	void DoCmdGetAllowedOutput(const TTEFSectionName& aSection);
	void DoCmdGetAllowedOutputCount();
	void DoCmdReset();
	void DoCmdCommit();

private:
	/**
	 * Wrapped object
	 */
	CRestrictedAudioOutput* iRestrictedAudioOutput;		
	/**
	 * Enum that contain allowed output preference
	 */
	static const TEnumEntryTable iEnumAllowedOutputPreference[];
	};
	
#endif /*T_CRESTRICTEDAUDIOOUTPUTDATA_H_*/
