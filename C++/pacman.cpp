// Team name : I don't know
// Team ID : 17
// Members : 王軒 0816095、林佑庭 0816136、寸少康 0816144

#include "STcpClient.h"
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <thread>
#include <windows.h>
#include <queue>
#include <stack>
#include <stdlib.h>

using namespace std;

std::vector<int> step;
int prev_step = 0;
pair<int, int> prev_pos = make_pair(-100, -100);
pair<int, int> dir;
int pstat = 0;
bool prev_mine = 0;
int pruning = 0;
int reverse[4] = {1, 0, 3, 2};

class Mythread
{
public:
	bool killed = false;
	/*
	輪到此程式移動

	change Step
	Step : vector, Step = {dir, is_throwLandmine}
			dir= 0: left, 1:right, 2: up, 3: down 4:do nothing
			is_throwLandmine= True/False
	*/

	pair<int, int> get_pos(pair<int, int> pos, int action){
		if(action == 0){
			return make_pair(pos.first - 25, pos.second);
		}
		else if(action == 1){
			return make_pair(pos.first + 25, pos.second);
		}
		else if(action == 2){
			return make_pair(pos.first, pos.second - 25);
		}
		else if(action == 3){
			return make_pair(pos.first, pos.second + 25);
		}
		else return make_pair(pos.first, pos.second);
	}

	bool is_wall(int parallel_wall[16][17], int vertical_wall[17][16], pair<int, int> pos,int action){
        int x,y;
        x = pos.first / 25;
        y = pos.second / 25;
		if(action == 0){
			if(vertical_wall[x][y]==0) return true;
			else return false;
		}
		else if(action == 1){
			if(vertical_wall[x+1][y]==0) return true;
			else return false;
		}
		else if(action == 2){
			if(parallel_wall[x][y]==0) return true;
			else return false;
		}
		else if(action == 3){
			if(parallel_wall[x][y+1]==0) return true;
			else return false;
		}
		else return false;
	}

	bool is_centered(pair<int, int> pos, pair<int, int> dir){
		int x_cord = pos.first - 1;
		int y_cord = pos.second - 1;
		
		if(x_cord % 25 == 0 && y_cord % 25 == 0){
			return true;
		}
		else if(((x_cord / 25 != (x_cord + dir.first) / 25) && ((x_cord + dir.first) % 25 != 0)) || ((y_cord / 25 != (y_cord + dir.second) / 25) && ((y_cord + dir.second) % 25 != 0))){
			return true;
		}
		else return false;
	}

    bool is_valid(int parallel_wall[16][17], int vertical_wall[17][16], pair<int, int> pos, int action, pair<int, int> dir){
        int x,y;
        x = pos.first / 25;
        y = pos.second / 25;

		if(is_centered(pos, dir)){
			if(action == 0){
				if(vertical_wall[x][y]==0 && !(dir.first > 0 && dir.first > dir.second)) return true;
				else return false;
			}
			else if(action == 1){
				if(vertical_wall[x+1][y]==0 && !(dir.first < 0 && dir.first < dir.second)) return true;
				else return false;
			}
			else if(action == 2){
				if(parallel_wall[x][y]==0 && !(dir.second > dir.first && dir.second > 0)) return true;
				else return false;
			}
			else if(action == 3){
				if(parallel_wall[x][y+1]==0 && !(dir.second < dir.first && dir.second < 0)) return true;
				else return false;
			}
			else return false;
		}
		else{
			return false;
		}
    }

	bool is_visited(vector<pair<int, int>> visited, pair<int, int> pos){
		for(int i = 0; i < visited.size(); i++){
			if(visited[i] == pos){
				return true;
			}
		}
		return false;
	}

