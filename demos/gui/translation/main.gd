
extends Panel

# member variables here, example:
# var a=2
# var b="textvar"

func _ready():
	# Initialization here
	pass



func _goto_scene():
	var s = load("res://controls.scn")
	var si = s.instance()
	get_parent().add_child(si)
	queue_free()
	pass


func _on_system_pressed():
	#will autodetect based on system, then fall back
	#to english if not found
	_goto_scene()

#NOTE: Changling locale will not change the text in the controls,
#      The scene must be reloaded for changes to take effect.      

func _on_english_pressed():
	TranslationServer.set_locale("en")
	_goto_scene()


func _on_spanish_pressed():
	TranslationServer.set_locale("es")
	_goto_scene()


func _on_japanese_pressed():
	TranslationServer.set_locale("ja")
	_goto_scene()
