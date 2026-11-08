#ifndef __ERROR_CODE_H__
#define __ERROR_CODE_H__

/**
 * @addtogroup error_code 返回值
 * @ingroup platform_common
 * @brief 框架中使用的返回值定义
 * 
 * @{
 */
/// 通用的错误码
#define RET_SUCCESS    0
/// 通用失败
#define RET_FAILED    -1
/// 内存申请失败
#define RET_NO_MEM    -2
/// 无效的地址
#define RET_INVALID_ADDRESS  -3
/// 无效的参数
#define RET_INVALID_PARAMETER -4
/// 无效的数据
#define RET_INVALID_DATA     -5
/// 溢出
#define RET_OVERFLOW        -6
/// 不支持
#define RET_NO_SUPPORTED    -7
/// 设备不存在
#define RET_NO_DEVICE       -8
/// （请求）超时
#define RET_TIMEOUT         -9
/// 数据不存在
#define RET_NO_DATA         -10
/// 资源不可用
#define RET_NO_AVAILABLE    -11
/// 数据长度超出指定大小
#define RET_OVERSIZE        -12
/// 内部错误
#define RET_INTERNAL_ERROR  -13
/// 底层IO错误
#define RET_IO_ERROR        -14
/// 协议返回失败
#define RET_PROTOCOL_FAILED  -15
/// CRC校验失败
#define RET_ERROR_CRC        -16
/// 对象不存在
#define RET_NO_OBJECT        -17
/// 对象未初始化
#define RET_NO_INIT         -18
/// 对象忙
#define RET_BUSY            -19
/// 对象不匹配
#define RET_TYPE_MISMATCH   -20
/// 不可访问
#define RET_NO_ACCESS       -21
/// 已存在
#define RET_EXIST           -22

/// 用户使用的错误码
#define _RET_USER_ERROR_BASE  80

/// 用于自定义模块错误码，-80 至 -126
#define RET_USER(_offset) -(_RET_USER_ERROR_BASE + (_offset))


/** @} 返回值定义 */

#endif /* __ERROR_CODE_H__ */
