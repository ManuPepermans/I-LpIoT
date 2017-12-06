define([],function(){
    var ui = {
        type: "clean",
        rows: [
            {
                view: "form", id: "file_contents_form", complexData: true, elements: [
                    {
                        view:"numberEdit",
                        label:"Active Access Class",
                        name:"data.active_access_class",
                        labelWidth:300 // TODO should not be necessary
                    },
                    {
                        view:"numberEdit",
                        label:"VID",
                        name:"data.vid",
                        labelWidth:300 // TODO should not be necessary
                    }
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