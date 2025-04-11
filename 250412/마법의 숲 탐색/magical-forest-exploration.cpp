#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;
int R, C, K, ans;

int dy[4] = { 1,-1,0,0 };
int dx[4] = { 0,0,1,-1 };

// 0은 빈칸, 나머지는 골렘 번호
// 낙하지점을 0으로 시작하면 세로 길이 +2 하기
int Map[73][73];
// 출구 확인 
bool golem_exit[73][73];
// d : (0,1,2,3) = (북,동,남,서)
struct golem_info {
	int c, d;
};

vector<golem_info>golem;

void input() {
	cin >> R >> C >> K;
	for (int i = 0; i < K; i++) {
		int c, d;
		cin >> c >> d;
		golem.push_back({ c,d });
	}
}

bool isValid(int y, int x) {
	return Map[y][x] == 0 && Map[y - 1][x] == 0 && Map[y + 1][x] == 0 && Map[y][x - 1] == 0 && Map[y][x + 1] == 0 && y <= R + 1 && x >= 2 && x <= C-1;
}

bool isValid2(int y, int x) {
	return y <= R + 2 && x >= 1 && x <= C && Map[y][x] != 0;
}

bool isValid3(int y, int x,int num) {
	return y <= R + 2 && x >= 1 && x <= C && Map[y][x]==num;
}

void mapRemove() {
	for (int i = 0; i < 73; i++) {
		for (int j = 0; j < 73; j++) {
			Map[i][j] = 0;
			golem_exit[i][j] = false;
		}
	}
}

void bfs(int y,int x,int num) {
	queue<pair<int, int>>q;
	int max_y = y;
	// 첫 십자 좌표 체크 및 4방향 좌표 큐에 추가
	bool visited[73][73] = { false };
	visited[y][x] = true;
	for (int i = 0; i < 4; i++) {
		int ny = y + dy[i];
		int nx = x + dx[i];
		visited[ny][nx] = true;
		q.push({ ny,nx });
	}

	// 지금 위치가 출구 또는 지금 위치가 출구가 아닐 때 2가지로 나눔
	while (!q.empty()) {
		pair<int, int> now = q.front();
		q.pop();
		visited[now.first][now.second] = true;

		// 최대 y좌표 갱신
		max_y = max(max_y, now.first);
		// 지금 위치가 출구일 때
		if (golem_exit[now.first][now.second]) {
			for (int i = 0; i < 4; i++) {
				int ny = now.first + dy[i];
				int nx = now.second + dx[i];
				if (visited[ny][nx])continue;
				// 다음 위치가 빈칸이 아니면 추가
				if (!isValid2(ny, nx))continue;
				visited[ny][nx] = true;
				q.push({ ny,nx });
			}
		}
		// 지금 위치가 출구가 아닐 때
		else if (Map[now.first][now.second] >=1) {
			for (int i = 0; i < 4; i++) {
				int ny = now.first + dy[i];
				int nx = now.second + dx[i];
				if (visited[ny][nx])continue;
				// 다음 위치가 자신의 골렘 번호일 때만 추가
				if (!isValid3(ny, nx,Map[now.first][now.second]))continue;
				visited[ny][nx] = true;
				q.push({ ny,nx });
			}
		}
	}
	// 정답 추가
	ans += max_y - 2;
}

void moveGolem(golem_info a,int num) {
	// R x C 맵에서 시작 중앙점은 (0,C)
	// 실제 맵은 2행부터 있음!
	int sR = 0;
	int sC = a.c;
	int sD = a.d;
	
	while (1) {
		// 남쪽
		sR++;
		if (isValid(sR,sC)) {
			continue;
		}
		sR--;
		// 서쪽
		sC--;
		if (isValid(sR,sC)) {
			sR++;
			if (isValid(sR, sC)) {
				if (sD == 0)sD = 3;
				else sD--;
				continue;
			}
			else {
				sR--;
			}
		}
		sC++;
		// 동쪽
		sC++;
		if (isValid(sR,sC)) {
			sR++;
			if (isValid(sR, sC)) {
				sD = (sD + 1) % 4;
				continue;
			}
			else {
				sR--;
			}
		}
		sC--;
		// 3가지 다 안되면 종료
		break;
	}

	// 이동 후 맵 밖이면 초기화
	if (sR <= 3) {
		mapRemove();
		return;
	}
	// 이동 후 맵 안이면 적용
	else {
		Map[sR][sC] = num;
		Map[sR-1][sC] = num;
		Map[sR+1][sC] = num;
		Map[sR][sC - 1] = num;
		Map[sR][sC + 1] = num;
		// 북
		if (sD == 0) {
			golem_exit[sR - 1][sC] = true;
		}
		// 동
		else if (sD == 1) {
			golem_exit[sR][sC + 1] = true;
		}
		// 남
		else if (sD == 2) {
			golem_exit[sR + 1][sC] = true;
		}
		// 서
		else if (sD == 3) {
			golem_exit[sR][sC - 1] = true;
		}

		// 도착 후 맵에서 BFS를 통한 최종 목적지 구하기
		bfs(sR,sC,num);
	}

}

void func() {
	for (int i = 0; i < K; i++) {
		moveGolem(golem[i],i+1);
	}
	cout << ans;
}

int main() {
	input();
	func();
	return 0;
}
