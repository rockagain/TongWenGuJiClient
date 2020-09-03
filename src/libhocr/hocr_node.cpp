//
// Created by lihuanpeng on 17-8-17.
//

#include "hocr_node.h"

using namespace std;
using namespace hocr;
/**
 * 构造方法
 * @param parent
 */
HocrNode::HocrNode() = default;

HocrNode::HocrNode(HocrNode *parent) : parent(parent = nullptr) {
    this->parent = parent;
}

HocrNode::~HocrNode() = default;

/**
 * 设置当前节点的父节点
 * @param Node* parent
 */
void HocrNode::SetParent(HocrNode *parent) {
    this->parent = parent;
}

//============================================================================
// 通用 Get 类方法集合
//============================================================================
/**
 * 获得包含这个节点所有属性的结构体
 * @return properties_t 类型的结构体，包含节点的所有属性
 */
hocr::properties HocrNode::GetProperties() {
    return properties;
}

/**
 * 获得节点的lang（语言）属性的值
 * @return string lang 节点的语言标识
 */
string HocrNode::GetLang() {
    if (!properties.lang.empty())
        return properties.lang;

    return "";
}

/**
 * 获得节点元素的bbox属性结构体
 * @return bbox_t bbox
 */
hocr::bbox HocrNode::GetBbox() {
    return properties.bbox;
}

/**
 * 获得bbox属性的字符串形式
 * @return
 */
string HocrNode::GetBboxStr() {
    string s;
    hocr::bbox _bbox = GetBbox();
    s.append("bbox ");
    s.append(to_string(_bbox.left) + " ");
    s.append(to_string(_bbox.top) + " ");
    s.append(to_string(_bbox.right) + " ");
    s.append(to_string(_bbox.bottom));
    return s;
}

/**
 * 获得节点的class属性
 * @return string node_class 节点的class属性
 */
string HocrNode::GetNodeClass() {
    return properties.node_class;
}

/**
 * 获得节点的id属性
 * @return string node_id 节点的id属性
 */
string HocrNode::GetNodeId() {
    return properties.node_id;
}

/**
 * 获得节点的基线属性
 * @return baseline_t 节点的基线属性
 */
hocr::baseline HocrNode::GetBaseLine() {
    return properties.baseline;
}

/**
 * 获得元素的image属性
 * @return string 元素的image地址
 */
string HocrNode::GetImage() {
    return properties.image;
}

/**
 * 获得元素的 imagemd5属性
 * @return string 元素的imagemd5属性
 */
string HocrNode::GetImageMd5() {
    return properties.imagemd5;
}

/**
 * 获得页面的逻辑页码
 * @return string 页面的逻辑页码 如：‘IV’ 或 ‘5’
 */
string HocrNode::GetLogicPageNo() {
    return properties.lpageno;
}

/**
 * 获得页面的物理页码
 * @return string 页面的物理页码
 */
string HocrNode::GetPhysicPageNo() {
    return properties.ppageno;
}

/**
 * 获得页面的扫描分辨率
 * @return scanres_t 页面的扫描分辨率
 */
hocr::scanres HocrNode::GetScanres() {
    return properties.scanres;
}

/**
 * 获得字词的识别信心度
 * @return float 字词的识别信心度，相当于后验概率，0-100之间的浮点值，单位（%）
 */
float HocrNode::GetXWconf() {
    return properties.x_wconf;
}

/**
 * 获得word对象的中文单字
 * @return
 */
string HocrNode::GetCharacter() {
    return properties.character;
}

/**
 * @brief HocrNode::GetXFont
 * @return
 */
string HocrNode::GetXFont()
{
    return properties.x_font;
}

//============================================================================
// 通用 Set 类方法集合
//============================================================================

/**
 * 设置元素的属性集合
 * @param properties_t properties 包含元素属性和结构体
 */
void HocrNode::SetProperties(hocr::properties properties) {
    this->properties = std::move(properties);
}

/**
 * 设置节点的class属性
 * @param string node_class
 */
