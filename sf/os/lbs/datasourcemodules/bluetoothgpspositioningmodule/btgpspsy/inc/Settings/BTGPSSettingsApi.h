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




#ifndef BTGPSSETTINGSAPI_H
#define BTGPSSETTINGSAPI_H

/**
*  Vocabulary class for BTG GPS Settings API.
*  
*/
class TBTGPSSettingsApi
    {
    public:
		/** 
		* The night mode states
		*/
	    enum TNightModeState
	    	{
	    	ENightModeNA = -1, /** Nightmode no applicable */
	    	ENightModeOff,     /** Nightmode off */
	    	ENightModeOn       /** Nightmode on */
	    	};
	    	
		/** 
		* The dialog states which are informed to the client
		*/
	    enum TDialogState
	    	{
	    	EDialogNA = -1, /** Dialog no applicable */
	    	EDialogOff,     /** Dialog off */
	    	EDialogOn       /** Dialog on */
	    	};

        /**
        * Bt device validity
        */
        enum TBtDeviceValidity
            {
            EBtDeviceInvalid = 0, /** Device is invalid */
            EBtDeviceNonNokGps,   /** Device is valid and non-Nok Gps */
            EBtDeviceNokGps,      /** Device is valid and Nok Gps */
            };
    };
    
#endif //BTGPSSETTINGSAPI_H
// End of File
