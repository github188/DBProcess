#include "stdafx.h"
#include "AdoOracle.h"
//#include "../Utils/Log.h"
CAdoOracle::CAdoOracle()
{
	m_strIP = "";
	m_strDBName = "";
	m_strDBUser = "";
	m_strDBPwd = "";
	m_hCheck = NULL;
	m_hEvent = NULL;
	m_nCheckTime = 3000;
}
CAdoOracle::~CAdoOracle()
{
	Close();
}
/*****************************************************************************
    *  @brief    : 打开数据库，并保存连接信息
    *  @author   : chenyi
    *  @date     : 2012/02/02 18:53:55
    *  @version  : 
	*  @inparam  : strDBServerIP 数据库IP
	               strDBName     数据库名
				   strDBUser     数据库用户名
	               strDBPwd      数据库密码
    *  @outparam :  
    *  @return   : 成功返回true
	*  @note     : 
*****************************************************************************/
bool CAdoOracle::Open( const CString& strDBServerIP, const CString& strDBName, const CString& strDBUser, const CString& strDBPwd)
{
	autoWriteLock lock(&m_RWLock);
	m_strIP = strDBServerIP;
	m_strDBName = strDBName;
	m_strDBUser = strDBUser;
	m_strDBPwd  = strDBPwd;
	m_strCon.Format(_T("Provider=OraOLEDB.Oracle.1;"
		"Password=%s;Persist Security Info=True;User ID=%s;"
		"Data Source=\"(DESCRIPTION=(ADDRESS_LIST=(ADDRESS=(PROTOCOL=TCP)(HOST=%s)(PORT=1521)))(CONNECT_DATA=(SID=%s)))\""),
		m_strDBPwd,m_strDBUser,m_strIP,m_strDBName);

	return m_con.Open(m_strCon);
}
/*****************************************************************************
    *  @brief    : 关闭数据库
    *  @author   : chenyi
    *  @date     : 2012/02/02 18:55:06
    *  @version  : 
    *  @inparam  : 
    *  @outparam :  
    *  @return   :
*****************************************************************************/
void CAdoOracle::Close()
{
	if (m_hCheck && m_hEvent)
	{
		SetEvent(m_hEvent);
		WaitForSingleObject(m_hCheck , INFINITE);
		CloseHandle(m_hCheck);
		CloseHandle(m_hEvent);
		m_hEvent = NULL;
		m_hCheck = NULL;
	}
	autoWriteLock lock(&m_RWLock);
	m_con.Close();
}
/*****************************************************************************
    *  @brief    : 获取最后操作的错误信息
    *  @author   : chenyi
    *  @date     : 2012/02/02 18:48:56
    *  @version  : 
    *  @inparam  : 
    *  @outparam :  
    *  @return   : 错误信息字符串，
*****************************************************************************/
CString CAdoOracle::GetLastErrorText()
{
	autoReadLock lock(&m_RWLock);
	return m_con.GetLastErrorText();
}
/*****************************************************************************
    *  @brief    : 执行无返回值的语句，可以是添加，删除，修改
    *  @author   : chenyi
    *  @date     : 2012/02/02 18:56:21
    *  @version  : 
    *  @inparam  : SQL语句
    *  @outparam : 
    *  @return   : 成功返回TRUE
	*  @note     : 
*****************************************************************************/
bool CAdoOracle::ExcuteNoResult( const CString& strSql )
{
	autoReadLock lock(&m_RWLock);
	return m_con.Execute(strSql);
}
/*****************************************************************************
    *  @brief    : 查询结果集
    *  @author   : chenyi
    *  @date     : 2012/02/02 19:11:50
    *  @version  : 
    *  @inparam  : strSql  查询字符串
    *  @outparam : rsOut   记录集类
    *  @return   : 成功返回true
	*  @note     : 
*****************************************************************************/
bool CAdoOracle::ExcuteQuery( const CString& strSql , OUT CAdoRecordSet& rsOut )
{
	autoReadLock lock(&m_RWLock);
	rsOut.SetAdoConnection(&m_con);
	return rsOut.Open(strSql);
}
/*****************************************************************************
    *  @brief    : 执行查询数量的SQL语句
    *  @author   : chenyi
    *  @date     : 2012/02/02 19:16:33
    *  @version  : 
    *  @inparam  : strSql 查询语句，一般为 select count(*) from ...
    *  @outparam : nCount 结果条数
    *  @return   : 成功返回true
    *  @note     : 
*****************************************************************************/
bool CAdoOracle::ExcuteSalary(const CString& strSql, OUT int& nCount)
{
	autoReadLock lock(&m_RWLock);
	CAdoRecordSet rs(&m_con);
	try
	{
		if (rs.Open(strSql) && !rs.IsEOF())
		{
			rs.GetCollect(long(0) , nCount);
			rs.MoveNext();
		}
		else
		{
			return false;
		}
	}
	catch(...)
	{
		return false;
	}
	return true;
}
/*****************************************************************************
    *  @brief    : 根据列名获取查询的数量
    *  @author   : chenyi
    *  @date     : 2012/02/02 19:20:55
    *  @version  : 
    *  @inparam  : strSql  查询语句
	               rowName 列名
    *  @outparam : nCount  数据条数
    *  @return   : 成功返回true
    *  @note     : 
*****************************************************************************/
bool CAdoOracle::GetQueryCount( const CString& strSql, const CString& rowName, OUT int& nCount )
{
	autoReadLock lock(&m_RWLock);
	CAdoRecordSet rs(&m_con);
	try
	{
		if (rs.Open(strSql) && !rs.IsEOF())
		{
			rs.GetCollect(rowName , nCount);
			rs.MoveNext();
		}
		else
		{
			return false;
		}
	}
	catch(...)
	{
		return false;
	}
	return true;
}
/*****************************************************************************
    *  @brief    : 判断给出的语句执行后能否得到记录
    *  @author   : chenyi
    *  @date     : 2012/02/02 19:24:12
    *  @version  : 
    *  @inparam  : strSql     查询语句
    *  @outparam : bExistOut  true表示存在，false不存在
    *  @return   : 成功返回true
    *  @note     : 
*****************************************************************************/
bool CAdoOracle::CheckInfoExsist( const CString& strSql, OUT bool& bExistOut)
{//如果没有数据，那么rs是空么？
	autoReadLock lock(&m_RWLock);
	CAdoRecordSet rs(&m_con);
	if (rs.Open(strSql))
	{
		bExistOut = !rs.IsEOF();
	}
	else
	{
		return false;
	}
	return true;
}
/*****************************************************************************
    *  @brief    : 获取表中的记录条数
    *  @author   : chenyi
    *  @date     : 2012/02/02 19:25:34
    *  @version  : 
    *  @inparam  : 表名
    *  @outparam : nCount 表中记录数
    *  @return   : 成功返回True
    *  @note     : 
*****************************************************************************/
bool CAdoOracle::GetTableRowNum( const CString& tableName, OUT int& nCount)
{
	autoReadLock lock(&m_RWLock);
	CString strSql = "";
	strSql.Format("select count(*) from %s" , tableName);
	return ExcuteSalary(strSql, nCount);
}
/*****************************************************************************
    *  @brief    : 获取指定序列的下一个序列
    *  @author   : chenyi
    *  @date     : 2012/02/02 19:26:46
    *  @version  : 
    *  @inparam  : strSequenceName  序列名
    *  @outparam : nSeqVal  下一个序列值
    *  @return   : 成功返回true
    *  @note     : 
*****************************************************************************/
bool CAdoOracle::GetSequenceNextValue( const CString& strSequenceName, OUT int & nSeqVal)
{
	autoReadLock lock(&m_RWLock);
	CAdoRecordSet rs(&m_con);
	CString strSql = "";
	strSql.Format("select %s.nextval as NextID from dual", strSequenceName);
	try
	{
		if (rs.Open(strSql)&&!rs.IsEOF())
		{
			rs.GetCollect("NextID" , nSeqVal);
			rs.MoveNext();
		}
		else
		{
			return false;
		}
	}
	catch(...)
	{
		return false;
	}
	return true;
}
/*****************************************************************************
    *  @brief    : 检查数据库线程
    *  @author   : chenyi
    *  @date     : 2012/02/02 19:34:35
    *  @version  : 
    *  @inparam  : lp  数据库类指针
    *  @outparam :  
    *  @return   : 0
    *  @note     : 
*****************************************************************************/
DWORD CAdoOracle::checkThread( LPVOID lp )
{
	CAdoOracle * pDB = (CAdoOracle*)lp;
	NULL_THROW(pDB);
	while(WAIT_TIMEOUT==WaitForSingleObject(pDB->m_hEvent , pDB->m_nCheckTime))
	{
		if (!pDB->checkDB())
		{
			pDB->reConDB();
		}
	}
	return 0;
}
/*****************************************************************************
    *  @brief    : 开启数据库重连
    *  @author   : chenyi
    *  @date     : 2012/02/02 18:49:57
    *  @version  : 
    *  @inparam  : nCheckTime 检查间隔时间,默认值为3000ms
    *  @outparam :  
    *  @return   :
*****************************************************************************/
void CAdoOracle::KeepAlive( int nCheckTime /*= 3000*/ )
{
	m_nCheckTime = nCheckTime;
	if (NULL==m_hCheck)
	{
		m_hEvent = CreateEvent(0, true, false , 0);
		//NULL_THROW(m_hEvent);
		m_hCheck = CreateThread(0, 0, CAdoOracle::checkThread, this ,0 ,0);
		//NULL_THROW(m_hCheck);
	}
}
/*****************************************************************************
    *  @brief    : 检查DB是否连接正常
    *  @author   : chenyi
    *  @date     : 2012/02/02 19:38:28
    *  @version  : 
    *  @inparam  : 
    *  @outparam :  
    *  @return   : 正常返回true，不正常返回false
    *  @note     :
*****************************************************************************/
bool CAdoOracle::checkDB()
{
	autoReadLock lock(&m_RWLock);
	if (!m_con.IsOpen())
	{
		return false;
	}

	CAdoRecordSet rs(&m_con);
	return rs.Open("select 1 from dual");
}
/*****************************************************************************
    *  @brief    : 重连数据库
    *  @author   : chenyi
    *  @date     : 2012/02/02 19:44:15
    *  @version  : 
    *  @inparam  : 
    *  @outparam :  
    *  @return   : 成功返回true，失败返回false
    *  @note     :
*****************************************************************************/
bool CAdoOracle::reConDB()
{
	autoWriteLock lock(&m_RWLock);
	m_con.Close();
	return m_con.Open(m_strCon);
}
/*****************************************************************************
    *  @brief    : 
    *  @author   : chenyi
    *  @date     : 2012/02/08 16:03:37
    *  @version  : 
    *  @inparam  : strSqlVec  需要执行的多条Sql语句
    *  @outparam :  
    *  @return   : 成功返回true，失败false
    *  @note     :
*****************************************************************************/
bool CAdoOracle::ExcuteMutiSql( const vector<CString> strSqlVec )
{
	autoReadLock lock(&m_RWLock);
	
	m_con.BeginTrans();
	for(size_t i = 0; i < strSqlVec.size();++i)
	{
		if (!ExcuteNoResult(strSqlVec[i]))
		{
			m_con.RollbackTrans();
			return false;
		}
	}
	m_con.CommitTrans();
	return true;
}
/*****************************************************************************
    *  @brief    : 获取数据库连接信息
    *  @author   : chenyi
    *  @date     : 2012/02/28 10:52:13
    *  @version  : 
    *  @inparam  : 
    *  @outparam : strDBServerIP 数据库IP地址
	               strDBName     数据库名
				   strDBUser     数据库用户名
				   strDBPwd      数据库密码
    *  @return   :
    *  @note     :
*****************************************************************************/
void CAdoOracle::GetDBInfo(CString& OUT strDBServerIP, CString& OUT strDBName, CString& OUT strDBUser,CString& OUT strDBPwd)
{
	autoReadLock lock(&m_RWLock);
	strDBServerIP = m_strIP;
	strDBName = m_strDBName;
	strDBUser = m_strDBUser;
	strDBPwd  = m_strDBPwd;
}