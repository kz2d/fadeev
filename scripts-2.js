function selectTabImplementation(node, newId) {
    const newTab = node.querySelector(`.section__tab[data-id=${newId}]`);
    const newPanel = node.querySelector(`.section__panel[data-id=${newId}]`);
    const oldTab = node.querySelector('.section__tab_active');
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
    if (!event.target.classList.contains('section__tab')) {
        return;
    }

    if (event.ctrlKey || event.metaKey || event.shiftKey || event.altKey) {
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