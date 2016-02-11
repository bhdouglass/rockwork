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

performance = new (function() {
    _jskit.make_proxies(this, _jskit.performance, ['now']);
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

console = new (function() {
    _jskit.make_proxies(this, _jskit.console,
        ['log', 'warn', 'error', 'info']
    );
})();

/*localStorage = new (function() {
    _jskit.make_proxies(this, _jskit.localstorage,
        ['clear', 'getItem', 'setItem', 'removeItem', 'key']
    );

    _jskit.make_properties(this, _jskit.localstorage,
        ['length']
    );
})();*/

//It appears that Proxy is not available since Qt is using Javascript v5
/*(function() {
    var proxy = _jskit.make_proxies({}, _jskit.localstorage, ['set', 'has', 'deleteProperty', 'keys', 'enumerate']);
    var methods = _jskit.make_proxies({}, _jskit.localstorage, ['clear', 'getItem', 'setItem', 'removeItem', 'key']);
    proxy.get = function get(p, name) { return methods[name] || _jskit.localstorage.get(p, name); }
    this.localStorage = Proxy.create(proxy);
})();*/

//inspired by https://developer.mozilla.org/en-US/docs/Web/API/Storage/LocalStorage
Object.defineProperty(window, "localStorage", new (function () {
    var storage = {};
    Object.defineProperty(storage, "getItem", {
        value: function (key) {
            var value = null;
            if (key !== undefined && key !== null && storage[key] !== undefined) {
                value = storage[key];
            }

            return value;
        },
        writable: false,
        configurable: false,
        enumerable: false
    });

    Object.defineProperty(storage, "key", {
        value: function (index) {
            return Object.keys(storage)[index];
        },
        writable: false,
        configurable: false,
        enumerable: false
    });

    Object.defineProperty(storage, "setItem", {
        value: function (key, value) {
            if (key !== undefined && key !== null) {
                _jskit.localstorage.setItem(key, value);
                storage[key] = (value && value.toString) ? value.toString() : value;
                return true;
            }
            else {
                return false;
            }
        },
        writable: false,
        configurable: false,
        enumerable: false
    });

    Object.defineProperty(storage, "length", {
        get: function () {
            return Object.keys(storage).length;
        },
        configurable: false,
        enumerable: false
    });

    Object.defineProperty(storage, "removeItem", {
        value: function (key) {
            if (key && storage[key]) {
                _jskit.localstorage.removeItem(key);
                delete storage[key];

                return true;
            }
            else {
                return false;
            }
        },
        writable: false,
        configurable: false,
        enumerable: false
    });

    Object.defineProperty(storage, "clear", {
        value: function (key) {
            for (var key in storage) {
                storage.removeItem(key);
            }

            return true;
        },
        writable: false,
        configurable: false,
        enumerable: false
    });

    this.get = function () {
        return storage;
    };

    this.configurable = false;
    this.enumerable = true;
})());

(function() {
    var keys = _jskit.localstorage.keys();
    for (var index in keys) {
        var value = _jskit.localstorage.getItem(keys[index]);
        localStorage.setItem(keys[index], value);
    }
})();
