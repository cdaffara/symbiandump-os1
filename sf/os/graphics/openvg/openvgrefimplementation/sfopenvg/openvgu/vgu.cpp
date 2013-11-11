/*
* Copyright (c) 2009 Symbian Foundation Ltd
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Symbian Foundation Ltd - initial contribution.
* 
* Contributors:
*
* Description:
* Implementation of VGU interface
*/

#include <e32base.h>
#include <vgu.h>

#include "vguinternal.h" //contains do_vguXX fucntion declarations

extern "C" {

EXPORT_C VGUErrorCode vguLine(VGPath path, VGfloat x0, VGfloat y0, VGfloat x1, VGfloat y1)
{
return do_vguLine(path, x0, y0, x1, y1);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EXPORT_C VGUErrorCode vguPolygon(VGPath path, const VGfloat * points, VGint count, VGboolean closed)
{
return do_vguPolygon(path, points, count, closed);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EXPORT_C VGUErrorCode vguRect(VGPath path, VGfloat x, VGfloat y, VGfloat width, VGfloat height)
{
return do_vguRect(path, x, y, width, height);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EXPORT_C VGUErrorCode vguRoundRect(VGPath path, VGfloat x, VGfloat y, VGfloat width, VGfloat height, VGfloat arcWidth, VGfloat arcHeight)
{
return do_vguRoundRect(path, x, y, width, height, arcWidth, arcHeight);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EXPORT_C VGUErrorCode vguEllipse(VGPath path, VGfloat cx, VGfloat cy, VGfloat width, VGfloat height)
{
return do_vguEllipse(path, cx, cy, width, height);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EXPORT_C VGUErrorCode vguArc(VGPath path, VGfloat x, VGfloat y, VGfloat width, VGfloat height, VGfloat startAngle, VGfloat angleExtent, VGUArcType arcType)
{
return do_vguArc(path, x, y, width, height, startAngle, angleExtent, arcType);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EXPORT_C VGUErrorCode vguComputeWarpQuadToSquare(VGfloat sx0, VGfloat sy0, VGfloat sx1, VGfloat sy1, VGfloat sx2, VGfloat sy2, VGfloat sx3, VGfloat sy3, VGfloat * matrix)
{
return do_vguComputeWarpQuadToSquare(sx0, sy0, sx1, sy1, sx2, sy2, sx3, sy3, matrix);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EXPORT_C VGUErrorCode vguComputeWarpSquareToQuad(VGfloat dx0, VGfloat dy0, VGfloat dx1, VGfloat dy1, VGfloat dx2, VGfloat dy2, VGfloat dx3, VGfloat dy3, VGfloat * matrix)
{
return do_vguComputeWarpSquareToQuad(dx0, dy0, dx1, dy1, dx2, dy2, dx3, dy3, matrix);
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

EXPORT_C VGUErrorCode vguComputeWarpQuadToQuad(VGfloat dx0, VGfloat dy0, VGfloat dx1, VGfloat dy1, VGfloat dx2, VGfloat dy2, VGfloat dx3, VGfloat dy3, VGfloat sx0, VGfloat sy0, VGfloat sx1, VGfloat sy1, VGfloat sx2, VGfloat sy2, VGfloat sx3, VGfloat sy3, VGfloat * matrix)
{
return do_vguComputeWarpQuadToQuad(dx0, dy0, dx1, dy1, dx2, dy2, dx3, dy3, sx0, sy0, sx1, sy1, sx2, sy2, sx3, sy3, matrix);
}

}
