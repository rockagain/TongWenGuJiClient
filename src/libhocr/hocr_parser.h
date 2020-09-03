//
// Created by lihuanpeng on 17-8-21.
//

#ifndef OCR_SERVICE_HOCRPARSER_H
#define OCR_SERVICE_HOCRPARSER_H

#include <vector>
#include "hocr_types.h"
#include "hocr_node.h"
#include "tinyxml2/tinyxml2.h"

using namespace tinyxml2;

namespace hocr {
    class HocrParser {
    public:
        HocrParser();

        ~HocrParser();

        bool Parse(const char *source);

        bool Parse(HocrNode *root);

        HocrNode *GetPageNode();

        string GetHocrString();

    private:
        HocrNode *page;
        XMLDocument *doc;

        hocr::properties SplitProperties(const string &source);

        void ReadAllElements(XMLElement *ele, HocrNode *parent);

        void ReadAttrs(XMLElement *ele, HocrNode *node);

        XMLElement *GenXMLElement(HocrNode *node);

        void ReadNodes(HocrNode *node, XMLNode *parent);

        string FindElementName(HocrNode *node);
    };
}

#endif //OCR_SERVICE_HOCRPARSER_H
