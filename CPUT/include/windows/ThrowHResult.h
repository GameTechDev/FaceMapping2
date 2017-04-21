/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#ifndef THROWHRESULT_H_C17A57AF161E4C808E79EDAA5B0B0686
#define THROWHRESULT_H_C17A57AF161E4C808E79EDAA5B0B0686
// ====================================================================================================================

#include <exception>
#include <string>
#include <winerror.h>
#include <tchar.h>

namespace cput {

	using tstring = std::basic_string<TCHAR>;

#if defined(CPUT_NO_THROW)
#include <atlbase.h>
#endif

	class hresult_exception : public std::exception {
	public:
		hresult_exception(std::string const& file, unsigned long line, HRESULT hr = E_FAIL, tstring const& msg = tstring())
			: mLine( line )
			, mResult ( hr )
			, mMessage( msg )
			, mFile( file ) {
		}

		~hresult_exception() {}

		virtual char const* what() const override {
			return "";
		}

		std::string file() const { return mFile; }
		unsigned long line() const { return mLine; }
		HRESULT result() const { return mResult; }
		tstring message() const { return mMessage; }

	private:
		unsigned long mLine;
		HRESULT mResult;
		tstring mMessage;
		std::string mFile;
	};

	inline HRESULT throw_hr(char const* file, unsigned long line, HRESULT hr, tstring const& message) {
		if(FAILED(hr)) {
#if defined(CPUT_NO_THROW)
			hresult_exception blah(file, line, hr, message);
			OutputDebugString(blah.message());
			ATLASSERT(false);
#else
			throw hresult_exception(file, line, hr, message);
#endif
		}
		return hr;
	}

	inline HRESULT display_error(hresult_exception const &bang, tstring const& title = tstring()) {
		::MessageBox(0, bang.message().c_str(), title.c_str(), 0);
		return bang.result();
	}
};

#define ThrowHResult(hr_) cput::throw_hr((__FILE__), __LINE__, hr_, _T(#hr_))
#define ThrowHResultMsg(hr_, msg_) cput::throw_hr((__FILE__), __LINE__, hr_, msg_)
// #define THRMT(hr_, msg_) cput::throw_hr(_T(__FILE__), __LINE__, hr_, _T(msg_))

// #define TCR(hr_) ck::throw_cr((__FILE__), __LINE__, hr_, _T(#hr_))
// #define TCRM(hr_, msg_) ck::throw_cr((__FILE__), __LINE__, hr_, msg_)
// #define THRMT(hr_, msg_) ck::throw_hr(_T(__FILE__), __LINE__, hr_, _T(msg_))


template<typename T>
inline T throw_win(const TCHAR *file, unsigned line, T status, const TCHAR *message = NULL, int error = GetLastError()) {
	if (!status) {
		throw_hr(file, line, HRESULT_FROM_WIN32(error), message);
	}
	return status;
}

// TWS => throw Win32 function status
// TWSM => Win32 status with message
// TWSMT => Win32 status with message constant needing _T()
#define TWS(status_) ck::throw_win(_T(__FILE__), __LINE__, status_, _T(#status_))
#define TWSM(status_, msg_) ck::throw_win(_T(__FILE__), __LINE__, status_, msg_)
#define TWSMT(status_, msg_) ck::throw_win(_T(__FILE__), __LINE__, status_, _T(msg_))

// variations with error code supplied
#define TWSE(status_, error_) ck::throw_win(_T(__FILE__), __LINE__, status_, NULL, error_)
#define TWSME(status_, error_, msg_) ck::throw_win(_T(__FILE__), __LINE__, status_, msg_, error_)
#define TWSMTE(status_, err_, msg_) ck::throw_win(_T(__FILE__), __LINE__, status_, _T(msg_), err_)

#define TRS(status_) ck::throw_win(_T(__FILE__), __LINE__, status_, _T(#status_), status_)

// ====================================================================================================================
#endif