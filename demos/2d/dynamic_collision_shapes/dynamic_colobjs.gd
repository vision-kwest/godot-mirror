
extends Node2D

# member variables here, example:
# var a=2
# var b="textvar"
const EMIT_INTERVAL=0.1
var timeout=EMIT_INTERVAL

func _process(delta):
	timeout-=delta
	if (timeout<0):
		timeout=EMIT_INTERVAL
		var ball = preload("res://ball.scn").instance()
		ball.set_pos( Vector2(randf() * get_viewport_rect().size.x, 0) )
		add_child(ball)
			
func _ready():
	set_process(true)
	# Initialization here
	pass