	int dfs(pair<int, int> cur_pos, int ghostStat[4][2], std::vector<std::vector<int>> propsStat, int parallel_wall[16][17], int vertical_wall[17][16], vector<pair<int, int>> visited, int reward, int depth){
		pair<int, int> pos;
		int temp = 0;
		int max_reward = -999;
		pair<int, int> pos_cord = make_pair(cur_pos.first/25, cur_pos.second/25);

		for(int i = 0; i < propsStat.size(); i++){
			//pellet
			if(pos_cord == make_pair(propsStat[i][1]/25, propsStat[i][2]/25) && propsStat[i][0] == 2){
				reward += 10*depth;
			}
			//fruit
			else if(pos_cord == make_pair(propsStat[i][1]/25, propsStat[i][2]/25) && propsStat[i][0] == 1){
                if(pstat > 0){
                    reward += 50*depth;
                }
                else{
				    reward += 200*depth;
                }
			}
			//inactive landmine
			else if(pos_cord == make_pair(propsStat[i][1]/25, propsStat[i][2]/25) && propsStat[i][0] == 0){
				reward += 5*depth;
			}
			//active landmine
			else if((abs(cur_pos.first - (propsStat[i][1] - 7)) + abs(cur_pos.second - (propsStat[i][2] - 7))) <= 20 && propsStat[i][0] == 3){
				reward -= 200*depth;
			}
		}

        if(pstat > 0){
			for (int i = 0; i <4;i++){
				if(pos_cord == make_pair(ghostStat[i][0]/25, ghostStat[i][1]/25)){
					if(!((ghostStat[i][0]/25 <= 8 && ghostStat[i][0]/25 >= 7) && (ghostStat[i][1]/25 <= 8 && ghostStat[i][1]/25 >= 7))){
						reward += 200*depth;	
					}
				}
			}
		}

		int flag = 0;
		if(depth == 0){
			return reward;
		}
		else{
			for(int i = 0; i < 4; i++){
				if(is_wall(parallel_wall, vertical_wall, cur_pos, i)){
					pos = get_pos(cur_pos, i);
					if(is_visited(visited, pos) == false){
						flag = 1;
						visited.push_back(cur_pos);
						temp = dfs(pos, ghostStat, propsStat,parallel_wall, vertical_wall, visited, reward, depth - 1);
						if(temp > max_reward){
							max_reward = temp;
						}
					}
				}
			}
			if(flag == 0){
				return reward;
			}
			else{
				return max_reward;
			}
		}
	}

	int gdfs(pair<int, int> cur_pos, int ghostStat[4][2], std::vector<std::vector<int>> propsStat, int parallel_wall[16][17], int vertical_wall[17][16], vector<pair<int, int>> gvisited, int reward, int depth){
		pair<int, int> pos;
		int temp = 0;
		int max_reward = 0;
		int x,y;
        x = cur_pos.first / 25;
        y = cur_pos.second / 25;

		for (int i = 0; i <4;i++){
			if(x == ghostStat[i][0]/25 && y == ghostStat[i][1]/25){
				reward += 200*depth;
				return reward;					
			}
		}
		if(depth == 0){
			return reward;
		}
		else{
			for(int i = 0; i < 4; i++){
				if(is_wall(parallel_wall, vertical_wall, cur_pos, i)){
					pos = get_pos(cur_pos, i);
					if(is_visited(gvisited, pos) == false){
						gvisited.push_back(cur_pos);
						temp = gdfs(pos, ghostStat, propsStat,parallel_wall, vertical_wall, gvisited, reward, depth - 1);
						if(temp > max_reward){
							max_reward = temp;
						}
					}
				}
			}
			return max_reward;
		}
	}

	int ndfs(pair<int, int> cur_pos, int ghostStat[4][2], std::vector<std::vector<int>> propsStat, int parallel_wall[16][17], int vertical_wall[17][16], vector<pair<int, int>> visited, int reward, int depth){
		pair<int, int> pos;
		int temp = 0;
		int max_reward = 0;
		pair<int, int> pos_cord = make_pair(cur_pos.first/25, cur_pos.second/25);
		int x,y;
        x = cur_pos.first / 25;
        y = cur_pos.second / 25;

		for(int i = 0; i < propsStat.size(); i++){
			//pellet
			int mh = (abs(x - (propsStat[i][1]/25)) + abs(y - propsStat[i][2]/25));
			if(propsStat[i][0] == 2){
				reward += (50 - mh)*(7 - depth);
			}
			//fruit
			else if(propsStat[i][0] == 1){
                if(pstat > 0){
                    reward += (50 - mh)*(7 - depth);
                }
                else{
				    reward += (200 - mh)*(7 - depth);
                }
			}
			//inactive landmine
			else if(propsStat[i][0] == 0){
				reward += (40 - mh)*(7 - depth);
			}
		}

		if(depth == 0){
			return reward;
		}
		else{
			for(int i = 0; i < 4; i++){
				if(is_wall(parallel_wall, vertical_wall, cur_pos, i)){
					pos = get_pos(cur_pos, i);
					if(is_visited(visited, pos) == false){
						visited.push_back(cur_pos);
						temp = dfs(pos, ghostStat, propsStat,parallel_wall, vertical_wall, visited, reward, depth - 1);
						if(temp > max_reward){
							max_reward = temp;
						}
					}
				}
			}
			return max_reward;
		}
	}

