/**
 * UI 控制器
 * 负责界面更新和用户交互
 */
class UIController {
    constructor(logger) {
        this.logger = logger;
        this.currentMode = 'auto';  // 当前模式
        this.lastTemp = null;
        this.lastLight = null;
        this.manualModeWarning = false;
        this.deviceTimeoutTimer = null;  // 设备消息超时计时器
        this.deviceTimeoutSeconds = 7;   // 超时时间（秒）

        this.cacheElements();
        this.bindEvents();
        
        // 初始化设备状态为离线
        this.updateDeviceStatus(false);
    }

    /**
     * 缓存DOM元素
     */
    cacheElements() {
        this.mqttStatus = document.getElementById('mqtt-status');
        this.deviceStatus = document.getElementById('device-status');
        this.mqttSwitch = document.getElementById('mqtt-switch');
        this.serverTabs = document.querySelectorAll('.server-tab');
        this.deviceId = document.getElementById('device-id');

        this.tempValue = document.getElementById('temp-value');
        this.lightValue = document.getElementById('light-value');
        this.tempCard = document.getElementById('temp-card');
        this.lightCard = document.getElementById('light-card');
        this.currentTemp = document.getElementById('current-temp');
        this.currentLight = document.getElementById('current-light');

        this.tempThreshold = document.getElementById('temp-threshold');
        this.lightThreshold = document.getElementById('light-threshold');
        this.numBtns = document.querySelectorAll('.num-btn');
        this.submitBtns = document.querySelectorAll('.submit-btn');

        this.modeBtns = document.querySelectorAll('.mode-btn');
        this.masterState = document.getElementById('master-state');
        this.relay3Switch = document.getElementById('relay3-switch');
        this.relay4Switch = document.getElementById('relay4-switch');
        this.lightActuator = document.getElementById('light-actuator');
        this.fanActuator = document.getElementById('fan-actuator');
        this.autoModeHint = document.getElementById('auto-mode-hint');

        this.toastContainer = document.getElementById('toast-container');

        // 窗口控制按钮
        this.minimizeBtn = document.getElementById('minimize-btn');
        this.maximizeBtn = document.getElementById('maximize-btn');
        this.closeBtn = document.getElementById('close-btn');
    }

    /**
     * 绑定事件
     */
    bindEvents() {
        // 设备 ID 输入框
        const deviceIdInput = document.getElementById('device-id-input');
        if (deviceIdInput) {
            deviceIdInput.addEventListener('change', (e) => {
                // 验证输入，确保是 3 位数字
                let value = e.target.value;
                if (!/^\d{3}$/.test(value)) {
                    // 如果格式不正确，恢复为默认值
                    e.target.value = '006';
                    this.showToast('设备 ID 必须是 3 位数字', 'warning');
                }
                this.showToast(`设备 ID 已更新为 PCT_100_${e.target.value}`, 'success');
            });
        }

        // MQTT 连接开关
        this.mqttSwitch.addEventListener('change', (e) => {
            if (e.target.checked) {
                window.mqttClient.connect(AppConfig.currentServer);
            } else {
                window.mqttClient.disconnect();
            }
        });

        // 服务器切换
        const internalConfig = document.getElementById('internal-config');
        this.serverTabs.forEach(tab => {
            tab.addEventListener('click', () => {
                if (tab.classList.contains('active')) return;

                this.serverTabs.forEach(t => t.classList.remove('active'));
                tab.classList.add('active');

                const server = tab.dataset.server;
                AppConfig.currentServer = server;

                // 显示/隐藏内网配置
                if (internalConfig) {
                    internalConfig.style.display = server === 'internal' ? 'flex' : 'none';
                }

                if (this.mqttSwitch.checked) {
                    window.mqttClient.connect(server);
                }
            });
        });

        // 模式切换
        this.modeBtns.forEach(btn => {
            btn.addEventListener('click', () => {
                if (btn.classList.contains('active')) return;

                const mode = btn.dataset.mode;
                this.setMode(mode);

                // 下发命令
                window.mqttClient.publishCommand({
                    cmd: 'set_mode',
                    mode: mode
                });

                this.showToast(`已切换到${mode === 'auto' ? '自动' : '手动'}模式`, 'success');
            });
        });

        // 继电器开关
        this.relay3Switch.addEventListener('change', (e) => {
            if (this.currentMode === 'auto') {
                e.preventDefault();
                e.target.checked = !e.target.checked;
                this.showToast('自动模式下无法手动控制继电器', 'warning');
                return;
            }
            window.mqttClient.publishCommand({
                cmd: 'set_relay',
                relay: 3,
                value: e.target.checked
            });
        });

        this.relay4Switch.addEventListener('change', (e) => {
            if (this.currentMode === 'auto') {
                e.preventDefault();
                e.target.checked = !e.target.checked;
                this.showToast('自动模式下无法手动控制继电器', 'warning');
                return;
            }
            window.mqttClient.publishCommand({
                cmd: 'set_relay',
                relay: 4,
                value: e.target.checked
            });
        });

        // 数值加减按钮
        this.numBtns.forEach(btn => {
            btn.addEventListener('click', () => {
                const target = document.getElementById(btn.dataset.target);
                const step = parseFloat(btn.dataset.step);
                const current = parseFloat(target.value) || 0;
                const newValue = Math.max(parseFloat(target.min), Math.min(parseFloat(target.max), current + step));
                target.value = newValue;
            });
        });

        // 阈值下发
        this.submitBtns.forEach(btn => {
            btn.addEventListener('click', () => {
                const cmd = btn.dataset.cmd;
                const type = btn.dataset.type;

                if (cmd === 'set_threshold') {
                    const cmdObj = { cmd: 'set_threshold' };
                    if (type === 'temp') {
                        cmdObj.temp = parseFloat(this.tempThreshold.value);
                    } else if (type === 'light') {
                        cmdObj.light = parseInt(this.lightThreshold.value);
                    }
                    window.mqttClient.publishCommand(cmdObj);
                    this.showToast('阈值下发成功', 'success');
                }
            });
        });

        // 窗口控制按钮
        if (this.minimizeBtn) {
            this.minimizeBtn.addEventListener('click', () => {
                if (window.require) {
                    const { ipcRenderer } = window.require('electron');
                    ipcRenderer.send('minimize-window');
                }
            });
        }

        if (this.maximizeBtn) {
            this.maximizeBtn.addEventListener('click', () => {
                if (window.require) {
                    const { ipcRenderer } = window.require('electron');
                    ipcRenderer.send('maximize-window');
                }
            });
        }

        if (this.closeBtn) {
            this.closeBtn.addEventListener('click', () => {
                if (window.require) {
                    const { ipcRenderer } = window.require('electron');
                    ipcRenderer.send('close-window');
                }
            });
        }
    }

