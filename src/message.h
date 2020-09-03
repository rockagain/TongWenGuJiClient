#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>


namespace Message {
const QString warning = QStringLiteral("警告");
const QString username_empty = QStringLiteral("请填写好账户名称！");
const QString code_empty = QStringLiteral("请填写好密码后再登陆!");
const QString error_4000 = QStringLiteral("错误提示");
const QString error_4001 = QStringLiteral("身份验证失败");
const QString error_4002 = QStringLiteral("用户名不能为空");
const QString error_4003 = QStringLiteral("密码不能为空");
}


#endif // MESSAGE_H
