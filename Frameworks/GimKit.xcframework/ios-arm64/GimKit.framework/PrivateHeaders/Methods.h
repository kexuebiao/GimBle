// (opid, bomi, bomr, invoke, reply, name, desc)
// bomi, bomr-> 0+, 长度; -1: 变长; -2: 方法不存在
// 'H': Host, 'D': Device

// 设备通用指令
METHOD(0xA0u, 00, 10, 'H', 'D', HANDSHAKE,                  "发起握手/发起握手过程")
METHOD(0xA1u, 16, 01, 'H', 'D', VERIFY_MD5,                 "MD5验证/Host对握手过程中收到的10字节随机数据加上Key做MD5散列，得到的16字节数据交给设备进行验证")
METHOD(0xA2u, 00, 01, 'H', 'D', GET_PROTO_REVISION,         "获取协议版本/获取通用电控蓝牙协议版本")
METHOD(0x22u, 00, -1, 'H', 'D', GET_COMPONENT_SN,           "设备概要/查询设备中各组件的版本号和SN")
METHOD(0x23u, 00, 04, 'H', 'D', GET_DEVICE_ID,              "查询设备ID/设备ID查询")
METHOD(0x27u, 00,  8, 'H', 'D', STAT_BIKE,                  "单车状态/单车类型和状态查询")
METHOD(0x28u, 01, 01, 'H', 'D', SWITCH_SERVICE_MODE,        "切换BLE服务/请求BLE服务变更，增加CPS+CSCS+FTMS")
METHOD(0x40u, 02, 02, 'H', 'D', SET_CONSOLE_STATE,          "设置表头状态/表头状态控制")
METHOD(0x43u, 23, 23, 'H', 'D', SET_RIDING_PARAM,           "设置骑行参数/设置骑行参数")
METHOD(0x44u, 01, 01, 'H', 'D', SET_TORQUE,                 "设置扭矩/扭矩设置")
METHOD(0x45u, 01, 01, 'H', 'D', SEIZE_KNOB_CONTROL,         "切换旋钮托管模式/设置旋钮托管状态")
METHOD(0x46u, 01, 01, 'H', 'D', SET_KNOB_DISPLAY_MODE,      "设置旋钮显示状态/设置旋钮显示状态")
METHOD(0x53u, 01, 01, 'H', 'D', SET_ERG_MODE,               "切换ERG模式/用于区分是否处于恒功率模式")

METHOD(0x31u, 00, 00, 'D', 'H', DISCONNECT_BLE,             "请求断开蓝牙/请求断开蓝牙")
METHOD(0x41u, -1, -1, 'D', 'H', NOTIFY_KEY_EVENT,           "上报按钮事件/按钮事件上报")
METHOD(0x47u, 00, 00, 'D', 'H', NOTIFY_BRAKE_EVENT,         "上报刹车事件")
METHOD(0x50u, 23, -2, 'D', 'H', NOTIFY_GIMKIT_DATA,         "上报实时数据/健身车实时数据上报")
METHOD(0x52u, 01, -2, 'D', 'H', NOTIFY_KNOB_ROTATION,       "上报旋钮转动事件/旋钮转动事件上报")

//力量设备
METHOD(0x70u, -1, -2, 'D', 'H', NOTIFY_STRENGTH_DATA,       "力量设备实时数据上报")
METHOD(0x71u, -1, -1, 'H', 'D', SET_STRENGTH_EQUIPMENT_MODE, "力量设备运行模式设置")

//划船器
METHOD(0x80u, -1, -2, 'D', 'H', NOTIFY_ROWERM_DATA,         "划船器实时数据上报")
METHOD(0x81u, -1, -1, 'H', 'D', SET_ROWERM_MODE,            "划船器运行模式设置")

//xbike
METHOD(0x55u, -1, -2, 'D', 'H', NOTIFY_XBIKE_DATA,       "xbike设备实时数据上报")



// MC(电机控制器) OTA指令
METHOD(0xD0u, -1, 02, 'H', 'D', MC_FIRMWARE_INFO,           "发送固件文件信息(大小和文件名)/用于升级固件时通知表表头固件的信息(电机控制器升级)")
METHOD(0xD1u, 02, 02, 'H', 'D', GET_BLE_MTU,                "获取设备支持的帧最大传输长度/用于升级固件时以确认的长度分多段传输")
METHOD(0xD2u, -1,  8, 'H', 'D', MC_FIRMWARE_CONTENT,        "发送升级文件数据/用于传输电机控制器固件文件内容(分多段，即多次传输)")
METHOD(0xD3u, 02, 02, 'H', 'D', MC_FIRMWARE_TRANSFER_END,   "升级文件传输结束/用于通知表头，传输电机控制器固件文件已结束")
METHOD(0xD4u, 02, 02, 'H', 'D', MC_FIRMWARE_RESULT,         "发送固件文件信息/用于升级固件时通知表表头固件的信息(电机控制器升级)")
METHOD(0xD5u, 00, 00, 'H', 'D', DB_REBOOT,                  "通知表头重启/用于通知表头，固件升级结束，表头需重启")