    /**
     * 更新 MQTT 连接状态
     */
    updateMQTTStatus(connected, status) {
        if (connected) {
            this.mqttStatus.classList.add('connected');
            this.mqttStatus.classList.remove('disconnected');
            this.mqttStatus.querySelector('.status-text').textContent = '已连接';
            this.mqttSwitch.checked = true;
        } else {
            this.mqttStatus.classList.remove('connected');
            this.mqttStatus.classList.add('disconnected');
            const statusText = {
                reconnecting: '重连中',
                disconnected: '已断开',
                offline: '离线'
            }[status] || '未连接';
            this.mqttStatus.querySelector('.status-text').textContent = statusText;
            this.mqttSwitch.checked = false;
            // 断开连接时重置阈值初始化状态
            this.thresholdInitialized = false;
            // 断开连接时设备也离线
            this.updateDeviceStatus(false);
        }
    }

    /**
     * 处理设备状态消息
     */
    handleStatusMessage(data) {
        // 温度
        if (typeof data.temperature === 'number') {
            this.updateSensorValue(this.tempValue, data.temperature.toFixed(1), this.lastTemp);
            this.lastTemp = data.temperature;

            if (typeof data.temp_threshold === 'number') {
                this.updateTempWarning(data.temperature, data.temp_threshold);
            }
        }

        // 光照
        if (typeof data.light === 'number') {
            this.updateSensorValue(this.lightValue, Math.round(data.light), this.lastLight);
            this.lastLight = data.light;

            if (typeof data.light_threshold === 'number') {
                this.updateLightWarning(data.light, data.light_threshold);
            }
        }

        // 模式
        if (data.mode) {
            this.setMode(data.mode, false);
        }

        // 总闸 (系统使能)
        if (typeof data.key1_lock === 'boolean') {
            this.updateMasterState(data.key1_lock);
        }

        // 继电器状态
        if (typeof data.relay3 === 'boolean') {
            this.updateRelayState(this.relay3Switch, this.lightActuator, data.relay3);
        }
        if (typeof data.relay4 === 'boolean') {
            this.updateRelayState(this.relay4Switch, this.fanActuator, data.relay4);
        }

        // 当前阈值显示 - 显示设备当前生效的阈值
        if (typeof data.temp_threshold === 'number') {
            this.currentTemp.textContent = data.temp_threshold.toFixed(1);
            // 只在连接成功后首次收到消息时初始化输入框
            if (!this.thresholdInitialized) {
                this.tempThreshold.value = data.temp_threshold;
            }
        }
        if (typeof data.light_threshold === 'number') {
            this.currentLight.textContent = data.light_threshold.toString();
            // 只在连接成功后首次收到消息时初始化输入框
            if (!this.thresholdInitialized) {
                this.lightThreshold.value = data.light_threshold;
            }
        }
        // 标记阈值已初始化
        this.thresholdInitialized = true;

        // 更新设备状态 - 只有MQTT连接成功后收到消息才标记设备在线
        if (window.mqttClient && window.mqttClient.isConnected) {
            this.updateDeviceStatus(true);
        }
    }

