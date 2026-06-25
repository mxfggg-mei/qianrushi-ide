/**
 * MQTT 客户端封装
 * 负责与 EMQX Broker (WebSocket) 通信
 */
class MQTTClient {
    constructor(logger) {
        this.client = null;
        this.logger = logger;
        this.isConnected = false;
        this.statusCallbacks = [];
        this.messageCallbacks = [];
        this.statusTimer = null;
    }

    /**
     * 连接到 MQTT Broker
     * @param {string} serverType - 'external' | 'internal'
     */
    connect(serverType = 'external') {
        const server = AppConfig.servers[serverType];
        if (!server) {
            this.logger.add(`未知的服务器类型: ${serverType}`, 'error');
            return;
        }

        this.logger.add(`正在连接 MQTT 服务器: ${server.label} (${server.url})`, 'info');

        // 更新当前服务器
        AppConfig.currentServer = serverType;

        // 如果已连接，先断开
        if (this.client && this.isConnected) {
            this.disconnect();
        }

        // 创建 MQTT 客户端 (使用 WebSocket)
        const clientId = `${AppConfig.defaultDevice.deviceId}_web_${Math.random().toString(16).substr(2, 6)}`;

        const options = {
            ...AppConfig.mqttOptions,
            clientId,
            username: AppConfig.defaultDevice.username,
            password: AppConfig.defaultDevice.password,
            rejectUnauthorized: false  // 自签名证书场景
        };

        try {
            this.client = mqtt.connect(server.url, options);
        } catch (e) {
            this.logger.add(`MQTT 客户端创建失败: ${e.message}`, 'error');
            return;
        }

        this.bindEvents();
    }

    /**
     * 绑定事件
     */
    bindEvents() {
        this.client.on('connect', () => {
            this.isConnected = true;
            this.logger.add('MQTT 连接成功', 'success');

            this.notifyStatus(true);

            // 订阅状态主题
            this.subscribe(AppConfig.topics.status);

            // 启动定时获取状态（每5秒）
            this.startStatusTimer();
        });

        this.client.on('reconnect', () => {
            this.logger.add('MQTT 正在重连...', 'warn');
            this.notifyStatus(false, 'reconnecting');
        });

        this.client.on('close', () => {
            this.isConnected = false;
            this.logger.add('MQTT 连接已关闭', 'warn');
            this.notifyStatus(false, 'disconnected');
            // 停止定时获取状态
            this.stopStatusTimer();
        });

        this.client.on('error', (err) => {
            this.logger.add(`MQTT 错误: ${err.message}`, 'error');
        });

        this.client.on('offline', () => {
            this.logger.add('MQTT 离线', 'warn');
            this.notifyStatus(false, 'offline');
        });

        this.client.on('message', (topic, payload) => {
            this.handleMessage(topic, payload);
        });
    }

    /**
     * 处理接收到的消息
     */
    handleMessage(topic, payload) {
        const message = payload.toString();
        this.logger.add(`[${topic}] ${message}`, 'rx');

        try {
            const data = JSON.parse(message);
            this.notifyMessage(topic, data);
        } catch (e) {
            this.logger.add(`JSON 解析失败: ${e.message}`, 'error');
        }
    }

    /**
     * 订阅主题
     */
    subscribe(topic) {
        if (!this.client || !this.isConnected) {
            this.logger.add('MQTT 未连接，无法订阅', 'error');
            return;
        }

        this.client.subscribe(topic, { qos: 1 }, (err) => {
            if (err) {
                this.logger.add(`订阅失败: ${err.message}`, 'error');
            } else {
                this.logger.add(`已订阅: ${topic}`, 'success');
                // 订阅成功后请求一次最新状态
                this.requestStatus();
            }
        });
    }

    /**
     * 取消订阅
     */
    unsubscribe(topic) {
        if (!this.client || !this.isConnected) return;
        this.client.unsubscribe(topic);
    }

    /**
     * 发布命令
     * @param {object} cmd - 命令对象
     */
    publishCommand(cmd) {
        if (!this.client || !this.isConnected) {
            this.logger.add('MQTT 未连接，无法下发命令', 'error');
            return false;
        }

        const topic = AppConfig.topics.command;
        const payload = JSON.stringify(cmd);

        this.client.publish(topic, payload, { qos: 1 }, (err) => {
            if (err) {
                this.logger.add(`下发失败: ${err.message}`, 'error');
            } else {
                this.logger.add(`[${topic}] ${payload}`, 'tx');
            }
        });

        return true;
    }

    /**
     * 请求设备状态
     */
    requestStatus() {
        return this.publishCommand({ cmd: 'get_status' });
    }

    /**
     * 启动定时获取状态（每3秒）
     */
    startStatusTimer() {
        // 如果已有定时器，先停止
        this.stopStatusTimer();
        
        this.statusTimer = setInterval(() => {
            if (this.isConnected) {
                this.requestStatus();
            }
        }, 3000);
    }

    /**
     * 停止定时获取状态
     */
    stopStatusTimer() {
        if (this.statusTimer) {
            clearInterval(this.statusTimer);
            this.statusTimer = null;
        }
    }

    /**
     * 断开连接
     */
    disconnect() {
        // 停止定时获取状态
        this.stopStatusTimer();
        
        if (this.client) {
            this.client.end(true);
            this.client = null;
        }
        this.isConnected = false;
        this.notifyStatus(false);
    }

    /**
     * 通知状态变化
     */
    notifyStatus(connected, status = 'connected') {
        this.statusCallbacks.forEach(cb => cb(connected, status));
    }

    /**
     * 通知收到消息
     */
    notifyMessage(topic, data) {
        this.messageCallbacks.forEach(cb => cb(topic, data));
    }

    /**
     * 注册状态回调
     */
    onStatusChange(callback) {
        this.statusCallbacks.push(callback);
    }

    /**
     * 注册消息回调
     */
    onMessage(callback) {
        this.messageCallbacks.push(callback);
    }
}

window.MQTTClient = MQTTClient;
