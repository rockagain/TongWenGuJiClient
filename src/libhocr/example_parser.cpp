//
// Created by lihuanpeng on 17-10-25.
//

#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
#include "hocr_parser.h"

using namespace std;
using namespace hocr;

// 测试解析xml文本到hocr对象
HocrNode *xml_to_hocr(const string &file) {
    // read sample file to string
    ifstream infile;
    infile.open(file);
    assert(infile.is_open());

    string hocr, tmp;
    while (getline(infile, tmp)) {
        hocr.append(tmp);
        hocr.append("\n");
    }
    infile.close();

    auto *parser = new HocrParser;
    // 循环把node对象打印出来检查
    if (parser->Parse(hocr.c_str())) {
        HocrNode *rootNode = parser->GetPageNode();
        return rootNode;
        for (auto node : rootNode->GetChildren()) {
            cout << node->GetNodeClass()
                 << " | "
                 << node->GetChildren().size()
                 << endl;
            for (auto _node : node->GetChildren()) {
                cout << _node->GetNodeClass()
                     << " | "
                     << _node->GetChildren().size()
                     << endl;
                for (auto __node : _node->GetChildren()) {
                    cout << __node->GetNodeClass()
                         << " | "
                         << __node->GetChildren().size()
                         << endl;
                    for (auto ___node : __node->GetChildren()) {
                        cout << ___node->GetNodeClass()
                             << " | "
                             << ___node->GetChildren().size()
                             << " | "
                             << ___node->GetCharacter()
                             << endl;
                    }
                }
            }
        }
//        return rootNode;
    }

    return nullptr;
}

// 测试hocr对象转xml文本
void hocr_to_xml(HocrNode *root_node) {
    auto *parser = new HocrParser;
    if (parser->Parse(root_node)) {
        string hocr_str = parser->GetHocrString();
        cout << hocr_str << endl;
    }
}

int main(int argc, char **argv) {

    HocrNode *node = xml_to_hocr("/home/lihuanpeng/Workspace/ocr-service/res/hocr_sample_2.html");
    hocr_to_xml(node);
}
