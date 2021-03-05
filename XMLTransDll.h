#ifndef XMLTRANSDLL_H
#define XMLTRANSDLL_H

#include <QDomDocument>
#include <QDomElement>
#include "Data_XMLInclude.h"

//! XML文档处理类
class XMLTransDll : public CXMLTransInterface
{
public:
    XMLTransDll();
    ~XMLTransDll();

    //! 拷贝构造函数
    XMLTransDll(const XMLTransDll &otherLog);

    //! 运算符重载函数
    XMLTransDll &operator=(const XMLTransDll &oterhLog);

    //! 获得内容    0：成功      1：不是对文件进行操作     2：文件读取失败
    virtual int GetValue(CXMLContent &xmlContent);

    //! 创建内容
    virtual QString CreateValue(CXMLContent &xmlContent);

    //! 设置XML文档的内容
    virtual void SetXmLString(const QString &strXMLFile);

    //! 设置是否是文件操作
    virtual void SetFileFlag(bool bIsFile);

    // 设置编码格式
    virtual void SetCode(const QString &strCode);

protected:
    //! 创建子节点
    int CreateSubContent(QDomElement &pxParElePt, CXMLContent &xmlContent);

    //! 获取子节点的内容
    int GetSubContent(const QDomElement &pxParElePt, CXMLContent &xmlContent);

public:
    QDomElement m_rootElement;      // XML文件根节点
    QDomDocument *m_pDoc;           // XML文档
    QString m_strXMLContent;        // XML字符串的内容
    bool m_bIsFileFlag;             // 是否对文件进行操作
    QString m_strCode;              // 编码格式
};

#endif // XMLTRANSDLL_H
