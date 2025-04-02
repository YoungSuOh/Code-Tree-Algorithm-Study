#include <iostream>
#include <vector>
#include <queue>
#include <set>

using namespace std;


// 변수 선언
int K; 
int M;
int dx[4] = { 0,0,1,-1 };
int dy[4] = { 1,-1,0,0 };

vector<vector<int>>grid, sub_grid, visited, rem_select;
queue<int>next_num; // 다음 유물 번호

struct POS {
    int x, y;
};

struct NODE {
    int val, deg, x, y; // 값, 각도, 행, 열
};

struct cmp {
    bool operator()(const NODE& a, const NODE& b) {
        if (a.val != b.val) return a.val < b.val; // 유물 1차 획득 가치 우선
        if (a.deg != b.deg) return a.deg > b.deg; // 회전 각도가 가장 작은 방법 선택
        if (a.y != b.y) return a.y > b.y; // 회전 중심의 열이 가장 작은 구간
        return a.x > b.x; // 행이 가장 작은 구간
    }
};

void copy_2d(const vector<vector<int>>& from, vector<vector<int>>& to) {
    for (int i = 0; i < to.size(); ++i) {
        for (int j = 0; j < to[i].size(); ++j) {
            to[i][j] = from[i][j];
        }
    }
}

void fill_2d(vector<vector<int>>& vec, int data) {
    for (int i = 0; i < vec.size(); ++i) {
        fill(vec[i].begin(), vec[i].end(), data);
    }
}

void rotate_grid(int x, int y, int deg) {
    int switch_len = 3, grid_len = grid.size();

    copy_2d(grid, sub_grid);  // grid 복사

    if (deg == 1) { // 90'
        for (int i = 0; i < switch_len; ++i) {
            for (int j = 0; j < switch_len; ++j) {
                sub_grid[x + i][y + j] = grid[x + switch_len - 1 - j][y + i];
            }
        }
    }

    else if (deg == 2) {  // 180'
        for (int i = 0; i < switch_len; ++i) {
            for (int j = 0; j < switch_len; ++j) {
                sub_grid[x + i][y + j] = grid[x + switch_len - 1 - i][y + switch_len - 1 - j];
            }
        }
    }

    else if (deg == 3) { // 270'
        for (int i = 0; i < switch_len; ++i) {
            for (int j = 0; j < switch_len; ++j) {
                sub_grid[x + i][y + j] = grid[x + j][y + switch_len - 1 - i];
            }
        }
    }
}

bool is_valid_5x5(int x, int y) {
    if (x < 0 || y < 0 || x >= 5 || y >= 5) return false;
    return true;
}


int get_score(const vector<vector<int>>& grid, bool do_mark) {
    fill_2d(visited, 0); // 방문 초기화
    fill_2d(rem_select, 0);

    queue<POS>bfs_q;
    set<int>mark_set;

    int acc = 0, mark = 0;
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[i].size(); j++) {
            if (visited[i][j]) continue;
            visited[i][j] = 1, rem_select[i][j] = ++mark;

            // bfs 큐 비우기
            while (!bfs_q.empty()) bfs_q.pop();
            bfs_q.push({ i,j });

            int cur_acc = 1, cur = grid[i][j];
            while (!bfs_q.empty()) {
                POS top = bfs_q.front(); bfs_q.pop();

                for (int k = 0; k < 4; k++) {
                    int nextX = top.x + dx[k];
                    int nextY = top.y + dy[k];
                    // 배열 범위 벗어나거나, 이미 방문했거나, 현재와 똑같은 숫자가 아니라면 continue
                    if (!is_valid_5x5(nextX, nextY) || visited[nextX][nextY] || grid[nextX][nextY] != cur) continue;
                    visited[nextX][nextY] = true; rem_select[nextX][nextY] = mark;

                    cur_acc++; bfs_q.push({ nextX, nextY });
                }
            }

            if (cur_acc >= 3) { // 조각들이 3개 이상 연결된 경우 유물
                acc += cur_acc;
                if (do_mark) mark_set.insert(mark);
            }

        }
    }

    if (do_mark) {
        for (int i = 0; i < grid.size(); i++) {
            for (int j = 0; j < grid[i].size(); j++) {
                if (mark_set.find(rem_select[i][j]) == mark_set.end()) rem_select[i][j] = 0;
                else rem_select[i][j] = 1;
            }
        }
    }

    return acc;
}


NODE simulate(int x, int y, int deg) {
    // x, y을 좌상단으로 deg 만큼 3*3 회전 
    rotate_grid(x, y, deg);

    // 1차 가치 획득
    int acc = get_score(sub_grid, false);
    return { acc, deg, x, y };
}

bool rotate_small_grid() {
    bool flag_continue = true;
    priority_queue<NODE, vector<NODE>, cmp> pq;

    for (int i = 0; i <= 2; ++i) {  // x : 0~2
        for (int j = 0; j <= 2; ++j) { /// y : 0~2
            for (int deg = 1; deg <= 3; ++deg) {  // 0도, 90도, 180도, 270도 회전 비교
                NODE sim = simulate(i, j, deg);
                pq.push(sim);
            }
        }
    }

    NODE top = pq.top();

    // 점수 못 얻을 경우, 리턴
    if (top.val == 0) {
        return flag_continue = false;
    }

    // pq 최상단 값 실제 grid에 반영
    rotate_grid(top.x, top.y, top.deg);
    copy_2d(sub_grid, grid);  // 이제 grid에 sub_grid를 복사하여 최종 반영

    return flag_continue;
}


int get_score_cascaded() {
    int acc = 0;
    while (true) {
        int cur_score = get_score(grid, true);
        if (cur_score == 0) return acc;
        acc += cur_score;


        // 빈 공간 채우기
        for (int i = 0; i < 5; i++) {  // 열번호가 작은 순으로
            for (int j = 4; j >= 0; j--) { // 동률이면 행번호가 큰 순으로 조각이 생겨남
                if (rem_select[j][i] == 1) {
                    grid[j][i] = next_num.front(); next_num.pop();
                }
            }
        }
    }
}

void run() {
    bool flag = true;

    while (K--) {
        flag = rotate_small_grid();
        if (!flag) return;
        cout << get_score_cascaded() << ' ';
    }
}

void input() {
    cin >> K >> M;

    grid.resize(5), sub_grid.resize(5), visited.resize(5), rem_select.resize(5);

    for (int i = 0; i < 5; i++) {
        grid[i].resize(5), sub_grid[i].resize(5), visited[i].resize(5), rem_select[i].resize(5);
        for (int j = 0; j < 5; j++) {
            cin >> grid[i][j];
        }
    }

    int num;
    for (int i = 0; i < M; i++) {
        cin >> num;
        next_num.push(num);
    }

}

void init() {
    while (!next_num.empty()) next_num.pop();
}

int main() {
    init();
    input();
    run();
    return 0;
}