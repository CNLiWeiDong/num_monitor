apt install -y gdb

gdb ./launcher
catch fork
catch vfork
set follow-fork-mode child
r



# 性能分析
autocannon -l -H Content-Type=application/json -m POST -b '{"jsonrpc":"2.0", "method":"eth_blockNumber","params":[],"id":67}' http://127.0.0.1:8081/game/info
