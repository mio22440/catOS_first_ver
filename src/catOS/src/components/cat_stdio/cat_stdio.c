#include "cat_stdio.h"
#include "bsp_usart.h"
#include "cat_support.h"

/************* macros and defines **************/
#define CAT_INT_LONG    10
#define MAX_STRNG_LEN   300

typedef enum
{
    CAT_ERROR_TYPE_NO_MATCH_ERROR_TYPE = 0,
    CAT_ERROR_TYPE_ARG_TYPE_NOT_SUPPORT,
    CAT_ERROR_END,
} cat_error_type_t;

/************* local vars **************/
static uint8_t *_cat_error_msg[] = 
{
    "[cat_stdio] no match error type !!\r\n",
    "[cat_stdio:cat_print]type not support !!\r\n",
};

/************* static **************/
static int32_t _cat_print_string(uint8_t *str);
static int32_t _cat_print_error(cat_error_type_t type);
static int32_t _cat_print_int(int32_t num, int32_t width);

static int32_t _cat_scan_string(uint8_t *str_dest, uint32_t buf_len);
static int32_t _cat_scan_int(int32_t *dest);


static int32_t _cat_print_string(uint8_t *str)
{
    int32_t ret = 0;

    if(NULL == str)
    {
        ret = -1;
    }
    else
    {
        while('\0' != *str)
        {
            cat_putchar(*str);
            str++;
        }
    }

    return ret;
}

static int32_t _cat_print_error(cat_error_type_t type)
{
    int32_t ret = 0;
    cat_putchar('\r');
    cat_putchar('\n');

    /* 检查参数type范围 */
    if(
        (CAT_ERROR_TYPE_NO_MATCH_ERROR_TYPE >= type) ||
        (CAT_ERROR_END                      <= type)    
    )
    {
        _cat_print_string(_cat_error_msg[CAT_ERROR_TYPE_NO_MATCH_ERROR_TYPE]);
    }
    else
    {
        _cat_print_string(_cat_error_msg[type]);
    }

    return ret;
}

static int32_t _cat_print_int(int32_t num, int32_t width)
{
    int32_t ret = 0;
    uint32_t tmp = 0;
    /* uint32_t max val = 4294967295 */
    uint8_t buf[CAT_INT_LONG] = {0};
    uint8_t i = 0;

    if(num < 0)
    {
        cat_putchar('-');
        num = -num;
    }

    do
    {
        buf[i++] = num % 10;
        num = num / 10;

    }while((0 != num) && (i < CAT_INT_LONG));

    /* 如果有对齐要求则先输出空格 */
    width = width - i - 1;
    while(width > 0)
    {
        cat_putchar(' ');
    }

    while(i > 0)
    {
        cat_putchar(buf[--i] + '0');
    }

    return ret;
}

static int32_t _cat_scan_string(uint8_t *str_dest, uint32_t buf_len)
{
    int32_t ret = 0;
    uint32_t i = 0;

    do
    {
        str_dest[i++] = cat_getchar();
    } while (
        ('\r' != str_dest[i-1]) &&
        ('\n' != str_dest[i-1]) &&
        ('\0' != str_dest[i-1]) &&
        (i < buf_len - 1)
    );

    str_dest[i] = '\0';

    return ret;
}

static int32_t _cat_scan_int(int32_t *dest)
{
    int32_t ret = 0;
    uint8_t is_negative = 0;
    uint8_t tmp_char = 0;
    int32_t tmp_int = 0;

    tmp_char = cat_getchar();
    if('-' == tmp_char)
    {
        is_negative = 1;
        tmp_char = cat_getchar();
    }
    else if('+' == tmp_char)
    {
        tmp_char = cat_getchar();
    }


    while(
        ('0' <= tmp_char) &&
        ('9' >= tmp_char)
    )
    {
        tmp_int = (tmp_int * 10) + (tmp_char - '0');
        tmp_char = cat_getchar();
    }

    if(1 == is_negative)
    {
        tmp_int = -tmp_int;
    }

    *dest = tmp_int;

    return ret;
}

/************* func **************/

uint8_t cat_getchar(void)
{
    uint8_t ret = 0;
    
#if 0
    ret = getchar();
#else //#if 0
    cat_bsp_uart_receive_byte((uint8_t *)&ret);
#endif //#if 0

    return ret;
}

uint8_t cat_putchar(uint8_t ch)
{
#if 0
    putchar(ch);
#else //#if 0
    cat_bsp_uart_transmit_byte((uint8_t *)&ch);
#endif //#if 0
    return ch;
}

int32_t cat_scanf(const uint8_t *format, ...)
{
    int32_t ret = 0;
    va_list ap;
    uint8_t *p = NULL;

    if(NULL == format)
    {
        ret = -1;
        return ret;
    }

    /* 获取参数 */
    va_start(ap, format);

    /* 挨个处理 */
    p = format;
    while('\0' != *p)
    {
        if('%' != *p)
        {
            //cat_putchar(*p);
            p++;
            continue;
        }
        else
        {
            p++;
        }

        switch (*p)
        {
        case 'd':
            p++;
            //_cat_print_int(va_arg(ap, int32_t));
            _cat_scan_int(va_arg(ap, int32_t *));
            break;

        case 's':
            p++;
            //_cat_print_string(va_arg(ap, uint8_t *));
            _cat_scan_string(va_arg(ap, uint8_t *), MAX_STRNG_LEN);
            break;
        
        default:
            /* 可以考虑单独定义一个输出纯字符串的函数，可以打印错误信息 */
            _cat_print_error(CAT_ERROR_TYPE_ARG_TYPE_NOT_SUPPORT);
            ret = -1;
            break;
        }

        if(-1 == ret)
        {
            break;
        }
    }

    va_end(ap);

    // cat_putchar('\r');
    // cat_putchar('\n');

    return ret;
}

int32_t cat_printf(const uint8_t *format, ...)
{
    int32_t ret = 0;
    va_list ap;
    uint8_t *p = NULL;          /**< 用来遍历format字符串 */
    uint8_t wid_buf[5] = {0};   /**< 用于保存宽度的字符串 */
    int32_t width = 0;         /**< 输出宽度(目前仅用于整型输出) */

    if(NULL == format)
    {
        ret = -1;
        return ret;
    }

    /* 获取参数 */
    va_start(ap, format);

    /* 挨个处理 */
    p = format;
    while('\0' != *p)
    {
        if('%' != *p)
        {
            /* 发现'%'就开始处理格式化输出内容 */
            cat_putchar(*p);
            p++;
            continue;
        }
        else
        {
            p++;
        }

        /* 获得宽度数据(如果有) */
        if(
            ((*p) >= '0') &&
            ((*p) <= '9')
        )
        {
            wid_buf[width++] = *p;
            p++;
        }
        if(width > 0)
        {
            wid_buf[width] = '\0';
            cat_atoi(&width, wid_buf);
        }

        switch (*p)
        {
        case 'd':
            p++;
            _cat_print_int(va_arg(ap, int32_t), width);
            width = 0;
            break;

        case 's':
            p++;
            _cat_print_string(va_arg(ap, uint8_t *));
            break;
        
        default:
            /* 可以考虑单独定义一个输出纯字符串的函数，可以打印错误信息 */
            _cat_print_error(CAT_ERROR_TYPE_ARG_TYPE_NOT_SUPPORT);
            ret = -1;
            break;
        }

        if(-1 == ret)
        {
            break;
        }
    }

    va_end(ap);

    return ret;
}

