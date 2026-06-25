/**
 * 主入口文件
 * 初始化所有模块并建立连接
 */
(function() {
    'use strict';

    // 等待 DOM 加载完成
    document.addEventListener('DOMContentLoaded', () => {
        init();
    });

    function init() {
        // 初始化日志
        const logger = new Logger();
        window.logger = logger;

        logger.add('系统启动中...', 'info');
        logger.add(`设备ID: ${AppConfig.defaultDevice.deviceId}`, 'info');
        logger.add('提示: 打开右上角开关连接MQTT平台', 'info');

        // 初始化UI控制器
        const ui = new UIController(logger);
        window.ui = ui;

        // 初始化MQTT客户端
        const mqttClient = new MQTTClient(logger);
        window.mqttClient = mqttClient;

        // MQTT 状态变化 -> UI更新
        mqttClient.onStatusChange((connected, status) => {
            ui.updateMQTTStatus(connected, status);
        });

        // MQTT 消息 -> UI更新
        mqttClient.onMessage((topic, data) => {
            if (topic === AppConfig.topics.status) {
                ui.handleStatusMessage(data);
            }
        });

        // 日志开关
        const logSwitch = document.getElementById('log-switch');
        logSwitch.addEventListener('change', (e) => {
            logger.setEnabled(e.target.checked);
            if (e.target.checked) {
                logger.add('日志已启用', 'info');
            }
        });

        // 窗口卸载时断开连接
        window.addEventListener('beforeunload', () => {
            if (mqttClient.client) {
                mqttClient.disconnect();
            }
        });

        // 暴露给Electron API（如有）
        if (window.electronAPI) {
            logger.add('检测到 Electron 环境', 'info');
            window.electronAPI.onConnect(() => mqttClient.connect('external'));
            window.electronAPI.onDisconnect(() => mqttClient.disconnect());
        }

        // 模拟数据 - 用于离线预览（仅在没有MQTT连接时显示）
        enableOfflinePreview(ui);

        // 初始化主题切换
        initThemeToggle();

        logger.add('系统初始化完成', 'success');
    }

    /**
     * 初始化主题切换功能
     */
    function initThemeToggle() {
        // 从localStorage读取主题设置，默认深色
        const savedTheme = localStorage.getItem('app-theme') || 'dark';
        applyTheme(savedTheme);
    }

    function applyTheme(theme) {
        const body = document.body;
        const html = document.documentElement;
        if (theme === 'light') {
            body.classList.add('light-theme');
            body.classList.remove('dark-theme');
            html.classList.add('light-theme');
            html.classList.remove('dark-theme');
        } else {
            body.classList.add('dark-theme');
            body.classList.remove('light-theme');
            html.classList.add('dark-theme');
            html.classList.remove('light-theme');
        }
    }

    // 全局主题切换函数 - 供HTML onclick调用
    window.toggleTheme = function() {
        const body = document.body;
        const isLight = body.classList.contains('light-theme');
        const newTheme = isLight ? 'dark' : 'light';
        applyTheme(newTheme);
        localStorage.setItem('app-theme', newTheme);
    };

    /**
     * 离线预览模式 - 显示静态默认值
     */
    function enableOfflinePreview(ui) {
        const demoTemp = 25.5;
        const demoLight = 250;
        const demoMode = 'auto';
        const demoKey1 = false;
        const demoRelay3 = false;
        const demoRelay4 = false;

        // 未连接时显示静态默认值（仅初始化一次）
        setTimeout(() => {
            if (window.mqttClient && !window.mqttClient.isConnected) {
                const tempThreshold = parseFloat(document.getElementById('temp-threshold').value);
                const lightThreshold = parseInt(document.getElementById('light-threshold').value);
                ui.handleStatusMessage({
                    temperature: demoTemp,
                    light: demoLight,
                    mode: demoMode,
                    key1_lock: demoKey1,
                    relay3: demoRelay3,
                    relay4: demoRelay4,
                    temp_threshold: tempThreshold,
                    light_threshold: lightThreshold
                });
            }
        }, 500);
    }
})();
