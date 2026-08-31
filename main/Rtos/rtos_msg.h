#ifndef RTOS_MSG_H
#define RTOS_MSG_H

#include "stdint.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

typedef enum {
    MSG_UART = 0,          // UART通信模块
    MSG_WIFI,              // WIFI模块
    MSG_MQTT,              // MQTT模块
    MSG_BLE,               // BLE模块
    MSG_BS21,              // BS21模块
    MSG_ALARM,             // 报警模块
    MSG_OTA,               // OTA模块
    MSG_AUDIO,             // 音频模块
    MSG_POWER,             // 电源模块
    MSG_STA_MAIN,          // sta_main模块（工作模式切换）
    MSG_SCHEDULE,          // 定时调度模块（营业时间/自动重启）
    MSG_SERVER,            // 网页服务器模块
    MSG_NET_API,           // 网络接口模块
    MSG_ULTRASONIC,         // 超声波模块
    MSG_TYPE_MAX,
} Task_Msg_Type_Enu;

typedef enum {
    WIFI_TASK = 0,
    STAMAIN_TASK,  // sta_main模块任务
    AUDIO_TASK,     // 音频模块任务
    TASK_TYPE_MAX,
}TaskType_Enu;

typedef struct {
    uint16_t type;
    uint16_t subtype;
    union{
        uint32_t  param;
        int32_t   sparam;
        void     *buf;
    } data;
}IO_MSG_t;


#define WIFI_TASK_MSG_QUEUE_LENGTH          50
#define WIFI_TASK_MSG_QUEUE_SIZE            sizeof(IO_MSG_t)

#define STAMAIN_TASK_MSG_QUEUE_LENGTH       50
#define STAMAIN_TASK_MSG_QUEUE_SIZE         sizeof(IO_MSG_t)

#define AUDIO_TASK_MSG_QUEUE_LENGTH          50
#define AUDIO_TASK_MSG_QUEUE_SIZE            sizeof(IO_MSG_t)

uint8_t Rtos_msg_create(void);

BaseType_t Rtos_msg_send(TaskType_Enu task_type, IO_MSG_t *msg, TickType_t ticks_to_wait);
BaseType_t Rtos_msg_send_from_isr(TaskType_Enu task_type, IO_MSG_t *msg, BaseType_t *pxHigherPriorityTaskWoken);

BaseType_t Rtos_msg_receive(TaskType_Enu task_type, IO_MSG_t *msg, TickType_t ticks_to_wait);

// ============ 各模块子命令枚举 ============

// BS21模块子命令
typedef enum {
    BS21_SUB_WHITELIST_UPDATE = 0,  // 名单更新
    BS21_SUB_RANGING_CTRL,          // 测距控制
    BS21_SUB_CONFIG_UPDATE,         // BLE配置更新通知（blePower/bleMac 变更）
    BS21_SUB_DUTY_START,            // 营业开始时间触发
    BS21_SUB_DUTY_END,              // 营业结束时间触发
} Msg_BS21_SubType_Enu;

// OTA模块子命令
typedef enum {
    OTA_SUB_START = 0,    // 启用OTA模式（param：channel + device_type 打包）
    OTA_SUB_STOP,         // 停止OTA
    OTA_SUB_FAIL,         // OTA失败
    OTA_SUB_SUCCESS,      // OTA成功
} Msg_OTA_SubType_Enu;


// BUZZER模块子命令
typedef enum {
    BUZZER_SUB_SET_VOLUME = 0,  // 设置蜂鸣器音量
} Msg_Buzzer_SubType_Enu;

// BLE模块子命令
typedef enum {
    BLE_SUB_START_ADV = 0,  // 开始广播
    BLE_SUB_STOP,           // 停止BLE
} Msg_BLE_SubType_Enu;

