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

	/// 終了処理
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
	/// サーバーに接続する
	bool initw(LPCTSTR szService_, LPCTSTR szTopic_)
	{
		// 初期化
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
	/// @retval NULL		失敗
	/// @retval Non-NULL	取得した文字列
	bool requestw( LPCTSTR szCmd_, std::wstring& strResult )
	{
		HSZ hszCmd = DdeCreateStringHandle( ddeinst_, szCmd_, CP_WINUNICODE );

		HDDEDATA hdata = DdeClientTransaction( NULL, 0, hconv_, hszCmd, CF_UNICODETEXT, XTYP_REQUEST, 9999, NULL );
		if( hdata==NULL ) {
			wprintf(L"*requestw[DdeClientTransaction error: 0x%X]\n", DdeGetLastError(ddeinst_));
			DdeFreeStringHandle( ddeinst_, hszCmd );
			return false;
		}

		bool rval = false;

#define USE_DDE_ACCESS_DATA
#ifdef  USE_DDE_ACCESS_DATA
		DWORD dwSize = 0;
		TCHAR* p = (TCHAR*)DdeAccessData( hdata, &dwSize );
		if (p == NULL) {
			wprintf(L"*requestw[DdeAccessData error: 0x%X]\n", DdeGetLastError(ddeinst_));
		}
		else {
			if (dwSize > 0) {
				strResult.assign(p, dwSize);
			}
			rval = true;
		}
		DdeUnaccessData( hdata );
#else
		// NotFound後のクエリーがなぜか "DdeGetData(1) error: 0x4009" になる。次の requestw は成功する・・・。
		std::vector<wchar_t> result;
		DWORD length = DdeGetData(hdata, NULL, 0, 0);
		UINT e = DdeGetLastError(ddeinst_);
		if (e != DMLERR_NO_ERROR) {
			wprintf(L"*requestw[DdeGetData(1) error: 0x%X]\n", e);
		}
		else {
			if (length > 0)
			{
				result.resize(length);

				DWORD actual = DdeGetData(hdata, (LPBYTE)&result[0], length, 0);
				UINT e = DdeGetLastError(ddeinst_);
				if (e == DMLERR_NO_ERROR)
				{
					strResult.assign(&result[0]);
					wprintf(L"*requestw[%d][%d][%s][%d]\n", length, actual, &result[0], strResult.size());
					rval = true;
				}
				else {
					wprintf(L"*requestw[DdeGetData(2) error: 0x%X]\n", e);
				}
			}
			else {
				wprintf(L"*requestw[DdeGetData(1) length = 0]\n");
			}
		}
		DdeFreeDataHandle( hdata );
#endif

		DdeFreeStringHandle( ddeinst_, hszCmd );

		return rval;
	}

	/// POKE
	bool pokew( LPCTSTR szCmd_, LPCTSTR szData0_ )
	{
		// TCHAR に変更
		std::vector<TCHAR> szData(lstrlen(szData0_)+1);
		wcsncpy_s( &szData[0], szData.size(), szData0_, lstrlen(szData0_) );
		szData[ lstrlen(szData0_) ] = '\0';

		// リクエスト
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