	int no_p(pair<int, int> cur_pos  , std::vector<std::vector<int>> propsStat){
		int x,y;
        x = cur_pos.first / 25;
        y = cur_pos.second / 25;
		int score = 0;

		for(int i = 0; i < propsStat.size();i++){
            if(propsStat[i][0] != 3){
			    score += 50 - (abs(x - (propsStat[i][1]/25)) + abs(y - propsStat[i][2]/25));
            }
		}
		return score;
	}

	bool is_back(pair<int, int> cur_pos, int x, int y){
		if(dir.first > 0){
			if((cur_pos.first/25 >= x/25) && (cur_pos.second/25 == y/25)){
				return true;
			}
		}
		else if(dir.first < 0){
			if((cur_pos.first/25 <= x/25) && (cur_pos.second/25 == y/25)){
				return true;
			}
		}
		else if(dir.second > 0){
			if((cur_pos.second/25 >= y/25) && (cur_pos.first/25 == x/25)){
				return true;
			}
		}
		else if(dir.second < 0){
			if((cur_pos.second/25 <= y/25) && (cur_pos.first/25 == x/25)){
				return true;
			}	
		}
		return false;
	}

	int landmine(pair<int, int> cur_pos, int ghostStat[4][2], int otherPlayerStat[3][5]){
		for(int i=0; i < 4; i++){
			if(is_back(cur_pos, ghostStat[i][0], ghostStat[i][1])){
				if(abs(cur_pos.first/25 - ghostStat[i][0]/25 ) + abs(cur_pos.second/25 - ghostStat[i][1]/25 ) <= 1){
					return true;
				}
			}
		}
		
		for(int i=0; i < 3; i++){
			if(is_back(cur_pos, otherPlayerStat[i][0], otherPlayerStat[i][1]) ){
				if(abs(cur_pos.first/25 - otherPlayerStat[i][0]/25 ) + abs(cur_pos.second/25 - otherPlayerStat[i][1]/25 ) <= 2){
					return 1;
				}
			}
		}
		return false;
	}

	void ldfs(pair<int, int> cur_pos, int ghostStat[4][2], int otherPlayerStat[3][5], int parallel_wall[16][17], int vertical_wall[17][16], vector<pair<int, int>> visited, int depth, int lm){
		pair<int, int> pos;
		int temp = 0;
		int max_reward = 0;
		pair<int, int> pos_cord = make_pair(cur_pos.first/25, cur_pos.second/25);
		int x,y;
        x = cur_pos.first / 25;
        y = cur_pos.second / 25;

		for(int i = 0; i < 4; i++){
			if(is_back(cur_pos, ghostStat[i][0], ghostStat[i][1])){
				if(abs(cur_pos.first/25 - ghostStat[i][0] ) + abs(cur_pos.second/25 - ghostStat[i][1]) <= 1){
					lm = 1;
				}
			}
		}
		for(int i=0; i < 3; i++){
			if(is_back(cur_pos, otherPlayerStat[i][0], otherPlayerStat[i][1]) ){
				if(abs(cur_pos.first/25 - otherPlayerStat[i][0] ) + abs(cur_pos.second/25 - otherPlayerStat[i][1]) <= 1){
					lm = 1;
				}
			}
		}

		if(depth == 0){
			return ;
		}
		else{
			for(int i = 0; i < 4; i++){
				if(is_wall(parallel_wall, vertical_wall, cur_pos, i)){
					pos = get_pos(cur_pos, i);
					if(is_visited(visited, pos) == false){
						visited.push_back(cur_pos);
						ldfs(pos, ghostStat, otherPlayerStat, parallel_wall, vertical_wall, visited, depth - 1, lm);
					}
				}
			}
			return;
		}
	}

