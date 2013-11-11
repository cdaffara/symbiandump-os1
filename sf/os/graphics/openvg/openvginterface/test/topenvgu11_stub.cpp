// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This test stub has been manually created based on the .def file generator
// utility available in ../group/openvg11
//
//

#include <VG/openvg.h>
#include <VG/vgu.h>

#include <e32def.h>
#include <e32uid.h>
    
#ifdef __cplusplus
extern "C" {
#endif


// --------------------------------------------------------------------------------------------------------------------------------------------
// OpenVG VGU functions
	EXPORT_C VGUErrorCode  vguArc(VGPath /*path*/, VGfloat /*x*/, VGfloat /*y*/, VGfloat /*width*/, VGfloat /*height*/, VGfloat /*startAngle*/, VGfloat /*angleExtent*/, VGUArcType /*arcType*/) { return VGU_ILLEGAL_ARGUMENT_ERROR; }
EXPORT_C VGUErrorCode  vguComputeWarpQuadToQuad(VGfloat /*dx0*/, VGfloat /*dy0*/, VGfloat /*dx1*/, VGfloat /*dy1*/, VGfloat /*dx2*/, VGfloat /*dy2*/, VGfloat /*dx3*/, VGfloat /*dy3*/, VGfloat /*sx0*/, VGfloat /*sy0*/, VGfloat /*sx1*/, VGfloat /*sy1*/, VGfloat /*sx2*/, VGfloat /*sy2*/, VGfloat /*sx3*/, VGfloat /*sy3*/, VGfloat * /*matrix*/) { return VGU_ILLEGAL_ARGUMENT_ERROR; }
EXPORT_C VGUErrorCode  vguComputeWarpQuadToSquare(VGfloat /*sx0*/, VGfloat /*sy0*/, VGfloat /*sx1*/, VGfloat /*sy1*/, VGfloat /*sx2*/, VGfloat /*sy2*/, VGfloat /*sx3*/, VGfloat /*sy3*/, VGfloat * /*matrix*/) { return VGU_ILLEGAL_ARGUMENT_ERROR; }
EXPORT_C VGUErrorCode  vguComputeWarpSquareToQuad(VGfloat /*dx0*/, VGfloat /*dy0*/, VGfloat /*dx1*/, VGfloat /*dy1*/, VGfloat /*dx2*/, VGfloat /*dy2*/, VGfloat /*dx3*/, VGfloat /*dy3*/, VGfloat * /*matrix*/) { return VGU_ILLEGAL_ARGUMENT_ERROR; }
EXPORT_C VGUErrorCode  vguEllipse(VGPath /*path*/, VGfloat /*cx*/, VGfloat /*cy*/, VGfloat /*width*/, VGfloat /*height*/) { return VGU_ILLEGAL_ARGUMENT_ERROR; }
EXPORT_C VGUErrorCode  vguLine(VGPath /*path*/, VGfloat /*x0*/, VGfloat /*y0*/, VGfloat /*x1*/, VGfloat /*y1*/) { return VGU_ILLEGAL_ARGUMENT_ERROR; }
EXPORT_C VGUErrorCode  vguPolygon(VGPath /*path*/, const VGfloat * /*points*/, VGint /*count*/, VGboolean /*closed*/) { return VGU_ILLEGAL_ARGUMENT_ERROR; }
EXPORT_C VGUErrorCode  vguRect(VGPath /*path*/, VGfloat /*x*/, VGfloat /*y*/, VGfloat /*width*/, VGfloat /*height*/) { return VGU_ILLEGAL_ARGUMENT_ERROR; }
EXPORT_C VGUErrorCode  vguRoundRect(VGPath /*path*/, VGfloat /*x*/, VGfloat /*y*/, VGfloat /*width*/, VGfloat /*height*/, VGfloat /*arcWidth*/, VGfloat /*arcHeight*/) { return VGU_ILLEGAL_ARGUMENT_ERROR; }

// --------------------------------------------------------------------------------------------------------------------------------------------
// END: OpenVG VGU
// --------------------------------------------------------------------------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif
