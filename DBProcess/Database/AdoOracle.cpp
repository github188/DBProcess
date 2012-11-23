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
    *  @brief    : �����ݿ⣬������������Ϣ
    *  @author   : chenyi
    *  @date     : 2012/02/02 18:53:55
    *  @version  : 
	*  @inparam  : strDBServerIP ���ݿ�IP
	               strDBName     ���ݿ���
				   strDBUser     ���ݿ��û���
	               strDBPwd      ���ݿ�����
    *  @outparam :  
    *  @return   : �ɹ�����true
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
    *  @brief    : �ر����ݿ�
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
    *  @brief    : ��ȡ�������Ĵ�����Ϣ
    *  @author   : chenyi
    *  @date     : 2012/02/02 18:48:56
    *  @version  : 
    *  @inparam  : 
    *  @outparam :  
    *  @return   : ������Ϣ�ַ�����
*****************************************************************************/
CString CAdoOracle::GetLastErrorText()
{
	autoReadLock lock(&m_RWLock);
	return m_con.GetLastErrorText();
}
/*****************************************************************************
    *  @brief    : ִ���޷���ֵ����䣬��������ӣ�ɾ�����޸�
    *  @author   : chenyi
    *  @date     : 2012/02/02 18:56:21
    *  @version  : 
    *  @inparam  : SQL���
    *  @outparam : 
    *  @return   : �ɹ�����TRUE
	*  @note     : 
*****************************************************************************/
bool CAdoOracle::ExcuteNoResult( const CString& strSql )
{
	autoReadLock lock(&m_RWLock);
	return m_con.Execute(strSql);
}
/*****************************************************************************
    *  @brief    : ��ѯ�����
    *  @author   : chenyi
    *  @date     : 2012/02/02 19:11:50
    *  @version  : 
    *  @inparam  : strSql  ��ѯ�ַ���
    *  @outparam : rsOut   ��¼����
    *  @return   : �ɹ�����true
	*  @note     : 
*****************************************************************************/
bool CAdoOracle::ExcuteQuery( const CString& strSql , OUT CAdoRecordSet& rsOut )
{
	autoReadLock lock(&m_RWLock);
	rsOut.SetAdoConnection(&m_con);
	return rsOut.Open(strSql);
}
/*****************************************************************************
    *  @brief    : ִ�в�ѯ������SQL���
    *  @author   : chenyi
    *  @date     : 2012/02/02 19:16:33
    *  @version  : 
    *  @inparam  : strSql ��ѯ��䣬һ��Ϊ select count(*) from ...
    *  @outparam : nCount �������
    *  @return   : �ɹ�����true
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
    *  @brief    : ����������ȡ��ѯ������
    *  @author   : chenyi
    *  @date     : 2012/02/02 19:20:55
    *  @version  : 
    *  @inparam  : strSql  ��ѯ���
	               rowName ����
    *  @outparam : nCount  ��������
    *  @return   : �ɹ�����true
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
    *  @brief    : �жϸ��������ִ�к��ܷ�õ���¼
    *  @author   : chenyi
    *  @date     : 2012/02/02 19:24:12
    *  @version  : 
    *  @inparam  : strSql     ��ѯ���
    *  @outparam : bExistOut  true��ʾ���ڣ�false������
    *  @return   : �ɹ�����true
    *  @note     : 
*****************************************************************************/
bool CAdoOracle::CheckInfoExsist( const CString& strSql, OUT bool& bExistOut)
{//���û�����ݣ���ôrs�ǿ�ô��
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
    *  @brief    : ��ȡ���еļ�¼����
    *  @author   : chenyi
    *  @date     : 2012/02/02 19:25:34
    *  @version  : 
    *  @inparam  : ����
    *  @outparam : nCount ���м�¼��
    *  @return   : �ɹ�����True
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
    *  @brief    : ��ȡָ�����е���һ������
    *  @author   : chenyi
    *  @date     : 2012/02/02 19:26:46
    *  @version  : 
    *  @inparam  : strSequenceName  ������
    *  @outparam : nSeqVal  ��һ������ֵ
    *  @return   : �ɹ�����true
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
    *  @brief    : ������ݿ��߳�
    *  @author   : chenyi
    *  @date     : 2012/02/02 19:34:35
    *  @version  : 
    *  @inparam  : lp  ���ݿ���ָ��
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
    *  @brief    : �������ݿ�����
    *  @author   : chenyi
    *  @date     : 2012/02/02 18:49:57
    *  @version  : 
    *  @inparam  : nCheckTime �����ʱ��,Ĭ��ֵΪ3000ms
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
    *  @brief    : ���DB�Ƿ���������
    *  @author   : chenyi
    *  @date     : 2012/02/02 19:38:28
    *  @version  : 
    *  @inparam  : 
    *  @outparam :  
    *  @return   : ��������true������������false
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
    *  @brief    : �������ݿ�
    *  @author   : chenyi
    *  @date     : 2012/02/02 19:44:15
    *  @version  : 
    *  @inparam  : 
    *  @outparam :  
    *  @return   : �ɹ�����true��ʧ�ܷ���false
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
    *  @inparam  : strSqlVec  ��Ҫִ�еĶ���Sql���
    *  @outparam :  
    *  @return   : �ɹ�����true��ʧ��false
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
    *  @brief    : ��ȡ���ݿ�������Ϣ
    *  @author   : chenyi
    *  @date     : 2012/02/28 10:52:13
    *  @version  : 
    *  @inparam  : 
    *  @outparam : strDBServerIP ���ݿ�IP��ַ
	               strDBName     ���ݿ���
				   strDBUser     ���ݿ��û���
				   strDBPwd      ���ݿ�����
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