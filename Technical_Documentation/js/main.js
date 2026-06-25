document.addEventListener('DOMContentLoaded', function() {
    initCircuitBoardAnimation();
    initStatCounter();
    initScrollAnimations();
    initCodeHighlight();
    initImageHoverEffects();
    initTabNavigation();
});

function initCircuitBoardAnimation() {
    const boardElements = document.querySelectorAll('.board-element');
    boardElements.forEach((element, index) => {
        element.style.animationDelay = `${index * 0.3}s`;
    });
}

function initStatCounter() {
    const statValues = document.querySelectorAll('.stat-value');
    statValues.forEach((stat) => {
        const target = stat.textContent;
        let current = 0;
        const duration = 1500;
        const step = duration / 30;
        
        const timer = setInterval(() => {
            if (current >= parseInt(target) || target === 'MQTT' || target === 'OLED') {
                stat.textContent = target;
                clearInterval(timer);
            } else {
                current++;
                stat.textContent = current;
            }
        }, step);
    });
}

function initScrollAnimations() {
    const observerOptions = {
        threshold: 0.1,
        rootMargin: '0px 0px -50px 0px'
    };
    
    const observer = new IntersectionObserver((entries) => {
        entries.forEach(entry => {
            if (entry.isIntersecting) {
                entry.target.style.opacity = '1';
                entry.target.style.transform = 'translateY(0)';
            }
        });
    }, observerOptions);
    
    document.querySelectorAll('.feature-card, .hardware-card, .module-card, .mode-card, .key-card').forEach(card => {
        card.style.opacity = '0';
        card.style.transform = 'translateY(30px)';
        card.style.transition = 'opacity 0.6s ease, transform 0.6s ease';
        observer.observe(card);
    });
}

function initCodeHighlight() {
    const codeBlocks = document.querySelectorAll('pre code, .message-code');
    codeBlocks.forEach(block => {
        const content = block.textContent;
        const highlighted = highlightJSON(content);
        block.innerHTML = highlighted;
    });
}

function highlightJSON(jsonStr) {
    try {
        JSON.parse(jsonStr);
        return jsonStr
            .replace(/"([^"]+)":/g, '<span style="color: #a78bfa">"$1"</span>:')
            .replace(/: "([^"]+)"/g, ': <span style="color: #34d399">"$1"</span>')
            .replace(/: (\d+\.?\d*)/g, ': <span style="color: #f87171">$1</span>')
            .replace(/: (true|false)/g, ': <span style="color: #60a5fa">$1</span>');
    } catch {
        return jsonStr;
    }
}

function initImageHoverEffects() {
    const cards = document.querySelectorAll('.feature-card, .hardware-card, .module-card');
    cards.forEach(card => {
        card.addEventListener('mouseenter', () => {
            card.style.transform = 'translateY(-8px) scale(1.01)';
        });
        card.addEventListener('mouseleave', () => {
            card.style.transform = 'translateY(0) scale(1)';
        });
    });
}

function initTabNavigation() {
    const navItems = document.querySelectorAll('.nav-item');
    navItems.forEach(item => {
        item.addEventListener('mouseenter', () => {
            const icon = item.querySelector('.nav-icon');
            if (icon) {
                icon.style.transform = 'scale(1.2) rotate(5deg)';
            }
        });
        item.addEventListener('mouseleave', () => {
            const icon = item.querySelector('.nav-icon');
            if (icon) {
                icon.style.transform = 'scale(1) rotate(0deg)';
            }
        });
    });
}

function createRippleEffect(e) {
    const button = e.currentTarget;
    const ripple = document.createElement('span');
    const rect = button.getBoundingClientRect();
    const size = Math.max(rect.width, rect.height);
    const x = e.clientX - rect.left - size / 2;
    const y = e.clientY - rect.top - size / 2;
    
    ripple.style.width = ripple.style.height = `${size}px`;
    ripple.style.left = `${x}px`;
    ripple.style.top = `${y}px`;
    ripple.classList.add('ripple');
    
    button.appendChild(ripple);
    
    setTimeout(() => {
        ripple.remove();
    }, 600);
}

document.querySelectorAll('.page-btn').forEach(btn => {
    btn.addEventListener('click', createRippleEffect);
});

document.querySelectorAll('.nav-item').forEach(item => {
    item.addEventListener('click', createRippleEffect);
});

let mouseX = 0;
let mouseY = 0;

document.addEventListener('mousemove', (e) => {
    mouseX = (e.clientX / window.innerWidth - 0.5) * 20;
    mouseY = (e.clientY / window.innerHeight - 0.5) * 20;
    
    const heroVisual = document.querySelector('.hero-visual');
    if (heroVisual) {
        heroVisual.style.transform = `translate(${mouseX * 0.5}px, ${mouseY * 0.5}px)`;
    }
});

function animateBackgroundGradient() {
    const body = document.body;
    let hue = 240;
    
    setInterval(() => {
        hue = (hue + 0.5) % 360;
        body.style.background = `radial-gradient(ellipse at center, rgba(${hue}, 102, 241, 0.03) 0%, #020617 70%)`;
    }, 50);
}

animateBackgroundGradient();

function initSearchFunctionality() {
    const searchInput = document.createElement('input');
    searchInput.type = 'text';
    searchInput.placeholder = '搜索文档...';
    searchInput.style.cssText = `
        position: fixed;
        top: 20px;
        right: 20px;
        width: 200px;
        padding: 10px 15px;
        background: rgba(30, 41, 59, 0.9);
        border: 1px solid #334155;
        border-radius: 10px;
        color: #f1f5f9;
        font-size: 14px;
        outline: none;
        z-index: 1000;
        transition: all 0.3s ease;
    `;
    
    searchInput.addEventListener('input', (e) => {
        const query = e.target.value.toLowerCase();
        const sections = document.querySelectorAll('section');
        
        sections.forEach(section => {
            const text = section.textContent.toLowerCase();
            if (text.includes(query)) {
                section.style.opacity = '1';
                section.style.display = 'block';
            } else {
                section.style.opacity = '0.3';
            }
        });
    });
    
    document.body.appendChild(searchInput);
}

initSearchFunctionality();

function initKeyboardShortcuts() {
    document.addEventListener('keydown', (e) => {
        if (e.ctrlKey || e.metaKey) {
            switch (e.key) {
                case 'g':
                    e.preventDefault();
                    document.querySelector('.nav-item[data-page="overview"]').click();
                    break;
                case 'a':
                    e.preventDefault();
                    document.querySelector('.nav-item[data-page="architecture"]').click();
                    break;
                case 'h':
                    e.preventDefault();
                    document.querySelector('.nav-item[data-page="hardware"]').click();
                    break;
                case 's':
                    e.preventDefault();
                    document.querySelector('.nav-item[data-page="software"]').click();
                    break;
                case 'o':
                    e.preventDefault();
                    document.querySelector('.nav-item[data-page="operation"]').click();
                    break;
                case 'm':
                    e.preventDefault();
                    document.querySelector('.nav-item[data-page="mqtt"]').click();
                    break;
                case 'c':
                    e.preventDefault();
                    document.querySelector('.nav-item[data-page="commands"]').click();
                    break;
                case 't':
                    e.preventDefault();
                    document.querySelector('.nav-item[data-page="troubleshooting"]').click();
                    break;
            }
        }
    });
}

initKeyboardShortcuts();