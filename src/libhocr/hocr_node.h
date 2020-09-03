//
// Created by lihuanpeng on 17-8-17.
//
// 文档节点的基类，所有节点类都继承自此类，享有一些通用方法
// 继承类可以重载所有方法以根据情况实现自己的私有特性
// 特性：
// 这是一个“实现接口”，不是一个纯“继承接口”，里面的方法都是“虚函数”，提供了缺省实现，在使用
// 缺省方法的情况下，子类直接调用就可以，无需再次声明。如果要重载某些方法，就要先在子类中
// 以 override 方式声明，并在CPP文件里实现即可。
//

#ifndef OCR_SERVICE_NODE_H
#define OCR_SERVICE_NODE_H

#include <vector>
#include <QString>
#include "hocr_types.h"

namespace hocr {
    class HocrNode {
    public:
        // 构造/析构方法
        HocrNode();

        explicit HocrNode(HocrNode *parent);

        ~HocrNode();

        //========================================================================
        // 通用Get类方法集合，获得元素的属性
        //========================================================================
        hocr::properties GetProperties();

        string GetNodeClass();

        string GetNodeId();

        string GetLang();

        hocr::baseline GetBaseLine();

        string GetImage();

        string GetImageMd5();

        string GetLogicPageNo();

        string GetPhysicPageNo();

        hocr::scanres GetScanres();

        float GetXWconf();

        hocr::bbox GetBbox();

        string GetBboxStr();

        string GetCharacter();

        string GetXFont();

        HocrNode *getParent();

        //========================================================================
        // 通用Set类方法集合，设置元素属性的值
        //========================================================================
        void SetParent(HocrNode *parent);

        void SetProperties(hocr::properties properties);

        void SetNodeClass(string node_class);

        void SetNodeId(string node_id);

        void SetLang(string lang);

        void SetBaseLine(hocr::baseline baseline);

        void SetBbox(hocr::bbox bbox);

        void SetBbox(uint32_t left, uint32_t top,
                     uint32_t right, uint32_t bottom);

        void SetImage(string image);

        void SetImageMd5(string imagemd5);

        void SetLogicPageNo(string lpageno);

        void SetPhysicPageNo(string ppageno);

        void SetScanres(hocr::scanres resolution);

        void SetXWconf(int x_wconf);

        void SetCharacter(string character);

        void SetXFont(string font_name);

        //========================================================================
        // 对此节点子元素的操作
        //========================================================================
        // 添加一个子元素
        void AddChild(HocrNode *child);

        // 获得此节点所有子元素的集合
        std::vector<HocrNode *> GetChildren();

        void Clear();

        //========================================================================
        // 高阳加的两个变量,因为放在private里面还要加getter和setter所以放在public里面了
        //========================================================================
        // 加载那里需要记录，而我的item都是新生成的
        bool hasPuncNode;

        string hasWhatPunc;

    private:
        //========================================================================
        // 公共变量
        //========================================================================
        // 当前节点对象的父节点
        HocrNode *parent = nullptr;

        // 此节点的子元素
        std::vector<HocrNode *> children;

        // 元素的属性（特性）结构，所有元素都继承同样一个描述特性的结构体
        // 注意：有的元素不包含某些特性，这种情况下进行赋值和取值就是了
        hocr::properties properties;

    };
}

#endif //OCR_SERVICE_NODE_H
