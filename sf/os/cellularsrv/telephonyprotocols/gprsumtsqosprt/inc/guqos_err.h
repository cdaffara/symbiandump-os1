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

#ifndef __GUQOS_ERR_H__
#define __GUQOS_ERR_H__

/*
 *  PCO Error codes:
 *  In all the cases listed below a common GTP cause code, 
 *  "User authentication failed", see 3GPP TS 29 060 [20],
 *  shall be used in the response message.
 */
enum TGuQoSReasonCode
{
    /*
     *  Error code No. 1 "Authorization failure of the request"
     *  This error code indicates that the secondary PDP context request is 
     *  rejected because the authorizing entity is unable to provide an 
     *  authorization decision for the binding information.
     */
    EGuqosSblpAuthorizationFailureOfRequest = -3062,  

    /* 
     *  Error code No. 2 "Missing binding information"
     *  This error code indicates that the secondary PDP context activation or PDP
     *  context modification request is rejected because the binding information 
     *  was not included in the request although required.
     */
    EGuqosSblpMissingBindingInfo            = -3061,

    /* 
     *  Error code No. 3 "Invalid binding information"
     *  This error code indicates that the secondary PDP context activation or PDP 
     *  context modification request is rejected because the authorizing entity 
     *  could not be resolved from the binding information.
     */
    EGuqosSblpInvalidBindingInfo            = -3060,

    /*
     *  Error code No. 4 "Binding information not allowed"
     *  This error code indicates that the secondary PDP context activation or PDP 
     *  context modification request is rejected because the Go interface is 
     *  disabled or not supported in the GGSN and hence binding information is not 
     *  allowed. The error code may also indicate that the PDP context modification 
     *  is rejected because binding information is not allowed for modification of 
     *  previously non-authorised PDP context or that the binding information is 
     *  not allowed when the PDP context is indicated to be used for IMS signaling.
     */
	EGuqosSblpBindingInfoNotAllowed         = -3059,

    /*
     *  Error code No.5 "Authorizing entity temporarily unavailable"
     *  This error code indicates that the secondary PDP context activation or PDP 
     *  context modification request is rejected because the authorizing entity 
     *  indicated by the binding information is temporarily unavailable.
     */
    EGuqosSblpAuthorizingEntityTemporarilyUnavailable   = -3058,

    /*  
     *  Error code No. 6 "No corresponding session"
     *  This error code indicates that the secondary PDP context activation request is rejected because the authorizing entity
     *  cannot associate the Authorisation token of binding information with any ongoing session or binding information
     *  contains invalid flow identifier(s).The error code also indicates that the PDP context modification request is rejected by
     *  the authorizing entity because the authorization token has changed or the binding information contains invalid flow
     *  identifier(s).
     */
	EGuqosSblpNoCorrespondingSession        = -3057,

    /*
     *  Error code No. 7 "Invalid bundling"
     *  This error code indicates that the secondary PDP context activation or PDP context modification request is rejected
     *  because the authorizing entity does not allow the grouping of the flow identifiers in the same PDP Context.
     */
	EGuqosSblpInvalidBundling	            = -3056
};
#endif // __GUQOS_ERR_H__
