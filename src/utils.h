#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

/**
 * @brief 通用工具类，包含字符串处理、文件操作等常用功能
 */
class Utils {
public:
    /**
     * @brief 去除字符串首尾的空白字符
     * @param str 输入字符串
     * @return 去除空白字符后的字符串
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief 去除字符串首尾的引号
     * @param str 输入字符串
     * @return 去除引号后的字符串
     */
    static std::string trimQuotes(const std::string& str);
    
    /**
     * @brief 按指定分隔符分割字符串
     * @param str 输入字符串
     * @param delimiter 分隔符
     * @return 分割后的字符串向量
     */
    static std::vector<std::string> split(const std::string& str, char delimiter);
    
    /**
     * @brief 检查文件是否存在
     * @param filepath 文件路径
     * @return 文件存在返回true，否则返回false
     */
    static bool fileExists(const std::string& filepath);
    
    /**
     * @brief 验证文件是否存在（别名函数，保持向后兼容）
     * @param filepath 文件路径
     * @return 文件存在返回true，否则返回false
     */
    static bool validateFile(const std::string& filepath);
    
    /**
     * @brief 去除字符串中的行内注释（# 及其后面），但保留以下情况：
     *        - 引号内的 # 视为普通字符（支持 '...' 与 "..."）
     *        - 使用 \# 可转义字面 #（只在引号外生效）
     * @param str 输入字符串
     * @return 去除行内注释后的字符串（并已去首尾空白）
     */
    static std::string removeInlineComment(const std::string& str);
};

#endif // UTILS_H
