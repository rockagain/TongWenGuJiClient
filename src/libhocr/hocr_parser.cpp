//
// Created by lihuanpeng on 17-8-21.
// todo: 需要增加操作TinyXML元素时的一些错误验证，防止程序崩溃
//

#include <string>
#include <iostream>
#include <cstring>
#include <sstream>
#include "hocr_parser.h"

using namespace std;
using namespace hocr;

HocrParser::HocrParser() {
    // 创建page节点对象，这是目前hocr的顶级节点
    page = new HocrNode;
    // 创建TinyXML对象
    doc = new XMLDocument;
};

HocrParser::~HocrParser() {
    delete (page);
    delete (doc);
};

/**
 * 把hocr文本解析成为hocr_node对象
 * @param source
 * @return
 */
bool HocrParser::Parse(const char *source) {
    if (source == nullptr)
        return false;

#ifdef DEBUG_L2
    cout << "==== Parser input:" << endl;
    cout << source << endl;
#endif

    // clear doc object first
    doc->Clear();
    doc->Parse(source);
    // 根节点
    XMLElement *root = doc->RootElement();
    // 先解析根节点的属性并赋值给page对象
    page->Clear();
    ReadAttrs(root, page);
    // 建立递归入口节点
    XMLElement *ele_carea = root->FirstChildElement();
    // 递归扫描所有节点
    if (ele_carea) {
        ReadAllElements(ele_carea, page);
        return true;
    }

    return false;
}


/**
 * 递归读取xml全部兄弟节点
 * //todo: 递归是写正确了，但是在递归过程中分配hocr_node父子节点时我认为不正确但程序
 * 运行结果却是正确的，暂时这样，以后回过头来再仔细分析递归过程中node对象的分配细节
 * @param ele
 */
void HocrParser::ReadAllElements(XMLElement *ele, HocrNode *parent) {
    while (ele) {
        auto *node = new HocrNode;
        ReadAttrs(ele, node);
        parent->AddChild(node);

        // 如果当前节点是ocr_word节点，则需要提取出中文单字赋给node对象的属性
        if (strcmp(node->GetNodeClass().c_str(), "ocrx_word") == 0) {
            XMLNode *char_node = ele->FirstChild();
            if (char_node)
                node->SetCharacter(ele->FirstChild()->Value());
        }

        // 检查当前节点下是否存在子节点
        XMLElement *ele_child = ele->FirstChildElement();
        if (ele_child) {
            ReadAllElements(ele_child, node);
        }

        // 改变待处理节点为当前对象的下一个节点
        ele = ele->NextSiblingElement();
    }
}

/**
 * 递归读取xml元素的全部属性并将属性值赋给指定的hocr_node对象
 * @param ele
 */
void HocrParser::ReadAttrs(XMLElement *ele, HocrNode *node) {
    const XMLAttribute *attr = ele->FirstAttribute();
    while (attr) {
        const char *name = attr->Name();
        const char *value = attr->Value();

        if (strcmp(name, "class") == 0)
            node->SetNodeClass(value);

        if (strcmp(name, "id") == 0)
            node->SetNodeId(value);

        if (strcmp(name, "lang") == 0)
            node->SetLang(name);

        if (strcmp(attr->Name(), "title") == 0) {
            auto _prop = SplitProperties(value);
            node->SetBaseLine(_prop.baseline);
            node->SetBbox(_prop.bbox);
            node->SetImage(_prop.image);
            node->SetImageMd5(_prop.imagemd5);
            node->SetLogicPageNo(_prop.lpageno);
            node->SetPhysicPageNo(_prop.ppageno);
            node->SetScanres(_prop.scanres);
            node->SetXWconf(_prop.x_wconf);
            node->SetXFont(_prop.x_font);
        }

        attr = attr->Next();
    }
}

/**
 * 将string形式的title字符串分割解析成hocr的属性-值
 * For example: image ""; bbox 0 0 3600 4800; ppageno 0
 * @param source
 * @param target
 */
