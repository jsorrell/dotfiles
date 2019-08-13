# Terminal
set -gx TERMINAL termite

# Terminal colors
set -gx TERM xterm-256color

# Set language environment
set -gx LANG en_US.UTF-8
set -gx LANGUAGE en_US.UTF-8
set -gx LC_ALL en_US.UTF-8
set -gx LC_CTYPE en_US.UTF-8

# Set preferred editors and pagers
set -gx EDITOR nvim
set -gx VISUAL nvim
set -gx PAGER less
set -gx MANPAGER 'less -X'

#Path
set -gx PATH $HOME/bin $GOPATH/bin $PATH
set -gx PATH (ruby -e 'puts Gem.user_dir')/bin $PATH

# Go
if test -d $HOME/go
	set GOPATH $HOME/go
	set PATH $PATH $GOPATH/bin
end

fish_vi_key_bindings
