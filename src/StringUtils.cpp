#include "StringUtils.h"
#include <cctype>
#include <vector>
#include <sstream>
#include <iostream>
#include <string>

namespace StringUtils {

    std::string Slice(const std::string &str, ssize_t start, ssize_t end) noexcept {
        if (start < 0) {
            start = str.size() + start;
            if (start < 0) // If start is still less than 0, set it to 0
                start = 0;
        }
        if (end <= 0) {
            end = str.size() + end;
            if (end < 0) // If end is still less than 0, set it to 0
                end = 0;
        }

        if (start >= str.size() || start >= end)
            return "";

        return str.substr(start, end - start);
    }

    std::string Capitalize(const std::string &str) noexcept {
        if (str.empty()) return "";
        std::string result = str;
        result[0] = std::toupper(result[0]);
        for (size_t i = 1; i < result.size(); ++i) {
            result[i] = std::tolower(result[i]);
        }
        return result;
    }

    std::string Upper(const std::string &str) noexcept {
        if (str.empty()) return "";
        std::string result = str;
        for (ssize_t i = 0; i < result.size(); i++) {
            result[i] = std::toupper(result[i]);
        }
        return result;
    }

    std::string Lower(const std::string &str) noexcept {
        if (str.empty()) return "";
        std::string result = str;
        for (ssize_t i = 0; i < result.size(); i++) {
            result[i] = std::tolower(result[i]);
        }
        return result;
    }

    std::string LStrip(const std::string &str) noexcept {
        if (str.empty()) return "";
        size_t i = 0;
        while (i < str.size() && std::isspace(str[i])) {
            i++;
        }
        return str.substr(i);
    }

    std::string RStrip(const std::string &str) noexcept {
        if (str.empty()) return "";
        size_t i = str.size() - 1;
        while (i > 0 && std::isspace(str[i])) {
            i--;
        }
        return str.substr(0, i + 1);
    }

    std::string Strip(const std::string &str) noexcept {
        return LStrip(RStrip(str));
    }

    std::string Center(const std::string &str, int width, char fill) noexcept {
        if (width <= str.size()) return str;
        int leftFill = (width - str.size()) / 2;
        int rightFill = width - str.size() - leftFill;
        return std::string(leftFill, fill) + str + std::string(rightFill, fill);
    }

    std::string LJust(const std::string &str, int width, char fill) noexcept {
        if (width < str.size()) {
            return str;
        }
        int rightFill = width - str.size();
        return str + std::string(rightFill, fill);
    }

    std::string RJust(const std::string &str, int width, char fill) noexcept {
        if (width < str.size()) {
            return str;
        }
        int leftFill = width - str.size();
        return std::string(leftFill, fill) + str;
    }

    std::string Replace(const std::string &str, const std::string &old, const std::string &rep) noexcept {
        if (str.empty()) return "";
        std::string retval = str;
        size_t pos = 0;
        while ((pos = retval.find(old, pos)) != std::string::npos) {
            retval.replace(pos, old.length(), rep);
            pos += rep.length();
        }
        return retval;
    }

    std::vector<std::string> Split(const std::string& str, const std::string& splt) noexcept {
        std::vector<std::string> tokens;
        if (splt.empty()) {
            size_t start = 0;
            for (size_t i = 0; i < str.size(); ++i) {
                if (std::isspace(str[i])) {
                    if (start != i) {
                        tokens.push_back(str.substr(start, i - start));
                    }
                    start = i + 1;
                } else if (i == str.size() - 1) {
                    tokens.push_back(str.substr(start));
                }
            }
        } else {
            size_t start = 0;
            size_t end = str.find(splt);
            while (end != std::string::npos) {
                std::string token = str.substr(start, end - start);
                tokens.push_back(token);
                start = end + splt.length();
                end = str.find(splt, start);
            }
            tokens.push_back(str.substr(start));
        }
        return tokens;
    }

    std::string Join(const std::string &str, const std::vector<std::string> &vect) noexcept {
        if (vect.empty()) return "";
        std::string retval;
        int i = 0;
        while (i < vect.size()) {
            retval = retval + vect[i] + str;
            i++;
        }
        retval.pop_back();
        return retval;
    }

    std::string ExpandTabs(const std::string &str, int tabsize) noexcept {
        int count = 0;
        std::string ret_str = "";
        std::string str_cpy = str;
        for (char c : str) {
            if (count == tabsize) {
                count = 0;
            }
            if (c == '\t') {
                for (int j = 0; j < tabsize - count; ++j) {
                    ret_str += ' ';
                }
                count = 0;
            } else if (c == '\n') {
                ret_str += c;
                str_cpy = str_cpy.substr(1);
            } else {
                ret_str += c;
                str_cpy = str_cpy.substr(1);
                ++count;
            }
        }
        return ret_str;
    }

    int EditDistance(const std::string &left, const std::string &right, bool ignorecase) noexcept {
        std::string l = left;
        std::string r = right;
        if (ignorecase) {
            l = Lower(left);
            r = Lower(right);
        }
        std::vector<std::vector<int>> matrix(l.size() + 1, std::vector<int>(r.size() + 1, 0));
        for (int i = 0; i <= l.size(); ++i) {
            matrix[i][0] = i;
        }
        for (int j = 0; j <= r.size(); ++j) {
            matrix[0][j] = j;
        }
        for (int i = 1; i <= l.size(); ++i) {
            for (int j = 1; j <= r.size(); ++j) {
                int cost = (l[i - 1] == r[j - 1]) ? 0 : 1;
                matrix[i][j] = std::min({ matrix[i - 1][j] + 1, matrix[i][j - 1] + 1, matrix[i - 1][j - 1] + cost });
            }
        }
        return matrix[l.size()][r.size()];
    }

}
