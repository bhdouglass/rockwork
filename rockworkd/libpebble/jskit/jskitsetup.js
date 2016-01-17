//Borrowed from https://github.com/pebble/pypkjs/blob/master/pypkjs/javascript/runtime.py#L17
_jskit.make_proxies = function(proxy, origin, names) {
    names.forEach(function(name) {
        proxy[name] = eval("(function " + name + "() { return origin[name].apply(origin, arguments); })");
    });

    return proxy;
}

_jskit.make_properties = function(proxy, origin, names) {
    names.forEach(function(name) {
        Object.defineProperty(proxy, name, {
            configurable: false,
            enumerable: true,
            get: function() {
                return origin[name];
            },
            set: function(value) {
                origin[name] = value;
            }
        });
    });

    return proxy;
}

Pebble = new (function() {
    _jskit.make_proxies(this, _jskit.pebble,
        ['sendAppMessage', 'showSimpleNotificationOnPebble', 'getAccountToken', 'getWatchToken',
        'addEventListener', 'removeEventListener', 'openURL', 'getTimelineToken', 'timelineSubscribe',
        'timelineUnsubscribe', 'timelineSubscriptions', 'getActiveWatchInfo']
    );
})();

function XMLHttpRequest() {
    var xhr = _jskit.pebble.createXMLHttpRequest();
    _jskit.make_proxies(this, xhr,
        ['open', 'setRequestHeader', 'overrideMimeType', 'send', 'getResponseHeader',
        'getAllResponseHeaders', 'abort', 'addEventListener', 'removeEventListener']);
    _jskit.make_properties(this, xhr,
        ['readyState', 'response', 'responseText', 'responseType', 'status',
        'statusText', 'timeout', 'onreadystatechange', 'ontimeout', 'onload',
        'onloadstart', 'onloadend', 'onprogress', 'onerror', 'onabort']);

    this.UNSENT = 0;
    this.OPENED = 1;
    this.HEADERS_RECEIVED = 2;
    this.LOADING = 3;
    this.DONE = 4;
}

function setInterval(func, time) {
    return _jskit.timer.setInterval(func, time);
}

function clearInterval(id) {
    _jskit.timer.clearInterval(id);
}

function setTimeout(func, time) {
    return _jskit.timer.setTimeout(func, time);
}

function clearTimeout(id) {
    _jskit.timer.clearTimeout(id);
}

navigator.geolocation = new (function() {
    _jskit.make_proxies(this, _jskit.geolocation,
        ['getCurrentPosition', 'watchPosition', 'clearWatch']
    );
})();

localStorage = new (function() {
    _jskit.make_proxies(this, _jskit.localstorage,
        ['clear', 'getItem', 'setItem', 'removeItem', 'key']
    );

    _jskit.make_properties(this, _jskit.localstorage,
        ['length']
    );
})();

console = new (function() {
    _jskit.make_proxies(this, _jskit.console,
        ['log', 'warn', 'error', 'info']
    );
})();
