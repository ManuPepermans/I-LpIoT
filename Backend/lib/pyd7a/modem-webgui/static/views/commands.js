define([
    "app",
    "models/commands",
    "models/modem",
],function(app, commands, modem){
    var selected_tag_id = null;

    var command_request_view = {
        type: "clean",
        rows: [
            {view: "toolbar", css: "highlighted_header header2", height: 40, cols: [
                {template: "Request"}
            ]},
            {
                view: "form", id: "execute_command_form", elements: [
                    {template: "Read file", type: "section"}, // TODO other operations
                    {view: "text", label: 'file ID', validate: webix.rules.isNumber, name: "file_id", value: "0"},
                    {view: "text", label: 'offset', validate: webix.rules.isNumber, name: "offset", value: "0"},
                    {view: "text", label: 'length', validate: webix.rules.isNumber, name: "length", value: "8"},
                    {
                        view: "select", label: "Interface", value: 0, name: "interface", options: [
                        {id: 0, value: "HOST"},
                        {id: 0xD7, value: "D7ASP"}
                    ],
                        on: {
                            onChange: function (value) {
                                if (value == 0xD7) {
                                    $$("d7asp_interface").show();
                                } else {
                                    $$("d7asp_interface").hide();
                                }
                            }
                        }
                    },
                    {
                        view: "fieldset",
                        id: "d7asp_interface",
                        label: "D7ASP Interface Configuration",
                        hidden: true,
                        body: {
                            rows: [
                                {template: "QoS", type: "section"},
                                {
                                    view: "select",
                                    label: "Response mode",
                                    value: "RESP_MODE_ALL",
                                    name: "qos_response_mode",
                                    options: "/responsemodes"
                                },
                                {template: "Addressee", type: "section"},
                                {
                                    view: "text",
                                    label: "Access Class",
                                    value: "1",
                                    validate: webix.rules.isNumber,
                                    name: "access_class"
                                },
                                {view: "select", label: "IdType", value: "NOID", name: "id_type", options: "/idtypes"},
                                {view: "text", label: "ID", validate: webix.rules.isNumber, name: "id", value: "0"},

                            ]
                        }
                    },
                    // interface config
                    // operation
                    // file

                ]
            }
        ]
    };

    var command_response_view = {
        rows: [
            {view: "toolbar", css: "highlighted_header header2", height: 40, cols: [{template: "Response(s)"}]},
            {id:"cmd_response", template:"#cmd_string#"}
        ]
    };

    var query_window = {
        view:"window",
        id:"query_window",
        position:"center",
        width: 1000,
        head: false,
        body:{ rows:[
            {
                view:"toolbar", css: "highlighted_header header1", height: 40, cols:[
                    { id: "title", template: "Command #tag_id#", data: {tag_id: ""}},
                    {
                        view: "button",
                        value: "Execute",
                        width: 90,
                        click: function () {
                            var form = $$("execute_command_form");
                            selected_tag_id = null;
                            if (form.validate()) {
                                // TODO post?
                                console.log(form.getValues());
                                modem.execute_command(form.getValues(), function (new_tag_id) {
                                    console.log("executed command, generated tag_id: " + new_tag_id);
                                    if(selected_tag_id == null) {
                                        selected_tag_id = new_tag_id;
                                        showCommandDetail(commands.data.getItem(new_tag_id));
                                    }
                                })
                            }
                        }
                    },
                    { view:"button", value:"Close", width:90, click:function(){
                        this.getTopParentView().hide();
                    }}
                ]
            },
            {
                type: "space",
                cols: [
                    command_request_view,
                    command_response_view
                ]
            }
        ]},
        on:{
            'onHide':function(){
                selected_tag_id = null;
            }
        }
    };

    var ui = {
        rows:[
            {view: "toolbar", css: "highlighted_header header1", height: 40, cols: [
                {template: "ALP Command Log"},
                {
                    view: "button",
                    value: "New Query",
                    width: 120,
                    click: function () {
                        showNewQueryWindow();
                    }
                }
            ]},
            {
                view:"datatable",
                id:"received_alp_commands_list",
                columns:[
                    {id:"id", header:"Tag", sort:"int"},
                    {id:"interface", header:"Interface"},
                    {id:"status", header:"Status", fillspace:true},
                    {id:"command_description", header:"Request", fillspace:true},
                    {id:"response_command_descriptions", header:"Response", fillspace:true},
                ],
                data:commands.data,
                on:{
                    'onItemClick':function(id){
                        showCommandDetail(this.getItem(id));
                    }
                }
            }
        ]
    };

    function showNewQueryWindow() {
        $$("title").parse({'tag_id': ''});
		$$("cmd_response").parse({'cmd_string': ''});
        $$("query_window").show();
    }

    function showCommandDetail(command){
		console.log("show detail: " + command.tag_id);
		$$("title").parse({'tag_id': command.tag_id});
		$$("cmd_response").parse({'cmd_string': command.response_command_descriptions.join("<br>")});
        $$("query_window").show();
    }

    function onInit() {
        // make sure changes to the command are updated in the querywindow, for example
        // the response is received async after execution. Update the UI after response has been received.
        // TODO check if we can get this through databinding, like it used for the datatable.
        commands.data.attachEvent("onDataUpdate", function(id, obj){
            console.log("onDataUpdated");
            // we only update the view when we are currently displaying this command
            if($$("query_window").isVisible && id == selected_tag_id) {
                showCommandDetail(commands.data.getItem(id));
            }
            return true;
        });

    }

	return {
		$ui: ui,
		$menu: "top:menu",
        $windows: [query_window],
        $oninit: onInit()
	};
});