//
//  libavg - Media Playback Engine. 
//  Copyright (C) 2003-2014 Ulrich von Zadow
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Current versions can be found at www.libavg.de
//

#include "GPUInvertFilter.h"
#include "ShaderRegistry.h"
#include "OGLShader.h"
#include "GLContextManager.h"

#include "../base/ObjectCounter.h"
#include "../base/Logger.h"

#define SHADERID_INVERT_COLOR "invert"

using namespace std;

namespace avg {

GPUInvertFilter::GPUInvertFilter(const IntPoint& size, bool bUseAlpha, bool bStandalone)
    : GPUFilter(SHADERID_INVERT_COLOR, bUseAlpha, bStandalone)
{
    ObjectCounter::get()->incRef(&typeid(*this));
    setDimensions(size);
    GLContextManager* pCM = GLContextManager::get();
    m_pTextureParam = pCM->createShaderParam<int>(SHADERID_INVERT_COLOR, "u_Texture");
}

GPUInvertFilter::~GPUInvertFilter()
{
    ObjectCounter::get()->decRef(&typeid(*this));
}

void GPUInvertFilter::applyOnGPU(GLContext* pContext, GLTexturePtr pSrcTex)
{
    getShader()->activate();
    m_pTextureParam->set(pContext, 0);
    draw(pContext, pSrcTex, WrapMode());
}

}

