def MaxAreaOfIsland(grid):
    row = len(grid)
    col = len(grid[0])
    arrived = [[False for j in range(col)] for i in range(row)]
    ans = 0

    def DFS(x, y):
        if x >= 0 and x < row and y >= 0 and y < col and not arrived[x][y] and grid[x][y] == 1:
            arrived[x][y] = True
            return 1 + DFS(x - 1, y) + DFS(x + 1, y) + DFS(x, y - 1) + DFS(x, y + 1)
        else:
            return 0
    for i in range(row):
        for j in range(col):
            area = DFS(i, j)
            if area > ans:
                ans = area
    return ans
