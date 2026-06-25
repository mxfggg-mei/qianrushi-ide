/**
 * 日志模块
 * 负责记录、显示和管理调试日志
 */
class Logger {
    constructor() {
        this.logContent = document.getElementById('log-content');
        this.logCount = document.getElementById('log-count');
        this.logSwitch = document.getElementById('log-switch');
        this.clearBtn = document.getElementById('clear-log');
        this.entries = [];
        this.maxEntries = AppConfig.maxLogEntries;
        this.enabled = true;

        this.clearBtn.addEventListener('click', () => this.clear());
    }

    /**
     * 添加日志
     * @param {string} message - 日志内容
     * @param {string} tag - 日志标签: info|success|warn|error|tx|rx
     */
    add(message, tag = 'info') {
        if (!this.enabled) return;

        const entry = {
            time: new Date(),
            message,
            tag
        };

        this.entries.push(entry);
        if (this.entries.length > this.maxEntries) {
            this.entries.shift();
        }

        this.render(entry);
        this.updateCount();
        this.scrollToBottom();
    }

    /**
     * 渲染单条日志
     */
    render(entry) {
        const logEntry = document.createElement('div');
        logEntry.className = 'log-entry';

        const timeStr = entry.time.toTimeString().slice(0, 8);
        const tagLabel = this.getTagLabel(entry.tag);

        logEntry.innerHTML = `
            <span class="log-time">${timeStr}</span>
            <span class="log-tag ${entry.tag}">${tagLabel}</span>
            <span class="log-message">${this.formatMessage(entry.message)}</span>
        `;

        this.logContent.appendChild(logEntry);

        // 限制DOM节点数
        while (this.logContent.children.length > this.maxEntries) {
            this.logContent.removeChild(this.logContent.firstChild);
        }
    }

    /**
     * 格式化消息
     */
    formatMessage(msg) {
        if (typeof msg === 'object') {
            return JSON.stringify(msg, null, 0);
        }
        return this.escapeHtml(String(msg));
    }

    /**
     * HTML转义
     */
    escapeHtml(str) {
        const div = document.createElement('div');
        div.textContent = str;
        return div.innerHTML;
    }

    /**
     * 获取标签显示文本
     */
    getTagLabel(tag) {
        const labels = {
            info: 'INFO',
            success: 'OK',
            warn: 'WARN',
            error: 'ERR',
            tx: 'TX',
            rx: 'RX'
        };
        return labels[tag] || 'INFO';
    }

    /**
     * 更新计数
     */
    updateCount() {
        this.logCount.textContent = this.entries.length;
    }

    /**
     * 滚动到底部
     */
    scrollToBottom() {
        this.logContent.scrollTop = this.logContent.scrollHeight;
    }

    /**
     * 清空日志
     */
    clear() {
        this.entries = [];
        this.logContent.innerHTML = '';
        this.updateCount();
    }

    /**
     * 启用/禁用日志
     */
    setEnabled(enabled) {
        this.enabled = enabled;
    }
}

// 便捷方法
window.Logger = Logger;
