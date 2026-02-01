#pragma once
#include <string>
#include <windows.h>
#include <vector>

class EncodingUtils {
public:
    // Конвертация между кодировками
    static std::string cp1251_to_utf8(const std::string& cp1251);
    static std::string utf8_to_cp1251(const std::string& utf8);

    // Утилиты
    static bool is_valid_utf8(const std::string& str);
};