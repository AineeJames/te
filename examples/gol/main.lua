local w, h
local grid = {}
local nextGrid = {}
local timer = 0
local updateInterval = 0.01 -- seconds per step
local running = true

-- helper function to create empty grid
local function createGrid()
    local g = {}
    for y = 1, h do
        g[y] = {}
        for x = 1, w do
            g[y][x] = 0
        end
    end
    return g
end

-- initialize
function te.load()
    w, h = te.window.getDimensions()
    grid = createGrid()
    nextGrid = createGrid()

    -- random starting pattern
    for y = 1, h do
        for x = 1, w do
            grid[y][x] = (math.random() > 0.8) and 1 or 0
        end
    end
end

-- count alive neighbors
local function countNeighbors(x, y)
    local count = 0
    for dy = -1, 1 do
        for dx = -1, 1 do
            if not (dx == 0 and dy == 0) then
                local nx = x + dx
                local ny = y + dy
                if nx >= 1 and nx <= w and ny >= 1 and ny <= h then
                    count = count + grid[ny][nx]
                end
            end
        end
    end
    return count
end

-- update grid
local function step()
    for y = 1, h do
        for x = 1, w do
            local n = countNeighbors(x, y)
            if grid[y][x] == 1 then
                -- alive
                nextGrid[y][x] = (n == 2 or n == 3) and 1 or 0
            else
                -- dead
                nextGrid[y][x] = (n == 3) and 1 or 0
            end
        end
    end
    -- swap grids
    grid, nextGrid = nextGrid, grid
end

function te.update(dt)
    timer = timer + dt
    if running and timer >= updateInterval then
        step()
        timer = 0
    end
end

function te.keypressed(key)
    if key == "space" then
        running = not running -- pause/resume
    elseif key == "r" then
        grid = createGrid()
        for y = 1, h do
            for x = 1, w do
                grid[y][x] = (math.random() > 0.8) and 1 or 0
            end
        end
    elseif key == "up" then
        -- increase speed by reducing interval
        updateInterval = math.max(0.01, updateInterval * 0.8)
    elseif key == "down" then
        -- decrease speed by increasing interval
        updateInterval = updateInterval * 1.25
    elseif key == "escape" then
        te.event.quit(0) -- exit with 0 exit code
    end
end

local neighborColors = {
    [0] = BLACK,
    [1] = DARK_GRAY,
    [2] = LIGHT_GREEN,
    [3] = GREEN,
    [4] = YELLOW,
    [5] = LIGHT_RED,
    [6] = RED,
    [7] = MAGENTA,
    [8] = WHITE,
}

function te.draw()
    te.graphics.clear()

    for y = 1, h do
        for x = 1, w do
            if grid[y][x] == 1 then
                local n = countNeighbors(x, y)
                local color = neighborColors[n] or WHITE
                te.graphics.setColor(color, BLACK)
                te.graphics.setCell(0xDB + 1, x, y)
            end
        end
    end

    -- HUD
    te.graphics.setColor(BLACK, WHITE)
    te.graphics.print("Space = Pause/Resume | R = Reset", 1, 1)
    te.graphics.print(string.format("Running: %s", running and "Yes" or "No"), 1, 2)
    te.graphics.print(string.format("Speed: %.2f steps/sec", 1 / updateInterval), 1, 3)

    local fpsStr = string.format("FPS: %d", te.window.getFPS())
    te.graphics.print(fpsStr, w - #fpsStr + 1, 1);
end
