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


#ifndef T_CAUDIO_OUTPUT_H
#define T_CAUDIO_OUTPUT_H

//User Includes
#include "DataWrapperBase.h"

//Epoc Includes
#include <audiooutput.h>//CAudioOutput
#include <audiooutputmessagetypes.h>
#include <maudiooutputobserver.h>
#include <sounddevice.h>//CMMFDevSound

class CT_CAudioOutputData : public CDataWrapperBase, public MAudioOutputObserver
	{
public:
	static	CT_CAudioOutputData* NewL();
	virtual TAny* GetObject();
	virtual TBool DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	CT_CAudioOutputData();
	
private:
	void DestroyData();

protected:
	void DoCmdNewL(const TTEFSectionName& aSection);
	void DoCmdDestructor();

	void DoCmdAudioOutput();
	void DoCmdDefaultAudioOutput();
	void DoCmdRegisterObserverL();
	void DoCmdSecureOutput();
	void DoCmdSetAudioOutputL(const TTEFSectionName& aSection);
	void DoCmdSetSecureOutputL(const TTEFSectionName& aSection);
	void DoCmdUnregisterObserver();

public:
	/**
     * From MAudioOutputObserver.
     */                
     void DefaultAudioOutputChanged( CAudioOutput& aAudioOutput, CAudioOutput::TAudioOutputPreference aNewDefault );
	
private:
	/**
	 * Wrapped object
	 */
    CAudioOutput* iAudioOutput; 
    /**
     * Pref of audio output
     */
    CAudioOutput::TAudioOutputPreference audioOutputPref;
    /**
     * Secure Output(DoCmdSecureOutput)
     */
    TBool iSecureOutput;
    /**
     * Enum for output options
     */
    static const TEnumEntryTable iEnumOutputOptions[];
	};


#endif //T_CAUDIO_OUTPUT_H