	void GetStep(int playerStat[4], int ghostStat[4][2], std::vector<std::vector<int>> propsStat,int parallel_wall[16][17], int vertical_wall[17][16], int otherPlayerStat[3][5])
	{
		unsigned seed;
		seed = (unsigned)time(NULL);
		srand(seed);
		int temp_step[2] = {0};
		vector<pair<int, int>> visited;
		vector<pair<int, int>> gvisited;
		pair<int, int> cur_pos = make_pair(playerStat[0], playerStat[1]);
		pair<int, int> pos;
		if(abs(cur_pos.first - prev_pos.first) >= 5 || abs(cur_pos.second - prev_pos.second) >= 5){
			dir = make_pair(cur_pos.first - prev_pos.first, cur_pos.second - prev_pos.second);
		}
		// direction
		int max = -999;
		int max_action = 0;
		int flag = 0;
        int temp2;
        pstat = playerStat[3];
        int reward[4];
		for (int i = 0; i < 4;i++){
			if(is_valid(parallel_wall, vertical_wall, cur_pos, i, dir)){
				visited.push_back(cur_pos);
				pos = get_pos(make_pair(playerStat[0], playerStat[1]), i);
				reward[i] = dfs(pos, ghostStat, propsStat, parallel_wall, vertical_wall, visited, 0, 9 - pruning);
				if(pstat <= 0){
                    temp2 = gdfs(pos, ghostStat, propsStat, parallel_wall, vertical_wall, gvisited, 0, 3);
					reward[i] -= temp2;
				}
			}
		}
        for(int i = 0; i < 4; i++){
            if(is_valid(parallel_wall, vertical_wall, cur_pos, i, dir)){
                if(max < reward[i]){
                    max = reward[i];
                    max_action = i;	
                }
            }
        }

		//no p neighborhood
		vector<pair<int, int>> nvisited;
		if(max < -500){
			temp_step[0] = 4;
		}
		else if(max == 0){
			for(int i = 0; i <4; i++){
				if(is_valid(parallel_wall, vertical_wall, cur_pos, i, dir)){
					pos = get_pos(make_pair(playerStat[0], playerStat[1]), i);
					int temp  = ndfs(pos, ghostStat, propsStat, parallel_wall, vertical_wall, nvisited, 0, 6);
					if(max < temp){
						max = temp;
						max_action = i;
					}
				}	
			}
            temp_step[0] = max_action;
            prev_step = max_action;
		}
		else{
			temp_step[0] = max_action;
			prev_step = max_action;
		}
		prev_pos = cur_pos;

		// is_throwLandmine
		temp_step[1] = 0;
		int t;
		if (playerStat[2] > 0)
		{
			t = landmine(cur_pos, ghostStat, otherPlayerStat);
			if(t == true &&  prev_mine == false){
				temp_step[1] = t;
			}
			prev_mine = temp_step[1];
		}

		if (killed == false)
		{
			step.resize(2);
			step[0] = temp_step[0];
			step[1] = temp_step[1];
		}
		delete this;
	}

	Mythread(int playerStat[4], int ghostStat[4][2], std::vector<std::vector<int>> propsStat, int parallel_wall[16][17], int vertical_wall[17][16], int otherPlayerStat[3][5])
	{
		std::thread t2(&Mythread::GetStep, this, playerStat, ghostStat, propsStat,parallel_wall, vertical_wall, otherPlayerStat);
		t2.detach();
	}
};

int main()
{
	int id_package;
	/*
	playerStat: <x,y,n_landmine,super_time,score>
	otherPlayerStat: 3*<x,y,n_landmine,super_time>
	ghostStat: 4*<[x,y],[x,y],[x,y],[x,y]>
	propsStat: n_props*<type,x,y>
	*/
	int parallel_wall[16][17];
	int vertical_wall[17][16];
	int playerStat[5];
	int otherPlayerStat[3][5];
	int ghostStat[4][2];
	int cur_dir = 0;
	std::vector<std::vector<int>> propsStat;
	// receive map
	if (GetMap(parallel_wall, vertical_wall))
	{   
		// start game
		while (true)
		{
			if (GetGameStat(id_package, playerStat, otherPlayerStat, ghostStat, propsStat))
				break;
			step.resize(2);
			step[0] = 5;
			step[1] = 2;
			Mythread *mythread = new Mythread(playerStat, ghostStat, propsStat,parallel_wall,vertical_wall, otherPlayerStat);
			Sleep(40);
			if (step[0] == 5)
			{
				pruning++;
				std::cout << "timeout" << std::endl;
				mythread->killed = true;
				step[0] = 4;
				step[1] = 0;
			}
			SendStep(id_package, step);
		}
	}
}
