// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalTechnology
 @released
*/

#ifndef LBS_POSREQUESTHANDLER_H
#define LBS_POSREQUESTHANDLER_H

// System  
#include <e32base.h>
#include <lbspositioninfo.h>
#include <lbs/lbssuplproxyposclient.h>
#include <lbs/epos_comasuplposition.h>
#include <lbssatellite.h>

//Component


class MRequestHandlerObserver
	{
public:	
	virtual void OnGetPositionComplete(TInt aError) = 0;
	virtual void OnGetSuplInfoComplete(TInt aError) = 0;
	};

/**
* This Class Handles verification requests.
*/
NONSHARABLE_CLASS(CSuplPosRequestHandler) : public CActive
    {
    public:  

    	enum TRequestType
    		{
    		ENone,			// no request outstanding
    		EGetPosition,	// getposition request outstanding
    		EGetSuplInfo,	// getsuplinfo request outstanding
    		};
        
        static CSuplPosRequestHandler* NewL(MRequestHandlerObserver& aObserver, RSuplProxyPosSubSession& aSubSession);
              
        virtual ~CSuplPosRequestHandler();

    public: 
    	void GetPositionL(COMASuplPosition* aPosition);
        void CancelGetPosition();
        void GetSuplInfoL(COMASuplInfoRequestList* aSuplInfoRequestList);
        void CancelGetSuplInfoL();
        void ResetState();
    private: // From CActive

       void RunL();
       void DoCancel();
       TInt RunError(TInt aError);
              
    private:
        // c'tor
        CSuplPosRequestHandler(MRequestHandlerObserver& aObserver, RSuplProxyPosSubSession& aSubSession);
        // Fills in the OMA position member from the LBS position info member
        void FillOMAPositionL(const TPositionInfoBase& aLbsPosition, COMASuplPosition* aSuplPosition);
        // Fills in the OMA SuplInfoRequestList
        TInt FillSuplInfoListL();        
        // whether the stored SuplInfoRequestList includes a pospayload request
        TBool RequestMaskIncludesPosPayload();
        // sets the request mask in the structure we pass to SPPM based for GetSuplInfoL()
        void SetRequestMaskFromSuplInfoListL();
    private:    // Data
    	// the subsession to use for making requests to LBS subsystem
        RSuplProxyPosSubSession& 	iSubSession;
        // passed by the client to store the returned position. Ownership not taken.
        COMASuplPosition*			iOmaPosition;
        // observer to inform when async request completes
        MRequestHandlerObserver&	iObserver;
        // Passed to the LBS subsystem to be filled with position
        TPositionCourseInfo		iPosition;	// pass in the biggest container we might need?
        // the type of request we are currently waiting for
        TRequestType				iRequestOutstanding;
        // passed by the client to store the returned supl info. Ownership not taken.     
        COMASuplInfoRequestList*    iSuplInfoReqList;  
        // Passed to the SPPM to be filled with supl info (not including any pospayload)
        TSuplInfoRequestData		iSuplInfoRequestData;	
        // Passed to the SPPM to be filled with pospayload if one requested in GetSuplInfoL()
        HBufC8*						iPosPayload;
        // Indicates whether we have already seen a GetSuplInfoL() request for pospayload + velocity 
        TBool						iFirstSUPLPOSPosPayloadProcessed;
    };

#endif      // LBS_POSREQUESTHANDLER_H


