/**
 * MQTT 配置文件
 * 集中管理 MQTT 连接参数
 */
const AppConfig = {
    // 设备默认配置（来自 mqtt_connect.h）
    defaultDevice: {
        get deviceId() {
            const input = document.getElementById('device-id-input');
            return input ? `PCT_100_${input.value}` : 'PCT_100_006';
        },
        username: 'dzdx_emqx',
        password: 'Jp4!sQ7$'
    },

    // 服务器配置
    servers: {
        external_ws: {
            url: 'ws://47.98.170.180:8083/mqtt',
            label: '外网 8083'
        },
        internal: {
            get url() {
                const ipInput = document.getElementById('internal-ip');
                const portInput = document.getElementById('internal-port');
                const ip = ipInput ? ipInput.value : '192.168.1.100';
                const port = portInput ? portInput.value : '8081';
                return `ws://${ip}:${port}/mqtt`;
            },
            label: '内网'
        }
    },

    // 当前选中的服务器类型
    currentServer: 'external_ws',

    // MQTT 主题模板
    topics: {
        // 根据 deviceId 动态生成
        get status() { return `chemctrl/${AppConfig.defaultDevice.deviceId}/status`; },
        get command() { return `chemctrl/${AppConfig.defaultDevice.deviceId}/command`; }
    },

    // MQTT 客户端配置
    mqttOptions: {
        clientId: `pct_web_${Math.random().toString(16).substr(2, 8)}`,
        keepalive: 60,
        clean: true,
        reconnectPeriod: 3000,
        connectTimeout: 10000,
        protocolVersion: 4,  // MQTT 3.1.1 (按照协议文档要求)
        username: 'dzdx_emqx',
        password: 'Jp4!sQ7$'
    },

    // 日志最大条数
    maxLogEntries: 200
};

// 暴露到 window
window.AppConfig = AppConfig;
