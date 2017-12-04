define([
    'models/commands',
    'models/files'
],function(commands, files){
	var instance = null;

    var destination_model = {
        COMMANDS: 0,
        FILES: 1
    };

    var tag_to_destination_model = {};
    var tag_to_file_id = {}

    function Modem(){
        if(instance !== null){
            throw new Error("Cannot instantiate more than one Modem, use Modem.getInstance()");
        }

        this.init();
    }

    Modem.prototype = {
        init: function(){
            namespace = '';
            socket = io.connect(location.protocol + '//' + document.domain + ':' + location.port + namespace);

            socket.on('connect', function() {
               console.log('connected');
            });

            socket.on('module_info', function(data) {
                console.log('connected to module: ' + data.uid);
                // TODO do not change mainToolbar directly, refactor
                $$('header').setValues({header:"oss7", status:
                    'Connected to ' + data.uid + ' using D7AP v' + data.d7ap_version + ' running app \'' + data.application_name +
                    '\' using git sha1 ' + data.git_sha1}
                );
            });

            socket.on('received_alp_command', function(resp) {
                console.log('received: ' + JSON.stringify(resp));
                var dest_view = tag_to_destination_model[resp['tag_id']];
                if(dest_view == destination_model.COMMANDS || !dest_view) // dest_view can be undefined in case of unsolicited resp
                    commands.add_response(resp['tag_id'], resp['response_command_description'], resp['response_command']['completed_with_error']);
                else if(dest_view == destination_model.FILES)
                    files.update_file(tag_to_file_id[resp['tag_id']], resp['response_command']['actions'][0]['operation']['file_data_parsed']);
            });
        },

        execute_command:function(command, cb) {
            socket.emit('execute_command', command, function(response_data){
                tag_to_destination_model[response_data['tag_id']] = destination_model.COMMANDS;
                commands.add_request(
                    response_data['tag_id'],
                    response_data['interface'],
                    command,
                    response_data['command_description']
                );

                cb(response_data['tag_id']);
            });
        },

        read_file:function(file_id) {
            socket.emit('read_local_system_file', {'system_file_id': file_id}, function(response_data){
                tag_to_destination_model[response_data['tag_id']] = destination_model.FILES;
                tag_to_file_id[response_data['tag_id']] = file_id;
            });
        },

        save_file:function(file_data) {
            socket.emit('write_local_system_file', file_data.data,
                function(response_data){
                    console.log("file saved: " + response_data);
                }
            );
        },

        info:function () {
            return info_string;
        }
    }

    Modem.getInstance = function(){
        if(instance === null){
            instance = new Modem();
        }
        return instance;
    };

	return Modem.getInstance();
});

