#include "XMLTransDll.h"

#include <QFile>
#include <QTextStream>

/************************************************************XML文档处理类**********************************************************/
XMLTransDll::XMLTransDll()
{
    m_pDoc = nullptr;
    m_bIsFileFlag = false;
    m_strCode = "utf-8";
}

XMLTransDll::~XMLTransDll()
{

}

XMLTransDll::XMLTransDll(const XMLTransDll &otherLog)
{
    *this = otherLog;
}

XMLTransDll &XMLTransDll::operator=(const XMLTransDll &oterhLog)
{
    // 这里的copyConstruct函数实际上已经失去了意义
    if (&oterhLog == this) {
        return *this;
    }

    return *this;
}

int XMLTransDll::GetValue(CXMLContent &xmlContent)
{
    m_pDoc = new QDomDocument;

    if (m_bIsFileFlag) {
        QFile file(m_strXMLContent);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            delete m_pDoc;
            m_pDoc = nullptr;
            return XML_FAILURE;
        }

        if (!m_pDoc->setContent(&file)) {
            delete m_pDoc;
            m_pDoc = nullptr;
            return XML_FAILURE;
        }
    } else {
        delete m_pDoc;
        m_pDoc = nullptr;
        return XML_FAILURE;
    }

    m_rootElement = m_pDoc->documentElement();
    GetSubContent(m_rootElement, xmlContent);
    return XML_SUCCESS;
}

QString XMLTransDll::CreateValue(CXMLContent &xmlContent)
{
    if (!m_bIsFileFlag) {   // 如果不是对文件进行操作，则先将xml文档内容清空
        m_strXMLContent.clear();
    }

    QFile file(m_strXMLContent);

    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        return "";
    }

    m_pDoc = new QDomDocument;

    QDomProcessingInstruction instruction;
    instruction = m_pDoc->createProcessingInstruction("xml", QString("version=\"1.0\" encoding=\"%1\"").arg(m_strCode));
    m_pDoc->appendChild(instruction);

    QDomComment currenttext = m_pDoc->createComment(m_strXMLContent);
    m_pDoc->appendChild(currenttext);

    QDomElement root;

    QString strName = xmlContent.GetName();
    if (strName == "") {
        root = m_pDoc->createElement("数据配置");
    } else {
        root = m_pDoc->createElement(strName);
    }

    m_pDoc->appendChild(root);

    m_rootElement = m_pDoc->documentElement();
    CreateSubContent(m_rootElement, xmlContent);

    if (m_bIsFileFlag) { // 如果是对文件操作，则保存到对应文档中
        QTextStream out(&file);
        m_pDoc->save(out, 4);
    }

    delete m_pDoc;
    m_pDoc = nullptr;

    return m_strXMLContent;
}

void XMLTransDll::SetXmLString(const QString &strXMLFile)
{
    m_strXMLContent = strXMLFile;
}

void XMLTransDll::SetFileFlag(bool bIsFile)
{
    m_bIsFileFlag = bIsFile;
}

void XMLTransDll::SetCode(const QString &strCode)
{
    m_strCode = strCode;
}

int XMLTransDll::CreateSubContent(QDomElement &pxParElePt, CXMLContent &xmlContent)
{
    QString strAttributeName = "";
    QString strAttributeValue = "";

    for (int i = 0; i < xmlContent.GetAttributeNum(); i++) {
        xmlContent.GetAttributeByIndex(i, strAttributeName, strAttributeValue);
        pxParElePt.setAttribute(strAttributeName, strAttributeValue);
    }

    // 创建子节点
    for (int i = 0; i < xmlContent.GetSubNum(); i++) {
        CXMLContent *xmlSubContent = xmlContent.GetSubByIndex(i);

        QDomElement newSubElement = m_pDoc->createElement(xmlSubContent->GetName());    // 获取XML文档处理类对象中各个属性键值对

        QString strValue = xmlSubContent->GetValue();
        if (!strValue.isEmpty()) {
            QDomText currentText = m_pDoc->createTextNode(strValue);
            newSubElement.appendChild(currentText);
        }

        // 增加注释
        QString strComment = xmlSubContent->GetComment();
        if (!strComment.isEmpty()) {
            QDomComment currentComment = m_pDoc->createComment(strComment);
            pxParElePt.appendChild(currentComment);
        }

        CreateSubContent(newSubElement, *(xmlSubContent));      // 设置XML文档各节点属性键值对
        pxParElePt.appendChild(newSubElement);
    }

    return XML_SUCCESS;
}

