# Replace ls with exa if available
function ls --wraps ls
	# Silently test if exa exists
	command which exa >/dev/null ^/dev/null
	if test $status != 1
		command exa $argv
	else
		# Fallback to ls
		command ls $argv
	end
end

