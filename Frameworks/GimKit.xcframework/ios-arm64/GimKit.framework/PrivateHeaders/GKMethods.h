//
// Created by Wang Haibing on 3/29/21.
//
#if !defined __STDC_VERSION__ || __STDC_VERSION__ < 199901L
#error "Please use a C99 compliant toolchain."
#endif

#ifndef GIMKIT_GKMETHODS_H
#define GIMKIT_GKMETHODS_H

#include <stdint.h> // for uint32_t, uint8_t, etc
#include <stdbool.h> // for bool, true, false

// Remember to keep them synchronized with NativeBridge.java
typedef enum {
    PREP_CANCELED = 0,
    PREP_TO_RIDE = 1,
    RIDING_STARTED = 2,
    RIDING_PAUSED = 3,
    RIDING_RESUMED = 4,
    RIDING_STOPPED = 5
} RidingState;

typedef struct KeyEvent {
    /**
     *  0x01: 表头按钮(短按), 0x02: 表头按钮(长按)
     */
    uint8_t code;
    /**
     * 0x01: press(按下), 0x02: release(释放)
     */
    uint8_t state;
}KeyEvent;

typedef struct DeviceComponentInfo {
    uint16_t address;
    uint8_t type;
    struct {
        uint8_t hardware;
        uint8_t flash;
        uint8_t eeprom;
    } version;
    uint8_t sn[15];
} DeviceComponentInfo;

typedef struct RidingParams {
    uint32_t mass;
    uint32_t wheel_diameter;
    uint32_t wind_resistance_coefficient;
    uint32_t rolling_friction_coefficient;
    uint16_t angle;
    uint16_t efficiency;
    uint8_t chain_wheel;
    uint8_t rear_wheel;
    uint8_t gear;
} RidingParams;

typedef struct GimkitData {
    /**
     * Distance, accumulated: in meter
     */
    uint32_t distance;
    /**
     * Calorie, accumulated: in cal
     */
    uint32_t calorie;
    /**
     * Cadence: Revolutions Per Minute.
     */
    uint16_t cadence;
    /**
     * Speed: Meters Per Hour.
     */
    uint16_t speed;
    /**
     * Power, in 0.1 power
     */
    uint16_t power;
    /**
     * State: the riding state. See {@link RidingState}
     */
    uint16_t state;
    /**
     * Motor state
     */
    uint16_t motor;
    /**
     * Temperature, in degree celsius, divided by 10 get the real value.
     */
    uint16_t temperature;
    /**
     * Heart Rate, beat per minute
     */
    uint8_t bpm;
    /**
     * Gear
     */
    uint8_t gear;
    /**
     * Torque, in 0.5 N·m
     */
    uint8_t torque;
} GimkitData;

/**
 * based on《力量设备上控板蓝牙通讯协议_V1.2.pdf》
 */
typedef struct StrengthData {
    uint32_t flag;      //bit0对应实时拉力数据，bit0为0表示无此数据，bit0为1表示有此数据。bit1：实时速度，bit2：实时功率，bit3：实时位置，bit4：电机状态，bit5：电机温度，bit6-bit31：undefined
    uint16_t tension;   //实时拉力 1N
    uint16_t speed;      //实时速度（实际是有符号的，java侧要做一下类型转换）0.001m/s
    uint16_t power;     //实时功率 0.1W
    uint16_t position;  //实时位置 0.001m
    uint16_t mcStatus;  //电机状态
    uint16_t mcTemperature; //电机温度 0.1摄氏度℃
} StrengthData;

/**
 * based on 《划船器中控蓝牙通信协议_v1.4.pdf》
 */
 typedef struct RowermData {
     uint32_t flag;     //bit0对应扭矩Torque数据，bit0为1表示有此数据，为0表示无此数据。bit1对应模式Mode数据，bit2对应挡位Level数据，...
     uint16_t torqueSet;   //划船器设置扭矩，单位0.5Nm
     uint8_t mode;      //模式
     uint8_t level;     //挡位
     uint16_t torqueRT; //EMC实时扭矩，单位0.5Nm
     uint16_t force;    //拉力，单位0.1Kg
     uint8_t direction; //拉绳方向 0:拉绳静止，1:拉绳拉出，2:拉绳回收
     uint16_t drawstringDistance;   //拉绳行程，单位0.01m
     uint16_t lastPullTime;     //最后一次拉绳拉出时间，单位ms
     uint16_t lastPushTime;     //最后一次拉绳回收时间，单位ms
     uint16_t strokeRate;       //桨频，单位bpm
     uint16_t shipSpeed;        //船速，单位0.1m/s
     uint16_t power;            //功率，单位0.1W
     uint16_t EMCSpeed;         //EMC转速，单位rpm
     uint16_t EMCCurrent;       //EMC电流，单位mA
     uint16_t EMCVoltage;       //EMC电压，单位mV
     uint16_t EMCTemperature;    //EMC温度，单位0.1摄氏度℃（实际是有符号的，java侧要做一下类型转换）
     uint16_t status;           //EMC状态
 } RowermData;

