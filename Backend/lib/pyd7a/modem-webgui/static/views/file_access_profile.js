define([],function(){
    var ui = {
        type: "clean",
        rows: [
            {
                view: "form", id: "file_contents_form", complexData: true, elements: [
                    {template: "Channel Header", type: "section"},
                    {
                        view: "select",
                        label: "Band",
                        value: 0,
                        name: "data.access_profile.channel_header.channel_band.value",
                        options: "/channel_bands"
                    },
                    {
                        view: "select",
                        label: "Coding",
                        value: 0,
                        name: "data.access_profile.channel_header.channel_coding.value",
                        options: "/channel_codings"
                    },
                    {
                        view: "select",
                        label: "Class",
                        value: 0,
                        name: "data.access_profile.channel_header.channel_class.value",
                        options: "/channel_classes"
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