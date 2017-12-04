define([
	"app",
	"models/modem"
],function(app, modem){

	var mainToolbar = {
		view: "toolbar",
		css:"header",
		height:40,
		elements:[
			{
				view: "template", template: "#header# - #status#", id: "header", data: {
				 	header: "oss7 modem interface", status: "not connected"
				} // TODO bind to modem's info property
			}
		]
}	;

	var menu = {
		view:"menu", id:"top:menu", 
		width:180, layout:"y", select:true,
		template:"<span class='webix_icon fa-#icon#'></span> #value# ",
		data:[
			{ value:"ALP Commands", id:"commands", href:"#!/top/commands", icon:"signal" },
			{ value: "Files", id: "files", href:"#!/top/files", icon: "folder-open-o" }
		]
	};

	var ui = {
		type:"material",
		rows:[
			mainToolbar,
			{
				cols:[
					{ type:"clean", css:"menu", padding:10, margin:20, borderless:true, rows: [ menu ]},
					{ rows:[ { height:10}, { type:"clean", css:"", padding:4, rows:[{ $subview:true }]}	]}
				]
			}
		]
	};

	return {
		$ui: ui,
		$menu: "top:menu"
	};
});
