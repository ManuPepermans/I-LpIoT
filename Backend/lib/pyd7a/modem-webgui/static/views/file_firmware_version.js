define([],function(){
    var ui = {
        type: "clean",
        rows: [
            {
                view: "form", id: "file_contents_form", complexData: true, elements: [
                    {
                        view:"text",
                        label:"D7A Protocol version major",
                        name:"data.d7a_protocol_version_major",
                        disabled:true,
                        labelWidth:300 // TODO should not be necessary
                    },
                    {
                        view:"text",
                        label:"D7A Protocol version minor",
                        name:"data.d7a_protocol_version_minor",
                        disabled:true,
                        labelWidth:300 // TODO should not be necessary
                    },
                    {
                        view:"text",
                        label:"Application name",
                        name:"data.application_name",
                        disabled:true,
                        labelWidth:300 // TODO should not be necessary
                    },
                    {
                        view:"text",
                        label:"git revision sha1",
                        name:"data.git_sha1",
                        disabled:true,
                        labelWidth:300 // TODO should not be necessary
                    },
                ]
            },
            {} // spacer
        ]
    };

	return {
		$ui: ui,
		$menu: "top:menu",
	};
});