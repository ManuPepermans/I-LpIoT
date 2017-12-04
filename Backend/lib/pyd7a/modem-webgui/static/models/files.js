define([],function(){

	var files = new webix.DataCollection({
		url: '/systemfiles'
	});

	return {
		data: files,

		update_file:function(file_id, data){
			files.data.each(function(file){
				if(file.file_id == file_id){
					file.data = data;
                    console.log(file);
                    files.updateItem(file.id, file);
				}
			});
		}
	};
});