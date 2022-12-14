/**
 * @file cat_support.h
 * @brief 
 * @author amoigus (648137125@qq.com)
 * @version 0.1
 * @date 2021-06-09
 * 
 * @copyright Copyright (c) 2021
 * 
 * @par 修改日志：
 * Date              Version Author      Description
 * 2021-06-09 1.0    amoigus             内容
 */

#ifndef CAT_STRCMP_H
#define CAT_STRCMP_H

#include "catos_defs.h"

/**
 * @brief 比较字符串是否相同
 * @param str1 
 * @param str2 
 * @return uint32_t 0:equal, -1:not equal, -2:NULL args
 */
int32_t cat_strcmp(const uint8_t *str1, const uint8_t *str2);

/**
 * @brief 字符串拷贝
 * @param dest 
 * @param src 
 * @return int32_t 0:成功，else:失败
 */
int32_t *cat_strcpy(uint8_t *dest, const uint8_t *src, uint32_t dest_len);

/**
 * @brief 字符串转有符号32
 * @param dest 
 * @param src 
 * @return int32_t 0:成功，else:失败
 */
int32_t cat_atoi(int32_t *dest, const uint8_t *src);

/**
 * @brief 有符号32转字符串
 * @param dest 
 * @param src 
 * @return int32_t 0:成功，else:失败
 */
int32_t cat_itoa(uint8_t *dest, int32_t src);



#endif

