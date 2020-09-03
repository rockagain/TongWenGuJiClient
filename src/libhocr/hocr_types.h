//
// Created by lihuanpeng on 17-8-16.
// 全局的类型定义
//

#ifndef OCR_SERVICE_BASE_TYPE_H
#define OCR_SERVICE_BASE_TYPE_H

#include <cstdint>
#include <string>

using std::string;

namespace hocr {
    // 文本行的坐标描述
    struct bbox {
        int left;      // 元素左边距离页面最左边的距离
        int top;       // 元素上边距离页面最上边的距离
        int right;     // 元素右边距离页面最左边的距离
        int bottom;    // 元素底边距离页面最上边的距离
    };

    // 文本行基线
    struct baseline {
        float angle;      // 元素基线相对于水平基线的逆时针方向旋转角度
        int crosses;      // 元素基线的最左边偏离垂直Y轴的距离
    };

    // 扫描图像的分辨率
    struct scanres {
        int x;    // X轴的分辨率
        int y;    // Y轴的分辨率
    };

    // TODO: 2017/8/18, 李焕朋：将来在这个properties_t结构中添加hOCR规范的所有特性
    struct properties {
        string node_class;   // 节点类型
        string node_id;      // 元素id
        string lang;         // 语言
        // 以下是存在于title属性中的
        hocr::baseline baseline;      // 元素基线
        hocr::bbox bbox;              // 节点元素盒模型
        string image;        // 文档图像地址
        string imagemd5;     // 图像地址MD5
        string lpageno;      // 页面的逻辑页码
        string ppageno;      // 页面的物理页码
        hocr::scanres scanres;        // 页面扫描的分辨率
        // 以下是特定与ocr_word对象的属性
        int x_wconf;            // 识别引擎特性：字、词的准确信息，类似一个后验概率
        string x_font;    // 字体标识 双行：font_double_row
        string character;    // 中文单字
        // string word;         // 两个以上单字组成的词
        // string sentence;     // 两个以上词组成的短语、句子
        // string section;      // 多个短语、句子组成的段落
    };

    // 版面元素类型
    enum {
        DEFAULT = 0x00,
        SINGLE_ROW,
        DOUBLE_ROW,
        BIG_CHAR,
        SMALL_CHAR,
        WORD,
        IMAGE,
        TABLE,
        ANNOTATION,
        MAIN_BODY,
        PUNC
    };
}

#endif //OCR_SERVICE_BASE_TYPE_H
