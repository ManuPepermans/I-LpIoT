define([],function(){
    var ui = {
        type: "clean",
        rows: [
            {
                view: "form", id: "file_contents_form", complexData: true, elements: [
                    { view:"text", label:"UID", name:"data.uid", disabled:true}
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