/**
* based on《xbike蓝牙通讯协议_V1.1.pdf》
*/
typedef struct XbikeData {
    uint32_t flag;      //bit0对应踏频数据，bit0为0表示无此数据，bit0为1表示有此数据。bit1：实时功率，bit2：实时扭矩，bit3：心率，bit4：累计总里程，bit5：累计总卡路里，bit6：电池状态，bit7-bit31：undefined
    uint16_t cadence;   //踏频 rpm
    uint16_t power;     //实时功率 0.1W
    uint16_t torque;    //实时扭矩 0.1Nm
    uint8_t heartRate;  //心率 bpm
    uint32_t totalDistance; //总里程 m
    uint32_t totalCalorie;  //总卡路里 cal
    uint8_t battStatus;     //电池状态
    uint8_t intensity;     // 强度等级(与路线骑行的档位(齿比)不同)
} XbikeData;

typedef struct GKMethodTable {
    void (*onForwardFrameRequest)(const void *any, const uint8_t *frame, size_t count);
    void (*onReceivedMethodCrcError)(const void *any, const uint8_t *frame, size_t count, uint16_t crcReceived, uint16_t crcCalculated);
    void (*onReceivedMethodProtocolError)(const void *any, const uint8_t *frame, size_t count);

    void (*onMethodDisconnectBleInvoke)(const void *any);
    void (*onMethodNotifyKeyEventInvoke)(const void *any, const KeyEvent * keyEvent, size_t count);
    void (*onMethodNotifyBrakeEventInvoke)(const void *any);
    void (*onMethodNotifyGimKitDataInvoke)(const void *any, const GimkitData *data);
    void (*onMethodNotifyKnobRotationInvoke)(const void *any, int rotation);

    void (*onMethodHandshakeReply)(const void *any, const uint8_t *random, size_t count);
    void (*onMethodHandshakeReplyError)(const void *any, int error);

    void (*onMethodVerifyMd5Reply)(const void *any, uint8_t result);
    void (*onMethodVerifyMd5ReplyError)(const void *any, int error, const uint8_t md5[16], size_t count);

    void (*onMethodGetProtocolRevisionReply)(const void *any, uint8_t revision);
    void (*onMethodGetProtocolRevisionReplyError)(const void *any, int error);

    void (*onMethodGetComponentSnReply)(const void *any, const DeviceComponentInfo *info, size_t count);
    void (*onMethodGetComponentSnReplyError)(const void *any, int error);

    void (*onMethodGetDeviceIdReply)(const void *any, uint32_t deviceId);
    void (*onMethodGetDeviceIdReplyError)(const void *any, int error);

    void (*onMethodStatBikeReply)(const void *any, uint8_t type, uint8_t voltage, uint8_t temperature, uint8_t powerSource);
    void (*onMethodStatBikeReplyError)(const void *any, int error);

    void (*onMethodSwitchServiceModeReply)(const void *any, uint8_t mode);
    void (*onMethodSwitchServiceModeReplyError)(const void *any, int error, uint8_t mode);

    void (*onMethodSetConsoleStateReply)(const void *any, uint16_t state);
    void (*onMethodSetConsoleStateReplyError)(const void *any, int error, uint16_t state);

    void (*onMethodSetRidingParamReply)(const void *any, const RidingParams *rp);
    void (*onMethodSetRidingParamReplyError)(const void *any, int error, const RidingParams *rp);

    void (*onMethodSetTorqueReply)(const void *any, uint8_t torque);
    void (*onMethodSetTorqueReplyError)(const void *any, int error, uint8_t torque);

    void (*onMethodSeizeKnobControlReply)(const void *any, uint8_t mode);
    void (*onMethodSeizeKnobControlReplyError)(const void *any, int error, uint8_t mode);

    void (*onMethodSetKnobDisplayModeReply)(const void *any, uint8_t mode);
    void (*onMethodSetKnobDisplayModeReplyError)(const void *any, int error, uint8_t mode);

    void (*onMethodSetErgModeReply)(const void *any, uint8_t erg);
    void (*onMethodSetErgModeReplyError)(const void *any, int error, uint8_t erg);

    //strength equipment
    void (*onMethodNotifyStrengthDataInvoke)(const void *any, const StrengthData *data);
    void (*onMethodSetStrengthEquipmentModeReply)(const void *any, const uint8_t *data, size_t count);
    void (*onMethodSetStrengthEquipmentModeReplyError)(const void *any, int error, const uint8_t *data, size_t count);
    void (*onMethodNotifyRowermDataInvoke)(const void *any, const RowermData *data);
    void (*onMethodSetRowermModeReply)(const void *any, const uint8_t *data, size_t count);
    void (*onMethodSetRowermModeReplyError)(const void *any, int error, const uint8_t *data, size_t count);
    void (*onMethodNotifyXbikeDataInvoke)(const void *any, const XbikeData *data);

    void (*onMethodMcFirmwareInfoReply)(const void *any, uint8_t result);
    void (*onMethodMcFirmwareInfoReplyError)(const void *any, int error, uint8_t result);
    
    void (*onMethodGetMtuReply)(const void *any, uint8_t size);
    void (*onMethodGetMtuReplyError)(const void *any, int error, uint8_t size);
    
    void (*onMethodMcFirmwareContentReply)(const void *any, uint8_t result);
    void (*onMethodMcFirmwareContentReplyError)(const void *any, int error, uint8_t result);
    
    void (*onMethodMcFirmwareTransferEndReply)(const void *any, uint8_t result);
    void (*onMethodMcFirmwareTransferEndReplyError)(const void *any, int error, uint8_t result);
    
    void (*onMethodMcFirmwareResultReply)(const void *any, uint8_t result);
    void (*onMethodMcFirmwareResultReplyError)(const void *any, int error, uint8_t result);


} GKMethodTable;

