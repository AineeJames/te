-- main.lua

local x, y

function te.load()
	x = 20
	y = 20
end

function te.update(dt)
	if te.keyboard.isDown("right") then
		x = x + 1
	end
	if te.keyboard.isDown("left") then
		x = x - 1
	end
	if te.keyboard.isDown("up") then
		y = y - 1
	end
	if te.keyboard.isDown("down") then
		y = y + 1
	end
end

function te.draw()
	te.graphics.clear()

	te.graphics.setColor(WHITE, BLACK)
	local _x = 1
	local _y = 1
	for i = 0, 255 do
		te.graphics.setCell(i, _x, _y)

		_x = _x + 1
		if _x > 16 then -- wrap every 16 glyphs
			_x = 1
			_y = _y + 1
		end
	end

	-- draw the player cell on top
	te.graphics.setColor(LIGHT_MAGENTA, BLACK)
	te.graphics.setCell(2, x, y)
end
