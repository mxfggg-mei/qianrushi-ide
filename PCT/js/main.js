history.scrollRestoration = 'manual';

document.addEventListener('DOMContentLoaded', () => {
    window.scrollTo({ top: 0, left: 0, behavior: 'instant' });
    
    const closeBtn = document.getElementById('close-btn');
    const minimizeBtn = document.getElementById('minimize-btn');
    const themeBtn = document.getElementById('theme-btn');
    const currentTimeEl = document.getElementById('current-time');
    const todoBadge = document.getElementById('todo-badge');
    const { ipcRenderer } = window.require('electron');
    
    closeBtn.addEventListener('click', () => {
        ipcRenderer.send('close-window');
    });
    
    minimizeBtn.addEventListener('click', () => {
        ipcRenderer.send('minimize-window');
    });
    
    function updateTime() {
        const now = new Date();
        const hours = String(now.getHours()).padStart(2, '0');
        const minutes = String(now.getMinutes()).padStart(2, '0');
        currentTimeEl.textContent = `${hours}:${minutes}`;
    }
    
    updateTime();
    setInterval(updateTime, 1000);
    
    function updateBadge() {
        const pendingCount = todos.filter(t => !t.completed).length;
        todoBadge.textContent = pendingCount;
        todoBadge.style.display = pendingCount > 0 ? 'flex' : 'none';
    }
    
    let isDarkTheme = localStorage.getItem('darkTheme') === 'true';
    if (isDarkTheme) {
        document.body.classList.add('dark-theme');
        themeBtn.innerHTML = `
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                <path d="M21 12.79A9 9 0 1 1 11.21 3 7 7 0 0 0 21 12.79z"/>
            </svg>
        `;
    }
    
    themeBtn.addEventListener('click', () => {
        isDarkTheme = !isDarkTheme;
        document.body.classList.toggle('dark-theme', isDarkTheme);
        localStorage.setItem('darkTheme', isDarkTheme);
        
        if (isDarkTheme) {
            themeBtn.innerHTML = `
                <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                    <path d="M21 12.79A9 9 0 1 1 11.21 3 7 7 0 0 0 21 12.79z"/>
                </svg>
            `;
        } else {
            themeBtn.innerHTML = `
                <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                    <circle cx="12" cy="12" r="5"/>
                    <line x1="12" y1="1" x2="12" y2="3"/>
                    <line x1="12" y1="21" x2="12" y2="23"/>
                    <line x1="4.22" y1="4.22" x2="5.64" y2="5.64"/>
                    <line x1="18.36" y1="18.36" x2="19.78" y2="19.78"/>
                    <line x1="1" y1="12" x2="3" y2="12"/>
                    <line x1="21" y1="12" x2="23" y2="12"/>
                    <line x1="4.22" y1="19.78" x2="5.64" y2="18.36"/>
                    <line x1="18.36" y1="5.64" x2="19.78" y2="4.22"/>
                </svg>
            `;
        }
    });
    
    const todoInput = document.getElementById('todo-input');
    const addBtn = document.getElementById('add-btn');
    const todoList = document.getElementById('todo-list');
    const filterTabs = document.querySelectorAll('.filter-tab');
    const quickActionBtns = document.querySelectorAll('.quick-action-btn');
    const particlesContainer = document.getElementById('particles');
    const rippleContainer = document.getElementById('ripple-container');

    let todos = [];
    let currentFilter = 'all';

    function loadTodos() {
        const stored = localStorage.getItem('todos');
        if (stored) {
            todos = JSON.parse(stored);
        } else {
            todos = [
                { id: 1, text: '完成项目报告', completed: false },
                { id: 2, text: '回复客户邮件', completed: false },
                { id: 3, text: '参加团队会议', completed: true },
                { id: 4, text: '整理工作文档', completed: false },
                { id: 5, text: '提交周报', completed: true }
            ];
        }
        renderTodos();
        updateStats();
    }

    function saveTodos() {
        localStorage.setItem('todos', JSON.stringify(todos));
    }

    function addTodo(text) {
        if (!text.trim()) return;
        
        const newTodo = {
            id: Date.now(),
            text: text.trim(),
            completed: false
        };
        
        todos.push(newTodo);
        saveTodos();
        renderTodos();
        updateStats();
        todoInput.value = '';
        
        createRipple(addBtn);
    }

    function toggleTodo(id) {
        const todo = todos.find(t => t.id === id);
        if (todo) {
            todo.completed = !todo.completed;
            saveTodos();
            renderTodos();
            updateStats();
        }
    }

    function deleteTodo(id) {
        todos = todos.filter(t => t.id !== id);
        saveTodos();
        renderTodos();
        updateStats();
    }

    function filterTodos(filter) {
        currentFilter = filter;
        filterTabs.forEach(tab => {
            tab.classList.toggle('active', tab.dataset.filter === filter);
        });
        renderTodos();
    }

    function renderTodos() {
        const filteredTodos = todos.filter(todo => {
            if (currentFilter === 'all') return true;
            return todo.completed === (currentFilter === 'completed');
        });

        if (filteredTodos.length === 0) {
            todoList.innerHTML = `
                <div class="empty-state">
                    <div class="empty-icon">✓</div>
                    <p>${currentFilter === 'completed' ? '暂无已完成待办' : '暂无待办任务，添加一个开始吧！'}</p>
                </div>
            `;
            return;
        }

        todoList.innerHTML = filteredTodos.map((todo, index) => `
            <div class="todo-item ${todo.completed ? 'completed' : ''}" style="animation-delay: ${index * 0.05}s">
                <div class="checkbox" onclick="toggleTodo(${todo.id})">
                    ${todo.completed ? '<span class="check-icon">✓</span>' : ''}
                </div>
                <span class="todo-text">${todo.text}</span>
                <button class="delete-btn" onclick="deleteTodo(${todo.id})">×</button>
            </div>
        `).join('');
    }

    function updateStats() {
        const total = todos.length;
        const completed = todos.filter(t => t.completed).length;
        const pending = total - completed;

        document.querySelector('.stat-number:not(.pending):not(.completed)').textContent = total;
        document.querySelector('.stat-number.pending').textContent = pending;
        document.querySelector('.stat-number.completed').textContent = completed;
        updateBadge();
    }

    function createRipple(element) {
        const rect = element.getBoundingClientRect();
        const ripple = document.createElement('div');
        ripple.className = 'ripple';
        ripple.style.left = `${rect.left + rect.width / 2 - 10}px`;
        ripple.style.top = `${rect.top + rect.height / 2 - 10}px`;
        rippleContainer.appendChild(ripple);
        
        setTimeout(() => ripple.remove(), 1000);
    }

    function initParticles() {
        for (let i = 0; i < 15; i++) {
            const particle = document.createElement('div');
            particle.className = 'particle';
            particle.style.left = `${Math.random() * 100}%`;
            particle.style.top = '0';
            particle.style.animationDelay = `${Math.random() * 10}s`;
            particle.style.animationDuration = `${8 + Math.random() * 6}s`;
            particlesContainer.appendChild(particle);
        }
    }

    addBtn.addEventListener('click', () => addTodo(todoInput.value));
    todoInput.addEventListener('keypress', (e) => {
        if (e.key === 'Enter') addTodo(todoInput.value);
    });

    filterTabs.forEach(tab => {
        tab.addEventListener('click', () => filterTodos(tab.dataset.filter));
    });

    quickActionBtns.forEach(btn => {
        btn.addEventListener('click', () => {
            addTodo(btn.dataset.text);
            createRipple(btn);
        });
    });

    window.toggleTodo = toggleTodo;
    window.deleteTodo = deleteTodo;

    loadTodos();
    initParticles();
});