    /**
     * 更新设备状态
     */
    updateDeviceStatus(online) {
        if (this.deviceStatus) {
            if (online) {
                // 设备在线 - 重置超时计时器
                this.resetDeviceTimeout();
                
                this.deviceStatus.classList.add('connected');
                this.deviceStatus.classList.remove('disconnected');
                this.deviceStatus.querySelector('.status-text').textContent = '在线';
            } else {
                // 设备离线 - 清除超时计时器
                this.clearDeviceTimeout();
                
                this.deviceStatus.classList.remove('connected');
                this.deviceStatus.classList.add('disconnected');
                this.deviceStatus.querySelector('.status-text').textContent = '离线';
            }
        }
    }

    /**
     * 重置设备消息超时计时器
     */
    resetDeviceTimeout() {
        // 清除之前的计时器
        this.clearDeviceTimeout();
        
        // 设置新的超时计时器
        this.deviceTimeoutTimer = setTimeout(() => {
            this.logger.add('设备消息超时，标记为离线', 'warn');
            this.updateDeviceStatus(false);
        }, this.deviceTimeoutSeconds * 1000);
    }

    /**
     * 清除设备消息超时计时器
     */
    clearDeviceTimeout() {
        if (this.deviceTimeoutTimer) {
            clearTimeout(this.deviceTimeoutTimer);
            this.deviceTimeoutTimer = null;
        }
    }

    /**
     * 更新传感器数值（带动画）
     */
    updateSensorValue(element, value, lastValue) {
        if (lastValue !== null && parseFloat(value) !== lastValue) {
            element.classList.add('updating');
            setTimeout(() => element.classList.remove('updating'), 400);
        }
        element.textContent = value;
    }

    /**
     * 温度预警
     */
    updateTempWarning(temp, threshold) {
        this.tempCard.classList.remove('warning', 'danger');
        if (temp >= threshold + 5) {
            this.tempCard.classList.add('danger');
        } else if (temp >= threshold) {
            this.tempCard.classList.add('warning');
        }
    }

    /**
     * 光照预警
     */
    updateLightWarning(light, threshold) {
        this.lightCard.classList.remove('warning', 'danger');
        if (light < threshold * 0.5) {
            this.lightCard.classList.add('danger');
        } else if (light < threshold) {
            this.lightCard.classList.add('warning');
        }
    }

    /**
     * 设置模式
     */
    setMode(mode, send = true) {
        this.currentMode = mode;

        this.modeBtns.forEach(btn => {
            btn.classList.toggle('active', btn.dataset.mode === mode);
        });

        // 自动模式下禁用手动控制
        const isAuto = mode === 'auto';
        this.relay3Switch.disabled = isAuto;
        this.relay4Switch.disabled = isAuto;
        this.autoModeHint.classList.toggle('hidden', !isAuto);

        if (isAuto) {
            this.lightActuator.style.opacity = '0.7';
            this.fanActuator.style.opacity = '0.7';
        } else {
            this.lightActuator.style.opacity = '1';
            this.fanActuator.style.opacity = '1';
        }
    }

    /**
     * 更新总闸状态
     */
    updateMasterState(on) {
        this.masterState.textContent = on ? '开启' : '断开';
        this.masterState.classList.toggle('on', on);
    }

    /**
     * 更新继电器状态
     */
    updateRelayState(switchEl, cardEl, on) {
        switchEl.checked = on;
        cardEl.classList.toggle('active', on);
    }

    /**
     * 显示Toast提示
     */
    showToast(message, type = 'info') {
        const toast = document.createElement('div');
        toast.className = `toast ${type}`;
        toast.textContent = message;

        this.toastContainer.appendChild(toast);

        setTimeout(() => {
            toast.classList.add('removing');
            setTimeout(() => toast.remove(), 300);
        }, 3000);
    }
}

window.UIController = UIController;
