
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
int prev_prev_step = 0;
int test[3] = {0, 1, 4};
int reverse = 0;
int count = 0;
int nvalid[5]={0};
int stuck = 0;
int maxv[4] = {0};
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

    
    bool is_valid(int parallel_wall[16][17], int vertical_wall[17][16], pair<int, int> pos,int action){
        int x,y;
        x = pos.first / 25;
        y = pos.second / 25;

		if( (action ^ prev_step) == 1 ){
			return false;
		}

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
		int max_reward = 0;
		int x,y;
        x = cur_pos.first / 25;
        y = cur_pos.second / 25;

		int neighbor = 0;

		for(int i = 0; i < propsStat.size(); i++){

			if(cur_pos == make_pair(propsStat[i][1] - 10, propsStat[i][2] - 10) && propsStat[i][0] == 2){
				reward += 1000*depth;
			}

			neighbor = abs(x - propsStat[i][1]%25)+ abs(y - propsStat[i][1]%25);
			//cout <<  neighbor ;
			if( neighbor < 32){
				reward += 32 - neighbor;
			}
		}

		if(depth == 3){
			for(int i = 0; i < 4 ; i++){
				if((x - ghostStat[i][0]%25 + y - ghostStat[i][1]%25) < 2){
					reward -= 100;
					return reward;
				}		
			}
		}

		if(depth == 0){
			return reward;
		}
		else{
			for(int i = 0; i < 4; i++){
				if(is_valid(parallel_wall, vertical_wall, cur_pos, i)){
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

	bool is_centered(pair<int, int> pos){
		if(((pos.first - 1) % 25 == 0) && ((pos.second - 1) % 25 == 0)){
			// cout << "true" << endl;
			return true;
		}
		else{
			return false;
		}
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
		pair<int, int> cur_pos = make_pair(playerStat[0], playerStat[1]);
		pair<int, int> pos;
		// cout << dfs(pos, ghostStat, propsStat, parallel_wall, vertical_wall, visited, 0, 3) << endl;
		// direction
		int max = 0;
		int max_action = 0;
		int temp;
		
		if(is_centered(cur_pos)){
			for (int i = 3; i >= 0;i--){
				if(is_valid(parallel_wall, vertical_wall, cur_pos, i)){
					nvalid[i] = 0;
					visited.push_back(cur_pos);
					pos = get_pos(make_pair(playerStat[0], playerStat[1]), i);
					temp = dfs(pos, ghostStat, propsStat, parallel_wall, vertical_wall, visited, 0, 3);
					maxv[i] = temp;
					if(max < temp){
						max = temp;
						max_action = i;
					}
					
				}
				else{
					nvalid[i] = 1;
					maxv[i] = 0;
				}
			}
			
			temp_step[0] = max_action;
		}
		else{
			temp_step[0] = prev_step;
		}

		if(prev_step == temp_step[0]){
			stuck++;
			if(stuck > 25){
				nvalid[prev_step] = 1;
				for(int i=0;i<4;i++){
					if(nvalid[i]==0){
						if(max_action < maxv[i]){
							max_action = i;
							temp_step[0] = max_action;
						}
						stuck = 0;
					}
				}
			}
		}

		prev_step = temp_step[0];
		memset(nvalid,0,sizeof(nvalid));


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

	void one_step(int playerStat[4], int ghostStat[4][2], std::vector<std::vector<int>> propsStat,int parallel_wall[16][17], int vertical_wall[17][16])
	{
        // cout << "playerStat: " << playerStat[4] << endl;       
		// Example:
		unsigned seed;
		seed = (unsigned)time(NULL);
		srand(seed);
		int temp_step[2] = {0};
		// direction
        int max = 0;
        int max_step = 0;
        int score;
		pair<int, int> pos;
        pos.first = playerStat[0];
        pos.second = playerStat[1];
        for (int action = 0; action < 5; action++){
            if(is_valid(parallel_wall, vertical_wall, pos, action)){
                score = simulate(playerStat, ghostStat, propsStat, action);
                if(score >= max){
                    max = score;
                    max_step = action;
                }
            }
        }
        if (max == 0){
            max_step = rand() % (4);
        }
		temp_step[0] = max_step;
		// is_throwLandmine
		temp_step[1] = 0;
		if (playerStat[2] > 0){
			temp_step[1] = rand() % 2;
		}
		if (killed == false){
			step.resize(2);
			step[0] = temp_step[0];
			step[1] = temp_step[1];
		}
		delete this;
	}

	int simulate(int playerStat[4], int ghostStat[4][2], std::vector<std::vector<int>> propsStat, int action){
        int score = 0;
        int step = 0;
		pair<int, int> pos;
        pos = get_pos(make_pair(playerStat[0], playerStat[1]), action);

        for (int i = 0; i < propsStat.size(); i++){
            // cout << pos.first << " " << pos.second << " " << propsStat[i][1] << " " << propsStat[i][2] << " " << propsStat[i][0] << endl;
            if(pos.first + 10 == propsStat[i][1] && pos.second + 10 == propsStat[i][2]){
                score += 10;
            }
        }
        // cout << score << endl;
        return score;
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
