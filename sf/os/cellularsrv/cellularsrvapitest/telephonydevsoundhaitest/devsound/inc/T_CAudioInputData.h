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


#ifndef T_CAUDIO_INPUT_H
#define T_CAUDIO_INPUT_H

//User Includes
#include "DataWrapperBase.h"

//Epoc includes
#include <audioinput.h>//CAudioInput
#include <audioinputmessagetypes.h>
#include <sounddevice.h>//CMMFDevSound

class CT_CAudioInputData : public CDataWrapperBase
	{
public:
	static	CT_CAudioInputData* NewL();
	virtual TAny* GetObject();
	virtual TBool DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	CT_CAudioInputData();
	
private:
	void DestroyData();

protected:
	void DoCmdNewL(const TTEFSectionName& aSection);
	void DoCmdDestructor();

	void DoCmdAudioInput();
	void DoCmdSetAudioInputL(const TTEFSectionName& aSection);
	
private:
	/**
	 * Wrapped object
	 */
    CAudioInput* iAudioInput; 
    /**
     * Enum for input options
     */
    static const TEnumEntryTable iEnumInputOptions[];
	};

#endif //T_CAUDIO_INPUT_H
