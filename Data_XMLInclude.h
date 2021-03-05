#ifndef DATA_XMLINCLUDE_H
#define DATA_XMLINCLUDE_H

#include <qglobal.h>

#ifdef XMLTRANSDLL_LIB
# define XMLTRANSDLL_EXPORT Q_DECL_EXPORT
#else
# define XMLTRANSDLL_EXPORT Q_DECL_IMPORT
#endif

#include <QString>
#include <QVector>


#define XML_SUCCESS	0			// 成功
#define XML_FAILURE	-1			// 失败

//! 文件读写管理类


//! 单个的节点类
class XMLTRANSDLL_EXPORT CXMLNodeEX
{
public:
	CXMLNodeEX();
	~CXMLNodeEX();

public:

	//! 设置节点键名
	void SetName(const QString &strName);

	//! 设置节点键值
	void SetValue(const QString &strValue);

	//! 获取节点键名
	QString GetName();

	//! 获取节点键值
	QString GetValue();

	//! 获取节点属性数量
	int GetAttributeNum();

	//! 增加节点属性
	void AddAttribute(const QString &strName, const QString &strValue);

	//! 根据节点键名获取对应的键值
	int GetAttributeValueByName(const QString &strName, QString &strValue);

	//! 根据节点键名修改对应的键值
	int AlterAttrbuteByName(const QString &strName, const QString &strValue);

	//! 根据索引获取对应节点的键名、键值对
	int GetAttributeByIndex(int iIndex, QString &strName, QString &strValue);

	//! 根据节点键名删除对应的节点
	int DelAttribute(const QString &strName);

	//! 清空所有的内容
	void ClearAll();

protected:
	QString m_strName;
	QString m_strValue;
	QVector<QString> m_vecAttributeNames;
	QVector<QString> m_vecAttributeValues;
};


//! XML文件对象类
class XMLTRANSDLL_EXPORT CXMLContent
{
public:
	CXMLContent();
	~CXMLContent();

public:
	//! 获得子节点的数量
	int GetSubNum();
	//! 根据子节点名获得子数量
	int GetSubNum(const QString &strName);

	//! 添加子节点
	CXMLContent *AddSub(const QString &strName, const QString &strValue = "");

	//! 根据节点名获取值
	int GetValueByName(const QString &strName, QString &strValue);

	//! 根据节点名和索引值查找子节点
	CXMLContent *FindSubContent(const QString &strName, int iIndex = 0);

	//! 删除子节点		若名称为空或索引值为-1则删除所有子节点
	int DelSub(const QString &strName, int iIndex = 0);

	//! 删除全部
	void DelAll();

	//! 根据序号获得子内容
	CXMLContent *GetSubByIndex(int iIndex);

	//! 设置名称
	void SetName(const QString &strName);

	//! 设置值
	void SetValue(const QString &strValue);

	//! 添加属性
	void AddAttribute(const QString &strName, const QString &strValue);

	//! 获得名称
	QString GetName();

	//! 获得值
	QString GetValue();

	//! 获得属性的个数
	int GetAttributeNum();

	//! 获取属性值
	int GetAttributeValueByName(const QString &strName, QString &strValue);

	//! 根据索引获取属性数据
	int GetAttributeByIndex(int iIndex, QString &strName, QString &strValue);

	//! 修改对应属性值
	int AlterAttributeValueByName(const QString &strName, const QString &strValue);

	//! 删除属性
	int DelAttribute(const QString &strName);

	//! 设置注释
	void SetComment(const QString &strComment);

	//! 获取注释
	QString GetComment();

protected:
	QVector<CXMLContent *> m_vecSubContets;		// 子节点
	CXMLNodeEX m_selfContent;						// 自身的数据
	QString m_strComment;							// 注释
};

//! XML文档处理基类
class CXMLTransInterface
{
public:
	//! 获得内容 
	virtual int GetValue(CXMLContent &xmlContent) = 0;

	//! 创建内容
	virtual QString CreateValue( CXMLContent &xmlContent) = 0;

	//! 设置XML文档的内容
	virtual void SetXmLString(const QString &strXMLFile) = 0;

	//! 设置是否是文件操作
	virtual void SetFileFlag(bool bIsFile) = 0;

	// 设置编码格式
	virtual void SetCode(const QString &strCode) = 0;
};


class XMLTRANSDLL_EXPORT CXMLFactory
{
public:
	static CXMLTransInterface *GetTracePt();
	static void RelTracePt();
};

#endif