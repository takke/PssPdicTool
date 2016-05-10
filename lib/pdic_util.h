#ifndef ___PDIC_UTIL_H_INCLUdED_
#define ___PDIC_UTIL_H_INCLUdED_

#include "hdde.h"
//#include "hreg.h"
#include <atlstr.h>
typedef std::wstring std_xstring;

/// PDIC の検索支援クラス
class PdicSearchManager
{
	HDdeManager& hdm_;
	std_xstring strErrorMsg_;

public:
	bool bPdicUnicode_;

	PdicSearchManager( HDdeManager& a_hdm ) : hdm_(a_hdm), bPdicUnicode_(false) {}

	/// 見出し語の取得
	bool get_wordw(std_xstring& strAnswer) {
		return hdm_.requestw(L"GetWord", strAnswer);
	}

	/// 訳語の取得
	bool get_japaw(std_xstring& strQuestion) {
		return hdm_.requestw(L"GetJapa", strQuestion);
	}

	/// 用例の取得
	bool get_expw(std_xstring& strDesc) {
		return hdm_.requestw(L"GetExp", strDesc);
	}


	/// 検索
	bool findw(LPCTSTR szFind) {
		return hdm_.pokew(L"Find", szFind);
	}
	/// ２つ目以降の検索
	bool findw_next() {
		return hdm_.pokew(L"Find", L"");
	}

	/// init_pdic などの失敗時の原因の取得
	LPCTSTR get_error_reason() { return strErrorMsg_.c_str(); }

	/**
	 * PDIC の準備
	 *
	 * @return pdic が起動していなければ false, 起動していてオープンできれば true を返す．
	 * @param mode		"Dictionary", "Simulate" など
	 */
	bool init_pdicw(LPCTSTR mode = L"Dictionary")
	{
		if (hdm_.initw(L"PDICU", mode)) {
			bPdicUnicode_ = true;
		}
		else {
			strErrorMsg_ = L"";
			return false;
		}

		// 辞書オープン
		if (hdm_.pokew(L"Open", L"") == false) {
			strErrorMsg_ = L"PDIC の辞書が開けませんでした\n";
			return false;
		}

		return true;
	}
};

#endif
