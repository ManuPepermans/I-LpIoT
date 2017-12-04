define([],function(){

	var commands = new webix.DataCollection({ data:[
	]});

	return {
		data: commands,
		$oninit:function(){
		},

		add_request:function(tag_id, interface, command, command_description){
			// TODO assert tag_id does not exist yet
			commands.add({
				'id': tag_id,  // use tag_id for id field so we can use tag_id directly in for example exists() and getItem(),
				'tag_id': tag_id, // however use tag_id field for displaying, since a id cannot be null and this would result in bogus id in GUI
				'interface': interface,
				'status':'Running',
				'command': command,
				'command_description': command_description,
				'response_command_descriptions': []});
		},

		add_response:function(tag_id, response_command_description, completed_with_error){
			if(commands.exists(tag_id)){
				commands.getItem(tag_id).response_command_descriptions.push(response_command_description);
				var status = "Completed";
				if(completed_with_error) {
					status += " (NOK)";
				} else {
					status += " (OK)";
				}

				commands.getItem(tag_id).status = status;
				commands.updateItem(tag_id);
			} else {
				commands.add({'id': tag_id, 'tag_id': tag_id, 'command': null, 'response_command_descriptions': [response_command_description]});
			}
		}
	};
});