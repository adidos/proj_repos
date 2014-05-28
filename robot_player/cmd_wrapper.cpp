#include "cmd_wrapper.h"
#include "DataXCmd.h"
#include "XLDataX.h"

#include <cstdlib>

shared_ptr<DataXCmd> CmdWrapper::pack_get_dir_cmd(int game_id)
{
	IDataX* param = new XLDataX();
    param->PutInt(DataID_GameId, game_id);

	shared_ptr<DataXCmd> cmd_ptr (new DataXCmd("GetDirReq", 1));
    cmd_ptr->set_userid(-1);
    cmd_ptr->set_datax(param);
   
	return cmd_ptr;
}

shared_ptr<DataXCmd> CmdWrapper::pack_enter_room_cmd(int game_id, int room_id)
{
	IDataX* data = new XLDataX();
	data->PutInt(DataID_GameId, game_id);
	data->PutInt(DataID_RoomId, room_id);

	shared_ptr<DataXCmd> cmd_ptr(new DataXCmd("EnterRoomReq", 1));
	cmd_ptr->set_userid (10086);
	cmd_ptr->set_datax(data);

	return cmd_ptr;
}

shared_ptr<DataXCmd> CmdWrapper::pack_game_action_cmd()
{
	shared_ptr<DataXCmd> cmd_ptr(new DataXCmd("GameActionNotify", 1));

	return cmd_ptr;
}