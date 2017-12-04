define([
    "app",
    "models/modem",
    "models/files",
],function(app, modem, files){
    function showFileDetail(file){
		console.log("show detail: " + file.file_id);
        
        // dynamically load form based on filename
        var filename = file.file_name;
        if(filename.startsWith("ACCESS_PROFILE"))
            filename = "access_profile"; // remove access specifier

        app.show("/top/files/file_" + filename.toLowerCase());
        $$('file_metadata_form').setValues(file);
        console.log('file data: ' + file.data);
        $$('file_contents_form').setValues(file); // TODO forms for all files now have the same id, find a better way
    }

    var ui = {
        rows:[
            { view: "toolbar", css: "highlighted_header header1", height: 40, cols: [
                {template: "Files"}
            ]},
            {
                cols: [
                    {
                        view:"datatable",
                        id:"file_list",
                        select: true,
                        columns:[
                            {id:"file_id", header:"ID", sort:"int"},
                            {id:"file_name", header:"Filename", fillspace:true}
                        ],
                        data:files.data,
                        on:{
                            'onItemClick':function(id){
                                modem.read_file(this.getItem(id).file_id);
                                showFileDetail(this.getItem(id));
                            }
                        }
                    },
                    {
                        rows: [
                            {
                                view: "toolbar" ,css: "highlighted_header header1", height: 40, cols: [
                                    {id: "file_details_title", template: "File details"},
                                    {
                                        view: "button",
                                        value: "Save",
                                        width: 90,
                                        click: function () {
                                            var form = $$("file_contents_form");
                                            selected_tag_id = null;
                                            if (form.validate()) {
                                                // TODO post?
                                                file_data = form.getValues();
                                                console.log('file_data: ' + file_data);
                                                modem.save_file(file_data);
                                            }
                                        }
                                    },
                                ]
                            },
                            {template: "File metadata", type: "section"},
                            { view: "form", id: "file_metadata_form", complexData: true, elements: [
                                {
                                    view:"text",
                                    label:"File name",
                                    name:"file_name",
                                    disabled:true,
                                    labelWidth:300 // TODO should not be necessary
                                },
                                {
                                    view:"text",
                                    label:"File ID",
                                    name:"file_id",
                                    disabled:true,
                                    labelWidth:300 // TODO should not be necessary
                                },
                                {
                                    view:"text",
                                    label:"File size",
                                    name:"data.length",
                                    disabled:true,
                                    labelWidth:300 // TODO should not be necessary
                                },
                            ]},
                            {template: "File contents", type: "section"},
                            { $subview: true }
                        ]
                    }

                ]
            }

        ]
    };

    function onInit() {
        // make sure changes to the file are updated in the file details pane, update the UI after details have been received.
        // TODO check if we can get this through databinding, like it is used for the datatable.
        files.data.attachEvent("onDataUpdate", function(id, obj){
            console.log("onDataUpdate");
            showFileDetail(files.data.getItem(id));
            return true;
        });

    }

	return {
		$ui: ui,
		$menu: "top:menu",
        $oninit:onInit()
	};
});