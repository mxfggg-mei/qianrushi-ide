document.addEventListener('DOMContentLoaded', function() {
    const navItems = document.querySelectorAll('.nav-item');
    const pageContents = document.querySelectorAll('.page-content');
    const prevBtn = document.getElementById('prev-btn');
    const nextBtn = document.getElementById('next-btn');
    const pageNumber = document.querySelector('.page-number');
    
    const pages = [
        { id: 'overview', name: '系统概述' },
        { id: 'architecture', name: '系统架构' },
        { id: 'hardware', name: '硬件模块' },
        { id: 'software', name: '软件模块' },
        { id: 'operation', name: '操作指南' },
        { id: 'mqtt', name: 'MQTT协议' },
        { id: 'commands', name: '串口命令' },
        { id: 'troubleshooting', name: '故障排查' }
    ];
    
    let currentPageIndex = 0;
    
    function showPage(index) {
        if (index < 0 || index >= pages.length) return;
        
        pageContents.forEach(page => {
            page.classList.remove('active');
        });
        
        navItems.forEach(item => {
            item.classList.remove('active');
        });
        
        const targetPage = document.getElementById(`page-${pages[index].id}`);
        const targetNav = document.querySelector(`.nav-item[data-page="${pages[index].id}"]`);
        
        if (targetPage) {
            targetPage.classList.add('active');
        }
        if (targetNav) {
            targetNav.classList.add('active');
        }
        
        currentPageIndex = index;
        pageNumber.textContent = index + 1;
        
        window.scrollTo({ top: 0, behavior: 'smooth' });
    }
    
    navItems.forEach((item, index) => {
        item.addEventListener('click', () => {
            showPage(index);
        });
    });
    
    prevBtn.addEventListener('click', () => {
        if (currentPageIndex > 0) {
            showPage(currentPageIndex - 1);
        }
    });
    
    nextBtn.addEventListener('click', () => {
        if (currentPageIndex < pages.length - 1) {
            showPage(currentPageIndex + 1);
        }
    });
    
    document.addEventListener('keydown', (e) => {
        if (e.key === 'ArrowLeft') {
            prevBtn.click();
        } else if (e.key === 'ArrowRight') {
            nextBtn.click();
        }
    });
    
    const sidebar = document.querySelector('.sidebar');
    const mainContent = document.querySelector('.main-content');
    
    if (window.innerWidth <= 768) {
        mainContent.addEventListener('click', () => {
            sidebar.classList.remove('open');
        });
    }
    
    window.addEventListener('resize', () => {
        if (window.innerWidth > 768) {
            sidebar.classList.remove('open');
        }
    });
});