int XMLTransDll::GetSubContent(const QDomElement &pxParElePt, CXMLContent &xmlContent)
{
    if (!pxParElePt.isNull()) {
        if (pxParElePt.nodeType() == QDomNode::ElementNode) {
            xmlContent.SetName(pxParElePt.tagName());

            QString str = pxParElePt.text();
            int iCount = pxParElePt.childNodes().count();
            if (!pxParElePt.text().isEmpty() && iCount == 1) {
                xmlContent.SetValue(pxParElePt.text());
            }

            QDomNamedNodeMap attributes = pxParElePt.attributes();

            for (int i = 0; i < attributes.length(); i++) {
                xmlContent.AddAttribute(attributes.item(i).nodeName(), attributes.item(i).nodeValue());
            }

            QDomElement child = pxParElePt.firstChildElement();

            while (!child.isNull()) {
                if (child.nodeType() == QDomNode::ElementNode) {
                    // 第一种
                    CXMLContent *pSubXMLContent = xmlContent.AddSub(child.tagName());
                    GetSubContent(child, *pSubXMLContent);
//                  delete pSubXMLContent;
//                  pSubXMLContent = nullptr;

                    // 第二种
                    //GetSubContent(child, *(xmlContent.AddSub(child.tagName())));
                }

                child = child.nextSiblingElement();
            }
        }
    }

    return XML_SUCCESS;
}

/************************************************************单个的节点类**********************************************************/
CXMLNodeEX::CXMLNodeEX()
{

}

CXMLNodeEX::~CXMLNodeEX()
{

}

void CXMLNodeEX::SetName(const QString &strName)
{
    m_strName = strName;
}

void CXMLNodeEX::SetValue(const QString &strValue)
{
    m_strValue = strValue;
}

QString CXMLNodeEX::GetName()
{
    return m_strName;
}

QString CXMLNodeEX::GetValue()
{
    return m_strValue;
}

int CXMLNodeEX::GetAttributeNum()
{
    return m_vecAttributeNames.count();
}

void CXMLNodeEX::AddAttribute(const QString &strName, const QString &strValue)
{
    QString strValueTemp = strValue;

    if (GetAttributeValueByName(strName, strValueTemp) == XML_SUCCESS) {    // 如果已经存在，则不添加
        return;
    }

    m_vecAttributeNames.push_back(strName);
    m_vecAttributeValues.push_back(strValue);
}

int CXMLNodeEX::GetAttributeValueByName(const QString &strName, QString &strValue)
{
    for (int i = 0; i < m_vecAttributeNames.count(); i++) {
        if (m_vecAttributeNames[i] == strName) {
            strValue = m_vecAttributeValues[i];
            return XML_SUCCESS;
        }
    }

    return XML_FAILURE;
}

int CXMLNodeEX::AlterAttrbuteByName(const QString &strName, const QString &strValue)
{
    for (int i = 0; i < m_vecAttributeNames.count(); i++) {
        if (m_vecAttributeNames[i] == strName) {
            m_vecAttributeValues[i] = strValue;
            return XML_SUCCESS;
        }
    }

    return XML_FAILURE;
}

int CXMLNodeEX::GetAttributeByIndex(int iIndex, QString &strName, QString &strValue)
{
    if (iIndex < 0 || iIndex >= m_vecAttributeNames.count()) {
        return XML_FAILURE;
    }

    strName = m_vecAttributeNames[iIndex];
    strValue = m_vecAttributeValues[iIndex];

    return XML_SUCCESS;
}

int CXMLNodeEX::DelAttribute(const QString &strName)
{
    QVector<QString>::iterator iterName = m_vecAttributeNames.begin();
    QVector<QString>::iterator iterValue = m_vecAttributeValues.begin();

    // 遍历节点键值对列表
    while (iterName != m_vecAttributeNames.end() && iterValue != m_vecAttributeValues.end()) {
        if ((*iterName) == strName) {
            m_vecAttributeNames.erase(iterName);
            m_vecAttributeValues.erase(iterValue);
            return XML_SUCCESS;
        }

        iterName++;
        iterValue++;
    }

    return XML_FAILURE;
}

void CXMLNodeEX::ClearAll()
{
    m_strName = "";
    m_strValue = "";
    m_vecAttributeNames.clear();
    m_vecAttributeValues.clear();
}

/************************************************************XML文件对象类**********************************************************/
CXMLContent::CXMLContent()
{
    m_strComment = "";
}

CXMLContent::~CXMLContent()
{
    DelAll();
}

int CXMLContent::GetSubNum()
{
    return m_vecSubContets.count();
}

int CXMLContent::GetSubNum(const QString &strName)
{
    int iNum = 0;

    for (int i = 0; i < m_vecSubContets.count(); i++) {
        if (m_vecSubContets[i]->m_selfContent.GetName() == strName) {
            iNum++;
        }
    }

    return iNum;
}

