#ifndef CMD_WRAPPER_H_
#define CMD_WRAPPER_H_

#include <memory>

class DataXCmd;

using namespace std;

class CmdWrapper
{
public:
	static shared_ptr<DataXCmd> pack_get_dir_cmd(int game_id);
	static shared_ptr<DataXCmd> pack_enter_room_cmd(int game_id, int room_id);
	static shared_ptr<DataXCmd> pack_game_action_cmd();
};

class CmdDecoder
{
public:



};

#endif /*CMD_WRAPPER_H_*/
