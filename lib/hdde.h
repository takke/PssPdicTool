#ifndef	___HDDE_H_INCLUDED_
#define	___HDDE_H_INCLUDED_

#include <ddeml.h>
#include <vector>

/// DDE Manager
class HDdeManager
{
public:
	DWORD		ddeinst_;			///< the instance of dde-client
private:
	HSZ			hszServiceName_;	///< ServiceName
	HSZ			hszTopicName_;		///< TopicName
	HCONV		hconv_;
	PFNCALLBACK pCallback_;			///< DDE callback function

public:
	HDdeManager( PFNCALLBACK a_pCallback ) : ddeinst_(0), pCallback_(a_pCallback) {}
	~HDdeManager() {
		UninitDdeServer();
	}

private:

	/// �I������
	void UninitDdeServer()
	{
		if( ddeinst_ ) {
			DdeFreeStringHandle( ddeinst_, hszServiceName_ );
			DdeFreeStringHandle( ddeinst_, hszTopicName_ );
			DdeUninitialize( ddeinst_ );
			ddeinst_ = 0;
		}
	}

public:
	/// �T�[�o�[�ɐڑ�����
	bool initw(LPCTSTR szService_, LPCTSTR szTopic_)
	{
		// ������
		UINT ret = DdeInitialize(&ddeinst_, pCallback_, APPCMD_CLIENTONLY, 0);
		if (ret != DMLERR_NO_ERROR)
			return false;

		hszServiceName_ = DdeCreateStringHandle(ddeinst_, szService_, CP_WINUNICODE);
		hszTopicName_ = DdeCreateStringHandle(ddeinst_, szTopic_, CP_WINUNICODE);

		hconv_ = DdeConnect(ddeinst_, hszServiceName_, hszTopicName_, NULL);
		if (hconv_ == 0) {
			printf("DdeConnect: error code : 0x%X\n", DdeGetLastError(ddeinst_));
			UninitDdeServer();
			return false;
		}
		return true;
	}

	/// REQUEST
	/// @retval NULL		���s
	/// @retval Non-NULL	�擾����������
	bool requestw( LPCTSTR szCmd_, std::wstring& strResult )
	{
		HSZ hszCmd = DdeCreateStringHandle( ddeinst_, szCmd_, CP_WINUNICODE );

		HDDEDATA hdata = DdeClientTransaction( NULL, 0, hconv_, hszCmd, CF_UNICODETEXT, XTYP_REQUEST, 9999, NULL );
		if( hdata==NULL ) {
			DdeFreeStringHandle( ddeinst_, hszCmd );
			return NULL;
		}
/*
		DWORD dwSize = 0;
		TCHAR* p = (TCHAR*)DdeAccessData( hdata, &dwSize );
//		int usize = dwSize/2-1;
		int usize = dwSize;
		std::wstring strResult;
		if (usize>=0) {
			strResult.assign(p, usize);
		}

		DdeUnaccessData( hdata );
*/
		bool rval = false;

		std::vector<wchar_t> result;
		DWORD length = DdeGetData(hdata, NULL, 0, 0);
		if (length > 0)
		{
			result.resize(length*2 + 1);

			DWORD actual = DdeGetData(hdata, (LPBYTE)&result[0], length*2, 0);
			if (DdeGetLastError(ddeinst_) == DMLERR_NO_ERROR)
			{
				{
					if (length > 2)
					{
						// remove '\r\n'
/*						int pos = *lenResult - 1;
						if (pos > 0)
						{
							if (Result[*lenResult - 1] == L'\n')
							{
								Result[*lenResult - 1] = L'\0';
							}
						}
						*lenResult = (int)wcslen(Result);
						pos = *lenResult - 2;
						if (pos > 0)
						{
							if (Result[*lenResult - 2] == L'\r')
							{
								Result[*lenResult - 2] = L'\0';
							}
						}

						*lenResult = (int)wcslen(Result);
*/					}
				}

				strResult.assign(&result[0]);
				wprintf(L"*requestw[%d][%d][%s][%d]\n", length, actual, &result[0], strResult.size());
				rval = true;
			}
		}

		DdeFreeDataHandle( hdata );
		DdeFreeStringHandle( ddeinst_, hszCmd );

		return rval;
	}

	/// POKE
	bool pokew( LPCTSTR szCmd_, LPCTSTR szData0_ )
	{
		// TCHAR �ɕύX
		std::vector<TCHAR> szData(lstrlen(szData0_)+1);
		wcsncpy_s( &szData[0], szData.size(), szData0_, lstrlen(szData0_) );
		szData[ lstrlen(szData0_) ] = '\0';

		// ���N�G�X�g
		HSZ hszCmd = DdeCreateStringHandle(ddeinst_, szCmd_, CP_WINUNICODE);

		HDDEDATA rval_h = DdeClientTransaction( 
								(LPBYTE)&szData[0], 
								(DWORD)((wcslen(szData0_) + 1) * sizeof(wchar_t)),
								hconv_,
								hszCmd,
								CF_UNICODETEXT,
								XTYP_POKE,
								9999,
								NULL);
		
		DdeFreeStringHandle( ddeinst_, hszCmd );
		
		return rval_h!=0;
	}

};

#endif