CXMLContent *CXMLContent::AddSub(const QString &strName, const QString &strValue /*= ""*/)
{
    CXMLContent *pNewXMLContent = new CXMLContent;

    pNewXMLContent->m_selfContent.SetName(strName);
    pNewXMLContent->m_selfContent.SetValue(strValue);
    m_vecSubContets.push_back(pNewXMLContent);

    return pNewXMLContent;
}

int CXMLContent::GetValueByName(const QString &strName, QString &strValue)
{
    strValue.clear();

    CXMLContent *pNode = FindSubContent(strName);

    if (pNode == nullptr) {
        return XML_FAILURE;
    }

    strValue = pNode->m_selfContent.GetValue();

    return XML_SUCCESS;
}

CXMLContent *CXMLContent::FindSubContent(const QString &strName, int iIndex /*= 0*/)
{
    int iNum = 0;

    for (int i = 0; i < m_vecSubContets.count(); i++) {
        if (m_vecSubContets[i]->m_selfContent.GetName() == strName) {
            if (iNum == iIndex) {
                return m_vecSubContets[i];
            }
            iNum++;
        }
    }

    return nullptr;
}

int CXMLContent::DelSub(const QString &strName, int iIndex)
{
    // 节点名为nullptr或索引值为-1则删除所有子节点
    if (strName == "" || iIndex == -1) {
        QVector<CXMLContent *>::iterator iter = m_vecSubContets.begin();

        while (iter != m_vecSubContets.end()) {
            delete (*iter);
            iter++;
        }
        m_vecSubContets.clear();

        return XML_SUCCESS;
    }

    int iNum = 0;
    QVector<CXMLContent *>::iterator iter = m_vecSubContets.begin();

    // 遍历所有子节点，根据节点名和索引值删除子节点
    while (iter != m_vecSubContets.end()) {
        if ((*iter)->m_selfContent.GetName() == strName) {
            if (iNum == iIndex) {
                delete (*iter);
                m_vecSubContets.erase(iter);

                return XML_SUCCESS;
            }

            iNum++;
        }

        iter++;
    }

    return XML_FAILURE;
}

void CXMLContent::DelAll()
{
    m_selfContent.ClearAll();

    for (int i = 0; i < m_vecSubContets.count(); i++) {
        delete m_vecSubContets[i];
    }

    m_vecSubContets.clear();
}

CXMLContent *CXMLContent::GetSubByIndex(int iIndex)
{
    if (iIndex >= 0 && iIndex < m_vecSubContets.count()) {
        return m_vecSubContets[iIndex];
    }

    return nullptr;
}

void CXMLContent::SetName(const QString &strName)
{
    m_selfContent.SetName(strName);
}

void CXMLContent::SetValue(const QString &strValue)
{
    m_selfContent.SetValue(strValue);
}

void CXMLContent::AddAttribute(const QString &strName, const QString &strValue)
{
    m_selfContent.AddAttribute(strName, strValue);
}

QString CXMLContent::GetName()
{
    return m_selfContent.GetName();
}

QString CXMLContent::GetValue()
{
    return m_selfContent.GetValue();
}

int CXMLContent::GetAttributeNum()
{
    return m_selfContent.GetAttributeNum();
}

int CXMLContent::GetAttributeValueByName(const QString &strName, QString &strValue)
{
    return m_selfContent.GetAttributeValueByName(strName, strValue);
}

int CXMLContent::GetAttributeByIndex(int iIndex, QString &strName, QString &strValue)
{
    return m_selfContent.GetAttributeByIndex(iIndex, strName, strValue);
}

int CXMLContent::AlterAttributeValueByName(const QString &strName, const QString &strValue)
{
    return m_selfContent.AlterAttrbuteByName(strName, strValue);
}

int CXMLContent::DelAttribute(const QString &strName)
{
    return m_selfContent.DelAttribute(strName);
}
void CXMLContent::SetComment(const QString &strComment)
{
    m_strComment = strComment;
}

QString CXMLContent::GetComment()
{
    return m_strComment;
}

/************************************************************XML文件对象类**********************************************************/
XMLTransDll *g_xmlTrans = nullptr;


CXMLTransInterface *CXMLFactory::GetTracePt()
{
    if (g_xmlTrans == nullptr) {
        g_xmlTrans = new XMLTransDll;
    }

    return g_xmlTrans;
}

void CXMLFactory::RelTracePt()
{
    if (g_xmlTrans != nullptr) {
        delete g_xmlTrans;
        g_xmlTrans = nullptr;
    }
}