// WIFI模块子命令
typedef enum {
    WIFI_SUB_CONNECTED = 0,    // 连接成功
    WIFI_SUB_DISCONNECT,       // 断开当前连接
    WIFI_SUB_STOP,             // 网络停止
    WIFI_SUB_GOT_IP,           // 获取IP
    WIFI_SUB_OTA_DOWNLOAD,     // 固件申请下载
    WIFI_SUB_SET_NET_STATE,    // 网络状态切换（param: SysNet_State_Enu）
    WIFI_SUB_STA_DISCONNECTED, // STA 断链事件（来自 WiFi 事件回调）
    WIFI_SUB_START_APSTA,      // 启动 AP+STA 双模式（正常工作）
    WIFI_SUB_STOP_AP_ONLY,     // 停止 AP，保持 STA
    WIFI_SUB_RESUME_AP,        // 恢复 AP（在 STA 基础上）
} Msg_WIFI_SubType_Enu;

// MQTT模块子命令
typedef enum {
    MQTT_SUB_START = 0,
    MQTT_SUB_STOP,
    MQTT_SUB_RESTART,
} Msg_MQTT_SubType_Enu;

// Audio模块子命令
typedef enum {
    AUDIO_SUB_PLAY = 0,       // 播放指定音频
    AUDIO_SUB_STOP,           // 停止播放
    AUDIO_SUB_PAUSE,          // 暂停播放
    AUDIO_SUB_RESUME,         // 恢复播放
    AUDIO_SUB_RECORD,         // 录音
    AUDIO_SUB_SET_VOLUME,     // 设置音量
    AUDIO_SUB_VOLUME_UPDATE,  // 音量配置批量刷新（从全局配置重新加载）
    AUDIO_SUB_VOICE_LIST_UPDATE, // 语音配置列表变更通知
} Msg_Audio_SubType_Enu;

// 电源模块子命令
typedef enum {
    POWER_SUB_CHARGE_STATUS = 0,  // 充放电状态上报
    POWER_SUB_BATTERY_REPORT,     // 电量上报
} Msg_Power_SubType_Enu;

// 报警模块子命令
typedef enum {
    ALARM_SUB_INFO_PROCESS = 0, // 报警信息处理
    ALARM_SUB_INFO_CLEAR,       // 报警解除
    ALARM_SUB_CONFIG_UPDATE,    // 告警模块配置更新（autoCancel/lowBattery 变更通知）
} Msg_Alarm_SubType_Enu;

// sta_main模块子命令
typedef enum {
    STA_MAIN_SUB_MODE_SWITCH = 0,  // 工作模式切换
    STA_MAIN_SUB_WIFI_IDLE,        // WiFi已停止反馈
    STA_MAIN_SUB_BLE_IDLE,         // BLE已停止反馈
    STA_MAIN_SUB_AUDIO_IDLE,       // Audio已停止反馈
    STA_MAIN_SUB_OTA_REQUEST,      // OTA启动请求（param：channel + device_type 打包）
    STA_MAIN_SUB_HOST_INFO_UPDATE, // 主机信息更新通知（来自MQTT服务器推送）
} Msg_StaMain_SubType_Enu;

// 定时调度模块子命令
typedef enum {
    SCHEDULE_SUB_AUTO_REBOOT = 0,     // 自动重启触发（时间到了发出的）
    SCHEDULE_SUB_REBOOT_ENABLE,       // 控制是否启用自动重启（param: 0=禁用, 1=启用）
    SCHEDULE_SUB_DUTY_ENABLE,         // 控制营业时间开关（param: 0=禁用, 1=启用）
    SCHEDULE_SUB_CONFIG_UPDATE,       // 配置更新（重新从 App_Server_Config 加载时间参数）
} Msg_Schedule_SubType_Enu;

// 网页服务器模块子命令
typedef enum {
    SERVER_SUB_START = 0,           // 启动HTTP服务器
    SERVER_SUB_STOP,                // 停止HTTP服务器
    SERVER_SUB_OTA_PROGRESS,        // OTA进度更新（param: 百分比0-100）
    SERVER_SUB_OTA_COMPLETE,        // OTA完成（param: 0=成功, 1=失败）
    SERVER_SUB_AUDIO_COMPLETE,      // 音频播放完成
} Msg_Server_SubType_Enu;

#endif /* RTOS_MSG_H */
