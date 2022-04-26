
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
		// cout << dir.first << " " << dir.second << endl;
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
		
		// if(pstat <= 0){
			if(x_cord % 25 == 0 && y_cord % 25 == 0){
				return true;
			}
			else if(((x_cord / 25 != (x_cord + dir.first) / 25) && ((x_cord + dir.first) % 25 != 0)) || ((y_cord / 25 != (y_cord + dir.second) / 25) && ((y_cord + dir.second) % 25 != 0))){
				return true;
			}
			else return false;
			// if(((x_cord % 25 - 21) * ((x_cord + dir.first) % 25 - 5)) <=  0 || ((y_cord % 25 - 21) * ((y_cord + dir.second) % 25 - 5)) <=  0){
			// 	return true;
			// }
			// else if(dir.first == 0 && dir.second == 0){
			// 	return true;
			// }
			// else{
			// 	return false;
			// }
		// }
		// else{
		// 	if(((x_cord % 25 - 18) * (abs(x_cord + dir.first) % 25 - 8)) <=  0 || ((y_cord % 25 - 18) * (abs(y_cord + dir.second) % 25 - 8)) <=  0){
		// 		return true;
		// 	}
		// 	else if(dir.first == 0 && dir.second == 0){
		// 		return true;
		// 	}
		// 	else{
		// 		return false;
		// 	}
		// }
	}

    bool is_valid(int parallel_wall[16][17], int vertical_wall[17][16], pair<int, int> pos, int action, pair<int, int> dir){
        int x,y;
        x = pos.first / 25;
        y = pos.second / 25;
		// cout << dir.first << " " << dir.second << endl;

		if(is_centered(pos, dir)){
		// if(((pos.first - 1) % 25 == 0) && ((pos.second - 1) % 25 == 0)){
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

	int dfs(pair<int, int> cur_pos, int ghostStat[4][2], std::vector<std::vector<int>> propsStat, int parallel_wall[16][17], int vertical_wall[17][16], vector<pair<int, int>> visited, int reward, int depth){
		pair<int, int> pos;
		int temp = 0;
		int max_reward = 0;
		pair<int, int> pos_cord = make_pair(cur_pos.first/25, cur_pos.second/25);

		// reward += 1;

		for(int i = 0; i < propsStat.size(); i++){
			// cout << propsStat[i][0] << " " << propsStat[i][1] << " " << propsStat[i][2] << endl;
			//active landmine
			// if(propsStat[i][0]==3){
			// 	cout << " LANDMINE:" << propsStat[i][1] << " " << propsStat[i][2] <<  " x y  " << propsStat[i][1]/25 << " " << propsStat[i][2]/25<< endl;
			// }

			//pellet
			if(pos_cord == make_pair(propsStat[i][1]/25, propsStat[i][2]/25) && propsStat[i][0] == 2){
				reward += 10*depth;
			}
			//fruit
			else if(pos_cord == make_pair(propsStat[i][1]/25, propsStat[i][2]/25) && propsStat[i][0] == 1){
				// cout << "hello there hello there hello there" << endl << endl << endl;
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
			// else if(pos_cord == make_pair((propsStat[i][1])/25, (propsStat[i][2])/25) && propsStat[i][0] == 3){
			// 	reward -= 50*depth;
			// }
			else if((abs(cur_pos.first - propsStat[i][1] ) + abs(cur_pos.second - propsStat[i][2])) <= 18 && propsStat[i][0] == 3){
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

	int gdfs(pair<int, int> cur_pos, int ghostStat[4][2], std::vector<std::vector<int>> propsStat, int parallel_wall[16][17], int vertical_wall[17][16], vector<pair<int, int>> gvisited, int reward, int depth){
		
		pair<int, int> pos;
		int temp = 0;
		int max_reward = 0;
		int x,y;
        x = cur_pos.first / 25;
        y = cur_pos.second / 25;

		for (int i = 0; i <4;i++){
			if(x == ghostStat[i][0]/25 && y == ghostStat[i][1]/25){
				reward += 300*depth;
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

	bool is_visited(vector<pair<int, int>> visited, pair<int, int> pos){
		for(int i = 0; i < visited.size(); i++){
			if(visited[i] == pos){
				return true;
			}
		}
		return false;
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

	void GetStep(int playerStat[4], int ghostStat[4][2], std::vector<std::vector<int>> propsStat,int parallel_wall[16][17], int vertical_wall[17][16])
	{
        // cout << "playerStat: " << playerStat[4] << endl;
		// Example:
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
        cout << "pstat " << pstat << endl;
		cout << "current position and direction: "  << cur_pos.first << " " << cur_pos.second << " " << dir.first << " " << dir.second << endl;
        int reward[4];
		for (int i = 0; i < 4;i++){
			cout << "action " << i << endl;
			if(is_valid(parallel_wall, vertical_wall, cur_pos, i, dir)){
				visited.push_back(cur_pos);
				pos = get_pos(make_pair(playerStat[0], playerStat[1]), i);
				reward[i] = dfs(pos, ghostStat, propsStat, parallel_wall, vertical_wall, visited, 0, 8);
				if(pstat <= 0){
                    temp2 = gdfs(pos, ghostStat, propsStat, parallel_wall, vertical_wall, gvisited, 0, 3);
					reward[i] -= temp2;
                    reward[reverse[i]] += temp2;
				}
			}
		}
        for(int i = 0; i < 4; i++){
            if(is_valid(parallel_wall, vertical_wall, cur_pos, i, dir)){
                cout << "reward: " << reward[i] << endl;
                if(max < reward[i]){
                    max = reward[i];
                    max_action = i;	
                }
            }
        }


		//no p neighborhood


		if(max < -500){
			cout << "3. no action available: " << endl;
			temp_step[0] = 4;
		}
		else if(max == 0){
			cout << " no p neighborhood";
			for(int i = 0; i <4; i++){
				if(is_valid(parallel_wall, vertical_wall, cur_pos, i, dir)){
					pos = get_pos(make_pair(playerStat[0], playerStat[1]), i);
					int temp = no_p(pos,propsStat);
					if(max < temp){
						cout << "i " << i << " reward: " << temp << endl;
						max = temp;
						max_action = i;
					}
				}	
			}
            temp_step[0] = max_action;
            prev_step = max_action;
		}
		else{
			cout << "best move: " << max_action << endl;
			temp_step[0] = max_action;
			prev_step = max_action;
		}

		// cout << "stat " << playerStat[3] << endl;
		cout << "final action: " << temp_step[0] << endl;

		cout << endl << endl;

		prev_pos = cur_pos;

		// cout << "move " << temp_step[0] << " " << prev_step << endl;
		// is_throwLandmine
		temp_step[1] = 0;
		if (playerStat[2] > 0)
		{
			temp_step[1] = rand() % 2;
		}
		if (killed == false)
		{
			step.resize(2);
			step[0] = temp_step[0];
			step[1] = temp_step[1];
		}
		delete this;
	}

	Mythread(int playerStat[4], int ghostStat[4][2], std::vector<std::vector<int>> propsStat, int parallel_wall[16][17], int vertical_wall[17][16])
	{
		std::thread t2(&Mythread::GetStep, this, playerStat, ghostStat, propsStat,parallel_wall, vertical_wall);
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
			// if(count == 15) count = 0;
			Mythread *mythread = new Mythread(playerStat, ghostStat, propsStat,parallel_wall,vertical_wall);
			Sleep(40);
			if (step[0] == 5)
			{
				std::cout << "timeout" << std::endl;
				mythread->killed = true;
				step[0] = 4;
				step[1] = 0;
			}
			// cout << step[0] << endl;
			SendStep(id_package, step);
		}
	}
}