typedef void (*MethodAssembledCallback)(uint8_t *frame, size_t count, const void *any);

size_t disassembleMethod(const void *any, const GKMethodTable *mtab, const uint8_t *data, size_t count);

bool assembleMethodSetRidingParamsInvoke(MethodAssembledCallback cb, const void *any, const RidingParams *params);

bool assembleMethodSetTorqueInvoke(double torque, MethodAssembledCallback cb, const void *any);

bool assembleMethodGetComponentSnInvoke(MethodAssembledCallback cb, const void *any);

bool assembleMethodHandshakeInvoke(MethodAssembledCallback cb, const void *any);

bool assembleMethodVerifyMd5Invoke(MethodAssembledCallback cb, const void *any, uint8_t *md5);

bool assembleMethodGetProtocolRevisionInvoke(MethodAssembledCallback cb, const void *any);

bool assembleMethodGetDeviceIdInvoke(MethodAssembledCallback cb, const void *any);

bool assembleMethodStatBikeRequestInvoke(MethodAssembledCallback cb, const void *any);

bool assembleMethodSwitchServiceModeInvoke(MethodAssembledCallback cb, const void *any);

bool assembleMethodSetConsoleStateInvoke(MethodAssembledCallback cb, const void *any, uint16_t state);

bool assembleMethodSeizeKnobControlInvoke(MethodAssembledCallback cb, const void *any, uint8_t mode);

bool assembleMethodSetKnobDisplayModeInvoke(MethodAssembledCallback cb, const void *any, uint8_t mode);

bool assembleMethodSetErgModeInvoke(MethodAssembledCallback cb, const void *any, uint8_t mode);

bool assembleMethodMcFirmwareInfoInvoke(MethodAssembledCallback cb, const void *any, uint8_t *fileInfo, size_t length);

bool assembleMethodMcFirmwareContentInvoke(MethodAssembledCallback cb, const void *any, uint8_t *content, size_t length);

bool assembleMethodGetMtuInvoke(MethodAssembledCallback cb, const void *any);

bool assembleMethodMcFirmwareTransferEndInvoke(MethodAssembledCallback cb, const void *any);

bool assembleMethodMcFirmwareResultInvoke(MethodAssembledCallback cb, const void *any);

bool assembleMethodDbRebootInvoke(MethodAssembledCallback cb, const void *any);

//strength equipment
bool assembleMethodSetStrengthEquipmentModeInvoke(MethodAssembledCallback cb, const void *any, uint8_t *params, size_t length);
//rowerm
bool assembleMethodSetRowermModeInvoke(MethodAssembledCallback cb, const void *any, uint8_t *params, size_t length);

#endif //GIMKIT_GKMETHODS_H
