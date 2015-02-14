
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


#include "SecondaryWindow.h"
#include "../avgconfigwrapper.h"

#include "Player.h"

#include "../base/Exception.h"
#include "../base/Logger.h"
#include "../base/ScopeTimer.h"
#include "../base/OSHelper.h"
#include "../base/StringHelper.h"

#include "../graphics/GLContext.h"
#include "../graphics/Filterflip.h"
#include "../graphics/Filterfliprgb.h"
#ifdef __linux__
#ifndef AVG_ENABLE_EGL
  #include "../graphics/SecondaryGLXContext.h"
#endif
#include "../graphics/GLContextManager.h"
#endif

#include <iostream>

using namespace std;

namespace avg {

SecondaryWindow::SecondaryWindow(const WindowParams& wp, bool bIsFullscreen,
        GLConfig glConfig, int index)
    : Window(wp, bIsFullscreen)
{
#ifdef __linux__
    GLContext* pMainContext = GLContext::getCurrent();
    GLContext* pGLContext;
    IntRect windowDimensions(wp.m_Pos, wp.m_Pos+wp.m_Size);
    string sDisplay = ":0." + toString(wp.m_DisplayServer);
    pGLContext = new SecondaryGLXContext(glConfig, sDisplay, windowDimensions,
            wp.m_bHasWindowFrame);
    setGLContext(pGLContext);
    
    pMainContext->activate();
#endif

    RenderThread renderer(m_CmdQueue, index);
    m_pThread = new boost::thread(renderer);
}

SecondaryWindow::~SecondaryWindow()
{
    m_CmdQueue.pushCmd(boost::bind(&RenderThread::stop, _1));
    m_pThread->join();
    delete m_pThread;
}

void SecondaryWindow::setTitle(const std::string& sTitle)
{
}

vector<EventPtr> SecondaryWindow::pollEvents()
{
    return vector<EventPtr>();
/*
    XEvent xev;
    while (XCheckWindowEvent(m_pDisplay, m_SecondWindow, ButtonPressMask, &xev)) {
        switch(xev.type) {
            case ButtonPress:
                cerr << "..." << endl;
                break;
            default:
                cerr << "?" << endl;
        }
    }
*/
}


void SecondaryWindow::render(Canvas* pCanvas, IntRect viewport, MCFBOPtr pFBO)
{
    m_CmdQueue.pushCmd(boost::bind(
            &RenderThread::render, _1, pCanvas, shared_from_this(), pFBO, viewport));
}

}