void HocrNode::SetNodeClass(string node_class) {
    properties.node_class = std::move(node_class);
}

/**
 * 设置元素的id属性
 * @param node_id string node_id
 */
void HocrNode::SetNodeId(string node_id) {
    properties.node_id = std::move(node_id);
}

/**
 * 设置元素的语言属性
 * @param string lang 语言，如：eng, chi_sim, chi_tra ...
 */
void HocrNode::SetLang(string lang) {
    properties.lang = std::move(lang);
}

/**
 * 设置元素的基线属性
 * @param baseline_t baseline 元素的基线属性结构体
 * @see baseline_t
 */
void HocrNode::SetBaseLine(hocr::baseline baseline) {
    properties.baseline = baseline;
}

/**
 * 给元素的bbox属性赋值
 * @param bbox
 * @see bbox_t
 */
void HocrNode::SetBbox(hocr::bbox bbox) {
    properties.bbox = bbox;
}

/**
 * 给元素的bbox属性赋值
 * @param left 元素左边距离页面最左边的距离
 * @param top 元素上边距离页面最上边的距离
 * @param right 元素右边距离页面最左边的距离
 * @param bottom 元素底边距离页面最上边的距离
 * @see bbox_t
 */
void HocrNode::SetBbox(uint32_t left, uint32_t top,
                       uint32_t right, uint32_t bottom) {
    properties.bbox.left = left;
    properties.bbox.top = top;
    properties.bbox.right = right;
    properties.bbox.bottom = bottom;
}

/**
 * 设置元素的image属性
 * @param string image 元素中image的地址
 */
void HocrNode::SetImage(string image) {
    properties.image = std::move(image);
}

/**
 * 设置元素的imagemd5属性
 * @param string imagemd5 元素中image的md5值
 */
void HocrNode::SetImageMd5(string imagemd5) {
    properties.imagemd5 = std::move(imagemd5);
}

/**
 * 设置页面的逻辑页码
 * @param string lpageno 页面的逻辑页码，如："IV" 或 "5"
 */
void HocrNode::SetLogicPageNo(string lpageno) {
    properties.lpageno = std::move(lpageno);
}

/**
 * 设置页面的物理页码
 * @param string ppageno 页面的逻辑页码，如 "10"
 */
void HocrNode::SetPhysicPageNo(string ppageno) {
    properties.ppageno = std::move(ppageno);
}

/**
 * 设置页面的扫描分辨率
 * @param scanres_t resolution 页面的扫描分辨率
 * @see scanres_t
 */
void HocrNode::SetScanres(hocr::scanres resolution) {
    properties.scanres = resolution;
}

/**
 * 设置字词的识别信心度
 * @param float x_wconf 字词的识别信心度，相当于后验概率，0-100之间的浮点值，单位（%）
 */
void HocrNode::SetXWconf(int x_wconf) {
    properties.x_wconf = x_wconf;
}

/**
 * 设置word属性中识别的中文单字
 * @param character
 */
void HocrNode::SetCharacter(string character) {
    properties.character = std::move(character);
}

/**
 * @brief HocrNode::SetXFont
 * @param font_name
 */
void HocrNode::SetXFont(string font_name)
{
    properties.x_font = std::move(font_name);
}

//============================================================================
// 对此节点子元素的操作
//============================================================================
/**
 * 向子元素变长数组尾部追加一个子元素
 * @param child
 */
void HocrNode::AddChild(HocrNode *child) {
    // todo: 重构时在这里要加入一些限制，判断子节点的属性是否非法，并做一些自动修正
    children.push_back(child);
    child->SetParent(this);
}

/**
 * 获得此节点所有子元素的集合
 * @return vector<Node *> child_elements
 */
std::vector<HocrNode *> HocrNode::GetChildren() {
    return children;
}

/**
 * 清除此节点的所有子节点
 */
void HocrNode::Clear() {
    // 清除子节点，但不回收空间
    // todo: 清除时最好是一起回收内存，否则长时间运行可能导致内存不足
    children.clear();
}

HocrNode *HocrNode::getParent() {
    return parent;
}
