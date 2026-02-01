#include "EncodingUtils.h"

std::string EncodingUtils::cp1251_to_utf8(const std::string& cp1251) {
    if (cp1251.empty()) return "";

    // CP-1251 -> UTF-16
    int wsize = MultiByteToWideChar(1251, 0, cp1251.c_str(), -1, NULL, 0);
    if (wsize <= 0) return cp1251;

    wchar_t* wbuf = new wchar_t[wsize];
    MultiByteToWideChar(1251, 0, cp1251.c_str(), -1, wbuf, wsize);

    // UTF-16 -> UTF-8
    int size = WideCharToMultiByte(CP_UTF8, 0, wbuf, -1, NULL, 0, NULL, NULL);
    if (size <= 0) {
        delete[] wbuf;
        return cp1251;
    }

    char* buf = new char[size];
    WideCharToMultiByte(CP_UTF8, 0, wbuf, -1, buf, size, NULL, NULL);

    std::string result(buf);
    delete[] wbuf;
    delete[] buf;

    return result;
}

std::string EncodingUtils::utf8_to_cp1251(const std::string& utf8) {
    if (utf8.empty()) return "";

    // UTF-8 -> UTF-16
    int wsize = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
    if (wsize <= 0) return utf8;

    wchar_t* wbuf = new wchar_t[wsize];
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, wbuf, wsize);

    // UTF-16 -> CP-1251
    int size = WideCharToMultiByte(1251, 0, wbuf, -1, NULL, 0, NULL, NULL);
    if (size <= 0) {
        delete[] wbuf;
        return utf8;
    }

    char* buf = new char[size];
    WideCharToMultiByte(1251, 0, wbuf, -1, buf, size, NULL, NULL);

    std::string result(buf);
    delete[] wbuf;
    delete[] buf;

    return result;
}

bool EncodingUtils::is_valid_utf8(const std::string& str) {
    int size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str.c_str(), -1, NULL, 0);
    return size > 0;
}