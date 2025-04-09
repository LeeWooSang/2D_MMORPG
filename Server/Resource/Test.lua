math.randomseed(os.time())
DIRECTION = 
{
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3
}

function MoveAI(oldX, oldY)
	local newX = oldX;
	local newY = oldY;
	local dir = math.random(DIRECTION.UP, DIRECTION.RIGHT)
	
	if dir == DIRECTION.UP then
		newY = newY - 1
	elseif dir == DIRECTION.DOWN then
		newY = newY + 1
	elseif dir == DIRECTION.LEFT then
		newX = newX - 1
	else
		newX = newX + 1
	end

	return { x = newX, y = newY }
end

function ChaseTargetStupidAI(oldX, oldY, targetX, targetY)
	local newX = oldX
	local newY = oldY

	if newX < targetX then	
		newX = newX + 1
	elseif newX > targetX then	
		newX = newX - 1
	elseif newY < targetY then	
		newY = newY + 1	
	else
		newY = newY - 1
	end

	return { x = newX, y = newY }
end

function SpeackAI()
end
