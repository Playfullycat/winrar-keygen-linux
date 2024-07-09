#ifdef _MSC_VER
#include <tchar.h>
#endif
#include <string.h>
#include <stdio.h>
#ifdef _MSC_VER
#include <windows.h>
#endif
#include <locale.h>
#include "WinRarConfig.hpp"
#include "WinRarKeygen.hpp"
#include <system_error>
#include <sstream> // 包含sstream头文件以使用std::ostringstream
#include <cstring> // 包含cstring头文件以使用std::strcpy

void Help() {
#ifdef _MSC_VER
    _putts(TEXT("Usage:"));
    _putts(TEXT("        winrar-keygen.exe <your name> <license type>"));
    _putts(TEXT(""));
    _putts(TEXT("Example:"));
    _putts(TEXT(""));
    _putts(TEXT("        winrar-keygen.exe \"Github\" \"Github.com\""));
    _putts(TEXT("  or:"));
    _putts(TEXT("        winrar-keygen.exe \"Github\" \"Github.com\" > rarreg.key"));
    _putts(TEXT("  or:"));
    _putts(TEXT("        winrar-keygen.exe \"Github\" \"Github.com\" | Out-File -Encoding ASCII rarreg.key\n"));
#else
    puts("Usage:");
    puts("        ./winrar-keygen <your name> <license type>");
    puts("");
    puts("Example:");
    puts("");
    puts("        ./winrar-keygen \"Github\" \"Github.com\"");
    puts("  or:");
    puts("        ./winrar-keygen \"Github\" \"Github.com\" > rarreg.key");
    puts("  or:");
    puts("        ./winrar-keygen \"Github\" \"Github.com\" | Out-File -Encoding ASCII rarreg.key\n");
#endif
}

void PrintRegisterInfo(const WinRarKeygen<WinRarConfig>::RegisterInfo& Info) {
#ifdef _MSC_VER
    _tprintf_s(TEXT("%hs\n"), "RAR registration data");
    _tprintf_s(TEXT("%hs\n"), Info.UserName.c_str());
    _tprintf_s(TEXT("%hs\n"), Info.LicenseType.c_str());
    _tprintf_s(TEXT("UID=%hs\n"), Info.UID.c_str());
#else
    printf("%s\n", "RAR registration data");
    printf("%s\n", Info.UserName.c_str());
    printf("%s\n", Info.LicenseType.c_str());
    printf("UID=%s\n", Info.UID.c_str());
#endif
    for (size_t i = 0; i < Info.HexData.length(); i += 54) {
#ifdef _MSC_VER
        _tprintf_s(TEXT("%.54hs\n"), Info.HexData.c_str() + i);
#else
        printf("%.54s\n", Info.HexData.c_str() + i);
#endif
    }
}

#ifdef _MSC_VER
std::string ToACP(PCWSTR lpszUnicodeString) {
    int len;

    len = WideCharToMultiByte(CP_ACP, 0, lpszUnicodeString, -1, NULL, 0, NULL, NULL);
    if (len == 0) {
        auto err = GetLastError();
        throw std::system_error(err, std::system_category());
    }

    std::string Result(len, '\x00');

    len = WideCharToMultiByte(CP_ACP, 0, lpszUnicodeString, -1, Result.data(), static_cast<int>(Result.length()), NULL, NULL);
    if (len == 0) {
        auto err = GetLastError();
        throw std::system_error(err, std::system_category());
    }

    while (Result.back() == '\x00') {
        Result.pop_back();
    }

    return Result;
}
#endif

#ifdef _MSC_VER
int _tmain(int argc, PTSTR argv[]) {
#else
int main(int argc, char* argv[]) {
#endif
    setlocale(LC_ALL, "");
    if (argc == 3) {
        try {
            PrintRegisterInfo(
#if (defined(_UNICODE) || defined(UNICODE)) && defined(_MSC_VER)
                WinRarKeygen<WinRarConfig>::GenerateRegisterInfo(ToACP(argv[1]).c_str(), ToACP(argv[2]).c_str())
#else
                WinRarKeygen<WinRarConfig>::GenerateRegisterInfo(argv[1], argv[2])
#endif
            );
        } catch (std::exception& e) {
#ifdef _MSC_VER
            _tprintf_s(TEXT("%hs\n"), e.what());
#else
            printf("%s\n", e.what());
#endif
            return -1;
        }
    } else {
        Help();
    }
    return 0;
}


extern "C"
{
    const char* GetKey(const char* user, const char* license)
    {
        auto RegInfo = WinRarKeygen<WinRarConfig>::GenerateRegisterInfo(user, license);

        // 构建注册信息字符串
        std::ostringstream oss;
        oss << "RAR registration data\n";
        oss << RegInfo.UserName << "\n";
        oss << RegInfo.LicenseType << "\n";
        oss << "UID=" << RegInfo.UID << "\n";
        for (size_t i = 0; i < RegInfo.HexData.length(); i += 54)
        {
            oss << RegInfo.HexData.substr(i, 54) << "\n";
        }

        // 将字符串复制到新的内存区域，并返回指针
        std::string result = oss.str();
        char* cstr = new char[result.length() + 1];
        std::strcpy(cstr, result.c_str());
        return cstr;
    }
}
