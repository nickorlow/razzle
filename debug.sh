tmux new-window
tmux send 'qemu-system-i386 -boot d -cdrom os.iso -m 512  -machine type=pc-i440fx-3.1 -monitor stdio -s -S' ENTER
tmux split-window -h
tmux send 'gdb kernel.elf' ENTER
tmux send 'target remote localhost:1234' ENTER
tmux send 'layout split' ENTER
#vncviewer 127.0.0.1:5900 -Scaling=100% 
#killall qemu-system-i386
