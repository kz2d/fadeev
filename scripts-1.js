function makeTabs(node) {
    let selected = node.querySelector('.section__tab_active').dataset.id;
    const tabs = node.querySelectorAll('.section__tab');
    const list = Array.from(tabs).map(node => node.dataset.id);
    const select = node.querySelector('.section__select');
    const tabsContainer = node.querySelector('.section__tabs');

    function selectTab(newId) {
        selected = newId;
        selectTabImplementation(node, newId);
    }

    select.addEventListener('input', () => {
        selectTab(select.value, select);
    });

    tabsContainer.addEventListener('click', (event) => {
        if (event.target.classList.contains('section__tab')) {
            const newId = event.target.dataset.id;
            selectTab(newId, select);
        }
    })

     tabsContainer.addEventListener('keydown', (event) => {
        onKeyDown(event, list, selected, selectTab, select);
     })
}

function makeMenu(node) {
    let expanded = false;
    const links = document.querySelector('.header__links');

    node.addEventListener('click', () => {
        expanded = !expanded;
        node.setAttribute('aria-expanded', expanded ? 'true' : 'false');
        node.querySelector('.header__menu-text').textContent = expanded ? 'Закрыть меню' : 'Открыть меню';
        links.classList.toggle('header__links_opened', expanded);
        links.classList.add('header__links-toggled');
    });
}

document.querySelectorAll('.main__devices').forEach(node => makeTabs(node));
document.querySelectorAll('.header__menu').forEach(node => makeMenu(node));