hocr::properties
HocrParser::SplitProperties(const string &source) {

    // 先分割分号，将image和bbox分离开
    vector<string> all_props;
    istringstream f(source);
    string s;
    while (getline(f, s, ';'))
        all_props.push_back(s);

    auto *target = new hocr::properties;
    // 循环分割子属性
    for (const auto &prop : all_props) {
        vector<string> props;
        istringstream p(prop);
        while (getline(p, s, ' ')) {
            if (strcmp(s.c_str(), " ") > 0) {
                props.push_back(s);
            }
        }

        // 分析image属性
        if (strcmp(props[0].c_str(), "image") == 0)
            target->image = props[1];

        // 分析ppageno属性
        if (strcmp(props[0].c_str(), "ppageno") == 0)
            target->ppageno = props[1];

        // 分析baseline属性
        if (strcmp(props[0].c_str(), "baseline") == 0) {
            baseline m_baseline = {
                    stof(props[1]),
                    stoi(props[2])
            };
            target->baseline = m_baseline;
        }

        // 分析x_wconf属性
        if (strcmp(props[0].c_str(), "x_wconf") == 0)
            target->x_wconf = stoi(props[1]);

        // 分析x_font属性
        if (strcmp(props[0].c_str(), "x_font") == 0)
            target->x_font = props[1];

        // 分析bbox属性
        if (strcmp(props[0].c_str(), "bbox") == 0) {
            bbox m_bbox = {0, 0, 0, 0};

            // left 和 top 的异常处理，如果越界则设置为0,即取最左边和最上边
            try {
                m_bbox.left   = stoi(props[1]);
            } catch (std::out_of_range &err) {
                std::cerr << err.what() << " Line:" << __LINE__ << " File:" << __FILE__ << endl;
                m_bbox.left   = 0;
            }

            try {
                m_bbox.top   = stoi(props[2]);
            } catch (std::out_of_range &err) {
                std::cerr << err.what() << " Line:" << __LINE__ << " File:" << __FILE__ << endl;
                m_bbox.top   = 0;
            }

            // todo: 为 right 和 bottom 也加上异常处理
            // 思路：如果right越界，则设置right值为图片的宽度值，即扩展到最右边但避免越界
            // bottom同理，如越界则设置bottom值为图片的高度
            m_bbox.right  = stoi(props[3]);
            m_bbox.bottom = stoi(props[4]);

            target->bbox = m_bbox;
        }
    }

    return *target;
}

/**
 * 获得page节点，即hocr的顶级节点
 * @return
 */
HocrNode *HocrParser::GetPageNode() {
    return this->page;
}

/**
 * 把hocr_node对象解析成为hocr格式的文本
 * @param root
 * @return
 */
bool HocrParser::Parse(HocrNode *root) {
    if (root->GetChildren().empty())
        return false;

    // clear doc object first
    doc->Clear();
    ReadNodes(root, doc);
    return true;
}

/**
 * 递归解析HocrNode对象，生成xml对象并建立父子关系
 * @param node
 * @param parent
 */
void HocrParser::ReadNodes(HocrNode *node, XMLNode *parent) {
    XMLNode *ele = GenXMLElement(node);
    parent->InsertEndChild(ele);

    for (auto child : node->GetChildren()) {
        ReadNodes(child, ele);
    }
}

/**
 * 获得解析得到的hocr文本
 * @return
 */
string HocrParser::GetHocrString() {
    auto *printer = new XMLPrinter;
    doc->Print(printer);
#ifdef DEBUG_L2
    cout << "==== Parser output:" << endl;
    cout << printer->CStr() << endl;
#endif
    return printer->CStr();
}

/**
 * 将指定的hocr_node对象生成为TinyXML2的节点
 * @param node
 * @return
 */
XMLElement *HocrParser::GenXMLElement(HocrNode *node) {
    if (strcmp(node->GetNodeClass().c_str(), "") == 0)
        return nullptr;

    XMLElement *ele = doc->NewElement(FindElementName(node).c_str());
    ele->SetAttribute("class", node->GetNodeClass().c_str());
    ele->SetAttribute("id", node->GetNodeId().c_str());

    // generate title strings
    string title;
    title.append(node->GetBboxStr());
    if (strcmp(node->GetNodeClass().c_str(), "ocrx_word") == 0) {
        title.append("; ");
        title.append("x_wconf " + to_string(node->GetXWconf()));
        ele->SetText(node->GetCharacter().c_str());
    }

    if (strcmp(node->GetNodeClass().c_str(), "ocr_par") == 0) {
        title.append("; ");
        ele->SetAttribute("lang", node->GetLang().c_str());
    }

    if (strcmp(node->GetNodeClass().c_str(), "ocr_line") == 0) {
        title.append("; x_font ");
        title.append(node->GetXFont());
        title.append(";");
    }

    ele->SetAttribute("title", title.c_str());

    return ele;
}

/**
 * 根据不同的node类型返回不同的xml元素类型
 * @param node
 * @return
 */
string HocrParser::FindElementName(HocrNode *node) {
    string node_class = node->GetNodeClass();
    string result = "div";

    if (strcmp(node_class.c_str(), "ocr_line") == 0 ||
        strcmp(node_class.c_str(), "ocrx_word") == 0)
        result = "span";

    if (strcmp(node_class.c_str(), "ocr_page") == 0 ||
        strcmp(node_class.c_str(), "ocr_carea") == 0)
        result = "div";

    if (strcmp(node_class.c_str(), "ocr_par") == 0)
        result = "p";

    return result;
}
