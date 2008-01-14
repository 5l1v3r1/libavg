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
//  Original author of this file is igor@c-base.org.
//

#ifndef _TrackerConfig_H_
#define _TrackerConfig_H_

#include "DeDistort.h"

#include "../base/Rect.h"

#include <boost/shared_ptr.hpp>
#include <string>

#include <libxml/xpath.h>

namespace avg {

struct BlobConfig
{
    BlobConfig(bool bIsTouch);
    virtual ~BlobConfig();
    void load(xmlNodePtr pParentNode, const std::string& sFilename);
    void save(xmlTextWriterPtr writer);

    bool m_bIsTouch;
    int m_Threshold; //min pixel val for detection
    double m_Similarity; //max distance for tracking blobs
    double m_AreaBounds[2]; //min, max for area in percents of screen size
    double m_EccentricityBounds[2]; //min, max for Eccentricity
};

typedef boost::shared_ptr<struct BlobConfig> BlobConfigPtr;

struct TrackerConfig
{
    TrackerConfig();
    virtual ~TrackerConfig();
    
    void load(const std::string& sCustomFilename = "");
    void save(const std::string& sCustomFilename = "");
    void parse(bool bOnlyDyn);
    xmlXPathObjectPtr findConfigNodes(const xmlChar* xpExpr);
    void setParam(const xmlChar* xpElement, const xmlChar* Value);
    std::string getParam(const xmlChar* xpExpr);

    // Camera params
    std::string m_sSource;
    std::string m_sDevice;
    std::string m_sPixFmt;
    IntPoint m_Size;
    int m_Channel;
    int m_FPS;
    int m_Brightness;
    int m_Exposure;
    int m_Gamma;
    int m_Gain;
    int m_Shutter;

    int m_HistoryUpdateInterval;
    bool m_bBrighterRegions; // detect brighter or darker pixels rel. to the background.
    int m_ContourVertexes;
    BlobConfigPtr m_pTouch;
    BlobConfigPtr m_pTrack;

    bool m_bCreateDebugImages;
    bool m_bCreateFingerImage;

    DeDistortPtr m_pTrafo;

private:
    void loadCamera(xmlNodePtr pParentNode, const std::string& sFilename, bool bOnlyDyn);
    void loadTracker(xmlNodePtr pParentNode, const std::string& sFilename);
    std::string getConfigFilename();
    xmlDocPtr m_Doc;
    xmlNodePtr m_pRoot;
};

}
#endif
