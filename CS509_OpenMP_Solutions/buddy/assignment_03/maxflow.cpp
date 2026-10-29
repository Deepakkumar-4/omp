#include "../../common.hpp"
struct Edge
{
    int to, rev, cap;
};
int main(int argc, char **argv)
{
    int threads = getThreads(argc, argv);
    // Dinic example. Residual-flow updates must stay serial to avoid conflicts.
    auto solve = [&](bool par)
    {
        int n = 6;
        vector<vector<Edge>> g(n);
        auto add = [&](int u, int v, int c)
        {int a=g[u].size(),b=g[v].size();g[u].push_back({v,b,c});g[v].push_back({u,a,0}); };
        add(0, 1, 10);
        add(0, 2, 10);
        add(1, 3, 4);
        add(1, 4, 8);
        add(2, 4, 9);
        add(4, 3, 6);
        add(3, 5, 10);
        add(4, 5, 10);
        int flow = 0;
        vector<int> level(n), ptr(n);
        auto dfs = [&](auto &&self, int u, int pushed) -> int
        {
            if (u == 5 || pushed == 0)
                return pushed;
            for (int &k = ptr[u]; k < (int)g[u].size(); k++)
            {
                Edge &e = g[u][k];
                if (level[e.to] != level[u] + 1 || e.cap <= 0)
                    continue;
                int sent = self(self, e.to, min(pushed, e.cap));
                if (sent == 0)
                    continue;
                e.cap -= sent;
                g[e.to][e.rev].cap += sent;
                return sent;
            }
            return 0;
        };
        while (true)
        {
            fill(level.begin(), level.end(), -1);
            queue<int> q;
            q.push(0);
            level[0] = 0;
            while (!q.empty())
            {
                int u = q.front();
                q.pop();
                for (auto &e : g[u])
                    if (e.cap > 0 && level[e.to] < 0)
                    {
                        level[e.to] = level[u] + 1;
                        q.push(e.to);
                    }
            }
            if (level[5] < 0)
                break;
            fill(ptr.begin(), ptr.end(), 0);
            while (int sent = dfs(dfs, 0, 100000000))
                flow += sent;
        }
        // Safe parallel postprocessing: count residual reachable cut vertices.
        if (par)
        {
            int reached = 0;
#pragma omp parallel for reduction(+ : reached) num_threads(threads)
            for (int v = 0; v < n; v++)
                if (level[v] >= 0)
                    reached++;
            cout << "Residual reachable vertices: " << reached << '\n';
        }
        return flow;
    };
    double t = omp_get_wtime();
    auto a = solve(false);
    double st = omp_get_wtime() - t;
    t = omp_get_wtime();
    auto b = solve(true);
    double pt = omp_get_wtime() - t;
    cout << "Max flow: " << b << '\n';
    status(a == b, st, pt, threads);
    return a != b;
}
