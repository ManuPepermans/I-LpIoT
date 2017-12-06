define([
    "libs/webix-mvc-core/core",
    "libs/webix-mvc-core/plugins/menu",
], function (core, menu) {
    var app = core.create({
        id: "oss7-modem-webgui",
        name: "OSS-7 Modem webgui",
        version: "0.0.1",
        debug: true,
        start: "/top/commands"
    });

    webix.debug_bind = true;

    app.use(menu);

    // a custom control to use instead of text when requiring numerical input.
    // getValue() returns an int instead of a string as the text control does.
    webix.protoUI({
        name: 'numberEdit',
        $cssName: 'text',
        defaults: {
            type: 'text',
            allowNull: false,
            nullValue: 0,
            attributes: {
                type: "number"
            }
        },
        $init: function (config) {
            this.attachEvent("onChange", function (n, o) {
                if ((typeof n !== 'number') && n !== o) {
                    var nv;
                    if (n === "" || n === null) {
                        if (config.allowNull) {
                            nv = null;
                        } else {
                            nv = config.nullValue || 0;
                        }
                    } else {
                        nv = parseInt(n);

                        if (isNaN(nv)) {
                            nv = o;
                        }
                    }
                    this.blockEvent();
                    this.setValue(nv);
                    this.unblockEvent();
                }
            });
        },
        type_setter: function () {
            return "text";
        },
        _applyChanges: function () {
            var newvalue = this.getInputNode().value;

            if (newvalue !== this.getValue())
                this.setValue(newvalue);
        },
        setValue: function (value) {
            var config = (this._settings || this.config);
            var oldvalue = config.value;
            if (oldvalue === value) return false;
            config.value = value;
            if (this._rendered_input) this.$setValue(value);
            this.callEvent("onChange", [value, oldvalue]);
        },
        getValue: function () {
            return (this._settings || this.config).value;
        },
    }, webix.ui.text);

    return app;
});