document.querySelectorAll('.main__devices').forEach(node => {
    const tabsContainer = node.querySelector('.section__tabs');
    const select = node.querySelector('.section__select');
    const tabs = tabsContainer.querySelectorAll('.section__tab');

    const list = [];
    let selected = '';
    tabs.forEach((node) => {
        list.push(node.dataset.id);
        if (node.classList.contains('.section__tab_active')) {
            selected = node.dataset.id;
        }
    });

    function selectTab(newId) {
        selected = newId;
        const newTab = tabsContainer.querySelector(`.section__tab[data-id=${newId}]`);
        const oldTab = tabsContainer.querySelector('.section__tab_active');
        const newPanel = node.querySelector(`.section__panel[data-id=${newId}]`);
        const oldPanel = node.querySelector('.section__panel:not(.section__panel_hidden)');

        oldTab.classList.remove('section__tab_active');
        oldTab.setAttribute('aria-selected', 'false');
        oldTab.removeAttribute('tabindex');
        newTab.classList.add('section__tab_active');
        newTab.setAttribute('aria-selected', 'true');
        newTab.setAttribute('tabindex', '0');
        newTab.focus({
            preventScroll: true
        });

        oldPanel.classList.add('section__panel_hidden');
        oldPanel.setAttribute('aria-hidden', 'true');
        newPanel.classList.remove('section__panel_hidden');
        newPanel.setAttribute('aria-hidden', 'false');
    }

    select.addEventListener('input', () => {
        selectTab(select.value);
    });

    tabsContainer.addEventListener('click', (event) => {
        if (event.target.classList.contains('section__tab')) {
            selectTab(event.target.dataset.id);
        }
    })

    tabsContainer.addEventListener('keydown', (event) => {
        if (!event.target.classList.contains('section__tab') || event.ctrlKey || event.metaKey || event.shiftKey || event.altKey) {
            return;
        }

        let index = list.indexOf(selected);
        if (event.keyCode === 37) {
            if (--index < 0){
                index = list.length - 1;
            }
        } else if (event.keyCode === 39) {
            if (++index >= list.length) {
                index = 0;
            }
        } else if (event.keyCode === 36) {
            index = 0;
        } else if (event.keyCode === 35) {
            index = list.length - 1;
        } else {
            return;
        }

        selectTab(list[index]);
    })
});
document.querySelectorAll('.header__menu').forEach(node => {
    let expanded = false;
    const menuTextNode = node.querySelector('.header__menu-text');

    node.addEventListener('click', () => {
        expanded = !expanded;
        node.setAttribute('aria-expanded', expanded ? 'true' : 'false');
        menuTextNode.textContent = expanded ? 'Закрыть меню' : 'Открыть меню';
        links.classList.toggle('header__links_opened', expanded);
        links.classList.add('header__links-toggled');
    });
});

const links = document.querySelector('.header__links');