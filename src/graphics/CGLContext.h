//
//  libavg - Media Playback Engine. 
//  Copyright (C) 2003-2011 Ulrich von Zadow
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
#ifndef _CGLContext_H_
#define _CGLContext_H_
#include "../api.h"

#include "OGLHelper.h"
#include "GLBufferCache.h"
#include "GLConfig.h"

#include "../base/GLMHelper.h"

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#undef check // Conflicts with boost
#elif defined linux
#include <GL/glx.h>
#elif defined _WIN32
#include <gl/gl.h>
#include <gl/glu.h>
#endif

#include <boost/shared_ptr.hpp>
#include <boost/thread/tss.hpp>

struct SDL_SysWMinfo;

namespace avg {

class CGLContext;
typedef boost::shared_ptr<CGLContext> CGLContextPtr;
class ShaderRegistry;
typedef boost::shared_ptr<ShaderRegistry> ShaderRegistryPtr;
class StandardShader;
typedef boost::shared_ptr<StandardShader> StandardShaderPtr;

class AVG_API CGLContext {
public:
    CGLContext(const GLConfig& glConfig, const IntPoint& windowSize=IntPoint(0,0), 
            const SDL_SysWMinfo* pSDLWMInfo=0);
    virtual ~CGLContext();
    void init();

    void activate();
    ShaderRegistryPtr getShaderRegistry() const;
    StandardShaderPtr getStandardShader();
    bool useGPUYUVConversion() const;
    bool useMinimalShader();

    // GL Object caching.
    GLBufferCache& getVertexBufferCache();
    GLBufferCache& getIndexBufferCache();
    GLBufferCache& getPBOCache();
    unsigned genFBO();
    void returnFBOToCache(unsigned fboID);

    // GL state cache.
    void setBlendColor(const glm::vec4& color);
    enum BlendMode {BLEND_BLEND, BLEND_ADD, BLEND_MIN, BLEND_MAX, BLEND_COPY};
    void setBlendMode(BlendMode mode, bool bPremultipliedAlpha = false);
    void bindTexture(unsigned unit, unsigned texID);

    const GLConfig& getConfig();
    void logConfig();
    size_t getVideoMemInstalled();
    size_t getVideoMemUsed();
    int getMaxTexSize();
    bool usePOTTextures();
    OGLMemoryMode getMemoryModeSupported();
    bool isGLES() const;

    bool initVBlank(int rate);
    void swapBuffers();

    static void enableErrorChecks(bool bEnable);
    static void checkError(const char* pszWhere);
    static void mandatoryCheckError(const char* pszWhere);

    static BlendMode stringToBlendMode(const std::string& s);

    static CGLContext* getCurrent();
    static CGLContext* getMain();
    static void setMain(CGLContext * pMainContext);

    static int nextMultiSampleValue(int curSamples);

protected:
    void getVersion(int& major, int& minor) const;


private:
    void checkGPUMemInfoSupport();
#ifdef _WIN32
    void checkWinError(BOOL bOK, const std::string& sWhere);
#endif

    // Vertical blank stuff.
    void initMacVBlank(int rate);
    enum VBMethod {VB_GLX, VB_APPLE, VB_WIN, VB_NONE};
    static VBMethod s_VBMethod;

#ifdef __APPLE__
    CGLContextObj m_Context;
#elif defined linux
    void createGLXContext(const GLConfig& glConfig, const IntPoint& windowSize, 
            const SDL_SysWMinfo* pSDLWMInfo);

    Display* m_pDisplay;
    Colormap m_Colormap;
    GLXDrawable m_Drawable;
    GLXContext m_Context;
#elif defined _WIN32
    HWND m_hwnd;
    HDC m_hDC;
    HGLRC m_Context;
#endif
 
    bool m_bOwnsContext;
    
    ShaderRegistryPtr m_pShaderRegistry;
    StandardShaderPtr m_pStandardShader;

    GLBufferCache m_VertexBufferCache;
    GLBufferCache m_IndexBufferCache;
    GLBufferCache m_PBOCache;
    std::vector<unsigned int> m_FBOIDs;

    int m_MaxTexSize;
    GLConfig m_GLConfig;
    bool m_bCheckedGPUMemInfoExtension;
    bool m_bGPUMemInfoSupported;
    bool m_bCheckedMemoryMode;
    OGLMemoryMode m_MemoryMode;

    // OpenGL state
    glm::vec4 m_BlendColor;
    BlendMode m_BlendMode;
    bool m_bPremultipliedAlpha;
    unsigned m_BoundTextures[16];

    int m_MajorGLVersion;
    int m_MinorGLVersion;

    static bool s_bErrorCheckEnabled;

    static boost::thread_specific_ptr<CGLContext*> s_pCurrentContext;
    static CGLContext* s_pMainContext;
};

}
#endif


