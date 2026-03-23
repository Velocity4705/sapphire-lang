// Page transition — fade out on leave, fade in on arrive
(function () {
    // Inject the overlay style once
    const style = document.createElement('style');
    style.textContent = `
        body { opacity: 0; transition: opacity 0.3s ease; }
        body.page-visible { opacity: 1; }
    `;
    document.head.appendChild(style);

    // Fade in on load
    window.addEventListener('DOMContentLoaded', () => {
        requestAnimationFrame(() => {
            document.body.classList.add('page-visible');
        });
    });

    // Fade out before navigating away (internal links only)
    document.addEventListener('click', (e) => {
        const link = e.target.closest('a');
        if (!link) return;

        const href = link.getAttribute('href');
        if (!href) return;

        // Skip: external links, anchors, new tab
        const isExternal = link.hostname && link.hostname !== location.hostname;
        const isAnchor   = href.startsWith('#');
        const isNewTab   = link.target === '_blank';
        if (isExternal || isAnchor || isNewTab) return;

        e.preventDefault();
        document.body.classList.remove('page-visible');

        setTimeout(() => {
            window.location.href = href;
        }, 280);
    });
})();
