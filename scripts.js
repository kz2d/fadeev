function selectTabImplementation(node, tabsNode, newId) {
    const newTab = tabsNode.querySelector(`.section__tab[data-id=${newId}]`);
    const oldTab = tabsNode.querySelector('.section__tab_active');
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

function onKeyDown(event, list, selected, selectTab, select) {
    if (!event.target.classList.contains('section__tab') || event.ctrlKey || event.metaKey || event.shiftKey || event.altKey) {
        return;
    }

    let index = list.indexOf(selected);
    if (event.which === 37) {
        // left
        --index;
    } else if (event.which === 39) {
        // right
        ++index;
    } else if (event.which === 36) {
        // home
        index = 0;
    } else if (event.which === 35) {
        // end
        index = list.length - 1;
    } else {
        return;
    }

    if (index >= list.length) {
        index = 0;
    } else if (index < 0) {
        index = list.length - 1;
    }

    selectTab(list[index], select);
}

function makeTabs(node) {
    const tabsContainer = node.querySelector('.section__tabs');
    const tabs = tabsContainer.querySelectorAll('.section__tab');
    const list = Array.from(tabs).map(node => node.dataset.id);
    const select = node.querySelector('.section__select');
    let selected = tabs.querySelector('.section__tab_active').dataset.id;

    function selectTab(newId) {
        selected = newId;
        selectTabImplementation(node, tabsContainer, newId);
    }

    select.addEventListener('input', () => {
        selectTab(select.value, select);
    });

    tabsContainer.addEventListener('click', (event) => {
        if (event.target.classList.contains('section__tab')) {
            selectTab(event.target.dataset.id, select);
        }
    })

    tabsContainer.addEventListener('keydown', (event) => {
        onKeyDown(event, list, selected, selectTab, select);
    })
}

function makeMenu(node) {
    let expanded = false;
    const menuTextNode = node.querySelector('.header__menu-text');

    node.addEventListener('click', () => {
        expanded = !expanded;
        node.setAttribute('aria-expanded', expanded ? 'true' : 'false');
        menuTextNode.textContent = expanded ? 'Закрыть меню' : 'Открыть меню';
        links.classList.toggle('header__links_opened', expanded);
        links.classList.add('header__links-toggled');
    });
}
const links = document.querySelector('.header__links');

document.querySelectorAll('.main__devices').forEach(node => makeTabs(node));
document.querySelectorAll('.header__menu').forEach(node => makeMenu(node));