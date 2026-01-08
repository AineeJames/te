-- main.lua

function te.load()
	sound = te.audio.newSource("sound.wav", "static")
	coin = te.audio.newSource("coin.wav", "static")
	music = te.audio.newSource("bgmusic.wav", "stream")
	music:setVolume(0.5)
	music:play()
	volume = 1
end

function te.update(dt) end

function te.keypressed(key)
	if key == "escape" then
		te.event.quit(0) -- exit with 0 exit code
	end
	if key == "space" then
		local choice = math.random(2) -- 1 or 2
		if choice == 1 then
			sound:play()
		else
			coin:play()
		end
	end
	if key == "up" and volume <= 1.0 then
		volume = volume + 0.1
		sound:setVolume(volume)
		coin:setVolume(volume)
	end
	if key == "down" and volume >= 0.0 then
		volume = volume - 0.1
		sound:setVolume(volume)
		coin:setVolume(volume)
	end
end

function te.draw()
	te.graphics.clear()

	te.graphics.setColor(WHITE, BLACK)
	te.graphics.print("press space to hear a noise", 10, 10)
	te.graphics.print("Volume: " .. volume, 10, 11)

	te.graphics.setColor(GREEN, BLACK)
	te.graphics.print(string.format("fps = %d", te.window.getFPS()), 1, 1)
end
