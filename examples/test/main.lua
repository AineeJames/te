-- main.lua

local x, y
local w, h
local speed

function te.load()
	w, h = te.window.getDimensions()
	te.log.debug(string.format("width, height = %d, %d", w, h))
	x = w / 2
	y = h / 2
	speed = 50
end

function te.update(dt)
	if te.keyboard.isDown("right") then
		x = x + speed * dt
	end
	if te.keyboard.isDown("left") then
		x = x - speed * dt
	end
	if te.keyboard.isDown("up") then
		y = y - speed * dt
	end
	if te.keyboard.isDown("down") then
		y = y + speed * dt
	end
end

function te.keypressed(key)
	if key == "escape" then
		te.log.info("Gracefully exiting!")
		te.event.quit(0) -- exit with 0 exit code
	end
end

function te.draw()
	te.graphics.clear()

	te.graphics.setColor(WHITE, BLACK)
	local _x = 1
	local _y = 1
	for i = 1, 256 do
		local offset = 5
		te.graphics.setCell(i, _x + offset, _y + offset)

		_x = _x + 1
		if _x > 16 then -- wrap every 16 glyphs
			_x = 1
			_y = _y + 1
		end
	end

	-- draw the player cell on top
	te.graphics.setColor(LIGHT_MAGENTA, BLACK)
	te.graphics.setCell(2, x, y)

	te.graphics.setColor(GREEN, BLACK)
	te.graphics.print(string.format("fps = %d", te.window.getFPS()), 1, 1)
	te.graphics.setColor(LIGHT_MAGENTA, BLACK)
	te.graphics.print(string.format("p = {%.2f, %.2f}", x, y), 1, 2)
end
